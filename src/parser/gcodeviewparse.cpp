// This file is a part of "Candle" application.
// This file was originally ported from "GcodeViewParse.java" class
// of "Universal GcodeSender" application written by Will Winder
// (https://github.com/winder/Universal-G-Code-Sender)

// Copyright 2015-2016 Hayrullin Denis Ravilevich

#include <QDebug>
#include "gcodeviewparse.h"

GcodeViewParse::GcodeViewParse(QObject *parent) :
    QObject(parent)
{
    absoluteMode = true;
    absoluteIJK = false;
    currentLine = 0;
    debug = true;

    m_min = QVector3D(qQNaN(), qQNaN(), qQNaN());
    m_max = QVector3D(qQNaN(), qQNaN(), qQNaN());

    m_minLength = qQNaN();
}

GcodeViewParse::~GcodeViewParse() = default;

QVector3D &GcodeViewParse::getMinimumExtremes()
{
    return m_min;
}

QVector3D &GcodeViewParse::getMaximumExtremes()
{
    return m_max;
}

void GcodeViewParse::testExtremes(QVector3D p3d)
{
    this->testExtremes(p3d.x(), p3d.y(), p3d.z());
}

void GcodeViewParse::testExtremes(double x, double y, double z)
{
    m_min.setX(Util::nMin(m_min.x(), x));
    m_min.setY(Util::nMin(m_min.y(), y));
    m_min.setZ(Util::nMin(m_min.z(), z));

    m_max.setX(Util::nMax(m_max.x(), x));
    m_max.setY(Util::nMax(m_max.y(), y));
    m_max.setZ(Util::nMax(m_max.z(), z));
}

void GcodeViewParse::testLength(const QVector3D &start, const QVector3D &end)
{
    double length = (start - end).length();
    if (!qIsNaN(length) && length != 0) m_minLength = qIsNaN(m_minLength) ? length : qMin<double>(m_minLength, length);
}

LineSegment::Container GcodeViewParse::toObjRedux(QByteArrayList const &gcode, double arcPrecision, bool arcDegreeMode)
{
    GcodeParser gp;

    for (auto &s : gcode) {
        gp.addCommand(s);
    }

    return getLinesFromParser(&gp, arcPrecision, arcDegreeMode);
}

LineSegment::Container &GcodeViewParse::getLineSegmentList()
{
    return m_lines;
}

void GcodeViewParse::reset()
{
    m_lines.clear();
    m_lineIndexes.clear();
    currentLine = 0;
    m_min = QVector3D(qQNaN(), qQNaN(), qQNaN());
    m_max = QVector3D(qQNaN(), qQNaN(), qQNaN());
    m_minLength = qQNaN();
}

double GcodeViewParse::getMinLength() const
{
    return m_minLength;
}

QSize GcodeViewParse::getResolution() const
{
    return QSize(((m_max.x() - m_min.x()) / m_minLength) + 1, ((m_max.y() - m_min.y()) / m_minLength) + 1);
}

LineSegment::Container GcodeViewParse::getLinesFromParser(GcodeParser *gp, double arcPrecision, bool arcDegreeMode)
{
    auto &psl = gp->getPointSegmentList();
    // For a line segment list ALL arcs must be converted to lines.
    double minArcLength = 0.1;

    QVector3D const * start = nullptr;
    QVector3D const * end;

    // Prepare segments indexes
    m_lineIndexes.resize(psl.size());

    int lineIndex = 0;
    for (auto &ps : psl) {
        bool isMetric = ps.isMetric(); // need to keep original unit
        ps.convertToMetric();

        end = &ps.point();

        // start is null for the first iteration.
        if (start != NULL) {
            // Expand arc for graphics.
            if (ps.isArc()) {
                auto const points =
                    GcodePreprocessorUtils::generatePointsAlongArcBDring(ps.plane(),
                    *start, *end, ps.center(), ps.isClockwise(), ps.getRadius(), minArcLength, arcPrecision, arcDegreeMode);
                // Create line segments from points.
                if (!points.empty()) {
                    QVector3D startPoint = *start;
                    for (auto const &nextPoint : points) {
                        if (nextPoint == startPoint) continue;
                       m_lines.emplace_back(startPoint, nextPoint, lineIndex, ps, isMetric);
                        this->testExtremes(nextPoint);
                        m_lineIndexes[ps.getLineNumber()].push_back(m_lines.size() - 1);
                        startPoint = nextPoint;
                    }
                    lineIndex++;
                }
            // Line
            } else {
                m_lines.emplace_back(*start, *end, lineIndex++, ps, isMetric);
                this->testExtremes(*end);
                this->testLength(*start, *end);
                m_lineIndexes[ps.getLineNumber()].push_back(m_lines.size() - 1);
            }
        }
        start = end;
    }

    return m_lines;
}

LineSegment::Container & GcodeViewParse::getLines()
{
    return m_lines;
}

indexVector &GcodeViewParse::getLinesIndexes()
{
    return m_lineIndexes;
}
