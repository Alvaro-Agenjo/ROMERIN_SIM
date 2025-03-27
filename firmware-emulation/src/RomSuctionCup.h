#pragma once
#include <Arduino.h>
#include "RomerinDefinitions.h"
#include <Servo.h>
#include <Wire.h>
#include "RomFilter.h"
#include "RomVL6180X.h"

#if PRESSURE_SENSOR_SELECTION == 0
    #include <Adafruit_BMP085.h>
#else
    #include "MS5611.h"
#endif

#define SUCTION_CUP_AREA 254 //CM2
#define DEFAULT_ATTACH_FORCE 60 //N
#define DEFAULT_MIN_DISTANCE 35 //Glass is a problem... it is detected farther
#define DEFAULT_MAX_DISCREPANCE 35

class RomSuctionCup
{
    //HIGH LEVEL COMMAND VARS
    enum modes{NORMAL, ATTACH_MODE} _mode = NORMAL;
    float _goal_force=DEFAULT_ATTACH_FORCE;
    uint8_t _min_distance=DEFAULT_MIN_DISTANCE;
    uint8_t _max_discrepance=DEFAULT_MAX_DISCREPANCE;
    

    //Low level VARS
    RomProximityArray distance_sensor;
    #if PRESSURE_SENSOR_SELECTION == 0
        Adafruit_BMP085 bmp;
    #else
        MS5611 pressure_sensor{0x77};
    #endif
    Servo turbina;
    float pressure=0;
    float atm_pressure=0;
    float temperature=0;
    float force=0;
    bool attached=false;
    uint16_t cicle=65535;
    uint8_t tgoal=0;
    uint8_t tvalue=0;
    filter<float,5> pfilter;
    bool i2creading=true;
    public:
        //RomSuctionCup();
        MS5611 &test_psensor=pressure_sensor; //test to accesss presure sensor REMOVE
        void enableI2Creading(bool set=true){i2creading=set;}
        void setup();
        void loop();
        void configureAttach(float goal_force, uint8_t min_distance, uint8_t max_discrepance );
        void attach();
        bool isAttached();
        void dettach();
        float getRelPressure(){return pressure;}
        float getAtmPressure(){return atm_pressure;}
        float getForce(){return force;}
        float getTemperature(){return temperature;}
        uint8_t *getDistances(){return distance_sensor.getDistances();}
        void setTurbine(uint8_t val){
            if(val>100)val=100;
            tgoal=val;
        }
};