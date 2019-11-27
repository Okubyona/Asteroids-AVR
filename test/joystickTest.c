
#include <avr/io.h>
#include <avr/interrupt.h>
#include "adc.h"
#include "tasks.h"
#include "timer.h"
#include "io.h"

int main(void) {
    DDRC = 0xFF; PORTC = 0x00;
    DDRD = 0xFF; PORTD = 0x00;

    while (1) {

    }

    return 0;
}
