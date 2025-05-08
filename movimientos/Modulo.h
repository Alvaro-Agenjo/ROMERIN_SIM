#include <string>
#include <QUdpSocket>

#include "RomMotorInfoA.h"

struct Modulo {
    QHostAddress ip;
    std::string name;
    MotorInfo motor[6];
    SuctionCupInfo suction_cup;
};