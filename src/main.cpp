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

// AMBIENT LIGHT SENSOR DEFINITIONS
// NOTE: 16-bit addresses
#define ALS_INSTANCE i2c0 // Address for ALS
#define ALS_I2C_ADDRESS 0b1000100 // 7 bit ALS slave address. add 1 for read and 0 for write
#define ALS_RESULT_R 0b1000100 // Register to read from
#define ALS_CONFIG 0b11000010000000

uint8_t length = 2; // Adjust length to 2 for 16-bit read
uint16_t data[2];

void ALS_init() {
    i2c_init(ALS_INSTANCE, 100 * 1000); // Initialize at 400 kHz (max MHz of ALS)
    gpio_set_function(ALS_SDA, GPIO_FUNC_I2C); // Sets serial data line
    gpio_set_function(ALS_SCL, GPIO_FUNC_I2C); // Sets serial clock line
}

bool ALS_write_register(uint8_t reg, uint8_t data)
{
    uint8_t buf[2];
    buf[0] = reg;
    buf[1] = data;

    int bytes_written = i2c_write_blocking(ALS_INSTANCE, ALS_I2C_ADDRESS, buf, 2, false);
    if (bytes_written != 2)
    {
        log(LogLevel::ERROR, "Failed to write to accelerometer register.");
        printf("fail");
        return false;
    }
    return true;
}

bool ALS_read(uint8_t reg, uint8_t length) {
    // Tell the device which register we want to read
    if (1 != i2c_write_blocking(ALS_INSTANCE, ALS_I2C_ADDRESS, &reg, 1, true)) {
        log(LogLevel::ERROR, "ALS::read_registers: Failed to select register address.");
        return false;
    }

    // Now read the data
    uint16_t bytes_read = i2c_read_blocking(ALS_INSTANCE, ALS_I2C_ADDRESS, data, length, false);
    if (bytes_read != length) {
        log(LogLevel::ERROR, "ALS::read_registers: Failed to read data.");
        return false;
    }

    return true;
}

void ALS_print(){
   // Do something with the read data (e.g., print it)
    for (int i = 0; i < length; i++)
    {
        printf("Data byte %d: 0x%02x\n", i, data[i]);
    }
    }

int main() {
    stdio_init_all();  // Initialize the standard I/O
    ALS_init();
    ALS_read(ALS_RESULT, length);
    ALS_print();

    return 0;
}
