//ADAPTATION: included RomDefs as non static and as parameter of the funtions thant made use of it
#pragma once
#include "Comunicacion/RomerinMessage.h"
#include "RomJoints/RomerinDefinitionsA.h"
#include "RomJoints/RomMotorInfoA.h"

//RomerinMsg executeMessage(const RomerinMsg &m);

//output Messages
RomerinMsg motor_info_message(uchar_t m_id,const MotorInfo &m_info);
RomerinMsg analog_info_message(float, float , float, float);
RomerinMsg fixed_motor_info_message(uchar_t m_id, MotorInfo &m_info);
RomerinMsg configuration_message(RomDefs &romdefs);
RomerinMsg configuration_message_v2(RomDefs &romdefs);
RomerinMsg state_message(RomState &state);
RomerinMsg suction_cup_info_message(float p, float f, float t, uint8_t dist[3]);
RomerinMsg suction_cup_info_message(const SuctionCupInfo & sc_info);
RomerinMsg text_message(const char *text);
RomerinMsg name_message(const char *name);

//input Messages
void romerin_getConfiguration(const uchar_t *buffer, RomDefs &romdefs);
void romerin_getConfiguration_v2(const uchar_t *buffer, RomDefs &romdefs);


