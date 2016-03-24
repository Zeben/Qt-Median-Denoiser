
#include "window.h"

#include <QWidget>
#include <QAbstractVideoBuffer>
#include <QAbstractVideoSurface>
#include <QVideoFrame>
#include <QPixmap>
#include <stdlib.h>
#include <QMessageBox>
#include <iostream>
#include <windows.h>
#include <QVector>
#include <QColor>
#include <QtAlgorithms>
#include <qmath.h>
#include <QImageWriter>


Window::Window(QWidget *parent) :
    QVideoWidget(parent),
    videoWidget(0)
{
    player = new QMediaPlayer(this); // выделяем память под видеосервис
    list = new QMediaPlaylist();
    player->setPlaylist(list); // присваиваем лист плееру

    videoWidget = new VideoWidget(this);
    player->setVideoOutput(videoWidget);

    // открыть видео по нажатию кнопки
    QPushButton *openButton = new QPushButton(tr("Открыть..."), this);
    openButton->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    connect(openButton, SIGNAL(clicked()), this, SLOT(open()));

    // проиграть видео по нажатию кнопки
    QPushButton *playButton = new QPushButton(tr("Играть"), this);
    playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    connect(playButton, SIGNAL(clicked()), player, SLOT(play()));

    QPushButton *puseButton = new QPushButton(tr("Пауза"),this);
    puseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    connect(puseButton, SIGNAL(clicked()), player, SLOT(pause()));

    // кнопка с применением сплиттера
    QPushButton *buttonNoiseFilter = new QPushButton(tr("Разбить на кадры"), this);
    buttonNoiseFilter->setIcon(style()->standardIcon(QStyle::SP_TitleBarNormalButton));
    connect(buttonNoiseFilter, SIGNAL(clicked()), this, SLOT(setNoiseFilter()));

    // кнопка для показа/скрытия листа
    QPushButton *buttonShowHideList = new QPushButton(tr("Показать/скрыть список"), this);
    connect(buttonShowHideList, SIGNAL(clicked()), this, SLOT(showHideList()));

    // кнопка для обновления списка
    buttonUpdateModel = new QPushButton(tr("Обновить список"), this);
    buttonUpdateModel->setToolTip("Программа не всегда обновляет фрейм-лист. \nЕсли после повторной разбивки будут показаны не все фреймы - программа сработает некорректно.");
    connect(buttonUpdateModel, SIGNAL(clicked()), this, SLOT(updateModel()));
    buttonUpdateModel->hide();

    // кнопка для сохранения видео
    buttonImplodeFrames = new QPushButton(tr("Конвертация (быстро)"), this);
    buttonImplodeFrames->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));
    buttonImplodeFrames->setToolTip("Быстрый алгоритм денойзинга. Не всегда эффективен против больших пятен\nПрограмма работает в один поток. Если дорожите временем - используйте эту опцию.");
    connect(buttonImplodeFrames, SIGNAL(clicked()), this, SLOT(implodeFrames()));
    buttonImplodeFrames->hide();

    buttonImplodeFramesAgressive = new QPushButton(tr("(агрессивно)"), this);
    buttonImplodeFramesAgressive->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));
    buttonImplodeFramesAgressive->setToolTip("Агрессивный и нереально медленный, но более точный алгоритм. \nПрограмма работает в один поток. Если у Вас стальные нервы - используйте эту опцию.");
    connect(buttonImplodeFramesAgressive, SIGNAL(clicked()), this, SLOT(implodeFramesAgressive()));
    buttonImplodeFramesAgressive->hide();

    checkmatrix = new QCheckBox(this);
    checkmatrix->setText("5x5");
    checkmatrix->setToolTip("Отметьте для предпросмотра изображений с применением агрессивного и медленного алгоритма денойзинга.");
    checkmatrix->hide();


    // создаем слайдер и задаем границы
    slider = new QSlider(Qt::Horizontal, this);

