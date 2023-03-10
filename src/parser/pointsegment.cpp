// This file is a part of "Candle" application.
// This file was originally ported from "PointSegment.java" class
// of "Universal GcodeSender" application written by Will Winder
// (https://github.com/winder/Universal-G-Code-Sender)

// Copyright 2015-2016 Hayrullin Denis Ravilevich

#include <QVector>

#include "pointsegment.h"

QVector<double> PointSegment::points()
{
    QVector<double> points;
    points.append(m_point.x());
    points.append(m_point.y());
    return points;
}

QVector<double> PointSegment::centerPoints(){
    QVector<double> points;
    if (m_arcProperties != nullptr) {
        points.append(m_arcProperties->center.x());
        points.append(m_arcProperties->center.y());
        points.append(m_arcProperties->center.z());
    }
    return points;
}
