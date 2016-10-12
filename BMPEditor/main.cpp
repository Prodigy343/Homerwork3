#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle("Fusion");
   // a.setWindowTitle("Tarea 3");
    MainWindow w;
    w.show();

    return a.exec();
}
