// #include <stdio.h>
// #include "pico/stdlib.h"
// #include "hardware/gpio.h"
// #include "hardware/pio.h"
// #include "hardware/i2c.h"

// #include "WS2812.pio.h"
// #include "drivers/logging/logging.h"
// #include "drivers/leds.h"
// #include "drivers/accelerometer.h"

// #include <cstdarg>
// #include <cstdio>
// #include <cstdint>

// #define LED_PIN 18 // Note: LED will only work off battery power
// #define SW1 2
// #define SW2 3
// #define TEMP_SDA 6
// #define TEMP_SCL 7
// #define ALS_SDA 12
// #define ALS_SCL 13
// #define IR_EMMIT 14
// #define TEMP_ALRT 23
// #define ALS_INT 22
// #define IR_SNSR

// // AMBIENT LIGHT SENSOR DEFINITIONS
// // NOTE: 16-bit addresses
// #define ALS_INSTANCE i2c0 // Address for ALS
// #define ALS_I2C_ADDRESS 0b1000100 // 7 bit ALS slave address. add 1 for read and 0 for write
// #define ALS_RESULT 0x00 // Register to read from
// #define ALS_CONFIG 0b1100111000000011


// uint8_t length = 2; // Adjust length to 2 for 16-bit read
// uint16_t data[2];

// void ALS_init() {
//     i2c_init(ALS_INSTANCE, 100 * 1000); // Initialize at 400 kHz (max MHz of ALS)
//     gpio_set_function(ALS_SDA, GPIO_FUNC_I2C); // Sets serial data line
//     gpio_set_function(ALS_SCL, GPIO_FUNC_I2C); // Sets serial clock line
// }

// bool ALS_write_register(uint8_t reg, uint16_t data)
// {
//     uint8_t buf[2];
//     buf[0] = reg;
//     buf[1] = data;

//     int bytes_written = i2c_write_blocking(ALS_INSTANCE, ALS_I2C_ADDRESS, buf, 2, false);
//     if (bytes_written != 2)
//     {
//         log(LogLevel::ERROR, "Failed to write to accelerometer register.");
//         printf("fail");
//         return false;
//     }
//     return true;
// }

// // bool ALS_read(uint8_t reg, uint8_t length) {
// //     // Tell the device which register we want to read
// //     if (1 != i2c_write_blocking(ALS_INSTANCE, ALS_I2C_ADDRESS, &reg, 1, true)) {
// //         log(LogLevel::ERROR, "ALS::read_registers: Failed to select register address.");
// //         return false;
// //     }

// //     // Now read the data
// //     uint16_t bytes_read = i2c_read_blocking(ALS_INSTANCE, ALS_I2C_ADDRESS, data, length, false);
// //     if (bytes_read != length) {
// //         log(LogLevel::ERROR, "ALS::read_registers: Failed to read data.");
// //         return false;
// //     }

// //     return true;
// // }

// // void ALS_print(){
// //    // Do something with the read data (e.g., print it)
// //     for (int i = 0; i < length; i++)
// //     {
// //         printf("Data byte %d: 0x%02x\n", i, data[i]);
// //     }
// //     }

// int main() {
//     stdio_init_all();  // Initialize the standard I/O
//     ALS_init();
//     // ALS_print();

//     return 0;
// }

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define ALS_I2C_ADDRESS 0x44 // 7-bit I2C address (0b1000100)
#define RESULT_REGISTER 0x00       // Result register address
#define CONFIG_REGISTER 0x01       // Configuration register address

// Function to write to a register
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

// Function to initialize the OPT3001
void init_opt3001(i2c_inst_t* i2c) {
    // Configure the sensor for continuous conversion with 800 ms conversion time and auto-range mode
    write_register(i2c, CONFIG_REGISTER, 0b1100110000000000); // Continuous conversion mode, auto-range
}

int main() {
    stdio_init_all(); // Initialize standard I/O

    // Initialize I2C (I2C0 in this case)
    i2c_init(i2c0, 100000); // 100 kHz
    gpio_set_function(12, GPIO_FUNC_I2C); // Set GPIO 12 as SDA
    gpio_set_function(13, GPIO_FUNC_I2C); // Set GPIO 13 as SCL
    gpio_pull_up(12); // Enable pull-up for SDA
    gpio_pull_up(13); // Enable pull-up for SCL

    init_opt3001(i2c0); // Initialize the sensor

    sleep_ms(1000); // Delay to ensure initialization is complete

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
