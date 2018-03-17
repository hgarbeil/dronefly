#ifndef MYSPECTROMETER_H
#define MYSPECTROMETER_H

#include <QObject>
#define MYSPEC_READY    0
#define MYSPEC_CAL_DARK 1
#define MYSPEC_CAL_LOW  2
#define MYSPEC_CAL_HIGH 3
#define MYSPEC_CAL_REF  4
#define MYSPEC_CAL_SCAN 5


class MySpectrometer : public QObject
{
    Q_OBJECT
public:
    explicit MySpectrometer(QObject *parent = 0);
    ~MySpectrometer () ;
    void setIntegrationTime (int time_msec) ;
    int getSpecSetup (int *msec, int *nscan_avg) ;
    void setRanges (int, int, int, int) ;
    void setNScansAvg (int nScans) ;
    void initSpec();
    void getScan (double *) ;
    void getScanAvg (double *) ;
    void setIntLevel (int) ;
    int spec_length ;
    int spectrometerMode ;
    double *outspec ;
    int range0, range1, range0h, range1h ;
    int maxDN, nscansAvg, intTime ;
    int intLevel ;
    bool flagPlus ;
    
signals:
    
public slots:
    
};

#endif // MYSPECTROMETER_H
