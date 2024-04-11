#include <stdio.h>
#include <string.h>

#include <hardware/irq.h>
#include <hardware/uart.h>
#include <pico/stdlib.h>
#include "ModbusPico.hpp"
#include <pico/unique_id.h>
#include "hardware/gpio.h"
#include "hardware/adc.h"

bool ModbusPico::debug=true;
uint8_t ModbusPico::Coils[COILS_MAX]={PICO_DEFAULT_LED_PIN};
uint8_t ModbusPico::panel_led_values[8] = {0};
uint8_t ModbusPico::panel_switch_values[8] = {0};

const char *  ModbusPico::InstrumentIdString="Pico Modbus MBE280 Version 3.0\0\0\0\0";

void ModbusPico::printResponse(int v)
{
  int loop;
  if(debug)
  {
   printf("printResponse (%d) : ",v);
   for(loop=0;loop<mb_response_buf_pos;loop++)
     printf("0x%02X ",mb_response_buf[loop]);
   printf("\n\r");
  }
}




uint8_t ModbusPico::mb_read_holding_registers(uint16_t start, uint16_t count)
{
  uint16_t val;
  for (int i = 0; i < count; i++)
  {
    if (mb_read_holding_register(start + i, &val) == MB_NO_ERROR)
      mb_response_add(val);
    else
      return MB_ERROR_ILLEGAL_DATA_ADDRESS;
  }
  return MB_NO_ERROR;
}





uint8_t ModbusPico::mb_read_holding_register(uint16_t addr, uint16_t* reg)
{
     uint16_t * pt16;

    if((addr>= MB_COMMAND_UNIT_TYPE_ID_STRING_REGISTER) &&
       (addr <(MB_COMMAND_UNIT_TYPE_ID_STRING_REGISTER+16)))
     {
             pt16 = (uint16_t *) ModbusPico::InstrumentIdString;
             *reg = pt16[addr-MB_COMMAND_UNIT_TYPE_ID_STRING_REGISTER];
             return MB_NO_ERROR;
     }

    switch(addr)
     {
        case MB_COMMAND_PANEL_SWITCH_0_INPUT_REGISTER:
        case MB_COMMAND_PANEL_SWITCH_1_INPUT_REGISTER:
        case MB_COMMAND_PANEL_SWITCH_2_INPUT_REGISTER:
        case MB_COMMAND_PANEL_SWITCH_3_INPUT_REGISTER:
        case MB_COMMAND_PANEL_SWITCH_4_INPUT_REGISTER:
        case MB_COMMAND_PANEL_SWITCH_5_INPUT_REGISTER:
        {
          *reg = panel_switch_values[addr - MB_COMMAND_PANEL_REGISTER_START];
          return MB_NO_ERROR;
        }
        case MB_COMMAND_UNIQUE_ID_REGISTER0:
        case MB_COMMAND_UNIQUE_ID_REGISTER1:
        case MB_COMMAND_UNIQUE_ID_REGISTER2:
        case MB_COMMAND_UNIQUE_ID_REGISTER3:
             *reg = UniqueID[addr-MB_COMMAND_UNIQUE_ID_REGISTER0];
             return MB_NO_ERROR;
        case MB_COMMAND_UNIT_TYPE_ID_REGISTER:
             *reg = UNIT_TYPE;
             return MB_NO_ERROR;
        case MB_COMMAND_UNIT_VERSION_ID_REGISTER:
             *reg = SOFTWARE_VERSION;
             return MB_NO_ERROR;
     }
     return ModbusManager::mb_read_holding_register(addr,reg);
}

uint8_t ModbusPico::mb_validate_input_register(uint16_t address, uint16_t * reg)
{
  return MB_NO_ERROR;
}


uint8_t ModbusPico::mb_read_input_registers(uint16_t start, uint16_t count)
{
   return MB_NO_ERROR;
}




uint8_t ModbusPico::mb_read_coil_status(uint16_t start, uint16_t count) {
  printResponse(1);
  uint8_t mask=1;
  uint8_t value=0;
  int loop;
  int nbyte = count/8;
  uint8_t  Mask;
  if((count % 8)!=0)
      nbyte++;
  if(debug)
     printf("read coil status start:%d count:%d \n\r",start,count);
  if((start+count) > COILS_MAX)
      return MB_ERROR_ILLEGAL_DATA_ADDRESS;
  mb_response_buf[2]=nbyte;
  for(loop=0;loop<count;loop++)
   {
      value |= gpio_get(Coils[start+loop])? mask : 0;
      mask *=2;
      if(((loop %8)== 7) || (loop == (count-1)))
       {
         mb_response_buf[mb_response_buf_pos++]= value;
         value = 0;
         mask=1;
       }
   }
  printResponse(2);
   return MB_NO_ERROR;
}

uint8_t ModbusPico::mb_write_single_coil(uint16_t start, uint16_t value) {
  if(debug)
     printf("Write SingleCoil start:%u  gpio:%u value:%u\n\r",start,value,Coils[start]);
  if(start>= COILS_MAX)
      return MB_ERROR_ILLEGAL_DATA_ADDRESS;
  
  gpio_put(Coils[start],value != 0 ? 1 : 0);
  mb_response_add_single_register(start);
  mb_response_buf_pos++;
  mb_response_add_single_register(value);
  return MB_NO_ERROR;
}

