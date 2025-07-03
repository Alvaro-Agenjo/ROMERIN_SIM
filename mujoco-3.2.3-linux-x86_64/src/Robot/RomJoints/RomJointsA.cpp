#include "RomJoints/RomJointsA.h"
#include <cmath>
#include <iostream>

void MotorSimulation::setModel(mjModel* m, mjData* d, QString name){
  this->model = m;
  this->modelData = d;
  this->moduleName = name;
}

void MotorSimulation::loop(int number)
{
  int num;
  float goal_angle_real =  m2q(goal_angle);

  float maxVel = (0.01*goal_velocity) * maxVelRadS;
  float velocityRad;

  if(moduleName == "THOR") num = number + 0;
  else if(moduleName == "FRIGG") num = number + 6;
  else if(moduleName == "ODIN") num = number + 12;
  else if(moduleName == "LOKI") num = number + 18;

    float Kp_pos = 18;
    float Ki_pos = 0; // Es 0 porque sino, con el tiempo va a segruir moviéndose
    float Kd_pos = 3;

    float Kp_vel = 15;
    float Kd_vel = 6;
  
    angle = getAngle(number);
    velocity = getVelocity(number); // En grados
    velocityRad = velocity * (M_PI/180); // En radianes

    if(!ltime)ltime=millis();
    double dt=(millis()-ltime)/1000; // Tiempo en segundos
    ltime=millis();

    if(torque)status|=0x80;
    else status &= ~0x80;
      // PID Posición

      double ep = (goal_angle_real - angle); // Error en radianes

      //if(ep > 0.02 && ep < 0.175 && ep > 0) ep = 0.18;
      //if(ep < -0.02 && ep > -0.175 && ep < 0) ep = -0.18;

      ep_sum += ep; // Para la parte de integral
      if(ep < 0.005) velocity = 0;
      acceleration = (ep>0.5) ? goal_acceleration:0;
      
      double Propor_pos = Kp_pos * ep;
      double Inte_pos = Ki_pos * ep_sum;
      double Der_pos = Kd_pos * (ep-ep_old);

      // PD Velocidad

      float vel_deseada = Propor_pos - velocityRad;
      if(vel_deseada < -maxVel) vel_deseada = - maxVel;
      if(vel_deseada > maxVel) vel_deseada = maxVel;
      vel_deseada = std::clamp(maxVel, -maxVel, vel_deseada);

      double ev = (vel_deseada - velocityRad); // Error de velocidad rads/s
      double Propor_vel = Kp_vel * ev;
      double Der_vel = Kd_vel * (ev - ev_old);

      torqueForce = (float)Propor_vel + (float)Der_vel; // La fuerza que se le va a dar al motor
      //if(moduleName == "THOR" && id == 2)printf("Fuerza torque: %f\n", torqueForce);
      modelData->ctrl[num] = (torque) ? torqueForce : 0;   // Se da la fuerza al motor
      
      
      ep_old = ep; //Se guarda el error para la siguiente iteración
      ev_old = ev; //Se guarda el error para la siguiente iteración
      
      ep_sum += ep;// Esto es para el integrador

      if(angle<min)goal_angle_real=min;
      if(angle>max)goal_angle_real=max;
      angle_old = angle;
}

