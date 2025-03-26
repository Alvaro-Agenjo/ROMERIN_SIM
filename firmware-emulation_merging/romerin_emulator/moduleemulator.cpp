#include "moduleemulator.h"
#include <QHostAddress>
#include <stdarg.h>
#include "RomerinMsgA.h"
#include "mainwindow.h"
#include <cstring> // Para std::strcpy

///////// UTILITIES ///////////////////////////////////////////////////////////
/// \brief ModuleEmulator::sendText
/// \param text

void ModuleEmulator::sendMessage(const RomerinMsg &m)
{
    if(mainwnd) mainwnd->sendVirtualMessage( id, m);
}
void ModuleEmulator::sendText(const char *text)
{
      sendMessage(text_message(text));
}
void ModuleEmulator::sendPrint(const char *fmt, ... )
{
    char text[300];
    va_list myargs;
    va_start(myargs,fmt);
    vsprintf(text,fmt,myargs);
    va_end(myargs);
    sendMessage( text_message(text));
}
void operator << (char out[], const QString &in){
    std::string aux = in.toStdString();
    std::strcpy(out,aux.c_str());
}
#define BT_DEBUG(X) {sendText(X);}
#define BT_DEBUG_PRINT(...) {sendPrint(__VA_ARGS__);}
static void QString2ipv4(const QString &sip, uint8_t ip[]){
    QHostAddress ipAddress(sip);
    auto ip32=ipAddress.toIPv4Address();
    ip[0]=(ip32 >> 24) & 0xFF;
    ip[1]=(ip32 >> 16) & 0xFF;
    ip[2]=(ip32 >> 8) & 0xFF;
    ip[3]= ip32  & 0xFF;
}
void ModuleEmulator::fill_robot_compact_data()
{
  joints.fill_compact_data(robot_compact_data);
  const auto &&d=suction_cup.getDistances();
  for(int  i=0;i<3;i++) robot_compact_data.sc_distance[i]=d[i];
  robot_compact_data.sc_force=(uint8_t)(suction_cup.getForce());
  robot_compact_data.sc_pressure=(uint8_t)(0.05*suction_cup.getRelPressure());
  robot_compact_data.sc_temp=(uint8_t)(suction_cup.getTemperature());
}
/////////CONSTRUCTION /////////////////////////////////////////////////////////
void ModuleEmulator::fromJson(const QJsonObject &json) {

    id = json["id"].toInt();
    position[0] = static_cast<int16_t>(1000*json["x"].toDouble(0));
    position[1] = static_cast<int16_t>(1000*json["y"].toDouble(0));
    position[2] = static_cast<int16_t>(1000*json["z"].toDouble(0));
    orientation[0] = static_cast<int16_t>(json["rotx"].toDouble(0));
    orientation[1] = static_cast<int16_t>(json["roty"].toDouble(0));
    orientation[2] = static_cast<int16_t>(json["rotz"].toDouble(0));

    /****real module data emulation***/
    MODULE_NAME << json["name"].toString();
    QString data = json["ip_address"].toString();
    if(!data.isEmpty())QString2ipv4(data, IP_ADDRESS);
    data = json["gateway"].toString();
    if(!data.isEmpty())QString2ipv4(data, GATEWAY_ADDRESS);
    data = json["mask"].toString();
    if(!data.isEmpty())QString2ipv4(data, SUBNET_MASK);

    WIFI_SSID << json["WIFI_SSID"].toString();
    WIFI_KEY << json["WIFI_KEY"].toString();

    QJsonArray array = json["lenghts"].toArray();
    for(int i =0;i<array.size();i++)
        lenghts[i]=static_cast<uint16_t>(array[i].toInt(0));
    array = json["mins"].toArray();
    for(int i =0;i<array.size();i++)
        limits[i].min=static_cast<uint16_t>(array[i].toInt(0));
    array = json["maxs"].toArray();
    for(int i =0;i<array.size();i++)
        limits[i].max=static_cast<uint16_t>(array[i].toInt(0));
    array = json["offsets"].toArray();
    for(int i =0;i<array.size();i++)
        offset[i]=static_cast<int8_t>(array[i].toInt(0));
    array = json["vel_profiles"].toArray();
    for(int i =0;i<array.size();i++)
        vel_profile[i]=static_cast<uint8_t>(array[i].toInt(0));
    array = json["acc_profiles"].toArray();
    for(int i =0;i<array.size();i++)
        acc_profile[i]=static_cast<uint16_t>(array[i].toInt(0));

    array = json["initial_position"].toArray();
    for(int i =0;i<array.size();i++)
        joints.initial_angle(i,array[i].toInt(180));



    init_configuration=json["compact_mode"].toBool(false)?ROM_CONF_COMPACT:0x00;
    init_configuration|=json["enable_bt"].toBool(false)?ROM_CONF_DISABLE_BT:0x00;

    //state effect due to json
    state.compact_mode=init_configuration&ROM_CONF_COMPACT;

}

