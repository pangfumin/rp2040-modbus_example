
#include <stdio.h>
#include <string.h>

#include <hardware/irq.h>
#include <hardware/uart.h>
#include <pico/stdlib.h>
#include "ModbusPico.hpp"



bool ModbusPico::debug=true;
uint8_t ModbusPico::Coils[COILS_MAX]={25,3,4,5,6,7,8,9};
uint8_t ModbusPico::Inputs[INPUTS_MAX]={14,15,16,17,18,19,20,21};

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

__attribute__((weak))
uint8_t ModbusPico::mb_read_input_status(uint16_t start, uint16_t count) {
  printResponse(1);
  uint8_t mask=1;
  uint8_t value=0;
  int loop;
  int nbyte = count/8;
  uint8_t  Mask;
  if((count % 8)!=0)
      nbyte++;
  if(debug)
    printf("read input status start:%d count:%d \n\r",start,count);
  if((start+count) > INPUTS_MAX)
      return MB_ERROR_ILLEGAL_DATA_ADDRESS;
  mb_response_buf[2]=nbyte;
  for(loop=0;loop<count;loop++)
   {
      value |= gpio_get(Inputs[start+loop])? mask : 0;
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


__attribute__((weak))
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

__attribute__((weak))
uint8_t ModbusPico::mb_write_single_coil(uint16_t start, uint16_t value) {
  if(debug)
     printf("Write SingleCoil start:%u  value:%u\n\r",start,value);
  if(start>= COILS_MAX)
      return MB_ERROR_ILLEGAL_DATA_ADDRESS;
  gpio_put(Coils[start],value != 0 ? 1 : 0);
  mb_response_add_single_register(start);
  mb_response_buf_pos++;
  mb_response_add_single_register(value);
  return MB_NO_ERROR;
}

__attribute__((weak))
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
     ModbusManager::mb_init(slave_address, uart_num, baudrate, data_bits, stop_bits, parity,
                            rx_pin, tx_pin, de_pin);

    // enable Coils
    for(loop=0;loop<COILS_MAX;loop++)
      {
         gpio_init(Coils[loop]);
         gpio_set_dir(Coils[loop], GPIO_OUT);
      }

    for(loop=0;loop<INPUTS_MAX;loop++)
      {
         gpio_init(Coils[loop]);
         gpio_set_dir(Coils[loop], GPIO_IN);
      }
 }


