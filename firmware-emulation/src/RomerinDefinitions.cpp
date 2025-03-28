//Romerin global variables implementations
#include "RomerinDefinitions.h"
//configurable global variables
#define _LITLEFS

#ifdef _LITLEFS 
 #define ROM_FS LittleFS 
 #include <LittleFS.h>
#else
define ROM_FS SPIFFS
#include "SPIFFS.h"
#endif




//#include "SPIFFS.h"

#include "RomDebug.h"
/* 
char RomDefs::MODULE_NAME[100]="NONE"; //"THOR"
uint8_t RomDefs::IP_ADDRESS[4]={192, 168, 1, 109};
uint8_t RomDefs::GATEWAY_ADDRESS[4]={192, 168, 1, 1};
uint8_t RomDefs::SUBNET_MASK[4]={255, 255, 255, 0};
char RomDefs::WIFI_SSID[50]="MHG";
char RomDefs::WIFI_KEY[50]="fll100218";
*/

char RomDefs::MODULE_NAME[100]="NONE"; //"THOR"
uint8_t RomDefs::IP_ADDRESS[4]={192, 168, 253, 180};
uint8_t RomDefs::GATEWAY_ADDRESS[4]={192, 168, 253, 254};
uint8_t RomDefs::SUBNET_MASK[4]={255, 255, 255, 0};
char RomDefs::WIFI_SSID[50]="ROM_WIFI";
char RomDefs::WIFI_KEY[50]="temp0ral";
uint16_t RomDefs::lenghts[6]={ROM_LENGHT_1,ROM_LENGHT_2,ROM_LENGHT_3,ROM_LENGHT_4,ROM_LENGHT_5,ROM_LENGHT_6};
MinMax RomDefs::limits[6]={ROM_MOTOR_2_MIN_MAX, ROM_MOTOR_3_MIN_MAX, ROM_MOTOR_4_MIN_MAX,
                           ROM_MOTOR_5_MIN_MAX, ROM_MOTOR_6_MIN_MAX, ROM_MOTOR_7_MIN_MAX};
int8_t RomDefs::offset[6]={0,0,0,0,0,0};
uint8_t RomDefs::vel_profile[6]={5,5,5,5,5,5};
uint8_t RomDefs::acc_profile[6]={25,25,25,25,25,25};

//info about the location of the module
int16_t RomDefs::position[3]={};
int16_t RomDefs::orientation[3]={};

uint8_t RomDefs::init_configuration{};

//Reads de configuration file if exists. Creates de configuration file 
// with the default configuration otherwise
bool RomDefs::readConfiguration(){
//probando apertura de ficheros: montando el sistema
  if(!ROM_FS.begin(true)){ //formats SPIFFS on error
     BT_DEBUG("An Error has occurred while mounting SPIFFS");
     return false;
  }
  #if SET_DEFAULT_WIFI_DATA == 1
    BT_DEBUG("Removing the previous configuration file");
    if (!ROM_FS.remove("/configuration.txt"))
      BT_DEBUG("Configuration file does not exist");
  #endif

  //if no file exists, create the file with default values
  if(!ROM_FS.exists("/configuration.txt")){
    BT_DEBUG("No configuration file found. Writing a new one");
    return writeConfiguration();
  }
  
  File conf_file = ROM_FS.open("/configuration.txt");
  if(!conf_file){
    BT_DEBUG("Unable to open the configuration file");
    return false;
  }
  //reads the binary data at the begining
  //version(u8)ip(4xu8)
  int file_version=0;
  if(conf_file.available())file_version=conf_file.read();
  if(file_version!=ROM_CONF_FILE_VERSION){
      conf_file.close();
      BT_DEBUG("Conf Version changed: reseting conf file.");
      return writeConfiguration();
  }
  //as a closed sytem it is, no reading errors are considered
  conf_file.read(IP_ADDRESS,4);
  BT_DEBUG_PRINT("config file: IP = %d.%d.%d.%d",IP_ADDRESS[0],IP_ADDRESS[1],IP_ADDRESS[2],IP_ADDRESS[3]);
  conf_file.read(GATEWAY_ADDRESS,4);
  conf_file.read(SUBNET_MASK,4);
  MODULE_NAME[conf_file.readBytesUntil('\0',MODULE_NAME, 99)]='\0';
  WIFI_SSID[conf_file.readBytesUntil('\0',WIFI_SSID, 49)]='\0';
  WIFI_KEY[conf_file.readBytesUntil('\0',WIFI_KEY, 49)]='\0';
  for(int i=0;i<6;i++){
     conf_file.read((uint8_t *)(&(lenghts[i])),2);
     conf_file.read((uint8_t *)(&(limits[i].min)),2);
     conf_file.read((uint8_t *)(&(limits[i].max)),2);
     offset[i]=conf_file.read();
     vel_profile[i]=conf_file.read();
     acc_profile[i]=conf_file.read();
  }
  for(int i=0;i<3;i++)conf_file.read((uint8_t *)(&(position[i])),2);
  for(int i=0;i<3;i++)conf_file.read((uint8_t *)(&(orientation[i])),2);
  init_configuration=conf_file.read();
  conf_file.close();
  return true;
}

 
bool RomDefs::writeConfiguration(){
 File conf_file = ROM_FS.open("/configuration.txt",FILE_WRITE);
  if(!conf_file){
    BT_DEBUG("Unable to write the configuration file");
    return false;
  }
  conf_file.write(ROM_CONF_FILE_VERSION);
  conf_file.write(IP_ADDRESS,4);
  conf_file.write(GATEWAY_ADDRESS,4);
  conf_file.write(SUBNET_MASK,4);
  conf_file.write((uint8_t *)MODULE_NAME,strlen(MODULE_NAME)+1);
  conf_file.write((uint8_t *)WIFI_SSID,strlen(WIFI_SSID)+1);
  conf_file.write((uint8_t *)WIFI_KEY,strlen(WIFI_KEY)+1);
  for(int i=0;i<6;i++){
     conf_file.write((uint8_t *)(&(lenghts[i])),2);
     conf_file.write((uint8_t *)(&(limits[i].min)),2);
     conf_file.write((uint8_t *)(&(limits[i].max)),2);
     conf_file.write(offset[i]);
     conf_file.write(vel_profile[i]);
     conf_file.write(acc_profile[i]);
  }
  for(int i=0;i<3;i++)conf_file.write((uint8_t *)(&(position[i])),2);
  for(int i=0;i<3;i++)conf_file.write((uint8_t *)(&(orientation[i])),2);
  conf_file.write(init_configuration);
  conf_file.close();
  return true;
}