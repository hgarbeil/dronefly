#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "processso2.h"
#include "QThread"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    statusString = new QString ("Ready") ;
    ppmmString = new QString ("") ;
    myspec = new MySpectrometer () ;
    myspec->initSpec() ;
    myspec->setIntLevel (2) ;
    gps = new QGPSDevice (this) ;
    gps->setPort ("/dev/ttyUSB0") ;
    //gps->init() ;
    pm = new phidgetsMot () ;
    pso2 = new ProcessSO2 () ;

    cscan = new CalScan () ;
    cscan->setObjects(myspec, pm);
    cscan->setPSO2(pso2) ;
    cscan->setFName ("/home/pi/calfiles/cal") ;
    cscan->setModeString (statusString);

    pso2->setCals (cscan->dark, 0) ;
    pso2->setCals (cscan->low, 1) ;
    pso2->setCals (cscan->high, 2) ;
    pso2->setCals (cscan->ref, 3) ;

    scanthread = new ScanThread () ;
    scanthread->setObjects (myspec, pm) ;
    scanthread->setPS (pso2) ;
    scanthread->setCalscan (cscan) ;
    scanthread->setModeString (statusString) ;




    QThread::sleep (5) ;
    aint = new AutoInt() ;
    aint->setSpec (myspec) ;
    aint->setModeString (this->statusString) ;
    firstFlag = true ;
    calFlag = false ;
    connect (aint, SIGNAL(complete(int)), this, SLOT(autoDone(int))) ;
    connect (aint, SIGNAL(setMax(float)), this, SLOT(setValueLabel(float))) ;
    connect (scanthread, SIGNAL(setVal(float)), this, SLOT(setValueLabel(float))) ;


    qtimer = new QTimer (this) ;
    qtimer->start(1000) ;
    connect (qtimer, SIGNAL(timeout()), this, SLOT(updateGUI())) ;
    connect (pm, SIGNAL(setCell(int)), this, SLOT(setCellButton(int))) ;



    polarFlag = false ;
    voltOnlyFlag = true ;

    scanState = false ;


}


void MainWindow::setParamFile (char *pfile) {
       char gpsdev[240] ;
       float lowCell, highCell, scanoff, caloff, scanscale ;
       float val0, val1 ;
       int d0, d1, r0, r1, r0h, r1h, ival ;
       int cellSer, scanSer ;
       int dpos, lpos, hpos, rpos ;
       int nmotors ;

       FILE *fin = fopen (pfile, "r") ;

       if (fin==0L) {
            return ;
       }
       //paramFile = QString (ifile);
       // first line is low and high cell conctn
       fscanf (fin, "%f %f", &val0, &val1) ;
       lowCell = val0 ;
       highCell = val1 ;

       printf ("Low and high cell vals are : %f %f\r\n", lowCell, highCell) ;
       pso2->setConcentrations (lowCell, highCell) ;
       // second line is start and stop of dark range
       fscanf (fin, "%d %d", &d0, &d1) ;
       printf ("Dark range is %d %d\r\n", d0, d1) ;


        // third line is start and stop of fit range
        fscanf (fin, "%d %d", &r0, &r1) ;
        printf ("Fit range is %d %d\r\n", r0, r1) ;
        fscanf (fin, "%d %d", &r0h, &r1h) ;
        pso2->setRanges (d0, d1, r0, r1, r0h, r1h) ;
        myspec->setRanges ( r0, r1, r0h, r1h) ;
        printf ("Long fit range is %d %d\r\n", r0h, r1h) ;

        // fourth line is nmotors, caloff, scanoff, scanscale
        fscanf (fin, "%d %f %f %f", &nmotors, &caloff, &scanoff, &scanscale) ;
        pm->setMotors (nmotors, caloff, scanoff, scanscale) ;
        // fifth line is cell positions ref,high, dark, low
        fscanf (fin, "%d %d %d %d", &rpos, &hpos, &dpos, &lpos) ;

        this->pm->setCalPositions (dpos, lpos, hpos, rpos) ;
        // slope offset correction - not used in this program
        fscanf (fin, "%f %f", &val0, &val1) ;
        // the two motor serial numbers
        fscanf (fin, "%d %d",  &cellSer, &scanSer) ;

        // gps serial device name
        fscanf (fin, "%s",  gpsdev) ;
        // 0 or 1 flag for volts
        fscanf (fin, "%d",  &ival) ;

        fscanf (fin, "%f %f",  &startTime, &endTime) ;


        //gps->setPort (gpsdev) ;
        gps->init() ;

        /*if (gps->gpsStatus)
                gps->start() ;
         */
        pm->setSerialNumbers (cellSer, scanSer) ;
        pm->init() ;
        bool mFlag ;
        if (nmotors >1) {
                mFlag = true ;
                polarFlag = true ;
//                ui->scanAvgCBox->setChecked(false);
//                ui->polarRB->setChecked(true) ;
//                ui->ppmmWidget->setPolarScales() ;
//                ui->MirrorScanCB->setChecked(true) ;
//                ui->menuScanner->setEnabled (true);
                pm->scanHome();

            } else{
                mFlag = false ;
                polarFlag = false ;
//                ui->scanAvgCBox->setChecked(true);
//                ui->polarRB->setEnabled(false) ;
//                ui->MirrorScanCB->setCheckable(false) ;
//                ui->MirrorScanCB->setChecked(false) ;
//                ui->menuScanner->setEnabled (false);

            }
        scanthread->setMirrorFlag (mFlag) ;
        //repeatscan->setMirrorFlag (mFlag) ;


        ui->aintButton->setStyleSheet ("background-color:orange") ;
        aint->start() ;
}


