#include "RomPower.h"

void RomPower::powerPolicy()
{
  //automatic basic power mode changes

  //return
  switch(mode){
      case POWER_MODE::ISOLATED:
      case POWER_MODE::SHARE:
        /*if((analog.getBatteryVolt()<15)&&
           (analog.getBusVolt()>analog.getBatteryVolt())&&
           (analog.getBusVolt()>15))
            setMode(POWER_MODE::CHARGE);*/
      break;
      case POWER_MODE::CHARGE:
      case POWER_MODE::BUS_ONLY:
        if(analog.getBatteryVolt()>16.8)
            setMode(POWER_MODE::ISOLATED);
      break;
  };

  
}
void RomPower::setup()
{
  analog.setup();
  pinMode(RELE_A, OUTPUT);
  pinMode(RELE_B, OUTPUT);
  setMode();
  powerPolicy();
}
void RomPower::loop()
{
  analog.loop();
  powerPolicy();


}
void RomPower::setMode(POWER_MODE mode){
    this->mode=mode;
    switch(mode){
        case POWER_MODE::ISOLATED:
          digitalWrite(RELE_A,LOW);
          digitalWrite(RELE_B,LOW);
        break;
        case POWER_MODE::SHARE:
          digitalWrite(RELE_A,LOW);
          digitalWrite(RELE_B,HIGH);
         
        break;
        case POWER_MODE::CHARGE:
          digitalWrite(RELE_A,HIGH);
          digitalWrite(RELE_B,LOW);
         
        break;
        case POWER_MODE::BUS_ONLY:
          digitalWrite(RELE_A,HIGH);
          digitalWrite(RELE_B,HIGH);
         
        break;
    }
}