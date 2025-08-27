#pragma once
#include <Arduino.h>
template<class T,int N, class U=T>
class filter{
T buffer[N]={0};
U acc;
int index=0;
public:
    filter(T init=0){acc=init;for(auto &el:buffer)el=init;}
    T add(T value){
        acc-=buffer[index];
        acc+=(buffer[index]=value);
        index=(index+1)%N;
        return acc/N; 
    }
};
class TIMER{
    ulong lt=0;
    uint16_t _time;
    public:
    TIMER(uint16_t time):_time(time){
      lt=millis();
    }
    //returns true if the time exceeds the conf time
    bool operator()(){
      if(millis()-lt>_time){
        lt=millis();
        return true;
      }
    return false;
    }
    //useful to implement a polling watchdog
    void reset(){lt=millis();}
  };
  