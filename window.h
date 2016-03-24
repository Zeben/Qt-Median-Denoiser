#ifndef WINDOW_H
#define WINDOW_H

#include "imagedialog.h"
#include "videowidget.h" // подключаем видеовиджет - уже настроен

#include <QFileDialog>
#include <QBoxLayout>   // композиторы
#include <QvideoWidget> // видеовиджет, куда стримится видео с плеера
#include <QPushButton> // кнопки
#include <QMediaPlayer> // класс работы с видео
#include <QMediaPlaylist> // плейлист, не отображается
#include <QSlider> // слайдер
#include <QListView> // представление для показа картинок
#include <QStringListModel> // модель для показа картинок
#include <QImage> // низкоуровневая работа с картинками
#include <QLabel>
#include <QCheckBox>
#include <QProgressBar>
#include <QCoreApplication>
#include <QDialog>

class Window : public QVideoWidget
{
    Q_OBJECT

public:
    Window(QWidget *parent = 0);
    ~Window();
    QStringListModel *listmodel;

private:
    ImageDialog *id;


    QMediaPlayer *player; // объект плеера
    QMediaPlaylist *list; // список видео, может пригодится
    QVideoWidget *videoWidget; // объект нашего виджета
    QSlider *slider; // типа перемотка
    QListView *listview, *playlistview;
    QStringListModel *playlistmodel;


    QProgressBar *progress;


    QPushButton *buttonUpdateModel;

    QPushButton *buttonImplodeFrames;
    QPushButton *buttonImplodeFramesAgressive;
    QCheckBox *checkmatrix;

private slots:
    void open(); // слот открытия файлов
    void addToPlay(const QString &filename); // добавить в плейлист для проигрывания
    void setNoiseFilter();
    void setPosition(qint64 progress);
    void seek(int sec);
    void durationChanged(qint64 sec);
    void showHideList();

    void updateModel();
    void updatePlayListModel();

    void implodeFrames();
    void implodeFramesAgressive();

public slots:
    void modelItemOriginalClicked(QModelIndex index);
    QImage newImage(QString fullpath);
    QImage newImageAgressive(QString fullpath);

signals:
    void updateModelSignal();
};

#endif // WINDOW_H
