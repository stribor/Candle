// This file is a part of "Candle" application.
// Copyright 2015-2016 Hayrullin Denis Ravilevich

#ifndef UTIL
#define UTIL

#include <QColor>
#include <QIcon>
#include <QImage>
#include <QAbstractButton>
#include <QVector3D>
#include <QEventLoop>
#include <QTimer>

// Switch between RGBA and RGB colors passed as shader attributes
#if 1
class VertColVec : public QVector4D {
public:
    using QVector4D::QVector4D;  // Inherit Base's constructors.
    // and add QColor one
    explicit VertColVec(QColor const &color) {
        setX(color.redF());
        setY(color.greenF());
        setZ(color.blueF());
        setW(color.alphaF());
    };
};
#else
class VertColVec : public QVector3D {
public:
    using QVector3D::QVector3D;  // Inherit Base's constructors.
    // and add QColor one
    explicit VertColVec(QColor const &color) {
        setX(color.redF());
        setY(color.greenF());
        setZ(color.blueF());
    };
};
#endif
class Util
{
public:
    static double nMin(double v1, double v2)
    {
        if (!qIsNaN(v1) && !qIsNaN(v2)) return qMin<double>(v1, v2);
        else if (!qIsNaN(v1)) return v1;
        else if (!qIsNaN(v2)) return v2;
        else return qQNaN();
    }

    static double nMax(double v1, double v2)
    {
        if (!qIsNaN(v1) && !qIsNaN(v2)) return qMax<double>(v1, v2);
        else if (!qIsNaN(v1)) return v1;
        else if (!qIsNaN(v2)) return v2;
        else return qQNaN();
    }

    static VertColVec colorToVector(QColor const &color)
    {
        return VertColVec(color);
    }

    static void waitEvents(int ms)
    {
        QEventLoop loop;

        QTimer::singleShot(ms, &loop, SLOT(quit()));
        loop.exec();
    }

    static QIcon invertIconColors(QIcon icon)
    {
        QImage img = icon.pixmap(icon.actualSize(QSize(64, 64))).toImage();
        img.invertPixels();

        return QIcon(QPixmap::fromImage(img));
    }

    static void invertButtonIconColors(QAbstractButton *button)
    {
        button->setIcon(invertIconColors(button->icon()));
    }
};

#endif // UTIL

