#include "mysocket.h"
#include "unistd.h"
//static inline QByteArray IntToArray(qint32 source);

MySocket::MySocket(QObject *parent) :
    QObject(parent)
{
    socket = new QTcpSocket (this) ;
    portNumber = 1024 ;
    serverString = "" ;
}


void MySocket::setServerString (QString s) {
    serverString =s ;
}

void MySocket::setPortNumber (int n){
    portNumber = n ;
}


bool MySocket::connectToHost (QString host){

    socket->connectToHost (host, portNumber) ;
    if (!socket->waitForConnected(5000)){
       // qDebug() << "Error " << socket->errorString() ;
    }

    return true ;
}


bool MySocket::connectToHost (){

    socket->connectToHost (serverString, portNumber) ;
    if (!socket->waitForConnected(5000)){
       // qDebug() << "Error " << socket->errorString() ;
    }

    return true ;
}


bool MySocket::reconnect (){
    socket->disconnect() ;
    qDebug() << "Disconnecting server  "  ;
    sleep (5) ;
    socket->connectToHost (serverString, portNumber) ;
    if (!socket->waitForConnected(5000)){
       // qDebug() << "Error " << socket->errorString() ;
        return false ;
    }
    qDebug() << "Server reconnected ? "  ;
    return true ;
}


bool MySocket::writeData(QByteArray data)
{

    //int status ;
    //status = socket->state() ;
    //socket->write(data) ;
    if(socket->state() == QAbstractSocket::ConnectedState)
        socket->write (data) ;
    /*
     *
     *
     *
     *
    socket->write ("WAYYYYYY HELLOW    ") ;

    if(socket->state() == QAbstractSocket::ConnectedState)
    {
        socket->write(IntToArray(data.size())); //write size of data
        socket->write(data); //write the data itself
        return socket->waitForBytesWritten();
    }
    else
        return false;
        */
    return (true) ;
}

void MySocket::closeUp () {
    socket->close () ;
}
/*
QByteArray IntToArray(qint32 source) //Use qint32 to ensure that the number have 4 bytes
{
    //Avoid use of cast, this is the Qt way to serialize objects
    QByteArray temp;
    QDataStream data(&temp, QIODevice::ReadWrite);
    data << source;
    return temp;
}
*/
