// This file is a part of "Candle" application.
// This file was originally ported from "GcodeParser.java" class
// of "Universal GcodeSender" application written by Will Winder
// (https://github.com/winder/Universal-G-Code-Sender)

// Copyright 2015-2016 Hayrullin Denis Ravilevich

#ifndef GCODEPARSER_H
#define GCODEPARSER_H

#include <QObject>
#include <QVector3D>
#include <cmath>
#include "pointsegment.h"
#include "gcodepreprocessorutils.h"

class GcodeParser : public QObject
{
    Q_OBJECT
public:
    explicit GcodeParser(QObject *parent = 0);
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
    PointSegment *addCommand(QString const &command);
    PointSegment *addCommand(QStringList const &args);
    QVector3D* getCurrentPoint();
    PointSegment::ContainerPtr expandArc();
    QStringList preprocessCommands(QStringList const &commands);
    QStringList preprocessCommand(QString const &command);
    QStringList convertArcsToLines(QString const &command);
    PointSegment::Container &getPointSegmentList();
    double getTraverseSpeed() const;
    void setTraverseSpeed(double traverseSpeed);
    int getCommandNumber() const;

signals:

public slots:

private:

    // Current state
    bool m_isMetric;
    bool m_inAbsoluteMode;
    bool m_inAbsoluteIJKMode;
    float m_lastGcodeCommand;
    QVector3D m_currentPoint;
    int m_commandNumber;
    PointSegment::planes m_currentPlane;
    GCodes m_lastGcodeCommandE{unknown};

    // Settings
    double m_speedOverride;
    int m_truncateDecimalLength;
    bool m_removeAllWhitespace;
    bool m_convertArcsToLines;
    double m_smallArcThreshold;
    // Not configurable outside, but maybe it should be.
    double m_smallArcSegmentLength;

    double m_lastSpeed;
    double m_traverseSpeed;
    double m_lastSpindleSpeed;

    // The gcode.
    PointSegment::Container m_points; //TODO convert to vector<PointSegment> and not reallocate

    PointSegment *processCommand(const QStringList &args);
    void handleMCode(float code, const QStringList &args);
    PointSegment *handleGCode(GCodes code, const QStringList &args);
    PointSegment *handleGCode(float code, const QStringList &args);
    PointSegment *addLinearPointSegment(const QVector3D &nextPoint, bool fastTraverse);
    PointSegment *addArcPointSegment(const QVector3D &nextPoint, bool clockwise, const QStringList &args);
    void setLastGcodeCommand(float num);
};

#endif // GCODEPARSER_H
