#ifndef MYSOCKET_H
#define MYSOCKET_H

#include <QObject>
#include <QTcpSocket>

class MySocket : public QObject
{
    Q_OBJECT
public:
    explicit MySocket(QObject *parent = 0);
    bool connectToHost(QString host);
    bool connectToHost () ;
    bool reconnect() ;
    void setServerString (QString) ;
    void setPortNumber (int ) ;
    void closeUp () ;
    QTcpSocket *socket ;
    bool writeData(QByteArray data) ;
    QString serverString ;
    int portNumber ;

signals:
    
public slots:
    
};

#endif // MYSOCKET_H
