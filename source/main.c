
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdio.h>
#include <stdlib.h>
#include "adc.h"
#include "tasks.h"
#include "timer.h"
#include "io.c"

typedef enum joystickSM { j_wait, j_left, j_right};
typedef enum laserSM { b_wait, b_press, b_cooldown};
typedef enum resetSM {r_wait, r_press, r_hold};
typedef enum gameSM {g_wait, g_difficulty, g_game, g_lose, g_win};

int joystickTick(int state);
int laserTick(int state);
int resetTick(int state);
int gameTick(int state);


static unsigned char playerPos = 0;
static unsigned char laserFired = 0;
static unsigned char asteroidHit = 1;

unsigned short EEMEM ee_highscore = 0;
unsigned short highscore;
unsigned short score;

int main(void) {
	DDRA = 0x00; PORTA = 0x0C;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xF0; PORTD = 0x0F;

	LCD_BuildCustomChars();
	highscore = eeprom_read_word(&ee_highscore);

	unsigned char i;
	static task task1, task2, task3, task4;
	task *tasks[] = {&task1, &task2, &task3, &task4};
	const unsigned short numTasks = sizeof(tasks) / sizeof(task*);

	task1.state = j_wait;
	task1.period = 100;
	task1.elapsedTime = task1.period;
	task1.TickFct = &joystickTick;

	task2.state = b_wait;
	task2.period = 50;
	task2.elapsedTime = task2.period;
	task2.TickFct = &laserTick;

	task3.state = r_wait;
	task3.period = 50;
	task3.elapsedTime = task3.period;
	task3.TickFct = &resetTick;

    task4.state = g_wait;
    task4.period = 200;
    task4.elapsedTime = task4.period;
    task4.TickFct = &gameTick;


	unsigned long GCD = tasks[0]->period;
	for (i = 1; i < numTasks; ++i) {
		GCD = findGCD(GCD, tasks[i]->period);
	}

	ADC_init();
	LCD_init();
	TimerSet(GCD);
	TimerOn();


	while (1) {
		for (i = 0; i < numTasks; i++) {
			if (tasks[i]->elapsedTime == tasks[i]->period) {
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += GCD;
		}

		while (!TimerFlag);
		TimerFlag = 0;
	}

	return 0;
}

int joystickTick(int state) {

	switch (state) {
		case j_wait:
		if (sensor1 < 120) { state = j_left; }
		else if (sensor1 > 128) { state = j_right; }
		else { state = j_wait; }
		break;

		case j_left:
		if (sensor1 < 120) { state = j_left; }
		else { state = j_wait; }
		break;

		case j_right:
		if (sensor1 > 128) { state = j_right; }
		else {state = j_wait; }
		break;
	}

	switch (state) {
		case j_wait: break;

		case j_left: playerPos = 0; break;

		case j_right: playerPos = 1; break;
	}

	return state;
}

int laserTick(int state) {
	unsigned char A3 = ~PINA & 0x08; //Grab input from PINA3; fourth pin

	switch (state) {
		case b_wait:
		state = A3 ? b_press: b_wait;
		break;

		case b_press:
		state = b_cooldown;
		break;

		case b_cooldown:
		state = laserFired ? b_cooldown : b_wait;
		break;
	}

	switch (state) {
		case b_wait: laserFired = 0; break;

		case b_press:
		laserFired = 1;
		break;

		case b_cooldown:
		if (asteroidHit) { laserFired = 0; }
		break;

	}


	return state;
}

int resetTick(int state) {
	unsigned char A2 = ~PINA & 0x04;

	switch (state) {
		case r_wait:
		state = A2 ? r_press: r_wait;
		break;

		case r_press:
		state = A2? r_hold: r_wait;
		break;

		case r_hold:
		state = A2 ? r_hold: r_wait;
		break;
	}

	switch (state) {
		case r_wait: break;

		case r_press:
		eeprom_write_word(&ee_highscore, 0);
        task4.state = wait;
		break;

		case r_hold: break;
	}

	return state;
}

int gameTick(int state) {
    unsigned char collision = 0;
    unsigned char i;
    char screen[17];
    char easy0[61];
    char easy1[61];
    sprintf(easy0, " p                   s     m   s       m          s s s     ");
    sprintf(easy1, "               s     s       s     s       m  s          L  ");

    unsigned char gametime = strlen(easy0);


    switch (state) {
        case g_wait:
            state = A3 ? g_difficulty: g_wait;
            break;

        case g_difficulty:
            state = A4 ? g_game: g_difficulty;
            break;

        case g_game:
            if (i < gametime) { state = g_game; }
            else if (collision) { state = g_lose; }
            else { state = g_win; }
            break;

        case g_lose:
            state = A4 ? g_wait: g_lose;
            break;

        case g_win:
            state = A4 ? g_wait: g_win;
            break;

    }

    switch (state) {
        case g_wait:
            playerPos = 0;
            asteroidHit = 1;

            sprintf(screen, "  Asteroids-AVR ");
            LCD_DisplayString(1, screen);
            break;

    }


    return state;
}
