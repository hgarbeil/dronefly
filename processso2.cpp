#include "processso2.h"
#include <math.h>
#include <QFile>

void polyfit (double *x, double *y, int npts, int degree, double *coefs) ;
void polyfit (double *x, double *y, int npts, int degree, double *coefs, double *yf) ;

ProcessSO2::ProcessSO2(QObject *parent) :
    QObject(parent)
{
    nptsSpec = 2048 ;
    dark0 = 318 ;
    dark1 = 414 ;
    range0 = 539 ;
    range1 = 664 ;
    range0h = 722 ;
    range1h = 857 ;

    lowd = new double [nptsSpec] ;
    highd = new double [nptsSpec] ;
    refd = new double [nptsSpec] ;
    abs_low = new double [nptsSpec] ;
    abs_high = new double [nptsSpec] ;
    abs_low3D = new double [nptsSpec] ;
    abs_high3D = new double [nptsSpec] ;
    darkScan = 0 ;

    satDN = 65535 * 0.98 ;
}


void ProcessSO2::setRanges (int d0, int d1, int r0, int r1, int r0h, int r1h){
    dark0 = d0 ;
    dark1 = d1 ;
    range0 = r0 ;
    range1 = r1 ;
    range0h = r0h ;
    range1h = r1h ;


}



void ProcessSO2::setConcentrations(float l, float h){
    lowCell = l ;
    highCell = h ;
}

void ProcessSO2::setScanDark (double *arr){
    darkScan = arr ;
}

void ProcessSO2::setCals (double *arr, int mode){

    switch (mode) {
    case 0 :
        dark = arr ;
        break ;
    case 1 :
        low = arr ;
        break ;
    case 2 :
        high = arr ;
        break ;
    case 3 :
        ref = arr ;
        break ;
    }


}

void ProcessSO2::setSatDN (int val){
    satDN = val ;
}

void ProcessSO2::remove3D (double *inarr, double *outarr) {
    int i, npts ;
    double *tmp0, *tmp1, *xind ;
    double *coefs = new double[4] ;

    npts = range1 - range0 + 1 ;
    tmp0 = new double [npts] ;
    xind = new double [npts] ;

    for (i=0; i<npts; i++) xind[i] = i ;
    for (i=0; i<this->nptsSpec; i++) *(outarr+i) = *(inarr+i) ;
    polyfit (xind, &inarr[range0], npts, 3, coefs, tmp0) ;
    for (i=0; i<npts; i++){
        *(outarr+i+range0)-= *(tmp0+i) ;
    }

    delete []tmp0 ;
    delete []xind ;

    npts = range1h - range0h + 1 ;
    tmp0 = new double [npts] ;
    xind = new double [npts] ;
    for (i=0; i<npts; i++) xind[i] = i ;
    polyfit (xind, &inarr[range0h], npts, 3, coefs, tmp0) ;
    for (i=0; i<npts; i++){
        *(outarr+i+range0h)-= *(tmp0+i) ;
    }

    delete [] coefs ;
    delete [] tmp0 ;
    delete [] xind ;


}

void ProcessSO2::darkSubtract (double *in, double *out) {
    for (int i=0; i<nptsSpec; i++){
            *(out+i)=*(in+i)-(*(darkScan+i)) ;
        // *(out+i)=*(in+i)-(*(dark+i)) ;
    }
}


void ProcessSO2::calSubtract() {
    // for debug
    /*
    QFile qf ("/home/fly/cals_bin") ;
    qf.open (QIODevice::WriteOnly) ;
    qf.write ((char *)dark, 2048 * 8) ;
    qf.write ((char *)low, 2048 * 8) ;
    qf.write ((char *)ref, 2048 * 8) ;
    qf.write ((char *)high, 2048 * 8) ;
    qf.close() ;
    */
    for (int i=0; i<nptsSpec; i++){
        *(lowd+i)=*(low+i)-*(dark+i) ;
        *(highd+i)=*(high+i)-*(dark+i) ;
        *(refd+i)=*(ref+i)-*(dark+i) ;
    }
}


void ProcessSO2::strayCorr (double *arr) {
    int i, count = 0 ;
    double total = 0. ;
    for (i=dark0; i<=dark1; i++){
        total+= *(arr+i) ;
        count++ ;
    }
    total /= count ;

    for (i=0; i<nptsSpec; i++){
        *(arr+i) -= total ;
    }
}

void ProcessSO2::calcAbsorb (double *arrin,  double *arrout){

    int i ;
    double val ;

    for (i=0; i<nptsSpec; i++){
        val = *(arrin+i) / *(refd+i);
        if (val<0)
            *(arrout+i) = 0. ;
        else
            *(arrout+i) = -log10(val) ;

    }
}


float ProcessSO2::calcMax (double *arrin){

    int i ;
    float val, maxval = -1;
    for (i=range0; i<range1h; i++){
        val = arrin[i] ;
        if (val > maxval) maxval = val ;
    }
    return maxval ;
}


