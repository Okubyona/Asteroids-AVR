
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "adc.h"
#include "tasks.h"
#include "timer.h"
#include "io.c"

typedef enum joystickSM { j_wait, j_left, j_right};
typedef enum laserSM { b_wait, b_press, b_cooldown};
typedef enum resetSM {r_wait, r_press, r_hold};
typedef enum menuSM {m_title, m_difficulty_select, m_highscore, m_idle};
typedef enum gameSM {g_wait, g_game, g_lose, g_win, g_highscore};

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
static unsigned char laserPos = 2;
static unsigned char laserRow = 0;
static unsigned char asteroidHit = 0; // 0-4 where 1 = small 2 = medium 3 = large and 0 = no hit;
static unsigned char resetFlag = 0;  //  tells menu/ game to reset game/ highscore.
static unsigned char menuEnd = 0;	// menu end for gameStart
static unsigned char gameStart = 0; // lets gameSM know when to run.
static unsigned char gameEnd = 0;
static unsigned char gameWon = 0;
static unsigned int gameTime = 0; // Game will run for 45 seconds

uint16_t EEMEM ee_highscore_easy;

uint16_t highscore;
unsigned short score;

int main(void) {
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xF0; PORTD = 0x0F;

	LCD_BuildCustomChars();
	highscore = eeprom_read_word(&ee_highscore_easy);

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
		if (sensor1 > 140) { state = j_right; }
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
		case r_wait:
		resetFlag = 0;
		break;

		case r_press:
		eeprom_write_word(&ee_highscore_easy, 0);
		highscore = &ee_highscore_easy;
		resetFlag = 1;
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
		if (resetFlag) { state = m_title; }
		break;

		case m_highscore:
		state = laserFired ? m_idle: m_highscore;
		if (resetFlag) { state = m_title; }
		break;

		case m_idle:
		state = gameEnd ? m_title: m_idle;
		if (resetFlag) { state = m_title; }
		break;
	}

	switch (state) {
		case m_title:
		menuEnd = 0;
		gameStart = 0;
		sprintf(buffer0, "Asteroids Puzzle");
		if (strcmp(buffer0, check0) != 0) { // if strings aren't equal
			LCD_DisplayString(1, buffer0);
			strcpy(check0, buffer0);
		}
		sprintf(buffer1, "                ");
		if (strcmp(buffer1, check1) != 0) {
			LCD_DisplayString(17, buffer1);
			strcpy(check1, buffer1);
		}
		break;

		case m_difficulty_select:
		sprintf(buffer0, "   Difficulty   ");
		if (strcmp(buffer0, check0) != 0) {
			LCD_DisplayString(1, buffer0);
			strcpy(check0, buffer0);
		}
		sprintf(buffer1, "      Easy      ");
		if (strcmp(buffer1, check1) != 0) {
			LCD_DisplayString(17, buffer1);
			strcpy(check1, buffer1);
			LCD_Cursor(17 + 4);
		}
		break;

		case m_highscore:
		sprintf(buffer0, "   High Score   ");
		if (strcmp(buffer0, check0) != 0) {
			LCD_DisplayString(1, buffer0);
			strcpy(check0, buffer0);
		}
		sprintf(buffer1, "       %d", highscore);
		if (strcmp(buffer1, check1) != 0) {
			LCD_DisplayString(17, buffer1);
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_WriteData(' ');
			LCD_Cursor(0);
			strcpy(check1, buffer1);
		}
		break;

		case m_idle:
		menuEnd = 1;
		break;

	}

	return state;
}

int gameTick(int state) {
	const unsigned char easy_level0[] = "     s   s m   L";
	const unsigned char easy_level1[] = "     m  s s  s s";
	static unsigned char level_row0[17];
	static unsigned char level_row1[17];

	switch (state) {
		case g_wait:
		state = menuEnd ? g_game: g_wait;
		laserFired = 0;
		break;

		case g_game:
		if (gameTime == 45000) {
			gameStart = 0;
			state = gameWon ? g_win: g_lose;
		}
		else { state = g_game; }
		break;

		case g_lose:
		state = laserFired ? g_wait: g_lose;
		break;

		case g_win:
		if (laserFired && score > highscore) { state = g_highscore; }
		else if (laserFired && score <= highscore) { state = g_wait; }
		else { state = g_win; }
		break;

		case g_highscore:
		state = laserFired ? g_wait: g_highscore;
		break;

	}

	switch (state) {
		case g_wait:
		gameTime = 0;
		gameStart = 0;
		gameEnd = 0;
		score = 0;
		asteroidHit = 0;
		activeLaser = 0;
		laserPos = 2;
		if (strcmp(level_row0, easy_level0) != 0) { strcpy(level_row0, easy_level0); }
		if (strcmp(level_row1, easy_level1) != 0) {	strcpy(level_row1, easy_level1); }
		break;

		case g_game:
		gameStart = 1;
		LCD_DrawGame(level_row0, level_row1, playerPos);

		if (laserFired && activeLaser == 0) {
			if (playerPos) { level_row1[laserPos] = 'l'; laserRow = 1;}
			else { level_row0[laserPos] = 'l'; laserRow = 0;}
			activeLaser = 1;
		}

		if (activeLaser) {
			if (laserRow && laserPos != 16) {
				if (level_row1[laserPos + 1] == ' ') {
					level_row1[laserPos] = level_row1[laserPos + 1];
					level_row1[laserPos + 1] = 'l';
				}
				else if (level_row1[laserPos + 1] == 's') {
					activeLaser = 0;
					score += 25;
					level_row1[laserPos] = ' ';
					level_row1[laserPos + 1] = ' ';
					laserPos = 2;
				}
				else if (level_row1[laserPos + 1] == 'm') {
					activeLaser = 0;
					score += 50;
					level_row1[laserPos] = ' ';
					if (level_row0[laserPos + 1] != ' ') { level_row0[laserPos] = 's'; }
					else { level_row0[laserPos + 1] = 's'; }
					level_row1[laserPos + 1] = 's';

				}
			}
			++laserPos;
		}



		gameTime += 100;
		break;

	}

	return state;
}
