/*	Author: Christopher Young - cyoun069@ucr.edu
 *  	Partner(s) Name: 
 *	Lab Section: 22
 *	Assignment: Lab #6 Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include "timer.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum States { SM_Start, Init, Cycle, Press1, Release1, Press2 } State;
unsigned char i, direction;


void TickFct() {
	switch(State) { // transitions
		case SM_Start:
			State = Init;
			break;
		case Init:
			State = Cycle;
			i = 0;
			break;
		case Cycle:
			if (PINA & 0x01) {
				State = Cycle;
			}
			else {
				State = Press1;
			}
			break;
		case Press1:
			if (!(PINA & 0x01)) {
				State = Press1;
			}
			else {
				State = Release1;
			}
			break;
		case Release1: 
			if (PINA & 0x01) {
				State = Release1;
			}
			else {
				State = Press2;
			}
			break;
		case Press2:
			if (!(PINA & 0x01)) {
				State = Press2;
			}
			else {
				State = Init;
			}
			break;
		default:
			State = SM_Start;
			break;
	}

	switch(State) { // action
		case Init:
			PORTB = 0x01;
			i = 0;
			direction = 0;
			break;
		case Cycle:
			if (direction) {
				PORTB = PINB >> 1;
			}
			else {
				PORTB = PINB << 1;
			}
			i++;

			if (i >= 2) {
				i = 0;
				direction = ~direction;
			}
			break;
		default:
			break;
	}
}

int main(void) {
	/* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRB = 0xFF; PORTB = 0x00; // Configure port C's 8 pins as output initialized as 0x00

	TimerSet(300);
	TimerOn();

	State = SM_Start;
	while (1) {
		TickFct();
		while (!TimerFlag) {}
		TimerFlag = 0;
	}
	return 1;
}
