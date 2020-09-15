#include "pwm.h"
#include <xc.h>
#include <pic16f18854.h>
#include <stdint.h>
#include <stdio.h>

void init_pwm(void) {
    CCP5CONbits.EN = 1;
    CCP5CONbits.MODE = 0b1111;
    CCP5CONbits.FMT = 1;
    CCPR5H = 0b00000011;
    CCPR5L = 0b01000000;
    CCP4CONbits.EN = 1;
    CCP4CONbits.MODE = 0b1111;
    CCP4CONbits.FMT = 1;
    CCPR4H = 0b00000011;
    CCPR4L = 0b01000000;

    PIR4bits.TMR2IF = 0;
    PR2 = 0xff;
    T2CONbits.CKPS = 0b000; //Prescale 1:1
    T2CONbits.ON = 1; // on
    PIR4bits.TMR2IF = 0;
}