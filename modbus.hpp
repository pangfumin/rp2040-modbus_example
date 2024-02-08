#pragma once

#include <stdint.h>
#include <hardware/uart.h>

// Constants


extern void on_mb_rx();

typedef enum { MB_DATA_READY, MB_DATA_INCOMPLETE, MB_INVALID_SLAVE_ADDRESS, MB_INVALID_FUNCTION } mb_state_t;

//#define MB_RX_BUF_SIZE 64
//#define MB_TX_BUF_SIZE 64

//#define MB_TIMEOUT       3

// at 115200 baud 3.5 character is  304 us
#define MB_TIMEOUT_US 350
class ModbusManager
{
public:
    void mb_init(uint8_t slave_address, uint8_t uart_num, 
                uint32_t baudrate, uint8_t data_bits, uint8_t stop_bits, uart_parity_t parity, 
                uint8_t rx_pin, uint8_t tx_pin, uint8_t de_pin);

    void mb_rx(uint8_t data);
    void mb_process();

    uint8_t uart_number;

    //dummy data stored here
    uint16_t state              = 0;
    uint16_t error_code         = 10;
    uint16_t busy_code          = 20;

    uint16_t command            = 100;
    uint16_t command_param[3]   = {0};

    uint16_t sensor_0           = 1000;
    uint16_t sensor_1           = 1001;
    uint16_t sensor_2           = 1002;

    // constant definition
   static const int MB_RX_BUF_SIZE=64;
   static const int MB_TX_BUF_SIZE=64;


    static const uint8_t MB_NO_ERROR=0x00;
    static const uint8_t MB_ERROR_ILLEGAL_FUNCTION=0x01;
    static const uint8_t MB_ERROR_ILLEGAL_DATA_ADDRESS=0x02;
    static const uint8_t MB_ERROR_ILLEGAL_DATA_VALUE=0x03;
    static const uint8_t MB_ERROR_SLAVE_DEVICE_FAILURE=0x04;
    // Commands
    static const uint8_t MB_READ_COIL_STATUS=0x01;
    static const uint8_t MB_READ_INPUT_STATUS=0x02;
    static const uint8_t MB_READ_HOLDING_REGISTERS=0x03;
    static const uint8_t MB_READ_INPUT_REGISTERS=0x04;
    static const uint8_t MB_WRITE_SINGLE_COIL=0x05;
    static const uint8_t MB_WRITE_SINGLE_REGISTER=0x06;
    static const uint8_t MB_WRITE_MULTIPLE_COILS=0x0F;
    static const uint8_t MB_WRITE_MULTIPLE_REGISTERS=0x10;
    // Registers adresses
    static const uint16_t MB_STATE_REGISTER=0;
    static const uint16_t MB_ERROR_CODE_REGISTER=10;
    static const uint16_t MB_BUSY_CODE_REGISTER=20;
    static const uint16_t MB_COMMAND_REGISTER=100;
    static const uint16_t MB_COMMAND_PARAM_0_REGISTER=110;
    static const uint16_t MB_COMMAND_PARAM_1_REGISTER=111;
    static const uint16_t MB_COMMAND_PARAM_2_REGISTER=112;

    static const uint16_t MB_SENSOR_0_REGISTER=1000;
    static const uint16_t MB_SENSOR_1_REGISTER=1001;
    static const uint16_t MB_SENSOR_2_REGISTER=1002;


protected:
    void mb_response_add(uint16_t value);
    void mb_response_add_single_register(uint16_t value);
    void mb_response_reset(uint8_t fn);

    void mb_tx(uint8_t* data, uint32_t size);

    uint32_t mb_get_tick_ms(void);

    uint16_t mb_calc_crc16(const uint8_t* buf, uint8_t len);
    mb_state_t mb_check_buf();
    void mb_reset_buf(); 
    void mb_error(uint8_t err);
    void mb_rx_rtu();
    void mb_response_tx();

    uint8_t de_pin;
    uint8_t mb_slave_address = 0;

    uint8_t mb_request_buf[MB_RX_BUF_SIZE];
    uint8_t mb_response_buf[MB_TX_BUF_SIZE];

    int mb_last_request_buf_pos = 0;
    int mb_request_buf_pos = 0;
    int mb_response_buf_pos = 0;

    uint64_t mb_timeout;

    virtual uint8_t mb_read_coil_status(uint16_t start, uint16_t count);
    virtual uint8_t mb_read_input_status(uint16_t start, uint16_t count);
    virtual uint8_t mb_read_holding_registers(uint16_t start, uint16_t count);
    virtual uint8_t mb_read_input_registers(uint16_t start, uint16_t count);
    virtual uint8_t mb_write_single_coil(uint16_t start, uint16_t value);
    virtual uint8_t mb_write_single_register(uint16_t start, uint16_t value);
    virtual uint8_t mb_write_multiple_coils(uint16_t start, uint8_t* values, uint16_t len);
    virtual uint8_t mb_write_multiple_registers(uint16_t start, uint16_t* values, uint16_t len);
    virtual uint8_t mb_read_holding_register(uint16_t addr, uint16_t* reg);



};

