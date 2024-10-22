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
#include "drivers/ir_driver.h"
#include "drivers/AS2.h"

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
#define LED_PIN 18
#define NUM_LEDS 1

// IR Sensor
#define IR_SENSOR 25

// Buttons
#define SW1 2



// Bluetooth 
#define BLT_RST 16 
#define BLT_CS 15 
#define BLT_INT 17 
#define BLT_TX 5
#define BLT_RX 4
#define UART_ID uart1             
#define BAUD_RATE 115200            
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY UART_PARITY_NONE

using namespace std;

bool IR_SENSOR_FLAG = false; 



int main() {

// Initilizations
    stdio_init_all(); // Initialize standard I/O for serial communication
    TEMP_init();      
    ALS_init();

    // Setup PWM for IR transmission
    setup_pwm(IR_PIN, CARRIER_FREQUENCY, DUTY_CYCLE_PERCENT);
    sleep_ms(1000);  // Wait before sending signal

    // Initilize LED
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN,GPIO_OUT);

    // Initialize button SW1
    gpio_init(SW1);
    gpio_set_dir(SW1, GPIO_IN);

    // Initialize Bluetooth UART
    bluetooth_init();

    // Send data over Bluetooth
    Instructions();

    IR_init();
  

    // Variables to track button and LED state
    int sw1_press_count = 0; // Tracks how many times SW1 has been pressed
    bool sw1_pressed = false; // Tracks if the SW1 button was pressed (for edge detection)
    
    // Timer variable for instructions
    absolute_time_t last_instruction_time = get_absolute_time();
    const int instruction_interval_ms = 15000; // 15 seconds

    // Main loop to continuously read and display the temperature
    while (true) {
        
        if (absolute_time_diff_us(last_instruction_time, get_absolute_time()) >= instruction_interval_ms*1000) {
            Instructions();
            last_instruction_time = get_absolute_time(); // Update the last instruction time
        }
        // Read the raw temperature value from the TMP75 sensor
        uint16_t raw_temp = read_temp_register();

        // Convert the raw temperature value to Celsius
        float temperature = convert_to_celsius(raw_temp);
        
        // Print the temperature to the terminal
        //printf("Temperature: %.2f°C\n", temperature);
        
        // Check the temperature conditions
        if (temperature > 29.0) {
            send_ir_signal(IR_PIN, ir_data_on, sizeof(ir_data_on) / sizeof(ir_data_on[0]));
        } else if (temperature < 27.0) {
            send_ir_signal(IR_PIN, ir_data_off, sizeof(ir_data_off) / sizeof(ir_data_off[0]));
        }

        // Check if SW1 is pressed (active low, so gpio_get(SW1) == 0 when pressed)
        if (gpio_get(SW1) == 0 && !sw1_pressed) {
            // Button press detected (rising edge)
            sw1_pressed = true;
            sw1_press_count = (sw1_press_count + 1) % 3; // Increment press count and wrap around every 4 presses

            // Handle behavior based on press count
            if (sw1_press_count == 1) {
                // First press: Manual mode, LED on
                gpio_put(LED_PIN, true);
                printf("Manual Mode Activated: LED turned ON.\n");

            } else if (sw1_press_count == 2) {
                // Second press: Manual mode, LED off
                gpio_put(LED_PIN, false);
                printf("Manual Mode Activated: LED turned OFF.\n");

            }  else if (sw1_press_count == 0) {
                // Fourth press: Revert to automatic lighting control mode
                printf("Automatic Mode Activated\n");
            }
        }

        // Reset button state when released
        if (gpio_get(SW1) != 0) {
            sw1_pressed = false;
        }

        // Automatic mode (sw1_press_count == 0) controls LED with ambient light sensor
        if (sw1_press_count == 0) {
            float LUX = ALS_read();
            if (LUX <= 100) {
                gpio_put(LED_PIN, true); // Turn LED on if ambient light is sufficient
                printf("Ambient Light: %.2f LUX, LED turned ON (Low Light).\n", LUX);
            } else {
                // Turn LED off
                gpio_put(LED_PIN, false); // Turn LED off if ambient light is sufficient
                printf("Ambient Light: %.2f LUX, LED turned OFF (Sufficient Light).\n", LUX);
            }
        }

        //sleep_ms(100); // Wait for 100ms before checking again
        // Check if data is available to read
        if (uart_is_readable(UART_ID)) {
            char ch = uart_getc(UART_ID); // Read a single character

           
            if (ch == 'A') //Temp read
            {
                uint16_t raw_temp = read_temp_register(); // Read the raw temperature value
                float temperature = convert_to_celsius(raw_temp); // Convert the raw value to Celsius
                char buffer[50];
                snprintf(buffer, sizeof(buffer), "TEMP: %.2fC\n", temperature);
                uart_puts(UART_ID, buffer);
               
            }
            // else if (ch=='B') //Aircon to auto
            // {
                
            // }
            // else if (ch == "C")//Aircon on
            // {

            // }
            // else if (ch == "D")//Aircon off
            // {
            //     /* code */
            // }
            else if (ch=='E')
            {
               float lux = ALS_read();
               char buffer[50];
               snprintf(buffer, sizeof(buffer), "LUX: %.2flx\n", lux);
               uart_puts(UART_ID, buffer);    
            }
            else if (ch == 'F')
            {
              sw1_press_count = 0; //Light auto mode 
            } else if (ch == 'G')
            {
                sw1_press_count = 1; //Light on 
                gpio_put(LED_PIN, true);
            }else if (ch == 'H')
            {
                sw1_press_count = 2; //Light off
                gpio_put(LED_PIN, false);  // Turn LED on
            }else if (ch == 'M'){
               char buffer[50];
               snprintf(buffer, sizeof(buffer), "MOTION SENSOR ACTIVATED\n");
               uart_puts(UART_ID, buffer); 
               IR_SENSOR_FLAG = true;
            }else if (ch == 'N')
            {
               char buffer[50];
               snprintf(buffer, sizeof(buffer), "MOTION SENSOR DEACTIVATION\n");
               uart_puts(UART_ID, buffer); 
               IR_SENSOR_FLAG = false;
            }else{
               char buffer[50];
               snprintf(buffer, sizeof(buffer), "INVALID CHARACTER\n");
               uart_puts(UART_ID, buffer); 
            }
        }
        if (IR_SENSOR_FLAG == true)
        {
            bool ir_check = gpio_get(IR_SENSOR);
            if (ir_check == 1)
            {
                gpio_put(LED_PIN, 1);
               char buffer[50];
               snprintf(buffer, sizeof(buffer), "MOTION DETECTED\n");
               uart_puts(UART_ID, buffer); 

            }
            
        }
        
    }

    return 0;
}