float MotorSimulation::getAngle(int numMotor){
  
  // const char* Q1[] = {"Q_THOR_2", "Q_THOR_3", "Q_THOR_4", "Q_THOR_5", "Q_THOR_6", "Q_THOR_7"};
  // const char* Q2[] = {"Q_ODIN_2", "Q_ODIN_3", "Q_ODIN_4", "Q_ODIN_5", "Q_ODIN_6", "Q_ODIN_7"};
  // const char* Q3[] = {"Q_LOKI_2", "Q_LOKI_3", "Q_LOKI_4", "Q_LOKI_5", "Q_LOKI_6", "Q_LOKI_7"};
  // const char* Q4[] = {"Q_FRIGG_2", "Q_FRIGG_3", "Q_FRIGG_4", "Q_FRIGG_5", "Q_FRIGG_6", "Q_FRIGG_7"};

  const char* Q1[] = {"THOR_Q1", "THOR_Q2", "THOR_Q3", "THOR_Q4", "THOR_Q5", "THOR_Q6"};
  const char* Q2[] = {"FRIGG_Q1", "FRIGG_Q2", "FRIGG_Q3", "FRIGG_Q4", "FRIGG_Q5", "FRIGG_Q6"};
  const char* Q3[] = {"ODIN_Q1", "ODIN_Q2", "ODIN_Q3", "ODIN_Q4", "ODIN_Q5", "ODIN_Q6"};
  const char* Q4[] = {"LOKI_Q1", "LOKI_Q2", "LOKI_Q3", "LOKI_Q4", "LOKI_Q5", "LOKI_Q6"};
  
  const char* Q[6];

  if (moduleName == "THOR") {
    for(int i = 0; i < 6; ++i) {
      Q[i] = Q1[i];
    }
  } else if (moduleName == "FRIGG") {
    for(int i = 0; i < 6; ++i) {
      Q[i] = Q2[i];
    }
  } else if (moduleName == "ODIN") {
    for(int i = 0; i < 6; ++i) {
      Q[i] = Q3[i];
    }
  } else if (moduleName == "LOKI") {
    for(int i = 0; i < 6; ++i) {
      Q[i] = Q4[i];
    }
  } else {
    for(int i = 0; i < 6; ++i) {
      Q[i] = NULL;
    }
  }

  int jointDir = mj_name2id(model, mjOBJ_JOINT, Q[numMotor]);
  if (jointDir == -1) {
    printf("Joint not found\n");
    return 0.0f;
  }

  //float angleQ = m2q((float)modelData->qpos[model->jnt_qposadr[jointDir]]) * romkin.deg2rad;
  auto angleQ = modelData->qpos[model->jnt_qposadr[jointDir]];
  //if(moduleName == "THOR")printf("Angle %i: %f\n", id, angleQ * romkin.deg2rad);
  return (angleQ);
}

float MotorSimulation::getVelocity(int numMotor){

  // const char* Q1[] = {"THOR_2", "Q_THOR_3", "Q_THOR_4", "Q_THOR_5", "Q_THOR_6", "Q_THOR_7"};
  // const char* Q2[] = {"Q_ODIN_2", "Q_ODIN_3", "Q_ODIN_4", "Q_ODIN_5", "Q_ODIN_6", "Q_ODIN_7"};
  // const char* Q3[] = {"Q_LOKI_2", "Q_LOKI_3", "Q_LOKI_4", "Q_LOKI_5", "Q_LOKI_6", "Q_LOKI_7"};
  // const char* Q4[] = {"Q_FRIGG_2", "Q_FRIGG_3", "Q_FRIGG_4", "Q_FRIGG_5", "Q_FRIGG_6", "Q_FRIGG_7"};
  
  const char* Q1[] = {"THOR_Q1", "THOR_Q2", "THOR_Q3", "THOR_Q4", "THOR_Q5", "THOR_Q6"};
  const char* Q2[] = {"FRIGG_Q1", "FRIGG_Q2", "FRIGG_Q3", "FRIGG_Q4", "FRIGG_Q5", "FRIGG_Q6"};
  const char* Q3[] = {"ODIN_Q1", "ODIN_Q2", "ODIN_Q3", "ODIN_Q4", "ODIN_Q5", "ODIN_Q6"};
  const char* Q4[] = {"LOKI_Q1", "LOKI_Q2", "LOKI_Q3", "LOKI_Q4", "LOKI_Q5", "LOKI_Q6"};
  
  const char* Q[6];

  if (moduleName == "THOR") {
    for(int i = 0; i < 6; ++i) {
      Q[i] = Q1[i];
    }
  } else if (moduleName == "FRIGG") {
    for(int i = 0; i < 6; ++i) {
      Q[i] = Q2[i];
    }
  } else if (moduleName == "ODIN") {
    for(int i = 0; i < 6; ++i) {
      Q[i] = Q3[i];
    }
  } else if (moduleName == "LOKI") {
    for(int i = 0; i < 6; ++i) {
      Q[i] = Q4[i];
    }
  } else {
    for(int i = 0; i < 6; ++i) {
      Q[i] = NULL;
      printf("Nombre de joint no encontrado\n");
    }
  }

  int jointDir = mj_name2id(model, mjOBJ_JOINT, Q[numMotor]);
  if (jointDir == -1) {
    printf("Joint not found\n");
    return 0.0f;
  }

    auto velQ = (modelData->qvel[model->jnt_dofadr[jointDir]])*(180.0f/M_PI);

    //if(velQ < 0) velQ = (-1)*velQ; // Para que la velocidad salga en valor absoluto

    return static_cast<float>(velQ);
}

