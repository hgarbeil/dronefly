#include "autoint.h"
#include <QDebug>

int AutoInt::intlevels [] = {10, 25, 50, 100, 150, 200, 250, 333, 500, 750, 1000,  1500} ;
int AutoInt::nscansAvg [] = {50, 35, 20, 10, 6, 5, 4, 3,2,1,1,1} ;
int AutoInt::numLevels = 12 ;
AutoInt::AutoInt(QObject *parent) :
    QThread(parent)
{

    multi = .9 ;

    nintLevels = 12 ;
    scanAvgFlag = true ;



}

void AutoInt::setScanAvgFlag (bool b){
    scanAvgFlag = b ;
}

void AutoInt::setSpec (MySpectrometer *myspec){
    this->myspec = myspec ;
}

void AutoInt::setModeString (QString *s) {
    this->statstring = s ;
}

void AutoInt::run () {

    int i, iter, j, intLevel=0 ;
    bool foundhigh = false ;
    double val, dnlimit, maxval ;
    double *sdat = new double [myspec->spec_length] ;
    dnlimit = multi * myspec->maxDN ;
    QThread::msleep (200) ;

    for (i=0; i<nintLevels; i++){
        //myspec->setNScansAvg (nscansAvg[i]);
        qDebug () << "I is : "<< i ;
        myspec->setIntegrationTime (intlevels[i]) ;
        statstring->sprintf ("Auto Int at %d", intlevels[i]) ;
        maxval = -1.E9 ;
        for (iter=0; iter<2; iter++) {
              myspec->getScan (sdat) ;

              for (j=myspec->range0; j<myspec->range1h; j++)
              {
                    val =sdat [j] ;
                    if (val > dnlimit) foundhigh = true ;
                    if (val > maxval) {
                        maxval = val ;
                    }

               }
              //if (iter==1) emit (plotData (intlevels[i], sdat)) ;
              QThread::usleep(250000);

        }
        emit (setMax(maxval)) ;
        if (foundhigh) {
            intLevel = i-1 ;
            break ;
        }

       intLevel = i ;
    }

    myspec->setIntegrationTime (intlevels[intLevel]) ;
    myspec->setIntLevel (intLevel) ;
    if (scanAvgFlag)
        myspec->setNScansAvg(nscansAvg[intLevel]) ;
    else
        myspec->setNScansAvg(1) ;
    statstring->sprintf ("SPEC at %d msecs", intlevels[intLevel]) ;
    emit (complete(intLevel)) ;
    delete [] sdat ;
}
