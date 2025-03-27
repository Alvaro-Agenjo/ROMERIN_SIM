#include "RomBlueTooth.h"
#include <stdarg.h>


String RomBT::_bt_name;
BluetoothSerial  RomBT::port;
RomerinMsg RomBT::last_message;
MsgReader RomBT::msg_reader;
void RomBT::setup(const char *bt_name )
{
 _bt_name=String(bt_name);
 port.begin(bt_name);
}
void RomBT::sendText(const char *text)
{
      RomerinMsg &&rep_msg = text_message(text);
      port.write(rep_msg.data,rep_msg.size+3);
}
void RomBT::sendPrint(const char *fmt, ... )
{
    char text[300];
    va_list myargs;
    va_start(myargs,fmt);
    vsprintf(text,fmt,myargs);
    va_end(myargs);
    RomerinMsg &&rep_msg = text_message(text);
    port.write(rep_msg.data,rep_msg.size+3);
}
void RomBT::sendMessage(const RomerinMsg &m)
{
    if(m.size)port.write(m.data,m.size+3);
}
bool RomBT::readMessage()
{
    while(port.available()){
      uchar_t aux=port.read();
      if(msg_reader.add_uchar(aux)){
        last_message=msg_reader.getMessage();
        return true;
      }
      
    }
    return false;
} 