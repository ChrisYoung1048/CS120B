/*	Author: Christopher Young - cyoun069@ucr.edu
 *  	Partner(s) Name: 
 *	Lab Section: 22
 *	Assignment: Lab #6 Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://drive.google.com/file/d/1iOx7EUXov2867OZLuzHVs2pvu0xQfupg/view?usp=sharing
 */
#include <avr/io.h>
#include "timer.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum States { SM_Start, Init, L1, L2, L3 } State;

void TickFct() {
	switch(State) { // transitions
		case SM_Start:
			State = Init;
			break;
		case Init:
			State = L1;
			break;
		case L1:
			State = L2;
			break;
		case L2:
			State = L3;
			break;
		case L3:
			State = L1;
			break;
		default:
			State = SM_Start;
			break;
	}

	switch(State) { // action
		case Init:
			PORTB = 0x00;
			break;
		case L1:
			PORTB = 0x01;
			break;
		case L2:
			PORTB = 0x02;
			break;
		case L3:
			PORTB = 0x04;
			break;
		default:
			break;
	}
}

int main(void) {
	/* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRB = 0xFF; PORTB = 0x00; // Configure port C's 8 pins as output initialized as 0x00

	TimerSet(1000);
	TimerOn();

	State = SM_Start;
	while (1) {
		TickFct();
		while (!TimerFlag) {}
		TimerFlag = 0;
	}
	return 1;
}
