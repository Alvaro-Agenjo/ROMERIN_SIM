#include "RomSuctionCupA.h"


uint8_t RomProximityArray::getMaxDifference()
{
   uint8_t * const (&d)=distances;
   uint8_t min=d[0]<d[1]?(d[0]<d[2]?d[0]:d[2]):(d[1]<d[2]?d[1]:d[2]);
   uint8_t max=d[0]>d[1]?(d[0]>d[2]?d[0]:d[2]):(d[1]>d[2]?d[1]:d[2]);
   return max-min;
}
uint8_t RomProximityArray::getMax(){
    uint8_t * const (&d)=distances;
    return (d[0]>d[1]?(d[0]>d[2]?d[0]:d[2]):(d[1]>d[2]?d[1]:d[2]));
 }
void RomProximityArray::setup(){

}
void RomProximityArray::loop(){

}

void RomSuctionCup::setup()
{
    distance_sensor.setup();  
}
void RomSuctionCup::loop()
{ 

    if(is_close_and_aligned())pressure=tvalue*22;
    else pressure*=0.2;
    force=(pressure*SUCTION_CUP_AREA)/10000.0F; //Newtons*/

    distance_sensor.loop();




        if(_mode==ATTACH_MODE){
            //conditions (with histeresis) for activating the turbine
            uint8_t max=distance_sensor.getMax();
            uint8_t dif=distance_sensor.getMaxDifference();
            if((max<_min_distance)&&(dif<_max_discrepance)){
                if((force<_goal_force)&&(tgoal==tvalue)&&(tgoal<100))tgoal++;
                if((force>_goal_force*1.2)&&(tgoal==tvalue)&&(tgoal>0))tgoal--;
            }else if(tgoal>0)tgoal--;
        }



   if(tgoal!=tvalue)
         tvalue+= tgoal<tvalue ? -1 : 1 ;


}

void RomSuctionCup::configureAttach(float goal_force, uint8_t min_distance, uint8_t max_discrepance )
{
_goal_force=goal_force;
_min_distance=min_distance;
_max_discrepance=max_discrepance;
}
void RomSuctionCup::attach()
{
    //emulation

_mode=ATTACH_MODE;
}
bool RomSuctionCup::isAttached(){
    if(_mode==ATTACH_MODE){
        if((force>=_goal_force*0.8F)&&(tvalue>10)) return true;
    }else{
        if(force>10) return true;
    }
return false;
}
void RomSuctionCup::dettach(){

_mode=NORMAL;
tgoal=0;
}
