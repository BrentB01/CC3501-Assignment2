#pragma once
#include <cstdint>        // Ensure this is included for uint32_t
#include "hardware/pio.h" // Include for PIO
#include <cstdarg>        // For variadic functions



void leds_init();
void leds_set(uint led_index,uint8_t red,uint8_t green, uint8_t blue);
void leds_update();
void leds_reset();
