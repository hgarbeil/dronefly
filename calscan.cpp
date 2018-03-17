
#include "calscan.h"
#include "autoint.h"
#include <QTime>
#include <QDebug>
#include <QFile>

CalScan::CalScan(QObject *parent) :
    QThread(parent)
{
    scandat = 0L ;
    calMode = 0 ;
    dark = new double  [2048] ;
    low = new double [2048] ;
    high = new double [2048] ;
    ref = new double [2048] ;
    scandat = new double [2048] ;

    workDir = "" ;
    filePref ="Cal" ;
    darkAtLevels = 0 ;
    darkAtLevels = new double [2048 * AutoInt::numLevels] ;
}



CalScan::~CalScan(){
    if (scandat) delete [] scandat ;
    if (darkAtLevels) delete[] darkAtLevels ;
    delete [] dark ;
    delete [] low ;
    delete [] high ;
    delete [] ref ;
}

void CalScan::setPSO2(ProcessSO2 *ps){
    this->ps = ps ;
}

void CalScan::setModeString (QString *modes) {
    modestring = modes ;
}


void CalScan::setWorkdir (QString workdir) {
    this->workDir = workdir ;
}

void CalScan::setObjects (MySpectrometer *myspec, phidgetsMot *mypm) {

    this->myspec = myspec ;
    this->mypm = mypm ;

}


void CalScan::setMode (int m) {
    // calMode ; 0:full, 1:dark, 2:low, 3:ref, 4:high
    calMode = m ;
}


void CalScan::autoDark () {

    //return ;
    int i, is, iscan;
    int orig_time, orig_nscans ;



    if (scandat) delete [] scandat ;
    scandat = new double [2048];
    myspec->getSpecSetup (&orig_time, &orig_nscans) ;


    mypm->setDark() ;


    for (i=0; i<AutoInt::numLevels; i++){
        myspec->setIntegrationTime (AutoInt::intlevels[i]);
        myspec->setNScansAvg (AutoInt::nscansAvg[i]) ;
        for (iscan=0; iscan<2; iscan++){

            modestring->sprintf ("Dark %d msec  scan : %d", AutoInt::intlevels[i], iscan) ;
            myspec->getScanAvg (scandat);
            emit (plotData(i, scandat)) ;
            //plotwidget->plotSpectrum (scandat, 2048) ;
            for (is=0; is<2048; is++) {
                if (iscan==0) darkAtLevels[i * 2048L +is] = scandat[is]/2. ;
                else  darkAtLevels[i * 2048L +is] += scandat[is] /2. ;
            }

        }
    }

    mypm->setRef() ;
    myspec->setIntegrationTime (orig_time) ;
    myspec->setNScansAvg (orig_nscans) ;

    delete [] scandat ;
    scandat = 0L ;
}

void CalScan::setTimeString (QString *str){
    timestring = new QString (*str) ;
}

