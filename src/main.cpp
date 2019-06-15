#include <QApplication>
#include <QIcon>
#include "mainwindow.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":pi/app.png"));

    MainWindow *mw = new MainWindow();
    mw->show();
    //mw->showMaximized();//FullScreen();

    return app.exec();
}
