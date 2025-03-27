#ifndef BTSERIALPORT_H
#define BTSERIALPORT_H

#include <QByteArray>
#include <QSerialPort>
#include <QObject>
#include <QTimer>
#include <QMutex>
#include <list>
#include "../include/RomerinMessage.h"

class BTport : public QObject
{
    Q_OBJECT
public:
    explicit BTport( QObject *parent = nullptr);
    bool open(QString portname);
    bool isOpen();
    void close();
    RomerinMsg getMessage();
    bool thereIsMessage(){return(messages.size()>0);}
    void sendMessage(const RomerinMsg &m);
    QString getPortName(){return port_name;}
private slots:
    void handleReadyRead();
    void handleTimeout();
    void handleError(QSerialPort::SerialPortError error);


private:
    QSerialPort m_serialPort ;
    QString port_name;
    std::list<RomerinMsg> messages;
    MsgReader reader;
    QTimer m_timer;
    QMutex mutex;
};

#endif // BTSERIALPORT_H
