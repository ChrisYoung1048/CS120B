/*	Author: Christopher Young - cyoun069@ucr.edu
 *  	Partner(s) Name: 
 *	Lab Section: 22
 *	Assignment: Lab #6 Exercise #3
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://drive.google.com/file/d/1ugjY_BrAfKrtqF0u4smzJ9ZwLwQ1XxD2/view?usp=sharing
 */
#include <avr/io.h>
#include "timer.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum States { SM_Start, Init, Wait, Increment, Decrement, Reset } State;
unsigned char i;


void TickFct() {
	switch(State) { // transitions
		case SM_Start:
			State = Init;
			break;
		case Init:
			State = Wait;
			break;
		case Wait:
			if (!(PINA & 0x01) && (PINA & 0x02)) { // if A0 && !A1
				State = Increment;
				if (PORTB < 9) PORTB = PINB + 1;
				i = 0;
			}
			else if ((PINA & 0x01) && !(PINA & 0x02)) { // if !A0 && A1
				State = Decrement;
				if (PORTB > 0) PORTB = PINB - 1;
				i = 0;
			}
			else if (!(PINA & 0x01) && !(PINA & 0x02)) { // if A0 && A1
				State = Reset;
			}
			else {
				State = Wait;
			}
			break;
		case Increment:
			if ((PINA & 0x01)) { // !A0
				State = Wait;
			}
			else if (!(PINA & 0x01) && !(PINA & 0x02)) { // A0 && A1
				State = Reset;
			}
			else { // A0 && !A1
				State = Increment;
			}
			break;
		case Decrement:
			if ((PINA & 0x02)) { // !A1
				State = Wait;
			}
			else if (!(PINA & 0x01) && !(PINA & 0x02)) { // A0 && A1
				State = Reset;
			}
			else { // !A0 && A1
				State = Decrement;
			}
			break;
		case Reset:
			if (!(PINA & 0x01) && !(PINA & 0x02)) { // A0 && A1
				State = Reset;
			}
			else {
				State = Wait;
			}
 			break;
		default:
			State = SM_Start;
			break;
	}

	switch(State) { // action
		case Init:
			PORTB = 0x07;
			break;
		case Increment:
			if (i == 10) {
				i = 0;
				if (PORTB < 9) PORTB = PINB + 1;
			}
			i++;
			break;
		case Decrement:
			if (i == 10) {
				i = 0;
				if (PORTB > 0) PORTB = PINB - 1;
			}
			i++;
			break;
		case Reset: 
			PORTB = 0x00;
			break;
		default:
			break;
	}
}

int main(void) {
	/* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRB = 0xFF; PORTB = 0x00; // Configure port B's 8 pins as output initialized as 0x00

	TimerSet(100);
	TimerOn();

	State = SM_Start;
	while (1) {
		TickFct();
		while (!TimerFlag) {}
		TimerFlag = 0;
	}
	return 1;
}