//FIXME: увеличить дискретность перемотки, ибо на маленьких видео хреново себя ведёт
    slider->setRange(0, player->duration() / 1000);

    // создаем лист фреймов
    listview = new QListView(this);
    listmodel = new QStringListModel(this);
    listview->hide();

    // создаем плейлист
    playlistview = new QListView(this);
    playlistmodel = new QStringListModel(this);
    playlistview->hide();

    progress = new QProgressBar(this);

    // создаем лейбл, в котором будет рендериться обработанная картинка

    id = new ImageDialog(this);

    // композиция кнопок плейлиста
    QBoxLayout *listControlsLayout = new QHBoxLayout;
    listControlsLayout->addWidget(buttonUpdateModel);
    listControlsLayout->addWidget(buttonImplodeFrames);
    listControlsLayout->addWidget(buttonImplodeFramesAgressive);
    listControlsLayout->addWidget(checkmatrix);

    // композиция фрейма с листом и кнопками
    QBoxLayout *listPlaylistAndControls = new QVBoxLayout;
    listPlaylistAndControls->addWidget(listview);
    listPlaylistAndControls->addWidget(playlistview);
    listPlaylistAndControls->addLayout(listControlsLayout);
    //listPlaylistAndControls->

    // композиция видеовиджета
    QBoxLayout *displayLayout = new QHBoxLayout;
    displayLayout->addWidget(videoWidget, 2);
    displayLayout->addLayout(listPlaylistAndControls);
    //displayLayout->addWidget(progress);
    //displayLayout->addWidget(listview, 1);

    // композиция кнопок
    QBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->addWidget(openButton);
    controlLayout->addWidget(playButton);
    controlLayout->addWidget(puseButton);
    controlLayout->addWidget(buttonNoiseFilter);
    controlLayout->addWidget(buttonShowHideList);
    controlLayout->addStretch(1); // пружинка

    QBoxLayout *sliderL = new QHBoxLayout;
    sliderL->addWidget(slider);
    sliderL->addWidget(progress);

    // общая композиция
    QBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(displayLayout);
    layout->addLayout(controlLayout);
    layout->addLayout(sliderL);

    setLayout(layout); // окончательная настройка для главного окна-виджета



    connect(slider, SIGNAL(sliderMoved(int)), this, SLOT(seek(int))); //
    connect(player, SIGNAL(positionChanged(qint64)), this, SLOT(setPosition(qint64)));
    connect(player, SIGNAL(durationChanged(qint64)), this, SLOT(durationChanged(qint64)));
    connect(this, SIGNAL(updateModelSignal()), this, SLOT(updateModel()));
    connect(listview, SIGNAL(clicked(QModelIndex)), id, SLOT(setupLabel(QModelIndex)));
    updateModel();
}

QString filename;
void Window::open() // слот открытия плеера
{
    filename = QFileDialog::getOpenFileName(this,
                                                        tr("Выбрать файл"),
                                                        QDir::currentPath(),
                                                        tr("mp4 (*.mp4 *.avi)")
                                                        );
    addToPlay(filename);
    qDebug() << "open";
}

void Window::addToPlay(const QString& filename)
{
    QUrl url = QUrl::fromLocalFile(filename);
    list->addMedia(url);
    qDebug() << "add to playlist";
}

void Window::setNoiseFilter()
{
    if(filename.isEmpty())
    {
        QMessageBox::critical(0, "Ошибка", "Видео не выбрано, нечего разбивать.");
    } else
    {

        /*узнаем директорию, в которой запускается программа*/
        QString pwd(QDir::currentPath());

        /*формируем комманду для запуска qtav*/
        //QString wincmd = pwd+"/QtAV/qtav -i \""+filename+"\" -pix_fmt gray -f image2 image-%07d.png";
        QString wincmd = pwd+"/QtAV/qtav -i \""+filename+"\" -f image2 image-%07d.png";
        /*отправляем её интерпретатору*/
        WinExec(wincmd.toStdString().c_str(), SW_HIDE);
        //system(wincmd.toStdString().c_str());
        //QMessageBox::information(0,"Сообщение", "Готово. Проверьте корневую папку с приложением или откройте список файлов.");
        std::cout << wincmd.toStdString();

        /*проверяем команду*/
        //qDebug() << "pwd: " << pwd;
        qDebug() << "wincmd: " << wincmd;
        emit updateModelSignal();
    }
}

