#include "Module/modulesimulator.h"
#include <QHostAddress>
#include <stdarg.h>
//#include "mainwindow.h"
#include <cstring> // Para std::strcpy


///////// UTILITIES ///////////////////////////////////////////////////////////
/// \brief ModuleSimulator::sendText
/// \param text

void ModuleSimulator::sendMessage(const RomerinMsg &m)
{
  sendVirtualMessage( id, m);
}
void ModuleSimulator::sendText(const char *text)
{
  sendMessage(text_message(text));
}
void ModuleSimulator::sendPrint(const char *fmt, ... )
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

//functions simulation

void ModuleSimulator::setControlModel(mjModel* m, mjData* d){
  this->model = m;
  this->modelData = d;
}

void ModuleSimulator::setup(mjModel* m, mjData* d){
    setControlModel(m, d);
    //RomDefs::readConfiguration(); //json is like de readconfig
    romkin.set_lenghts(lenghts); //emulates Kinematics initialization
    joints.setup(this, m, d);
    joints.setGoalAnglesConversions();
    setEstadoInicial(m,d);
    //suction_cup.setup();
}
void ModuleSimulator::loop()
{
    hardwareSettings();
    //for(int i = 0; i < 6; i ++) joints.updateMotorInfo(joints.getMotorSim(i), i);
    if(millis() % 10 == 0){
      softwareSettings();
    }
}

void ModuleSimulator::hardwareSettings(){
    joints.loop(MODULE_NAME);
}

void ModuleSimulator::softwareSettings(){
    updateState();
    sendRegularMessages();
}


void ModuleSimulator::sendRegularMessages()
{

    //PORT::sendMessage(state_message(state));
    sendMessage(state_message(state));

      /*if(state.compact_mode){ //only one message with all the critical info compresed
        sendMessage(romerinMsg_robot_compact_data(robot_compact_data));
      }*/
      //else{  //verbose info (default mode)

        for(int i=0;i<6;i++){
          sendMessage(motor_info_message(i,joints.motors[i]));
        }
        
        //sendMessage(suction_cup_info_message(suction_cup.getRelPressure(), suction_cup.getForce(),
        //suction_cup.getTemperature(),suction_cup.getDistances()));
        /* #ifdef POWER_INFO
            sendMessage(analog_info_message(power.getBatteryVolt(),power.getBatteryAmp(),
                              0,power.get12vAmp()));
        #endif
        
      }*/
}

