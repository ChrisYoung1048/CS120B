/*	Author: Christopher Young - cyoun069@ucr.edu
 *  	Partner(s) Name: 
 *	Lab Section: 22
 *	Assignment: Lab #5 Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum States { Init, Wait, Down1, Down2, Reset } State;



void TickFct() {
	switch(State) { // Transitions
		case Init:
			State = Wait;
			break;
		case Wait:
			if ((~PINA & 0x01) && (~PINA & 0x02)) { // if A0 and A1
				State = Reset;
			}
			else if ((~PINA & 0x01)) { // if A0
				if (PINC < 9) PORTC = PINC + 1;	// if C < 9			
				State = Down1;
			}
			else if ((~PINA & 0x02)) { // if A1
				if (PINC > 0) PORTC = PINC - 1;	// if C > 0;			
				State = Down2;
			}
			else {
				State = Wait;
			}
			break;
		case Down1:
			if (!(~PINA & 0x01)) { // if !A0
				State = Wait;
			}
			else if ((~PINA & 0x01) && (~PINA & 0x02)) { // if A0 and A1
				State = Reset;
			}
			else {
				State = Down1;
			}
			break;
		case Down2:
			if (!(~PINA & 0x02)) { // if !A1
				State = Wait;
			}
			else if ((~PINA & 0x02) && (~PINA & 0x01)) { // if A1 and A0
				State = Reset;
			}
			else {
				State = Down2;
			}
			break;
		case Reset:
			if ((~PINA & 0x01) && (~PINA & 0x02)) { // if A0 and A1
				State = Reset;
			}
			else {
				State = Wait;
			}
		default:
			State = Init;
			break;
	}

	switch(State) { // Actions
		case Init:
			//PORTC = 7;
			break;
		case Reset:
			PORTC = 0;
			break;
		default:
			break;
	}
}

int main(void) {
	/* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRC = 0xFF; PORTC = 0x07; // Configure port C's 8 pins as output initialized as 0x07

	//TimerSet(2000);
	//TimerOn;

	State = Init;
	while (1) {
		TickFct();	
	}
	return 1;
}
