
#include "RomerinMsgA.h"

RomerinMsg motor_info_message(uchar_t m_id, const MotorInfo &m_info)
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
    if(state.compact_mode)aux|=0x10;
    punt+=romerin_writeUChar(punt,aux);
    punt+=romerin_writeUChar(punt,state.cicle_time);
    msg.size=(punt-msg.info)+1;
    return msg;  

}

RomerinMsg configuration_message(RomDefs &romdefs){
    RomerinMsg msg; 
        uchar_t *punt=msg.info;
    msg.id = ROM_CONFIG;
    for(int i=0;i<4;i++)punt+=romerin_writeUChar(punt, romdefs.IP_ADDRESS[i]);
    for(int i=0;i<4;i++)punt+=romerin_writeUChar(punt, romdefs.GATEWAY_ADDRESS[i]);
    for(int i=0;i<4;i++)punt+=romerin_writeUChar(punt, romdefs.SUBNET_MASK[i]);

    punt+=romerin_writeString(punt,romdefs.MODULE_NAME);
    punt+=romerin_writeString(punt,romdefs.WIFI_SSID);
    punt+=romerin_writeString(punt,romdefs.WIFI_KEY);

    msg.size=(punt-msg.info)+1;
    return msg;
}
void romerin_getConfiguration(const uchar_t *buffer,RomDefs &romdefs)
{
    for(int i=0;i<4;i++) romdefs.IP_ADDRESS[i]=*(buffer++);
    for(int i=0;i<4;i++) romdefs.GATEWAY_ADDRESS[i]=*(buffer++);
    for(int i=0;i<4;i++) romdefs.SUBNET_MASK[i]=*(buffer++);
    buffer=romerin_getString(buffer, romdefs.MODULE_NAME,99);
    buffer=romerin_getString(buffer, romdefs.WIFI_SSID,49);
    buffer=romerin_getString(buffer, romdefs.WIFI_KEY,49);
}



RomerinMsg configuration_message_v2(RomDefs &romdefs){
    RomerinMsg msg; 
    uchar_t *punt=msg.info;
    msg.id = ROM_CONFIG_V2;

    for(int i=0;i<6;i++){ //6*(2+2+2+1+1+1)
        punt+=romerin_writeUInt16(punt,  romdefs.lenghts[i]);
        punt+=romerin_writeUInt16(punt,  romdefs.limits[i].min);
        punt+=romerin_writeUInt16(punt,  romdefs.limits[i].max);
        punt+=romerin_writeUChar(punt, romdefs.offset[i]);
        punt+=romerin_writeUChar(punt, romdefs.vel_profile[i]);
        punt+=romerin_writeUChar(punt, romdefs.acc_profile[i]);
    }
    for(int i=0;i<3;i++)punt+=romerin_writeInt16(punt, romdefs.position[i]);
    for(int i=0;i<3;i++)punt+=romerin_writeInt16(punt, romdefs.orientation[i]);
    punt+=romerin_writeUChar(punt,romdefs.init_configuration);

    msg.size=(punt-msg.info)+1;
    return msg;
}
void romerin_getConfiguration_v2(const uchar_t *buffer, RomDefs &romdefs)
{
    for(int i=0;i<6;i++){
        romdefs.lenghts[i]=romerin_getUInt16(buffer);buffer+=2;
        romdefs.limits[i].min=romerin_getUInt16(buffer);buffer+=2;
        romdefs.limits[i].max=romerin_getUInt16(buffer);buffer+=2;
        romdefs.offset[i]=*(buffer++);
        romdefs.vel_profile[i]=*(buffer++);
        romdefs.acc_profile[i]=*(buffer++);
    }
    for(int i=0;i<3;i++){romdefs.position[i]=romerin_getInt16(buffer);buffer+=2;}
    for(int i=0;i<3;i++){romdefs.orientation[i]=romerin_getInt16(buffer);buffer+=2;}
    romdefs.init_configuration=*(buffer++);
}
