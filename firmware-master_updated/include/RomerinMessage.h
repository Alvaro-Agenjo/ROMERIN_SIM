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
            uchar_t size; //size id + info
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
//RomerinMsg romerinMsg_sendConfiguration(ConfigurationInfo &conf);
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
