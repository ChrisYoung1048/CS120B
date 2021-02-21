/*	Author: Christopher Young - cyoun069@ucr.edu
 *  	Partner(s) Name: 
 *	Lab Section: 22
 *	Assignment: Lab #10 Exercise #3
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

void set_PWM(double frequency) {
	static double current_frequency;
	if (frequency != current_frequency) {
		if (!frequency) { TCCR3B &= 0x08; }
		else { TCCR3B |= 0x03; }

		if (frequency < 0.954) { OCR3A = 0xFFFF; }
		else if (frequency > 31250) { OCR3A = 0x0000; }
		else { OCR3A = (short) (8000000 / (128 * frequency)) - 1;}
		
		TCNT3 = 0;
		current_frequency = frequency;
	}
}

void PWM_on() {
	TCCR3A = (1 << COM3A0);
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	set_PWM(0);
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

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
	unsigned char press = ~PINA & 0x01;
	switch(state) {
		case insideLock_release: 
			if (press) { state = insideLock_press; unlocked = 0x00; }
			else { state = insideLock_release; }
			break;
		case insideLock_press:
			state = (press) ? insideLock_press : insideLock_release; break;
		default: state = insideLock_release;
	}
	
	return state;
}

enum doorbell_States { doorbell_init, doorbell_wait, doorbell_play};
double noteSequence[] = {293.66, 587.33, 440.00, 415.30, 392.00, 349.23, 293.66, 349.23, 392.00};
double noteTimeSequence[] = {2, 2, 2, 2, 2, 2, 1, 1, 1};
double downTimeSequence[] = {1, 1, 2, 1, 1, 1, 0, 0, 0};
unsigned char note_i, note_j, rest_k;

int doorbellSMTick(int state) {
	unsigned char press = ~PINA & 0x80;
	switch(state) { // transition
		case doorbell_init:
			state = doorbell_wait;
			break;
		case doorbell_wait:
			if (press) { // A7
				state = doorbell_play;
			}
			else {
				state = doorbell_wait;
			}
			break;
		case doorbell_play:
			if (note_i <= 9) {
				state = doorbell_play;
			}
			else {
				state = doorbell_wait;
				set_PWM(0);
				note_i = 0;
				note_j = 0;
				rest_k = 0;
			}
			break;
		default:
			state = doorbell_init;
			break;
	}

	switch(state) { // action
		case doorbell_init:
			note_i = 0;
			note_j = 0;
			rest_k = 0;
			break;
		case doorbell_play:
			if (note_j < noteTimeSequence[note_i]) {
				set_PWM(noteSequence[note_i]);
				++note_j;
			}
			else if (rest_k < downTimeSequence[note_i]) {
				set_PWM(0);
				++rest_k;
			}
			else {
				++note_i;
				note_j = 0;
				rest_k = 0;
			}
			break;
		default:
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
			output = 0x40 | unlocked << 7;
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
	DDRB = 0xFF; PORTB = 0x40; // Configure port B's 8 pins as output initialized as 0x00
	DDRC = 0xF0; PORTC = 0x0F; // Configure port C7-C4 as outputs and C3-C0 as inputs

	static task task1, task2, task3, task4;
	task *tasks[] = { &task1, &task2, &task3, &task4 };
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	const char start = -1;

	// Task 1 (detectSequenceSM)
	task1.state = start;
	task1.period = 200;
	task1.elapsedTime = task1.period;
	task1.TickFct = &detectSequenceSMTick;

	// Task 3 (insideLockSM)
	task3.state = start;
	task3.period = 200;
	task3.elapsedTime = task3.period;
	task3.TickFct = &insideLockSMTick;

	// Task 2 (displaySM)
	task2.state = start;
	task2.period = 100;
	task2.elapsedTime = task2.period;
	task2.TickFct = &displaySMTick;

	// Task 4 (doorbellSM)
	task4.state = start;
	task4.period = 200;
	task4.elapsedTime = task4.period;
	task4.TickFct = &doorbellSMTick;	

	unsigned short i; // iterator

	unsigned long GCD = tasks[0]->period;
	for (i = 0; i < numTasks; i++) {
		GCD = findGCD(GCD, tasks[i]->period);
	}

	unsigned char timerPeriod = GCD;
	TimerSet(timerPeriod);
	TimerOn();

	PWM_on();

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
