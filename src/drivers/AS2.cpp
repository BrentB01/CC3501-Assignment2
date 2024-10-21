#include <stdio.h>
#include "pico/stdlib.h"
#include <cstdarg>
#include <cstdio>
#include <cstdint>
#include "pico/stdlib.h"

#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "WS2812.pio.h"
#include "hardware/uart.h"


// IR Driver Definitions
#define IR_PIN 14  // GPIO for IR transmission
#define CARRIER_FREQUENCY 38000
#define DUTY_CYCLE_PERCENT 33

// Temp Sensor Definitions 
#define TEMP_SDA 6
#define TEMP_SCL 7
#define TEMP_I2C_SLAVE 0x48 // TMP75 I2C address
#define RESULT_REGISTER 0x00 // Temperature register address

// Ambient Light Sensor
#define ALS_I2C_ADDRESS 0x44 // 7-bit I2C address (0b1000100)
#define RESULT_REGISTER 0x00       // Result register address
#define CONFIG_REGISTER 0x01       // Configuration register address

// LED Definitions 
#define LED_PIN 29
#define NUM_LEDS 1

// IR Sensor
#define IR_SENSOR 25

// Buttons
#define SW1 2

// IR Emmitter 
#define IR_Emmit 14

// Bluetooth 
#define BLT_RST 16 
#define BLT_CS 15 
#define BLT_INT 17 
#define BLT_TX 5
#define BLT_RX 4
#define UART_ID uart1               // Use UART1
#define BAUD_RATE 115200            // Ensure this matches the Raspberry Pi
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY UART_PARITY_NONE


void bluetooth_init() {
    uart_init(UART_ID, BAUD_RATE); // Initialize UART
    gpio_set_function(BLT_TX, GPIO_FUNC_UART); // Set GPIO for TX
    gpio_set_function(BLT_RX, GPIO_FUNC_UART); // Set GPIO for RX
    uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY); // Set UART format
}

void Instructions() {
    char buffer1[50];
    snprintf(buffer1, sizeof(buffer1), "IOT INSTRUCTIONS: \n");
    uart_puts(UART_ID, buffer1); // Send data over Bluetooth

    char buffer2[150];
    sniprintf(buffer2,sizeof(buffer2), "Temperature Instructions: \n Type 'A' for temp\n Type 'B' for aircon automode\n Type 'C' for aircon on\n Type 'D' for aircon off\n");
    uart_puts(UART_ID,buffer2);
    //Press 'L' for light level \n 

    char buffer3[150];
    sniprintf(buffer3,sizeof(buffer3), "Light Instructions: \n Type 'E' for Lux\n Type 'F' for AutoIllumination \n Type 'G' for light on \n Type 'H' for light off \n");
    uart_puts(UART_ID,buffer3);

    char buffer4[150];
    sniprintf(buffer4,sizeof(buffer4), "Motion Detections Instructions: \n Type 'M' to enable motion detection\n Type 'N' to diable motion detection\n");
    uart_puts(UART_ID,buffer4);
}

void IR_init(){
    gpio_init(IR_SENSOR);
    gpio_set_dir(IR_SENSOR,false);
}