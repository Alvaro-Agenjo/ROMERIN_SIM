
#pragma once
#include <Arduino.h>
#include "RomerinDefinitions.h"
#include "RomUtils.h"
#define N_FILTER 10

/** @class RomAnalog
*   @brief Romerin class that handles de analog reading of internal intensities and voltages.
*
*/
class RomAnalog{
    class Calibration{
        float pointAD,pointReal,m;
        public:
          /** @brief utility class for adapting the values 
           *  @param min real value corresponding to the 0 V reading
           *  @param max 
           */

          void configure(float ad1, float real1,  float ad2, float real2){
              pointAD=ad1;
              pointReal=real1;
              m=(real2-real1)/(ad2-ad1);
          }
          Calibration(float ad1, float real1,  float ad2, float real2){
              configure(ad1, real1,  ad2, real2);
          }
          float operator()(float raw){
              return pointReal+(raw-pointAD)*m;

          }

    }cal_vbat, cal_ibat, cal_i12v;

    int vbat=0; 
    int ibat=0;
    int i12v=0;
    filter<int,N_FILTER,long> vbatf;
    filter<int,N_FILTER,long> ibatf;
    filter<int,N_FILTER,long> i12vf;


   public:
   RomAnalog():cal_vbat(  0.4,2.2 , 2.67,18.7),
               cal_ibat(0,0 , 3,15),
               cal_i12v(0,0 , 3,15){}

   void setup()
   {
        pinMode(V_BAT,     INPUT);
        pinMode(I_12V,     INPUT);
        pinMode(I_BAT,   INPUT);
        pinMode(I_INFO_LIM_CORRIENTE,   INPUT); 
   }
   void loop()
   {
       vbat=vbatf.add(analogRead(V_BAT));
       ibat=ibatf.add(analogRead(I_BAT));
       i12v=i12vf.add(analogRead(I_12V));
   }
    float getBatteryVolt(){return cal_vbat((vbat*3.3)/4096.0);}
    float getBatteryAmp() {return cal_ibat((ibat*3.3)/4096.0);}
    float get12vAmp() {return cal_i12v((i12v*3.3)/4096.0);}
};