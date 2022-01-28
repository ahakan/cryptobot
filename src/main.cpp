#include "mainwindow.h"
#include "buywindow.h"

#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w(QCoreApplication::applicationDirPath());

//    w.show();

    BuyWindow b;
    b.show();

    return a.exec();
}