void MainWindow::updateGUI () {

    ui->StatLE->setText (*statusString) ;
    if (calFlag == false) {
        ui->scanButton->setEnabled (false) ;
    }
    else ui->scanButton->setEnabled (true ) ;
    //ui->ppmmLE->setText (ppmmString->toLatin1().data()) ;
    ui->GPS_LE->setText (*gps->timestring) ;
    ui->GPSP_LE->setText (*gps->posstring) ;
}

void MainWindow::autoDone (int lev) {

    if (firstFlag) {
        ui->calButton->setStyleSheet ("background-color:green") ;
        cscan->fileCal("/home/pi/calfiles/cal") ;
        ui->scanButton->setEnabled (true) ;
        calFlag = true ;
        firstFlag = false ;
    }
    ui->aintButton->setStyleSheet ("background-color:green") ;
    //ui->lineEdit->setText (*statusString) ;
}

MainWindow::~MainWindow()
{
    delete ui;
    delete myspec ;
}

void MainWindow::on_aintButton_clicked()
{

    ui->aintButton->setStyleSheet ("background-color:orange") ;
    aint->start() ;
}

void MainWindow::on_scanButton_clicked()
{
    // if not scanning ....
    QString workDir ("/home/pi/data") ;
    if (!scanState){
        // start scanning
        QString filestring ;
        QTime qtim = QTime::currentTime () ;
        QDate qdat = QDate::currentDate() ;
        filestring = workDir+"/"+filestring.sprintf ("Scan_%04d_%02d_%02d_%02d%02d",
            qdat.year(), qdat.month(), qdat.day(), qtim.hour(), qtim.minute()) ;

        //this->writeParamFile (filestring+"_params.txt");
        pso2->processCals() ;
        ui->scanButton->setEnabled (false) ;



        scanthread->setScanFile (filestring) ;
        scanthread->nscans = 10000 ;
        myspec->setNScansAvg (AutoInt::nscansAvg[myspec->intLevel]) ;
        scanthread->setGPS (this->gps) ;
        //ui->plotWidget->detachCals() ;
        ui->scanButton->setStyleSheet ("background-color:green;color:yellow") ;
        ui->scanButton->setText ("Stop SCAN") ;
        pm->setRef() ;
        scanthread->start() ;
        scanState = true ;


    }
    else {
        scanthread->scanning = false ;
        scanState = false ;

        ui->scanButton->setStyleSheet ("background-color:yellow;color:black") ;
        ui->scanButton->setText ("SCAN") ;
    }
}

void MainWindow::on_calButton_clicked()
{

    QTime qtim = QTime::currentTime () ;
    QDate qdat = QDate::currentDate() ;
    timestring.sprintf ("/home/pi/calfiles/Cal_%04d_%02d_%02d_%02d%02d",
        qdat.year(), qdat.month(), qdat.day(), qtim.hour(), qtim.minute()) ;



    myspec->setNScansAvg (AutoInt::nscansAvg[myspec->intLevel]);
    //calFlag = 0 ;
    cscan->setFName ("/home/pi/calfiles/cal") ;
    cscan->setTimeString (&timestring) ;
    cscan->setMode (0) ;
    cscan->start() ;


}


void MainWindow::setCellButton (int bnum){
    switch (bnum) {
        case 0 :
            ui->cellButton->setText ("Dark") ;
            break ;
        case 1 :
            ui->cellButton->setText ("Low") ;
            break ;
        case 2 :
            ui->cellButton->setText ("High") ;
            break ;
        case 3 :
            ui->cellButton->setText ("Ref") ;
            break ;
    }
}

void MainWindow::setValueLabel (float val) {
    int ival = int(val) ;
    QString str = QString("%1").arg(ival) ;
    ui->valueLabel->setText(str) ;
}

void MainWindow::on_insLowButton_clicked()
{
    scanthread->insertCell (0,10) ;
}

void MainWindow::on_insHighButton_clicked()
{
    scanthread->insertCell (1,10) ;
}
