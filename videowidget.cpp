#include "videowidget.h"

VideoWidget::VideoWidget(QWidget *parent) : QVideoWidget(parent)
{
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    QPalette p = palette(); // создаем палитру для видеовиджета
    p.setColor(QPalette::Window, Qt::black); // красив в синий
    this->setPalette(p); // применяем настройки палитры для видеовиджета

    setAttribute(Qt::WA_OpaquePaintEvent);
}
