
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
typedef enum menuSM {m_title, m_difficulty_select, m_highscore, m_idle};
typedef enum gameSM {g_wait, g_game, g_lose, g_win};

int joystickTick(int state);
int laserTick(int state);
int resetTick(int state);
int menuTick( int state);
int gameTick(int state);

#define JOYSTICK_X_IDLE 124;
#define JOYSTICK_Y_IDLE 122;

static unsigned char playerPos = 0;
static unsigned char laserFired = 0; // tell game to draw laser
static unsigned char activeLaser = 0; // only one laser can be on the screen at a time
static unsigned char asteroidHit = 0; // 0-4 where 1 = small 2 = medium 3 = large and 0 = no hit;
static unsigned char resetFlag = 0;  //  tells menu/ game to reset game/ highscore.
static unsigned char menuEnd = 0;	// menu end for gameStart
static unsigned char gameEnd = 0; // lets gameSM know when to run.

uint16 EEMEM ee_highscore_easy = 0;

uint16 highscore;
unsigned short score;

int main(void) {
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xF0; PORTD = 0x0F;

	LCD_BuildCustomChars();
	highscore = eeprom_read_word(&ee_highscore);

	unsigned char i;
	static task task1, task2, task3, task4, task5;
	task *tasks[] = {&task1, &task2, &task3, &task4, &task5};
	const unsigned short numTasks = sizeof(tasks) / sizeof(task*);

	task1.state = j_wait;
	task1.period = 50;
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

    task4.state = m_title;
    task4.period = 50;
    task4.elapsedTime = task4.period;
    task4.TickFct = &menuTick;

    task5.state = g_wait;
    task5.period = 100;
    task5.elapsedTime = task5.period;
    task5.TickFct = &gameTick;


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

        case j_up:

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
	       state = A3 ? b_cooldown: b_wait;
           break;
	}

	switch (state) {
		case b_wait: laserFired = 0; break;

		case b_press:
            if (gameStart) {
                laserFired = activeLaser ? 0: 1;
            }
            else {
                laserFired = A3 ? 1: 0;
            }
            // laser will only be fired when there is no laser character on the
            // screen.
            break;

		case b_cooldown:
		   state = A3 ? b_cooldown: b_wait;
           laserFired = 0;
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
        highscore = ee_highscore;
		break;

		case r_hold: break;
	}

	return state;
}

int menuTick(int state) {
	static char check0[17];
	static char check1[17];
	static char buffer0[17];
	static char buffer1[17];

    switch (state) {
        case m_title:
            state = laserFired ? m_difficulty_select: m_title;
            break;

		case m_difficulty_select:
			state = laserFired ? m_highscore: m_difficulty_select;
			break;

		case m_highscore:
			state = laserFired ? m_idle: m_highscore;
			break;

		case m_idle:
			state = gameEnd ? m_title: m_idle;
			break;
    }

    switch (state) {
		case m_title:
			menuEnd = 0;
			sprintf(buffer0, "Asteroids Puzzle");
			if (strcmp(buffer0, check0) != 0) { // if strings aren't equal
				LCD_DisplayString(1, buffer);
				check0 = buffer0;
			}

			sprintf(buffer, "                ");
			if (strcmp(buffer1, check1) != 0) {
				LCD_DisplayString(17, buffer);
				check1 = buffer1;
			}
			break;

		case m_difficulty_select:
			sprintf(buffer0, "   Difficulty   ");
			if (strcmp(buffer0, check0) != 0) {
				LCD_DisplayString(1, buffer0);
				check0 = buffer0;
			}
			sprintf(buffer1, "      Easy      ");
			if (strcmp(buffer1, check1) != 0) {
				LCD_DisplayString(1, buffer1);
				check1 = buffer1;
			}
			break;

		case m_highscore;
			sprintf(buffer0, "   High Score   ");
			if (strcmp(buffer0, check0) != 0) {
				LCD_DisplayString(1, buffer0);
				check0 = buffer0;
			}
			sprintf(buffer1, "      %d", highscore);
			break;

		case m_idle:
			menuEnd = 1;
			break;

    }

    return state;
}

int gameTick(int state) {

    return state;
}
