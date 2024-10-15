#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "drivers/leds.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"

#include "WS2812.pio.h"
#include "drivers/logging/logging.h"

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


extern uint8_t length; // Declare it as extern

// Create a buffer to store the read data globally
extern uint8_t data[6];  // Array to store accelerometer X, Y, Z data (assuming 6 bytes)



// Initialise the i2c bus at 400kHz
bool accel_write_register(uint8_t reg, uint8_t data)
{
    uint8_t buf[2];
    buf[0] = reg;
    buf[1] = data;
    int bytes_written = i2c_write_blocking(ACCEL_INSTANCE, I2C_ADDRESS, buf, 2, false);
    if (bytes_written != 2)
    {
        log(LogLevel::ERROR, "Failed to write to accelerometer register.");
        printf("fail");
        return false;
    }
    return true;
}

void accel_init(){
    i2c_init(ACCEL_INSTANCE, 400 * 1000);      // Initialize the thing at 400 kHz
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C); // Sets serial data line
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C); // Sets serial clock line
}

bool accel_read(uint8_t reg, uint8_t length){


   // Tell the device which register we want to read
    if (1 != i2c_write_blocking(ACCEL_INSTANCE, I2C_ADDRESS, &reg, 1, true))
    {
        log(LogLevel::ERROR, "lis3dh::read_registers: Failed to select register address.");
        return false;
    }

    // Now read the data
    int bytes_read = i2c_read_blocking(ACCEL_INSTANCE, I2C_ADDRESS, data, length, false);
    if (bytes_read != length)
    {
        log(LogLevel::ERROR, "lis3dh::read_registers: Failed to read data.");
        return false;
    }
 return 1;
}