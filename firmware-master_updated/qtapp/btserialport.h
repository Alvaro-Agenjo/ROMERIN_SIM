#ifndef BTSERIALPORT_H
#define BTSERIALPORT_H

#include <QByteArray>
#include <QSerialPort>
#include <QObject>
#include <QTimer>
#include <QMutex>
struct ConfigurationInfo;
class BTserialPort : public QObject
{
    Q_OBJECT
public:
    explicit BTserialPort( QObject *parent = nullptr);
    bool open(QString portname);
    bool isOpen();
    void close();
    QByteArray getMessage();
    void sendMessage(QByteArray &m);
    void sendTorqueMessage(unsigned char motor_id,bool on=true);
    void sendSuctionCupMessage(unsigned char value);
    void sendGoalAngle(unsigned char motor_id, float value);
    void sendReboot(unsigned char motor_id);
    void sendGetFixedMotorInfo(unsigned char motor_id);
    void sendGetName();
    void sendScanWiFi();
    void sendInitWiFi();
    void sendGetConfiguration();
    void sendConfiguration(ConfigurationInfo &conf);
    void send1byteCommand(unsigned char comm);
    void sendVelocityProfile(unsigned char motor_id, float value);
private slots:
    void handleReadyRead();
    void handleTimeout();
    void handleError(QSerialPort::SerialPortError error);


private:
    QSerialPort m_serialPort ;
    QByteArray m_readData;
    QTimer m_timer;
    QMutex mutex;


};

#endif // BTSERIALPORT_H
