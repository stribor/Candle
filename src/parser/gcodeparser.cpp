// This file is a part of "Candle" application.
// This file was originally ported from "GcodeParser.java" class
// of "Universal GcodeSender" application written by Will Winder
// (https://github.com/winder/Universal-G-Code-Sender)

// Copyright 2015-2016 Hayrullin Denis Ravilevich

#include "gcodeparser.h"

#include <QDebug>
#include <QListIterator>
#include <QMessageBox>
#include <QObject>

GcodeParser::GcodeParser()
{
    reset();
}

// Resets the current state.
void GcodeParser::reset(const QVector3D &initialPoint)
{
    qDebug() << "reseting gp" << initialPoint;

    m_points.clear();

    // The unspoken home location.
    m_currentPoint = initialPoint;
    m_currentPlane = PointSegment::XY;
    m_points.emplace_back(m_currentPoint, -1);
}

/**
* Add a command to be processed.
*/
PointSegment* GcodeParser::addCommand(QByteArray const &command)
{
    const auto &stripped = command;
    //    auto stripped = GcodePreprocessorUtils::removeComment(command);
    auto args = GcodePreprocessorUtils::splitCommand(stripped);
    return addCommand(args);
}

/**
* Add a command which has already been broken up into its arguments.
*/
PointSegment* GcodeParser::addCommand(QByteArrayList const &args)
{
    if (args.isEmpty()) {
        return nullptr;
    }
    return processCommand(args);
}

/**
* Expands the last point in the list if it is an arc according to the
* the parsers settings.
*/
// NOTE!! PointSegment::ContainerPtr is valid until m_points is not modified afterwards!
PointSegment::ContainerPtr GcodeParser::expandArc()
{
    PointSegment *startSegment = &m_points[m_points.size() - 2];
    PointSegment *lastSegment = &m_points[m_points.size() - 1];

    // Can only expand arcs.
    if (!lastSegment->isArc()) {
        return {};
    }

    // Get precalculated stuff.
    QVector3D const &start = startSegment->point();
    QVector3D const &end = lastSegment->point();
    QVector3D const &center = lastSegment->center();
    double const radius = lastSegment->getRadius();
    bool const clockwise = lastSegment->isClockwise();
    bool const lastIsMetric = lastSegment->isMetric();
    PointSegment::planes const plane = startSegment->plane();

    //
    // Start expansion.
    //

    auto expandedPoints = GcodePreprocessorUtils::generatePointsAlongArcBDring(plane, start, end, center, clockwise, radius, m_smallArcThreshold, m_smallArcSegmentLength, false);

    // Validate output of expansion.
    if (expandedPoints.empty()) {
        return {};
    }

    // Remove the last point now that we're about to expand it.
    m_points.pop_back();
    m_commandNumber--;

    // Initialize return value
    PointSegment::ContainerPtr psl;

    // Create line segments from points.
    auto psi =expandedPoints.cbegin();
    // skip first element.
    if (psi!=expandedPoints.end()) ++psi;

    // !reserve space so returned pointers aren't shifted
    m_points.reserve(m_points.size() + expandedPoints.size());
    while (psi != expandedPoints.end()) {
        auto temp = PointSegment(*psi, m_commandNumber++);
        temp.setIsMetric(lastIsMetric);
        m_points.push_back(temp);
        psl.push_back(&m_points.back());
        std::advance(psi, 1);
    }

    // Update the new endpoint.
    m_currentPoint = m_points.back().point();

    return psl;
}

PointSegment *GcodeParser::processCommand(QByteArrayList const &args)
{
    PointSegment *ps = nullptr;
    GcodePreprocessorUtils::gcodesContainer gCodes;
    double speed;
    double spindleSpeed;
    double dwell;
    for (const auto &arg : args) {
        auto const gc = GcodePreprocessorUtils::parseGCodeEnum(arg);
        if (gc != unknown) {
            gCodes.push_back(gc);
        } else if (GcodePreprocessorUtils::parseCoord(arg, 'F', speed)) {// Handle F code
            m_lastSpeed = m_isMetric ? speed : speed * 25.4;
        } else if (GcodePreprocessorUtils::parseCoord(arg, 'S', spindleSpeed)) {// Handle S code
            m_lastSpindleSpeed = spindleSpeed;
        } else if (GcodePreprocessorUtils::parseCoord(arg, 'P', dwell)) {// Handle P code
            m_points.back().setDwell(dwell);
        }
    }

    // handle G codes.
    // If there was no command, add the implicit one to the party.
    if (gCodes.empty() && m_lastGcodeCommand != unknown) {
        gCodes.push_back(m_lastGcodeCommand);
    }

    for (auto code : gCodes) {
        ps = handleGCode(code, args);
    }

    return ps;
}

