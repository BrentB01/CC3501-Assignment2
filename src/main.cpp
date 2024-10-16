
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <cstdint>        // Ensure this is included for uint32_t
#include "hardware/pio.h" // Include for PIO
#include <cstdarg>        // For variadic functions
#include "drivers/OPTO3001.h"
#include "hardware/gpio.h"
#include "WS2812.pio.h"
#include "drivers/logging/logging.h"
#include "drivers/leds.h"
#include <cstdarg>
#include <cstdio>
#include <cstdint>

#define LED_PIN 18 // Note: LED will only work off battery power
#define SW1 2
#define SW2 3
#define TEMP_SDA 6
#define TEMP_SCL 7
#define ALS_SDA 12
#define ALS_SCL 13
#define IR_EMMIT 14
#define TEMP_ALRT 23
#define ALS_INT 22
#define IR_SNSR

#define ALS_I2C_ADDRESS 0x44 // 7-bit I2C address (0b1000100)
#define RESULT_REGISTER 0x00       // Result register address
#define CONFIG_REGISTER 0x01       // Configuration register address




int main() {
    stdio_init_all(); // Initialize standard I/O   
    ALS_init(); 
    sleep_ms(200); // Delay to ensure initialization is complete

    while (true) {
        uint16_t result = read_register(i2c0, RESULT_REGISTER); // Read the result register
        // The output is in a 16-bit format; extract lux value from it
        uint8_t exponent = (result >> 12) & 0x0F; // Exponent
        uint16_t mantissa = result & 0x0FFF; // Mantissa
        float lux = (0.01 * (1 << exponent) * mantissa); // Calculate lux

        printf("Ambient Light: %.2f lux\n", lux); // Print the light level
        sleep_ms(1000); // Delay before the next read
    }

    return 0;
}
