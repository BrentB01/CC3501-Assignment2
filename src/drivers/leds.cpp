#include "leds.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "WS2812.pio.h"
#include <cstdarg>
#include <cstdio>

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"

#include "WS2812.pio.h"
#include "drivers/logging/logging.h"
#include <cstdint>

#define LED_PIN 14
#define NUM_LEDS 12



//initialise 
// setting 
// update
// turn off
//Declare function

uint32_t led_data[12]; // Array for 12 LEDs 

void leds_init(){
stdio_init_all();
// Initialise PIO0 to control the LED chain
    uint32_t pio_program_offset = pio_add_program(pio0, &ws2812_program);
    ws2812_program_init(pio0, 0, pio_program_offset, LED_PIN, 800000, false);
}

void leds_set(uint led_index,uint8_t red,uint8_t green, uint8_t blue){
        // Create LED data
        led_data[led_index] = (red << 24) | (green << 16) | (blue << 8); // First LED data
}

void leds_update(){
 // Send data to all LEDs
        for (int i = 0; i < NUM_LEDS; i++) {
            pio_sm_put_blocking(pio0, 0, led_data[i]);
        }
    sleep_ms(500);

}

// Function to reset all LEDs (turn them off)
void leds_reset()
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        led_data[i] = 0; // Set all LEDs to off
    }
    
    // Send the reset data to all LEDs
    for (int i = 0; i < NUM_LEDS; i++)
    {
        pio_sm_put_blocking(pio0, 0, led_data[i]);
    }
}
