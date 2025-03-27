#include "RomJointsA.h"
#include "math.h"

void MotorEmulation::loop()
{
    if(!ltime)ltime=millis();
    double dt=(millis()-ltime)/1000.0;
    ltime=millis();

    if(torque)status|=0x80;
    else status &= ~0x80;

    if(control_mode==0){//position control
        double ep = fabs(goal_angle-angle);
        double acc_time=velocity/acceleration;
        double dir=angle<goal_angle?1:-1;
        acceleration = ep>0.5?goal_acceleration:0;
        if(ep<=velocity*acc_time-0.5*acceleration*acc_time*acc_time)
            current+=(velocity-=acceleration*dt);//deceleration phase
        else if(velocity < goal_velocity) current+= (velocity +=acceleration*dt);
        if(velocity<0)current=velocity=0;
        if(velocity>goal_velocity)velocity=goal_velocity;
        if(ep<0.5)velocity=0;
        angle += dir*velocity*dt;
    }

    current*=0.8;
    temp +=0.001*current*current*dt;
    temp = 25+(temp-25)*0.99;
    if(angle<min)angle=min;
    if(angle>max)angle=max;
    //simulate the control loop of angle, velocity and current and voltage...
}




void RomJoints::setVelocity(int8_t id, float vel){ //grad/sec
    if(id>5)return;
dynamixels[id].goal_velocity=vel;
}
void RomJoints::setAcceleration(int8_t id, float acc){ //grad/sec2 mx64
    if(id>5)return;
dynamixels[id].goal_acceleration=acc;
}
void RomJoints::initial_angle(int8_t id, float angle){
    if(id>5)return;
    dynamixels[id].angle=angle;
    dynamixels[id].goal_angle=angle;
}
//equivalent to the motor reset
void RomJoints::setupMotor(int8_t id)
{
   MotorEmulation &m=dynamixels[id];
   m.id=ROM_MOTOR_IDS[id];
   m.status=0;
   m.torque=false;
   m.min=romdefs->limits[id].min;
   m.max=romdefs->limits[id].max;
   m.current=m.velocity=0;
   m.control_mode=0;
   m.goal_velocity=romdefs->vel_profile[id];
   m.goal_acceleration=romdefs->acc_profile[id];
   m.goal_angle=m.angle;
   motors[id].max_angle=m.max;
   motors[id].min_angle=m.min;
   motors[id].velocity=0;
   if(m.angle<m.min)m.angle=m.min;
   if(m.angle>m.max)m.angle=m.max;

}

//initial reading and setting of all motor parameters
void RomJoints::setup(RomDefs *modefs)
{
    romdefs=modefs;
    //SETUP MOTORS CONFIGURATION
    for(int i=0;i<DXL_ID_CNT;i++){
        setupMotor(i);

    }
}

//ciclic reading of position, temperature, intensity etc. via syncread
void RomJoints::loop()
{
  uint8_t i;
  //dxl.ledOn(1);
  for(int i=0;i<DXL_ID_CNT;i++){
//////////// READ STATUS EMULATION
    motors[i].status=dynamixels[i].status;
    motors[i].torqueEnabled=dynamixels[i].status&0x80;


///////////READ POS VEL ETC.

    motors[i].intensity=dynamixels[i].current; //mamp
    motors[i].position=dynamixels[i].angle - romdefs->offset[i]; //grad
    motors[i].velocity=dynamixels[i].velocity; //grad/sec

///////////READ VOLT TEMP.

    motors[i].voltage=dynamixels[i].volt; //volts
    motors[i].temperature=dynamixels[i].temp; //celsius
   }
//dynamixel emulations. placed a the end to simulate the com delays.
  for(auto &mot:dynamixels)mot.loop();
}

void RomJoints::torque(int8_t id, bool on_off){
 if(id<DXL_ID_CNT)dynamixels[id].torque=on_off;
}
void RomJoints::controlMode(int8_t id, uint8_t control_mode){
  if(id<DXL_ID_CNT)motors[id].operatingMode = dynamixels[id].control_mode=control_mode;
}

void RomJoints::setGoalCurrent(int8_t id,float value)
{
 if(id<DXL_ID_CNT)dynamixels[id].goal_current=value;
}

void RomJoints::setGoalAngle(int8_t id,float ang)
{
 if(id<DXL_ID_CNT)dynamixels[id].goal_angle=ang+romdefs->offset[id];
}
void RomJoints::reboot(int8_t id)
{
 if(id<DXL_ID_CNT){setupMotor(id);}
}



int RomJoints::mapIds(int id) {
  for (int j = 0; j < DXL_ID_CNT; j++) {
    if (ROM_MOTOR_IDS[j] == id) {
      return j;
    }
  }

  return -1;
}

void RomJoints::fill_compact_data(RobotCompactData &data)
{
  for(int i=0;i<6;i++){
    data.angle[i]=(uint16_t)(10*motors[i].position);//tenths of degs
    data.intensity[i]=(uint8_t)(motors[i].intensity/20.0); //20 mA
    data.status[i]=motors[i].torqueEnabled?motors[i].status|0x80:motors[i].status&0x7F;
    data.volt[i]=(uint8_t)(10*motors[i].voltage);//tenths of degs
    data.temp[i]=motors[i].temperature;
    data.vel[i]=(uint8_t)motors[i].velocity;
  }
}
