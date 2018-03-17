#include "repeatScan.h"
#include "autoint.h"
#include <QFile>
#include <QTextStream>
#include <QHostInfo>


extern float convertMSecsToFrac (int msecs) ;
repeatScan::repeatScan(QObject *parent) :
    QThread(parent)
{
    scandat = new double [2048+10] ;
    nscans = 250 ;
    scanning = false ;
    timeList.clear() ;
    ppmmList.clear() ;
    angleList.clear() ;
    scanFile="" ;
    gps = 0l ;
    startTime = 8. ;
    endTime = 15. ;
    nscansInsert = 10 ;
    workdir = "" ;
    strcpy (voltString,"") ;
    polarFlag = false ;
    autoLevelFlag = false ;
    hostname = QHostInfo::localHostName() ;
    mysocket = 0 ;
    serverString = "" ;
    cs = 0 ;


}

repeatScan::~repeatScan(){
    delete [] scandat ;
}

void repeatScan::setCalscan (CalScan *cs){
    this->cs = cs ;
}

void repeatScan::setMirrorFlag (bool b){
    mirrorFlag = b ;
}

void repeatScan::setWorkdir (QString str) {
    workdir = str ;
}

void repeatScan::setSocket (MySocket *m){
    mysocket = m ;
}

void repeatScan::setStartTime (float t0, float t1){
    startTime = t0 ;
    endTime = t1 ;
}

void repeatScan::setGPS (QGPSDevice *g){
    gps = g ;
}

void repeatScan::setScanFile (QString str){
    scanFile = str ;
}

void repeatScan::setModeString (QString *modes) {
    modestring = modes ;
}

void repeatScan::setObjects (MySpectrometer *myspec, phidgetsMot *mypm) {

    this->myspec = myspec ;
    this->mypm = mypm ;

}


void repeatScan::setPS (ProcessSO2 *p){
    pSO2 = p ;
}


void repeatScan::run () {

    bool satAlerted = false ;
    int scannum = 0, scaninsert=0 , curAngle = 0, inttime ;
    int satlo, sathi ;
    float val, mxval ;
    float *data = new float [2048] ;
    float fractime ;
    int oldHour = -1, oldDay = -1 ;
    scanning = true ;
    QString outstr ;
    QTime time ;
    QDate dat ;
    ppmmList.clear() ;
    timeList.clear() ;
    QString str = this->scanFile+".txt" ;
    QString filestring ;
    QFile *qf=0 ;
    QFile *qfbin =0  ;
    QTextStream *qts ;
    if (mirrorFlag) mypm->scanSet (mypm->startAng) ;
    str = this->scanFile+".bin" ;
    emit (satAlert(false)) ;
    time = QTime::currentTime () ;

    while  (scanning){
        dat = QDate::currentDate() ;
        time = QTime::currentTime () ;
        fractime = (float) time.hour() + time.minute() / 60. ;
        if (fractime < startTime || fractime > endTime){
            this->sleep (10) ;
            if (qf){
                if (qf->isOpen()) qf->close() ;
                if (qfbin->isOpen()) qfbin->close() ;
                qf=0 ;
                qfbin = 0 ;
            }
            continue ;

        }
        if (dat.day() != oldDay) {
            ppmmList.clear() ;
            timeList.clear() ;
            oldDay = dat.day() ;
            scannum = 0 ;
            // for a new day reset serverconnection
            emit (reconnect()) ;

        }
        if (oldHour != time.hour()) {
            // open a new file for writing text and binary
            QTime qtim = QTime::currentTime () ;
            QDate qdat = QDate::currentDate() ;
            filestring = workdir+"/"+ filestring.sprintf ("%04d_%02d_%02d_%02d%02d", qdat.year(), qdat.month(), qdat.day(), qtim.hour(), qtim.minute()) ;
            if (qf) qf->close() ;
            if (qfbin) qfbin->close() ;
            qf = new QFile (filestring+".txt") ;
            qf->open (QIODevice::Append | QIODevice::Text) ;
            qts = new QTextStream (qf) ;
            QString filebin  (filestring + ".bin") ;
            qfbin = new QFile (filebin) ;
            qfbin->open (QIODevice::Append ) ;
            oldHour = time.hour() ;
            cs->autoDark() ;

        }
        if (myspec->nscansAvg > 1)
            myspec->getScanAvg(scandat) ;
        else
            myspec->getScan (scandat) ;
        if (mirrorFlag)
            curAngle = mypm->curAng ;


        for (int i=0; i<2048; i++)data[i] =(float)scandat[i] ;
        qfbin->write ((char *) data, 2048 * 4) ;
        pSO2->setScanDark (&cs->darkAtLevels[2048L * myspec->intLevel]) ;
        inttime = AutoInt::intlevels[myspec->intLevel];


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
                    emit (intChange(1));

            }
            else if (mxval > .9 *myspec->maxDN) {
                emit ( intChange(-1) ) ;

            }
        }

        timeList.append (scannum) ;
        ppmmList.append (val) ;
        angleList.append (curAngle) ;
        if (mirrorFlag) modestring->sprintf ("SCAN : %d   ANGLE : %d   PPMM : %6.1f", scannum, curAngle, val) ;
        else modestring->sprintf ("SCAN : %d  PPMM : %6.1f", scannum, val) ;
        outstr.sprintf ("%d %04d %02d %02d %02d %02d %6.3f %s %s %d %d  %6.0f  %4d  %03d  %6.1f  %s\r\n",
                        scannum, dat.year(), dat.month(), dat.day(), time.hour(),
                        time.minute(), time.second()+time.msec()/1000.,
                        gps->timestring->toLatin1().data(),
                        gps->posstring->toLatin1().data(), satlo, sathi,
                        mxval, inttime, curAngle, val,  voltString) ;
        /*
         *
         *
         *
         *     serverString.sprintf ("%s : %d  %04d %02d %02d %02d %02d %6.3f %s %s %d %d  %6.0f  %4d  %03d  %6.1f  %s\r\n",
        this->hostname.toLatin1().data(), scannum, dat.year(), dat.month(), dat.day(), time.hour(),
        time.minute(), time.second()+time.msec()/1000.,
        gps->timestring->toLatin1().data(),
        gps->posstring->toLatin1().data(),satlo, sathi,
        mxval, inttime, curAngle, val,  voltString) ;
        */
        serverString.sprintf ("%s %d  %04d %02d %02d %02d %02d %6.3f %s %s %d %6.0f  %4d %6.0f  %6.1f  %s\r\n",
                              this->hostname.toLatin1().data(), scannum, dat.year(), dat.month(), dat.day(), time.hour(),
                              time.minute(), time.second()+time.msec()/1000.,
                              gps->timestring->toLatin1().data(),
                              gps->posstring->toLatin1().data(),satlo,
                              mxval, inttime, (float) curAngle, val,  voltString) ;



        *qts << outstr.toLatin1().data() ;
        emit (plotData(scannum, scandat)) ;
        if (mirrorFlag){

            mypm->scanIncrement() ;
        } else
        this->msleep (100) ;
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

    if (qf){
        if (qf->isOpen()) qf->close() ;
        if (qfbin->isOpen()) qfbin->close() ;
        qf=0 ;
        qfbin = 0 ;
    }
    delete [] data ;

}


void repeatScan::insertCell (int cellnum, int nscans){

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

void repeatScan::setVoltString (char *str) {

    strcpy (voltString, str);
}