float MotorSimulation::m2q(float goalAng){
  switch(id){
    case 1:{
      //printf("ang1: %f\n", (goalAng - M_PI) * romkin.rad2deg);
      return (goalAng - M_PI);  
      
    }
    break;
    case 2:{
      //printf("ang2: %f\n", (goalAng - M_PI) * romkin.rad2deg);
        return (goalAng - M_PI);
    }
    break;
    case 3:{
      //printf("ang3: %f\n", (goalAng - M_PI/2) * romkin.rad2deg);
        return (goalAng - (M_PI/2));
    }
    break;
    case 4:{
      //printf("ang4: %f\n", (((goalAng) * romkin.factor_4) - M_PI) * romkin.rad2deg);
        q3 = (((goalAng - M_PI) * romkin.factor_4));
        return q3;
    }
    case 5:{
      //printf("ang5: %f\n", ((goal_ang5 - goalAng)/2) * romkin.rad2deg);
        q4 = ((goal_ang5 - goalAng)/2);
        return q4;
    }
    case 6:{
      //printf("ang6: %f\n", (((goalAng + goal_ang4)/2 - M_PI) * romkin.factor_6) * romkin.rad2deg);
        q5 = ((goalAng + goal_ang4)/2 - M_PI) * romkin.factor_6;
        return q5;
    }
    default:{
        return 0;
    } 
  }
}

float RomJoints::q2m(int mot_id){
  float m;
  switch(mot_id){
    case 0:{
        return (dynamixels[0].angle * romkin.rad2deg + 180);
    }
    break;
    case 1:{
        return (dynamixels[1].angle * romkin.rad2deg + 180);
    }
    break;
    case 2:{
        return (dynamixels[2].angle * romkin.rad2deg + (180/2));
    }
    break;
    case 3:{
        m = (dynamixels[3].angle  / (float)romkin.factor_4) * romkin.rad2deg + 180;
        return m;
    }
    break;
    case 4:{
        m = ((dynamixels[5].angle/romkin.factor_6) - dynamixels[4].angle) * romkin.rad2deg +180;
        return m;
    }
    break;
    case 5:{
        m = (dynamixels[4].angle + (dynamixels[5].angle/romkin.factor_6))* romkin.rad2deg +180;
        return m;
    }
    break;
    default:{
      return 0;
    }
  }
}

float MotorSimulation::q2m(float angle_get){
  float m;
  switch(id){
    case 1:{
        return (angle_get * romkin.rad2deg + 180);
    }
    break;
    case 2:{
        return (angle_get * romkin.rad2deg + 180);
    }
    break;
    case 3:{
        return (angle_get * romkin.rad2deg + (90));
    }
    break;
    case 4:{
        m = (angle_get  / Ang3) * romkin.rad2deg + 180;
        return m;
    }
    break;
    case 5:{
        m = ((Ang4/romkin.factor_6) - angle_get) * romkin.rad2deg +180;
        return m;
    }
    break;
    case 6:{
        m = (Ang4 + (angle_get/romkin.factor_6))* romkin.rad2deg +180;
        return m;
    }
    break;
    default:{
      return 0;
    }
  }
}

void RomJoints::setVelocity(int8_t id, float vel){ //grad/sec
dynamixels[id].goal_velocity=vel;
}
void RomJoints::setAcceleration(int8_t id, float acc){ //grad/sec2 mx64
dynamixels[id].goal_acceleration=acc;
}
//equivalent to the motor reset
void RomJoints::setupMotor(int8_t id)
{
   int j= 0;

   MotorSimulation &m=dynamixels[id];

   m.id=ROM_MOTOR_IDS[id];
   m.status=0;
   m.torque=false;
   m.min=romdefs->limits[id].min;
   m.max=romdefs->limits[id].max;
   //m.current=m.velocity=0;
   m.control_mode=0;
   m.goal_velocity=romdefs->vel_profile[id];
   m.goal_acceleration=romdefs->acc_profile[id];
   m.goal_angle=m.initialAngle * romkin.deg2rad;
   motors[id].max_angle=m.max;
   motors[id].min_angle=m.min;
   if(m.angle<m.min)m.angle=m.min;
   if(m.angle>m.max)m.angle=m.max;

}

//initial reading and setting of all motor parameters
void RomJoints::setup(RomDefs *modefs, mjModel* m, mjData* d){
    romdefs=modefs;
    this->model = m;
    this->modelData = d;
    //SETUP MOTORS CONFIGURATION
    for(int i=0;i<DXL_ID_CNT;i++){
        setupMotor(i);
    }
}

