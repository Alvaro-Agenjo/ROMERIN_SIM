#pragma once
//#include "RomerinDefinitions.h"
#include "RomBlueTooth.h"
#include "RomWiFi.h"

//DEBUG
#define ENABLE_BT_DEBUG 1
#define ENABLE_WIFI_DEBUG 1
#define ENABLE_BT_DEBUG_RCV 0
#define ENABLE_BT_DEBUG_PRINTF 
#define ENABLE_BT_DEBUG_WIFI 0 //set to 1 to print info about udp packets
#define BT_DEBUG(X) {if(ENABLE_BT_DEBUG)RomBT::sendText(X);if(ENABLE_WIFI_DEBUG)RomWiFi::sendText(X);}
#ifdef ENABLE_BT_DEBUG_PRINTF
#define BT_DEBUG_PRINT(...) {if(ENABLE_BT_DEBUG) RomBT::sendPrint(__VA_ARGS__);if(ENABLE_WIFI_DEBUG)RomWiFi::sendPrint(__VA_ARGS__);}
#else 
#define BT_DEBUG_PRINT(...) ;
#endif

