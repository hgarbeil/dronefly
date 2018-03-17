#ifndef REPEATSCANDIALOG_H
#define REPEATSCANDIALOG_H

#include <QDialog>

namespace Ui {
class RepeatScanDialog;
}

class RepeatScanDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit RepeatScanDialog(QWidget *parent = 0);
    void setTimes (float startTime, float stopTime) ;
    float startTime, endTime ;
    ~RepeatScanDialog();
    
private slots:
    void on_okButton_accepted();

private:
    Ui::RepeatScanDialog *ui;
};

#endif // REPEATSCANDIALOG_H
