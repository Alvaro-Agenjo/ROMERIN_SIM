## Details to consider

 - If the status of the motors is not read correctly, it is most likely that there is a wrong connection, that is, there is a cable that does not connect two motors for example.
 - The type of the suction cup to be used can be set by modifying the PRESSURE_SENSOR_SELECTION variable defined in RomerinDefinitions.h. The sensor MS5611 corresponds to the sensors of the new version of the circular crown.
 - If the pressure and temperature data are incorrect, the PRESSURE_SENSOR_SELECTION variable is most likely incorrect.

If Dynamixel2Arduino is not installed. Type the following text where the platformio.ini file is located:
```
pio lib -g install https://github.com/ROBOTIS-GIT/Dynamixel2Arduino/archive/refs/tags/0.4.4.zip
```