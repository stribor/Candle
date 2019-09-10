// This file is a part of "Candle" application.
// This file was originally ported from "GcodePreprocessorUtils.java" class
// of "Universal GcodeSender" application written by Will Winder
// (https://github.com/winder/Universal-G-Code-Sender)

// Copyright 2015-2016 Hayrullin Denis Ravilevich

#ifndef GCODEPREPROCESSORUTILS_H
#define GCODEPREPROCESSORUTILS_H

#include <QObject>
#include <QMatrix4x4>
#include <cmath>
#include "pointsegment.h"

enum GCodes{
    unknown,
    G00,
    G01,
    G38_2,
    G38_3, // not yet used
    G38_4, // not yet used
    G38_5, // not yet used
    G02,
    G03,
    G07, // not yet used
    G08, // not yet used
    G17,
    G18,
    G19,
    G20,
    G21,
    G05_1, // not yet used
    G05_2, // not yet used
    G90,
    G90_1,
    G91,
    G91_1
};

class GcodePreprocessorUtils : public QObject
{
    Q_OBJECT
public:
    using gcodesContainer = std::vector<GCodes>;
    using vectoContainer = std::vector<QVector3D>;

    static QString overrideSpeed(QString command, double speed, double *original = NULL);
    static QString removeComment(QString command);
    static QString parseComment(QString command);
    static QString truncateDecimals(int length, QString command);
    static QString removeAllWhitespace(QString command);
    static gcodesContainer parseCodesEnum(const QStringList &args, QChar);
    static QList<float> parseCodes(const QStringList &args, QChar code);
    static QList<int> parseGCodes(QString const &command);
    static QList<int> parseMCodes(QString const &command);
    static QStringList splitCommand(const QString &command);
    static double parseCoord(QStringList const &argList, QChar c);
    static QVector3D updatePointWithCommand(const QVector3D &initial, double x, double y, double z, bool absoluteMode);
    static QVector3D updatePointWithCommand(const QStringList &commandArgs, const QVector3D &initial, bool absoluteMode);
    static QVector3D updatePointWithCommand(const QString &command, const QVector3D &initial, bool absoluteMode);
    static QVector3D convertRToCenter(QVector3D start, QVector3D end, double radius, bool absoluteIJK, bool clockwise);
    static QVector3D updateCenterWithCommand(QStringList const &commandArgs, QVector3D initial, QVector3D nextPoint, bool absoluteIJKMode, bool clockwise);
    static QString generateG1FromPoints(QVector3D const &start, QVector3D const &end, bool absoluteMode, int precision);
    static double getAngle(QVector3D start, QVector3D end);
    static double calculateSweep(double startAngle, double endAngle, bool isCw);
    static vectoContainer generatePointsAlongArcBDring(PointSegment::planes plane, QVector3D start, QVector3D end, QVector3D center, bool clockwise, double R, double minArcLength, double arcPrecision, bool arcDegreeMode);
    static vectoContainer generatePointsAlongArcBDring(PointSegment::planes plane, QVector3D p1, QVector3D p2, QVector3D center, bool isCw, double radius, double startAngle, double sweep, int numPoints);
    static inline bool isDigit(char c);
    static inline bool isLetter(char c);
    static inline char toUpper(char c);
signals:

public slots:

private:

};

#endif // GCODEPREPROCESSORUTILS_H