//ciclic reading of position, temperature, intensity etc. via syncread
void RomJoints::loop(QString module_name)
{

  this->moduleName = module_name;
  uint8_t i, j;
  //dxl.ledOn(1);
  if(moduleName == "THOR") j = 0;
  else if(moduleName == "FRIGG") j = 6;
  else if(moduleName == "ODIN") j = 12;
  else if(moduleName == "LOKI") j = 18;
  

  //dynamixel simulations.
  int numMotor = 0;
  int numSuct = 0;
  for(auto &mot:dynamixels){
    if(!mot.getSetup()){
      mot.setModel(model, modelData, moduleName);
      mot.setSetup(true);
    }   
    mot.loop(numMotor);
    numMotor++;
  }
  if(!suctioncup.getSetup()){
    suctioncup.setModel(model, modelData, moduleName);
    suctioncup.setSetup(true);
  }
  suctioncup.loop(moduleName);

  for(int i=0;i<DXL_ID_CNT;i++){
    //////////// READ STATUS EMULATION
    motors[i].status=dynamixels[i].status;
    motors[i].torqueEnabled=dynamixels[i].status&0x80;

    ///////////READ POS VEL ETC.

    motors[i].intensity= dynamixels[i].getTorqueForce(); //mamp
    motors[i].position= q2m(i);// Manda el ángulo en grados
    //motors[i].position= dynamixels[i].angle * romkin.rad2deg;
    motors[i].velocity=dynamixels[i].velocity; //grad/sec

    ///////////READ VOLT TEMP.

    motors[i].voltage= 5; //volts
    motors[i].temperature= 21; //celsius

    ///////////ANGLE STORAGE

    if(i == 3){
        Ang3 = dynamixels[i].angle;
        goal_ang3 = dynamixels[i].goal_angle;
        q3 = dynamixels[i].q3;
    }
    if(i == 4){
        Ang4 = dynamixels[i].angle;
        goal_ang4 = dynamixels[i].goal_angle;
        q4 = dynamixels[i].q4;
    }
    if(i == 5){
        Ang5 = dynamixels[i].angle;
        goal_ang5 = dynamixels[i].goal_angle;
        q5 = dynamixels[i].q5;
    }
  
  //////////////////////////////////////////////////////////
  //        Codified torque into current (sym_only)       //
  //////////////////////////////////////////////////////////

  if(i == 0) motors[i].intensity *= 4.0 / 6.0;  // 4A / 6Nm
  else if( i < 3) motors[i].intensity *= 5.0 / 10.0; // 5A / 10 Nm
  else motors[i].intensity *= 1.4 / 1.9; // 1.4A / 1.9Nm
  
  motors[i].intensity *= 1000.0; //conversion to mA
}
  setGoalAnglesConversions();

  ///////////FORCE STORAGE
  suction_cup.suctionForce = suctioncup.suctForce;  //IS units 
  
  ///////////PRESSURE STORAGE
  suction_cup.suctionPressure = suctioncup.suctForce/ (M_PI* 0.095 * 0.095);
  
  ///////////TEMPERATURE STORAGE
  suction_cup.temperature = suctioncup.goal_suctForce;
  
  ///////////DISTANCES STORAGE
  for(int i = 0; i < 3; i++){
    suction_cup.distances[i] = suctioncup.distance[i];
  }
}

void RomJoints::setGoalAnglesConversions(){
  for(int i = 0; i < 6; i++){

    dynamixels[i].Ang3 = Ang3;
    dynamixels[i].Ang4 = Ang4;
    dynamixels[i].Ang5 = Ang5;

    dynamixels[i].goal_ang3 = dynamixels[3].goal_angle;
    dynamixels[i].goal_ang4 = dynamixels[4].goal_angle;
    dynamixels[i].goal_ang5 = dynamixels[5].goal_angle;

    dynamixels[i].q3 = q3;
    dynamixels[i].q4 = q4;
    dynamixels[i].q5 = q5;
  }
}

void RomJoints::torque(int8_t id, bool on_off){
 if(id<DXL_ID_CNT)dynamixels[id].torque=on_off;
}
void RomJoints::controlMode(int8_t id, uint8_t control_mode){
  //if(id<DXL_ID_CNT)motors[id].operatingMode = dynamixels[id].control_mode=control_mode;
}

/*void RomJoints::setGoalCurrent(int8_t id,float value)
{
 if(id<DXL_ID_CNT)dynamixels[id].goal_current=value;
}*/

void RomJoints::initial_angle(int8_t id, float angle){
    if(id>5)return;
    dynamixels[id].initialAngle= angle;
}

void RomJoints::setMass(int8_t id, float masa){
    if(id>5)return;
    mass[id]= masa;
}

void RomJoints::setGoalAngle(int8_t id,float ang)
{
 if(id<DXL_ID_CNT){
  dynamixels[id].goal_angle=(ang+romdefs->offset[id]) * (2*M_PI/360);
 }
}