void Window::setPosition(qint64 progress)
{
    slider->setValue(progress / 100);
    qDebug() << "position/progress = " << progress;
    qDebug() << player->duration() / 1000;
}

void Window::seek(int sec)
{
    player->setPosition(sec * 100);
}

void Window::durationChanged(qint64 sec)
{
    slider->setMaximum(sec / 100);
    qDebug() << "change_duration";
}

void Window::showHideList()
{
    if(listview->isHidden())
    {
        listview->show();
        //playlistview->show();
        buttonUpdateModel->show();
        buttonImplodeFrames->show();
        //checkmatrix->show();
        buttonImplodeFramesAgressive->show();
    }
    else
    {
        listview->hide();
        playlistview->hide();
        buttonUpdateModel->hide();
        buttonImplodeFrames->hide();
        checkmatrix->hide();
        buttonImplodeFramesAgressive->hide();
    }
}

void Window::updateModel()
{
    qDebug() << "updating model...";
    QDir dir;
    dir.setNameFilters(QStringList() << "*.png");
    listmodel->setStringList(dir.entryList());

    listview->setModel(listmodel);
}

void Window::updatePlayListModel()
{
    qDebug() << "updating playlist...";
}

void Window::modelItemOriginalClicked(QModelIndex index)
{

    QString pwd2 = QDir::currentPath();
    pwd2.append("/");
    pwd2.append(listmodel->data(listmodel->index(index.row(), 0), 0).toString());
    qDebug() << pwd2;
    QPixmap pm = QPixmap::fromImage(QImage(pwd2));
}

QImage Window::newImage(QString fullpath)
{
    // расчёт медианы в матрице 3х3
    QImage origin;
    origin.load(fullpath);
    QVector<QRgb> pixelVector;
    QVector<int> arrRange = {-1,0,1};
    int h, w, valueOut, valueInt, pixelCenter = 0;
        QCoreApplication::processEvents();
        for(h = 1; h < origin.height() - 1; h++)
        {
            for(w = 1; w < origin.width() - 1; w++)
            {
                foreach (valueOut, arrRange)
                {
                    foreach (valueInt, arrRange)
                    {
                        pixelVector.append(QColor(origin.pixel(w + valueInt, h + valueOut)).rgb());
                        ++pixelCenter;
                    }

                }
                std::sort(pixelVector.begin(), pixelVector.end());
                origin.setPixel(w, h, pixelVector.at(pixelCenter/2));
                pixelVector.clear();
                pixelCenter = 0;
            }
        }
    return origin;
}

QImage Window::newImageAgressive(QString fullpath)
{
    // расчёт медианы в матрице 5x5
    QImage origin;
    origin.load(fullpath);
    QVector<QRgb> pixelVector;
    QVector<int> arrRange = {-2,-1,0,1,2};
    int h, w, valueOut, valueInt, pixelCenter = 0;
        QCoreApplication::processEvents();
        for(h = 2; h < origin.height() - 2; h++)
        {
            for(w = 2; w < origin.width() - 2; w++)
            {
                foreach (valueOut, arrRange)
                {
                    foreach (valueInt, arrRange)
                    {
                        pixelVector.append(QColor(origin.pixel(w + valueInt, h + valueOut)).rgb());
                        ++pixelCenter;
                    }

                }
                std::sort(pixelVector.begin(), pixelVector.end());
                origin.setPixel(w, h, pixelVector.at(pixelCenter/2));
                pixelVector.clear();
                pixelCenter = 0;
            }
        }
    return origin;
}

