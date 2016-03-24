#include "window.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Window win;
    win.resize(800, 600);
    win.setWindowTitle("MainWindow");
    win.show();
    return app.exec();
}
