#include "colorpicker.h"

#include <QPainter>

ColorPicker::ColorPicker(QWidget *parent) :
    QToolButton(parent)
{
//    m_layout = new QHBoxLayout(this);
//    m_frame = new QFrame(this);
//    m_button = new QToolButton(this);
//
//    m_frame->setFrameShape(QFrame::Box);
//
//    m_button->setText("...");
//
//    m_layout->setMargin(0);
//    m_layout->addWidget(m_frame, 1);
//    m_layout->addWidget(m_button);

    connect(this, &QToolButton::clicked, this, &ColorPicker::onButtonClicked);
}

void ColorPicker::paintEvent(QPaintEvent *event)
{
    // draw original stule frame
    QToolButton::paintEvent(event);

    // fill current color inside
    QPainter p(this);
    // draw something that will show transparency if any
    auto draw_rect = rect().adjusted(3, 3, -3, -3);
    p.fillRect(draw_rect.adjusted(0, 0, 0, -draw_rect.height() / 2), Qt::black);
    p.fillRect(draw_rect.adjusted(0, draw_rect.height() / 2, 0, 0), Qt::white);

    // draw color
    p.fillRect(draw_rect, m_color);
}

QColor ColorPicker::color() const
{
    return m_color;
}

void ColorPicker::setColor(const QColor &color)
{
    m_color = color;
    update();
//    m_frame->setStyleSheet(QString("background-color: %1").arg(color.name()));
}

void ColorPicker::onButtonClicked()
{
    QColor color = QColorDialog::getColor(m_color, this, "", QColorDialog::ShowAlphaChannel);

    if (color.isValid()) {
        setColor(color);
        emit colorSelected(color);
    }
}

