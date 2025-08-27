
#pragma once
#include "RomerinMessage.h"
#include "RomBodyDefinitions.h"
//#include "RomMotorInfo.h"
//#include <RomJoints.h>
RomerinMsg executeMessage(const RomerinMsg &m);

//output Messages
//RomerinMsg motor_info_message(uchar_t m_id, MotorInfo &m_info);
//RomerinMsg analog_info_message(float, float , float, float);
//RomerinMsg fixed_motor_info_message(uchar_t m_id, MotorInfo &m_info);
RomerinMsg configuration_message();

RomerinMsg state_message(RomBodyState &state);
//RomerinMsg suction_cup_info_message(float p, float f, float t, uint8_t dist[3]);
RomerinMsg text_message(const char *text);
RomerinMsg name_message(const char *name);

//input Messages
void romerin_getConfiguration(const uchar_t *buffer);

 