uint8_t ModbusPico::mb_write_single_register(uint16_t start, uint16_t value) {
  // if(debug)
  //    printf("Write SingleCoil start:%u  gpio:%u value:%u\n\r",start,value,Coils[start]);
  // if(start>= COILS_MAX)
  //     return MB_ERROR_ILLEGAL_DATA_ADDRESS;
  
  // gpio_put(Coils[start],value != 0 ? 1 : 0);
  // mb_response_add_single_register(start);
  // mb_response_buf_pos++;
  // mb_response_add_single_register(value);

    uint16_t val;
    uint16_t addr = start;

    switch (addr) 
    {
      case MB_COMMAND_REGISTER:
          command = value;
          break;

      case MB_COMMAND_PARAM_0_REGISTER:
          command_param[0] = value;
          break;

      case MB_COMMAND_PARAM_1_REGISTER:
          command_param[1] = value;
          break;

      case MB_COMMAND_PARAM_2_REGISTER:
          command_param[2] = value;
          break;
        
      case MB_COMMAND_PANEL_LED_0_OUTPUT_REGISTER:
      case MB_COMMAND_PANEL_LED_1_OUTPUT_REGISTER:
      case MB_COMMAND_PANEL_LED_2_OUTPUT_REGISTER:
      case MB_COMMAND_PANEL_LED_3_OUTPUT_REGISTER:
      case MB_COMMAND_PANEL_LED_4_OUTPUT_REGISTER:
      case MB_COMMAND_PANEL_LED_5_OUTPUT_REGISTER:
      case MB_COMMAND_PANEL_LED_6_OUTPUT_REGISTER:
      case MB_COMMAND_PANEL_LED_7_OUTPUT_REGISTER:
      {
        uint16_t temp = addr - MB_COMMAND_PANEL_LED_0_OUTPUT_REGISTER;
        panel_led_values[temp] = value & 0xFF;
        break;
      }

      default:
          return MB_ERROR_ILLEGAL_DATA_ADDRESS;
    }
    
    mb_response_add_single_register(start);
    mb_response_buf_pos++;
    mb_response_add_single_register(value);


  return MB_NO_ERROR;
}

uint8_t ModbusPico::mb_write_multiple_coils(uint16_t start, uint8_t* values, uint16_t len) {
  uint8_t mask=1;
  uint8_t value=0;
  int loop;
  // first values is number of byte 

  int nbyte =   *(values++);
  if(debug)
    {
      printf("write multiple coils start=%d len=%d values= ",start,len);
      for(loop=0;loop<len;loop++)
      {
        printf(" %02X ",values[loop]);
      }
      printf("\n\r");
      printResponse(11);
    }
  if((start+len) > COILS_MAX)
      return MB_ERROR_ILLEGAL_DATA_ADDRESS;
  mb_response_buf[2]=nbyte;
  for(loop=0;loop<len;loop++)
   {
      if(debug)
          printf("out%d: %s\n\r",loop+start, *values & mask ? "ON" : "OFF");
      gpio_put(Coils[start+loop], *values & mask ? 1 : 0);
      mask *=2;
      if((loop %16)== 7)
       {
         values++;
         mask=1;
       }
   }
  printResponse(12);

  mb_response_add_single_register(start);
  mb_response_buf_pos++;
  mb_response_add_single_register(len);
  printResponse(13);
  return MB_NO_ERROR;
}


void ModbusPico::mb_init(uint8_t slave_address, uint8_t uart_num,
                        uint32_t baudrate, uint8_t data_bits, uint8_t stop_bits, uart_parity_t parity,
                        uint8_t rx_pin, uint8_t tx_pin, uint8_t de_pin)
 {
     int loop;
     if(debug)
         printf("init\n\r");

     if(debug)
         printf("initialize coils\n\r");

    // enable Coils
    for(loop=0;loop<COILS_MAX;loop++)
    {
      gpio_init(Coils[loop]);
      gpio_set_dir(Coils[loop], GPIO_OUT);
    }



    if(debug)
        printf("Read Unique ID: 0x");
    pico_unique_board_id_t board_id;
    pico_get_unique_board_id(&board_id);
    for(loop=0;loop<4;loop++)
      UniqueID[loop] = board_id.id[loop*2+1] | (board_id.id[loop*2]<<8);
    if(debug)
    {
      for(loop=0;loop<PICO_UNIQUE_BOARD_ID_SIZE_BYTES;loop++)
        printf("%02X",board_id.id[loop]);
//           printf("%02X",board_id.id[PICO_UNIQUE_BOARD_ID_SIZE_BYTES - loop -1]);
      printf("\n\r");
    }
    ModbusManager::mb_init(slave_address, uart_num, baudrate, data_bits, stop_bits, parity,
        rx_pin, tx_pin, de_pin);

 }


