#include <Arduino.h>
//#include <ESP32CAN.h>
//Romerin generic includes
#include "RomerinDefinitions.h"
#include "RomerinMsg.h"
#include "RomDebug.h"
#include "RomUtils.h"
//Romerin components
#include "RomPower.h" 
#include "RomBuzzer.h"
#include "RomJoints.h"
#include "RomBlueTooth.h"
#include "RomWiFi.h"
#include "RomSuctionCup.h"
#include "../shared/romkin.cpp"


RomJoints joints;
RomBuzzer buzzer;
RomState state;
RomSuctionCup suction_cup;
RomPower power;

RobotCompactData robot_compact_data{};
#define ENABLE_BT
//#define ONLY_CPU    //comment in normal mode.this is for testing pourposes only
//#define SLOW_INIT
#define POWER_INFO
#define SUCTION_CUP
//LEDS
#define TEST_LEDS 1
int leds[4]={LED_INFO1_R,LED_INFO2_R,LED_INFO3_R,LED_INFO4_R};
int ledpos=0;

RomerinMsg executeMessage(const RomerinMsg &m);
template <class PORT> void sendRegularMessages();
void sendRegularBtMessages();
void sendRegularWifiMessages();
void updateState();

void fill_robot_compact_data()
{
  joints.fill_compact_data(robot_compact_data);
  const auto &&d=suction_cup.getDistances();
  for(int  i=0;i<3;i++) robot_compact_data.sc_distance[i]=d[i];
  robot_compact_data.sc_force=(uint8_t)(suction_cup.getForce());
  robot_compact_data.sc_pressure=(uint8_t)(0.05*suction_cup.getRelPressure());
  robot_compact_data.sc_pressure=(uint8_t)(suction_cup.getTemperature());
}

void setup() {
  RomDefs::readConfiguration();
  state.compact_mode=RomDefs::init_configuration&ROM_CONF_COMPACT;
  
  //kinematics lenghts
  RomKin::set_lenghts(RomDefs::lenghts[0],RomDefs::lenghts[1],RomDefs::lenghts[2],
                      RomDefs::lenghts[3],RomDefs::lenghts[4],RomDefs::lenghts[5]);
  #ifdef ENABLE_BT
  RomBT::setup();
  #endif
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
    #ifdef SUCTION_CUP
    suction_cup.setup();
    #endif
  #endif
  


  RomWiFi::setup();

  
   buzzer.blocking_beep(30);
   buzzer.blocking_beep(30,4000);
   buzzer.blocking_beep(30);

   delay(1000);
}


void loop() {
  static TIMER wifi_update(WIFI_MS_UPDATE), bt_update(BT_MS_UPDATE);
  #ifndef ONLY_CPU
    #ifdef POWER_INFO 
    power.loop();
    #endif
  joints.loop();
  buzzer.loop();
    #ifdef SUCTION_CUP
    suction_cup.loop();
    #endif
  #endif
  updateState(); //checks the robot state
  RomWiFi::loop();

  if(state.compact_mode)fill_robot_compact_data();
  if(RomWiFi::isConected2Master() && wifi_update())sendRegularMessages<RomWiFi>();
 
    
  if(RomBT::hasClient()){ //someone is connected to BT
    if(bt_update())sendRegularMessages<RomBT>();
    while(RomBT::readMessage())RomBT::sendMessage(executeMessage(RomBT::getMessage()));
  }

  if(TEST_LEDS){
    for(int i=0;i<4;i++)digitalWrite(leds[i], LOW);
    digitalWrite(leds[ledpos],HIGH);
    if(++ledpos==4)ledpos=0;
  }

}

//utility class to implement  time controlled comms
/*class ROM_TIMER{
  ulong lt=0;
  uint16_t _time;
  public:
  ROM_TIMER(uint16_t time):_time(time){
    lt=millis();
  }
  bool operator()(){
    if(millis()-lt>_time){
      lt=millis();
      return true;
    }
  return false;
  }
};*/


template <class PORT>
void sendRegularMessages(){

  PORT::sendMessage(state_message(state));
  if(state.compact_mode){ //only one message with all the critical info compresed
    PORT::sendMessage(romerinMsg_robot_compact_data(robot_compact_data));
  }
  else{  //verbose info (default mode)
    for(int i=0;i<6;i++)PORT::sendMessage(motor_info_message(i,joints.motors[i]));
    PORT::sendMessage(suction_cup_info_message(suction_cup.getRelPressure(), suction_cup.getForce(), 
                          suction_cup.getTemperature(),suction_cup.getDistances()));
    #ifdef POWER_INFO 
        PORT::sendMessage(analog_info_message(power.getBatteryVolt(),power.getBatteryAmp(),
                          0,power.get12vAmp()));
    #endif
    }
  


}

RomerinMsg executeMessage(const RomerinMsg &m)
{
  
  switch(m.id){

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
      RomDefs::writeConfiguration();
      BT_DEBUG("CONFIG: config changes will take effect after rebooting the robot");
      break; 
    case ROM_GET_CONFIG_V2:
      BT_DEBUG("GET_CONFIG_V2");
      return configuration_message_v2(); 
    case ROM_CONFIG_V2:
      romerin_getConfiguration_v2(m.info);
      RomDefs::writeConfiguration();
      BT_DEBUG("CONFIG: some config changes (min, max, profiles) will take effect after rebooting");
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
    case ROM_COMPACT_MODE_ON:
      state.compact_mode=true;
      BT_DEBUG("COMPACT MODE ON");
    break;
    case ROM_COMPACT_MODE_OFF:
      state.compact_mode=false;
      BT_DEBUG("COMPACT MODE OFF");
    break;
    case ROM_SET_MOTOR_ZERO:{
      uchar_t m_id;
      uint16_t val;
      if(check_and_get_ServoOffset(m,m_id,val)){
        BT_DEBUG_PRINT("offset for m:%d set at %d",m_id,val);
        joints.set_current_pos_as(m_id,val);
      }else {
        BT_DEBUG("Offset Message error");
      }
    }
    break;
    default:
      BT_DEBUG("UNKNOWN MESSAGE");
  }
  return RomerinMsg::none();
}

void updateState()
{
  static filter<uchar_t,10,uint16_t> ciclef{};
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
  state.cicle_time=(uint8_t)ciclef.add((uchar_t)cicle);
  #ifndef ONLY_CPU
  suction_cup.enableI2Creading(state.power_enabled);
  #endif
}

