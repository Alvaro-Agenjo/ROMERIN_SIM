#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include "RomBodyDefinitions.h"
#include "RomBodyMsg.h"
#include "RomCircularMsgBuffer.h"

#ifdef _WIN32
    #include "AsyncUdp.h"
#else
    #include "AsyncUDP.h"
#endif

class RomBodyWiFi{

    static enum WIFI_STATE {NONE, INITIALIZING, CONNECTED} state;
    static long _timeout;
    static long _time;
    static long _stop_time;
    static long _time_watch_dog;
    static AsyncUDP udp;
    static MsgReader msg_reader;
    static const char * translateEncryptionType(wifi_auth_mode_t encryptionType);
    static RomCircularMsgBuffer buffer;
public:
    static IPAddress ip_remote;
    static void scanNetworks();
    static void init(int timeout=5000, const char *ssid=RomBodyDefs::WIFI_SSID, const char *psswd=RomBodyDefs::WIFI_KEY);
    static void setup();
    static void loop();
    static void listenUDP();
    static bool isConnected(){return state==WIFI_STATE::CONNECTED;}
    static RomerinMsg executeWifiMessage(const RomerinWifiMsg &m);
    static void sendMessage(const RomerinMsg &m, const IPAddress &ip=ip_remote);
    static bool isConected2Master(){return((isConnected())&&(RomBodyWiFi::ip_remote[0]));}
    static void sendText(const char *text);
    static void sendPrint(const char *fmt, ...);

};


#define ENABLE_ROM_DEBUG 1
#define ENABLE_ROM_DEBUG_PRINTF 

#define ROM_DEBUG(X) {if(ENABLE_ROM_DEBUG)RomBodyWiFi::sendText(X);}
#ifdef ENABLE_ROM_DEBUG_PRINTF 
#define ROM_DEBUG_PRINT(...) {if(ENABLE_ROM_DEBUG)RomBodyWiFi::sendPrint(__VA_ARGS__);}
#else 
#define BT_DEBUG_PRINT(...) ;
#endif