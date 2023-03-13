// This file is a part of "Candle" application.
// This file was originally ported from "PointSegment.java" class
// of "Universal GcodeSender" application written by Will Winder
// (https://github.com/winder/Universal-G-Code-Sender)

// Copyright 2015-2016 Hayrullin Denis Ravilevich

#ifndef POINTSEGMENT_H
#define POINTSEGMENT_H

#include <QVector3D>
#include <memory>

#include "arcproperties.h"

class PointSegment {
public:
#ifdef USE_STD_CONTAINERS
    using Container = std::vector<PointSegment>;
    using ContainerPtr = std::vector<PointSegment *>;
#else
    using Container = QVector<PointSegment>;
    using ContainerPtr = QVector<PointSegment *>;
#endif
    enum planes {
        XY,
        ZX,
        YZ
    };

    PointSegment() = default;

    PointSegment(PointSegment const &ps) noexcept
            : m_point(ps.m_point),
              m_toolhead(ps.m_toolhead),
              m_lineNumber(ps.m_lineNumber),
              m_speed(ps.m_speed),
              m_plane(ps.m_plane),
              m_isZMovement(ps.m_isZMovement),
              m_isArc(ps.m_isArc),
              m_isMetric(ps.m_isMetric),
              m_isFastTraverse(ps.m_isFastTraverse),
              m_isAbsolute(ps.m_isAbsolute),
              m_isClockwise(ps.m_isClockwise) {

        if (ps.m_arcProperties) {
            m_arcProperties = std::make_unique<ArcProperties>(ps.center(), ps.getRadius()/*, ps.isClockwise()*/);
        }
    }

    PointSegment &operator=(PointSegment &&other) noexcept {
        // Guard self assignment
        if (this == &other)
            return *this;

        m_point = other.m_point;
        m_toolhead = other.m_toolhead;
        m_lineNumber = other.m_lineNumber;
        m_speed = other.m_speed;
        m_plane = other.m_plane;
        m_isZMovement = other.m_isZMovement;
        m_isMetric = other.m_isMetric;
        m_isFastTraverse = other.m_isFastTraverse;
        m_isAbsolute = other.m_isAbsolute;
        m_isClockwise = other.m_isClockwise;
        m_isArc = other.m_isArc;
        m_arcProperties = std::move(other.m_arcProperties);
        return *this;
    }

    PointSegment(QVector3D const &b, int num) : m_point(b), m_lineNumber(num) {
    }

    PointSegment(QVector3D const &point, int num, QVector3D const &center, double radius, bool clockwise)
            : m_point(point),
              m_lineNumber(num),
              m_isArc(true),
              m_isClockwise(clockwise) {
        m_arcProperties = std::make_unique<ArcProperties>(center, radius/*, clockwise*/);
    }

    ~PointSegment() = default;

    void setPoint(QVector3D const &point) {
        this->m_point = point;
    }

    [[nodiscard]] QVector3D const &point() {
        return m_point;
    }

    [[nodiscard]] QVector<double> points();

    void setToolHead(int head) {
        m_toolhead = head;
    }

    [[nodiscard]] int getToolhead() const {
        return m_toolhead;
    }

    void setLineNumber(int num) {
        m_lineNumber = num;
    }

    [[nodiscard]] int getLineNumber() const {
        return m_lineNumber;
    }

    void setSpeed(double s) {
        m_speed = s;
    }

    [[nodiscard]] double getSpeed() const {
        return m_speed;
    }

    void setIsZMovement(bool isZ) {
        m_isZMovement = isZ;
    }

    [[nodiscard]] bool isZMovement() const {
        return m_isZMovement;
    }

    void setIsMetric(bool isMetric) {
        m_isMetric = isMetric;
    }

    [[nodiscard]] bool isMetric() const {
        return m_isMetric;
    }

    void setIsArc(bool isA) {
        m_isArc = isA;
    }

    [[nodiscard]] bool isArc() const {
        return m_isArc;
    }

    void setIsFastTraverse(bool isF) {
        m_isFastTraverse = isF;
    }

    [[nodiscard]] bool isFastTraverse() const {
        return m_isFastTraverse;
    }

    void setArcCenter(const QVector3D &center) {
        if (m_arcProperties == nullptr) {
            m_arcProperties = std::make_unique<ArcProperties>(center, 0/*, true*/);
            setIsClockwise(true);
            setIsArc(true);
        } else {
            m_arcProperties->center = center;
        }
    }

    [[nodiscard]] QVector<double> centerPoints();

    [[nodiscard]] QVector3D &center() const {
        return m_arcProperties->center;
    }

    void setIsClockwise(bool clockwise) {
        m_isClockwise = clockwise;
    }

    [[nodiscard]] bool isClockwise() const {
        Q_ASSERT(isArc());
        return m_isClockwise;
    }

    void setRadius(double rad) {
        Q_ASSERT(isArc());
        m_arcProperties->radius = rad;
    }

    [[nodiscard]] double getRadius() const {
        Q_ASSERT(isArc());
        return m_arcProperties->radius;
    }

    void convertToMetric() {
        if (m_isMetric) {
            return;
        }

        m_isMetric = true;
        m_point.setX(m_point.x() * 25.4f);
        m_point.setY(m_point.y() * 25.4f);
        m_point.setZ(m_point.z() * 25.4f);

        if (m_isArc && m_arcProperties != nullptr) {
            m_arcProperties->center.setX(m_arcProperties->center.x() * 25.4f);
            m_arcProperties->center.setY(m_arcProperties->center.y() * 25.4f);
            m_arcProperties->center.setZ(m_arcProperties->center.z() * 25.4f);
            m_arcProperties->radius *= 25.4;
        }
    }

    [[nodiscard]] bool isAbsolute() const {
        return m_isAbsolute;
    }

    void setIsAbsolute(bool isAbsolute) {
        m_isAbsolute = isAbsolute;
    }

    [[nodiscard]] planes plane() const {
        return m_plane;
    }

    void setPlane(const planes &plane) {
        m_plane = plane;
    }

    [[nodiscard]] double getSpindleSpeed() const {
        return m_spindleSpeed;
    }

    void setSpindleSpeed(double spindleSpeed) {
        m_spindleSpeed = spindleSpeed;
    }


    [[nodiscard]] double getDwell() const {
        return m_dwell;
    }

    void setDwell(double dwell) {
        m_dwell = dwell;
    }

private:
    std::unique_ptr<ArcProperties> m_arcProperties;
    QVector3D m_point;
    int m_toolhead{0};
    int m_lineNumber{-1};
    double m_speed{0};
    double m_spindleSpeed{0};
    double m_dwell{0};
    planes m_plane{XY};
#if 0
    bool m_isZMovement:1{false};
    bool m_isArc:1{false};
    bool m_isMetric:1{true};
    bool m_isFastTraverse:1{false};
    bool m_isAbsolute:1{true};
    bool m_isClockwise:1{true};
#else
    bool m_isZMovement{false};
    bool m_isArc{false};
    bool m_isMetric{true};
    bool m_isFastTraverse{false};
    bool m_isAbsolute{true};
    bool m_isClockwise{true};
#endif
};

#endif // POINTSEGMENT_H
