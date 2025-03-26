#pragma once
#include "RomerinMessage.h"
#define CIRCULAR_BUFFER_SIZE 100
struct RomerinWifiMsg:public RomerinMsg{
      IPAddress ip; 
      RomerinWifiMsg(const RomerinMsg &m,  const IPAddress &rem_ip):RomerinMsg(m),ip(rem_ip){}
      RomerinWifiMsg():RomerinMsg(),ip(){}
      static RomerinWifiMsg &none(){static RomerinWifiMsg _none;return _none;}   
};
class RomCircularMsgBuffer{
     RomerinWifiMsg buffer[CIRCULAR_BUFFER_SIZE];
     uchar_t init=0;
     uchar_t end=0;
  public:
     
     void push(const RomerinWifiMsg &m)
     {
         buffer[end]=m;
         if(++end>=CIRCULAR_BUFFER_SIZE)end=0;
     }
     void push_single(const RomerinWifiMsg &m, bool (*f)(const RomerinMsg &, const RomerinMsg &)){
         //check if there is message that should be overriden
         uchar_t ind=init;
         while(ind!=end){
             if(f(m,buffer[ind])){buffer[ind]=m;return;}
             if(++ind>=CIRCULAR_BUFFER_SIZE)ind=0;
         }
         push(m);
     }
     bool there_is_msg(){return init!=end;}
     RomerinWifiMsg getMessage(){
         if(init==end)return RomerinWifiMsg::none();
         uchar_t ind=init++;
         if(init>=CIRCULAR_BUFFER_SIZE)init=0;
         return buffer[ind];
     }

};