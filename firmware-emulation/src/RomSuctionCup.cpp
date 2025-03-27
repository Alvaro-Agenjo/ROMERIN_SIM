#include "RomSuctionCup.h"
#include "RomDebug.h"

void RomSuctionCup::setup()
{
   pinMode(PWM_TURBINA,OUTPUT);
   digitalWrite(PWM_TURBINA, LOW);
   delay(2000); //dos segundos con la linea baja
   turbina.attach(PWM_TURBINA,PWM_CHANNEL_TURBINA);
   turbina.writeMicroseconds(800);
   
   
   Wire.begin(-1,-1,(uint32_t)1000000); //400000
   if(ENABLE_PRESSURE_SENSOR ){
    #if PRESSURE_SENSOR_SELECTION == 0
        if (!bmp.begin(0)) {
            BT_DEBUG("Could not find a valid BMP085 sensor, check wiring!");
        }
        atm_pressure=bmp.readPressure();
    #else
        if(!pressure_sensor.begin()){
             BT_DEBUG("ERROR: MS5611 NOT found");
        }
        else {
            delay(10);
            pressure_sensor.read();
            delay(10);
        }
        //pressure_sensor.reset(1);
        atm_pressure=pressure_sensor.getPressure();
    #endif
   }//ENABLE PRESSURE SENSOR

   if(ENABLE_VL6180){
    distance_sensor.setup();
   }
}
void RomSuctionCup::loop()
{ 
    if(i2creading || true){
        
        if(ENABLE_PRESSURE_SENSOR  ) {
            if(cicle%4 == 0)pressure_sensor.read();

             
            #if PRESSURE_SENSOR_SELECTION == 0
            if(cicle%1000 == 0)temperature=bmp.readTemperature();
            #else
            if(cicle%4 == 1)temperature=pressure_sensor.getTemperature();
            #endif
            

            if(cicle%4 == 2) {  // This takes 14ms to be calculated
                float pres = 0;
                #if PRESSURE_SENSOR_SELECTION == 0
                    pres=pfilter.add(bmp.readPressure());
                #else
                    pres=pfilter.add(pressure_sensor.getPressure());
                #endif
                //if((tvalue==0)&&(atm_pressure-pres<0)&&(atm_pressure-pres>-500))atm_pressure=(pres+5*atm_pressure)/6;
                if(tvalue<5)atm_pressure=(pres+5*atm_pressure)/6;
                pressure=100.0F*(atm_pressure-pres);
                if(pressure<10)pressure=0;
                force=pressure>20?((pressure-20)*SUCTION_CUP_AREA)/10000.0F:0; //Newtons
                

            }
        }

        if(ENABLE_VL6180) {  // This takes 34ms to be calculated
            if(cicle%4 == 3)
                distance_sensor.loop();
        }
        if(_mode==ATTACH_MODE){
            //conditions (with histeresis) for activating the turbine
            uint8_t max=distance_sensor.getMax();
            uint8_t dif=distance_sensor.getMaxDifference();
            if((max<_min_distance)&&(dif<_max_discrepance)){
                if((force<_goal_force)&&(tgoal==tvalue)&&(tgoal<100))tgoal++;
                if((force>_goal_force*1.2)&&(tgoal==tvalue)&&(tgoal>0))tgoal--;
            }else if(tgoal>0)tgoal--;
        }

        
        if (--cicle < 1) cicle = 65535;
    }//enable I2Creading

   if(tgoal!=tvalue){   // This takes 1ms to be calculated
         if(tgoal<tvalue)tvalue--; else tvalue++;
         turbina.writeMicroseconds(800+(tvalue*(2200-800))/100);
   }

}

void RomSuctionCup::configureAttach(float goal_force, uint8_t min_distance, uint8_t max_discrepance )
{
_goal_force=goal_force;
_min_distance=min_distance;
_max_discrepance=max_discrepance;
}
void RomSuctionCup::attach()
{
_mode=ATTACH_MODE;
}
bool RomSuctionCup::isAttached(){
if(force>=_goal_force*0.8F) return true;
return false;
}
void RomSuctionCup::dettach(){
_mode=NORMAL;
tgoal=0;
}