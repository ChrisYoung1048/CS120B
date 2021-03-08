/*	Author: Christopher Young - cyoun069@ucr.edu
 *  	Partner(s) Name: 
 *	Lab Section: 22
 *	Assignment: Lab #12 Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://drive.google.com/file/d/1Jsn5YJKUVq266btIBlwvjQWtK50bsSEp/view?usp=sharing
 */
#include <avr/io.h>
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


//--------------------------------------
// LED Matrix Demo SynchSM
// Period: 100 ms
//--------------------------------------
enum Demo_States {shift};
int Demo_Tick(int state);

enum Display_States {Display_init, Display_display};
int Display_Tick(int state);

enum Control_States {Control_wait, Control_press};
int Control_Tick(int state);


//shared variables
unsigned char row = 0x00;
unsigned char col = 0x00;

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
	DDRC = 0xFF; PORTC = 0x00; // Configure port C's 8 pins as output initialized as 0x00
  	DDRD = 0xFF; PORTD = 0x00; // Configure port D's 8 pins as output initialized as 0x00

	static task task1, task2, task3;
	task *tasks[] = { &task3, &task2 };
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	const char start = -1;

	// Task 1 (demo)
	task1.state = start;
	task1.period = 150;
	task1.elapsedTime = task1.period;
	task1.TickFct = &Demo_Tick;

	// Task 2 (display)
	task2.state = start;
	task2.period = 1;
	task2.elapsedTime = task2.period;
	task2.TickFct = &Display_Tick;

	// Task 3 (control)
	task3.state = start;
	task3.period = 100;
	task3.elapsedTime = task3.period;
	task3.TickFct = &Control_Tick;

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

int Demo_Tick(int state) {

	// Local Variables
	static unsigned char pattern = 0x80;	// LED pattern - 0: LED off; 1: LED on
	static unsigned char row = 0xFE;  	// Row(s) displaying pattern. 
							// 0: display pattern on row
							// 1: do NOT display pattern on row
	// Transitions
	switch (state) {
		case shift:	
			break;
		default:	
			state = shift;
			break;
	}
	
	// Actions
	switch (state) {
		case shift:	
			if (row == 0xEF && pattern == 0x01) { // Reset demo 
				pattern = 0x80;		    
				row = 0xFE;
			} else if (pattern == 0x01) { // Move LED to start of next row
				pattern = 0x80;
				row = (row << 1) | 0x01;
			} else { // Shift LED one spot to the right on current row
				pattern >>= 1;
			}
			break;
		default:
			break;
	}

	PORTC = pattern;	// Pattern to display
	PORTD = row;		// Row(s) displaying pattern	
	return state;
}

int Display_Tick(int state) {
	switch(state) {
		case Display_init: state = Display_display; break;
		case Display_display: state = Display_display; break;
		default: state = Display_init;
	}

	switch(state) {
		case Display_init:
			col = 0x01;
			row = 0x1F;
			break;
		case Display_display:
			PORTC = col;
			PORTD = ~row;
			break;
	}

	return state;
}

int Control_Tick(int state) {
	unsigned char A0 = ~PINA & 0x01; // up
	unsigned char A1 = ~PINA & 0x02; // down
	unsigned char A2 = ~PINA & 0x04; // left
	unsigned char A3 = ~PINA & 0x08; // right
	switch(state) {
		case Control_wait:
			if(A0) {
				state = Control_press;
				if (row > 0x01) row = row >> 1;
			}
			else if(A1) {
				state = Control_press;
				if (row < 0x10) row = row << 1;
			}
			else if(A2) {
				state = Control_press;
				if (col > 0x01) col = col >> 1;
			}
			else if(A3) {
				state = Control_press;
				if (col < 0x80) col = col << 1;
			}
			break;
		case Control_press:
			if(A0 || A1 || A2 || A3) state = Control_press;
			else state = Control_wait;
			break;
		default: state = Control_wait;
	}

	switch(state) {
		default: break;
	}

	return state;
}
