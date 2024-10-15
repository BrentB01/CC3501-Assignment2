#pragma once
#include <cstdint>        // Ensure this is included for uint32_t
#include "hardware/pio.h" // Include for PIO
#include <cstdarg>        // For variadic functions

extern uint8_t length; // Declare it as extern

void accel_init();
bool accel_write_register(uint8_t reg, uint8_t data);
bool accel_read(uint8_t reg, uint8_t);
void accel_print();