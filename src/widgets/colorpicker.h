#ifndef COLORPICKER_H
#define COLORPICKER_H

#include <QWidget>
#include <QHBoxLayout>
#include <QFrame>
#include <QToolButton>
#include <QColorDialog>

class ColorPicker : public QToolButton
{
    Q_OBJECT
public:
    explicit ColorPicker(QWidget *parent = 0);

    QColor color() const;
    void setColor(const QColor &color);

signals:
    void colorSelected(QColor color);

public slots:

private slots:
    void onButtonClicked();
protected:
    virtual void paintEvent(QPaintEvent *event) override;

private:
//    QHBoxLayout *m_layout;
//    QFrame *m_frame;
//    QToolButton *m_button;
    QColor m_color;
};

#endif // COLORPICKER_H
