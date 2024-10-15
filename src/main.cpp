// ALS Sensor

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

#define LED_PIN 18 //Note LED Will only work off battery power 
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


//AMBIENT LIGHT SENSOR DEFININITIONS 
//NOTE: 16 bit addresses 
#define ALS_INSTANCE i2c0 //Address for ALS
#define ALS_I2C_ADDRESS 0b1000100 // ALS_Adress for ALS (4 options)
#define ALS_RESULT 0x00 // This is the register to read from 
#define ALS_CONFIG 0x01 //Default config

uint8_t length =1;



void ALS_init(){
    i2c_init(ALS_INSTANCE, 400 * 1000);      // Initialize the thing at 400 kHz, (NOTE: THIS IS THE MAX MHZ of ALS)
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


int main()
{
   stdio_init_all();  // Initialize the standard I/  
   ALS_init();
   ALS_write_register(ALS_RESULT, ALS_CONFIG);

    return 0;
}

