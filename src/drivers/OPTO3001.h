#pragma once
#include <cstdint>        // Ensure this is included for uint32_t
#include "hardware/pio.h" // Include for PIO
#include "hardware/i2c.h" // Include for I2C functionality
#include <cstdarg>        // For variadic functions

// void init_opt3001(i2c_inst_t* i2c);
void ALS_init();
void write_register(i2c_inst_t* i2c, uint8_t reg, uint16_t value);
uint16_t read_register(i2c_inst_t* i2c, uint8_t reg);
