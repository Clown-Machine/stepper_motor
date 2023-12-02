#include <stdlib.h>

#include "msp.h"
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

#include "global.h"
#include "stepper.h"

#define IN1 GPIO_PORT_P6, GPIO_PIN0
#define IN2 GPIO_PORT_P3, GPIO_PIN2
#define IN3 GPIO_PORT_P3, GPIO_PIN3
#define IN4 GPIO_PORT_P4, GPIO_PIN1

#define BTN GPIO_PORT_P1, GPIO_PIN5

void configurePorts(){
    /* configure P2.0, P2.1 and P2.2 as GPIO */
    GPIO_setAsInputPin(BTN);
    GPIO_enableInterrupt(BTN);
    Interrupt_enableInterrupt(INT_PORT1);
}

/* Timer_A UpMode Configuration Parameter */
void configureTimerUpMode() {
    // Set the SMCLK to 3MHz (assumed frequency)
    //CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_3);

    /* Configuring Timer_A1 for Up Mode */
    Timer_A_configureUpMode(TIMER_A1_BASE, &timerConfig);

    /* Enabling interrupts and starting the timer */
    Interrupt_enableSleepOnIsrExit();
    Interrupt_enableInterrupt(INT_TA1_0);
    Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_UP_MODE);
    /* Enabling MASTER interrupts */
    Interrupt_enableMaster();
}

Stepper_t stepper;

void main(void) {
    WDT_A_holdTimer();

    stepper = Stepper_INIT(IN1, IN2, IN3, IN4);

    configurePorts();
    configureTimerUpMode();

    while(1) {
        __sleep();
    }
}

void TA1_0_IRQHandler(void) {
    Timer_A_clearCaptureCompareInterrupt(TIMER_A1_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);

    Stepper_MoveIfShuld(&stepper);
}

int speed = 0;
void PORT1_IRQHandler(void) {
    GPIO_clearInterruptFlag(BTN);

    speed = (speed+10)%200;
    Stepper_setSpeed_Percentage(&stepper, speed-100);
}
