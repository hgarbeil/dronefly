#ifndef AUTOINT_H
#define AUTOINT_H

#include <QThread>
#include "myspectrometer.h"

class AutoInt : public QThread
{
    Q_OBJECT
public:
    explicit AutoInt(QObject *parent = 0);
    bool scanAvgFlag ;
    void setSpec (MySpectrometer *myspec) ;
    void setModeString (QString *) ;
    void setScanAvgFlag(bool) ;
    MySpectrometer *myspec ;
    static int intlevels[13] ;
    static int nscansAvg [13] ;
    static int numLevels ;
    float multi ;
    int nintLevels ;
    QString *statstring ;

    
protected:
    void run () ;

signals:
    void plotData(int, double *);
    void complete (int intlevel) ;
public slots:
    
};

#endif // AUTOINT_H
