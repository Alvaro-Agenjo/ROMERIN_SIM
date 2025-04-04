#pragma once
#include <stdint.h>

//ADAPTATION: static removed from RomDefs and read and write removed also

//constant definitions
#define TIME_OUT_WIFI_MASTER 5000  //after 5 seconds without wifi messages, no output messages will be sent
//configurable variables
#define ROM_CONF_FILE_VERSION 2

#define ROM_CONF_COMPACT 0x01
#define ROM_CONF_DISABLE_BT 0x10

struct MinMax{int16_t min,max;};

struct RomDefs{
     char MODULE_NAME[100]{}; //"THOR"
    uint8_t IP_ADDRESS[4]{}; //180
    uint8_t GATEWAY_ADDRESS[4]{}; //180
    uint8_t SUBNET_MASK[4]{}; //180
    char WIFI_SSID[50]{}; //"ROM_WIFI"
    char WIFI_KEY[50]{}; //"temp0ral"
    uint16_t lenghts[6]{};
    MinMax limits[6]{};
    int8_t offset[6];
    uint8_t vel_profile[6]{};
    uint8_t acc_profile[6]{};
    int16_t position[3]{}; //mm
    int16_t orientation[3]{}; //degs
    uint8_t init_configuration; //0x01 compact mode;  0x10 BT disabled;
};
/*struct RomDefs{
    static char MODULE_NAME[100]; //"THOR"
    static uint8_t IP_ADDRESS[4]; //180
    static uint8_t GATEWAY_ADDRESS[4]; //180
    static uint8_t SUBNET_MASK[4]; //180
    static char WIFI_SSID[50]; //"ROM_WIFI"
    static char WIFI_KEY[50]; //"temp0ral"
    static uint16_t lenghts[6];
    static MinMax limits[6];
    static int8_t offset[6];
    static uint8_t vel_profile[6];
    static uint8_t acc_profile[6];

    static bool readConfiguration();
    static bool writeConfiguration();
};*/

//Romerin Module State
struct RomState{
    bool connected_to_wifi=false;
    bool connected_to_bt=false;
    bool connected_to_can=false;
    bool power_enabled=false;
    bool compact_mode=false; //the robot send the data in a compact way
    uint8_t cicle_time;
};


//DINAMIXEL
#define ROM_DXL_RX 3
#define ROM_DXL_TX 1
#define ROM_DXL_DIR_PIN 4  //pin select
#define ROM_DXL_BAUD_RATE 1000000

//Default lengths in mm(check documentation)
#define ROM_LENGHT_1 68
#define ROM_LENGHT_2 236
#define ROM_LENGHT_3 15
#define ROM_LENGHT_4 280
#define ROM_LENGHT_5 22
#define ROM_LENGHT_6 87

// #define ROM_MOTOR_1_MIN_MAX {0, 360}
#define ROM_MOTOR_2_MIN_MAX {70, 290}
#define ROM_MOTOR_3_MIN_MAX {70, 290}
#define ROM_MOTOR_4_MIN_MAX {55, 260}
#define ROM_MOTOR_5_MIN_MAX {0, 360}
#define ROM_MOTOR_6_MIN_MAX {0, 360}
#define ROM_MOTOR_7_MIN_MAX {0, 360}

#define BLOCKING_THRESHOLD 5.0  // In degrees


#define PWM_CHANNEL_TURBINA 1
#define PWM_CHANNEL_BUZZER 2

#define BUZZER 25
#define PWM_TURBINA 23

#define LED_INFO1_R 26
#define LED_INFO2_R 15
#define LED_INFO3_R 27
#define LED_INFO4_R 13

#define RELE_A 12
#define RELE_B 14

//tension Bateria
#define V_BAT 33
#define ADC_V_BAT 4 

//consumo Ventosa
#define I_BAT 35
#define ADC_I_BAT 7 

//consumo 12V
#define I_12V 32
#define ADC_I12V 6 

//SEÑAL DIGITAL informa limitacion de corriente
#define I_INFO_LIM_CORRIENTE 34

// Pressure sensor selection (0-Adafruit_BMP085, 1-MS5611 (Last version))
#define PRESSURE_SENSOR_SELECTION 1

// Suction cup sensor activation
#define ENABLE_PRESSURE_SENSOR 1
#define ENABLE_VL6180 1

// Set the wifi default data (1 if set by default)
#define SET_DEFAULT_WIFI_DATA 0

// Firmware version
#if PRESSURE_SENSOR_SELECTION == 0
    #define FIRWARE_VERSION "1a"
#else
    #define FIRWARE_VERSION "1b"
#endif
