#ifndef IR_DRIVER_H
#define IR_DRIVER_H

#include <stdint.h>  // For uint16_t
#include <stddef.h>  // For size_t

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

// Function declarations
void setup_pwm(unsigned int gpio, unsigned int freq, float duty_cycle_percent);
void send_ir_signal(unsigned int gpio, const uint16_t* ir_data, size_t data_length);

#endif // IR_DRIVER_H
