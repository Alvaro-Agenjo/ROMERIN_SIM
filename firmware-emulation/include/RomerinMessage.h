#pragma once
#define ROM_HEADER "\xaa\xaa"
#define ROM_HEADER_1 0xaa
#define ROM_HEADER_2 0xaa
#define ROM_NONE 0
#define ROM_MOTOR_INFO 0x01
#define ROM_ANALOG_INFO 0x02
#define ROM_TORQUE_ON 0x03
#define ROM_TORQUE_OFF 0x04
#define ROM_TEXT 0x05
#define ROM_SUCTION_CUP 0x06
#define ROM_GOAL_ANG 0x07
#define ROM_REBOOT_MOTOR 0x08
#define ROM_GET_FIXED_MOTOR_INFO 0x09
#define ROM_FIXED_MOTOR_INFO 0x10
#define ROM_GET_NAME 0x11
#define ROM_NAME 0x12
#define ROM_SCAN_WIFI 0x13
#define ROM_INIT_WIFI 0x14
#define ROM_GET_MOTOR_INFO 0x15
#define ROM_GET_CONFIG 0x16
#define ROM_CONFIG 0x17
#define ROM_STATE 0x18
#define ROM_SUCTIONCUP_INFO 0x19
#define ROM_TEST_A_MESSAGE 0x20 //messages only for testing purposes
#define ROM_TEST_B_MESSAGE 0x21 //messages only for testing purposes
#define ROM_VELOCITY_PROFILE 0x22
#define ROM_SET_MASTER_IP 0x23  //message to set the IP of the remote
#define ROM_GOAL_CURRENT 0x24
#define ROM_CONTROL_MODE 0x25
#define ROM_COMPACT_MODE_ON 0x26 
#define ROM_COMPACT_MODE_OFF 0x27 
#define ROM_GET_CONFIG_V2 0x30  //messsage to get the extended config params 
#define ROM_CONFIG_V2 0x31  //messsage to set the extended config params
#define ROM_SET_MOTOR_ZERO  0x32//message to configure the offset of a motor
#define ROM_COMPACT_ROBOT_DATA 0x40 //message for compact info 
#define ROM_SIMULATION 0xF0 //special message to encapsulate messages


#include <stdint.h>
/**UTILITY UNIONS FOR CONVERSIONS**/
typedef unsigned char uchar_t;



union double2byteConversor
{
                uchar_t bytes[8];
                double real;
};
union float2byteConversor
{
                uchar_t bytes[4];
                float real;
};
union int2byteConversor
{
                uchar_t bytes[4];
                int32_t integer;
};
union int16_2byteConversor
{
                uchar_t bytes[2];
                int16_t integer;
};
union uint16_2byteConversor
{
                uchar_t bytes[2];
                uint16_t integer;
};

//GET VALUES FROM MESSAGE
const uchar_t *romerin_getString(const uchar_t *buffer, char *cad, int max=200);
float romerin_getFloat(const uchar_t *buffer);
uint16_t romerin_getUInt16(const uchar_t *buffer);

//WRITE VALUE IN A MESSAGE
uchar_t romerin_writeDouble(uchar_t *buffer, double val);
uchar_t romerin_writeFloat(uchar_t *buffer, float val);
uchar_t romerin_writeUInt16(uchar_t *buffer, int val);
uchar_t romerin_writeString(uchar_t *buffer, const char *text);

//MESSAGE DEFINITION
#ifndef _WIN32
#define PACKED __attribute__((packed))
#else
#define PACKED
#endif
#pragma pack(1)
struct RomerinMsg
{

