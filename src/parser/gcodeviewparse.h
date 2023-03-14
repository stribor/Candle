// This file is a part of "Candle" application.
// This file was originally ported from "GcodeViewParse.java" class
// of "Universal GcodeSender" application written by Will Winder
// (https://github.com/winder/Universal-G-Code-Sender)

// Copyright 2015-2016 Hayrullin Denis Ravilevich

#ifndef GCODEVIEWPARSE_H
#define GCODEVIEWPARSE_H

#include <QObject>
#include <QVector3D>
#include <QVector2D>
#include "linesegment.h"
#include "gcodeparser.h"
#include "utils/util.h"

#ifdef USE_STD_CONTAINERS
using indexContainer = std::vector<int>;
using indexVector = std::vector<indexContainer>;
#else
using indexContainer = QVector<int>;
using indexVector = QList<indexContainer>;
#endif

class GcodeViewParse
{
public:

    explicit GcodeViewParse();

    QVector3D &getMinimumExtremes();
    QVector3D &getMaximumExtremes();
    double getMinLength() const;
    QSize getResolution() const;
    LineSegment::Container toObjRedux(CommandList const &gcode, double arcPrecision, bool arcDegreeMode);
    LineSegment::Container &getLineSegmentList();
    LineSegment::Container getLinesFromParser(GcodeParser *gp, double arcPrecision, bool arcDegreeMode);

    LineSegment::Container & getLines();
    indexVector &getLinesIndexes();

    void reset();

private:

    // Parsed object
    QVector3D m_min, m_max;
    double m_minLength;
    LineSegment::Container m_lines;
    indexVector m_lineIndexes;

    // Parsing state.
    QVector3D lastPoint;
    int currentLine; // for assigning line numbers to segments.

    bool absoluteMode;
    bool absoluteIJK;

    // Debug
    bool debug;
    void testExtremes(QVector3D p3d);
    void testExtremes(double x, double y, double z);
    void testLength(const QVector3D &start, const QVector3D &end);
};

#endif // GCODEVIEWPARSE_H
