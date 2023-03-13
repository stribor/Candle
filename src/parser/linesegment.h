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

class LineSegment {
public:
#ifdef USE_STD_CONTAINERS
    using Container = std::vector<LineSegment>;
#else
    using Container = QVector<LineSegment>;
#endif

    LineSegment() = default;

    LineSegment(QVector3D a, QVector3D b, int num, PointSegment const &ps, bool isMetric)
            : m_speed(ps.getSpeed()),
              m_spindleSpeed(ps.getSpindleSpeed()),
              m_dwell(ps.getDwell()),
              m_first(a),
              m_second(b),
              m_lineNumber(num),
              m_plane(ps.plane()),
              m_isZMovement(ps.isZMovement()),
              m_isArc(ps.isArc()),
              m_isMetric(isMetric),
              m_isFastTraverse(ps.isFastTraverse()),
              m_isAbsolute(ps.isAbsolute()) {

        if (m_isArc)
            m_isClockwise = ps.isClockwise();
    }

    [[nodiscard]] int getLineNumber() const { return m_lineNumber; }

    [[nodiscard]] QList<QVector3D> getPointArray();

    [[nodiscard]] QList<double> getPoints();

    [[nodiscard]] QVector3D const &getStart() const { return m_first; }

    void setStart(QVector3D vector) { m_first = vector; }

    QVector3D const &getEnd() const { return m_second; }

    void setEnd(QVector3D vector) { m_second = vector; }

    void setToolHead(int head) { m_toolhead = head; }

    [[nodiscard]] int getToolhead() const { return m_toolhead; }

    void setSpeed(double s) { m_speed = s; }

    [[nodiscard]] double getSpeed() const { return m_speed; }

    void setIsZMovement(bool isZ) { m_isZMovement = isZ; }

    [[nodiscard]] bool isZMovement() const { return m_isZMovement; }

    void setIsArc(bool isA) { m_isArc = isA; }

    [[nodiscard]] bool isArc() const { return m_isArc; }

    void setIsFastTraverse(bool isF) { m_isFastTraverse = isF; }

    [[nodiscard]] bool isFastTraverse() const { return m_isFastTraverse; }

    bool contains(const QVector3D &point) const;

    [[nodiscard]] bool drawn() const { return m_drawn; }

    void setDrawn(bool drawn) { m_drawn = drawn; }

    [[nodiscard]] bool isMetric() const { return m_isMetric; }

    void setIsMetric(bool isMetric) { m_isMetric = isMetric; }

    [[nodiscard]] bool isAbsolute() const { return m_isAbsolute; }

    void setIsAbsolute(bool isAbsolute) { m_isAbsolute = isAbsolute; }

    [[nodiscard]] bool isHightlight() const { return m_isHightlight; }

    void setIsHightlight(bool isHightlight) { m_isHightlight = isHightlight; }

    [[nodiscard]] int vertexIndex() const { return m_vertexIndex; }

    void setVertexIndex(int vertexIndex) { m_vertexIndex = vertexIndex; }

    [[nodiscard]] double getSpindleSpeed() const { return m_spindleSpeed; }

    void setSpindleSpeed(double spindleSpeed) { m_spindleSpeed = spindleSpeed; }

    [[nodiscard]] double getDwell() const { return m_dwell; }

    void setDwell(double dwell) { m_dwell = dwell; }

    [[nodiscard]] bool isClockwise() const { return m_isClockwise; }

    void setIsClockwise(bool isClockwise) { m_isClockwise = isClockwise; }

    [[nodiscard]] PointSegment::planes plane() const { return m_plane; }

    void setPlane(const PointSegment::planes &plane) { m_plane = plane; }

private:
    double m_speed{};
    double m_spindleSpeed{};
    double m_dwell{};
    QVector3D m_first{};
    QVector3D m_second{};

    //DEFAULT TOOLHEAD ASSUMED TO BE 0!
    int m_toolhead{0};

    // Line properties
    int m_lineNumber{-1};
    int m_vertexIndex{-1};
    PointSegment::planes m_plane{};
#if 0
    // use less memory but bit slower
    bool m_isZMovement:1{false};
    bool m_isArc:1{false};
    bool m_isMetric:1{true};
    bool m_isFastTraverse:1{false};
    bool m_isAbsolute:1{true};
    bool m_isClockwise:1{false};
    bool m_isHightlight:1{false};
    bool m_drawn:1{false};
#else
    // bit faster but use bit more memory
    bool m_isZMovement{false};
    bool m_isArc{false};
    bool m_isMetric{true};
    bool m_isFastTraverse{false};
    bool m_isAbsolute{true};
    bool m_isClockwise{false};
    bool m_isHightlight{false};
    bool m_drawn{false};
#endif
};

#endif // LINESEGMENT_H
