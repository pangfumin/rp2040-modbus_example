 #pragma once

#include "modbus.hpp"
#include <vector>


class ModbusPico : public ModbusManager
{
public:
    static bool debug;
    static const int COILS_MAX=8;
    static const int INPUTS_MAX=8;

    static uint8_t Coils[COILS_MAX];
    static uint8_t Inputs[INPUTS_MAX];

    void mb_init(uint8_t slave_address, uint8_t uart_num,
             uint32_t baudrate, uint8_t data_bits, uint8_t stop_bits, uart_parity_t parity,
             uint8_t rx_pin, uint8_t tx_pin, uint8_t de_pin);
protected:
    void printResponse(int v);
    uint8_t mb_write_single_coil(uint16_t start, uint16_t value);
    uint8_t mb_write_multiple_coils(uint16_t start, uint8_t* values, uint16_t len);
    uint8_t mb_read_coil_status(uint16_t start, uint16_t count);
    uint8_t mb_read_input_status(uint16_t start, uint16_t count);

};