    union{
        uchar_t data[258];
        struct{
            uchar_t header[2];
            uchar_t size; //size = id + info
            uchar_t id;
            uchar_t info[254];
        }PACKED;
 
    };
    RomerinMsg(){header[0]=ROM_HEADER_1;header[1]=ROM_HEADER_2;size=0;}
    RomerinMsg(uchar_t cmd){header[0]=ROM_HEADER_1;header[1]=ROM_HEADER_2;size=1;id=cmd;}
    RomerinMsg(uchar_t cmd,uchar_t value){header[0]=ROM_HEADER_1;header[1]=ROM_HEADER_2;size=2;id=cmd;info[0]=value;}
    uchar_t &operator[](int ind){return data[ind];}
    static RomerinMsg &none(){static RomerinMsg _none;return _none;}    
};
#pragma pack()
//BASIC MESSAGE STRUCTURES TO EASY THE COMMUNICATION
struct MotorInfoData
{
 float position = 0;
 float velocity = 0;
 float intensity = 0;
 uint8_t temperature = 0;
 float voltage = 0;
 uint8_t status = 0;
 uint8_t operatingMode = 0;
 uint8_t id = 0;
};
struct FixedMotorInfoData
{
    float max_angle =0;
    float min_angle=10.0F;
};
struct AnalogInfoData{
    float vbat;
    float ibat;
    float vbus;
    float ibus;
};
struct SuctionCupInfoData{
    float pressure;
    float temperature;
    float force;
    uint8_t distance[3];
};
struct RobotCompactData{ //12+5*6+3+1+1=47 bytes
    int16_t angle[6]; //0.1 * deg
    uint8_t intensity[6]; // 20 * mAmp 
    uint8_t status[6];
    uint8_t volt[6]; //0.1Volts
    uint8_t vel[6];
    uint8_t temp[6];
    uint8_t sc_distance[3]; //mm
    uint8_t sc_force; //N up to 120 
    uint8_t sc_pressure; //0.05*Pascals up to 3000
    uint8_t sc_temp; //degs
};
//simplest chk8 for streams smaller than 255 bytes
inline uint8_t chk8(const uint8_t *buff, uint8_t len)
{
    unsigned int sum;       // nothing gained in using smaller types!
    for ( sum = 0 ; len != 0 ; len-- )sum += *(buff++);   
    return (uint8_t)sum;
}
//GENERIC MESSAGE READER
class MsgReader
{
    RomerinMsg mens;
    uchar_t index;
    public:
    MsgReader():index(0){}
      bool add_uchar(uchar_t data);
      RomerinMsg getMessage();

};

//INLINE DEFS
//READERS
inline float romerin_getFloat(const uchar_t *buffer){
    float2byteConversor aux;
        for(int i=0;i<4;i++)aux.bytes[i]=buffer[i];
        return aux.real;
}
inline uint16_t romerin_getUInt16(const uchar_t *buffer)
{
    uint16_2byteConversor aux;
        for(int i=0;i<2;i++)aux.bytes[i]=buffer[i];
        return aux.integer;
}
inline int16_t romerin_getInt16(const uchar_t *buffer)
{
    int16_2byteConversor aux;
        for(int i=0;i<2;i++)aux.bytes[i]=buffer[i];
        return aux.integer;
}
inline const uchar_t *romerin_getString(const uchar_t *buffer, char *cad, int max){
    int i=0;
    while((i<max)&&(buffer[i]!=0)){cad[i]=buffer[i];i++;}
    cad[i++]=0;
        return buffer+i;
}
inline MotorInfoData romerin_getMotorInfo(const uchar_t *buffer)
{
    MotorInfoData mot;
    mot.id=buffer[0];buffer++;
    mot.status=buffer[0];buffer++;
    mot.operatingMode=buffer[0];buffer++;
    mot.position=romerin_getFloat(buffer);buffer+=4;
    mot.velocity=romerin_getFloat(buffer);buffer+=4;
    mot.intensity=romerin_getFloat(buffer);buffer+=4;
    mot.temperature=buffer[0];buffer++;
    mot.voltage=romerin_getFloat(buffer);
    return mot;
}
inline FixedMotorInfoData romerin_getFixedMotorInfo(const uchar_t *buffer)
{
    FixedMotorInfoData mot;
    mot.max_angle=romerin_getFloat(buffer);buffer+=4;
    mot.min_angle=romerin_getFloat(buffer);buffer+=4;
    return mot;
}
inline AnalogInfoData romerin_getAnalogInfo(const uchar_t *buffer)
{
    AnalogInfoData analog;
    analog.vbat=romerin_getFloat(buffer);buffer+=4;
    analog.ibat=romerin_getFloat(buffer);buffer+=4;
    analog.vbus=romerin_getFloat(buffer);buffer+=4;
    analog.ibus=romerin_getFloat(buffer);buffer+=4;
    return analog;
}
inline SuctionCupInfoData romerin_getSuctionCupInfo(const uchar_t *buffer){
    SuctionCupInfoData sc;
    sc.pressure=romerin_getFloat(buffer);buffer+=4;
    sc.force=romerin_getFloat(buffer);buffer+=4;
    sc.temperature=romerin_getFloat(buffer);buffer+=4;
    sc.distance[0]=*(buffer++);
    sc.distance[1]=*(buffer++);
    sc.distance[2]=*(buffer++);
    return sc;
};
//WRITERS
inline uchar_t romerin_writeDouble(uchar_t *buffer, double val){
        double2byteConversor aux;
        aux.real=val;
        for(int i=0;i<8;i++)buffer[i]=aux.bytes[i];
        return 8;
}
inline uchar_t romerin_writeFloat(uchar_t *buffer, float val){
        float2byteConversor aux;
        aux.real=val;
        for(int i=0;i<4;i++)buffer[i]=aux.bytes[i];
        return 4;
}
inline uchar_t romerin_writeUInt16(uchar_t *buffer, int val)
{
    uint16_2byteConversor aux;
        aux.integer=val;
        for(int i=0;i<2;i++)buffer[i]=aux.bytes[i];
        return 2;
}
inline uchar_t romerin_writeInt16(uchar_t *buffer, int val)
{
    int16_2byteConversor aux;
        aux.integer=val;
        for(int i=0;i<2;i++)buffer[i]=aux.bytes[i];
        return 2;
}
inline uchar_t romerin_writeUChar(uchar_t *buffer, uchar_t val)
{
    buffer[0]=val;
        return 1;
}

