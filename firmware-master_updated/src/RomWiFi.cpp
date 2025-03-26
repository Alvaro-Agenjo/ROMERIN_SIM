#include "RomDebug.h"
#include "RomWiFi.h"
#include "RomBlueTooth.h"
#include <stdarg.h>

RomCircularMsgBuffer RomWiFi::buffer;
MsgReader RomWiFi::msg_reader;
AsyncUDP RomWiFi::udp;
IPAddress RomWiFi::ip_remote(0,0,0,0);
long RomWiFi::_time_watch_dog=0;
RomWiFi::WIFI_STATE RomWiFi::state;
long RomWiFi::_timeout=0;
long RomWiFi::_time=0;
long RomWiFi::_stop_time=0;

void RomWiFi::init(int trials, const char *ssid, const char *psswd)
{
  const IPAddress local_IP(RomDefs::IP_ADDRESS[0], RomDefs::IP_ADDRESS[1], RomDefs::IP_ADDRESS[2], RomDefs::IP_ADDRESS[3]);
  const IPAddress gateway(RomDefs::GATEWAY_ADDRESS[0], RomDefs::GATEWAY_ADDRESS[1], RomDefs::GATEWAY_ADDRESS[2], RomDefs::GATEWAY_ADDRESS[3]);
  const IPAddress subnet(RomDefs::SUBNET_MASK[0], RomDefs::SUBNET_MASK[1], RomDefs::SUBNET_MASK[2], RomDefs::SUBNET_MASK[3]); 
    if(WiFi.status() == WL_CONNECTED){
        WiFi.disconnect();
        BT_DEBUG("Disconnected of current WiFi.");
    }
    if (!WiFi.config(local_IP, gateway, subnet)) BT_DEBUG("STA Failed to configure");
    
    state=WIFI_STATE::INITIALIZING;

    WiFi.begin(RomDefs::WIFI_SSID, RomDefs::WIFI_KEY);
    BT_DEBUG("Establishing connection to WiFi..");
    _time=millis();
     
    _timeout = trials;
}
void RomWiFi::setup()
{
  init(1000);
}
void RomWiFi::loop()
{
    switch(state){
        case WIFI_STATE::NONE:
          if (millis() - _stop_time > _timeout)
            init();
          return;
        case WIFI_STATE::INITIALIZING:
             if(WiFi.status() == WL_CONNECTED){
                 state=WIFI_STATE::CONNECTED;
                 BT_DEBUG("WiFi connected.");
                 char aux[20];
                 WiFi.localIP().toString().toCharArray(aux,20);
                 BT_DEBUG_PRINT("%s IP address: %s", RomDefs::MODULE_NAME, aux);
                 _time_watch_dog=millis();
                 listenUDP();
                 
             }else if(millis()-_time>_timeout){
                 _stop_time = millis();
                 state=WIFI_STATE::NONE;
                 BT_DEBUG("Unable to conect to WiFi.");
             }
        break;
        case WIFI_STATE::CONNECTED:
           if(millis()-_time_watch_dog>TIME_OUT_WIFI_MASTER)ip_remote[0]=0;
           while(buffer.there_is_msg()){
                RomerinWifiMsg &&m=buffer.getMessage();
                RomerinMsg &&resp=executeWifiMessage(m);
                RomWiFi::sendMessage(resp,m.ip);
           }
        break;
    }
}

RomerinMsg RomWiFi::executeWifiMessage(const RomerinWifiMsg &m)
{
  switch(m.id){
    case ROM_SET_MASTER_IP:
      _time_watch_dog=millis();
      //si aun no hay master... adopto el master y respondo con mis datos.
      if(!RomWiFi::ip_remote[0])RomWiFi::ip_remote=m.ip;
        else if(RomWiFi::ip_remote[3]!=(m.ip)[3])break;
      return name_message(RomDefs::MODULE_NAME);
      
  }
return executeMessage(m); //remaining messages are executed as always
}

