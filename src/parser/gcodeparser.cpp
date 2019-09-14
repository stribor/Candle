// This file is a part of "Candle" application.
// This file was originally ported from "GcodeParser.java" class
// of "Universal GcodeSender" application written by Will Winder
// (https://github.com/winder/Universal-G-Code-Sender)

// Copyright 2015-2016 Hayrullin Denis Ravilevich

#include <QListIterator>
#include <QDebug>
#include "gcodeparser.h"

GcodeParser::GcodeParser(QObject *parent) : QObject(parent)
{
    m_isMetric = true;
    m_inAbsoluteMode = true;
    m_inAbsoluteIJKMode = false;
    m_lastGcodeCommand = -1;
    m_commandNumber = 0;

    // Settings
    m_speedOverride = -1;
    m_truncateDecimalLength = 40;
    m_removeAllWhitespace = true;
    m_convertArcsToLines = false;
    m_smallArcThreshold = 1.0;
    // Not configurable outside, but maybe it should be.
    m_smallArcSegmentLength = 0.3;
    m_lastSpeed = 0;
    m_lastSpindleSpeed = 0;
    m_traverseSpeed = 300;
    reset();
}

GcodeParser::~GcodeParser() = default;

bool GcodeParser::getConvertArcsToLines() const {
    return m_convertArcsToLines;
}

void GcodeParser::setConvertArcsToLines(bool convertArcsToLines) {
    this->m_convertArcsToLines = convertArcsToLines;
}

bool GcodeParser::getRemoveAllWhitespace() const{
    return m_removeAllWhitespace;
}

void GcodeParser::setRemoveAllWhitespace(bool removeAllWhitespace) {
    this->m_removeAllWhitespace = removeAllWhitespace;
}

double GcodeParser::getSmallArcSegmentLength() const {
    return m_smallArcSegmentLength;
}

void GcodeParser::setSmallArcSegmentLength(double smallArcSegmentLength) {
    this->m_smallArcSegmentLength = smallArcSegmentLength;
}

double GcodeParser::getSmallArcThreshold() const {
    return m_smallArcThreshold;
}

void GcodeParser::setSmallArcThreshold(double smallArcThreshold) {
    this->m_smallArcThreshold = smallArcThreshold;
}

double GcodeParser::getSpeedOverride() const {
    return m_speedOverride;
}

void GcodeParser::setSpeedOverride(double speedOverride) {
    this->m_speedOverride = speedOverride;
}

int GcodeParser::getTruncateDecimalLength() const {
    return m_truncateDecimalLength;
}

void GcodeParser::setTruncateDecimalLength(int truncateDecimalLength) {
    this->m_truncateDecimalLength = truncateDecimalLength;
}

// Resets the current state.
void GcodeParser::reset(const QVector3D &initialPoint)
{
    qDebug() << "reseting gp" << initialPoint;

    this->m_points.clear();
    this->m_points.reserve(21002710);

    // The unspoken home location.
    m_currentPoint = initialPoint;
    m_currentPlane = PointSegment::XY;
    this->m_points.push_back(PointSegment(this->m_currentPoint, -1));
}

/**
* Add a command to be processed.
*/
PointSegment* GcodeParser::addCommand(QByteArray const &command)
{
    auto &stripped = command;
//    auto stripped = GcodePreprocessorUtils::removeComment(command);
    auto args = GcodePreprocessorUtils::splitCommand(stripped);
    return this->addCommand(args);
}

/**
* Add a command which has already been broken up into its arguments.
*/
PointSegment* GcodeParser::addCommand(QByteArrayList const &args)
{
    if (args.isEmpty()) {
        return NULL;
    }
    return processCommand(args);
}

/**
* Warning, this should only be used when modifying live gcode, such as when
* expanding an arc or canned cycle into line segments.
*/
void GcodeParser::setLastGcodeCommand(float num) {
    this->m_lastGcodeCommand = num;
}

/**
* Gets the point at the end of the list.
*/
QVector3D *GcodeParser::getCurrentPoint() {
    return &m_currentPoint;
}

