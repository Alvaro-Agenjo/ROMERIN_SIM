#include "btserialport.h"
#include <QMutexLocker>
#include "RomerinMsg.h"

BTserialPort::BTserialPort(QObject *parent) :
    QObject(parent)
{
   // connect(&m_serialPort, &QSerialPort::readyRead, this, &BTserialPort::handleReadyRead);
    connect(&m_serialPort, &QSerialPort::errorOccurred, this, &BTserialPort::handleError);
    connect(&m_timer, &QTimer::timeout, this, &BTserialPort::handleTimeout);
    m_timer.start(50); //timeout

}
bool BTserialPort::open(QString portname)
{
    m_serialPort.setPortName(portname);
    m_serialPort.setBaudRate(QSerialPort::Baud115200);
    return m_serialPort.open(QIODevice::ReadWrite);
 }
void BTserialPort::close()
{

    m_serialPort.close();
}
void BTserialPort::handleReadyRead()
{
    QMutexLocker locker(&mutex);
    m_readData.append(m_serialPort.readAll());

}
//if success returns one message. Otherwise the message would be empty
bool BTserialPort::isOpen()
{
    return m_serialPort.isOpen();
}
void BTserialPort::sendMessage(QByteArray &m)
{
    QByteArray mens("\xaa\xaa\x00",3);
    mens[2]=m.size();
    mens.append(m);
    if(isOpen())m_serialPort.write(mens);
}
void BTserialPort::sendTorqueMessage(unsigned char motor_id,bool on)
{
    QByteArray mens(2,motor_id);
    mens[0]=(on?ROM_TORQUE_ON:ROM_TORQUE_OFF);
    sendMessage(mens);
}
void BTserialPort::sendSuctionCupMessage(unsigned char value)
{
    if(value>100)value=100;
    QByteArray mens(2,value);
    mens[0]=ROM_SUCTION_CUP;
    sendMessage(mens);
}
void BTserialPort::sendReboot(unsigned char motor_id)
{
    QByteArray mens(2,motor_id);
    mens[0]=ROM_REBOOT_MOTOR;
    sendMessage(mens);
}
void BTserialPort::sendGetFixedMotorInfo(unsigned char motor_id)
{
    QByteArray mens(2,motor_id);
    mens[0]=ROM_GET_FIXED_MOTOR_INFO;
    sendMessage(mens);
}
void BTserialPort::send1byteCommand(unsigned char comm)
{
    QByteArray mens(1,comm);
    sendMessage(mens);
}
void BTserialPort::sendGetName()
{
    send1byteCommand(ROM_GET_NAME);
}
void BTserialPort::sendScanWiFi()
{
    send1byteCommand(ROM_SCAN_WIFI);
}
void BTserialPort::sendInitWiFi()
{
    send1byteCommand(ROM_INIT_WIFI);
}
void BTserialPort::sendGetConfiguration()
{
    send1byteCommand(ROM_GET_CONFIG);
}
void BTserialPort::sendGoalAngle(unsigned char motor_id, float value)
{
    QByteArray mens(5,0);
    mens[0]=ROM_GOAL_ANG;
    mens[1]=motor_id;
    putFloatOnQByte(value, mens, 2);
    sendMessage(mens);
}
void BTserialPort::sendVelocityProfile(unsigned char motor_id, float value)
{
    QByteArray mens(5,0);
    mens[0]=ROM_VELOCITY_PROFILE;
    mens[1]=motor_id;
    putFloatOnQByte(value, mens, 2);
    sendMessage(mens);
}

#include "configdlg.h"
 void BTserialPort::sendConfiguration(ConfigurationInfo &conf)
 {
     QByteArray mens(13,0);
     mens[0]=ROM_CONFIG;
     int pos=1;
     for(int i=0;i<4;i++)mens[pos++]=conf.ip[i];
     for(int i=0;i<4;i++)mens[pos++]=conf.gateway[i];
     for(int i=0;i<4;i++)mens[pos++]=conf.mask[i];
     mens.append(conf.name); mens.append('\0');
     mens.append(conf.wifi); mens.append('\0');
     mens.append(conf.key); mens.append('\0');
     sendMessage(mens);
 }
QByteArray BTserialPort::getMessage()
{
    const QByteArray HEADER(ROM_HEADER,2);
    QByteArray mens;
    QMutexLocker locker(&mutex);

    int index=0;
    //mens = QByteArray("init bytes: ")+QByteArray::number(m_readData.size())+m_readData.left(50) ;
    //mens = m_readData.left(50);
    if((index=m_readData.indexOf(HEADER))>=0)m_readData.remove(0,index);
    int asize=m_readData.size();
    if((index<0)&&(asize>3)){//there was an error, remove everybyte except the last one
        m_readData.remove(0,asize-1);
        return mens;
    }
    if((index<0)||(asize<3))return mens;
    int size=int(m_readData[2]);
    if(size>asize-3)return mens; //not enough data

    m_readData.remove(0,3);
    mens = m_readData.left(size); //mens has only the message
    m_readData.remove(0,size);
    return mens;
}

void BTserialPort::handleTimeout()
{ //code executed if there are 500 without input bytes
 handleReadyRead();

}
void BTserialPort::handleError(QSerialPort::SerialPortError serialPortError)
{
    if (serialPortError == QSerialPort::ReadError) {

    }
}
