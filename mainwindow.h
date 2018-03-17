#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "myspectrometer.h"
#include "phidgetsMot.h"
#include "calscan.h"
#include "scanthread.h"
#include "repeatScan.h"
#include "qgpsdevice.h"
#include "processso2.h"
#include "myinterfacekit.h"
#include "autoint.h"
#include "mysocket.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    MySpectrometer *myspec ;
    QGPSDevice *gps ;
    ProcessSO2 *pso2 ;
    QTimer *qtimer  ;
    phidgetsMot *pm ;
    CalScan *cscan ;
    ScanThread *scanthread ;
    AutoInt *aint ;
    QString *statusString, *ppmmString, timestring ;
    void setParamFile (char *pfile);
    bool voltOnlyFlag, polarFlag, firstFlag, scanState, calFlag ;
    float startTime, endTime ;
    ~MainWindow();


private:
    Ui::MainWindow *ui;

private slots :
    void updateGUI() ;
    void autoDone(int) ;
    void on_aintButton_clicked();
    void on_scanButton_clicked();
    void on_calButton_clicked();
};

#endif // MAINWINDOW_H
