// This file is a part of "Candle" application.
// Copyright 2015-2016 Hayrullin Denis Ravilevich

#ifndef STYLEDTOOLBUTTON_H
#define STYLEDTOOLBUTTON_H

#include <QWidget>
#include <QAbstractButton>
#include <QPainter>
#include <QStyle>
#include <QStyleOptionFrame>

class StyledToolButton : public QAbstractButton
{
public:
    explicit StyledToolButton(QWidget *parent = 0);

    bool isHover();

    QColor backColor() const;
    void setBackColor(const QColor &backColor);

    QColor foreColor() const;
    void setForeColor(const QColor &foreColor);

    QColor highlightColor() const;
    void setHighlightColor(const QColor &highlightColor);

protected:
#if QT_VERSION_MAJOR < 6
    void enterEvent(QEvent *) override;
#else
    void enterEvent(QEnterEvent *) override;
#endif
    void leaveEvent(QEvent *) override;

private:
    void paintEvent(QPaintEvent *e) override;

    bool m_hovered;
    QColor m_backColor;
    QColor m_foreColor;
    QColor m_highlightColor;
};

#endif // STYLEDTOOLBUTTON_H
