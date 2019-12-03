
#include "io.h"

#define SET_BIT(p,i) ((p) |= (1 << (i)))
#define CLR_BIT(p,i) ((p) &= ~(1 << (i)))
#define GET_BIT(p,i) ((p) & (1 << (i)))

/*-------------------------------------------------------------------------*/

#define DATA_BUS PORTC		// port connected to pins 7-14 of LCD display
#define CONTROL_BUS PORTD	// port connected to pins 4 and 6 of LCD disp.
#define RS 6			// pin number of uC connected to pin 4 of LCD disp.
#define E 7			// pin number of uC connected to pin 6 of LCD disp.

/*-------------------------------------------------------------------------*/

void LCD_ClearScreen(void) {
   LCD_WriteCommand(0x01);
}

void LCD_init(void) {

    //wait for 100 ms.
	delay_ms(100);
	LCD_WriteCommand(0x38);
	LCD_WriteCommand(0x06);
	LCD_WriteCommand(0x0f);
	LCD_WriteCommand(0x01);
	delay_ms(10);
}

void LCD_WriteCommand (unsigned char Command) {
   CLR_BIT(CONTROL_BUS,RS);
   DATA_BUS = Command;
   SET_BIT(CONTROL_BUS,E);
   asm("nop");
   CLR_BIT(CONTROL_BUS,E);
   delay_ms(2); // ClearScreen requires 1.52ms to execute
}

void LCD_WriteData(unsigned char Data) {
   SET_BIT(CONTROL_BUS,RS);
   DATA_BUS = Data;
   SET_BIT(CONTROL_BUS,E);
   asm("nop");
   CLR_BIT(CONTROL_BUS,E);
   delay_ms(1);
}

void LCD_Cursor(unsigned char column) {
   if ( column < 17 ) { // 16x1 LCD: column < 9
						// 16x2 LCD: column < 17
      LCD_WriteCommand(0x80 + column - 1);
   } else {
      LCD_WriteCommand(0xB8 + column - 9);	// 16x1 LCD: column - 1
											// 16x2 LCD: column - 9
   }
}

void LCD_DisplayString(unsigned char column, const unsigned char* string) {
    //LCD_ClearScreen();
    unsigned char c = column;
    while(*string) {
        LCD_Cursor(c++);
        LCD_WriteData(*string++);
   }
}

//Function that builds a given custom char
void LCD_CustomChar(unsigned char location, unsigned char* customData) {
    unsigned char i;
    // LCD CGRAM can only support 8 characters
    if (location < 8 ) {
        // command 0x40 and up forces device to point to CGRAM addresses
        LCD_WriteCommand(0x40 + (location * 8));
        //write byte data for current custom character
        for (i = 0; i < 8; ++i) {
            LCD_WriteData(customData[i]);
        }
    }

    LCD_WriteCommand(0x80);
}

// Function that builds a list of custom chars
void LCD_BuildCustomChars() {
    //custom characters to be used with 16x2 LCD
	unsigned char motherShip[] = { 0x09, 0x1C, 0x0E, 0x0F, 0x0F, 0x0E, 0x1C, 0x09 };
	unsigned char basicLaser[] = { 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x00, 0x00, 0x00 };
	unsigned char megaLaser[] =  { 0x1F, 0x00, 0x00, 0x1F, 0x1F, 0x00, 0x00, 0x1F };
	unsigned char bigAsteroid[] = {0x0E, 0x1E, 0x1E, 0x2F, 0x0F, 0x1F, 0x1F, 0x0E };
	unsigned char medAsteroid[] = {0x00, 0x0C, 0x1E, 0x1F, 0x0F, 0x0F, 0x0E, 0x00 };
	unsigned char smallAsteroid[] = {0x00, 0x00, 0x06, 0x0F, 0x0F, 0x06, 0x00, 0x00};
	// each char is assigned a 'location' from 0-5 in order from top to bottom

    LCD_CustomChar(0, motherShip);
	LCD_CustomChar(1, basicLaser);
	LCD_CustomChar(2, megaLaser);
	LCD_CustomChar(3, bigAsteroid);
	LCD_CustomChar(4, medAsteroid);
	LCD_CustomChar(5, smallAsteroid);

}

void delay_ms(int miliSec) //for 8 Mhz crystal

{
    int i,j;
    for(i=0;i<miliSec;i++)
    for(j=0;j<775;j++)
  {
   asm("nop");
  }
}
