/*	Author: Christopher Young - cyoun069@ucr.edu
 *  	Partner(s) Name: 
 *	Lab Section: 22
 *	Assignment: Lab #9 Exercise #4
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo: https://drive.google.com/file/d/1_lzHjy6bPkt-dBzG3duVaeEXs2lDVHvA/view?usp=sharing
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
			blinkingLED = 0x00;
			break;
		case BL_2:
			blinkingLED = 0x08;
			break;
		default:
			break;
	}
}

enum S_States {S_Init, S_Wait, S_Play1, S_Play2} S_State;
unsigned char speakerOutput;
void TickFct_Speaker() {
	switch(S_State) {
		case S_Init:
			S_State = S_Wait;
			break;
		case S_Wait:
			if (!(PINA & 0x04)) { // A0
				S_State = S_Play1;
			}
			else {
				S_State = S_Wait;
			}
			break;
		case S_Play1:
			if (PINA & 0x04) { // !A0
				S_State = S_Wait;
			}
			else {
				S_State = S_Play2;
			}
			break;
		case S_Play2:
			if (PINA & 0x04) { // !A0
				S_State = S_Wait;
			}
			else {
				S_State = S_Play1;
			}
			break;
		default:
			break;
	}

	switch(S_State) {
		case S_Play1:
			speakerOutput = 0x10;
			break;
		case S_Play2:
			speakerOutput = 0x00;
			break;
		default:
			break;
	}
}

enum FC_States {FC_Init, FC_Wait, FC_Up, FC_Down} FC_State;
static double frequency;
void TickFct_FrequencyControl() {
	switch(FC_State) {
		case FC_Init:
			FC_State = FC_Wait;
			break;
		case FC_Wait:
			if (!(PINA & 0x01) && (PINA & 0x02) && (PINA & 0x04)) { // only A0
				FC_State = FC_Up;
				if (frequency > 0) { --frequency; }
			}
			else if ((PINA & 0x01) && !(PINA & 0x02) && (PINA & 0x04)) { // only A1
				FC_State = FC_Down;
				++frequency;
			}
			else {
				FC_State = FC_Wait;
			}
			break;
		case FC_Up:
			if (PINA & 0x01) { // !A0
				FC_State = FC_Wait;
			}
			else {
				FC_State = FC_Up;
			}	
			break;
		case FC_Down:
			if (PINA & 0x02) { // !A1
				FC_State = FC_Wait;
			}
			else {
				FC_State = FC_Down;
			}	
			break;
		default:
			break;
	}

	switch(FC_State) {
		case FC_Init:
			frequency = 3;
			break;
		default:
			break;
	}
}

enum CL_States {CL_Init, CL_Combine} CL_State;
void TickFct_CombineLed() {
	switch(CL_State) {
		case CL_Init:
			CL_State = CL_Combine;
		case CL_Combine:
			CL_State = CL_Combine;
		default:
			break;	
	}

	switch (CL_State) {
		case CL_Combine:
			PORTB = threeLEDs | blinkingLED | speakerOutput;
		default:
			break;
	}
}

int main(void) {
	/* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRB = 0xFF; PORTB = 0x00; // Configure port B's 8 pins as output initialized as 0x00
  	
	unsigned long TL_elapsedTime = 300;
	unsigned long BL_elapsedTime = 1000;
	unsigned long S_elapsedTime = frequency;
	unsigned long FC_elapsedTime = 1;
	unsigned long CL_elapsedTime = 300;
	const unsigned long timerPeriod = 1;
	
	TimerSet(timerPeriod);
	TimerOn();
	
	TL_State = TL_Init;
	BL_State = BL_Init;
	S_State = S_Init;
	FC_State = FC_Init;
	CL_State = CL_Init;

	while (1) {
		if (TL_elapsedTime >= 300) {
			TickFct_ThreeLeds();
			TL_elapsedTime = 0;
		}
		if (BL_elapsedTime >= 1000) {
			TickFct_BlinkLed();
			BL_elapsedTime = 0;
		}
		if (S_elapsedTime >= frequency) {
			TickFct_Speaker();
			S_elapsedTime = 0;
		}
		if (FC_elapsedTime >= 1) {
			TickFct_FrequencyControl();
			FC_elapsedTime = 0;
		}
		if (CL_elapsedTime >= 1) {
			TickFct_CombineLed();
			CL_elapsedTime = 0;
		}
		while (!TimerFlag) {}
		TimerFlag = 0;

		TL_elapsedTime += timerPeriod;
		BL_elapsedTime += timerPeriod;
		S_elapsedTime += timerPeriod;
		FC_elapsedTime += timerPeriod;
		CL_elapsedTime += timerPeriod;
	}
	return 1;
}
