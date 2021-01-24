/*	Author: Christopher Young - cyoun069@ucr.edu
 *  	Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #4 Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag=0; // ISR raises, main() lowers

void TimerISR() {
   TimerFlag = 1;
}

enum States { SMStart, Init, Up, Down } State;

void TickFct() {
	switch(State) { // Transitions
		case SMStart:
			State = Init;
			break;
		case Init:
			State = Up;
			break;
		case Up:
			if (!(PINA & 0x01)) { // if not A0
				State = Up;
			}
			else if (PINA & 0x01) { // if A0
				PORTB = PINB ^ 0x03; // flip bit B1 and B0;
				State = Down;
			}
			break;
		case Down:
			if (PINA & 0x01) { // if A0
				State = Down;
			}
			else if (!(PINA & 0x01)) {
				State = Up;
			}
			break;
		default:
			State = SMStart;
	}

	switch(State) { // Actions
		case Init:
			PORTB = 0x01;
			break;
		default:
			break;
	}
}

int main(void) {
	/* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF; // Configure port D's 8 pins as inputs
	DDRB = 0xFF; PORTB = 0x00; // Configure port B's 8 pins as outputs initialized as 0's

	//TimerSet(2000);
	//TimerOn;

	State = SMStart;
	while (1) {
		TickFct();	
		
	}
	return 1;
}
