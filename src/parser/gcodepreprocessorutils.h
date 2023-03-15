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
#include <string_view>
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
using CommandView = std::string_view;
using CommandList = std::vector<std::string>;
inline Command fromQString(QString const &str) { return str.toStdString(); }
inline QString toQString(CommandView str) { return QString::fromUtf8(str); }
inline bool commandContains(CommandView str, CommandView lookFor) { return str.find(lookFor) != Command::npos; }
#else
using Command = QByteArray;
using CommandView = QByteArrayView;
using CommandList = QByteArrayList;
inline Command fromQString(QString const &str) { return str.toUtf8(); }
inline QString toQString(CommandView str) { return QString::fromUtf8(str); }
inline bool commandContains(CommandView str, CommandView lookFor) { return str.contains(lookFor); }
#endif
namespace GcodePreprocessorUtils
{
    using gcodesContainer = std::vector<GCodes>;
    using vectoContainer = std::vector<QVector3D>;

    Command overrideSpeed(CommandView command, double speed, double *original = NULL);
    Command removeComment(CommandView command);
    QString parseComment(QString command);
    Command truncateDecimals(int length, CommandView command);
    Command removeAllWhitespace(CommandView command);
    GCodes parseGCodeEnum(CommandView arg);
    gcodesContainer parseCodesEnum(CommandList const &args, QChar);
    QList<float> parseCodes(const QStringList &args, QChar code);
    QList<int> parseGCodes(QString const &command);
    QList<int> parseMCodes(QString const &command);
    CommandList splitCommand(CommandView command);
    double parseCoord(CommandList const &argList, char c);
    bool parseCoord(CommandView arg, char c, double &outVal);
    bool has_M2_M30(CommandList const &command);
    QVector3D updatePointWithCommand(const QVector3D &initial, double x, double y, double z, bool absoluteMode);
    QVector3D updatePointWithCommand(CommandList const &commandArgs, const QVector3D &initial, bool absoluteMode);
    QVector3D updatePointWithCommand(CommandView command, const QVector3D &initial, bool absoluteMode);
    QVector3D convertRToCenter(QVector3D start, QVector3D end, double radius, bool absoluteIJK, bool clockwise);
    QVector3D updateCenterWithCommand(CommandList const &commandArgs, QVector3D initial, QVector3D nextPoint, bool absoluteIJKMode, bool clockwise);
    QString generateG1FromPoints(QVector3D const &start, QVector3D const &end, bool absoluteMode, int precision);
    double getAngle(QVector3D start, QVector3D end);
    double calculateSweep(double startAngle, double endAngle, bool isCw);
    vectoContainer generatePointsAlongArcBDring(PointSegment::planes plane, QVector3D start, QVector3D end, QVector3D center, bool clockwise, double R, double minArcLength, double arcPrecision, bool arcDegreeMode);
    vectoContainer generatePointsAlongArcBDring(PointSegment::planes plane, QVector3D p1, QVector3D p2, QVector3D center, bool isCw, double radius, double startAngle, double sweep, int numPoints);

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
    double AtoF(std::string_view c);
};

#endif // GCODEPREPROCESSORUTILS_H
