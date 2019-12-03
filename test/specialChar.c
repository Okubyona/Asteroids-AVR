
#include <avr/io.h>
<<<<<<< HEAD
#include <avr/interrupt.h>
#include <stdio.h>
#include "adc.h"
#include "tasks.h"
#include "timer.h"
#include "io.h"

int main(void) {
    DDRC = 0xFF; PORTC = 0x00;
    DDRD = 0xC0; PORTD = 0x7F;  //PD6 and PD7 are set for input

    //custom characters to be used with 16x2 LCD
    unsigned char motherShip[] = { 0x09, 0x1C, 0x0E, 0x0F, 0x0F, 0x0E, 0x1C, 0x09 };
    unsigned char basicLaser[] = { 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x00, 0x00, 0x00 };
    unsigned char megaLaser[] =  { 0x1F, 0x00, 0x00, 0x1F, 0x1F, 0x00, 0x00, 0x1F };
    unsigned char bigAsteroid[] = {0x0E, 0x1E, 0x1E, 0x2F, 0x0F, 0x1F, 0x1F, 0x0E };
    unsigned char medAsteroid[] = {0x00, 0x0C, 0x1E, 0x1F, 0x0F, 0x0F, 0x0E, 0x00 };
    unsigned char smallAsteroid[] = {0x00, 0x00, 0x06, 0x0F, 0x0F, 0x06, 0x00, 0x00};
    // each char is assigned a 'location' from 0-5 in order from top to bottom

    LCD_init();

    LCD_CustomChar(0, motherShip);
    LCD_CustomChar(1, basicLaser);
    LCD_CustomChar(2, megaLaser);
    LCD_CustomChar(3, bigAsteroid);
    LCD_CustomChar(4, medAsteroid);
    LCD_CustomChar(5, smallAsteroid);

    for (unsigned char i = 0; i < 6; ++i) {
        LCD_WriteData(i);
        LCD_WriteData(' ');
    }

    while(1) ;
=======
#include <stdio.h>
#include "io.h"

int main(void) {
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xC0; PORTD = 0x7F;  //PD6 and PD7 are set for input

	//custom characters to be used with 16x2 LCD
	unsigned char motherShip[] = { 0x09, 0x1C, 0x0E, 0x0F, 0x0F, 0x0E, 0x1C, 0x09 };
	unsigned char basicLaser[] = { 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x00, 0x00, 0x00 };
	unsigned char megaLaser[] =  { 0x1F, 0x00, 0x00, 0x1F, 0x1F, 0x00, 0x00, 0x1F };
	unsigned char bigAsteroid[] = {0x0E, 0x1E, 0x1E, 0x2F, 0x0F, 0x1F, 0x1F, 0x0E };
	unsigned char medAsteroid[] = {0x00, 0x0C, 0x1E, 0x1F, 0x0F, 0x0F, 0x0E, 0x00 };
	unsigned char smallAsteroid[] = {0x00, 0x00, 0x06, 0x0F, 0x0F, 0x06, 0x00, 0x00};
	// each char is assigned a 'location' from 0-5 in order from top to bottom

	LCD_init();

	LCD_CustomChar(0, motherShip);
	LCD_CustomChar(1, basicLaser);
	LCD_CustomChar(2, megaLaser);
	LCD_CustomChar(3, bigAsteroid);
	LCD_CustomChar(4, medAsteroid);
	LCD_CustomChar(5, smallAsteroid);

	for (unsigned char i = 0; i < 6; ++i) {
		LCD_WriteData(i);
		LCD_WriteData(' ');
	}
	LCD_Cursor(13);

	while(1);
>>>>>>> hotfix

}