/**
* Expands the last point in the list if it is an arc according to the
* the parsers settings.
*/
// NOTE!! PointSegment::ContainerPtr is valid until m_points is not modified afterwards!
PointSegment::ContainerPtr GcodeParser::expandArc()
{
    PointSegment *startSegment = &this->m_points[this->m_points.size() - 2];
    PointSegment *lastSegment = &this->m_points[this->m_points.size() - 1];

    // Can only expand arcs.
    if (!lastSegment->isArc()) {
        return {};
    }

    // Get precalculated stuff.
    QVector3D const &start = startSegment->point();
    QVector3D const &end = lastSegment->point();
    QVector3D &center = lastSegment->center();
    double radius = lastSegment->getRadius();
    bool clockwise = lastSegment->isClockwise();
    bool lastIsMetric = lastSegment->isMetric();
    PointSegment::planes plane = startSegment->plane();

    //
    // Start expansion.
    //

    auto expandedPoints = GcodePreprocessorUtils::generatePointsAlongArcBDring(plane, start, end, center, clockwise, radius, m_smallArcThreshold, m_smallArcSegmentLength, false);

    // Validate output of expansion.
    if (expandedPoints.empty()) {
        return {};
    }

    // Remove the last point now that we're about to expand it.
    this->m_points.pop_back();
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
        this->m_points.push_back(temp);
        psl.push_back(&this->m_points.back());
        std::advance(psi, 1);
    }

    // Update the new endpoint.
    this->m_currentPoint = this->m_points.back().point();

    return psl;
}

PointSegment::Container &GcodeParser::getPointSegmentList()
{
    return this->m_points;
}
double GcodeParser::getTraverseSpeed() const
{
    return m_traverseSpeed;
}

void GcodeParser::setTraverseSpeed(double traverseSpeed)
{
    m_traverseSpeed = traverseSpeed;
}

int GcodeParser::getCommandNumber() const
{
    return m_commandNumber - 1;
}


PointSegment *GcodeParser::processCommand(QByteArrayList const &args)
{
    PointSegment *ps = NULL;
    GcodePreprocessorUtils::gcodesContainer gCodes;
    double speed;
    double spindleSpeed;
    double dwell;
    for (auto &arg : args) {
        auto const gc = GcodePreprocessorUtils::parseGCodeEnum(arg);
        if (gc != unknown) {
            gCodes.push_back(gc);
        } else if (GcodePreprocessorUtils::parseCoord(arg, 'F', speed)) { // Handle F code
            this->m_lastSpeed = this->m_isMetric ? speed : speed * 25.4;
        } else if (GcodePreprocessorUtils::parseCoord(arg, 'S', spindleSpeed)) { // Handle S code
            this->m_lastSpindleSpeed = spindleSpeed;
        } else if (GcodePreprocessorUtils::parseCoord(arg, 'P', dwell)) { // Handle P code
            this->m_points.back().setDwell(dwell);
        }
    }

    // handle G codes.
    // If there was no command, add the implicit one to the party.
    if (gCodes.empty() && m_lastGcodeCommand != unknown) {
        gCodes.push_back(m_lastGcodeCommandE);
    }

    for (auto code : gCodes) {
        ps = handleGCode(code, args);
    }

    return ps;
}

PointSegment *GcodeParser::addLinearPointSegment(const QVector3D &nextPoint, bool fastTraverse)
{
#ifdef USE_STD_CONTAINERS
    auto &ps = this->m_points.emplace_back(nextPoint, m_commandNumber++);
#else
    this->m_points.push_back(PointSegment(nextPoint, m_commandNumber++));
    auto &ps = this->m_points.back();
#endif
//    PointSegment ps(nextPoint, m_commandNumber++);

    // Check for z-only
    bool zOnly =
            (this->m_currentPoint.x() == nextPoint.x()) &&
            (this->m_currentPoint.y() == nextPoint.y()) &&
            (this->m_currentPoint.z() != nextPoint.z());

    ps.setIsMetric(this->m_isMetric);
    ps.setIsZMovement(zOnly);
    ps.setIsFastTraverse(fastTraverse);
    ps.setIsAbsolute(this->m_inAbsoluteMode);
    ps.setSpeed(fastTraverse ? this->m_traverseSpeed : this->m_lastSpeed);
    ps.setSpindleSpeed(this->m_lastSpindleSpeed);

    // Save off the endpoint.
    this->m_currentPoint = nextPoint;

    return &ps;
}

PointSegment *GcodeParser::addArcPointSegment(const QVector3D &nextPoint, bool clockwise, QByteArrayList const &args)
{
//    PointSegment ps(nextPoint, m_commandNumber++);
#ifndef USE_STD_CONTAINERS
    auto &ps = this->m_points.emplace_back(nextPoint, m_commandNumber++);
#else
    this->m_points.push_back(PointSegment(nextPoint, m_commandNumber++));
    auto &ps = this->m_points.back();
#endif
    QVector3D center = GcodePreprocessorUtils::updateCenterWithCommand(args, this->m_currentPoint, nextPoint, this->m_inAbsoluteIJKMode, clockwise);
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

        radius = sqrt(pow((double)((m * this->m_currentPoint).x() - (m * center).x()), 2.0)
                        + pow((double)((m * this->m_currentPoint).y() - (m * center).y()), 2.0));
    }

    ps.setIsMetric(this->m_isMetric);
    ps.setArcCenter(center);
    ps.setIsArc(true);
    ps.setRadius(radius);
    ps.setIsClockwise(clockwise);
    ps.setIsAbsolute(this->m_inAbsoluteMode);
    ps.setSpeed(this->m_lastSpeed);
    ps.setSpindleSpeed(this->m_lastSpindleSpeed);
    ps.setPlane(m_currentPlane);
