#include "imagedialog.h"
#include "window.h"
#include <QBoxLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QImage>
#include <QImageWriter>
#include <QMessageBox>

ImageDialog::ImageDialog(QWidget *parent) : QDialog(parent)
{
    //setAttribute(Qt::WA_DeleteOnClose);

    imageLabel = new QLabel(this);

    buttonOriginal = new QPushButton(tr("Оригинал"), this);


    buttonApply3x3 = new QPushButton(tr("3x3 (быстро)"), this);


    buttonApply4x4 = new QPushButton(tr("4x4 (агрессивно)"), this);


    buttonFix = new QPushButton(tr("Применить изменения"), this);

    QBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addWidget(buttonOriginal);
    buttonsLayout->addWidget(buttonApply3x3);
    buttonsLayout->addWidget(buttonApply4x4);
    buttonsLayout->addWidget(buttonFix);
    buttonsLayout->addStretch(1);

    QBoxLayout *labelLayout = new QHBoxLayout;
    labelLayout->addWidget(imageLabel);

    QBoxLayout *genericLayout = new QVBoxLayout;
    genericLayout->addLayout(labelLayout);
    genericLayout->addLayout(buttonsLayout);

    setLayout(genericLayout);

    connect(buttonOriginal, SIGNAL(clicked()), this, SLOT(setupLabel()));
    connect(buttonApply3x3, SIGNAL(clicked()), this, SLOT(setupLabelFastFilter()));
    connect(buttonApply4x4, SIGNAL(clicked()), this, SLOT(setupLabelAgressiveFilter()));
    connect(buttonFix, SIGNAL(clicked()), this, SLOT(saveImage()));
}

void ImageDialog::setupLabel()
{
    Window *w = new Window;
    QString pwd2 = QDir::currentPath();
    pwd2.append("/");
    pwd2.append(w->listmodel->data(w->listmodel->index(this->index.row(),0),0).toString());

    pmBuffer = QPixmap::fromImage(QImage(pwd2));
    imageLabel->clear();
    imageLabel->setPixmap(pmBuffer);
}

void ImageDialog::setupLabel(QModelIndex index)
{
    this->index = index;
    Window *w = new Window;
    QString pwd2 = QDir::currentPath();
    pwd2.append("/");
    pwd2.append(w->listmodel->data(w->listmodel->index(index.row(),0),0).toString());

    qDebug() << pwd2;
    pmBuffer = QPixmap::fromImage(QImage(pwd2));

    this->pwd2 = pwd2;

    imageLabel->clear();
    imageLabel->setPixmap(pmBuffer);
    //imageLabel->setFixedSize(pm.size());
    this->setFixedSize(this->pmBuffer.size());
    this->setWindowTitle(w->listmodel->data(w->listmodel->index(index.row(),0),0).toString());
    this->show();
}

void ImageDialog::setupLabelFastFilter()
{
    Window *w = new Window;

    pmBuffer = QPixmap::fromImage(w->newImage(this->pwd2));
    imageLabel->clear();
    imageLabel->setPixmap(pmBuffer);
}

void ImageDialog::setupLabelAgressiveFilter()
{
    Window *w = new Window;

    this->pmBuffer = QPixmap::fromImage(w->newImageAgressive(this->pwd2));
    imageLabel->clear();
    imageLabel->setPixmap(pmBuffer);
}

void ImageDialog::saveImage()
{
    QImage img(pmBuffer.toImage());
    QImageWriter writer(this->pwd2, "png");
    writer.write(img);
    QMessageBox::information(0, "Сообщение", "Готово.");
}

ImageDialog::~ImageDialog()
{
    qDebug() << "~imagedialog";
}