float ProcessSO2::calcMax (double *arrin, int *nsatLo, int *nsatHi){

    int i ;
    float val, maxval = -1;
    bool losatFlag = false ;
    *nsatLo = 0 ;
    *nsatHi = 0 ;
    for (i=range0; i<range1h; i++){
        val = arrin[i] ;
        if (val > maxval) maxval = val ;
    }
    if (maxval >= satDN){



        for (i=range0; i<range1; i++){
            val = arrin[i] ;
            if (val >= satDN) {
                *nsatLo = range1 - i -1 ;
                *nsatHi = range1h - range0h + 1 ;
                losatFlag = true ;
                break ;
            }
        }
        if (!losatFlag)
        for (i=range0h; i<range1h; i++){
            val = arrin[i] ;
            if (val >= satDN) {
                *nsatHi = range1h - i -1 ;


                break ;
            }
        }


    }
    return maxval ;
}



void ProcessSO2::processCals() {

    calSubtract () ;
    strayCorr(lowd) ;
    strayCorr(highd) ;
    strayCorr(refd) ;
    calcAbsorb(lowd, abs_low) ;
    calcAbsorb(highd, abs_high) ;
    remove3D (abs_low, abs_low3D) ;
    remove3D (abs_high, abs_high3D) ;
/*
    QFile qfa ("lowd") ;
    qfa.open (QIODevice::WriteOnly) ;
    qfa.write ((char *)lowd, nptsSpec * 8) ;
    qfa.close () ;
    QFile qfb ("refd") ;
    qfb.open (QIODevice::WriteOnly) ;
    qfb.write ((char *)refd, nptsSpec * 8) ;
    qfb.close () ;
    QFile qf ("abslow") ;
    qf.open (QIODevice::WriteOnly) ;
    qf.write ((char *)abs_low, nptsSpec * 8) ;
    qf.close () ;
    QFile qfh ("abslow3D") ;
    qfh.open (QIODevice::WriteOnly) ;
    qfh.write ((char *)abs_low3D, nptsSpec * 8) ;
    qfh.close () ;
*/
}


float  ProcessSO2::processScan (double *indat, int satLo, int  satHi) {
    int npts_fit ;
    double *indatd = new double [nptsSpec] ;
    double *abs_scan = new double [nptsSpec] ;
    double *abs_scan3D = new double [nptsSpec] ;
    double lowcoef[2], highcoef[2], highcoef_long[2] ;
    float ppmm_low, ppmm_high, ppmm_high_H, ppmmval ;
    bool satflagLo = false ;


    darkSubtract (indat, indatd) ;
    strayCorr (indatd) ;
    calcAbsorb (indatd, abs_scan) ;
    remove3D (abs_scan, abs_scan3D) ;

    npts_fit = range1 -range0 + 1 - satLo ;
    // do the low and high conctn fits

    if (npts_fit > 4 ){
    polyfit (abs_low3D+range0, abs_scan3D+range0, npts_fit, 1, lowcoef ) ;
    polyfit (abs_high3D+range0, abs_scan3D+range0, npts_fit, 1, highcoef ) ;
    npts_fit = range1h -range0h + 1 - satHi ;
    } else {
        satflagLo = true ;
    }

    if (npts_fit > 4 && satLo==0){
        polyfit (abs_high3D+range0h, abs_scan3D+range0h, npts_fit, 1, highcoef_long ) ;
    } else {
        highcoef_long[1] = 0. ;
    }
    if (!isnan(lowcoef[1]) && !satflagLo)
    {

    ppmm_low = lowcoef [1] * lowCell ;
    ppmm_high = highcoef [1] * highCell ;
    ppmm_high_H = highcoef_long[1] * highCell ;

    ppmmval = fitppmm (ppmm_low, ppmm_high, ppmm_high_H) ;
    }
    else ppmmval = -999. ;

    delete [] indatd ;
    delete [] abs_scan ;
    delete [] abs_scan3D ;
    return ppmmval ;
}


/// Routine to return ppmm based on input ppmm values
/// from the 2 wavelength ranges
float ProcessSO2::fitppmm (float low, float high, float highH) {
    float den, wlow, whigh, ppmmval ;
    float highCell2 = highCell * 1.5 ;

    ppmmval = -999. ;
    if (low <= lowCell) return low ;
    /*moved up from below */
    if (highH > highCell2){
        return highH ;

    }
    if (high <= highCell && highH <= highCell) {
        den = (highCell- low) + (high-lowCell) ;
        wlow = (highCell-low) / den ;
        whigh = (high-lowCell) / den ;
        ppmmval = low * wlow + high * whigh ;
        return ppmmval ;
      }
    if (high <= highCell && highH > highCell && highH <= highCell2){
        den= (highCell2 - high) + (highH - highCell) ;
        wlow = (highCell2 - high) / den ;
        whigh = (highH - highCell) / den ;
        ppmmval = wlow * high + whigh * highH ;
        return ppmmval ;
    }
    // 14 & 17
    /* Move this up to condition 2
    if (highH > highCell2){
        return highH ;

    }
    */
    if (high > highCell && highH < highCell) {
        return high ;

    }
    if (high > highCell && highH > highCell)
    {
        den =  (highCell2 - high) + (highH - highCell) ;
        wlow = (highCell2 - high)  / den ;
        whigh = (highH - highCell) / den ;
        ppmmval = wlow * high + whigh * highH ;
        return ppmmval ;
    }

return ppmmval ;

}
