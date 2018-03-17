#include "scanthread.h"
#include "autoint.h"
#include <QFile>
#include <QTextStream>
#include <QHostInfo>

ScanThread::ScanThread(QObject *parent) :
    QThread(parent)
{
    scandat = new double [2048] ;
    nscans = 250 ;
    scanning = false ;
    timeList.clear() ;
    ppmmList.clear() ;
    angleList.clear() ;
    scanFile="" ;
    gps = 0l ;
    nscansInsert = 10 ;
    mirrorFlag = false ;
    strcpy (voltString,"") ;
    polarFlag = false ;
    autoLevelFlag = false ;
    hostname = QHostInfo::localHostName() ;
    mysocket = 0 ;
    serverString = "" ;
    cs = 0 ;



}

ScanThread::~ScanThread(){
    delete [] scandat ;
}


void ScanThread::setCalscan (CalScan *cs){
    this->cs = cs ;
}

void ScanThread::setMirrorFlag (bool b){
    mirrorFlag = b ;
}


void ScanThread::setGPS (QGPSDevice *g){
    gps = g ;
}


void ScanThread::setSocket (MySocket *m){
    int status ;
    mysocket = m ;
    //mysocket->connectToHost () ;
    //status = mysocket->writeData ("Hello there") ;
}

void ScanThread::setScanFile (QString str){
    scanFile = str ;
}

void ScanThread::setModeString (QString *modes) {
    modestring = modes ;
}

void ScanThread::setObjects (MySpectrometer *myspec, phidgetsMot *mypm) {

    this->myspec = myspec ;
    this->mypm = mypm ;

}


void ScanThread::setPS (ProcessSO2 *p){
    pSO2 = p ;
}


void ScanThread::run () {

    bool satAlerted = false ;
    int scannum = 0, scaninsert=0, satlo, sathi, inttime;
    int npix = 2048, curAngle = 90 ;
    float val, mxval ;
    float *data = new float [npix] ;


    scanning = true ;
    QString outstr ;
    QTime time ;
    QDate dat ;
    ppmmList.clear() ;
    timeList.clear() ;
    angleList.clear() ;
    QString str = this->scanFile+".txt" ;

    QFile qf (str) ;
    str = this->scanFile+".bin" ;
    QFile qfbin (str) ;
    qf.open (QIODevice::WriteOnly | QIODevice::Text) ;
    qfbin.open (QIODevice::WriteOnly ) ;
    QTextStream qts (&qf) ;
    if (mirrorFlag) mypm->scanSet (mypm->startAng) ;
    emit (satAlert(false)) ;
    cs->autoDark() ;
    curAngle = 0 ;
    while (scannum <nscans && scanning){

        if (mirrorFlag)
        curAngle = mypm->curAng ;
        inttime = AutoInt::intlevels[myspec->intLevel];
        if (myspec->nscansAvg > 1)
            myspec->getScanAvg(scandat) ;
        else
            myspec->getScan (scandat) ;



        //plotwidget->plotSpectrum (scandat, npix) ;
        for (int i=0; i<npix; i++)data[i] =(float)scandat[i] ;
        qfbin.write ((char *) data, npix * 4) ;
        pSO2->setScanDark (&cs->darkAtLevels[2048 * myspec->intLevel]) ;

        mxval = pSO2->calcMax(scandat, &satlo, &sathi);
        val = pSO2->processScan (scandat, satlo, sathi) ;

        if (mxval > .9 * myspec->maxDN) {
            emit (satAlert(true)) ;
            satAlerted= true ;
        }
        else {
            if (satAlerted) {
                emit (satAlert (false)) ;
                satAlerted = false ;
            }
        }

        if (autoLevelFlag){
        if (mxval < .4 * myspec->maxDN) {
            // increase integration time
                emit((intChange(1))) ;


        }
        else if (mxval > .9 *myspec->maxDN) {

                emit(intChange(-1));

        }}

        dat = QDate::currentDate() ;
        time = QTime::currentTime () ;

        timeList.append (scannum) ;
        ppmmList.append (val) ;
        angleList.append (curAngle) ;
        if (mirrorFlag) modestring->sprintf ("SCAN : %d   ANGLE : %d   PPMM : %6.1f", scannum, curAngle, val) ;
        else modestring->sprintf ("SCAN : %d  PPMM : %6.1f", scannum, val) ;

        outstr.sprintf ("%d %04d %02d %02d %02d %02d %6.3f  %s %s %d %d  %6.0f  %4d  %03d  %6.1f  %s\r\n",
                        scannum, dat.year(), dat.month(), dat.day(), time.hour(),
                        time.minute(), time.second()+time.msec()/1000.,
                        gps->timestring->toLatin1().data(),
                        gps->posstring->toLatin1().data(),
                        satlo, sathi, mxval, inttime, curAngle, val,  voltString) ;
        serverString.sprintf ("%s  %d %04d %02d %02d %02d %02d %6.3f  %s %s %d %d %6.0f  %4d  %03d  %6.1f  %s\r\n",
                              this->hostname.toLatin1().data(), scannum, dat.year(), dat.month(), dat.day(), time.hour(),
                              time.minute(), time.second()+time.msec()/1000.,
                              gps->timestring->toLatin1().data(),
                              gps->posstring->toLatin1().data(),
                              satlo, sathi, mxval, inttime, curAngle, val,  voltString) ;


        qts << outstr.toLatin1().data() ;
        //if (!polarFlag)
        //emit (plotData(scannum, scandat)) ;
        //else
        //demit (plotData(scannum, curangle, scandat)) ;
        if (mirrorFlag){
            //curAngle = mypm->curAng ;
            mypm->scanIncrement() ;
        }
        else this->msleep (100) ;
        //ppmmPlotWidget->plotData(timeList, ppmmList) ;
        //plotwidget->plotSpectrum (scandat, npix) ;
        if (nscansInsert > 0) {
            scaninsert++ ;
            if (scaninsert >= nscansInsert) {
                scaninsert = 0 ;
                mypm->setRef() ;
                nscansInsert = 0 ;
            }
        }
        scannum++ ;
     }
    modestring->sprintf ("SCAN COMPLETE") ;
    qf.close() ;
    qfbin.close() ;
    delete [] data ;
    emit (scanComplete()) ;

}


void ScanThread::insertCell (int cellnum, int nscans){


    switch (cellnum) {
        case 0 :
            mypm->setLow() ;
            break ;
        case 1:
            mypm->setHigh() ;
            break ;
        default :
            mypm->setRef() ;
    }
    nscansInsert = nscans ;


}


void ScanThread::setVoltString (char *str) {

    strcpy (voltString, str) ;
}


