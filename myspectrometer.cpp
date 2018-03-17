#include <stdio.h>
#include "myspectrometer.h"
#include <api/SeaBreezeWrapper.h>


MySpectrometer::MySpectrometer(QObject *parent) :
    QObject(parent)
{
    spec_length = 2048 ;



    flagPlus = true ;
    maxDN = 65535 ;
    initSpec() ;
    nscansAvg= 20 ;
    intTime = 50 ;
    intLevel = 2 ;

}

MySpectrometer::~MySpectrometer(){
    int error, flag ;
    seabreeze_close_spectrometer (0, &error) ;

}


void MySpectrometer::setRanges (int r0, int r1, int r0h, int r1h){
    range0h = r0h ;
    range1h = r1h ;
    range0 = r0 ;
    range1 = r1 ;
}

int MySpectrometer::getSpecSetup (int *msec, int *nscan_avg){
    *msec = intTime ;
    *nscan_avg = nscansAvg ;
    return intTime ;
}



void MySpectrometer::initSpec (){
    int i, device_count=0, devnum ;
    int error,flag  ;
    char msg [80] ;
    printf ("Opening Spectrometers") ;
    for(i = 0; i < 1; i++) {
           flag = seabreeze_open_spectrometer(i, &error);
           seabreeze_get_error_string(error,msg, sizeof(msg) ) ;
           printf("Result is (%d) [%s]\n", flag, msg );
           if(0 == flag) {
               device_count++;
               devnum = i ;
           } else {
               break ;
           }
    }

    // set the integration time
    seabreeze_set_integration_time_microsec(0, &error, 500000);
    seabreeze_get_model (0, &error, msg,  sizeof(msg)) ;
    if (strstr(msg,"PLUS")== NULL){
        flagPlus = false ;
        this->maxDN = 4095 ;
    }


    seabreeze_get_error_string(error, msg, sizeof(msg)) ;
    printf ("Setting integration time") ;
    printf("Result is [%s]\n",msg );

    // get spectrum length
    spec_length = seabreeze_get_formatted_spectrum_length(0, &error);
    printf ("Getting spec lenght") ;
    seabreeze_get_error_string(error, msg, sizeof(msg)) ;
    printf("Result is (%d) [%s]\n", spec_length, msg);
}


void MySpectrometer::setIntLevel (int l){
    intLevel = l ;
}

void MySpectrometer::setIntegrationTime (int time_msec){
    int error ;

    intTime = time_msec ;
    seabreeze_set_integration_time_microsec(0, &error, time_msec*1000);
    //printf ("Setting integration time") ;
    //sprintf(errstr, "Result is [%s]\n", seabreeze_get_error_string(error));
    //qDebug ()<< errstr ;
}

void MySpectrometer::getScan (double *outdata) {

    int  error ;
    seabreeze_get_formatted_spectrum(0, &error, outdata, spec_length);
    //printf("Result is (%d) [%s]\n", flag, seabreeze_get_error_string(error));


}

void MySpectrometer::getScanAvg (double *outdat){
    int i, ii ;
    double  *tmp ;
    tmp = new double [spec_length] ;
    for (i=0; i<2048; i++) outdat[i]=0. ;
    for (i=0; i<nscansAvg ; i++){
        getScan(tmp) ;
        for (ii=0; ii<2048; ii++) outdat[ii] += (tmp[ii]/nscansAvg) ;
    }
    delete [] tmp ;
}

void MySpectrometer::setNScansAvg (int n) {

    nscansAvg = n ;
}
