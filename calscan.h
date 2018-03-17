#ifndef CALSCAN_H
#define CALSCAN_H

#include <QThread>
#include "myspectrometer.h"
#include "phidgetsMot.h"
#include "processso2.h"

class CalScan : public QThread
{
    Q_OBJECT
public:
    explicit CalScan(QObject *parent = 0);
    ~CalScan() ;
    void setObjects (MySpectrometer *myspec, phidgetsMot *mypm) ;
    void setModeString (QString *) ;
    void setWorkdir (QString) ;
    void setMode (int) ;
    void setFName (QString) ;
    void setTimeString (QString *) ;
    void setPSO2 (ProcessSO2 *ps) ;
    void fileCal (QString wdir);
    void autoDark () ;

    MySpectrometer *myspec ;
    
    phidgetsMot *mypm ;
    ProcessSO2 *ps ;


    int calMode ;
    double *dark, *low, *high, *ref, *scandat, *darkAtLevels ;
    QString *modestring, *timestring, workDir, filePref;
    QString darkFile, refFile, lowFile, highFile ;

protected:
    void run() ;
signals:
    void plotData (int, double*) ;
    void calFlag (int cflag) ;
    void satAlert (bool) ;
public slots:
    
};

#endif // CALSCAN_H
