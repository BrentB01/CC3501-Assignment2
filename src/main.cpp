#include <stdio.h>
#include "pico/stdlib.h"
#include <cstdarg>
#include <cstdio>
#include <cstdint>
#include "pico/stdlib.h"


#include "drivers/TMP75AIDR.cpp"
#include "drivers/TMP75AIDR.h"
#include "drivers/leds.h"
#include "drivers/logging/logging.h"
#include "drivers/OPTO3001.h"
#include "drivers/ir_driver.h"

#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "WS2812.pio.h"

// IR Driver Definitions
#define IR_PIN 14  // GPIO for IR transmission
#define CARRIER_FREQUENCY 38000
#define DUTY_CYCLE_PERCENT 33

// Temp Sensor Definitions 
#define TEMP_SDA 6
#define TEMP_SCL 7
#define TEMP_I2C_SLAVE 0x48 // TMP75 I2C address
#define RESULT_REGISTER 0x00 // Temperature register address

// Ambient Light Sensor
#define ALS_I2C_ADDRESS 0x44 // 7-bit I2C address (0b1000100)
#define RESULT_REGISTER 0x00       // Result register address
#define CONFIG_REGISTER 0x01       // Configuration register address

// LED Definitions 
#define LED_PIN 29
#define NUM_LEDS 1

// IR Sensor
#define IR_SENSOR 25

// Buttons
#define SW1 2

// IR Emmitter 
#define IR_Emmit 14

// Bluetooth 
#define BLT_RST 16 
#define BLT_CS 15 
#define BLT_INT 17 
#define BLT_TX 5
#define BLT_RX 4
#define UART_ID uart1               // Use UART1
#define BAUD_RATE 115200            // Ensure this matches the Raspberry Pi
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY UART_PARITY_NONE

using namespace std;

int main() {
    stdio_init_all(); // Initialize standard I/O for serial communication

    // Initialize I2C for the TMP75 sensor
    TEMP_init();      
    
    // Initialize ALS 
    ALS_init();

    // Setup PWM for IR transmission
    setup_pwm(IR_PIN, CARRIER_FREQUENCY, DUTY_CYCLE_PERCENT);
    sleep_ms(1000);  // Wait before sending signal

    // Initilize LED
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN,GPIO_OUT);

    // Initialize button SW1
    gpio_init(SW1);
    gpio_set_dir(SW1, GPIO_IN);

    // Variables to track button and LED state
    int sw1_press_count = 0; // Tracks how many times SW1 has been pressed
    bool sw1_pressed = false; // Tracks if the SW1 button was pressed (for edge detection)

    // Main loop to continuously read and display the temperature
    while (true) {
        // Read the raw temperature value from the TMP75 sensor
        uint16_t raw_temp = read_temp_register();

        // Convert the raw temperature value to Celsius
        float temperature = convert_to_celsius(raw_temp);
        
        // Print the temperature to the terminal
        printf("Temperature: %.2f°C\n", temperature);
        
        // Check the temperature conditions
        if (temperature > 29.0) {
            send_ir_signal(IR_PIN, ir_data_on, sizeof(ir_data_on) / sizeof(ir_data_on[0]));
        } else if (temperature < 27.0) {
            send_ir_signal(IR_PIN, ir_data_off, sizeof(ir_data_off) / sizeof(ir_data_off[0]));
        }

        // Check if SW1 is pressed (active low, so gpio_get(SW1) == 0 when pressed)
        if (gpio_get(SW1) == 0 && !sw1_pressed) {
            // Button press detected (rising edge)
            sw1_pressed = true;
            sw1_press_count = (sw1_press_count + 1) % 3; // Increment press count and wrap around every 4 presses

            // Handle behavior based on press count
            if (sw1_press_count == 1) {
                // First press: Manual mode, LED on
                gpio_put(LED_PIN, true);
                printf("Manual Mode Activated: LED turned ON.\n");

            } else if (sw1_press_count == 2) {
                // Second press: Manual mode, LED off
                gpio_put(LED_PIN, false);
                printf("Manual Mode Activated: LED turned OFF.\n");

            }  else if (sw1_press_count == 0) {
                // Fourth press: Revert to automatic lighting control mode
                printf("Automatic Mode Activated\n");
            }
        }

        // Reset button state when released
        if (gpio_get(SW1) != 0) {
            sw1_pressed = false;
        }

        // Automatic mode (sw1_press_count == 0) controls LED with ambient light sensor
        if (sw1_press_count == 0) {
            float LUX = ALS_read();
            if (LUX <= 100) {
                gpio_put(LED_PIN, true);  // Turn LED on if ambient light is low
                printf("Ambient Light: %.2f LUX, LED turned ON (Low Light).\n", LUX);
            } else {
                gpio_put(LED_PIN, false); // Turn LED off if ambient light is sufficient
                printf("Ambient Light: %.2f LUX, LED turned OFF (Sufficient Light).\n", LUX);
            }
        }

        sleep_ms(100); // Wait for 100ms before checking again
    }

    return 0;
}
