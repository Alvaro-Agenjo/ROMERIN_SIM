
#include "RomBodyWiFi.h"
#include <stdarg.h>

RomCircularMsgBuffer RomBodyWiFi::buffer;
MsgReader RomBodyWiFi::msg_reader;
AsyncUDP RomBodyWiFi::udp;
IPAddress RomBodyWiFi::ip_remote(0,0,0,0);
long RomBodyWiFi::_time_watch_dog=0;
RomBodyWiFi::WIFI_STATE RomBodyWiFi::state;
long RomBodyWiFi::_timeout=0;
long RomBodyWiFi::_time=0;
long RomBodyWiFi::_stop_time=0;

void RomBodyWiFi::init(int trials, const char *ssid, const char *psswd)
{
  const IPAddress local_IP(RomBodyDefs::IP_ADDRESS[0], RomBodyDefs::IP_ADDRESS[1], RomBodyDefs::IP_ADDRESS[2], RomBodyDefs::IP_ADDRESS[3]);
  const IPAddress gateway(RomBodyDefs::GATEWAY_ADDRESS[0], RomBodyDefs::GATEWAY_ADDRESS[1], RomBodyDefs::GATEWAY_ADDRESS[2], RomBodyDefs::GATEWAY_ADDRESS[3]);
  const IPAddress subnet(RomBodyDefs::SUBNET_MASK[0], RomBodyDefs::SUBNET_MASK[1], RomBodyDefs::SUBNET_MASK[2], RomBodyDefs::SUBNET_MASK[3]); 
    if(WiFi.status() == WL_CONNECTED){
        WiFi.disconnect();
        ROM_DEBUG("Disconnected of current WiFi.");
    }
    if (!WiFi.config(local_IP, gateway, subnet)) ROM_DEBUG("STA Failed to configure");
    
    state=WIFI_STATE::INITIALIZING;

    WiFi.begin(RomBodyDefs::WIFI_SSID, RomBodyDefs::WIFI_KEY);
    ROM_DEBUG("Establishing connection to WiFi..");
    _time=millis();
     
    _timeout = trials;
}
void RomBodyWiFi::setup()
{
  init(1000);
}
void RomBodyWiFi::loop()
{
    switch(state){
        case WIFI_STATE::NONE:
          if (millis() - _stop_time > _timeout)
            init();
          return;
        case WIFI_STATE::INITIALIZING:
             if(WiFi.status() == WL_CONNECTED){
                 state=WIFI_STATE::CONNECTED;
                 ROM_DEBUG("WiFi connected.");
                 char aux[20];
                 WiFi.localIP().toString().toCharArray(aux,20);
                 ROM_DEBUG_PRINT("%s IP address: %s", RomBodyDefs::MODULE_NAME, aux);
                 _time_watch_dog=millis();
                 listenUDP();
                 
             }else if(millis()-_time>_timeout){
                 _stop_time = millis();
                 state=WIFI_STATE::NONE;
                 ROM_DEBUG("Unable to conect to WiFi.");
             }
        break;
        case WIFI_STATE::CONNECTED:
           if(millis()-_time_watch_dog>TIME_OUT_WIFI_MASTER)ip_remote[0]=0;
           while(buffer.there_is_msg()){
                RomerinWifiMsg &&m=buffer.getMessage();
                RomerinMsg &&resp=executeWifiMessage(m);
                RomBodyWiFi::sendMessage(resp,m.ip);
           }
        break;
    }
}

RomerinMsg RomBodyWiFi::executeWifiMessage(const RomerinWifiMsg &m)
{
  switch(m.id){
    case ROM_SET_MASTER_IP:
      _time_watch_dog=millis();
      //si aun no hay master... adopto el master y respondo con mis datos.
      if(!RomBodyWiFi::ip_remote[0])RomBodyWiFi::ip_remote=m.ip;
        else if(RomBodyWiFi::ip_remote[3]!=(m.ip)[3])break;
      return name_message(RomBodyDefs::MODULE_NAME);
      
  }
return executeMessage(m); //remaining messages are executed as always
}

void RomBodyWiFi::sendMessage(const RomerinMsg &m, const IPAddress &ip)
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
void RomBodyWiFi::listenUDP()
{
  if (udp.listen(12000)) {
    ROM_DEBUG_PRINT("UDP Listening at port: %d", 12000);
    
    udp.onPacket([](AsyncUDPPacket packet) {
        for(int i=0;i<packet.length();i++){
            if(msg_reader.add_uchar(packet.data()[i])){
                buffer.push_single(RomerinWifiMsg(msg_reader.getMessage(),packet.remoteIP()),single_message);
                //RomerinMsg &&m=executeWifiMessage(RomerinWifiMsg(msg_reader.getMessage(),packet.remoteIP()));
                //RomWiFi::sendMessage(m,packet.remoteIP());
            }
        }
    });
  }
}
const char *RomBodyWiFi::translateEncryptionType(wifi_auth_mode_t encryptionType) {
 
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
void RomBodyWiFi::scanNetworks() {
 
  int numberOfNetworks = WiFi.scanNetworks(false,true);
  ROM_DEBUG_PRINT("Number of networks found: %d ",numberOfNetworks);
 
  Serial.println(numberOfNetworks);
  char aux[100];
  for (int i = 0; i < numberOfNetworks; i++) {
    WiFi.SSID(i).toCharArray(aux,100);
    ROM_DEBUG_PRINT("NetWork name: %s",aux); 
    ROM_DEBUG_PRINT("Signal strength: %d",WiFi.RSSI(i));
    WiFi.BSSIDstr(i).toCharArray(aux,100);
    ROM_DEBUG_PRINT("MAC address: %s",aux);
    ROM_DEBUG_PRINT("Encryption type: %s", translateEncryptionType(WiFi.encryptionType(i)));
    ROM_DEBUG_PRINT("-----------------------");
 
  }
}
void RomBodyWiFi::sendText(const char *text)
{
      RomerinMsg &&rep_msg = text_message(text);
      RomBodyWiFi::sendMessage(rep_msg);
}
void RomBodyWiFi::sendPrint(const char *fmt, ... )
{
    char text[300];
    va_list myargs;
    va_start(myargs,fmt);
    vsprintf(text,fmt,myargs);
    va_end(myargs);
    RomerinMsg &&rep_msg = text_message(text);
    RomBodyWiFi::sendMessage(rep_msg);
}