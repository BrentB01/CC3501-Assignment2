#include "ir_driver.h"
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/timer.h"
#include <hardware/clocks.h>

#define HIGH 1
#define LOW 0

/*
// Raw IR data for turning the device ON

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

// Raw IR data for turning the device OFF

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
*/

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