//    this->m_points.push_back(ps); // alrady inplace

    // Save off the endpoint.
    this->m_currentPoint = nextPoint;
    return &ps;
}

void GcodeParser::handleMCode(float /*code*/, QByteArrayList const &args)
{
    double spindleSpeed = GcodePreprocessorUtils::parseCoord(args, 'S');
    if (!qIsNaN(spindleSpeed)) this->m_lastSpindleSpeed = spindleSpeed;
}

PointSegment * GcodeParser::handleGCode(GCodes code, QByteArrayList const &args)
{
    PointSegment *ps = NULL;

    QVector3D nextPoint = GcodePreprocessorUtils::updatePointWithCommand(args, this->m_currentPoint, this->m_inAbsoluteMode);
    // should this use qFuzzyCompare()?
    switch (code) {
    case G00: ps = addLinearPointSegment(nextPoint, true);break;
    case G01: ps = addLinearPointSegment(nextPoint, false); break;
    case G38_2:  ps = addLinearPointSegment(nextPoint, false); break;
    case G02: ps = addArcPointSegment(nextPoint, true, args); break;
    case G03: ps = addArcPointSegment(nextPoint, false, args); break;
    case G17: this->m_currentPlane = PointSegment::XY; break;
    case G18: this->m_currentPlane = PointSegment::ZX; break;
    case G19: this->m_currentPlane = PointSegment::YZ; break;
    case G20: this->m_isMetric = false; break;
    case G21: this->m_isMetric = true; break;
    case G90: this->m_inAbsoluteMode = true; break;
    case G90_1:  this->m_inAbsoluteIJKMode = true; break;
    case G91: this->m_inAbsoluteMode = false; break;
    case G91_1:  this->m_inAbsoluteIJKMode = false; break;
    default: break;
    }
    if (code == G00 || code == G01 || code == G02 || code == G03 || code == G38_2) {
        m_lastGcodeCommandE = code;
    }

    return ps;
}

PointSegment * GcodeParser::handleGCode(float code, QByteArrayList const &args)
{
    PointSegment *ps = NULL;

    QVector3D nextPoint = GcodePreprocessorUtils::updatePointWithCommand(args, this->m_currentPoint, this->m_inAbsoluteMode);
    // should this use qFuzzyCompare()?
    if (code == 1.0f) ps = addLinearPointSegment(nextPoint, false);
    else if (code == 0.0f) ps = addLinearPointSegment(nextPoint, true);
    else if (code == 38.2f) ps = addLinearPointSegment(nextPoint, false);
    else if (code == 2.0f) ps = addArcPointSegment(nextPoint, true, args);
    else if (code == 3.0f) ps = addArcPointSegment(nextPoint, false, args);
    else if (code == 17.0f) this->m_currentPlane = PointSegment::XY;
    else if (code == 18.0f) this->m_currentPlane = PointSegment::ZX;
    else if (code == 19.0f) this->m_currentPlane = PointSegment::YZ;
    else if (code == 20.0f) this->m_isMetric = false;
    else if (code == 21.0f) this->m_isMetric = true;
    else if (code == 90.0f) this->m_inAbsoluteMode = true;
    else if (code == 90.1f) this->m_inAbsoluteIJKMode = true;
    else if (code == 91.0f) this->m_inAbsoluteMode = false;
    else if (code == 91.1f) this->m_inAbsoluteIJKMode = false;

    if (code == 0.0f || code == 1.0f || code == 2.0f || code == 3.0f || code == 38.2f) this->m_lastGcodeCommand = code;

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
    bool hasComment = (newCommand.length() != command.length());

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
            QStringList arcLines = convertArcsToLines(newCommand);
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
    QVector3D start = this->m_currentPoint;

    PointSegment *ps = addCommand(command);

    if (ps == NULL || !ps->isArc()) {
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
        //Point3d end = segment.point();
        QVector3D const &end = segment->point();
        result.append(GcodePreprocessorUtils::generateG1FromPoints(start, end, this->m_inAbsoluteMode, m_truncateDecimalLength));
        start = segment->point();
    }

    return result;

}
