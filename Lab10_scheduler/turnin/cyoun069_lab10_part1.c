/*	Author: Christopher Young - cyoun069@ucr.edu
 *  	Partner(s) Name: 
 *	Lab Section: 22
 *	Assignment: Lab #10 Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo : https://drive.google.com/file/d/1xG5d39b-7AlDrVnCMRxOPoUQpHd_0rpu/view?usp=sharing
 */
#include <avr/io.h>
#include "keypad.h"
#include "timer.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

typedef struct _task {
	signed char state;
	unsigned long int period;
	unsigned long int elapsedTime;
	int (*TickFct) (int);
} task;

//shared variables
unsigned char led0_output = 0x00;
unsigned char led1_output = 0x00;
unsigned char pause = 0;

enum pauseButtonSM_States { pauseButton_wait, pauseButton_press, pauseButton_release };
int pauseButtonSMTick(int state) {
	//local variables
	unsigned char press = ~PINA & 0x01;

	switch(state) { // transitions
		case pauseButton_wait:
			state = press == 0x01? pauseButton_press: pauseButton_wait; break;
		case pauseButton_press:
			state = pauseButton_release; break;
		case pauseButton_release:
			state = press = 0x00? pauseButton_wait: pauseButton_press; break;
		default: state = pauseButton_wait; break;
	}

	switch(state) { // actions
		case pauseButton_wait: break;
		case pauseButton_press:
			pause = (pause == 0) ? 1 : 0; 
			break;
		case pauseButton_release: break;
	}

	return state;
}

enum toggleLED0_States { toggleLED0_wait, toggleLED0_blink };
int toggleLED0SMTick(int state) {
		switch(state) { // transitions
			case toggleLED0_wait: state = !pause? toggleLED0_blink: toggleLED0_wait; break;
			case toggleLED0_blink: state = pause? toggleLED0_wait: toggleLED0_blink; break;
			default: state = toggleLED0_wait; break;
		}
		
		switch(state) { // actions

			case toggleLED0_wait: break;
			case toggleLED0_blink:
				led0_output = (led0_output == 0x00) ? 0x01 : 0x00;
				break;
		}

		// for exercise 1
		if (GetKeypadKey() != '\0') { led0_output = led0_output | 0x80; } 

		return state;
}

enum toggleLED1_States { toggleLED1_wait, toggleLED1_blink };
int toggleLED1SMTick(int state) {
	switch(state) { // transitions
		case toggleLED1_wait: state = !pause? toggleLED1_blink: toggleLED1_wait; break;
		case toggleLED1_blink: state = pause? toggleLED1_wait: toggleLED1_blink; break;
		default: state = toggleLED1_wait; break;
	}
	
	switch(state) { // actions
		case toggleLED1_wait: break;
		case toggleLED1_blink:
			led1_output = (led1_output == 0x00) ? 0x01 : 0x00;
			break;
	}

	return state;
}

enum display_States { display_display };
int displaySMTick(int state) {
	//local variables
	unsigned char output;
	
	switch(state) {
		case display_display: state = display_display; break;
		default: state = display_display; break;
	}

	switch(state) {
		case display_display:
			output = led0_output | led1_output << 1;
			break;
	}

	PORTB = output; // write combined and shared variables to PORTB
	return state;
}

unsigned long int findGCD(unsigned long int a, unsigned long int b) {
	unsigned long int c;
	while(1) {
		c = a%b;
		if (c == 0) { return b;}
		a = b;
		b = c;
	}
	return 0;
}

int main(void) {
	/* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRB = 0xFF; PORTB = 0x00; // Configure port B's 8 pins as output initialized as 0x00
	DDRC = 0xF0; PORTC = 0x0F; // Configure port C7-C4 as outputs and C3-C0 as inputs

	static task task1, task2, task3, task4;
	task *tasks[] = { &task1, &task2, &task3, &task4 };
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	const char start = -1;

	// Task 1 (pauseButtonToggleSM)
	task1.state = start;
	task1.period = 50;
	task1.elapsedTime = task1.period;
	task1.TickFct = &pauseButtonSMTick;

	// Task 2 (toggleLED0SM)
	task2.state = start;
	task2.period = 500;
	task2.elapsedTime = task2.period;
	task2.TickFct = &toggleLED0SMTick;

	// Task 3 (toggleLED1SM)
	task3.state = start;
	task3.period = 1000;
	task3.elapsedTime = task3.period;
	task3.TickFct = &toggleLED1SMTick;

	// Task 4 (displaySM)
	task4.state = start;
	task4.period = 10;
	task4.elapsedTime = task4.period;
	task4.TickFct = &displaySMTick;
	
	unsigned short i; // iterator

	unsigned long GCD = tasks[0]->period;
	for (i = 0; i < numTasks; i++) {
		GCD = findGCD(GCD, tasks[i]->period);
	}

	unsigned char timerPeriod = GCD;
	TimerSet(timerPeriod);
	TimerOn();

	while (1) {
		for (i = 0; i < numTasks; i++) { // scheduler
			if (tasks[i]->elapsedTime == tasks[i]->period) { // if tick
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += timerPeriod;
		}
		while (!TimerFlag) {}
		TimerFlag = 0;
		
	}
	return 1;
}
