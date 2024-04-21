#include <hardware/irq.h>
#include <hardware/uart.h>
#include <pico/stdlib.h>
#include <stdio.h>
#include <math.h> 

#include "pico/multicore.h"
#include "panel_io.hpp"
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

auto_init_mutex(io_mutex);
uint32_t owner_out;

uint8_t panel_led_values_temp[8];
uint8_t panel_switch_values_temp[8];
uint8_t led_history_ringbuffer[8][32] = {0};
uint8_t led_history_ringbuffer_index[8] = {0};
uint8_t led_history_one_count[8][8] = {0}; // per bit



typedef enum {
  PANEL_LED_IMMEDIATE = 0,
  PANEL_LED_LOW_PASS_FILTED = 1,
} PanelLedPattern;


uint8_t param_delay_us = 50;
uint8_t param_led_pattern = PANEL_LED_LOW_PASS_FILTED;

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

    if (mutex_try_enter(&io_mutex,&owner_out)){
      // param
      param_led_pattern = modbus.command_param[0];
      param_delay_us = modbus.command_param[1];

      for (int out_reg_idx = 0; out_reg_idx < 8; out_reg_idx++) {
        panel_led_values_temp[out_reg_idx]= modbus.panel_led_values[out_reg_idx];
      }

      for (int in_reg_idx = 0; in_reg_idx < 6; in_reg_idx++) {
        modbus.panel_switch_values[in_reg_idx] = panel_switch_values_temp[in_reg_idx];
      }
      mutex_exit(&io_mutex);
    }

    // uint16_t time_sec = (uint16_t) (time_us_64()/(1000));
    // modbus.sensor_0 = time_sec;
    // modbus.sensor_1 = time_sec;
    // modbus.sensor_2 = time_sec;

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

  init_gpio();
  clear_all();
 
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


  uint8_t led_mode_temp = param_led_pattern;
  uint64_t param_delay_us_temp = param_delay_us;

  while(true)
  {

    uint8_t input_temp[8];
    uint8_t output_temp[8];
    for (int in_reg_idx = 0; in_reg_idx < 6; in_reg_idx++) {
      input_temp[in_reg_idx] = get_input(in_reg_idx);
    }


    if (mutex_try_enter(&io_mutex,&owner_out)){
      led_mode_temp = param_led_pattern;
      param_delay_us_temp = param_delay_us;

      for (int in_reg_idx = 0; in_reg_idx < 6; in_reg_idx++) {
        panel_switch_values_temp[in_reg_idx] = input_temp[in_reg_idx];
      }

      for (int out_reg_idx = 0; out_reg_idx < 8; out_reg_idx++) {
        output_temp[out_reg_idx] = panel_led_values_temp[out_reg_idx];
      }

      mutex_exit(&io_mutex);
    }

    PanelLedPattern led_mode = 
      led_mode_temp == 0 ? PANEL_LED_IMMEDIATE : PANEL_LED_LOW_PASS_FILTED;

    if (led_mode == PANEL_LED_IMMEDIATE) {
      for (int out_reg_idx = 0; out_reg_idx < 8; out_reg_idx++) {
        uint8_t value = output_temp[out_reg_idx];
        set_output(out_reg_idx 
          + ModbusPico::MB_COMMAND_PANEL_LED_0_OUTPUT_REGISTER 
          - ModbusPico::MB_COMMAND_PANEL_REGISTER_START, 
          value);
      }
    } else if (led_mode == PANEL_LED_LOW_PASS_FILTED)  {
      // Update LED history in ring buffer
      for (int out_reg_idx = 0; out_reg_idx < 8; out_reg_idx++) {
        uint8_t value = output_temp[out_reg_idx];
        uint8_t next_index 
          = led_history_ringbuffer_index[out_reg_idx] + 1 == 32 ? 
            0 : led_history_ringbuffer_index[out_reg_idx] + 1;
        uint8_t pop_data = led_history_ringbuffer[out_reg_idx][next_index];
        uint8_t push_data = value & 0xFF;
        led_history_ringbuffer[out_reg_idx][led_history_ringbuffer_index[out_reg_idx]] = push_data;
        for (int i = 0; i < 8; i++) {
          led_history_one_count[out_reg_idx][i] -= (pop_data >> i) & 0x01;
          led_history_one_count[out_reg_idx][i] += (push_data >> i) & 0x01;
        }

        led_history_ringbuffer_index[out_reg_idx] = next_index;
      }

      // Update LED output acording to brightness and phase
      for (int phase = 0; phase < 31; phase++) {
          for (int out_reg_idx = 0; out_reg_idx < 8; out_reg_idx++) {
            uint8_t value = 0;
            for (int bitidx = 0; bitidx < 8; bitidx++) {
              uint64_t bit_brightness = led_history_one_count[out_reg_idx][bitidx];
              if (brightness_phase_lookup[bit_brightness][phase]) {
                value |= 1 << bitidx;
              }
            }

            set_output(out_reg_idx
              + ModbusPico::MB_COMMAND_PANEL_LED_0_OUTPUT_REGISTER 
              - ModbusPico::MB_COMMAND_PANEL_REGISTER_START, 
              value);
          }
        sleep_us(param_delay_us_temp);  // delay
      }
    }
    // sleep_ms(1000);
  }
}

