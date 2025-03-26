#pragma once

#include "RomerinDXL.h"
// #include <chrono>
#include <utility>
#include "RomMotorInfo.h"
#include "RomerinMessage.h"


/** @class RomerinJoints
*   @brief Romerin class that handles all the dynamixel motors.
*   setup: joint initialiaztions
*   loop: ciclic tasks that include the polling of motor info. 
*
*/
class RomJoints
{
    Dynamixel2Arduino dxl;
    RomerinDXLPortHandler dxl_port;
    void setupMotor(int8_t id);
public:
    RomJoints(){}
    void setup();
    void loop();
    void reboot(int8_t id);
    void torque(int8_t id, bool on_off=true);
    void controlMode(int8_t id, uint8_t control_mode);
    void setGoalCurrent(int8_t id,float value);
    void setGoalAngle(int8_t id,float ang);
    void setVelocity(int8_t id, float vel);
    void setAcceleration(int8_t id, float acc);
    void fill_compact_data(RobotCompactData &data);
    int mapIds(int id);

    MotorInfo motors[6];
    int last_dxl_error;
    int errors=0;
    int cicleNumber=0;
};