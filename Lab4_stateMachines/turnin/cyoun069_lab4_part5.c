/*	Author: Christopher Young - cyoun069@ucr.edu
 *  	Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #4 Exercise #5
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

enum LS_States { LS_SMStart, LS_Wait, LS_LockDown, LS_Sequence } LS_State;
char buttonSequence[4] = {4, 1, 2, 1};
int i = 0;

void TickFct() {
	switch(LS_State) { // Transitions
		case LS_SMStart:
			LS_State = LS_Wait;
			break;
		case LS_Wait:
			if (PINA & 0x80) { // if A7
				LS_State = LS_LockDown;
			}
			else if (PINA > 0x00) { // if A2 or A1 or A0
				LS_State = LS_Sequence;
			}
			else {
				LS_State = LS_Wait;
			}
			break;
		case LS_LockDown:
			if (PINA & 0x80) { // if A7
				LS_State = LS_LockDown;
			}
			else {
				LS_State = LS_Wait;
			}
			break;
		case LS_Sequence:
			if (PINA == buttonSequence[i]) { // if the input matches the next sequence element
				i++;
				if (i == 4) { // if end of sequence is reached
					PORTB = PINB ^ 0x01;
					i = 0;
				}
				LS_State = LS_Wait;
			}
			else { // if the sequence is broken
				i = 0;
				LS_State = LS_Wait;
			}
			break;

		default:
			LS_State = LS_SMStart;
			break;
	}

	switch(LS_State) { // Actions
		case LS_SMStart:
			PORTB = 0x00;
			break;
		case LS_LockDown:
			PORTB = 0x00;
			break;
		default:
			break;
	}

	PORTC = LS_State; // PORTC tracks current state for debugging purposes
}

int main(void) {
	/* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0x00; // Configure port A's 8 pins as inputs
	DDRB = 0xFF; PORTB = 0x00; // Configure port B's 8 pins as output initialized as 0x00
	DDRC = 0xFF; PORTC = 0x00; // Configure port C's 8 pins as output initialized as 0x00

	//TimerSet(2000);
	//TimerOn;

	LS_State = LS_SMStart;
	while (1) {
		TickFct();	
	}
	return 1;
}