void CalScan::run () {

    double *totalSpec ;

    int i, is, j, npts ;
    char str[240] ;
    float *data = new float [2048] ;
    //mypm->set
    npts = myspec->spec_length ;
    if (scandat) delete [] scandat ;
    scandat = new double [npts+4] ;

    QString fnamebase, fname, fname1 ;
    fnamebase="/home/pi/calfiles/cal";
    bool satAlerted = false ;
    float mxval ;
    totalSpec = dark ;
    //mypm->e() ;
    if (calMode != 0)
    {
    switch (calMode) {
        case 1:
            mypm->setDark () ;
            totalSpec = dark ;
            fname = fnamebase +"_dark.bin" ;
            darkFile = QString(fname) ;
            strcpy (str, "dark") ;
            emit (calFlag (1)) ;
            break ;
        case 2:
            mypm->setLow() ;
            totalSpec = low ;
            fname = fnamebase +"_low.bin" ;
            lowFile = QString(fname) ;
            strcpy (str, "low") ;
            emit (calFlag (2)) ;
            break ;
        case 3:
            mypm->setHigh() ;
            totalSpec = high ;
            fname = fnamebase +"_high.bin" ;
            highFile = QString(fname) ;
            strcpy (str, "high") ;
            emit (calFlag (4)) ;
            break ;
        case 4 :
            mypm->setRef () ;
            totalSpec = ref ;
            fname = fnamebase +"_ref.bin" ;
            refFile = QString(fname) ;
            strcpy (str, "ref") ;
            emit (calFlag (8)) ;
            break ;
    }


    QFile outfil(fname);
    outfil.open (QIODevice::WriteOnly) ;

    for (i=0; i<npts; i++) *(totalSpec+i) = 0. ;
    for (i=0; i< 10; i++){
        modestring->sprintf ("Calibrate %s : %d", str, i) ;
        myspec->getScanAvg (scandat) ;
        mxval = ps->calcMax(scandat);
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
        for ( is=0; is<2048; is++) data[is]=(float) scandat[is] ;
        outfil.write ((char *)data, npts*4) ;


        //emit (plotData(i, &scandat[0])) ;
        for (j=0; j<npts; j++) *(totalSpec+j) += scandat[j] / 10. ;
        //this->msleep(100) ;
    }
    outfil.close() ;

    } // end of single scan type - below is the full scan mode
    else {
    calMode = 4;
    while (calMode >0)
    {

        switch (calMode) {
            case 1:
                mypm->setDark () ;
                totalSpec = dark ;
                fname = fnamebase +"_dark.bin" ;
                fname1 = *timestring + "_dark.bin" ;
                qDebug() << fname1  ;
                darkFile = QString(fname) ;
                strcpy (str, "dark") ;
                //emit (calFlag (1)) ;
                break ;
            case 2:
                mypm->setLow() ;
                totalSpec = low ;
                fname = fnamebase +"_low.bin" ;
                fname1 = *timestring + "_low.bin" ;
                lowFile = QString(fname) ;
                strcpy (str, "low") ;
                //emit (calFlag (2)) ;
                break ;
            case 3:
                mypm->setHigh() ;
                totalSpec = high ;
                strcpy (str, "high") ;
                fname = fnamebase +"_high.bin" ;
                fname1 = *timestring + "_high.bin" ;
                highFile = QString(fname) ;
                emit (calFlag (4)) ;
                break ;
            case 4 :
                mypm->setRef () ;
                fname = fnamebase +"_ref.bin" ;
                fname1 = *timestring + "_ref.bin" ;
                strcpy (str, "ref") ;
                refFile = QString(fname) ;
                emit (calFlag (8)) ;
                totalSpec = ref ;
                break ;
        }
        QFile outfil (fname);
        QFile outfil1 (fname1) ;
        outfil.open (QIODevice::WriteOnly) ;
        outfil1.open (QIODevice::WriteOnly) ;
        for (i=0; i<npts; i++) *(totalSpec+i) = 0. ;
        for (i=0; i< 10; i++){
            modestring->sprintf ("Calibrate %s : %d", str, i) ;
            if (myspec->nscansAvg > 1)
                myspec->getScanAvg (scandat) ;
            else
                myspec->getScan (scandat) ;
            //qDebug () << " in loop " << scandat[300] ;

            mxval = ps->calcMax(scandat);
            if (mxval > .9 * myspec->maxDN) {
                satAlerted= true ;
            }
            else {
                if (satAlerted) {
                    satAlerted = false ;
                }
            }

            for (is=0; is<2048; is++) data[is]=(float) scandat[is] ;
            outfil.write ((char *)data, npts*4) ;
            outfil1.write ((char *)data, npts*4) ;
            //plotwidget->plotSpectrum (scandat, npts) ;
            ;

            emit (plotData(i, &scandat[0])) ;
            for (j=0; j<npts; j++) *(totalSpec+j) +=  scandat[j] / 10. ;
            this->msleep (100) ;
        }
        calMode-- ;


        outfil.close () ;
        outfil1.close() ;




    } // end of while
    emit (calFlag(1)) ;
    } // end of full scan else
    modestring->sprintf ("Calibration Complete") ;
    mypm->setRef() ;

    delete [] data ;
    delete [] scandat ;
    scandat=0L ;
    data=0L ;
}

void CalScan::setFName (QString pref){
    filePref = pref ;
}


void CalScan::fileCal (QString workdir) {

    int i, j ;
    float total ;
    float *tmpdata = new float [2048*10] ;

    QString str = workdir+"_dark.bin" ;


    if (str.length() < 2) return ;
    QFile *qf = new QFile(str) ;
    qf->open (QIODevice::ReadOnly) ;
    qf->read ((char *)tmpdata, 2048 * 4 *10) ;
    qf->close() ;

    for (j=0; j<2048; j++){
        total = 0. ;
        for (i=0; i<10; i++){
            total+= *(tmpdata+i*2048+j) / 10. ;
        }
        this->ps->dark[j] = total ;
    }
    str.replace ("dark","low") ;


    qf = new QFile(str) ;
        qf->open (QIODevice::ReadOnly) ;
        qf->read ((char *)tmpdata, 2048 * 4 *10) ;
        qf->close() ;

        for (j=0; j<2048; j++){
            total = 0. ;
            for (i=0; i<10; i++){
                    total+= *(tmpdata+i*2048+j) / 10. ;
            }
            this->ps->low[j] = total ;
        }
        str.replace ("low","high") ;

        qf = new QFile(str) ;
        qf->open (QIODevice::ReadOnly) ;
        qf->read ((char *)tmpdata, 2048 * 4 *10) ;
        qf->close() ;

        for (j=0; j<2048; j++){
            total = 0. ;
            for (i=0; i<10; i++){
                    total+= *(tmpdata+i*2048+j) / 10. ;
            }
            this->ps->high[j] = total ;
        }
        str.replace ("high","ref") ;

        qf = new QFile(str) ;
        qf->open (QIODevice::ReadOnly) ;
        qf->read ((char *)tmpdata, 2048 * 4 *10) ;
        qf->close() ;

        for (j=0; j<2048; j++){
            total = 0. ;
            for (i=0; i<10; i++){
                    total+= *(tmpdata+i*2048+j) / 10. ;
            }
            this->ps->ref[j] = total ;
        }

        delete [] tmpdata ;
}
