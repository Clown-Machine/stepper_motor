#include "stepper.h"
#include "stdlib.h"

const int MIN_SPEED_DELAY = 4000; // ~4.8 RPM
const int MAX_SPEED_DELAY = 1500; // ~14.6 RPM

typedef struct Step_t {
    StepNumber_t stepNumber;

    PinValue_t COIL1_Value;
    PinValue_t COIL2_Value;
    PinValue_t COIL3_Value;
    PinValue_t COIL4_Value;
} Step_t;


Step_t _step(StepNumber_t stepNumber) {
    switch(stepNumber) {                                                // Step COIL_1  COIL_2  COIL_3  COIL_4
        case 0:
            return (Step_t) {0, HIGH, LOW, HIGH, LOW};;    // 0    HIGH    LOW     HIGH    LOW
        case 1:
            return (Step_t) {1, LOW, HIGH, HIGH, LOW};     // 1    LOW     HIGH    HIGH    LOW
        case 2:
            return (Step_t) {2, LOW, HIGH, LOW, HIGH};     // 2    LOW     HIGH    LOW     HIGH
        case 3:
            return (Step_t) {3, HIGH, LOW, LOW, HIGH};     // 3    HIGH    LOW     LOW     HIGH
        default:
            return (Step_t) {};
    }
}

Step_t step(StepNumber_t stepNumber) {
    switch(stepNumber) {                                                // Step COIL_1  COIL_2  COIL_3  COIL_4
        case 0:
            return (Step_t) {0, HIGH, HIGH, LOW, LOW};;    // 0    HIGH    LOW     HIGH    LOW
        case 1:
            return (Step_t) {1, LOW, HIGH, HIGH, LOW};     // 1    LOW     HIGH    HIGH    LOW
        case 2:
            return (Step_t) {2, LOW, LOW, HIGH, HIGH};     // 2    LOW     HIGH    LOW     HIGH
        case 3:
            return (Step_t) {3, HIGH, LOW, LOW, HIGH};     // 3    HIGH    LOW     LOW     HIGH
        default:
            return (Step_t) {};
    }
}

Stepper_t Stepper_INIT(uint_fast8_t coil1_port, uint_fast8_t coil1_pin,
                         uint_fast8_t coil2_port, uint_fast8_t coil2_pin,
                         uint_fast8_t coil3_port, uint_fast8_t coil3_pin,
                         uint_fast8_t coil4_port, uint_fast8_t coil4_pin) {

    GPIO_setAsOutputPin(coil1_port, coil1_pin);
    GPIO_setAsOutputPin(coil2_port, coil2_pin);
    GPIO_setAsOutputPin(coil3_port, coil3_pin);
    GPIO_setAsOutputPin(coil4_port, coil4_pin);

    return (Stepper_t) {
        .COIL1_Port = coil1_port,
        .COIL1_Pin = coil1_pin,
        .COIL2_Port = coil2_port,
        .COIL2_Pin = coil2_pin,
        .COIL3_Port = coil3_port,
        .COIL3_Pin = coil3_pin,
        .COIL4_Port = coil4_port,
        .COIL4_Pin = coil4_pin,
        .step_delay = MIN_SPEED_DELAY,
        .current_time = 0,
        .time_increment = 500, // default should be 500
        .number_of_steps = 2048,
        .current_step = 0,
        .direction = STATIONARY
    };
}

void Stepper_setSpeed_RPM(Stepper_t * stepper, speed_t speed_rpm) {
    stepper->step_delay = 60L * 1000L * 1000L / stepper->number_of_steps / abs(speed_rpm);
    if(speed_rpm > 0)
        stepper->direction = CLOCKWISE;
    else if(speed_rpm < 0)
        stepper->direction = COUNTERCLOCKWISE;
    else
        stepper->direction = STATIONARY;
}

void Stepper_setSpeed_Percentage(Stepper_t * stepper, speed_t speed_percentage) {
    // speed:100 = x:(max_speed-min_speed)
    stepper->step_delay = MIN_SPEED_DELAY - ((float) abs(speed_percentage) * (float) (MIN_SPEED_DELAY-MAX_SPEED_DELAY) / (float) 100);
    if(speed_percentage > 0)
        stepper->direction = CLOCKWISE;
    else if(speed_percentage < 0)
        stepper->direction = COUNTERCLOCKWISE;
    else
        stepper->direction = STATIONARY;
}

uint_fast8_t Stepper_shouldMoveAfterIncrement(Stepper_t * stepper) {
    stepper->current_time+=stepper->time_increment;
    if (stepper->current_time >= stepper->step_delay) { // check if enough time has passed since the last step
        stepper->current_time -= stepper->step_delay; // resets the current time subtracting the step time
        return 1;
    }
    return 0;
}

void Stepper_MoveIfShuld(Stepper_t * stepper) {
    if(Stepper_shouldMoveAfterIncrement(stepper) && stepper->direction != STATIONARY) {
        int8_t increment = stepper->direction == CLOCKWISE ? 1 : -1;
        stepper->current_step = (stepper->current_step+increment)%4;
        if(stepper->current_step < 0)
            stepper->current_step = 3;

        Step_t currentStep = step(stepper->current_step);
        GPIO_setOutputOnPin(stepper->COIL1_Port, stepper->COIL1_Pin, currentStep.COIL1_Value);
        GPIO_setOutputOnPin(stepper->COIL2_Port, stepper->COIL2_Pin, currentStep.COIL2_Value);
        GPIO_setOutputOnPin(stepper->COIL3_Port, stepper->COIL3_Pin, currentStep.COIL3_Value);
        GPIO_setOutputOnPin(stepper->COIL4_Port, stepper->COIL4_Pin, currentStep.COIL4_Value);


    }
}

// x/30000000=0.000001 -> x = 3

// delay at 15 RPM: 60*1000*1000/2048/15 = ~1950µs => 2000µs = ~14.6 RPM
// delay at 5 RPM: 60*1000*1000/2048/15 = ~5.853µs => 6000µs = ~4.8 RPM
//            timer default delay = 1 ms =  1000µs
// timer frequency = 3MHz
// timer period = 3000


const Timer_A_UpModeConfig timerConfig = {
        .clockSource = TIMER_A_CLOCKSOURCE_SMCLK,                                       // SMCLK Clock Source
        .clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1,                            // SMCLK/1 = 3MHz
        .timerPeriod = 1500,                                                            // every millisecond second
        .timerInterruptEnable_TAIE=TIMER_A_TAIE_INTERRUPT_DISABLE,                      // Disable Timer interrupt
        .captureCompareInterruptEnable_CCR0_CCIE= TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE,   // Enable CCR0 interrupt
        .timerClear=TIMER_A_DO_CLEAR                                                    // Clear value
};
