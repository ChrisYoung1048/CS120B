/*	Author: Christopher Young - cyoun069@ucr.edu
 *  	Partner(s) Name: 
 *	Lab Section: 22
 *	Assignment: Lab #9 Exercise #1
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

enum TL_States {TL_Init, TL_1, TL_2, TL_3} TL_State;
unsigned char threeLEDs;
void TickFct_ThreeLeds() {
	switch(TL_State) { // transition
		case TL_Init:
			TL_State = TL_1;
			break;
		case TL_1:
			TL_State = TL_2;
			break;
		case TL_2:
			TL_State = TL_3;
			break;
		case TL_3:
			TL_State = TL_1;
			break;
		default:
			TL_State = TL_Init;
			break;
	}

	switch(TL_State) { // action
		case TL_1:
			threeLEDs = 0x01;
			break;
		case TL_2:
			threeLEDs = 0x02;
			break;
		case TL_3:
			threeLEDs = 0x04;
			break;
		default:
			break;
	}
}

enum BL_States {BL_Init, BL_1, BL_2} BL_State;
unsigned char blinkingLED;
void TickFct_BlinkLed() {
	switch(BL_State) { // transition
		case BL_Init:
			BL_State = BL_1;
			break;
		case BL_1:
			BL_State = BL_2;
			break;
		case BL_2:
			BL_State = BL_1;
			break;
		default:
			break;
	}
	
	switch(BL_State) { // action
		case BL_1:
			blinkingLED = 0x01;
			break;
		case BL_2:
			blinkingLED = 0x08;
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

	PWM_on();	

	while (1) {
		TickFct();
		while (!TimerFlag) {}
		TimerFlag = 0;
	}
	return 1;
}
