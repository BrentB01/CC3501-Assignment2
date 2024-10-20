#include <stdio.h>
#include "pico/stdlib.h"
#include <cstdarg>
#include <cstdio>
#include <cstdint>

#include "drivers/TMP75AIDR.cpp"
#include "drivers/TMP75AIDR.h"
#include "drivers/leds.h"
#include "drivers/logging/logging.h"
#include "drivers/OPTO3001.h"

#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "WS2812.pio.h"

//Temp Sensor Definitions 
#define TEMP_SDA 6
#define TEMP_SCL 7
#define TEMP_I2C_SLAVE 0x48 // TMP75 I2C address
#define RESULT_REGISTER 0x00 // Temperature register address

//Ambient Light Sensor
#define ALS_I2C_ADDRESS 0x44 // 7-bit I2C address (0b1000100)
#define RESULT_REGISTER 0x00       // Result register address
#define CONFIG_REGISTER 0x01       // Configuration register address

//LED Definitions 
#define LED_PIN 29
#define NUM_LEDS 1

//IR Sensor
#define IR_SENSOR 25



int main() {
    
    
    stdio_init_all(); // Initialize standard I/O for serial communication

    // Initialize I2C for the TMP75 sensor
    TEMP_init();      
    
    // Initialize ALS 
    ALS_init();


    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN,GPIO_OUT);
    
    
   
    

    // Main loop to continuously read and display the temperature
    while (true) {
        // Read the raw temperature value from the TMP75 sensor
        uint16_t raw_temp = read_temp_register();
        // Convert the raw temperature value to Celsius
        float temperature = convert_to_celsius(raw_temp);
        // Print the temperature to the terminal
        printf("Temperature: %.2f°C\n", temperature);
        // Wait for 1 second before reading again
        sleep_ms(100);
        float LUX = ALS_read();
        if (LUX<= 100)
        {
            gpio_put(LED_PIN,true);
        }
        else
        {
            gpio_put(LED_PIN,false);
        }
        
        
        
        
    }

    return 0;
}
