#pragma once

#include "RomerinDXL.h"
// #include <chrono>
#include <utility>

struct MotorInfo
{
 float position = 0;
 float velocity = 0;
 float intensity = 0;
 uint8_t temperature = 0;
 float voltage = 0;
 uint8_t status = 0;
 uint8_t operatingMode = 0;
 bool torqueEnabled = false;
 float min_angle = 0.0F;
 float max_angle = 360.0F;
};

// struct TrajectoryData {
//     std::vector<float> time;
//     std::vector<float> position;
//     std::vector<float> velocity;
//     std::vector<float> acceleration;
//     std::vector<float> torque;
// };

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
    // void setTrajectory(int8_t id, const TrajectoryData& traj);
    int mapIds(int id);

    MotorInfo motors[6];
    int last_dxl_error;
    int errors=0;
    int cicleNumber=0;
    // std::vector<TrajectoryData> trajectory_;
    // std::vector<std::chrono::time_point<std::chrono::system_clock>> trajectoryStart;
};