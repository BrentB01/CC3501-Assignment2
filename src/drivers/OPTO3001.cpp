#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <cstdint>        // Ensure this is included for uint32_t
#include "hardware/pio.h" // Include for PIO
#include <cstdarg>        // For variadic functions

#define ALS_I2C_ADDRESS 0x44 // 7-bit I2C address (0b1000100)
#define RESULT_REGISTER 0x00       // Result register address
#define CONFIG_REGISTER 0x01       // Configuration register address

extern uint8_t reg;
extern uint16_t value;
extern i2c_inst_t* i2c;

void write_register(i2c_inst_t* i2c, uint8_t reg, uint16_t value) {
    uint8_t data[3];
    data[0] = reg; // Register address
    data[1] = (value >> 8) & 0xFF; // MSB
    data[2] = value & 0xFF; // LSB
    int result = i2c_write_blocking(i2c, ALS_I2C_ADDRESS, data, 3, false);
    if (result == PICO_ERROR_GENERIC) {
        printf("I2C Write Error!\n");
    }
}

// Function to read from a register
uint16_t read_register(i2c_inst_t* i2c, uint8_t reg) {
    uint8_t data[2] = {0};
    int result = i2c_write_blocking(i2c, ALS_I2C_ADDRESS, &reg, 1, true); // Send register address
    if (result == PICO_ERROR_GENERIC) {
        printf("I2C Write Error during read!\n");
    }
    result = i2c_read_blocking(i2c, ALS_I2C_ADDRESS, data, 2, false); // Read data
    if (result == PICO_ERROR_GENERIC) {
        printf("I2C Read Error!\n");
    }
    return (data[0] << 8) | data[1]; // Combine MSB and LSB
}
 
 void ALS_init(){
 // Initialize I2C (I2C0 in this case)
    i2c_init(i2c0, 100000); // 100 kHz
    gpio_set_function(12, GPIO_FUNC_I2C); // Set GPIO 12 as SDA
    gpio_set_function(13, GPIO_FUNC_I2C); // Set GPIO 13 as SCL
    write_register(i2c0, CONFIG_REGISTER, 0b1100110000000000); //Configures the register 
}

float ALS_read(){
     uint16_t result = read_register(i2c0, RESULT_REGISTER); // Read the result register
        // The output is in a 16-bit format; extract lux value from it
        uint8_t exponent = (result >> 12) & 0x0F; // Exponent
        uint16_t mantissa = result & 0x0FFF; // Mantissa
        float lux = (0.01 * (1 << exponent) * mantissa); // Calculate lux

        //printf("Ambient Light: %.2f lux\n", lux); // Print the light level
        return lux; 
}

// This code will continually monitor the ALS
/* Consider chnaging in future 

 while (true) {
        uint16_t result = read_register(i2c0, RESULT_REGISTER); // Read the result register
        // The output is in a 16-bit format; extract lux value from it
        uint8_t exponent = (result >> 12) & 0x0F; // Exponent
        uint16_t mantissa = result & 0x0FFF; // Mantissa
        float lux = (0.01 * (1 << exponent) * mantissa); // Calculate lux

        printf("Ambient Light: %.2f lux\n", lux); // Print the light level
        sleep_ms(1000); // Delay before the next read
    }

*/