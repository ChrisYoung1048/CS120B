/*	Author: Christopher Young - cyoun069@ucr.edu
 *  	Partner(s) Name: 
 *	Lab Section: 22
 *	Assignment: Lab #8 Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo: https://drive.google.com/file/d/1Nt91-DJR4I_05GgNSVX4rPIVoJxV9nnV/view?usp=sharing
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

enum States {Init, Wait, C4, D4, E4} State;

void TickFct() {
	switch(State) { // transition
		case Init:
			State = Wait;
			break;
		case Wait:
			if (!(PINA & 0x01) && (PINA & 0x02) && (PINA & 0x04)) { // only A0
				State = C4;
			}
			else if ((PINA & 0x01) && !(PINA & 0x02) && (PINA & 0x04)) { // only A1
				State = D4;
			}
			else if ((PINA & 0x01) && (PINA & 0x02) && !(PINA & 0x04)) { // only A2
				State = E4;
			}
			else {
				State = Wait;
			}
			break;
		case C4:
			if ((PINA & 0x01)) { // !A0
				State = Wait;
			}
			else {
				State = C4;
			}
			break;
		case D4:
			if ((PINA & 0x02)) { // !A1
				State = Wait;
			}
			else {
				State = D4;
			}
			break;
		case E4:
			if ((PINA & 0x04)) { // !A2
				State = Wait;
			}
			else {
				State = E4;
			}
			break;
		default:
			State = Init;
			break;
	}

	switch(State) { // action
		case Wait:
			set_PWM(0);
			break;
		case C4:
			set_PWM(261.63);
			break;
		case D4:
			set_PWM(293.66);
			break;
		case E4:
			set_PWM(329.63);
			break;
		default:
			break;
	}
}

int main(void) {
	/* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRB = 0xFF; PORTB = 0x40; // Configure port B's 8 pins as output initialized as 0x00
  
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