RomerinMsg ModuleSimulator::executeMessage(const RomerinMsg &m)
{   
    switch(m.id){
      case ROM_TORQUE_ON:
        joints.torque(m.info[0]);
        BT_DEBUG_PRINT("TORQUE ON %i ", m.info[0]);
        BT_DEBUG_PRINT("Motor %i ahora es: %i ", m.info[0],joints.motors[m.info[0]].torqueEnabled);

        break;
      case ROM_TORQUE_OFF:
        joints.torque(m.info[0],false);
        BT_DEBUG_PRINT("TORQUE OFF %i", m.info[0]);

        break;
      case ROM_SUCTION_CUP:{
        float value=romerin_getFloat(m.info+1);
        joints.setTurbine(m.info[0]);
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
        //suction_cup.attach();
        BT_DEBUG("ATTACH ON - TEST A");
        break;
      case ROM_TEST_B_MESSAGE:
        //suction_cup.dettach();
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
      default:

        BT_DEBUG("UNKNOWN MESSAGE");
    }
    return RomerinMsg::none();
}

void ModuleSimulator::updateState()
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

void ModuleSimulator::sendVirtualMessage(uint8_t vid, const RomerinMsg &m){
  
  static int num{};
  if(!m.size)return;
  if(ip_port && !sender.isNull()){
    auto &&mv=romerinMsg_simulate(vid,m);
    ip_port->writeDatagram((const char *)(mv.data),mv.size+3,sender,12001);
    //qDebug()<<"mensaje: "<<++num;
  }
    
}
#include <iostream>
void ModuleSimulator::fromJson(const QJsonObject &json) {

    if(MODULE_NAME == json["name"].toString()){

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
      for(int i =0;i<array.size();i++){
          limits[i].min=static_cast<int16_t>(array[i].toInt());
      }
      array = json["maxs"].toArray();
      for(int i =0;i<array.size();i++){
          limits[i].max=static_cast<int16_t>(array[i].toInt(0));
      }
      array = json["offsets"].toArray();
      for(int i =0;i<array.size();i++)
          offset[i]=static_cast<int16_t>(array[i].toInt(0));
      array = json["vel_profiles"].toArray();
      for(int i =0;i<array.size();i++)
          vel_profile[i]=static_cast<int16_t>(array[i].toInt(0));
      array = json["acc_profiles"].toArray();
      for(int i =0;i<array.size();i++)
          acc_profile[i]=static_cast<int16_t>(array[i].toInt(0));

      array = json["initial_position"].toArray();
      for(int i =0;i<array.size();i++){
          joints.initial_angle(i,array[i].toInt(0));
      }
      array = json["mass"].toArray();
      for(int i =0;i<array.size();i++){
          joints.setMass(i,array[i].toDouble());
      }

      init_configuration=json["compact_mode"].toBool(false)?ROM_CONF_COMPACT:0x00;
      init_configuration|=json["enable_bt"].toBool(false)?ROM_CONF_DISABLE_BT:0x00;

      //state effect due to json
      state.compact_mode=init_configuration&ROM_CONF_COMPACT;
    }
}

void ModuleSimulator::setEstadoInicial(mjModel* m, mjData* d){

  int i = id -1;

    const char* Q1[] = {"Q_THOR_2", "Q_ODIN_2", "Q_LOKI_2", "Q_FRIGG_2"};
    const char* Q2[] = {"Q_THOR_3", "Q_ODIN_3", "Q_LOKI_3", "Q_FRIGG_3"};
    const char* Q3[] = {"Q_THOR_4", "Q_ODIN_4", "Q_LOKI_4", "Q_FRIGG_4"};
    const char* Q4[] = {"Q_THOR_5", "Q_ODIN_5", "Q_LOKI_5", "Q_FRIGG_5"};
    const char* Q5[] = {"Q_THOR_6", "Q_ODIN_6", "Q_LOKI_6", "Q_FRIGG_6"};
    const char* Q6[] = {"Q_THOR_7", "Q_ODIN_7", "Q_LOKI_7", "Q_FRIGG_7"};

        int joint_Q1 = mj_name2id(m, mjOBJ_JOINT, Q1[i]);
        int joint_Q2 = mj_name2id(m, mjOBJ_JOINT, Q2[i]);
        int joint_Q3 = mj_name2id(m, mjOBJ_JOINT, Q3[i]);
        int joint_Q4 = mj_name2id(m, mjOBJ_JOINT, Q4[i]);
        int joint_Q5 = mj_name2id(m, mjOBJ_JOINT, Q5[i]);
        int joint_Q6 = mj_name2id(m, mjOBJ_JOINT, Q6[i]);
        if (joint_Q1 == -1 || joint_Q2 == -1 || joint_Q3 == -1 || joint_Q4 == -1 || joint_Q5 == -1 || joint_Q6 == -1) {
          printf("Joint no encontrada\n");
        } else {
            /*d->qpos[m->jnt_qposadr[joint_Q1]] = (joints.getMotorSim(0)->initialAngle_Real) * RomKin.deg2rad;
            d->qpos[m->jnt_qposadr[joint_Q2]] = (joints.getMotorSim(1)->initialAngle_Real) * RomKin.deg2rad;
            d->qpos[m->jnt_qposadr[joint_Q3]] = (joints.getMotorSim(2)->initialAngle_Real) * RomKin.deg2rad;
            d->qpos[m->jnt_qposadr[joint_Q4]] = (joints.getMotorSim(3)->initialAngle_Real) * RomKin.deg2rad;
            d->qpos[m->jnt_qposadr[joint_Q5]] = (0.5*(joints.getMotorSim(5)->initialAngle_Real - joints.getMotorSim(4)->initialAngle_Real))* RomKin.deg2rad;
            d->qpos[m->jnt_qposadr[joint_Q6]] = (((joints.getMotorSim(5)->initialAngle_Real + joints.getMotorSim(4)->initialAngle_Real)/2) * joints.getMotorSim(5)->romkin.factor_6) * RomKin.deg2rad;*/

            /*if(MODULE_NAME == "THOR"){
              printf("Angle 1: %lf\n",joints.getMotorSim(0)->m2q(joints.getMotorSim(0)->getInitialAngle() * RomKin.deg2rad));
              printf("Angle 2: %lf\n",joints.getMotorSim(1)->m2q(joints.getMotorSim(1)->getInitialAngle() * RomKin.deg2rad));
              printf("Angle 3: %lf\n",joints.getMotorSim(2)->m2q(joints.getMotorSim(2)->getInitialAngle() * RomKin.deg2rad));
              printf("Angle 4: %lf\n",joints.getMotorSim(3)->m2q(joints.getMotorSim(3)->getInitialAngle() * RomKin.deg2rad));
              printf("Angle 5: %lf\n",joints.getMotorSim(4)->m2q(joints.getMotorSim(4)->getInitialAngle() * RomKin.deg2rad));
              printf("Angle 6: %lf\n",joints.getMotorSim(5)->m2q(joints.getMotorSim(5)->getInitialAngle() * RomKin.deg2rad));
              printf("-----------------\n");
            }*/
            
            d->qpos[m->jnt_qposadr[joint_Q1]] = joints.getMotorSim(0)->m2q(joints.getMotorSim(0)->getInitialAngle() * RomKin.deg2rad);
            d->qpos[m->jnt_qposadr[joint_Q2]] = joints.getMotorSim(1)->m2q(joints.getMotorSim(1)->getInitialAngle() * RomKin.deg2rad);
            d->qpos[m->jnt_qposadr[joint_Q3]] = joints.getMotorSim(2)->m2q(joints.getMotorSim(2)->getInitialAngle() * RomKin.deg2rad);
            d->qpos[m->jnt_qposadr[joint_Q4]] = joints.getMotorSim(3)->m2q(joints.getMotorSim(3)->getInitialAngle() * RomKin.deg2rad);
            d->qpos[m->jnt_qposadr[joint_Q5]] = joints.getMotorSim(4)->m2q(joints.getMotorSim(4)->getInitialAngle() * RomKin.deg2rad);
            d->qpos[m->jnt_qposadr[joint_Q6]] = joints.getMotorSim(5)->m2q(joints.getMotorSim(5)->getInitialAngle() * RomKin.deg2rad);
          }
}