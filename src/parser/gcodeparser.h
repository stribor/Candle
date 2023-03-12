// This file is a part of "Candle" application.
// This file was originally ported from "GcodeParser.java" class
// of "Universal GcodeSender" application written by Will Winder
// (https://github.com/winder/Universal-G-Code-Sender)

// Copyright 2015-2016 Hayrullin Denis Ravilevich

#ifndef GCODEPARSER_H
#define GCODEPARSER_H

#include <QVector3D>
#include <cmath>
#include "pointsegment.h"
#include "gcodepreprocessorutils.h"

class GcodeParser
{
public:
    explicit GcodeParser();
    ~GcodeParser();

    bool getConvertArcsToLines() const;
    void setConvertArcsToLines(bool convertArcsToLines);
    bool getRemoveAllWhitespace() const;
    void setRemoveAllWhitespace(bool removeAllWhitespace);
    double getSmallArcSegmentLength() const;
    void setSmallArcSegmentLength(double smallArcSegmentLength);
    double getSmallArcThreshold() const;
    void setSmallArcThreshold(double smallArcThreshold);
    double getSpeedOverride() const;
    void setSpeedOverride(double speedOverride);
    int getTruncateDecimalLength() const;
    void setTruncateDecimalLength(int truncateDecimalLength);
    void reset(const QVector3D &initialPoint = QVector3D(qQNaN(), qQNaN(), qQNaN()));
    PointSegment *addCommand(QByteArray const &command);
    PointSegment *addCommand(QByteArrayList const &args);
    QVector3D* getCurrentPoint();
    PointSegment::ContainerPtr expandArc();
    QStringList preprocessCommands(QByteArrayList const &commands);
    QStringList preprocessCommand(QByteArray const &command);
    QStringList convertArcsToLines(QByteArray const &command);
    PointSegment::Container &getPointSegmentList();
    double getTraverseSpeed() const;
    void setTraverseSpeed(double traverseSpeed);
    int getCommandNumber() const;

private:

    // Current state
    bool m_isMetric{true};
    bool m_inAbsoluteMode{true};
    bool m_inAbsoluteIJKMode{false};
    float m_lastGcodeCommand{-1};
    QVector3D m_currentPoint = QVector3D(qQNaN(), qQNaN(), qQNaN());
    int m_commandNumber{0};
    PointSegment::planes m_currentPlane{PointSegment::XY};
    GCodes m_lastGcodeCommandE{unknown};

    // Settings
    double m_speedOverride{-1};
    int m_truncateDecimalLength{40};
    bool m_removeAllWhitespace{true};
    bool m_convertArcsToLines{false};
    double m_smallArcThreshold{1.0};
    // Not configurable outside, but maybe it should be.
    double m_smallArcSegmentLength{0.3};

    double m_lastSpeed{0};
    double m_traverseSpeed{300};
    double m_lastSpindleSpeed{0};

    // The gcode.
    PointSegment::Container m_points;

    PointSegment *processCommand(QByteArrayList const &args);
    void handleMCode(float, QByteArrayList const &args);
    PointSegment *handleGCode(GCodes code, QByteArrayList const &args);

    PointSegment *addLinearPointSegment(const QVector3D &nextPoint, bool fastTraverse);
    PointSegment *addArcPointSegment(const QVector3D &nextPoint, bool clockwise, QByteArrayList const &args);
    void setLastGcodeCommand(float num);
};

#endif // GCODEPARSER_H
