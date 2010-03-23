#include <QtGui>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QLocale hindi(QLocale::Hindi,QLocale::India);
    QLocale::setDefault(hindi);
    MainWindow w;
    w.show();
    return a.exec();
}
