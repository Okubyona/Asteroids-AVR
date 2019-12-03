
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "adc.h"
#include "tasks.h"
#include "timer.h"
#include "io.h"

int main(void) {
    DDRB = 0xFF; PORTC = 0x00;
    DDRD = 0xC0; PORTD = 0x7F;  // Pins used for LCD are set to output, rest are
                                // set as input
    LCD_BuildCustomChars();

    unsigned char i;
    static task task1, task2, task3, task4;
    task *tasks[] = {&task1, &task2, &task3, &task4}
    const unsigned short numTasks = sizeof(tasks) / sizeof(task*);


    unsigned long GCD = tasks[0].period;
	for (i = 1; i < numTasks; ++i) {
		GCD = findGCD(GCD, tasks[i].period);
	}

    ADC_init();
    LCD_init();
    TimerSet(GCD);
    TimerOn();


    while (1) {

    }

    return 0;
}
