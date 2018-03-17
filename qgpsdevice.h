#ifndef QGPSDEVICE_H
#define QGPSDEVICE_H

#include <QObject>
#include <QtSerialPort/QSerialPort>

class QGPSDevice : public QObject
{
    Q_OBJECT
public:
    explicit QGPSDevice(QObject *parent = 0);
    ~QGPSDevice() ;
    void init () ;
    void setPort (QString) ;
    void parseGPS (const char *) ;
    void parseTime (float) ;

    QSerialPort *qsport ;
    QByteArray *indata ;
    QString outString ;
    QString *timestring, *posstring ;
    bool gpsStatus ;

    bool clearNext, fixFlag ;
    float latVal, lonVal, gpsTime, altVal ;
    int  fix,  nSats, hr, min, sec;
    char latCh [80], lonCh[80] ;

signals:
    void sendString(char*) ;
public slots:
    void readData() ;
};

#endif // QGPSDEVICE_H
