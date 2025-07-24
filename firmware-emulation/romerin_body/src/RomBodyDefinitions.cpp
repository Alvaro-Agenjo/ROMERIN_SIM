//Romerin global variables implementations
#include "RomBodyDefinitions.h"
//configurable global variables
#define _LITLEFS

#ifdef _LITLEFS 
 #define ROM_FS LittleFS 
 #include <LittleFS.h>
#else
# define ROM_FS SPIFFS
# include "SPIFFS.h"
#endif




//#include "SPIFFS.h"

#include "RomBodyWifi.h"
#define NO_LABO
#ifdef NO_LABO
# define WIFI_SSID_DEF "MHG"
# define WIFI_KEY_DEF "fll100218"
# define GATEWAY_DEF {192, 168, 1, 1}
# define IP_DEF {192, 168, 1, 117}
#else
# define WIFI_SSID_DEF "ROM_WIFI"
# define WIFI_KEY_DEF "temp0ral"
# define GATEWAY_DEF {192, 168, 253, 254}
# define IP_DEF {192, 168, 253, 180}
#endif

char RomBodyDefs::MODULE_NAME[100]="4L_BODY"; 
uint8_t RomBodyDefs::IP_ADDRESS[4]=IP_DEF;
uint8_t RomBodyDefs::GATEWAY_ADDRESS[4]=GATEWAY_DEF;
uint8_t RomBodyDefs::SUBNET_MASK[4]={255, 255, 255, 0};
char RomBodyDefs::WIFI_SSID[50]=WIFI_SSID_DEF;
char RomBodyDefs::WIFI_KEY[50]=WIFI_KEY_DEF;

//info about the location and mass of the body
uint16_t RomBodyDefs::mass=400;
int16_t RomBodyDefs::position[3]={};
int16_t RomBodyDefs::orientation[3]={};





//Reads de configuration file if exists. Creates de configuration file 
// with the default configuration otherwise
bool RomBodyDefs::readConfiguration(){
//probando apertura de ficheros: montando el sistema

  if(!ROM_FS.begin(true)){ //formats SPIFFS on error
    ROM_DEBUG("An Error has occurred while mounting SPIFFS");
     return false;
  }
  #if SET_DEFAULT_WIFI_DATA == 1
    BT_DEBUG("Removing the previous configuration file");
    if (!ROM_FS.remove("/configuration.txt"))
      BT_DEBUG("Configuration file does not exist");
  #endif

  //if no file exists, create the file with default values
  if(!ROM_FS.exists("/configuration.txt")){
    ROM_DEBUG("No configuration file found. Writing a new one");
    return writeConfiguration();
  }
  
  File conf_file = ROM_FS.open("/configuration.txt");
  if(!conf_file){
    ROM_DEBUG("Unable to open the configuration file");
    return false;
  }
  //reads the binary data at the begining
  //version(u8)ip(4xu8)
  int file_version=0;
  if(conf_file.available())file_version=conf_file.read();
  if(file_version!=ROM_BODY_CONF_FILE_VERSION){
      conf_file.close();
      ROM_DEBUG("Conf Version changed: reseting conf file.");
      return writeConfiguration();
  }
  //as a closed sytem it is, no reading errors are considered
  conf_file.read(IP_ADDRESS,4);
  ROM_DEBUG_PRINT("config file: IP = %d.%d.%d.%d",IP_ADDRESS[0],IP_ADDRESS[1],IP_ADDRESS[2],IP_ADDRESS[3]);
  conf_file.read(GATEWAY_ADDRESS,4);
  conf_file.read(SUBNET_MASK,4);
  MODULE_NAME[conf_file.readBytesUntil('\0',MODULE_NAME, 99)]='\0';
  WIFI_SSID[conf_file.readBytesUntil('\0',WIFI_SSID, 49)]='\0';
  WIFI_KEY[conf_file.readBytesUntil('\0',WIFI_KEY, 49)]='\0';
  conf_file.read((uint8_t *)(&mass),2);
  for(int i=0;i<3;i++)conf_file.read((uint8_t *)(&(position[i])),2);
  for(int i=0;i<3;i++)conf_file.read((uint8_t *)(&(orientation[i])),2);
 
  conf_file.close();
  return true;
}

 
bool RomBodyDefs::writeConfiguration(){
 File conf_file = ROM_FS.open("/configuration.txt",FILE_WRITE);
  if(!conf_file){
    ROM_DEBUG("Unable to write the configuration file");
    return false;
  }
  conf_file.write(ROM_BODY_CONF_FILE_VERSION);
  conf_file.write(IP_ADDRESS,4);
  conf_file.write(GATEWAY_ADDRESS,4);
  conf_file.write(SUBNET_MASK,4);
  conf_file.write((uint8_t *)MODULE_NAME,strlen(MODULE_NAME)+1);
  conf_file.write((uint8_t *)WIFI_SSID,strlen(WIFI_SSID)+1);
  conf_file.write((uint8_t *)WIFI_KEY,strlen(WIFI_KEY)+1);
  conf_file.write((uint8_t *)(&mass),2);
  for(int i=0;i<3;i++)conf_file.write((uint8_t *)(&(position[i])),2);
  for(int i=0;i<3;i++)conf_file.write((uint8_t *)(&(orientation[i])),2);
  conf_file.close();
  return true;
}