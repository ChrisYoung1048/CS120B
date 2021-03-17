/*	Author: Christopher Young - cyoun069@ucr.edu
 *  	Partner(s) Name: 
 *	Lab Section: 22
 *	Assignment: Lab #13 Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo: https://drive.google.com/file/d/1WEzHe8KmdU5Zm7V7VsGRIUDvVeO3U_X8/view?usp=sharing
 *
 */
#include <avr/io.h>
#include "timer.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

//unsigned short input = ADC;
void A2D_init() {
      ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	// ADEN: Enables analog-to-digital conversion
	// ADSC: Starts analog-to-digital conversion
	// ADATE: Enables auto-triggering, allowing for constant
	//	    analog to digital conversions.
}

typedef struct _task {
	signed char state;
	unsigned long int period;
	unsigned long int elapsedTime;
	int (*TickFct) (int);
} task;

enum Display_States {Display_init, Display_display};
int Display_Tick(int state);

//shared variables

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
	//DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRB = 0xFF; PORTB = 0x00; // Configure port B's 8 pins as output initialized as 0x00
	DDRC = 0xFF; PORTC = 0x00; // Configure port C's 8 pins as output initialized as 0x00
  	DDRD = 0xFF; PORTD = 0x00; // Configure port D's 8 pins as output initialized as 0x00

	static task task1;
	task *tasks[] = { &task1 };
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	const char start = -1;

	// Task 1 (display)
	task1.state = start;
	task1.period = 150;
	task1.elapsedTime = task1.period;
	task1.TickFct = &Display_Tick;

	unsigned short i; // iterator
	unsigned long GCD = tasks[0]->period;
	for (i = 0; i < numTasks; i++) {
		GCD = findGCD(GCD, tasks[i]->period);
	}
	
	unsigned char timerPeriod = GCD;
	TimerSet(timerPeriod);
	TimerOn();

	A2D_init();

	while (1) {
		PORTB = (char) ADC;
		PORTD = (char) (ADC >> 8);
		
	}
	return 1;
}

int Display_Tick(int state) {
	switch(state) {
		case Display_init: state = Display_display; break;
		case Display_display: state = Display_display; break;
		default: state = Display_init;
	}

	switch(state) {
		case Display_init:
			break;
		case Display_display:
			PORTB = (char) ADC;
		PORTD = (char) (ADC >> 8);
			break;
	}

	return state;
}
