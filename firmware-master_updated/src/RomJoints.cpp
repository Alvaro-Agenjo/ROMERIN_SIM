#include "RomJoints.h"

#define DXL_PROTOCOL_1 1.0F
#define DXL_PROTOCOL_2 2.0F

using namespace ControlTableItem; 

const uint8_t DXL_ID_CNT = 6;
const int ROM_MOTOR_IDS[] = {1, 2, 3, 4, 5, 6};

//auxiliar structures
const struct{float min; float max;} motor_min_max[]={
ROM_MOTOR_2_MIN_MAX, ROM_MOTOR_3_MIN_MAX, ROM_MOTOR_4_MIN_MAX,
ROM_MOTOR_5_MIN_MAX, ROM_MOTOR_6_MIN_MAX, ROM_MOTOR_7_MIN_MAX
};


const uint16_t SR_START_ADDR_LOAD_VEL_POS = 126; //controltable address Mx64 XL430 XH540 current, vel, pos
const uint16_t SR_ADDR_LOAD_VEL_POS_LEN = 10; //2+4+4
struct sr_data_load_vel_pos{
  int16_t present_current;
  int32_t present_velocity;
  int32_t present_position;
} __attribute__((packed));

const uint16_t SR_START_ADDR_VOLT_TEMP = 144; //controltable address Mx64 XL430 XH540 voltage temp
const uint16_t SR_ADDR_LEN_VOLT_TEMP_LEN = 3; //2+1
struct sr_data_volt_temp{
  int16_t present_voltage;
  int8_t present_temperature;
} __attribute__((packed));

const uint16_t SR_STATUS_ADDR = 70;

const uint16_t SR_TORQUE_ADDR = 64;

struct br_data_xel_status{
  int8_t status;
} __attribute__((packed));



sr_data_load_vel_pos sr_load_vel_pos[DXL_ID_CNT];
sr_data_volt_temp sr_volt_temp[DXL_ID_CNT];


const uint16_t user_pkt_buf_cap = 256;
uint8_t user_pkt_buf[user_pkt_buf_cap];

//MX64 infos
DYNAMIXEL::InfoSyncReadInst_t sr_infos_load_vel_pos;
DYNAMIXEL::XELInfoSyncRead_t info_xels_sr_load_vel_pos[DXL_ID_CNT];
DYNAMIXEL::InfoSyncReadInst_t sr_infos_volt_temp;
DYNAMIXEL::XELInfoSyncRead_t info_xels_sr_volt_temp[DXL_ID_CNT];

// DYNAMIXEL::InfoFastSyncReadInst_t sr_infos_load_vel_pos;
// DYNAMIXEL::XELInfoFastSyncRead_t info_xels_sr_load_vel_pos[DXL_ID_CNT];
// DYNAMIXEL::InfoFastSyncReadInst_t sr_infos_volt_temp;
// DYNAMIXEL::XELInfoFastSyncRead_t info_xels_sr_volt_temp[DXL_ID_CNT];


//status infos (de momento excluyendo el 106)
br_data_xel_status br_xel_status[DXL_ID_CNT];
DYNAMIXEL::InfoBulkReadInst_t br_infos_status;
DYNAMIXEL::XELInfoBulkRead_t info_xels_br_status[DXL_ID_CNT];

//torques infos (de momento excluyendo el 106)
br_data_xel_status br_xel_torque[DXL_ID_CNT];
DYNAMIXEL::InfoBulkReadInst_t br_infos_torque;
DYNAMIXEL::XELInfoBulkRead_t info_xels_br_torque[DXL_ID_CNT];