void RomWiFi::sendMessage(const RomerinMsg &m, const IPAddress &ip)
{
  if(!ip[0])return;
  if(m.size){
    AsyncUDPMessage mens;
    mens.write(m.data,m.size+3);
    udp.sendTo(mens,ip,12001);  
  }
}
//function that decides if a message sould be overriden
bool single_message(const RomerinMsg &m1,const RomerinMsg &m2){
  if(m1.id!=m2.id)return false; 
  //aqui habría que especificar que mismo tipo de mensaje para articulaciones diferentes son diferentes
  switch (m1.id)
  {
    case ROM_TORQUE_ON:
    case ROM_TORQUE_OFF:
    case ROM_CONTROL_MODE:
    case ROM_GOAL_ANG:
    case ROM_GOAL_CURRENT:
    case ROM_VELOCITY_PROFILE:
    case ROM_REBOOT_MOTOR:
    case ROM_GET_MOTOR_INFO:
    case ROM_GET_FIXED_MOTOR_INFO:
      if(m1.info[0]==m2.info[0])return true;
      return false;
    default:
      return true;
  }
  return true;
}
void RomWiFi::listenUDP()
{
  if (udp.listen(12000)) {
    BT_DEBUG_PRINT("UDP Listening at port: %d", 12000);
    
    udp.onPacket([](AsyncUDPPacket packet) {
        for(int i=0;i<packet.length();i++){
            if(ENABLE_BT_DEBUG_WIFI)BT_DEBUG_PRINT("udp: %d",packet.data()[i]);  // Carlos
            if(msg_reader.add_uchar(packet.data()[i])){
                buffer.push_single(RomerinWifiMsg(msg_reader.getMessage(),packet.remoteIP()),single_message);
                //RomerinMsg &&m=executeWifiMessage(RomerinWifiMsg(msg_reader.getMessage(),packet.remoteIP()));
                //RomWiFi::sendMessage(m,packet.remoteIP());
            }
        }

      if(ENABLE_BT_DEBUG_WIFI)BT_DEBUG_PRINT("Got %u bytes of data", packet.length());
    });
  }
}
const char *RomWiFi::translateEncryptionType(wifi_auth_mode_t encryptionType) {
 
  switch (encryptionType) {
    case (WIFI_AUTH_OPEN):
      return "Open";
    case (WIFI_AUTH_WEP):
      return "WEP";
    case (WIFI_AUTH_WPA_PSK):
      return "WPA_PSK";
    case (WIFI_AUTH_WPA2_PSK):
      return "WPA2_PSK";
    case (WIFI_AUTH_WPA_WPA2_PSK):
      return "WPA_WPA2_PSK";
    case (WIFI_AUTH_WPA2_ENTERPRISE):
      return "WPA2_ENTERPRISE";
    case (WIFI_AUTH_MAX):
       return "WIFI_AUTH_MAX";
  }
  return "UNKNOWN";
}
void RomWiFi::scanNetworks() {
 
  int numberOfNetworks = WiFi.scanNetworks(false,true);
  BT_DEBUG_PRINT("Number of networks found: %d ",numberOfNetworks);
 
  Serial.println(numberOfNetworks);
  char aux[100];
  for (int i = 0; i < numberOfNetworks; i++) {
    WiFi.SSID(i).toCharArray(aux,100);
    BT_DEBUG_PRINT("NetWork name: %s",aux); 
    BT_DEBUG_PRINT("Signal strength: %d",WiFi.RSSI(i));
    WiFi.BSSIDstr(i).toCharArray(aux,100);
    BT_DEBUG_PRINT("MAC address: %s",aux);
    BT_DEBUG_PRINT("Encryption type: %s", translateEncryptionType(WiFi.encryptionType(i)));
    BT_DEBUG_PRINT("-----------------------");
 
  }
}
void RomWiFi::sendText(const char *text)
{
      RomerinMsg &&rep_msg = text_message(text);
      RomWiFi::sendMessage(rep_msg);
}
void RomWiFi::sendPrint(const char *fmt, ... )
{
    char text[300];
    va_list myargs;
    va_start(myargs,fmt);
    vsprintf(text,fmt,myargs);
    va_end(myargs);
    RomerinMsg &&rep_msg = text_message(text);
    RomWiFi::sendMessage(rep_msg);
}