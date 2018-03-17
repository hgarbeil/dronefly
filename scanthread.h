#ifndef SCANTHREAD_H
#define SCANTHREAD_H

#include <QThread>
#include "myspectrometer.h"
#include "phidgetsMot.h"
#include "mysocket.h"
#include "qgpsdevice.h"
#include "processso2.h"
#include "calscan.h"
#include <QList>
#include <QVector>
#include <QTime>
#include <QDate>


class ScanThread : public QThread
{
    Q_OBJECT
public:
    explicit ScanThread(QObject *parent = 0);
    ~ScanThread() ;
    void setObjects (MySpectrometer *myspec, phidgetsMot *mypm) ;
    void setSocket (MySocket *m) ;
    void setModeString (QString *) ;
    void setPS (ProcessSO2 *ps) ;
    void setScanFile (QString);
    void setGPS (QGPSDevice *) ;
    void insertCell (int, int) ;
    void setMirrorFlag (bool) ;
    void setVoltString (char *) ;
    void setCalscan (CalScan *cs) ;
    //QList <QTime> timeList ;
    QVector <double> ppmmList ;
    QVector <double> timeList ;
    QVector <double> angleList ;
    MySpectrometer *myspec ;
    MySocket *mysocket ;
    ProcessSO2 *pSO2 ;
    QGPSDevice *gps ;

    phidgetsMot *mypm ;

    char voltString [120] ;
    bool voltOnlyFlag, scanning , mirrorFlag, polarFlag, autoLevelFlag ;
    int nscans, nscansInsert ;
    double *scandat ;

    QString scanFile, hostname, *modestring;
    QString serverString ;
    CalScan *cs ;

protected:
    void run() ;
signals:
    void plotData (int, double*) ;
    void satAlert (bool) ;
    void scanComplete () ;
    void intChange (int) ;
public slots:

    
};

#endif // SCANTHREAD_H
