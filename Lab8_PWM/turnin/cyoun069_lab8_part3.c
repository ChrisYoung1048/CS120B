/*	Author: Christopher Young - cyoun069@ucr.edu
 *  	Partner(s) Name: 
 *	Lab Section: 22
 *	Assignment: Lab #8 Exercise #3
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

void set_PWM(double frequency) {
	static double current_frequency;
	if (frequency != current_frequency) {
		if (!frequency) { TCCR3B &= 0x08; }
		else { TCCR3B |= 0x03; }

		if (frequency < 0.954) { OCR3A = 0xFFFF; }
		else if (frequency > 31250) { OCR3A = 0x0000; }
		else { OCR3A = (short) (8000000 / (128 * frequency)) - 1;}
		
		TCNT3 = 0;
		current_frequency = frequency;
	}
}

void PWM_on() {
	TCCR3A = (1 << COM3A0);
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	set_PWM(0);
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

enum States {Init, Wait, Play} State;
double noteSequence[] = {293.66, 587.33, 440.00, 415.30, 392.00, 349.23, 293.66, 349.23, 392.00};
double noteTimeSequence[] = {2, 2, 2, 2, 2, 2, 1, 1, 1};
double downTimeSequence[] = {1, 1, 2, 1, 1, 1, 0, 0, 0};
unsigned char i, j, k;

void TickFct() {
	switch(State) { // transition
		case Init:
			State = Wait;
			break;
		case Wait:
			if (!(PINA & 0x01)) { // only A0
				State = Play;
			}
			else {
				State = Wait;
			}
			break;
		case Play:
			if (i <= 9) {
				State = Play;
			}
			else {
				State = Wait;
				set_PWM(0);
				i = 0;
				j = 0;
			}
			break;
		default:
			State = Init;
			break;
	}

	switch(State) { // action
		case Init:
			i = 0;
			j = 0;
			k = 0;
			PORTB = 0x40;
			break;
		case Play:
			if (j < noteTimeSequence[i]) {
				set_PWM(noteSequence[i]);
				++j;
			}
			else if (k < downTimeSequence[i]) {
				set_PWM(0);
				++k;
			}
			else {
				++i;
				j = 0;
				k = 0;
			}
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
