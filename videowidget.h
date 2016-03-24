#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H
#include <QVideoWidget>
#include <QPalette>

class VideoWidget : public QVideoWidget
{
    Q_OBJECT

public:
    VideoWidget(QWidget *parent = 0); // видеовиджет принадлежит базовому виджету
};

#endif // VIDEOWIDGET_H
