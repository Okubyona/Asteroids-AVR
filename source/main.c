
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include "adc.h"
#include "tasks.h"
#include "timer.h"
#include "io.c"

typedef enum joystickSM { j_wait, j_left, j_right};
typedef enum buttonSM { b_wait, b_press,};
typedef enum resetSM {r_wait, r_press};
typedef enum gameSM {g_wait, g_difficulty, g_game, g_lose, g_win};

int joystickTick(int state);

static unsigned char playerPos = 0;


int main(void) {
	DDRA = 0x00; PORTA = 0x0C;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xF0; PORTD = 0x0F;

	LCD_BuildCustomChars();

	unsigned char i;
	static task task1, task2, task3, task4;
	task *tasks[] = {&task1/*, &task2, &task3, &task4*/};
	const unsigned short numTasks = sizeof(tasks) / sizeof(task*);

	task1.state = j_wait;
	task1.period = 100;
	task1.elapsedTime = task1.period;
	task1.TickFct = &joystickTick;

	unsigned long GCD = tasks[0]->period;
	for (i = 1; i < numTasks; ++i) {
		GCD = findGCD(GCD, tasks[i]->period);
	}

	char buffer[16];

	ADC_init();
	LCD_init();
	TimerSet(50);
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

int joystickTick (int state) {

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