PointSegment *GcodeParser::addLinearPointSegment(const QVector3D &nextPoint, bool fastTraverse)
{
#ifdef USE_STD_CONTAINERS
    auto &ps = m_points.emplace_back(nextPoint, m_commandNumber++);
#else
    m_points.push_back(PointSegment(nextPoint, m_commandNumber++));
    auto &ps = m_points.back();
#endif

    // Check for z-only
    bool const zOnly =
            (m_currentPoint.x() == nextPoint.x()) &&
            (m_currentPoint.y() == nextPoint.y()) &&
            (m_currentPoint.z() != nextPoint.z());

    ps.setIsMetric(m_isMetric);
    ps.setIsZMovement(zOnly);
    ps.setIsFastTraverse(fastTraverse);
    ps.setIsAbsolute(m_inAbsoluteMode);
    ps.setSpeed(fastTraverse ? m_traverseSpeed : m_lastSpeed);
    ps.setSpindleSpeed(m_lastSpindleSpeed);

    // Save off the endpoint.
    m_currentPoint = nextPoint;

    return &ps;
}

PointSegment *GcodeParser::addArcPointSegment(const QVector3D &nextPoint, bool clockwise, QByteArrayList const &args)
{
#ifdef USE_STD_CONTAINERS
    auto &ps = m_points.emplace_back(nextPoint, m_commandNumber++);
#else
    m_points.push_back(PointSegment(nextPoint, m_commandNumber++));
    auto &ps = m_points.back();
#endif
    QVector3D const center = GcodePreprocessorUtils::updateCenterWithCommand(args, m_currentPoint, nextPoint, m_inAbsoluteIJKMode, clockwise);
    double radius = GcodePreprocessorUtils::parseCoord(args, 'R');

    // Calculate radius if necessary.
    if (qIsNaN(radius)) {

        QMatrix4x4 m;
        m.setToIdentity();
        switch (m_currentPlane) {
        case PointSegment::XY:
            break;
        case PointSegment::ZX:
            m.rotate(90, 1.0, 0.0, 0.0);
            break;
        case PointSegment::YZ:
            m.rotate(-90, 0.0, 1.0, 0.0);
            break;
        }

        radius = sqrt(pow((double) ((m.map(m_currentPoint)).x() - (m.map(center)).x()), 2.0) + pow((double) ((m.map(m_currentPoint)).y() - (m.map(center)).y()), 2.0));
    }

    ps.setIsMetric(m_isMetric);
    ps.setArcCenter(center);
    ps.setIsArc(true);
    ps.setRadius(radius);
    ps.setIsClockwise(clockwise);
    ps.setIsAbsolute(m_inAbsoluteMode);
    ps.setSpeed(m_lastSpeed);
    ps.setSpindleSpeed(m_lastSpindleSpeed);
    ps.setPlane(m_currentPlane);

    // Save off the endpoint.
    m_currentPoint = nextPoint;
    return &ps;
}

void GcodeParser::handleMCode(GCodes /*code*/, QByteArrayList const &args)
{
    double const spindleSpeed = GcodePreprocessorUtils::parseCoord(args, 'S');
    if (!qIsNaN(spindleSpeed)) m_lastSpindleSpeed = spindleSpeed;
}

