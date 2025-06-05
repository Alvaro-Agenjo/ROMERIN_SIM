#ifndef MODEL_H
#define MODEL_H
#include <QHostAddress>
#include <QString>
#include <QUdpSocket>
//#include <memory>
#include <QFile>
//#include "btport.h"

//--------------------------------------------//
#include <list>
#include "../include/RomerinMessage.h"
#include "configdlg.h"


//class RomerinModule;
class Module;
struct ConfigurationInfo;
struct ConfigurationInfoV2;
class ModuleController
{
public:
    QFile * _file;
    QString name;
    bool simulated=false;
    uint8_t virtualid=0;
    QHostAddress ip;
    //BTport *port;
    //QString port_name;
    static QUdpSocket * ip_port;
    //RomerinModule *tab;
    Module *mod;
    uchar_t robot_state;
    uchar_t robot_cicle_time;
    uchar_t wifi_watch_dog=0;
    //uchar_t bt_watch_dog=0;
    bool conected=false;
    std::list<RomerinMsg> ip_msgs;
    //ModuleController():name(""),ip(),port(0),port_name(""),tab(0),wifi_watch_dog(0),bt_watch_dog(0){}
    ModuleController():name(""),ip(), mod(0), wifi_watch_dog(0){}
    ~ModuleController(){
        // if(port)port->close();
        // delete port;
    }
    RomerinMsg executeMessage(const RomerinMsg &m );
    void sendMessage(const RomerinMsg &m);
    //void reset_bt_watchdog(){bt_watch_dog=0;}
    void reset_wifi_watchdog(){wifi_watch_dog=0;}
    bool isConnected();
    void sendConfig(ConfigurationInfo &info);
    void sendConfigV2(ConfigurationInfoV2 &info);
    void loop();
    void getConfig();
    void init_rec(ulong init_t,const QString &directory);
    void stop_rec();
    void save_data();
    void setFile(QFile *file){_file = file;}
    //std::unique_ptr<QFile> file;
    ulong init_t;

    //--------------------------------------------//
    void activateMotors(bool on = true);
};



#endif // MODEL_H
