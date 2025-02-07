 #pragma once

#include "modbus.hpp"
#include <vector>

class ModbusPico : public ModbusManager
{
public:
    static const uint16_t UNIT_TYPE=2280;
    static const uint16_t SOFTWARE_VERSION=0x0300;
    static bool debug;
    static const int COILS_MAX=8;
    static const int INPUTS_MAX=8;
    static const int DS18B20_MAX=16;
    static const int ADC_MAX=5;
    static const int BME280_MAX=2; // two sensors


    // be aware than adc3 is vsys * 2/3 only on pico
    //  adc4 is the onboard temperature
    // all adc returns the raw 12bits

    static uint8_t Coils[COILS_MAX];
    static uint8_t Inputs[INPUTS_MAX];
    static uint8_t Adc[ADC_MAX];


    uint16_t dsSensorCount;

    int16_t bme280Sensors[BME280_MAX*2*3];
    uint16_t bme280Valid;
    int16_t dsSensors[DS18B20_MAX];
    uint16_t bme280_ID[BME280_MAX];

    // _t means temporary
    //  to sync whit second core
    int16_t _t_dsSensors[DS18B20_MAX];
    int16_t _t_bme280Sensors[BME280_MAX*2*3];
    uint16_t _t_bme280_ID[BME280_MAX];
    int16_t dsSensorsAddress[DS18B20_MAX * 4];
    // holdin register add on
    // unique board ID register0 is bit 0..15 register3 is bit 48..64
    static const uint16_t MB_COMMAND_UNIQUE_ID_REGISTER0=200;
    static const uint16_t MB_COMMAND_UNIQUE_ID_REGISTER1=201;
    static const uint16_t MB_COMMAND_UNIQUE_ID_REGISTER2=202;
    static const uint16_t MB_COMMAND_UNIQUE_ID_REGISTER3=203;

    static const uint16_t MB_COMMAND_UNIT_TYPE_ID_REGISTER=204;
    static const uint16_t MB_COMMAND_UNIT_VERSION_ID_REGISTER=205;

    static const uint16_t MB_COMMAND_UNIT_TYPE_ID_STRING_REGISTER=300;
    static const char * InstrumentIdString;


    // DS18B20 sensor register
    static const uint16_t MB_COMMAND_DS18B20_REGISTER_COUNT=2000;
    static const uint16_t MB_COMMAND_DS18B20_REGISTER=2001; // up to 2016
    static const uint16_t MB_COMMAND_DS18B20_REGISTER_ADDRESS=2020; // up to 2016
    static const uint16_t MB_COMMAND_MB280_ID_REGISTER=2200; //
    static const uint16_t MB_COMMAND_MB280_1_REGISTER=2210; //
    static const uint16_t MB_COMMAND_MB280_2_REGISTER=2220; //

    // ADC
    static const uint16_t MB_COMMAND_ADC_REGISTER=2100;  //up to 2104
    // *** special register 2105 is the onboard temperature in celsius * 10.0

    uint16_t UniqueID[4];

    void mb_init(uint8_t slave_address, uint8_t uart_num,
             uint32_t baudrate, uint8_t data_bits, uint8_t stop_bits, uart_parity_t parity,
             uint8_t rx_pin, uint8_t tx_pin, uint8_t de_pin);
protected:
    void printResponse(int v);
    uint8_t mb_write_single_coil(uint16_t start, uint16_t value);
    uint8_t mb_write_multiple_coils(uint16_t start, uint8_t* values, uint16_t len);
    uint8_t mb_read_coil_status(uint16_t start, uint16_t count);
    uint8_t mb_read_input_status(uint16_t start, uint16_t count);
    uint8_t mb_read_holding_register(uint16_t addr, uint16_t* reg);
    uint8_t mb_read_holding_registers(uint16_t start, uint16_t count);
    uint8_t mb_read_input_registers(uint16_t start, uint16_t count);
    uint8_t mb_validate_input_register(uint16_t address, uint16_t * reg);
};



