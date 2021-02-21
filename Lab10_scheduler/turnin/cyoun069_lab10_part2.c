/*	Author: Christopher Young - cyoun069@ucr.edu
 *  	Partner(s) Name: 
 *	Lab Section: 22
 *	Assignment: Lab #10 Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
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
unsigned char i = 0;
unsigned char unlocked = 0x00;

enum detectSequenceSM_States { detectSequence_wait, detectSequence_begin, detectSequence_released, detectSequence_pressed };
unsigned char numberSequence[] = {'1', '2', '3', '4', '5'};
int sequenceLength = 5;
int detectSequenceSMTick(int state) {
	switch(state) {
		case detectSequence_wait: 
			if (GetKeypadKey() == '#') { state = detectSequence_begin; }
			else { state = detectSequence_wait; }
			break;
		case detectSequence_begin:
			if (GetKeypadKey() == '\0') { state = detectSequence_released; }
			else { state = detectSequence_begin; } 
			break;
		case detectSequence_released:
			if (i == sequenceLength) { 
				state = detectSequence_wait;
				//PORTB = 0xFF;
				unlocked = 0x01; // unlocked
			}
			else if (GetKeypadKey() != '\0') { // if there is an input
				if (GetKeypadKey() == numberSequence[i]) { 
					PORTB = PINB + 1;					
					state = detectSequence_pressed; 
					i++;
				}
				else { state = detectSequence_wait; }
			}
			else { state = detectSequence_released; }
			break;
		case detectSequence_pressed:
			if (GetKeypadKey() == '\0') { state = detectSequence_released; }
			else { state = detectSequence_pressed; } 
			break;
		default: state = detectSequence_wait;
	}

	switch(state) {
		case detectSequence_begin: i = 0; break;
		default: break;
	}

	return state;
}

enum insideLock_States { insideLock_release, insideLock_press };
int insideLockSMTick(int state) {
	switch(state) {
		case insideLock_release: 
			if (PINA & 0x01) { state = insideLock_press; unlocked = 0x00; }
			else { state = insideLock_release; }
			break;
		case insideLock_press:
			state = (PINA & 0x01) ? insideLock_press : insideLock_release; break;
		default: state = insideLock_release;
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
			output = unlocked;
			break;
	}

	PORTB = output << 7; // write combined and shared variables to PORTB
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

	static task task1, task2, task3;
	task *tasks[] = { &task1, &task2, &task3 };
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	const char start = -1;

	// Task 1 (detectSequenceSM)
	task1.state = start;
	task1.period = 50;
	task1.elapsedTime = task1.period;
	task1.TickFct = &detectSequenceSMTick;

	// Task 3 (insideLockSM)
	task3.state = start;
	task3.period = 100;
	task3.elapsedTime = task3.period;
	task3.TickFct = &insideLockSMTick;

	// Task 2 (displaySM)
	task2.state = start;
	task2.period = 10;
	task2.elapsedTime = task2.period;
	task2.TickFct = &displaySMTick;

	
	
	
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
