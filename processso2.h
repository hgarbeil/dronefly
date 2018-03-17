#ifndef PROCESSSO2_H
#define PROCESSSO2_H

#include <QObject>

class ProcessSO2 : public QObject
{
    Q_OBJECT
public:
    explicit ProcessSO2(QObject *parent = 0);
    int  nptsSpec, dark0, dark1, range0, range1, range0h, range1h ;
    void setRanges (int d0, int d1, int r0, int r1, int r0h, int r1h) ;
    void setScanDark (double *) ;
    void setSatDN (int) ;
    void setConcentrations (float lowCell, float highCell) ;
    float lowCell, highCell ;
    double *dark, *low, *high, *ref, *darkScan ;
    double *lowd, *highd, *refd, *scand ;
    double *abs_low, *abs_high, *abs_low3D, *abs_high3D ;
    int satDN ;
    void strayCorr(double*) ;
    void remove3D (double *, double *) ;
    void calSubtract() ;
    void setCals (double *arr, int mode) ;
    void calcAbsorb (double *, double  *) ;
    float processScan (double *) ;
    float processScan (double *, int, int) ;
    void darkSubtract (double*, double *) ;
    float fitppmm (float, float, float) ;
    float calcMax (double *) ;
    float calcMax (double *val, int *satLo, int *satHi) ;
    void processCals () ;
signals:
    
public slots:
    
};

#endif // PROCESSSO2_H
