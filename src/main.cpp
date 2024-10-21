#include <stdint.h>  // For uint16_t
#include <stddef.h>  // For size_t
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/timer.h"
#include <hardware/clocks.h>

#define HIGH 1
#define LOW 0

#define SW2 3



// IR data arrays
const uint16_t ir_data_on[] = {
    3255, 1654, 448, 380, 447, 379, 447, 1196, 448, 379, 447, 1198, 445, 378, 448,
    379, 447, 379, 447, 1197, 446, 1196, 448, 379, 447, 379, 447, 378, 448, 1197,
    447, 1198, 445, 379, 447, 379, 447, 379, 447, 379, 448, 379, 447, 377, 449,
    379, 447, 379, 447, 378, 448, 379, 447, 379, 447, 378, 448, 378, 448, 1197,
    447, 379, 448, 378, 448, 380, 446, 379, 447, 380, 446, 379, 447, 379, 447,
    1196, 447, 379, 447, 379, 447, 379, 447, 379, 448, 1197, 447, 1197, 446, 1196,
    447, 1196, 447, 1196, 447, 1197, 446, 1197, 447, 1196, 448, 378, 448, 377,
    449, 1196, 448, 379, 447, 378, 448, 379, 447, 378, 448, 378, 449, 379, 447,
    379, 447, 379, 447, 1196, 447, 1197, 446, 379, 447, 379, 447, 1196, 448, 380,
    447, 378, 448, 378, 448, 379, 447, 378, 448, 1198, 445, 378, 448, 379, 447,
    379, 447, 378, 448, 379, 447, 380, 447, 379, 447, 378, 448, 379, 447, 378,
    448, 379, 447, 379, 447, 379, 447, 379, 447, 379, 447, 378, 448, 380, 447,
    1196, 447, 379, 447, 379, 447, 378, 448, 379, 447, 379, 447, 379, 447, 1196,
    447, 1196, 447, 379, 448, 379, 447, 379, 447, 378, 448, 379, 447, 379, 447,
    379, 447, 378, 448, 379, 447, 378, 448, 379, 447, 1198, 446, 378, 448, 379,
    447, 1197, 447, 378, 448, 379, 447, 378, 448, 379, 447, 1196, 447, 380, 447,
    380, 446, 1196, 447, 379, 447, 378, 448, 1196, 447, 379, 447, 380, 446, 1196,
    447, 379, 448
};
const uint16_t ir_data_off[] = {
    3255, 1654, 448, 380, 446, 380, 447, 1196, 448, 379, 447, 1195, 448, 380, 446,
    379, 447, 378, 448, 1196, 448, 1196, 447, 380, 447, 379, 447, 379, 447, 1197,
    447, 1197, 446, 379, 447, 379, 447, 380, 446, 380, 446, 379, 447, 381, 446,
    379, 447, 379, 447, 380, 446, 379, 447, 379, 447, 379, 447, 379, 447, 1198,
    446, 379, 447, 379, 447, 380, 447, 380, 446, 379, 447, 379, 447, 379, 447,
    1196, 447, 379, 447, 380, 446, 379, 447, 379, 447, 1197, 447, 379, 448, 379,
    447, 379, 447, 380, 446, 379, 447, 379, 447, 1196, 447, 379, 447, 1197, 446,
    1197, 447, 1196, 447, 1197, 446, 1196, 447, 1196, 447
};

// Set PWM frequency and duty cycle
void setup_pwm(unsigned int gpio, unsigned int freq, float duty_cycle_percent) {
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(gpio);
    uint32_t clock = clock_get_hz(clk_sys);
    uint32_t divider16 = clock / freq / 4096;
    pwm_set_clkdiv(slice_num, divider16);

    uint16_t top = clock / (divider16 * freq) - 1;
    pwm_set_wrap(slice_num, top);

    uint16_t level = (uint16_t)(duty_cycle_percent * top / 100.0);
    pwm_set_chan_level(slice_num, pwm_gpio_to_channel(gpio), level);
    pwm_set_enabled(slice_num, false);  // Start with PWM disabled
}

// Enable or disable the PWM output
void set_pwm_output(uint gpio, bool enable) {
    uint slice_num = pwm_gpio_to_slice_num(gpio);
    pwm_set_enabled(slice_num, enable);
}

// Send IR signal (generic function for both ON and OFF)
void send_ir_signal(unsigned int gpio, const uint16_t* ir_data, size_t data_length) {
    for (int i = 0; i < data_length; i++) {
        if (i % 2 == 0) {
            set_pwm_output(gpio, HIGH);  // Use the passed gpio
        } else {
            set_pwm_output(gpio, LOW);
        }
        sleep_us(ir_data[i]);
    }
    set_pwm_output(gpio, LOW);  // Ensure PWM is disabled after signal
}

int main() {
    const int ir_gpio = 15;  // Replace with your GPIO pin for IR emitter
    setup_pwm(ir_gpio, 38000, 33.0);  // Set PWM for 38kHz with 33% duty cycle

    // Initialize other necessary components, e.g., temperature sensor, buttons, etc.
    gpio_init(SW2);
    gpio_set_dir(SW2, GPIO_IN);
    

    while (1) {
        if (gpio_get(SW2) == 0) {
            send_ir_signal(ir_gpio, ir_data_on, sizeof(ir_data_on) / sizeof(ir_data_on[0]));
        } else if (gpio_get(SW2) == 1) {
            send_ir_signal(ir_gpio, ir_data_off, sizeof(ir_data_off) / sizeof(ir_data_off[0]));
        }

        sleep_ms(1000);  // Adjust the delay as needed
    }
    return 0;
}