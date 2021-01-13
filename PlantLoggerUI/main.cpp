#include "mainwindow.h"
#include <QApplication>

#include <QDate>
#include <QTime>
#include <QDebug>
#include <QDateTime>

#include <QFile>
#include <QTextStream>

#define COMMANDLINE 0

int main(int argc, char *argv[])
{

#if COMMANDLINE == 1
    if(argc != 2)
    {
        return 1;
    }

    QFile file(argv[1]);
    QTextStream out(&file);

    if(!file.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        return 1;
    }

    file.close();
#endif

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
