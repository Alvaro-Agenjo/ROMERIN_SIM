## ROMERIN MAIN FIRMWARE AND QT APPS
This repository includes the following romerin compoments:
 - src & platformio.ini & include & shared & board : ROBOT FIRMWARE TO BE DOWNLOADED AT THE ESP32 on the RomerinV2-- electronics
 - romerin_interface: qt interface to comitor and control any robot module (prefereably Qt5 due to the use of gamepads) .
 - romerin_emulator: a qt aplication that behaves as if a set of robots were present at the same network (prefereably Qt5).
 - romerin_body: firmware for the esp32-S3-touch-LCD-1.28, thas serves as acceleremeter of the body and central commands and info.
 - test: some cpp programs and matlab scripts whose intent is to test some of the algorithms included into the other projects

 


## Details to consider version 2023

 - If the status of the motors is not read correctly, it is most likely that there is a wrong connection, that is, there is a cable that does not connect two motors for example.
 - The type of the suction cup to be used can be set by modifying the PRESSURE_SENSOR_SELECTION variable defined in RomerinDefinitions.h. The sensor MS5611 corresponds to the sensors of the new version of the circular crown.
 - If the pressure and temperature data are incorrect, the PRESSURE_SENSOR_SELECTION variable is most likely incorrect.

If Dynamixel2Arduino is not installed. Type the following text where the platformio.ini file is located:
```
pio lib -g install https://github.com/ROBOTIS-GIT/Dynamixel2Arduino/archive/refs/tags/0.4.4.zip
```