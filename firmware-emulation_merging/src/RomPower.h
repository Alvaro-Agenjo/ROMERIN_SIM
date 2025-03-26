#pragma once
#include <Arduino.h>
#include "RomerinDefinitions.h"
#include "RomAnalog.h"
//Romerin Class whose rol it to control de power modes

class RomPower
{
    enum POWER_MODE{ISOLATED, SHARE, CHARGE, BUS_ONLY} mode;
    RomAnalog analog;
    void powerPolicy();
public:
    void setup();
    void loop();
    void setMode(POWER_MODE mode=ISOLATED);
      
    float getBatteryVolt(){return analog.getBatteryVolt();}
    float getBatteryAmp() {return analog.getBatteryAmp();}
    float get12vAmp() {return analog.get12vAmp();}
  
};