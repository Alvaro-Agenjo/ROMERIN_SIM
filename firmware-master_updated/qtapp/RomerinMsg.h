#ifndef ROMERINMSG_H
#define ROMERINMSG_H

#define ROM_HEADER "\xaa\xaa"
#define ROM_NONE 0x00
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

typedef unsigned char uchar_t;

union float2byteConversor
{
        uchar_t bytes[4];
        float real;
};
inline float getFloatFromQByte(QByteArray &m, int pos)
{
    float2byteConversor aux;
    for(int i=0;i<4;i++)aux.bytes[i]=m[pos+i];
    return aux.real;
}
inline void putFloatOnQByte(float value, QByteArray &m, int pos)
{
    float2byteConversor aux;
    aux.real=value;
    for(int i=0;i<4;i++)m[pos+i]=aux.bytes[i];
}


#endif // ROMERINMSG_H
