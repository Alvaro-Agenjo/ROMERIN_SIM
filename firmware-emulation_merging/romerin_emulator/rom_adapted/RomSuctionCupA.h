#pragma once

#include "RomerinDefinitionsA.h"



#define SUCTION_CUP_AREA 254 //CM2
#define DEFAULT_ATTACH_FORCE 60 //N
#define DEFAULT_MIN_DISTANCE 35 //Glass is a problem... it is detected farther
#define DEFAULT_MAX_DISCREPANCE 35
class RomProximityArray
{
  public:
    uint8_t distances[3]{255,255,255};


   void setup();
   void loop();
   uint8_t getDistance(int i){return distances[i];}
   uint8_t *getDistances(){return distances;}
   uint8_t getMaxDifference();
   uint8_t getMax();
};
class RomSuctionCup
{
    //HIGH LEVEL COMMAND VARS
    enum modes{NORMAL, ATTACH_MODE} _mode = NORMAL;
    float _goal_force=DEFAULT_ATTACH_FORCE;
    uint8_t _min_distance=DEFAULT_MIN_DISTANCE;
    uint8_t _max_discrepance=DEFAULT_MAX_DISCREPANCE;
    

    //Low level VARS
    RomProximityArray distance_sensor;

    float pressure=0;
    int32_t atm_pressure=0;
    float temperature=27.3;
    float force=0;
    bool attached=false;
    uint8_t tgoal=0;
    uint8_t tvalue=0;

    public:
        //RomSuctionCup();

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
        void setTurbine(uint8_t val){tgoal=val>100?100:val;}

        //emulations
        void emulate_close(bool near){
            for(auto &d:distance_sensor.distances)d=near?10:255;
        }
        bool is_close_and_aligned(){
            uint8_t max=distance_sensor.getMax();
            uint8_t dif=distance_sensor.getMaxDifference();
            return (max<_min_distance)&&(dif<_max_discrepance);
        }
};
