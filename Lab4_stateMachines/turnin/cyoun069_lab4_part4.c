/*	Author: Christopher Young - cyoun069@ucr.edu
 *  	Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #4 Exercise #4
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

enum LS_States { LS_SMStart, LS_Wait, LS_LockDown, LS_Step1, LS_Step2, LS_Step3 } LS_State;

void TickFct() {
	switch(LS_State) { // Transitions
		case LS_SMStart:
			LS_State = LS_Wait;
			break;
		case LS_Wait:
			if (PINA & 0x80) { // if A7
				LS_State = LS_LockDown;
			}
			else if ((PINA & 0x07) == 0x04) { // if A2, !A1, !A0
				LS_State = LS_Step1;
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
		case LS_Step1:
			if ((PINA & 0x07) == 0x04) { // if A2, !A1, !A0
				LS_State = LS_Step1;
			}
			else if ((PINA & 0x07) == 0x00) { //if !A2, !A1, !A0
				LS_State = LS_Step2;
			}
			else {
				LS_State = LS_Wait;
			}
			break;
		case LS_Step2:
			if ((PINA & 0x07) == 0x00) { //if !A2, !A1, !A0
				LS_State = LS_Step2;
			}
			else if ((PINA & 0x07) == 0x02) { //if !A2, A1, !A0 
				PORTB = PINB ^ 0x01;
				LS_State = LS_Step3;
			}
			else {
				LS_State = LS_Wait;
			}
			break;
		case LS_Step3:
			if ((PINA & 0x07) == 0x02) { //if !A2, A1, !A0 
				LS_State = LS_Step3;
			}
			else {
				LS_State = LS_Wait;
			}
			break;
		default:
			LS_State = LS_SMStart;
			break;
	}

	switch(LS_State) { // Actions
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
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
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
