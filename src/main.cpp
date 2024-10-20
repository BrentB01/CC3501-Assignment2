#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "drivers/TMP75AIDR.cpp"
#include "drivers/TMP75AIDR.h"

#define TEMP_SDA 6
#define TEMP_SCL 7
#define TEMP_I2C_SLAVE 0x48 // TMP75 I2C address
#define RESULT_REGISTER 0x00 // Temperature register address

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
