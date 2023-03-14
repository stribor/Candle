// This file is a part of "Candle" application.
// This file was originally ported from "GcodePreprocessorUtils.java" class
// of "Universal GcodeSender" application written by Will Winder
// (https://github.com/winder/Universal-G-Code-Sender)

// Copyright 2015-2016 Hayrullin Denis Ravilevich

#ifndef GCODEPREPROCESSORUTILS_H
#define GCODEPREPROCESSORUTILS_H

#include <QByteArray>
#include <QMatrix4x4>
#include <cmath>
#include <string>
#include <vector>
#include "QtCore/qcontainerfwd.h"
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

#ifdef USE_STD_CONTAINERS
using Command = std::string;
using CommandList = std::vector<std::string>;
inline Command fromQString(QString const &str) { return str.toStdString(); }
inline QString toQString(Command const &str) { return QString::fromStdString(str); }
#else
using Command = QByteArray;
using CommandList = QByteArrayList;
inline Command fromQString(QString const &str) { return str.toUtf8(); }
inline QString toQString(Command const &str) { return str; }
#endif
class GcodePreprocessorUtils
{
public:
    using gcodesContainer = std::vector<GCodes>;
    using vectoContainer = std::vector<QVector3D>;

    static Command overrideSpeed(Command command, double speed, double *original = NULL);
    static Command removeComment(Command const &command);
    static QString parseComment(QString command);
    static Command truncateDecimals(int length, Command const &command);
    static Command removeAllWhitespace(Command command);
    static GCodes parseGCodeEnum(Command const &arg);
    static gcodesContainer parseCodesEnum(CommandList const &args, QChar);
    static QList<float> parseCodes(const QStringList &args, QChar code);
    static QList<int> parseGCodes(QString const &command);
    static QList<int> parseMCodes(QString const &command);
    static CommandList splitCommand(Command const &command);
    static double parseCoord(CommandList const &argList, char c);
    static bool parseCoord(Command const &arg, char c, double &outVal);
    static QVector3D updatePointWithCommand(const QVector3D &initial, double x, double y, double z, bool absoluteMode);
    static QVector3D updatePointWithCommand(CommandList const &commandArgs, const QVector3D &initial, bool absoluteMode);
    static QVector3D updatePointWithCommand(Command const &command, const QVector3D &initial, bool absoluteMode);
    static QVector3D convertRToCenter(QVector3D start, QVector3D end, double radius, bool absoluteIJK, bool clockwise);
    static QVector3D updateCenterWithCommand(CommandList const &commandArgs, QVector3D initial, QVector3D nextPoint, bool absoluteIJKMode, bool clockwise);
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
    static double AtoF(std::string_view c);
};

#endif // GCODEPREPROCESSORUTILS_H
