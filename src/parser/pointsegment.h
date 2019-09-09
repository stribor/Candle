// This file is a part of "Candle" application.
// This file was originally ported from "PointSegment.java" class
// of "Universal GcodeSender" application written by Will Winder
// (https://github.com/winder/Universal-G-Code-Sender)

// Copyright 2015-2016 Hayrullin Denis Ravilevich

#ifndef POINTSEGMENT_H
#define POINTSEGMENT_H

#include <QVector3D>

#include "arcproperties.h"

class PointSegment
{
public:
#ifdef USE_STD_CONTAINERS
    using Container = std::vector<PointSegment*>;
#else
    using Container = QVector<PointSegment*>;
//    using Container = QList<PointSegment*>;
#endif
    enum planes {
        XY,
        ZX,
        YZ
    };

    PointSegment();
    PointSegment(PointSegment const &ps);
    PointSegment(QVector3D const &b, int num);
    PointSegment(QVector3D const &point, int num, QVector3D const &center, double radius, bool clockwise);
    ~PointSegment();
    void setPoint(QVector3D const &point);
    QVector3D const & point();

    QVector<double> points();
    void setToolHead(int head);
    int getToolhead() const;
    void setLineNumber(int num);
    int getLineNumber() const;
    void setSpeed(double s);
    double getSpeed() const;
    void setIsZMovement(bool isZ);
    bool isZMovement() const;
    void setIsMetric(bool m_isMetric);
    bool isMetric() const;
    void setIsArc(bool isA);
    bool isArc() const;
    void setIsFastTraverse(bool isF);
    bool isFastTraverse() const;
    void setArcCenter(const QVector3D &center);
    QVector<double> centerPoints();
    QVector3D &center() const;
    void setIsClockwise(bool clockwise);
    bool isClockwise() const;
    void setRadius(double rad);
    double getRadius() const;
    void convertToMetric();

    bool isAbsolute() const;
    void setIsAbsolute(bool isAbsolute);

    planes plane() const;
    void setPlane(const planes &plane);

    double getSpindleSpeed() const;
    void setSpindleSpeed(double spindleSpeed);

    double getDwell() const;
    void setDwell(double dwell);

private:
    ArcProperties *m_arcProperties{};
    QVector3D m_point;
    int m_toolhead;
    int m_lineNumber;
    double m_speed;
    double m_spindleSpeed;
    double m_dwell;
    planes m_plane;
    bool m_isMetric:1;
    bool m_isZMovement:1;
    bool m_isArc:1;
    bool m_isFastTraverse:1;
    bool m_isAbsolute:1;
};

#endif // POINTSEGMENT_H
