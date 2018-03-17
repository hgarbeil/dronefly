#ifndef MYINTERFACEKIT_H
#define MYINTERFACEKIT_H

#include <QObject>
#include <phidget21.h>

class myInterfaceKit : public QObject
{
    Q_OBJECT
public:
    explicit myInterfaceKit(QObject *parent = 0);
    ~myInterfaceKit() ;
    void getResultsString (char *) ;
    void setVOnly (bool);
    void setLabel (QString *) ;
    CPhidgetInterfaceKitHandle ifKit ;
    QString *voltTempString, *ancilLabel, outlabel;
    int voltSensor, tempSensor, curSensor ;
    bool vOnlyFlag ;

signals:

public slots:
    void getReadings() ;
};

#endif // MYINTERFACEKIT_H
