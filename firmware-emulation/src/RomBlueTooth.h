#pragma once
#include <Arduino.h>
#include "RomerinDefinitions.h"
#include "BluetoothSerial.h"
#include "RomerinMsg.h"


class RomBT{
    static String _bt_name;
    static BluetoothSerial port;
    static RomerinMsg last_message;
    static MsgReader msg_reader;
public:
    static void setup(const char *bt_name = RomDefs::MODULE_NAME);
    static void sendText(const char *text);
    static void sendPrint(const char *fmt, ...);
    static bool hasClient(){return port.hasClient();}
    static void sendMessage(const RomerinMsg &m);
    static bool readMessage(); //return true if there is a new message. if true The message is saved on last_message
    static RomerinMsg getMessage(){return last_message;}
};
