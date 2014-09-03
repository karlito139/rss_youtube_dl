#include "mainwindow.h"
#include <QApplication>


QString *pathToFiles;



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}

