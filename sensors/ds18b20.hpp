#include <cstdio>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "one_wire.h"

// check this github for one wire
// https://github.com/adamboardman/pico-onewire

class  ds18b20
{
  public:
      static bool debug;
      ds18b20(uint8_t gpioPin);
      ~ds18b20();
      static const uint8_t sensorMax=10;
      rom_address_t sensorAddress[sensorMax];
      uint8_t count;
      uint8_t scanSensors();
      int16_t getTemperatureInt16(uint8_t sensorIdx);
      void startConversion();
  private:
      One_wire *onewire;
};