inline uchar_t romerin_writeString(uchar_t *buffer, const char *text)
{
    uint8_t index=0;
    while((text[index]!=0)&&(index<250)){
        buffer[index]=text[index];index++;
    }
    buffer[index]=0;
return index+1;
}

//MESSAGE READER
inline bool MsgReader::add_uchar(uchar_t data)
{
 if((index==0)&&(data==ROM_HEADER_1)){mens[index++]=data;return false;}
 if((index==1)&&(data==ROM_HEADER_2)){mens[index++]=data;return false;}
 if(index==2){mens[index++]=data;return false;}
 if((index>2)&&(index-3<mens.size))
 { 
     mens[index++]=data;
     if(index-3==mens.size)return true;
    return false;
 }
 index=0;
 return false;
}
inline RomerinMsg MsgReader::getMessage(){
    index=0;
    return mens;
}
//MessagesConstructors

inline RomerinMsg romerinMsg_Torque(unsigned char motor_id,bool on=true){return RomerinMsg(on?ROM_TORQUE_ON:ROM_TORQUE_OFF,motor_id);}
inline RomerinMsg romerinMsg_SuctionCupPWM(unsigned char value){return RomerinMsg(ROM_SUCTION_CUP,(value<100?value:100));}
inline RomerinMsg romerinMsg_ServoReboot(unsigned char motor_id){return RomerinMsg(ROM_REBOOT_MOTOR,motor_id);}
inline RomerinMsg romerinMsg_ScanWiFi(){return RomerinMsg(ROM_SCAN_WIFI);}
inline RomerinMsg romerinMsg_InitWiFi(){return RomerinMsg(ROM_INIT_WIFI);}
inline RomerinMsg romerinMsg_GetConfiguration(){return RomerinMsg(ROM_GET_CONFIG);}
inline RomerinMsg romerinMsg_GetConfiguration_v2(){return RomerinMsg(ROM_GET_CONFIG_V2);}

