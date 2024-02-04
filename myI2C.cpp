#include "myI2C.hpp"

const uint sda_pin= 10;
const uint scl_pin= 11;

i2c_inst_t * i2c= i2c1;

void i2cWrite(uint8_t addr,uint8_t * p_data, uint8_t data_size,uint8_t nostop)
{
   i2c_write_blocking(i2c,addr,p_data,data_size,nostop ? true : false);
}

void i2cRead(uint8_t addr,uint8_t  * p_data, uint8_t data_size)
{
   i2c_read_blocking(i2c,addr,p_data,data_size,false);
}


// dummy spi
void spiWrite(uint8_t * p_data,uint8_t data_size)
{
  return;
}

void spiRead(uint8_t * p_data,uint8_t data_size)
{
  return;
}






