#pragma once
#include <stdint.h>



//constant definitions
#define TIME_OUT_WIFI_MASTER 5000  //after 5 seconds without wifi messages, no output messages will be sent
//configurable variables. v3: includes the compact mode
#define ROM_BODY_CONF_FILE_VERSION 1



struct RomBodyDefs{
    static char MODULE_NAME[100]; //"THOR"
    static uint8_t IP_ADDRESS[4]; //180
    static uint8_t GATEWAY_ADDRESS[4]; //180
    static uint8_t SUBNET_MASK[4]; //180
    static char WIFI_SSID[50]; //"ROM_WIFI"
    static char WIFI_KEY[50]; //"temp0ral"

    static uint16_t mass; //grams

    static int16_t position[3]; //mm position of the body sensor respect the body center
    static int16_t orientation[3]; //degs

    

    static bool readConfiguration();
    static bool writeConfiguration();
};

//Romerin Module State
struct RomBodyState{
    bool connected_to_wifi=false;
    bool power_enabled=false;
    uint8_t cicle_time;
};

// Set the wifi default data (1 if set by default)
#define SET_DEFAULT_WIFI_DATA 0


