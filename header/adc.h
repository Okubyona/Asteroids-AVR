
#ifndef ADC_H
#define ADC_H

#include <avr/io.h>
#include <avr/interrupt.h>

volatile unsigned char sensor0;
volatile unsigned char sensor1;

void ADC_init() {
    //ADMUX |= ( 1 << REFS0);
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADEN);
    // REFS0: setting this bit sets the reference voltage to AVCC
    // ADPS[2:0]: setting these bits sets a circuit clock frequency to help
    //      divide the system clock speed to the required range for ADC circuitry
    //      (i.e.) range is 50KHz - 200KHz; if system clock speed is 8MHz then
    /*  ADPS2 ADPS1 ADPS0    Division Factor
          0     0     0              2      (default)
          0     0     1              4
          0     1     0              8
          1     1     0              64

          These values set the ADC circuit speed to 8MHz / (Division Factor)
          This program will be using a division factor of 64 so
          ADC clock speed = 8MHz / 64 = 125KHz
    */
    // ADEN: setting this bit eneables analog-to-digital conversion.
    ADCSRA |= (1 << ADIE) | (1 << ADATE);
    // ADSC: setting this bit starts the first conversion
    // ADIE: setting this bit enables ADC conversion interrupts
    //      this means ISR(ADC_vect) is/will be in use
    // ADATE: setting this bit enables auto-triggering. SInce we are
    //        in Free Running Mode, a new conversion will trigger whenever
    //        the previous conversion completes.

    ADMUX |= (1 << ADLAR);
    ADCSRA |= (1 << ADSC);
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
        //ADMUX = 0x20 on default
        //case for channel 0
        case 0x20:
            sensor0 = ADCH;
            // Changes ADC channel to 1
            ADMUX = (ADMUX & 0xF0) | (0x01 & 0x0F);
            break;

        case 0x21:
            sensor1 = ADCH;
            // Changes ADC channel to 0
            ADMUX = (ADMUX & 0xF0) | (0x00 & 0x0F);
            break;
    }


    //ADCSRA |= (1 << ADSC); Not needed b/c ADATE bit is set
}

#endif
