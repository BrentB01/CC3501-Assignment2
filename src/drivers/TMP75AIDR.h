#pragma once
#include <cstdint>        // Ensure this is included for uint32_t
#include "hardware/pio.h" // Include for PIO
#include "hardware/i2c.h" // Include for I2C functionality
#include <cstdarg>        // For variadic functions

void TEMP_init();
uint16_t read_temp_register(i2c_inst_t* i2c, uint8_t reg);
float convert_to_celsius(uint16_t raw_value);