
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "adc.h"
#include "tasks.h"
#include "timer.h"
#include "io.h"

typedef enum joystickTick{channel0, channel1};

int joystickTick(int state);

int main(void) {
    DDRC = 0xFF; PORTC = 0x00;
    DDRD = 0xFF; PORTD = 0x00;

    char adcVal[10];

    ADC_init();
    LCD_init();
    TimerSet(10);
    TimerOn();


    while (1) {
        //Loads ADC val from channel 0 into adcVal
        sprintf(adcVal, "X: %d", sensor1);
        LCD_DisplayString(1, adcVal);

        sprintf(adcVal, "Y: %d", sensor0);
        LCD_DisplayString(17, adcVal);


        while (!TimerFlag);
        TimerFlag = 0;
    }

    return 0;
}
