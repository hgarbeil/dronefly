#ifndef REPEATSCAN_H
#define REPEATSCAN_H

#include <QThread>
#include "myspectrometer.h"
#include "phidgetsMot.h"
#include "qgpsdevice.h"
#include "processso2.h"
#include "calscan.h"
#include "mysocket.h"
#include <QList>
#include <QVector>
#include <QTime>
#include <QDate>


class repeatScan : public QThread
{
    Q_OBJECT
public:
    explicit repeatScan(QObject *parent = 0);
    ~repeatScan() ;
    void setObjects (MySpectrometer *myspec, phidgetsMot *mypm) ;
    void setStartTime (float t0, float t1) ;
    void setModeString (QString *) ;
    void setPS (ProcessSO2 *ps) ;
    void setScanFile (QString);
    void setGPS (QGPSDevice *) ;
    void setWorkdir (QString w) ;
    void insertCell (int, int) ;
    void setMirrorFlag (bool);
    void setVoltString (char *) ;
    void setSocket (MySocket *) ;
    void setCalscan (CalScan *cs) ;
    //QList <QTime> timeList ;
    QVector <double> ppmmList ;
    QVector <double> timeList ;

    QVector <double> angleList ;
    QString workdir ;
    MySpectrometer *myspec ;
    MySocket *mysocket ;
    ProcessSO2 *pSO2 ;
    QGPSDevice *gps ;

    phidgetsMot *mypm ;
    CalScan *cs ;

    char voltString [120] ;
    bool voltOnlyFlag ;
    bool scanning, mirrorFlag, polarFlag, autoLevelFlag ;
    int nscans, nscansInsert ;
    float startTime, endTime ;
    double *scandat ;
    QString scanFile, hostname ;
    QString serverString ;


    QString *modestring ;
protected:
    void run() ;
signals:
    void plotData (int, double*) ;
    void satAlert (bool) ;
    void scanComplete() ;
    void intChange (int);
    void reconnect() ;
public slots:

    
};

#endif // REPEATSCAN_H
