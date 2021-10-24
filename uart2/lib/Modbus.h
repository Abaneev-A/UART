#pragma once
#include <stdint.h>
//#include <iostream>

#define NUMBER_REG 4
#define MAX_REG_ADR 3
#define MAX_NUMBER 1000

class Modbus
{
    uint8_t address;

    void code3(uint8_t* RX, uint8_t* TX, uint16_t* storage, uint16_t& sizeTX);

    void code6(uint8_t* RX, uint8_t* TX, uint16_t* storage, uint16_t& sizeTX);

    void code10(uint8_t* RX, uint8_t* TX, uint16_t* storage, uint16_t& sizeTX);

    void error(uint8_t code, uint8_t* TX, uint16_t& sizeTX, uint16_t exception_code);

    uint16_t reg_adress(uint8_t* RX);

    uint16_t num_register(uint8_t* RX);

    uint16_t new_number(uint8_t* RX);

    uint8_t get_high_byte(uint16_t word);

    uint8_t get_low_byte(uint16_t word);

    uint16_t get_word(uint8_t high, uint8_t low);

    void add_crc(uint8_t* TX, uint8_t);

public:

    Modbus(uint8_t adress);
    
    void parsing(uint8_t* RX, uint8_t* TX, uint16_t* storage, uint16_t sizeRX, uint16_t& sizeTX);

    uint16_t crc_16(uint8_t* buffer, uint16_t buffer_size);
    
};