void RomJoints::setVelocity(int8_t id, float vel){ //grad/sec
  int32_t  units;
  units=(int32_t)(vel/1.38);
  dxl.writeControlTableItem(PROFILE_VELOCITY, ROM_MOTOR_IDS[id], units);
 
}
void RomJoints::setAcceleration(int8_t id, float acc){ //grad/sec2 mx64
   dxl.writeControlTableItem(PROFILE_ACCELERATION, ROM_MOTOR_IDS[id], (int32_t)(acc/21.4));    
}
void RomJoints::setupMotor(int8_t id)
{
  dxl.torqueOff(ROM_MOTOR_IDS[id]);
  dxl.writeControlTableItem(MAX_POSITION_LIMIT,ROM_MOTOR_IDS[id],11.3637F*(motor_min_max[id].max));
  dxl.writeControlTableItem(MIN_POSITION_LIMIT,ROM_MOTOR_IDS[id],11.3637F*(motor_min_max[id].min));
    motors[id].max_angle=0.088*dxl.readControlTableItem(MAX_POSITION_LIMIT,ROM_MOTOR_IDS[id]);
    motors[id].min_angle=0.088*dxl.readControlTableItem(MIN_POSITION_LIMIT,ROM_MOTOR_IDS[id]);
    dxl.writeControlTableItem(PROFILE_ACCELERATION, ROM_MOTOR_IDS[id], 25);    
    dxl.writeControlTableItem(PROFILE_VELOCITY, ROM_MOTOR_IDS[id], 5);

    controlMode(id, OP_POSITION);
}

//initial reading and setting of all motor parameters
void RomJoints::setup()
{
  dxl.setPort(dxl_port);
  dxl.begin(ROM_DXL_BAUD_RATE);

  //enumeration of motors 
  // dxl.setModelNumber(0,XH540_W270);
  dxl.setModelNumber(1,MX64_2);
  dxl.setModelNumber(2,XH540_W270);
  dxl.setModelNumber(3,XH540_W270);
  dxl.setModelNumber(4,XL430_W250);
  dxl.setModelNumber(5,XL430_W250);
  dxl.setModelNumber(6,XL430_W250);
  

  dxl.torqueOn(0);
  dxl.setPortProtocolVersion(DXL_PROTOCOL_2);
  for(int i=0;i<DXL_ID_CNT;i++)dxl.ledOn(i);

 // Fill the members of structure to syncRead for the mx64 (1st part)
  sr_infos_load_vel_pos.packet.p_buf = user_pkt_buf;
  sr_infos_load_vel_pos.packet.buf_capacity = user_pkt_buf_cap;
  sr_infos_load_vel_pos.packet.is_completed = false;
  sr_infos_load_vel_pos.addr = SR_START_ADDR_LOAD_VEL_POS;
  sr_infos_load_vel_pos.addr_length = SR_ADDR_LOAD_VEL_POS_LEN;
  sr_infos_load_vel_pos.p_xels = info_xels_sr_load_vel_pos;
  sr_infos_load_vel_pos.xel_count = 0;  

  for(int i = 0; i<DXL_ID_CNT;i++){
    info_xels_sr_load_vel_pos[i].id = ROM_MOTOR_IDS[i];
    info_xels_sr_load_vel_pos[i].p_recv_buf = (uint8_t*)&sr_load_vel_pos[i];
    sr_infos_load_vel_pos.xel_count++;
  }
  sr_infos_load_vel_pos.is_info_changed = true;

 // Fill the members of structure to syncRead for the mx64 (2nd part)
  sr_infos_volt_temp.packet.p_buf = user_pkt_buf;
  sr_infos_volt_temp.packet.buf_capacity = user_pkt_buf_cap;
  sr_infos_volt_temp.packet.is_completed = false;
  sr_infos_volt_temp.addr = SR_START_ADDR_VOLT_TEMP;
  sr_infos_volt_temp.addr_length = SR_ADDR_LEN_VOLT_TEMP_LEN;
  sr_infos_volt_temp.p_xels = info_xels_sr_volt_temp;
  sr_infos_volt_temp.xel_count = 0;  

  for(int i = 0; i<DXL_ID_CNT;i++){
    info_xels_sr_volt_temp[i].id = ROM_MOTOR_IDS[i];
    info_xels_sr_volt_temp[i].p_recv_buf = (uint8_t*)&sr_volt_temp[i];
    sr_infos_volt_temp.xel_count++;
  }
  sr_infos_volt_temp.is_info_changed = true;


 

 //Fill the members of structure bulk read all status (except 106)
  // fill the members of structure for bulkRead using external user packet buffer
  br_infos_status.packet.p_buf = user_pkt_buf;
  br_infos_status.packet.buf_capacity = user_pkt_buf_cap;
  br_infos_status.packet.is_completed = false;
  br_infos_status.p_xels = info_xels_br_status;
  br_infos_status.xel_count = 0;
  for(int i=0;i<DXL_ID_CNT;i++){
    info_xels_br_status[i].id = ROM_MOTOR_IDS[i];
    info_xels_br_status[i].addr = SR_STATUS_ADDR; 
    info_xels_br_status[i].addr_length = 1; 
    info_xels_br_status[i].p_recv_buf = (uint8_t*)&br_xel_status[i];
    br_infos_status.xel_count++;
  }
  br_infos_status.is_info_changed = true;

//Fill the members of structure bulk read all torque state 
  // fill the members of structure for bulkRead using external user packet buffer
br_infos_torque.packet.p_buf = user_pkt_buf;
  br_infos_torque.packet.buf_capacity = user_pkt_buf_cap;
  br_infos_torque.packet.is_completed = false;
  br_infos_torque.p_xels = info_xels_br_torque;
  br_infos_torque.xel_count = 0;
  for(int i=0;i<DXL_ID_CNT;i++){
    info_xels_br_torque[i].id = ROM_MOTOR_IDS[i];
    info_xels_br_torque[i].addr = SR_TORQUE_ADDR; 
    info_xels_br_torque[i].addr_length = 1; 
    info_xels_br_torque[i].p_recv_buf = (uint8_t*)&br_xel_torque[i];
    br_infos_torque.xel_count++;
  }
  br_infos_torque.is_info_changed = true;


//SETUP MOTORS CONFIGURATION
for(int i=0;i<DXL_ID_CNT;i++)setupMotor(i);

}

