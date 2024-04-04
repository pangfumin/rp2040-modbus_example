#include <hardware/irq.h>
#include <hardware/uart.h>
#include <pico/stdlib.h>
#include <stdio.h>
#include "pico/multicore.h"
#include "hardware/i2c.h"
#include "sensors/ds18b20.hpp"
#include "myI2C.hpp"
#include "sensors/BME280.h"
#include "ModbusPico.hpp"

//////////////////////////////////////////////////////////////////
// Modbus parameters
#define MB_SLAVE_ADDRESS 2
#define MB_BAUDRATE      115200
#define MB_DATA_BITS     8
#define MB_STOP_BITS     1
#define MB_PARITY        UART_PARITY_NONE

// MB_TIMEOUT is related to baud rate
// please change it in modbus.hpp

#define MB_UART_NUMBER   0
#define MB_TX_PIN        0
#define MB_RX_PIN        1

#define MB_DE_PIN        2


ModbusPico modbus;
ds18b20 ds_sensor(13);

//////////////////////////////////////////////////////////////////


// костыль, тк irq_handler не модет быть функция - метод класса
// внутри .hpp файла реализовать не получилось, тк надо вызывать функцию - метод класса
// работает и норм -_-
void on_mb_rx() 
{
  if (modbus.uart_number == 0)
    modbus.mb_rx(uart_getc(uart0));
  else
    modbus.mb_rx(uart_getc(uart1));
}

void modbus_process_on_core_1()
{
 int loop;
  while(true)
  {
   // do we have to update sensors
   //if(multicore_fifo_rvalid())
     {
      //  for(loop=0;loop<modbus.dsSensorCount;loop++)
      //    {
      //      modbus.dsSensors[loop] = modbus._t_dsSensors[loop];
      //    }
      //  for(loop=0;loop<(modbus.BME280_MAX*2*3);loop++) // number of sensors 3 values of 32 bits to 16 bits register.
      //   {
      //     modbus.bme280Sensors[loop]= modbus._t_bme280Sensors[loop];
      //   }
        for(loop=0;loop<modbus.BME280_MAX;loop++) {
          // modbus.bme280_ID[loop]= modbus._t_bme280_ID[loop];
          modbus.bme280_ID[loop] = (loop == 0? BME280_ID : BME280_ID2);
        }
          

      //multicore_fifo_pop_blocking();
     }
    modbus.mb_process();

    uint16_t time_sec = (uint16_t) (time_us_64()/(1000*1000));

    modbus.sensor_0 = time_sec*10;
    modbus.sensor_1 = time_sec*20;
    modbus.sensor_2 = time_sec*30;
  }
}


void UpdateDS18B20Sensor()
{
        ds_sensor.scanSensors();

           modbus.dsSensorCount=modbus.DS18B20_MAX;
        if(ds_sensor.count > modbus.DS18B20_MAX)
           modbus.dsSensorCount = modbus.DS18B20_MAX;
        else
           modbus.dsSensorCount = ds_sensor.count;

        if(modbus.dsSensorCount>0)
        {
          // fill sensor address
          for(int loop=0;loop<modbus.dsSensorCount;loop++)
             for(int reg16=0;reg16<4;reg16++)
              {
                  modbus.dsSensorsAddress[loop*4 + reg16]= (uint16_t) ds_sensor.sensorAddress[loop].rom[reg16*2]<<8 |
							   (uint16_t) (ds_sensor.sensorAddress[loop].rom[reg16*2+1]);
              }
        }

}



