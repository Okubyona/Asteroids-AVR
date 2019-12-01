
#include <avr/io.h>
#include <avr/interrupt.h>
#include "adc.h"
#include "tasks.h"
#include "timer.h"
#include "io.h"

int main(void) {
    DDRC = 0xFF; PORTC = 0x00;
    DDRD = 0xFF; PORTD = 0x00;

    ADC_init();
    LCD_init();


    TimerSet(10);
    TimerOn();

    while (1) {
        LCD_DisplayString(1, "X: ");

        LCD_DisplayInteger(4, sensor0);

        LCD_DisplayString(17, "Y: ");

        LCD_DisplayInteger(20, sensor1);

    }

    return 0;
}
