#ifndef IMAGEDIALOG_H
#define IMAGEDIALOG_H
// по клику в плейлист мы должны показать исходную картинку
// должно быть две кнопки: 3х3, 4х4
// по их нажатию - применять к кадрам
// ещё кнопка "сохранить изменения"
#include <QDebug>
#include <QLabel>
#include <QPushButton>
#include <QDialog>
#include <QModelIndex>

class ImageDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ImageDialog(QWidget *parent = 0);
    QLabel      *imageLabel;
    ~ImageDialog();

signals:

private:
    QPushButton *buttonOriginal,
                *buttonApply3x3,
                *buttonApply4x4,
                *buttonFix;

    QModelIndex index;
    QString pwd2;
    QPixmap pmBuffer;

private slots:
    void setupLabel();
    void setupLabel(QModelIndex index);
    void setupLabelFastFilter();
    void setupLabelAgressiveFilter();
    void saveImage();


};

#endif // IMAGEDIALOG_H
