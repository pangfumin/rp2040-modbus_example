 #pragma once

#include "modbus.hpp"
#include "panel_io.hpp"
#include <vector>

class ModbusPico : public ModbusManager
{
public:
    static const uint16_t UNIT_TYPE=2280;
    static const uint16_t SOFTWARE_VERSION=0x0300;
    static bool debug;
    static const int COILS_MAX=1;

    static uint8_t Coils[COILS_MAX];

    // _t means temporary
    //  to sync whit second core


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

    // panel register
    static const uint16_t MB_COMMAND_PANEL_REGISTER_START=1100;
    static const uint16_t MB_COMMAND_PANEL_SWITCH_0_INPUT_REGISTER=1100;
    static const uint16_t MB_COMMAND_PANEL_SWITCH_1_INPUT_REGISTER=1101;
    static const uint16_t MB_COMMAND_PANEL_SWITCH_2_INPUT_REGISTER=1102;
    static const uint16_t MB_COMMAND_PANEL_SWITCH_3_INPUT_REGISTER=1103;
    static const uint16_t MB_COMMAND_PANEL_SWITCH_4_INPUT_REGISTER=1104;
    static const uint16_t MB_COMMAND_PANEL_SWITCH_5_INPUT_REGISTER=1105;

    static const uint16_t MB_COMMAND_PANEL_LED_0_OUTPUT_REGISTER=1108;
    static const uint16_t MB_COMMAND_PANEL_LED_1_OUTPUT_REGISTER=1109;
    static const uint16_t MB_COMMAND_PANEL_LED_2_OUTPUT_REGISTER=1110;
    static const uint16_t MB_COMMAND_PANEL_LED_3_OUTPUT_REGISTER=1111;
    static const uint16_t MB_COMMAND_PANEL_LED_4_OUTPUT_REGISTER=1112;
    static const uint16_t MB_COMMAND_PANEL_LED_5_OUTPUT_REGISTER=1113;
    static const uint16_t MB_COMMAND_PANEL_LED_6_OUTPUT_REGISTER=1114;
    static const uint16_t MB_COMMAND_PANEL_LED_7_OUTPUT_REGISTER=1115;

    static uint8_t led_history_ringbuffer[8][32];
    static uint8_t led_history_ringbuffer_index[8];
    static uint8_t led_history_one_count[8][8]; // per bit

    uint16_t UniqueID[4];

    void mb_init(uint8_t slave_address, uint8_t uart_num,
             uint32_t baudrate, uint8_t data_bits, uint8_t stop_bits, uart_parity_t parity,
             uint8_t rx_pin, uint8_t tx_pin, uint8_t de_pin);
protected:
    void printResponse(int v);
    uint8_t mb_write_single_coil(uint16_t start, uint16_t value);
    uint8_t mb_write_multiple_coils(uint16_t start, uint8_t* values, uint16_t len);
    uint8_t mb_write_single_register(uint16_t start, uint16_t value);
    uint8_t mb_read_coil_status(uint16_t start, uint16_t count);
    // uint8_t mb_read_input_status(uint16_t start, uint16_t count);
    uint8_t mb_read_holding_register(uint16_t addr, uint16_t* reg);
    uint8_t mb_read_holding_registers(uint16_t start, uint16_t count);
    uint8_t mb_read_input_registers(uint16_t start, uint16_t count);
    uint8_t mb_validate_input_register(uint16_t address, uint16_t * reg);
};



