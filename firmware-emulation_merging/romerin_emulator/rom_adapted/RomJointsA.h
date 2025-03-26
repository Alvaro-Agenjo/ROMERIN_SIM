#pragma once


#include <utility>
#include "RomMotorInfoA.h"
#include "RomerinDefinitionsA.h"
#include "RomerinMessage.h"

const uint8_t DXL_ID_CNT = 6;
const uint8_t ROM_MOTOR_IDS[] = {1, 2, 3, 4, 5, 6};
#include <time.h>
//dxl emulation
struct MotorEmulation{
    uint8_t id;
    bool torque{};
    uint8_t status{};
    uint8_t control_mode;
    float min;
    float max;
    float temp{25};
    float volt{};
    float goal_angle{};
    float goal_current{};
    float goal_velocity{};
    float goal_acceleration{};
    float angle{}, velocity{}, current{}, acceleration{};
    void loop(); //emulation of the dynemixel control loop
    unsigned long millis(){
        return static_cast<long>(clock()/ (0.001*CLOCKS_PER_SEC));
    }
    unsigned long ltime=0;
};

class RomJoints
{

    void setupMotor(int8_t id);
    MotorEmulation dynamixels[DXL_ID_CNT];
    RomDefs *romdefs=nullptr;
public:
    RomJoints(){}
    void setup(RomDefs *moddefs);
    void loop();
    void reboot(int8_t id);
    void torque(int8_t id, bool on_off=true);
    void controlMode(int8_t id, uint8_t control_mode);
    void setGoalCurrent(int8_t id,float value);
    void setGoalAngle(int8_t id,float ang);
    void setVelocity(int8_t id, float vel);
    void setAcceleration(int8_t id, float acc);
    void fill_compact_data(RobotCompactData &data);
    void initial_angle(int8_t id, float angle);
    int mapIds(int id);

    MotorInfo motors[6];


};
