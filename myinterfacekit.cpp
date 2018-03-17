#include "myinterfacekit.h"
#include <QTimer>
#include"stdio.h"

myInterfaceKit::myInterfaceKit(QObject *parent) : QObject(parent)
{
    ifKit = 0 ;
    const char *err ;
    CPhidgetInterfaceKit_create (&ifKit) ;
    if (ifKit == 0)
    {
        return ;

    }


    int result ;
    CPhidget_open ((CPhidgetHandle) ifKit, -1) ;

    if((result = CPhidget_waitForAttachment((CPhidgetHandle)ifKit, 10000)))
    {
            CPhidget_getErrorDescription(result, &err);
            printf("Problem waiting for attachment: %s\n", err);
            return;
    }
    curSensor = 4 ;
    voltSensor = 6 ;
    tempSensor = 7 ;
    voltTempString = new QString("") ;
    outlabel = "" ;
    vOnlyFlag = true ;

    //connect (timer, SIGNAL(timeout()), this, SLOT(getReadings())) ;
    //timer->start(3000) ;
}

void myInterfaceKit::setVOnly (bool b){
    vOnlyFlag = b ;
}


void myInterfaceKit::setLabel (QString *lab){
    ancilLabel = lab ;
}

myInterfaceKit::~myInterfaceKit (){

    if (ifKit){
        CPhidget_close ((CPhidgetHandle)ifKit) ;
        CPhidget_delete ((CPhidgetHandle)ifKit) ;
    }
}


void myInterfaceKit::getReadings (){

    float volts, temperature, current ;
    int valVolt, valTemp, valCurrent ;
    CPhidgetInterfaceKit_setRatiometric (ifKit, 0);
    CPhidgetInterfaceKit_getSensorRawValue (ifKit, 5, &valVolt );
    volts = valVolt / 4.095f /13.62 - 36.7107 ;
    
    CPhidgetInterfaceKit_setRatiometric (ifKit,1);
    CPhidgetInterfaceKit_getSensorRawValue (ifKit, 6, &valTemp) ;
    temperature = valTemp / 4.095 * .2222 - 61.111 ;
    CPhidgetInterfaceKit_getSensorRawValue (ifKit, 4, &valCurrent );
    current = valCurrent / 4.095f /13.2 - 37.8787 ;
    //qDebug () << "Volts : " << volts<<" Temp : "<< temperature << " \r\n" ;
    if (!vOnlyFlag)
        voltTempString->sprintf ("%6.1f %5.2f %5.2f", temperature, volts, current) ;
        //voltTempString->sprintf ("%6.1f degC  %5.2f Amps", volts, temperature, current) ;
    else
        voltTempString->sprintf ("%5.2f", volts) ;


}


void myInterfaceKit::getResultsString(char *rstring) {

    float volts, temperature, current ;
    int valVolt, valTemp, valCurrent ;
    CPhidgetInterfaceKit_setRatiometric (ifKit,0);
    CPhidgetInterfaceKit_getSensorRawValue (ifKit, 5, &valVolt );
    volts = valVolt / 4.095f /13.62 - 36.7107 ;
    CPhidgetInterfaceKit_setRatiometric (ifKit,1);
    CPhidgetInterfaceKit_getSensorRawValue (ifKit, 6, &valTemp) ;
    temperature = valTemp / 4.095 * .2222 - 61.111 ;
    CPhidgetInterfaceKit_getSensorRawValue (ifKit, 4, &valCurrent );
    current = valCurrent / 4.095f /13.2 - 37.8787 ;
    //qDebug () << "Volts : " << volts<<" Temp : "<< temperature << " \r\n" ;
    //ancilLabel->sprintf ("V: %5.2f  T: %6.1f", volts, temperature) ;

    //voltTempString->sprintf ("%5.2f V  %6.1f degC  %5.2f Amps", volts, temperature, current) ;

    //sprintf (rstring, "%5.2f %6.1f  %5.2f", volts, temperature, current) ;
    if (!vOnlyFlag){
        outlabel = QString ("%1 V   %2 degC   %3 Amps").arg(volts,5, 'f',2).arg(temperature,5,'f',1).arg(current,5,'f',2) ;
        sprintf (rstring, "%5.2f %6.1f  %5.2f", volts, temperature, current) ;
    }
    else {
        outlabel = QString ("%1 V").arg(volts,5, 'f',2) ;
        sprintf (rstring, "%5.2f", volts) ;
    }
    *ancilLabel = outlabel ;

}
