
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

#define TEMP_SDA 6
#define TEMP_SCL 7
#define TEMP_ALERT 23
#define TEMP_I2C_SLAVE 0x48

extern uint8_t reg;
extern uint16_t value;
extern i2c_inst_t* i2c;
uint8_t data[2];

void write_temp_register(i2c_inst_t* i2c, uint8_t reg, uint16_t value) {
    uint8_t data[3];
    data[0] = reg; // Register address
    data[1] = (value >> 8) & 0xFF; // MSB
    data[2] = value & 0xFF; // LSB
    int result = i2c_write_blocking(i2c, TEMP_I2C_SLAVE, data, 3, false);
    if (result == PICO_ERROR_GENERIC) {
        printf("I2C Write Error!\n");
    }
}

// Function to read from a register
uint16_t read_temp_register(i2c_inst_t* i2c, uint8_t reg) {
    uint8_t data[2] = {0};
    int result = i2c_write_blocking(i2c, TEMP_I2C_SLAVE, &reg, 1, true); // Send register address
    if (result == PICO_ERROR_GENERIC) {
        printf("I2C Write Error during read!\n");
    }
    result = i2c_read_blocking(i2c, TEMP_I2C_SLAVE, data, 2, false); // Read data
    if (result == PICO_ERROR_GENERIC) {
        printf("I2C Read Error!\n");
    }
    return (data[0] << 8) | data[1]; // Combine MSB and LSB
}

float convert_to_celsius(uint16_t raw_value) {
    // TMP75 outputs a 12-bit value; shift to right-align the 12-bit temperature
    int16_t temp = (int16_t)(raw_value >> 4);
    // Convert to Celsius (0.0625°C per bit)
    return temp * 0.0625f;
}
 
 void TEMP_init(){
 // Initialize I2C (I2C0 in this case)
    i2c_init(i2c0, 100000); // 100 kHz
    gpio_set_function(TEMP_SDA, GPIO_FUNC_I2C); // Set GPIO 12 as SDA
    gpio_set_function(TEMP_SCL, GPIO_FUNC_I2C); // Set GPIO 13 as SCL
 }

int main() {
    stdio_init_all(); // Initialize standard I/O   
    TEMP_init();
    data[0] = CONFIG_REGISTER; // Register address
    data[1] = 0x00;
    i2c_write_blocking(i2c0, TEMP_I2C_SLAVE, data, 2, false);
    read_temp_register(i2c0,RESULT_REGISTER);

    while (true) {
        uint16_t raw_temp = read_temp_register(i2c0, RESULT_REGISTER); // Read raw temperature value
        float temperature = convert_to_celsius(raw_temp); // Convert raw value to Celsius
        printf("Temperature: %.2f°C\n", temperature); // Print temperature to terminal
        sleep_ms(1000); // Wait for 1 second before reading again
    }

    return 0;
}
