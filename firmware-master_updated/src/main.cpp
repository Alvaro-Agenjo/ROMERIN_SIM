#include <Arduino.h>
//#include <ESP32CAN.h>
//Romerin generic includes
#include "RomerinDefinitions.h"
#include "RomerinMsg.h"
#include "RomDebug.h"
//Romerin components
#include "RomPower.h" 
#include "RomBuzzer.h"
#include "RomJoints.h"
#include "RomBlueTooth.h"
#include "RomWiFi.h"
#include "RomSuctionCup.h"

RomJoints joints;
RomBuzzer buzzer;
RomState state;
RomSuctionCup suction_cup;
RomPower power;

//#define ONLY_CPU  
//#define SLOW_INIT
#define POWER_INFO
//LEDS
#define TEST_LEDS 1
int leds[4]={LED_INFO1_R,LED_INFO2_R,LED_INFO3_R,LED_INFO4_R};
int ledpos=0;

RomerinMsg executeMessage(const RomerinMsg &m);
void sendRegularBtMessages();
void sendRegularWifiMessages();
void updateState();
void setup() {
  RomDefs::readConfiguration();
  RomBT::setup();
  #ifdef SLOW_INIT
  for(int i=10;i>0;i--){
    BT_DEBUG_PRINT("initializing: %d",i);
    delay(1000);
  }
  #endif
  
  for(int led:leds)pinMode(led,OUTPUT);

  #ifndef ONLY_CPU
  
    #ifdef POWER_INFO 
    power.setup();
    #endif
  joints.setup();
  buzzer.setup();
  suction_cup.setup();
  #endif
  
  RomWiFi::setup();

  
   buzzer.blocking_beep(30);
   buzzer.blocking_beep(30,4000);
   buzzer.blocking_beep(30);

   delay(1000);
}


void loop() {
  #ifndef ONLY_CPU
    #ifdef POWER_INFO 
    power.loop();
    #endif
  joints.loop();
  buzzer.loop();
  suction_cup.loop();
  #endif
  updateState(); //checks the robot state
  RomWiFi::loop();
  if(RomWiFi::isConected2Master())sendRegularWifiMessages(); //someone is conected to wifi as master
  if(RomBT::hasClient()){ //someone is connected to BT
    sendRegularBtMessages();
    while(RomBT::readMessage()){
      RomBT::sendMessage(executeMessage(RomBT::getMessage()));
      BT_DEBUG("message completed");
    }//while
  }//if has client


  if(TEST_LEDS){
    for(int i=0;i<4;i++)digitalWrite(leds[i], LOW);
    digitalWrite(leds[ledpos],HIGH);
    if(++ledpos==4)ledpos=0;
  }

}

