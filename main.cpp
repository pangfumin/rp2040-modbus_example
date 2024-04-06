#include <hardware/irq.h>
#include <hardware/uart.h>
#include <pico/stdlib.h>
#include <stdio.h>
#include <math.h> 

#include "pico/multicore.h"
#include "hardware/i2c.h"
#include "ModbusPico.hpp"

//////////////////////////////////////////////////////////////////
// Modbus parameters
#define MB_SLAVE_ADDRESS 1
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

auto_init_mutex(my_mutex);
uint32_t owner_out;

// uint16_t data = 0;
// void trylock(int core){
//     uint32_t owner_out;
//     if (mutex_try_enter(&my_mutex,&owner_out)){
//         // printf("from core%d: in Mutex!!\n",core); 
//         sleep_ms(10);
//         mutex_exit(&my_mutex);
//         // printf("from core%d: Out Mutex!!\n",core); 

//         sleep_ms(10);
//     }
//     else{
//         // printf("from core%d: locked %d\n",core,owner_out);
//         sleep_ms(10);
//     }
// }

uint8_t led_history_one_count_temp[8][8];

static char brightness_phase_lookup[32][31] =
{ //
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0 }, //  0/31 =  0%
				{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
						0, 0, 0, 0 }, //  1/31 =  3%
				{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
						0, 0, 0, 0 }, //  2/31 =  6%
				{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
						0, 0, 0, 0 }, //  3/31 = 10%
				{ 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
						0, 0, 0, 0 }, //  4/31 = 13%
				{ 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0,
						0, 0, 0, 0 }, //  5/31 = 16%
				{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,
						0, 0, 0, 0 }, //  6/31 = 19%
				{ 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0,
						1, 0, 0, 0 }, //  7/31 = 23%
				{ 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0,
						1, 0, 0, 0 }, //  8/31 = 26%
				{ 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0,
						0, 1, 0, 0 }, //  9/31 = 29%
				{ 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0,
						0, 1, 0, 0 }, // 10/31 = 32%
				{ 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0,
						0, 1, 0, 0 }, // 11/31 = 35%
				{ 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1,
						0, 1, 0, 0 }, // 12/31 = 39%
				{ 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1,
						0, 0, 1, 0 }, // 13/31 = 42%
				{ 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0,
						1, 0, 1, 0 }, // 14/31 = 45%
				{ 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0,
						1, 0, 1, 0 }, // 15/31 = 48%
				{ 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0,
						1, 0, 1, 0 }, // 16/31 = 52%
				{ 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
						1, 0, 1, 0 }, // 17/31 = 55%
				{ 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1,
						0, 1, 1, 0 }, // 18/31 = 58%
				{ 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1,
						0, 1, 1, 0 }, // 19/31 = 61%
				{ 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1,
						0, 1, 1, 0 }, // 20/31 = 65%
				{ 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0,
						1, 1, 0, 1 }, // 21/31 = 68%
				{ 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0,
						1, 1, 0, 1 }, // 22/31 = 71%
				{ 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1,
						1, 1, 0, 1 }, // 23/31 = 74%
				{ 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1,
						1, 1, 0, 1 }, // 24/31 = 77%
				{ 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1,
						1, 0, 1, 1 }, // 25/31 = 81%
				{ 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1,
						1, 0, 1, 1 }, // 26/31 = 84%
				{ 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1,
						0, 1, 1, 1 }, // 27/31 = 87%
				{ 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
						1, 1, 1, 1 }, // 28/31 = 90%
				{ 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1,
						1, 1, 1, 1 }, // 29/31 = 94%
				{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
						1, 1, 1, 1 }, // 30/31 = 97%
				{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
						1, 1, 1, 1 } // 31/31 = 100%
		};


ModbusPico modbus;

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
   // do we have to update
    modbus.mb_process();

    uint16_t time_sec = (uint16_t) (time_us_64()/(1000));

    // if(multicore_fifo_rvalid())
    // {
    
    //   // for(loop=0;loop<modbus.BME280_MAX;loop++) {
    //   //   // modbus.bme280_ID[loop]= modbus._t_bme280_ID[loop];
    //   //   modbus.bme280_ID[loop] = (loop == 0? BME280_ID : BME280_ID2);
    //   // }
      
        

    //   uint32_t g = multicore_fifo_pop_blocking();
    //   modbus.sensor_0 = g;
    // }

    if (mutex_try_enter(&my_mutex,&owner_out)){
        for (int i = 0; i < 8; i++) {
          for (int j = 0; j < 8; j++) {
            led_history_one_count_temp[i][j] = modbus.led_history_one_count[i][j];
          }
        }
        mutex_exit(&my_mutex);
    }

    uint8_t b =  led_history_one_count_temp[7][0];


    modbus.sensor_0 = b;
    modbus.sensor_1 = time_sec;
    modbus.sensor_2 = time_sec;

    // // char bright = 31 * sin()
    // for (int i = 0; i < 360; i++) {
    //    uint64_t bright =  31 * sin(i / 180.0 * 3.14159);

    //    if (bright < 0)
    //      bright = -bright;

    //     // modbus.mb_process();

    //     // uint16_t time_sec = (uint16_t) (time_us_64()/(1000*1000));

    //     // modbus.sensor_0 = time_sec*10;
    //     // modbus.sensor_1 = time_sec*20;
    //     // modbus.sensor_2 = time_sec*30;

    //     // modbus.sensor_0 = bright;
    //     for (int phase =0; phase < 31; phase++) {
          
    //       set_output(DEBUG_OUTPUT, brightness_phase_lookup[bright][phase]);
    //       sleep_us(50);
    //     }
        


    // }
  }
}

int main(void)
{

  stdio_init_all();
  int loop;
  uint8_t sensorFirstTime=1;

  // put a delay to enable USB serial
  sleep_ms(1000);

  printf("Modbus demo firmware start\r\n");

 
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

  while(true)
  {

    uint16_t time_sec = (uint16_t) (time_us_64()/(1000));
    uint8_t temp[8][8];
     if (mutex_try_enter(&my_mutex,&owner_out)){
        // printf("from core%d: in Mutex!!\n",core); 
        // sleep_ms(10);
        // data = time_sec;

        for (int i = 0; i < 8; i++) {
          for (int bitidx = 0; bitidx < 8; bitidx++) {
            temp[i][bitidx] = led_history_one_count_temp[i][bitidx];
          }
        }
        mutex_exit(&my_mutex);
    }


            
    
    for (int phase = 0; phase < 31; phase++) {
        for (int i = 0; i < 8; i++) {
          uint8_t value = 0;
          for (int bitidx = 0; bitidx < 8; bitidx++) {
            uint64_t bit_brightness = temp[i][bitidx];
            if (brightness_phase_lookup[bit_brightness][phase]) {
              value |= 1 << bitidx;
            }
            
          }

          set_output(i 
            + ModbusPico::MB_COMMAND_PANEL_LED_0_OUTPUT_REGISTER 
            - ModbusPico::MB_COMMAND_PANEL_REGISTER_START, 
            value);

        }

      // sleep_us(50);
    }

    // sleep_ms(1000);
  }
}

