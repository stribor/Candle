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
#include <utility>

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

namespace Util
{
    constexpr double nMin(double v1, double v2)
    {
        if (!qIsNaN(v1) && !qIsNaN(v2)) return qMin<double>(v1, v2);
        else if (!qIsNaN(v1)) return v1;
        else if (!qIsNaN(v2)) return v2;
        else return qQNaN();
    }

    constexpr double nMax(double v1, double v2)
    {
        if (!qIsNaN(v1) && !qIsNaN(v2)) return qMax<double>(v1, v2);
        else if (!qIsNaN(v1)) return v1;
        else if (!qIsNaN(v2)) return v2;
        else return qQNaN();
    }

    inline QIcon invertIconColors(QIcon icon)
    {
        QImage img = icon.pixmap(icon.actualSize(QSize(64, 64))).toImage();
        img.invertPixels();

        return QIcon(QPixmap::fromImage(img));
    }

    inline void invertButtonIconColors(QAbstractButton *button)
    {
        button->setIcon(invertIconColors(button->icon()));
    }

    /// \brief copy elements from range [first, last) to range [d_first, d_first + (last - first)) while pred(first) is true
    /// \return pair of iterators: first - iterator to the first element in the range [first, last) for which pred(first) is false,
    ///  second - iterator to the element in the destination range, one past the last element copied
    template<typename InputIterator, typename OutputIterator, typename Pred>
    std::pair<InputIterator, OutputIterator> copy_while(InputIterator first, InputIterator last, OutputIterator d_first, Pred pred)
    {
        while (first != last && pred(first)) {
            *d_first = *first;
            ++first;
            ++d_first;
        }
        return {first, d_first};
    }

    /// \brief copy elements from range [first, last) to range [d_first, d_first + (last - first)) while pred(first) is false
    /// \return pair of iterators: first - iterator to the first element in the range [first, last) for which pred(first) is true,
    ///  second - iterator to the element in the destination range, one past the last element copied
    template<typename InputIterator, typename OutputIterator, typename Pred>
    std::pair<InputIterator, OutputIterator> copy_until(InputIterator first, InputIterator last, OutputIterator d_first, Pred pred)
    {
        while (first != last && !pred(first)) {
            *d_first = *first;
            ++first;
            ++d_first;
        }
        return {first, d_first};
    }
}

#endif // UTIL

