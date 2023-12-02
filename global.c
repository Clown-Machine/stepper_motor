#include "global.h"

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

void GPIO_setOutputOnPin(uint_fast8_t selectedPort, uint_fast16_t selectedPins, PinValue_t value) {
    switch(value) {
        case HIGH:
            GPIO_setOutputHighOnPin(selectedPort, selectedPins);
            break;
        case LOW:
            GPIO_setOutputLowOnPin(selectedPort, selectedPins);
            break;
        case TOGGLE:
            GPIO_toggleOutputOnPin(selectedPort, selectedPins);
            break;
        default:
            break;
    }
}
