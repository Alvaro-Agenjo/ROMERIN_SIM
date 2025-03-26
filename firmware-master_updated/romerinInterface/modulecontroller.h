#ifndef MODEL_H
#define MODEL_H
#include <QHostAddress>
#include <QString>
#include <QUdpSocket>
#include "btport.h"

#include <list>
#define WATCHDOG 100
class RomerinModule;
struct ConfigurationInfo;
class ModuleController
{
public:
   QString name;
   QHostAddress ip;
   BTport *port;
   QString port_name;
   static QUdpSocket * ip_port;
   RomerinModule *tab;
   uchar_t robot_state;
   uchar_t robot_cicle_time;
   uchar_t wifi_watch_dog=0;
   uchar_t bt_watch_dog=0;
   bool conected=false;
   std::list<RomerinMsg> ip_msgs;
   ModuleController():name(""),ip(),port(0),port_name(""),tab(0),wifi_watch_dog(0),bt_watch_dog(0){}
   ~ModuleController(){
       if(port)port->close();
       delete port;
   }
   RomerinMsg executeMessage(const RomerinMsg &m );
   void sendMessage(const RomerinMsg &m);
   void reset_bt_watchdog(){bt_watch_dog=0;}
   void reset_wifi_watchdog(){wifi_watch_dog=0;}
   bool isConnected(){return((bt_watch_dog<WATCHDOG)||(wifi_watch_dog<WATCHDOG));}
   void sendConfig(ConfigurationInfo &info);
   void loop();
};



#endif // MODEL_H