//functions emulation
void ModuleEmulator::setup()
{
    //RomDefs::readConfiguration(); //json is like de readconfig
    romkin.set_lenghts(lenghts); //emulates Kinematics initialization
    //power.setup();
    joints.setup(this);
    suction_cup.setup();
    //delay(1000);
}
void ModuleEmulator::loop()
{
    joints.loop();
    suction_cup.loop();
    attached=suction_cup.isAttached();
    updateState();
    if(state.compact_mode)fill_robot_compact_data();
    sendRegularMessages();
}

RomerinMsg ModuleEmulator::executeMessage(const RomerinMsg &m)
{
    switch(m.id){
      case ROM_TORQUE_ON:
        joints.torque(m.info[0]);
        //buzzer.beep(500);
        BT_DEBUG_PRINT("TORQUE ON %i ", m.info[0]);
        //joints.loop();
        //BT_DEBUG_PRINT("Motor %i ahora es: %i ", m.info[0],joints.motors[m.info[0]].torqueEnabled);

        break;
      case ROM_TORQUE_OFF:
        joints.torque(m.info[0],false);
        //buzzer.play(1);
        BT_DEBUG_PRINT("TORQUE OFF %i", m.info[0]);

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
        //buzzer.play(0);
        BT_DEBUG_PRINT("REBOOT %i", m.info[0]);
        break;
      case ROM_GET_FIXED_MOTOR_INFO:
        BT_DEBUG("GET_FIXED_INFO");
        return fixed_motor_info_message(m.info[0],joints.motors[m.info[0]]);
        break;
      case ROM_GET_NAME:
        BT_DEBUG("GET_NAME");
        return name_message(RomDefs::MODULE_NAME);
        break;
      case ROM_SCAN_WIFI:
        //RomWiFi::scanNetworks();
        BT_DEBUG("SCAN_WIFIs");
        break;
      case ROM_INIT_WIFI:
        //RomWiFi::init();
        BT_DEBUG("INIT_WIFI");
        break;
      case ROM_GET_MOTOR_INFO:
        BT_DEBUG("GET_MOTOR_INFO");
        return motor_info_message(m.info[0],joints.motors[m.info[0]]);
        break;
      case ROM_GET_CONFIG:
        BT_DEBUG("GET_CONFIG");
        return configuration_message(*this);
        break;
      case ROM_CONFIG:
        romerin_getConfiguration(m.info,*this);
        BT_DEBUG("CONFIG: config changes will take effect after rebooting the robot");
        break;
      case ROM_GET_CONFIG_V2:
        BT_DEBUG("GET_CONFIG_V2");
        return configuration_message_v2(*this);
        break;
      case ROM_CONFIG_V2:
        romerin_getConfiguration_v2(m.info, *this);
        BT_DEBUG("CONFIG: some config changes (min, max, profiles) will take effect after rebooting");
        break;
      case ROM_TEST_A_MESSAGE: //messages only for testing purposes
        suction_cup.attach();
        BT_DEBUG("ATTACH ON - TEST A");
        break;
      case ROM_TEST_B_MESSAGE:
        suction_cup.dettach();
        BT_DEBUG("ATTACH OFF");
        break;
      case ROM_GOAL_CURRENT: {
        float value=romerin_getFloat(m.info+1);
        //joints.setGoalCurrent(m.info[0],value);
        BT_DEBUG_PRINT("I:%i->%f", m.info[0], value);
        }
        break;
      case ROM_CONTROL_MODE: {
        uint16_t value=romerin_getUInt16(m.info+1);
        //joints.controlMode(m.info[0],value);
        BT_DEBUG_PRINT("mode:%i->%i", m.info[0], value);
        }
        break;
        case ROM_COMPACT_MODE_ON:
          state.compact_mode=true;
        break;
        case ROM_COMPACT_MODE_OFF:
          state.compact_mode=false;
        break;
      default:

        BT_DEBUG("UNKNOWN MESSAGE");
    }
    return RomerinMsg::none();
}


void ModuleEmulator::updateState()
{


  if(!ltime)ltime=millis();
  ulong cicle=millis()-ltime;
  ltime=millis();
  if(cicle>255)cicle=255;

  state.connected_to_wifi=true;
  state.connected_to_bt=false;
  state.connected_to_can=false;
  state.power_enabled=true;
  state.cicle_time=(uint8_t)cicle;

}
void ModuleEmulator::sendRegularMessages()
{

    //PORT::sendMessage(state_message(state));
    sendMessage(state_message(state));

      if(state.compact_mode){ //only one message with all the critical info compresed
        sendMessage(romerinMsg_robot_compact_data(robot_compact_data));
      }
      else{  //verbose info (default mode)
        for(int i=0;i<6;i++)sendMessage(motor_info_message(i,joints.motors[i]));
        sendMessage(suction_cup_info_message(suction_cup.getRelPressure(), suction_cup.getForce(),
                             suction_cup.getTemperature(),suction_cup.getDistances()));
       /* #ifdef POWER_INFO
            sendMessage(analog_info_message(power.getBatteryVolt(),power.getBatteryAmp(),
                              0,power.get12vAmp()));
        #endif
        */
      }
}
