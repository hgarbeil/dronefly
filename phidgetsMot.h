#ifndef MYPM_H
#define MYPM_H

#include <QObject>
#include <phidget21.h>
#include <stdio.h>
#include <unistd.h>
//#define CPhidgetAdvancedServoHandle PhidgetRCServoHandle

class phidgetsMot : public QObject {
    Q_OBJECT
	public :
    CPhidgetAdvancedServoHandle servo, servo1, servoCell, servoScan ;
    phidgetsMot (QObject *parent=0) ;
	~phidgetsMot () ;
    void setPosition (CPhidgetAdvancedServoHandle serv, int loc) ;
    void setCalPositions (int, int, int, int) ;
    void setScanScaleOff (float,float) ;
    void setMotors (int, float, float, float) ;
    void setSerialNumbers (int, int) ;
    void setMirrorAngles (int, int, int) ;
    void scanHome() ;
    void scanSet (int) ;
    void scanIncrement () ;
    void setDark () ;
    void setLow() ;
    void setHigh() ;
    void setRef() ;
    void init () ;
    bool firstTimeMirrorFlag ;
    int low, ref,  high, dark ;
    int startAng, endAng, incAng, curAng ;
    int cellSer, scanSer ;
    int nmotors, lastPos ;
    float scanscale, scanoff, caloff ;

signals :
    void cellState (int) ;
} ;



#endif
