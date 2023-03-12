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
    GcodeParser();
 
    [[nodiscard]] bool getConvertArcsToLines() const {
        return m_convertArcsToLines;
    }
    void setConvertArcsToLines(bool convertArcsToLines){
        m_convertArcsToLines = convertArcsToLines;
    }
    [[nodiscard]] bool getRemoveAllWhitespace() const{
        return m_removeAllWhitespace;
    }
    void setRemoveAllWhitespace(bool removeAllWhitespace){
        m_removeAllWhitespace = removeAllWhitespace;
    }
    [[nodiscard]] double getSmallArcSegmentLength() const{
        return m_smallArcSegmentLength;
    }
    void setSmallArcSegmentLength(double smallArcSegmentLength){
        m_smallArcSegmentLength = smallArcSegmentLength;
    }
    [[nodiscard]] double getSmallArcThreshold() const{
        return m_smallArcThreshold;
    }
    void setSmallArcThreshold(double smallArcThreshold){
        m_smallArcThreshold = smallArcThreshold;
    }
    [[nodiscard]] double getSpeedOverride() const {
        return m_speedOverride;
    }
    void setSpeedOverride(double speedOverride){
        m_speedOverride = speedOverride;
    }
    [[nodiscard]] int getTruncateDecimalLength() const{
        return m_truncateDecimalLength;
    }
    void setTruncateDecimalLength(int truncateDecimalLength){
        m_truncateDecimalLength = truncateDecimalLength;
    }
    void reset(const QVector3D &initialPoint = QVector3D(qQNaN(), qQNaN(), qQNaN()));
    PointSegment *addCommand(QByteArray const &command);
    PointSegment *addCommand(QByteArrayList const &args);
    QVector3D* getCurrentPoint();
    PointSegment::ContainerPtr expandArc();
    QStringList preprocessCommands(QByteArrayList const &commands);
    QStringList preprocessCommand(QByteArray const &command);
    QStringList convertArcsToLines(QByteArray const &command);
    PointSegment::Container &getPointSegmentList();
    [[nodiscard]] double getTraverseSpeed() const;
    void setTraverseSpeed(double traverseSpeed);
    [[nodiscard]] int getCommandNumber() const;

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
