// This file is a part of "Candle" application.
// This file was originally ported from "LineSegment.java" class
// of "Universal GcodeSender" application written by Will Winder
// (https://github.com/winder/Universal-G-Code-Sender)

// Copyright 2015-2016 Hayrullin Denis Ravilevich

#ifndef LINESEGMENT_H
#define LINESEGMENT_H

#include <QVector3D>
#include "pointsegment.h"
#include <vector>

class LineSegment
{
public:
#ifdef USE_STD_CONTAINERS
    using Container = std::vector<LineSegment>;
#else
    using Container = QVector<LineSegment>;
//    using Container = QList<LineSegment>;
#endif
    LineSegment();
    LineSegment(QVector3D a, QVector3D b, int num, PointSegment const &ps, bool isMetric);
    LineSegment(LineSegment const &initial);
    ~LineSegment();

    int getLineNumber() const;
    QList<QVector3D> getPointArray();
    QList<double> getPoints();

    QVector3D const &getStart() const;
    void setStart(QVector3D vector);

    QVector3D const &getEnd() const;
    void setEnd(QVector3D vector);

    void setToolHead(int head);
    int getToolhead() const;
    void setSpeed(double s);
    double getSpeed() const;
    void setIsZMovement(bool isZ);
    bool isZMovement() const;
    void setIsArc(bool isA);
    bool isArc() const;
    void setIsFastTraverse(bool isF);
    bool isFastTraverse() const;

    bool contains(const QVector3D &point) const;

    bool drawn() const;
    void setDrawn(bool drawn);

    bool isMetric() const;
    void setIsMetric(bool isMetric);

    bool isAbsolute() const;
    void setIsAbsolute(bool isAbsolute);

    bool isHightlight() const;
    void setIsHightlight(bool isHightlight);

    int vertexIndex() const;
    void setVertexIndex(int vertexIndex);

    double getSpindleSpeed() const;
    void setSpindleSpeed(double spindleSpeed);

    double getDwell() const;
    void setDwell(double dwell);

    bool isClockwise() const;
    void setIsClockwise(bool isClockwise);

    PointSegment::planes plane() const;
    void setPlane(const PointSegment::planes &plane);

private:
    double m_speed;
    double m_spindleSpeed;
    double m_dwell;
    QVector3D m_first, m_second;
    int m_toolhead;

    // Line properties
    int m_lineNumber;
    int m_vertexIndex;
    PointSegment::planes m_plane;
#if 0
    // use less memory but bit slower
    bool m_isZMovement:1;
    bool m_isArc:1;
    bool m_isMetric:1;
    bool m_isFastTraverse:1;
    bool m_isAbsolute:1;
    bool m_isClockwise:1;
    bool m_isHightlight:1;
    bool m_drawn:1;
#else
    // bit faster but use bit more memory
    bool m_isZMovement;
    bool m_isArc;
    bool m_isMetric;
    bool m_isFastTraverse;
    bool m_isAbsolute;
    bool m_isClockwise;
    bool m_isHightlight;
    bool m_drawn;
#endif
};

#endif // LINESEGMENT_H
