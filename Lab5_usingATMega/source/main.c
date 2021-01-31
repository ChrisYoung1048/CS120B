/*	Author: Christopher Young - cyoun069@ucr.edu
 *  	Partner(s) Name: 
 *	Lab Section: 22
 *	Assignment: Lab #5 Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://drive.google.com/file/d/1uOo7hZao_xpnEmJZKU-vltfradVn1jV7/view?usp=sharing
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum States {SM_Start, Init, Wait, Press} State;
char tempB, direction;

void TickFct() {
	switch(State) { // transitions
		case SM_Start:
			State = Init;
			break;
		case Init:
			State = Wait;
			break;
		case Wait:
			if (!(PINA & 0x01)) {
				if (PINB == 0x3F) { // when B is 0011 1111
					PORTB = 0x00;
					direction = ~direction;
				}

				else if (direction & 0x01) {
					tempB = PINB >> 1;
					tempB+= 32;
					PORTB = tempB;
				}
				else {
					tempB = PINB << 1;
					tempB++;
					PORTB = tempB;
				}

				State = Press;
			}
			else {
				State = Wait;
			}
			break;
		case Press:
			if (!(PINA & 0x01)) {
				State = Press;
			}
			else {
				State = Wait;
			}
			break;
		default:
			State = SM_Start;
	}
	
	switch(State) { // actions 
		case Init:
			PORTB = 0x00;
			direction = 0x00;
			break;
		default:
			break;
	}
}

int main(void) {
	/* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRB = 0xFF; PORTB = 0x00; // Configure port B's 8 pins as output initialized as 0x00

	
	while (1) {
		TickFct();
	}
	return 1;
}