inline RomerinMsg romerinMsg_VelocityProfile(unsigned char motor_id, float value){
    RomerinMsg m(ROM_VELOCITY_PROFILE,motor_id);
    romerin_writeFloat(m.info+1,value);m.size+=4;
    return m;
}
inline RomerinMsg romerinMsg_ServoGoalAngle(unsigned char motor_id, float value){
    RomerinMsg m(ROM_GOAL_ANG,motor_id);
    romerin_writeFloat(m.info+1,value);m.size+=4;
    return m;
}
inline RomerinMsg romerinMsg_ServoGoalCurrent(unsigned char motor_id, float value){
    RomerinMsg m(ROM_GOAL_CURRENT, motor_id);
    romerin_writeFloat(m.info+1,value);m.size+=4;
    return m;
}
inline RomerinMsg romerinMsg_ServoControlMode(unsigned char motor_id, int value){
    RomerinMsg m(ROM_CONTROL_MODE, motor_id);
    romerin_writeUInt16(m.info+1,value);m.size+=2;
    return m;
}
//the offset of the motor is adjusted so the current position becames the value
inline RomerinMsg romerinMsg_ServoOffset(unsigned char motor_id, uint16_t new_value){
    RomerinMsg m(ROM_SET_MOTOR_ZERO, motor_id);
    romerin_writeUInt16(m.info+1,new_value);m.size+=2;
    //a simple crc_8 to improve security
    m.info[3]=~((uint8_t)((new_value+motor_id)&0xFF));m.size++;
    return m;
}
//checks the integrity of ServoOffsetMessage
inline bool check_and_get_ServoOffset(const RomerinMsg &im, uchar_t &m_id, uint16_t &val){
    if(im.id!=ROM_SET_MOTOR_ZERO)return false;
    const uchar_t *punt=im.info;
    m_id=*(punt++);
    val=romerin_getUInt16(punt);punt+=2;
    uchar_t crc=*(punt++), crc_in=~((uchar_t)((val+m_id)&0xFF));
    return crc==crc_in;
}
//encapsulates messages
inline RomerinMsg romerinMsg_simulate(unsigned char id, const RomerinMsg &im){
    RomerinMsg m(ROM_SIMULATION, id);
    for(int i=0;i<im.size+1;i++)m.info[i+1]=im.data[i+2];
    m.size=im.size+3;
    return m;
}
//retrieves the original message
inline RomerinMsg get_simulatedMsg(const RomerinMsg &im){
    if(im.id!=ROM_SIMULATION)return RomerinMsg::none();
    RomerinMsg m{};
    for(int i=0;i+2<im.size;i++)m.data[i+2]=im.info[i+1];
    return m;
}

//The message includes the robot compact data and a chk8 at the end.
inline RomerinMsg romerinMsg_robot_compact_data(const RobotCompactData &data){
    RomerinMsg msg;
    uchar_t *punt=msg.info;
    msg.id = ROM_COMPACT_ROBOT_DATA;
    for(int i=0;i<6;i++){
        punt+=romerin_writeUInt16(punt,data.angle[i]);
        punt+=romerin_writeUChar(punt,data.intensity[i]);
        punt+=romerin_writeUChar(punt, data.status[i]);
        punt+=romerin_writeUChar(punt, data.volt[i]);
        punt+=romerin_writeUChar(punt, data.vel[i]);
        punt+=romerin_writeUChar(punt, data.temp[i]);
    }
    for(int i=0;i<3;i++)punt+=romerin_writeUChar(punt, data.sc_distance[i]);
    punt+=romerin_writeUChar(punt, data.sc_force);
    punt+=romerin_writeUChar(punt,data.sc_pressure);
    punt+=romerin_writeUChar(punt,data.sc_temp);
    punt+=romerin_writeUChar(punt, chk8(msg.info, punt-msg.info));
    msg.size=(punt-msg.info)+1;
    return msg;
}
inline bool romerin_get_robot_compact_data(const RomerinMsg &m, RobotCompactData &data)
{
    if(m.id!=ROM_COMPACT_ROBOT_DATA)return false;
    const uchar_t *punt=m.info;
    for(int i = 0;i<6;i++){
        data.angle[i]=romerin_getUInt16(punt);punt+=2;
        data.intensity[i]=*(punt++);
        data.status[i]=*(punt++);
        data.volt[i]=*(punt++);
        data.vel[i]=*(punt++);
        data.temp[i]=*(punt++);
    }
    for(int i=0;i<3;i++)data.sc_distance[i]=*(punt++);
    data.sc_force=*(punt++);
    data.sc_pressure=*(punt++);
    data.sc_temp=*(punt++);
    uint8_t chk = chk8(m.info, punt-m.info);
    if(chk!=*punt) return false;
    return true;
}
inline MotorInfoData compact_data2motor_info_data(const RobotCompactData &data, uint8_t id)
{
    MotorInfoData mot;
    mot.id=id;
    mot.status=data.status[id];
    mot.position=data.angle[id]/10.0F;
    mot.velocity=data.vel[id];
    mot.intensity=data.intensity[id]*20;
    mot.temperature=data.temp[id];
    mot.voltage=data.volt[id]/10.0F;
    return mot;
}
inline SuctionCupInfoData compact_data2sc_info_data(const RobotCompactData &data)
{
    SuctionCupInfoData scid{};
    for(int  i=0;i<3;i++)scid.distance[i]=data.sc_distance[i];
    scid.pressure=20*data.sc_pressure;
    scid.force=data.sc_force;
    scid.temperature=data.sc_temp;
    return scid;

}
