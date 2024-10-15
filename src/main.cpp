#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "drivers/leds.h"
#include "drivers/accelerometer.h"
#include "hardware/i2c.h"

#include "WS2812.pio.h"
#include "drivers/logging/logging.h"

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

// Function to handle the button press and toggle labs 
void check_button() {
    bool button_state = gpio_get(BUTTON_PIN);  // Active low (pressed = 0)
    if (button_state) {  // If button is pressed (and was not previously pressed)
        //Move to the next case of the system (the next task)
        if(button_last_state == 2){
            button_last_state = 0;
        } else{
            button_last_state++;
        }
        sleep_ms(300);  // Debounce delay to prevent multiple toggles on a single press
    }
    button_last_state = button_state;  // Update last button state
}

// Lab 7 code block
void run_lab7() {
    uint8_t red = 0, green = 0, blue = 0;

    // Loop through and set LEDs like Lab 7
    for (int i = 0; i < NUM_LEDS; i++) {
        red = i * (255 / 12);
        blue = 255 - red;
        green = 0;
        leds_set(i, red, green, blue);
        check_button();
        leds_update();
        sleep_ms(5);
    }
    leds_reset();

    for (int i = 0; i < NUM_LEDS; i++) {
        uint8_t b = 12 - i;
        blue = i * (255 / 12);
        red = 255 - blue;
        green = 0;
        leds_set(b, red, green, blue);
        check_button();
        leds_update();
        sleep_ms(5);
    }
    leds_reset();

    // Add a check after each cycle to see if the button is pressed
    check_button();  // Ensure the button can be pressed to switch to Lab 8
}

// Lab 8 code block
void run_lab8() {
    accel_init();
    accel_write_register(CTRLREG1, CTRLDATA);

    // Read the most and least significant bits from each axis
    accel_read(OUT_X_L, 1);
    uint8_t x_lsb = data[0];
    accel_read(OUT_X_H, 1);
    uint8_t x_msb = data[0];

    accel_read(OUT_Y_L, 1);
    uint8_t y_lsb = data[0];
    accel_read(OUT_Y_H, 1);
    uint8_t y_msb = data[0];

    accel_read(OUT_Z_L, 1);
    uint8_t z_lsb = data[0];
    accel_read(OUT_Z_H, 1);
    uint8_t z_msb = data[0];

    // Combine LSB and MSB
    int16_t x_accel_raw = (int16_t)((x_msb << 8) | x_lsb);
    int16_t y_accel_raw = (int16_t)((y_msb << 8) | y_lsb);
    int16_t z_accel_raw = (int16_t)((z_msb << 8) | z_lsb);

    // Convert raw data to mg
    float x_accel = x_accel_raw * 4 / 1000.0;
    float y_accel = y_accel_raw * 4 / 1000.0;
    float z_accel = z_accel_raw * 4 / 1000.0;

    // Handle X-axis LEDs
    if (x_accel > 20) leds_set(7, 100, 0, 0);
    else if (x_accel > 1.5 && x_accel <= 20) leds_set(6, 100, 0, 0);
    else if (x_accel >= -1.5 && x_accel <= 1.5) {
        leds_set(4, 0, 100, 0);
        leds_set(5, 0, 100, 0);
        leds_set(6, 0, 100, 0);
        leds_set(7, 0, 100, 0);
    } else if (x_accel < -1.5 && x_accel > -20) leds_set(5, 100, 0, 0);
    else if (x_accel <= -20) leds_set(4, 100, 0, 0);

    // Handle Y-axis LEDs
    if (y_accel > 20) leds_set(8, 100, 0, 0);
    else if (y_accel > 1.5 && y_accel <= 20) leds_set(9, 100, 0, 0);
    else if (y_accel >= -1.5 && y_accel <= 1.5) {
        leds_set(8, 0, 100, 0);
        leds_set(9, 0, 100, 0);
        leds_set(10, 0, 100, 0);
        leds_set(11, 0, 100, 0);
    } else if (y_accel < -1.5 && y_accel > -20) leds_set(10, 100, 0, 0);
    else if (y_accel <= -20) leds_set(11, 100, 0, 0);

    leds_update();
    sleep_ms(20);
    leds_reset();

    // Print the result
    printf("X-axis acceleration: %.4f g\n", x_accel);
    printf("Y-axis acceleration: %.4f g\n", y_accel);
    printf("Z-axis acceleration: %.4f g\n", z_accel);

    // Add a check after each cycle to see if the button is pressed
    check_button();  // Ensure the button can be pressed to switch to Lab 7
}