//ciclic reading of position, temperature, intensity etc. via syncread
void RomJoints::loop()
{
  uint8_t i, recv_cnt;
//dxl.ledOn(1);

// READ STATUS
  // if (cicleNumber%10 == 0) {  // DOES NOT WORK WITH THIS METHODOLOGY. CHANGE TO getStatus(), etc.
    br_infos_status.is_info_changed=true;
    recv_cnt = dxl.bulkRead(&br_infos_status);
    if(recv_cnt  == DXL_ID_CNT){ 
      for(i=0; i<recv_cnt; i++){
          int id= br_infos_status.p_xels[i].id;
          int identifier = mapIds(id);
          if (identifier == -1)
            break;
          if((id>=ROM_MOTOR_IDS[0]) && (id<=ROM_MOTOR_IDS[DXL_ID_CNT - 1]))
            motors[identifier].status=br_xel_status[i].status; 
      }
    }else
    {  
      // return;
      last_dxl_error=dxl.getLastLibErrCode();
      errors++;
    }
  // }

  //READ TORQUE STATE
  br_infos_torque.is_info_changed=true;
  recv_cnt = dxl.bulkRead(&br_infos_torque);
  if(recv_cnt  == DXL_ID_CNT){ 
    for(i=0; i<recv_cnt; i++){
        int id= br_infos_torque.p_xels[i].id;

        auto identifier = mapIds(id);
        if (identifier == -1)
          break;
        if((id>=ROM_MOTOR_IDS[0]) && (id<=ROM_MOTOR_IDS[DXL_ID_CNT - 1]))
          motors[identifier].torqueEnabled=(br_xel_torque[i].status==0x01?true:false);
        
    }
  }else
  {  
    // return;
    last_dxl_error=dxl.getLastLibErrCode();
    errors++;
  }
// }

  //READ POS VEL ETC.
  sr_infos_load_vel_pos.is_info_changed = true;
  recv_cnt = dxl.syncRead(&sr_infos_load_vel_pos);
  if(recv_cnt  == DXL_ID_CNT){ 
    for(i=0; i<recv_cnt; i++){
        int id= sr_infos_load_vel_pos.p_xels[i].id;
        // BT_DEBUG_PRINT("id = %i",id);
        auto identifier = mapIds(id);
        if (identifier == -1)
          break;

        if((id>=ROM_MOTOR_IDS[0]) && (id<=ROM_MOTOR_IDS[DXL_ID_CNT - 1])) {
          motors[identifier].intensity=3.36*(sr_load_vel_pos[i].present_current); //mamp
          motors[identifier].position=0.088*(sr_load_vel_pos[i].present_position); //grad
          motors[identifier].velocity=1.38*(sr_load_vel_pos[i].present_velocity); //grad/sec
        }
     // DEBUG_SERIAL.print(", Error: ");DEBUG_SERIAL.println(sr_infos.p_xels[i].error);

    }
  }else
  {  
    // return;
    last_dxl_error=dxl.getLastLibErrCode();
    errors++;
  }

  //--------------------------------------------------------------------------
  // if (cicleNumber%100 == 0) {
    sr_infos_volt_temp.is_info_changed = true;
    recv_cnt = dxl.syncRead(&sr_infos_volt_temp);
    
    if(recv_cnt  == DXL_ID_CNT){ 
      for(i=0; i<recv_cnt; i++){
          int id= sr_infos_volt_temp.p_xels[i].id;

          auto identifier = mapIds(id);
          if (identifier == -1)
            break;
          
          if((id>=ROM_MOTOR_IDS[0]) && (id<=ROM_MOTOR_IDS[DXL_ID_CNT - 1])){
              motors[identifier].voltage=0.1*sr_volt_temp[i].present_voltage; //volts
              motors[identifier].temperature=sr_volt_temp[i].present_temperature; //celsius
          } 
      
      // DEBUG_SERIAL.print(", Error: ");DEBUG_SERIAL.println(sr_infos.p_xels[i].error);

      }
    }else
    {  
      // return;
      last_dxl_error=dxl.getLastLibErrCode();
      errors++;
    }
  // }

  // cicleNumber++;
  // if (cicleNumber > 200)
  //   cicleNumber = 0;
}
void RomJoints::torque(int8_t id, bool on_off)
{

 if(id<DXL_ID_CNT){
     if(on_off)dxl.torqueOn(ROM_MOTOR_IDS[id]);
     else dxl.torqueOff(ROM_MOTOR_IDS[id]);
     //motors[id].torqueEnabled=on_off;
 }

}
void RomJoints::controlMode(int8_t id, uint8_t control_mode)
{
  if(id<DXL_ID_CNT) {
    motors[id].operatingMode = control_mode;
    dxl.setOperatingMode(ROM_MOTOR_IDS[id], control_mode);
  }
}

void RomJoints::setGoalCurrent(int8_t id,float value)
{
 if(id<DXL_ID_CNT)dxl.setGoalCurrent(ROM_MOTOR_IDS[id],value,UNIT_MILLI_AMPERE);
}

void RomJoints::setGoalAngle(int8_t id,float ang)
{
 if(id<DXL_ID_CNT)dxl.setGoalPosition(ROM_MOTOR_IDS[id],ang,UNIT_DEGREE);
}
void RomJoints::reboot(int8_t id)
{

 if(id<DXL_ID_CNT){
     dxl.reboot(ROM_MOTOR_IDS[id]);
     setupMotor(id);
 }
 
}

// void RomJoints::setTrajectory(int8_t id, const TrajectoryData& traj) {
//   if(id<DXL_ID_CNT){
//     trajectoryStart.at(id) = std::chrono::system_clock::now();
//     trajectory_.at(id) = traj;
//   }
// }

int RomJoints::mapIds(int id) {
  for (int j = 0; j < DXL_ID_CNT; j++) {
    if (ROM_MOTOR_IDS[j] == id) {
      return j;
    }
  }

  return -1;
}