
#include "RomerinMsg.h"

RomerinMsg motor_info_message(uchar_t m_id, MotorInfo &m_info)
{
    RomerinMsg msg;
    uchar_t *punt=msg.info;
    msg.id = ROM_MOTOR_INFO;
    punt+=romerin_writeUChar(punt, m_id);
    punt+=romerin_writeUChar(punt, m_info.torqueEnabled?m_info.status|0x80:m_info.status&0x7F);
    punt+=romerin_writeUChar(punt, m_info.operatingMode);
    punt+=romerin_writeFloat(punt,m_info.position);
    punt+=romerin_writeFloat(punt,m_info.velocity);
    punt+=romerin_writeFloat(punt,m_info.intensity);
    punt+=romerin_writeUChar(punt,m_info.temperature);
    punt+=romerin_writeFloat(punt,m_info.voltage);
    msg.size=(punt-msg.info)+1;
    return msg;
}
RomerinMsg suction_cup_info_message(float p, float f, float t, uint8_t dist[3])
{
    RomerinMsg msg;
    uchar_t *punt=msg.info;
    msg.id = ROM_SUCTIONCUP_INFO;
    punt+=romerin_writeFloat(punt,p);
    punt+=romerin_writeFloat(punt,f);
    punt+=romerin_writeFloat(punt,t);
    punt+=romerin_writeUChar(punt, dist[0]);
    punt+=romerin_writeUChar(punt, dist[1]);
    punt+=romerin_writeUChar(punt, dist[2]);
    msg.size=(punt-msg.info)+1;
    return msg;  
}
RomerinMsg analog_info_message(float vbat, float ibat, float vbus, float ibus)
{
    RomerinMsg msg;
    uchar_t *punt=msg.info;
    msg.id = ROM_ANALOG_INFO;
    punt+=romerin_writeFloat(punt,vbat);
    punt+=romerin_writeFloat(punt,ibat);
    punt+=romerin_writeFloat(punt,vbus);
    punt+=romerin_writeFloat(punt,ibus);
    msg.size=(punt-msg.info)+1;
    return msg;  
}
RomerinMsg fixed_motor_info_message(uchar_t m_id, MotorInfo &m_info)
{
    RomerinMsg msg;
    uchar_t *punt=msg.info;
    msg.id = ROM_FIXED_MOTOR_INFO;
    punt+=romerin_writeUChar(punt, m_id);
    punt+=romerin_writeFloat(punt,m_info.max_angle);
    punt+=romerin_writeFloat(punt,m_info.min_angle);
    msg.size=(punt-msg.info)+1;
    return msg;
}
RomerinMsg text_message(const char *text){
    RomerinMsg msg; 
    uchar_t *punt=msg.info;
    msg.id = ROM_TEXT;
    punt+=romerin_writeString(punt,text);
    msg.size=(punt-msg.info)+1;
    return msg;
}
RomerinMsg name_message(const char *name){
    RomerinMsg msg; 
    uchar_t *punt=msg.info;
    msg.id = ROM_NAME;
    punt+=romerin_writeString(punt,name);
    msg.size=(punt-msg.info)+1;
    return msg; 
}
RomerinMsg state_message(RomState &state)
{
    RomerinMsg msg; 
    uchar_t *punt=msg.info, aux=0;
    msg.id = ROM_STATE;
    if(state.connected_to_bt)aux|=0x01;
    if(state.connected_to_wifi)aux|=0x02;
    if(state.connected_to_can)aux|=0x04;
    if(state.power_enabled)aux|=0x08;
    punt+=romerin_writeUChar(punt,aux);
    punt+=romerin_writeUChar(punt,state.cicle_time);
    msg.size=(punt-msg.info)+1;
    return msg;  

}

RomerinMsg configuration_message(){
    RomerinMsg msg; 
        uchar_t *punt=msg.info;
    msg.id = ROM_CONFIG;
    for(int i=0;i<4;i++)punt+=romerin_writeUChar(punt, RomDefs::IP_ADDRESS[i]);
    for(int i=0;i<4;i++)punt+=romerin_writeUChar(punt, RomDefs::GATEWAY_ADDRESS[i]);
    for(int i=0;i<4;i++)punt+=romerin_writeUChar(punt, RomDefs::SUBNET_MASK[i]);

    punt+=romerin_writeString(punt,RomDefs::MODULE_NAME);
    punt+=romerin_writeString(punt,RomDefs::WIFI_SSID);
    punt+=romerin_writeString(punt,RomDefs::WIFI_KEY);

    msg.size=(punt-msg.info)+1;
    return msg;
}
void romerin_getConfiguration(const uchar_t *buffer)
{
    for(int i=0;i<4;i++)RomDefs::IP_ADDRESS[i]=*(buffer++);
    for(int i=0;i<4;i++)RomDefs::GATEWAY_ADDRESS[i]=*(buffer++);
    for(int i=0;i<4;i++)RomDefs::SUBNET_MASK[i]=*(buffer++);
    buffer=romerin_getString(buffer,RomDefs::MODULE_NAME,99);
    buffer=romerin_getString(buffer,RomDefs::WIFI_SSID,49);
    buffer=romerin_getString(buffer,RomDefs::WIFI_KEY,49); 
    RomDefs::writeConfiguration();
}
