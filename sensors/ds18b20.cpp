#include <cstdio>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "one_wire.h"
#include "ds18b20.hpp"

// check this github for one wire
// https://github.com/adamboardman/pico-onewire

    bool ds18b20::debug= false;

    ds18b20::ds18b20(uint8_t gpioPin)
    {
	onewire = new One_wire(gpioPin);
        if(onewire)
          onewire->init();
        if(debug)
          printf("init DS18B20 gpio:%d  onewire:%s\r\n",gpioPin,onewire == __null ? "None" : "ok");
        count=0;
    }

    ds18b20::~ds18b20()
    {
     if(onewire)
      delete onewire;
     onewire=__null;
    }

uint8_t ds18b20::scanSensors()
    {
      int loop;
      // get number of sensor
      if(debug) printf("Get sensor count:");
      count = onewire->find_and_count_devices_on_bus();
      if(debug) printf("%d\r\n",count);

      // get Address of sensor

      if(count > ds18b20::sensorMax)
         count = ds18b20::sensorMax

      for(loop=0;loop<count;loop++)
          {
            sensorAddress[loop] = onewire->get_address(loop);
            if(debug)
              { printf("Sensor #%d :",loop);
	        printf("%02x%02x%02x%02x%02x%02x%02x%02x\r\n",
                        sensorAddress[loop].rom[0], sensorAddress[loop].rom[1],
                        sensorAddress[loop].rom[2], sensorAddress[loop].rom[3],
                        sensorAddress[loop].rom[4], sensorAddress[loop].rom[5],
                        sensorAddress[loop].rom[6], sensorAddress[loop].rom[7]);
              }
          }
       return count;
}

int16_t ds18b20::getTemperatureInt16(uint8_t sensorIdx)
    {
      float temp;
      if(sensorIdx >= count)
         return 0x7fff;
      temp=onewire->temperature(sensorAddress[sensorIdx]);
      if(debug)
         printf("ds18b20 sensor #%d : %3.1f'C\r\n",sensorIdx,temp);
      if(temp == onewire->invalid_conversion)
         return 0x7fff;
      return (int16_t ) (temp * 100.0);
    }


void ds18b20::startConversion()
{
    rom_address_t null_address{};
    onewire->convert_temperature(null_address,true,true);
}




