#ifndef GLOBAL_H
#define GLOBAL_H

#include "msp.h"

typedef enum {
    HIGH, LOW, TOGGLE
} PinValue_t;

typedef uint_fast64_t time_t;

void GPIO_setOutputOnPin(uint_fast8_t selectedPort, uint_fast16_t selectedPins, PinValue_t value);

#endif
