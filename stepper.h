#ifndef STEPPER_H
#define STEPPER_H

#include "global.h"

#include "msp.h"
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

typedef int8_t speed_t;

typedef enum {
    STATIONARY,
    CLOCKWISE,
    COUNTERCLOCKWISE
} direction_t;

typedef int8_t StepNumber_t;


typedef struct {
    uint_fast8_t COIL1_Port;
    uint_fast8_t COIL1_Pin;
    uint_fast8_t COIL2_Port;
    uint_fast8_t COIL2_Pin;
    uint_fast8_t COIL3_Port;
    uint_fast8_t COIL3_Pin;
    uint_fast8_t COIL4_Port;
    uint_fast8_t COIL4_Pin;

    time_t step_delay;
    time_t current_time;
    time_t time_increment;

    direction_t direction;

    uint_fast32_t number_of_steps;

    StepNumber_t current_step;
} Stepper_t;

Stepper_t Stepper_INIT(uint_fast8_t coil1_port, uint_fast8_t coil1_pin,
                         uint_fast8_t coil2_port, uint_fast8_t coil2_pin,
                         uint_fast8_t coil3_port, uint_fast8_t coil3_pin,
                         uint_fast8_t coil4_port, uint_fast8_t coil4_pin);

extern const Timer_A_UpModeConfig timerConfig;

void Stepper_setSpeed_RPM(Stepper_t * stepper, speed_t speed_rpm);
void Stepper_setSpeed_Percentage(Stepper_t * stepper, speed_t speed_percentage);
void Stepper_MoveIfShuld(Stepper_t * stepper);

#endif
