#include "repeatscandialog.h"
#include "ui_repeatscandialog.h"
#include <QString>

RepeatScanDialog::RepeatScanDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RepeatScanDialog)
{
    ui->setupUi(this);
    startTime = 8. ;
    endTime = 15;
}

RepeatScanDialog::~RepeatScanDialog()
{
    delete ui;
}


void RepeatScanDialog::setTimes(float startTime, float stopTime){
    this->startTime = startTime ;
    this->endTime = stopTime ;
    QString str = QString ("%1").arg(startTime) ;
    ui->scanStartTimeLE->setText(str) ;
    QString str1 = QString ("%1").arg(stopTime) ;
    ui->scanStopTimeLE->setText(str1) ;
}

void RepeatScanDialog::on_okButton_accepted()
{
    startTime = ui->scanStartTimeLE->text().toFloat () ;
    endTime = ui->scanStopTimeLE->text().toFloat () ;
}
