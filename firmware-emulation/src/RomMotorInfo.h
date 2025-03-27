#pragma once
#include <stdint.h>
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
