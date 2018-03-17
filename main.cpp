#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    char pfile [240] ;
    QApplication a(argc, argv);
    MainWindow w;
    strcpy (pfile, *++argv) ;
    w.setParamFile (pfile) ;
    w.show();



    return a.exec();
}
