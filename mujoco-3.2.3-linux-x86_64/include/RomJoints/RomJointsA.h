#pragma once

#include <unistd.h>

#include <utility>
#include "Kinematics/romkin.h"
#include "RomJoints/RomMotorInfoA.h"
#include "RomJoints/RomerinDefinitionsA.h"
#include "mujoco/mujoco.h"
#include "GLFW/glfw3.h"

#include <QJsonObject>
#include <QJsonArray>

const uint8_t DXL_ID_CNT = 6;
const uint8_t ROM_MOTOR_IDS[] = {1, 2, 3, 4, 5, 6};
#include <time.h>

struct SuctionSimulation{
    mjData* modelData;
    mjModel* model;
    QString moduleName;

    float suctForce;
    float goal_suctForce;

    bool setup = false;

    void setModel(mjModel* m, mjData* d, QString n);
    void setSetup(bool val){this->setup = val;}
    //void setSuctForceInit(float num){this->goal_suctForce = num;}
    bool getSetup(){return setup;}
    void loop(QString name);
    float getSuctForce(){return this->suctForce;}

    float extractSuctData();

};

struct MotorSimulation{

    mjData* modelData;
    mjModel* model;
    QString moduleName;
    _RomKin<float> romkin;

    float posref = 0;

    bool setup = false;
    uint8_t id;
    bool torque{};
    uint8_t status{};
    uint8_t control_mode;
    float mass = 0;
    float initialAngle = 0;
    float min;
    float max;
    float torqueForce;
    float length;
    float temp{25};
    float volt{};
    float goal_angle{};
    float Ang3, Ang4, Ang5;
    float goal_ang3, goal_ang4, goal_ang5;
    float q3 = 0, q4 = 0, q5 = 0;
    //float goal_current{};
    float goal_velocity{};
    float maxVelRadS = 4.8;
    float goal_acceleration{};
    float angle{}, velocity{}, current = 5.0, acceleration{};
    float angle_old = 0;
    float ep_sum{};
    float ep_old{};//Error de posición anterior
    float ev_old{};//Error de velocidad anterior
    float e_integral{};//Error integral anterior
    float m2q(float goalAng); // Convierte el ángulo de simulación a interfaz
    float q2m(float angle);////////////////////////////////
    void loop(int number); //simulation of the dynemixel control loop
    void setModel(mjModel* m, mjData* d, QString n);
    void setSetup(bool val){this->setup = val;}
    float getAngle(int number);
    float getVelocity(int number);
    bool getSetup(){return setup;}
    //float getGoalAngle(){return goal_angle;}
    float getTorqueForce(){return torqueForce;}
    float getGoal_angle(){return goal_angle;}
    float getInitialAngle(){return initialAngle;}
    unsigned long millis(){
        return static_cast<long>(clock()/ (0.001*CLOCKS_PER_SEC));
    }
    unsigned long ltime=0;
};

class RomJoints
{
    mjModel* model;
    mjData* modelData;
    QString moduleName;
    float mass[6];
    MotorSimulation dynamixels[DXL_ID_CNT];
    SuctionSimulation suctioncup;
    RomDefs *romdefs=nullptr;
    _RomKin<float> romkin;
    float goal_ang3, goal_ang4, goal_ang5;
    float Ang3, Ang4, Ang5;
    float q3 = 0, q4 = 0, q5 = 0;

    void setupMotor(int8_t id);
    
public:
    RomJoints(){}
    void setup(RomDefs *moddefs, mjModel* m, mjData* d);
    void loop(QString module_name);
    void reboot(int8_t id);
    void torque(int8_t id, bool on_off=true);
    void controlMode(int8_t id, uint8_t control_mode);
    //void setGoalCurrent(int8_t id,float value);
    void initial_angle(int8_t id, float angle);
    void setMass(int8_t id, float angle);
    void setGoalAngle(int8_t id,float ang);
    void setVelocity(int8_t id, float vel);
    void setAcceleration(int8_t id, float acc);
    void setTurbine(int8_t force);
    float q2m(int id);/////////////////////////////////////////////////////////////////
    int mapIds(int id);
    MotorSimulation* getMotorSim(int i){return &dynamixels[i];}
    MotorInfo motors[6];
    void setGoalAnglesConversions();
};

