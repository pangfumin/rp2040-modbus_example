#pragma once
#include "hardware/i2c.h"

extern const uint sda_pin;
extern const uint scl_pin;
extern i2c_inst_t * i2c;

void i2cWrite(uint8_t addr,uint8_t * p_data, uint8_t data_size,uint8_t nostop);
void i2cRead(uint8_t addr,uint8_t  * p_data, uint8_t data_size);

void spiWrite(uint8_t * p_data,uint8_t data_size);
void spiRead(uint8_t * p_data,uint8_t data_size);


