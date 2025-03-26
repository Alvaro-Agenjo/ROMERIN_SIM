#include "btport.h"
#include <QMutexLocker>
#include "../include/RomerinMessage.h"
#include <QDebug>
BTport::BTport(QObject *parent) :
    QObject(parent)
{
   // connect(&m_serialPort, &QSerialPort::readyRead, this, &BTserialPort::handleReadyRead);
    connect(&m_serialPort, &QSerialPort::errorOccurred, this, &BTport::handleError);
    connect(&m_timer, &QTimer::timeout, this, &BTport::handleTimeout);
    m_timer.start(50); //timeout

}
bool BTport::open(QString portname)
{
    port_name=portname;
    m_serialPort.setPortName(port_name);
    m_serialPort.setBaudRate(QSerialPort::Baud115200);
    return m_serialPort.open(QIODevice::ReadWrite);
 }
void BTport::close()
{

    m_serialPort.close();
}
void BTport::handleReadyRead()
{
    QMutexLocker locker(&mutex);
    QByteArray data = m_serialPort.readAll();
    for (int i = 0; i < data.size(); ++i) {
        if(reader.add_uchar(data[i])){
                 messages.push_back(reader.getMessage());
        }
    }
 }
//if success returns one message. Otherwise the message would be empty
bool BTport::isOpen()
{
    return m_serialPort.isOpen();
}
void BTport::sendMessage(const RomerinMsg &m)
{
    QByteArray mens((const char *)(m.data),m.size+3);
    if(isOpen())m_serialPort.write(mens);
}
RomerinMsg BTport::getMessage()
{
     QMutexLocker locker(&mutex);
     if(messages.size()>0){
         RomerinMsg ret=messages.front();
         messages.pop_front();
         return ret;
     }
     return RomerinMsg::none();
}
void BTport::handleTimeout()
{ //code executed if there are 500 without input bytes
 handleReadyRead();

}
void BTport::handleError(QSerialPort::SerialPortError serialPortError)
{
    if (serialPortError == QSerialPort::ReadError) {
        qDebug()<<"Algo malo ha pasado";
    }
}
