#ifndef MODEL_H
#define MODEL_H

#include <QHostAddress>
#include <QString>
#include <QUdpSocket>
#include <QFile>
#include <list>

#include "../include/RomerinMessage.h"
#include "configdlg.h"


class Module;
struct ConfigurationInfo;
struct ConfigurationInfoV2;
class ModuleController
{
public:
    QFile * _file = nullptr;

    QString name;
    bool simulated=false;
    uint8_t virtualid=0;
    QHostAddress ip;
    static QUdpSocket * ip_port;

    Module *mod;
    uchar_t robot_state;
    uchar_t robot_cicle_time;
    uchar_t wifi_watch_dog=0;
    bool conected=false;
    std::list<RomerinMsg> ip_msgs;
    ModuleController():name(""),ip(), mod(0), wifi_watch_dog(0){}

    RomerinMsg executeMessage(const RomerinMsg &m );
    void sendMessage(const RomerinMsg &m);
    void reset_wifi_watchdog(){wifi_watch_dog=0;}
    bool isConnected();
    void sendConfig(ConfigurationInfo &info);
    void sendConfigV2(ConfigurationInfoV2 &info);
    void loop();
    void getConfig();
    void save_data();
    void setFile(QFile *file){_file = file;}
    ulong init_t;
};

#endif // MODEL_H