void Window::implodeFrames()
{
    /*узнаем директорию, в которой запускается программа*/
    QString pwd(QDir::currentPath());
    progress->setRange(0, listmodel->rowCount());
    progress->setValue(listmodel->rowCount() / 2);
    // переписываем все картинки
    // считываем количество элементов с листа
    for(int h = 1; h <= listmodel->rowCount(); h++)
    {
        progress->setValue(h);
        // количество циклов зависит от количества обрабатываемых кадров
        // считываем полный путь и название обрабатываемого фрейма
        QString pwd3 = QDir::currentPath();
        pwd3.append("/");
        pwd3.append(listmodel->data(listmodel->index(h-1, 0), 0).toString());
        qDebug() << pwd3;
        if(!pwd3.contains("image"))
        {
            QMessageBox::critical(0, "Ошибка", "Похоже, что Вы попытались склеить картинки, не относящиеся к исходному видео. Придумайте что-нибудь другое.");
            break;
        }

        QImage img(newImage(pwd3));
        QImageWriter writer(pwd3, "png");
        writer.setText("Author", "azaza");
        writer.write(img);

        // нужно перезаписать файлы
    }

    /*формируем комманду для запуска qtav*/
    QDir mkd;
    mkd.mkdir(pwd+"/exported");
    QString wincmd = pwd+"/QtAV/qtav -r 24 -y -i image-%07d.png -vcodec mpeg4 -b:v 40000000 -q 1 \""+pwd+"/exported/output.avi\"";

    /*отправляем её интерпретатору*/
    WinExec(wincmd.toStdString().c_str(), SW_HIDE);
    //system(wincmd.toStdString().c_str());
    //QMessageBox::information(0,"Сообщение", "Готово. Проверьте корневую папку с приложением или откройте список файлов.");
    std::cout << wincmd.toStdString();

    /*проверяем команду*/
    //qDebug() << "pwd: " << pwd;
    qDebug() << "wincmd: " << wincmd;
    progress->setValue(0);
    emit updateModelSignal();
}

void Window::implodeFramesAgressive()
{
    /*узнаем директорию, в которой запускается программа*/
    QString pwd(QDir::currentPath());
    progress->setRange(0, listmodel->rowCount());
    progress->setValue(listmodel->rowCount() / 2);
    // переписываем все картинки
    // считываем количество элементов с листа
    for(int h = 1; h <= listmodel->rowCount(); h++)
    {
        progress->setValue(h);
        // количество циклов зависит от количества обрабатываемых кадров
        // считываем полный путь и название обрабатываемого фрейма
        QString pwd3 = QDir::currentPath();
        pwd3.append("/");
        pwd3.append(listmodel->data(listmodel->index(h-1, 0), 0).toString());
        qDebug() << pwd3;
        if(!pwd3.contains("image"))
        {
            QMessageBox::critical(0, "Ошибка", "Похоже, что Вы попытались склеить картинки, не относящиеся к исходному видео. Придумайте что-нибудь другое.");
            break;
        }

        QImage img(newImageAgressive(pwd3));
        QImageWriter writer(pwd3, "png");
        writer.setText("Author", "azaza");
        writer.write(img);

        // нужно перезаписать файлы
    }

    /*формируем комманду для запуска qtav*/
    QDir mkd;
    mkd.mkdir(pwd+"/exported");
    QString wincmd = pwd+"/QtAV/qtav -r 24 -y -i image-%07d.png -vcodec mpeg4 -b:v 40000000 -q 1 \""+pwd+"/exported/output.avi\"";

    /*отправляем её интерпретатору*/
    WinExec(wincmd.toStdString().c_str(), SW_HIDE);
    //system(wincmd.toStdString().c_str());
    //QMessageBox::information(0,"Сообщение", "Готово. Проверьте корневую папку с приложением или откройте список файлов.");
    std::cout << wincmd.toStdString();

    /*проверяем команду*/
    //qDebug() << "pwd: " << pwd;
    qDebug() << "wincmd: " << wincmd;
    progress->setValue(0);
    emit updateModelSignal();
}


Window::~Window()
{
}