void RomJoints::setTurbine(int8_t force)
{
 suctioncup.goal_suctForce=force;
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

// SUCTION

void SuctionSimulation::setModel(mjModel* m, mjData* d, QString n){
  this->model = m;
  this->modelData = d;
  this->moduleName = n;
}

void SuctionSimulation::loop(QString name){
int num = 0;
const char * sforce, *sdist[3];

  // -----------------------------------------------------
  // Al ajustar el rango del actuador en el archivo 
  // modelo.xml no seria necesario el multiplicador x0.1
  // ----------------------------------------------------

  // if(name == "THOR"){ modelData->ctrl[24] = (0.1 * goal_suctForce); /*printf("Fuerza de succion: %f\n", extractSuctData());*/}
  // else if(name == "LOKI") modelData->ctrl[25] = (0.1 * goal_suctForce);
  // else if(name == "ODIN") modelData->ctrl[26] = (0.1 * goal_suctForce);
  // else if(name == "FRIGG") modelData->ctrl[27] = (0.1 * goal_suctForce);
  // else printf("MODULE NAME INCORRECTO");

  if(name == "THOR"){ num = 24; sforce = "Preassure_THOR"; sdist[0] = "THOR_prx_1"; sdist[1] = "THOR_prx_2"; sdist[2] = "THOR_prx_3";}
  else if(name == "FRIGG"){ num = 25; sforce = "Preassure_FRIGG"; sdist[0] = "FRIGG_prx_1"; sdist[1] = "FRIGG_prx_2"; sdist[2] = "FRIGG_prx_3";}
  else if(name == "ODIN"){ num = 26; sforce = "Preassure_ODIN"; sdist[0] = "ODIN_prx_1"; sdist[1] = "ODIN_prx_2"; sdist[2] = "ODIN_prx_3";}
  else if(name == "LOKI"){ num = 27; sforce = "Preassure_LOKI"; sdist[0] = "LOKI_prx_1"; sdist[1] = "LOKI_prx_2"; sdist[2] = "LOKI_prx_3";}
  else printf("MODULE NAME INCORRECTO");

  ////////////// ACTUALIZACIÓN DE FUERZA
  modelData->ctrl[num] = goal_suctForce;

  ////////////// LECTURA DE SENSORES DE FUERZA
  int sensor_id = mj_name2id(model, mjOBJ_SENSOR, sforce);
  int sensor_start = model->sensor_adr[sensor_id];
  suctForce = modelData->sensordata[sensor_start];

  ////////////// LECTURA DE SENSORES DE distancia
  for(int i = 0; i< 3; i++){
    sensor_id = mj_name2id(model, mjOBJ_SENSOR, sdist[i]);
    sensor_start = model->sensor_adr[sensor_id];
    distance[i] =(int)(modelData->sensordata[sensor_start] * 1000.0);
  }
}

//Saca el valor de la fuerza que se está aplicando en la base de las ventosas

float SuctionSimulation::extractSuctData() {
    const char* name;
    // if(moduleName == "THOR") name = "pressure_sensor_THOR";
    // else if(moduleName == "ODIN") name = "pressure_sensor_ODIN";
    // else if(moduleName == "LOKI") name = "pressure_sensor_LOKI";
    // else if(moduleName == "FRIGG") name = "pressure_sensor_FRIGG";

    if(moduleName == "THOR") name = "Preassure_THOR";
    else if(moduleName == "ODIN") name = "Preassure_ODIN";
    else if(moduleName == "LOKI") name = "Preassure_LOKI";
    else if(moduleName == "FRIGG") name = "Preassure_FRIGG";

    int sensor_id = mj_name2id(model, mjOBJ_SENSOR, name);

    if (sensor_id == -1) {
        printf("Error: Sensor no encontrado.\n");
        return 0.0;
    }

    int sensor_start = model->sensor_adr[sensor_id];

    float touch_force = modelData->sensordata[sensor_start];

    return touch_force;
}

uint8_t * SuctionSimulation::getDistances(){
    uint8_t distancias[3]= {};
    const char* name;
    if(moduleName == "THOR") name = "THOR_prx_1";
    else if(moduleName == "ODIN") name = "ODIN_prx_1";
    else if(moduleName == "LOKI") name = "LOKI_prx_1";
    else if(moduleName == "FRIGG") name = "FRIGG_prx_1";

    int sensor_id = mj_name2id(model, mjOBJ_SENSOR, name);

    if (sensor_id == -1) {
        printf("Error: Sensor no encontrado.\n");
        return nullptr;
    }

    for(int i = 0; i < 3; i++){
    int sensor_start = model->sensor_adr[sensor_id + i];
    
    
    distancias[i] = modelData->sensordata[sensor_start];
    }
    return distancias;
}