void sendRegularBtMessages()
{
  for(int i=0;i<6;i++)RomBT::sendMessage(motor_info_message(i,joints.motors[i]));
  #ifdef POWER_INFO 
      RomBT::sendMessage(analog_info_message(power.getBatteryVolt(),power.getBatteryAmp(),
                        0,power.get12vAmp()));
  #endif
  RomBT::sendMessage(state_message(state));
  RomBT::sendMessage(suction_cup_info_message(suction_cup.getRelPressure(), suction_cup.getForce(), 
                        suction_cup.getTemperature(),suction_cup.getDistances()));
}
void sendRegularWifiMessages()
{
  for(int i=0;i<6;i++)RomWiFi::sendMessage(motor_info_message(i,joints.motors[i]));
  #ifdef POWER_INFO 
  RomWiFi::sendMessage(analog_info_message(power.getBatteryVolt(),power.getBatteryAmp(),
                        0,power.get12vAmp()));
  #endif
  RomWiFi::sendMessage(state_message(state));
  RomWiFi::sendMessage(suction_cup_info_message(suction_cup.getRelPressure(), suction_cup.getForce(), 
                        suction_cup.getTemperature(),suction_cup.getDistances()));
}
RomerinMsg executeMessage(const RomerinMsg &m)
{
  
  switch(m.id){
    // case ROM_COUPLER_MOVE:  // TO IMPLEMENT
    //   float value=romerin_getFloat(m.info+1);
    //   joints.setGoalAngle(m.info[0],value);
    //   BT_DEBUG_PRINT("m:%i->%f", m.info[0], value);
    //   }
    case ROM_TORQUE_ON:
      joints.torque(m.info[0]);
      BT_DEBUG_PRINT("TORQUE ON %i", m.info[0]);
      buzzer.beep(500);
      break;
    case ROM_TORQUE_OFF:
      joints.torque(m.info[0],false);
      BT_DEBUG_PRINT("TORQUE OFF %i", m.info[0]);
      buzzer.play(1);
      break;
    case ROM_SUCTION_CUP:{
      suction_cup.setTurbine(m.info[0]);
      BT_DEBUG_PRINT("SUCTION_CUP %i", m.info[0]);
      }
      break;
    case ROM_GOAL_ANG:{
      float value=romerin_getFloat(m.info+1);
      joints.setGoalAngle(m.info[0],value);
      BT_DEBUG_PRINT("m:%i->%f", m.info[0], value);
      }
      break;
    case ROM_VELOCITY_PROFILE:{
      float value=romerin_getFloat(m.info+1);
      joints.setVelocity(m.info[0],value);
      BT_DEBUG_PRINT("vel:%i->%f", m.info[0], value);
      } 
      break;
    case ROM_REBOOT_MOTOR:
      joints.reboot(m.info[0]);
      buzzer.play(0);
      BT_DEBUG_PRINT("REBOOT %i", m.info[0]);
      break;
    case ROM_GET_FIXED_MOTOR_INFO:
      BT_DEBUG("GET_FIXED_INFO");
      return fixed_motor_info_message(m.info[0],joints.motors[m.info[0]]);
    case ROM_GET_NAME:
      BT_DEBUG("GET_NAME");
      return name_message(RomDefs::MODULE_NAME);
    case ROM_SCAN_WIFI:
      RomWiFi::scanNetworks();
      BT_DEBUG("SCAN_WIFIs");
      break;
    case ROM_INIT_WIFI:
      RomWiFi::init();
      BT_DEBUG("INIT_WIFI");
      break;
    case ROM_GET_MOTOR_INFO:
      BT_DEBUG("GET_MOTOR_INFO");
      return motor_info_message(m.info[0],joints.motors[m.info[0]]); 
    case ROM_GET_CONFIG:
      BT_DEBUG("GET_CONFIG");
      return configuration_message(); 
    case ROM_CONFIG:
      romerin_getConfiguration(m.info);
      BT_DEBUG("CONFIG: config changes will take effect after rebooting the robot");
      break; 
    case ROM_TEST_A_MESSAGE: //messages only for testing purposes
      //BT_DEBUG("ATTACH ON");
      //suction_cup.attach();
      suction_cup.test_psensor.reset();
      break;
    case ROM_TEST_B_MESSAGE:
    {
      //BT_DEBUG("ATTACH OFF");
      //suction_cup.dettach();
      if(suction_cup.test_psensor.isConnected()){BT_DEBUG("PRESSURE SENSOR CONECTED");}
      else {BT_DEBUG("PRESSURE SENSOR NOT CONNECTED");}
      auto val=suction_cup.test_psensor.getPressure();
      BT_DEBUG_PRINT("P[Bar]:%f", val);
      auto val2=suction_cup.test_psensor.getTemperature();
      BT_DEBUG_PRINT("T[C]:%f", val2);
      auto ID=suction_cup.test_psensor.getDeviceID();
      BT_DEBUG_PRINT("ID SENSOR=%X",ID);
      for(auto i=0;i<6;i++){BT_DEBUG_PRINT("C[%d]=%X",i,suction_cup.test_psensor.getProm(i));}

    }
      break;
    case ROM_GOAL_CURRENT: {
      float value=romerin_getFloat(m.info+1);
      joints.setGoalCurrent(m.info[0],value);
      BT_DEBUG_PRINT("I:%i->%f", m.info[0], value);
      }
      break;
    case ROM_CONTROL_MODE: {
      uint16_t value=romerin_getUInt16(m.info+1);
      joints.controlMode(m.info[0],value);
      BT_DEBUG_PRINT("mode:%i->%i", m.info[0], value);
      }
      break;
    default:
      BT_DEBUG("UNKNOWN MESSAGE");
  }
  return RomerinMsg::none();
}

void updateState()
{
  static ulong ltime=0;
  if(!ltime)ltime=millis();
  ulong cicle=millis()-ltime;
  ltime=millis();
  if(cicle>255)cicle=255;

  state.connected_to_wifi=RomWiFi::isConnected();
  state.connected_to_bt=RomBT::hasClient();
  state.connected_to_can=false;
  #ifdef POWER_INFO 
  state.power_enabled=(power.getBatteryVolt()>12);
  #else
  state.power_enabled=true;
  #endif
  state.cicle_time=(uint8_t)cicle;
  #ifndef ONLY_CPU
  suction_cup.enableI2Creading(state.power_enabled);
  #endif
}