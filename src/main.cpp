// ALS Sensor

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/i2c.h"


#include "WS2812.pio.h"
#include "drivers/logging/logging.h"
#include "drivers/leds.h"
#include "drivers/accelerometer.h"



 

#include <cstdarg>
#include <cstdio>
#include <cstdint>

// Define GPIO pin for button
#define BUTTON_PIN 15

#define LED_PIN 14
#define NUM_LEDS 12

#define ACCEL_INSTANCE i2c0 // Use the first I2C instance in the chain
#define SDA_PIN 16          // Pin from schematic
#define SCL_PIN 17          // Pin from schematic
#define I2C_ADDRESS 0x19    // Hex address of Accel
#define OUT_X_L 0x28
#define OUT_X_H 0x29
#define OUT_Y_L 0x2A
#define OUT_Y_H 0x2B
#define OUT_Z_L 0x2C
#define OUT_Z_H 0x2D
#define CTRLREG1 0x20
#define CTRLDATA 0b01111111

uint8_t length = 1;
uint8_t data[6]; // Array to store accelerometer X, Y, Z data (assuming 6 bytes)

int16_t button_last_state = 0;  // Store last button state

// Function prototypes
void check_button();
void run_lab7();
void run_lab8();

int main() {
    stdio_init_all();
    
    // Initialize the button GPIO pin
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);  // Enable internal pull-up resistor
    
    leds_init();
    
    while (true) {
        check_button();  // Check if the button was pressed

        switch (button_last_state) {
            case 0:
                run_lab7();  
                break;
            case 1:
                run_lab8(); // Run Lab 8 (accelerometer-controlled LEDs)
                break;
            case 2: 
                break;
        }


        sleep_ms(100);  // Small delay to avoid button bouncing issues
    }

    return 0;
}

