
#pragma once
#include <Arduino.h>
#include <Dynamixel2Arduino.h>
#include "RomerinDefinitions.h"


class RomerinDXLPortHandler : public DYNAMIXEL::SerialPortHandler
{
 
  public:
    RomerinDXLPortHandler(HardwareSerial& port = Serial2, const int dir_pin = ROM_DXL_DIR_PIN, int8_t rx_pin=ROM_DXL_RX,int8_t tx_pin=ROM_DXL_TX)
    : SerialPortHandler(port, dir_pin), port_(port), dir_pin_(dir_pin),rx_pin_(rx_pin), tx_pin_(tx_pin)
    {}
    void begin(unsigned long baud=ROM_DXL_BAUD_RATE) override
    {

      baud_ = baud;
      port_.begin(baud_,SERIAL_8N1,rx_pin_,tx_pin_);
      
      if(dir_pin_ != -1){
        pinMode(dir_pin_, OUTPUT);
        digitalWrite(dir_pin_, LOW);
        while(digitalRead(dir_pin_) != LOW);
      }
      setOpenState(true);
    }
    virtual size_t write(uint8_t c) override
    {
      size_t ret = 0;
      digitalWrite(dir_pin_, HIGH);
      while(digitalRead(dir_pin_) != HIGH);
      
      ret = port_.write(c);

      port_.flush();
      digitalWrite(dir_pin_, LOW);
      while(digitalRead(dir_pin_) != LOW);
      
      return ret;
    }

    virtual size_t write(uint8_t *buf, size_t len) override
    {
      size_t ret;
      digitalWrite(dir_pin_, HIGH);
      while(digitalRead(dir_pin_) != HIGH);

      ret = port_.write(buf, len);

      port_.flush();
      digitalWrite(dir_pin_, LOW);
      while(digitalRead(dir_pin_) != LOW);

      return ret;     
    }

  private:
    HardwareSerial& port_;
    unsigned long baud_;
    const int dir_pin_;
    const int8_t rx_pin_, tx_pin_;
};