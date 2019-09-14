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
    G91_1,
};

class GcodePreprocessorUtils : public QObject
{
    Q_OBJECT
public:
    using gcodesContainer = std::vector<GCodes>;
    using vectoContainer = std::vector<QVector3D>;

    static QByteArray overrideSpeed(QString command, double speed, double *original = NULL);
    static QByteArray removeComment(QByteArray command);
    static QString parseComment(QString command);
    static QByteArray truncateDecimals(int length, QString command);
    static QByteArray removeAllWhitespace(QByteArray command);
    static GCodes parseGCodeEnum(QByteArray const &arg);
    static gcodesContainer parseCodesEnum(QByteArrayList const &args, QChar);
    static QList<float> parseCodes(const QStringList &args, QChar code);
    static QList<int> parseGCodes(QString const &command);
    static QList<int> parseMCodes(QString const &command);
    static QByteArrayList splitCommand(QByteArray const &command);
    static double parseCoord(QByteArrayList const &argList, char c);
    static bool parseCoord(QByteArray const &arg, char c, double &outVal);
    static QVector3D updatePointWithCommand(const QVector3D &initial, double x, double y, double z, bool absoluteMode);
    static QVector3D updatePointWithCommand(QByteArrayList const &commandArgs, const QVector3D &initial, bool absoluteMode);
    static QVector3D updatePointWithCommand(QByteArray const &command, const QVector3D &initial, bool absoluteMode);
    static QVector3D convertRToCenter(QVector3D start, QVector3D end, double radius, bool absoluteIJK, bool clockwise);
    static QVector3D updateCenterWithCommand(QByteArrayList const &commandArgs, QVector3D initial, QVector3D nextPoint, bool absoluteIJKMode, bool clockwise);
    static QString generateG1FromPoints(QVector3D const &start, QVector3D const &end, bool absoluteMode, int precision);
    static double getAngle(QVector3D start, QVector3D end);
    static double calculateSweep(double startAngle, double endAngle, bool isCw);
    static vectoContainer generatePointsAlongArcBDring(PointSegment::planes plane, QVector3D start, QVector3D end, QVector3D center, bool clockwise, double R, double minArcLength, double arcPrecision, bool arcDegreeMode);
    static vectoContainer generatePointsAlongArcBDring(PointSegment::planes plane, QVector3D p1, QVector3D p2, QVector3D center, bool isCw, double radius, double startAngle, double sweep, int numPoints);

    constexpr static bool isDigit(char c) {
        return c > 47 && c < 58;
    }

    constexpr static bool isLetter(char c){
        return (c > 64 && c < 91) || (c > 96 && c < 123);
    }
    constexpr static char toUpper(char c) {
        return (c >= 'a' && c <= 'z') ? c - 32 : c;
    }
    constexpr static char toLower(char c) {
        return (c >= 'A' && c <= 'Z') ? c + 32 : c;
    }
    static double AtoF(char const *c);
signals:

public slots:

private:

};

#endif // GCODEPREPROCESSORUTILS_H