PointSegment * GcodeParser::handleGCode(GCodes code, QByteArrayList const &args)
{
    PointSegment *ps = nullptr;

    QVector3D const nextPoint = GcodePreprocessorUtils::updatePointWithCommand(args, m_currentPoint, m_inAbsoluteMode);
    // should this use qFuzzyCompare()?
    switch (code) {
    case G00: ps = addLinearPointSegment(nextPoint, true); break;
    case G01:
    case G38_2: ps = addLinearPointSegment(nextPoint, false); break;
    case G02: ps = addArcPointSegment(nextPoint, true, args); break;
    case G03: ps = addArcPointSegment(nextPoint, false, args); break;
    case G17: m_currentPlane = PointSegment::XY; break;
    case G18: m_currentPlane = PointSegment::ZX; break;
    case G19: m_currentPlane = PointSegment::YZ; break;
    case G20: m_isMetric = false; break;
    case G21: m_isMetric = true; break;
    case G90: m_inAbsoluteMode = true; break;
    case G90_1: m_inAbsoluteIJKMode = true; break;
    case G91:
        m_inAbsoluteMode = false;
        if (qIsNaN(m_currentPoint.x()) || qIsNaN(m_currentPoint.y()) || qIsNaN(m_currentPoint.z())) {
            int const res = QMessageBox::warning(nullptr, QObject::tr("GcodeParser"),
                                                 QObject::tr("GcodeParser error:Switching to relative mode without previously set current position. Select Ok to set unknown coordinates to 0 or ignore to continue"),
                                                 QMessageBox::Ok | QMessageBox::Ignore);
            if (res == QMessageBox::Ok) {
                if (qIsNaN(m_currentPoint.x())) m_currentPoint.setX(0.0);
                if (qIsNaN(m_currentPoint.y())) m_currentPoint.setY(0.0);
                if (qIsNaN(m_currentPoint.z())) m_currentPoint.setZ(0.0);
            }
        }
        break;
    case G91_1: m_inAbsoluteIJKMode = false; break;
    default: break;
    }
    if (code == G00 || code == G01 || code == G02 || code == G03 || code == G38_2) {
        m_lastGcodeCommand = code;
    }

    return ps;
}

QStringList GcodeParser::preprocessCommands(QByteArrayList const &commands) {

    QStringList result;

    for (auto const & command : commands) {
        result.append(preprocessCommand(command));
    }

    return result;
}

QStringList GcodeParser::preprocessCommand(QByteArray const &command) {

    QStringList result;

    // Remove comments from command.
    auto newCommand = GcodePreprocessorUtils::removeComment(command);
    auto rawCommand = newCommand;
    bool const hasComment = (newCommand.length() != command.length());

    if (m_removeAllWhitespace) {
        newCommand = GcodePreprocessorUtils::removeAllWhitespace(newCommand);
    }

    if (newCommand.size() > 0) {

        // Override feed speed
        if (m_speedOverride > 0) {
            newCommand = GcodePreprocessorUtils::overrideSpeed(newCommand, m_speedOverride);
        }

        if (m_truncateDecimalLength > 0) {
            newCommand = GcodePreprocessorUtils::truncateDecimals(m_truncateDecimalLength, newCommand);
        }

        // If this is enabled we need to parse the gcode as we go along.
        if (m_convertArcsToLines) { // || this.expandCannedCycles) {
            QStringList const arcLines = convertArcsToLines(newCommand);
            if (!arcLines.empty()) {
                result.append(arcLines);
            } else {
                result.append(newCommand);
            }
        } else if (hasComment) {
            // Maintain line level comment.
            QString origCmd = command; // to keep command const as comments should be rare case
            result.append(origCmd.replace(rawCommand, newCommand));
        } else {
            result.append(newCommand);
        }
    } else if (hasComment) {
        // Reinsert comment-only lines.
        result.append(command);
    }

    return result;
}

QStringList GcodeParser::convertArcsToLines(QByteArray const &command)
{
    QVector3D start = m_currentPoint;

    PointSegment const *ps = addCommand(command);

    if (ps == nullptr || !ps->isArc()) {
        return {};
    }

    auto psl = expandArc();

    if (psl.empty()) {
        return {};
    }

    QStringList result;
    // Create an array of new commands out of the of the segments in psl.
    // Don't add them to the gcode parser since it is who expanded them.
    for (auto &segment : psl) {
        QVector3D const &end = segment->point();
        result.append(GcodePreprocessorUtils::generateG1FromPoints(start, end, m_inAbsoluteMode, m_truncateDecimalLength));
        start = segment->point();
    }

    return result;

}
