#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define TEMP_SDA 6
#define TEMP_SCL 7
#define TEMP_I2C_SLAVE 0x48 // TMP75 I2C address
#define RESULT_REGISTER 0x00 // Temperature register address

// Function to initialize the I2C interface for temperature sensor
void TEMP_init() {
    // Initialize I2C at 100kHz
    i2c_init(i2c1, 100000);
    // Set up I2C pins for TEMP_SDA and TEMP_SCL
    gpio_set_function(TEMP_SDA, GPIO_FUNC_I2C);
    gpio_set_function(TEMP_SCL, GPIO_FUNC_I2C);
    // Internal pull-ups are already enabled by default for I2C
}

// Function to read temperature register from TMP75 sensor
uint16_t read_temp_register() {
    uint8_t reg = RESULT_REGISTER;
    uint8_t data[2];

    // Write the register address to the TMP75
    int result = i2c_write_blocking(i2c1, TEMP_I2C_SLAVE, &reg, 1, true);
    if (result == PICO_ERROR_GENERIC) {
        printf("I2C Write Error during read!\n");
        return 0;
    }

    // Read 2 bytes of data from the temperature register
    result = i2c_read_blocking(i2c1, TEMP_I2C_SLAVE, data, 2, false);
    if (result == PICO_ERROR_GENERIC) {
        printf("I2C Read Error!\n");
        return 0;
    }

    // Combine the two bytes to form the raw temperature value
    return (data[0] << 8) | data[1];
}

// Function to convert the raw temperature value to Celsius
float convert_to_celsius(uint16_t raw_value) {
    // TMP75 outputs a 12-bit value; shift to right-align the 12-bit temperature
    int16_t temp = (int16_t)(raw_value >> 4);
    // Convert to Celsius (0.0625°C per bit)
    return temp * 0.0625f;
}

int main() {
    stdio_init_all(); // Initialize standard I/O for serial communication
    TEMP_init();      // Initialize I2C for the TMP75 sensor

    // Main loop to continuously read and display the temperature
    while (true) {
        // Read the raw temperature value from the TMP75 sensor
        uint16_t raw_temp = read_temp_register();
        // Convert the raw temperature value to Celsius
        float temperature = convert_to_celsius(raw_temp);
        // Print the temperature to the terminal
        printf("Temperature: %.2f°C\n", temperature);
        // Wait for 1 second before reading again
        sleep_ms(1000);
    }

    return 0;
}