int main(void)
{

  stdio_init_all();
  int loop;
  uint8_t sensorFirstTime=1;

  // put a delay to enable USB serial
    sleep_ms(3000);



  printf("Modbus demo firmware start\r\n");

  //Initialize I2C port at 400 kHz
  i2c_init(i2c, 400 * 1000);
  gpio_set_function(sda_pin, GPIO_FUNC_I2C);
  gpio_set_function(scl_pin, GPIO_FUNC_I2C);
  gpio_pull_up(sda_pin);
  gpio_pull_up(scl_pin);
  BME280 bme280[2]= { BME280(), BME280()};
  //bme280[0].setAddress(0x76);
  //bme280[0].reset();
  //bme280[1].setAddress(0x77);
  //bme280[1].reset();

  bme280[0].begin(0x76);

  bme280[0].writeConfigRegister(BME280_STANDBY_500_US,BME280_FILTER_OFF,0);
  bme280[0].writeControlRegisters(BME280_OVERSAMPLING_1X,BME280_OVERSAMPLING_1X,BME280_OVERSAMPLING_1X,BME280_MODE_NORMAL);
  bme280[1].begin(0x77);
  bme280[1].writeConfigRegister(BME280_STANDBY_500_US,BME280_FILTER_OFF,0);
  bme280[1].writeControlRegisters(BME280_OVERSAMPLING_1X,BME280_OVERSAMPLING_1X,BME280_OVERSAMPLING_1X,BME280_MODE_NORMAL);

  modbus.mb_init(MB_SLAVE_ADDRESS,
                 MB_UART_NUMBER,
                 MB_BAUDRATE,
                 MB_DATA_BITS,
                 MB_STOP_BITS,
                 MB_PARITY,
                 MB_RX_PIN,
                 MB_TX_PIN,
                 MB_DE_PIN);

  multicore_launch_core1(modbus_process_on_core_1);
  UpdateDS18B20Sensor();
  for(loop=0;loop<modbus.BME280_MAX;loop++)
      bme280[loop].read();

  while(true)
  {
//    printf("core 0 in sleep...");
    // if(ds_sensor.count==0)
    //  {
    //    UpdateDS18B20Sensor();
    //  }
    // else
    // {
    //   if(!sensorFirstTime)
    //    {
    //     for(loop=0;loop<modbus.dsSensorCount;loop++)
    //        modbus._t_dsSensors[loop] = ds_sensor.getTemperatureInt16(loop);


    //    for(loop=0;loop<modbus.BME280_MAX;loop++)
    //     {
    //      modbus._t_bme280_ID[loop]=bme280[loop].readId();

    //      if((modbus._t_bme280_ID[loop] == BME280_ID) ||
    //         (modbus._t_bme280_ID[loop] == BME280_ID2))
    //        {
    //            // ok found sensor
    //         bme280[loop].read();
    //         int32_t temp;
    //         uint32_t hum;
    //         uint32_t pres;
    //         temp = bme280[loop].temperature();
    //         hum = bme280[loop].humidity();
    //         pres = bme280[loop].pressure();

    //         modbus._t_bme280Sensors[loop*6]= temp>>16;
    //         modbus._t_bme280Sensors[loop*6+1]= temp&0xffff;
    //         if(modbus._t_bme280_ID[loop] == BME280_ID)
    //         {
    //           modbus._t_bme280Sensors[loop*6+2]= hum>>16;
    //           modbus._t_bme280Sensors[loop*6+3]= hum&0xffff;
    //         }
    //         else
    //         {
    //           modbus._t_bme280Sensors[loop*6+2]=  0;
    //           modbus._t_bme280Sensors[loop*6+3]=  0;
    //         }
    //         modbus._t_bme280Sensors[loop*6+4]= pres>>16;
    //         modbus._t_bme280Sensors[loop*6+5]= pres&0xffff;
    //      }
    //      else
    //      {
    //         for(int loop2=0;loop2<6;loop2++)
    //             modbus._t_bme280Sensors[loop*6+loop2]= 0;
    //      }

    //     }
    //      // send flag to second core to update
    //     multicore_fifo_push_blocking(1);
    //   }
    //     ds_sensor.startConversion();
    //     sensorFirstTime=0;
    // }
      sleep_ms(5000);
  }
}

