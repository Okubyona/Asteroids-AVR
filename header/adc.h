
#ifndef ADC_H
#define ADC_H

#include <avr/io.h>
#include <avr/interrupt.h>

void ADC_init() {
    ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
    // ADEN: setting this bit eneables analog-to-digital conversion.
    // ADSC: setting this bit starts the first conversion
    // ADATE: setting this bit enables auto-triggering. SInce we are
    //        in Free Running Mode, a new conversion will trigger whenever
    //        the previous conversion completes.

    ADMUX |= (1 << ADLAR);
    // ADLAR: setting this bit changes the way data is stored in the
    //      ADC data registers ADCL and ADCH.
    // When ADLAR is set to zero (default):
    //      ADCL stores ADC[0:7]
    //      ADCH stores ADC[8:9]
    // When ADLAR is set to 1:
    //      ADCL stores ADC[0:1]
    //      ADCH stores ADC[2:9]

    /* Note: Setting ADLAR to 1 has ADCH store the most significant bits of ADC */
    /* This is useful for data that does not need 10 bits of resolution
            as you can just use ADCH instead of ADC*/
}

ISR(ADC_vect) {

    switch (ADMUX) {
        case 0x00:
            ADMUX = 0x01;

            break;

        case 0x01;
            ADMUX = 0x00;
            break;

        default:
            ADMUX = 0;
            break;
    }
}

#endif
