/*
 * Modbus master (client) with libmodbus demo  for RS485 HAT 
 * https://www.hwhardsoft.de/english/projects/rs485-shield/
 * 
 * Version 1.0
 * Copyright (C) 2023  Hartmut Wendt  www.zihatec.de
 * 
 * 
 * License: GPLv3 https://www.gnu.org/licenses/gpl.txt
 */



#include <stdio.h>
#include <modbus.h>
#include <errno.h>

int main(void) {
  modbus_t *ctx = NULL;
  ctx = modbus_new_rtu("/dev/ttyUSB0",115200, 'N', 8, 1);
  uint16_t tab_reg[32] = {0};
  int rc;
  int i;

  modbus_set_slave(ctx, 1);  // define slave address

  modbus_rtu_set_serial_mode(ctx, MODBUS_RTU_RS232);
  modbus_rtu_set_rts(ctx, MODBUS_RTU_RTS_NONE); // we use auto RTS function by the HAT
  printf("debug 0 \n");
  modbus_set_response_timeout(ctx, 0, 100000000);
  printf("debug 1\n");
  if (modbus_connect(ctx) == -1)
  {
    fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));   
    modbus_free(ctx);	
    return -1;
  }

  printf("debug 2\n");
  // read register 5 registers (starting at address 1) of slave 1
  rc = modbus_read_registers(ctx, 1105, 1, tab_reg);
  if (rc == -1) {
    fprintf(stderr, "%s\n", modbus_strerror(errno));
    return -1;
  }

  printf("debug 3\n");
  
  for (i=0; i < rc; i++) {
    printf("reg[%d]=%d (0x%X)\n", i, tab_reg[i], tab_reg[i]);
  }


  modbus_close(ctx);
  modbus_free(ctx);
}
