/*	Author: Christopher Young - cyoun069@ucr.edu
 *  	Partner(s) Name: 
 *	Lab Section: 22
 *	Assignment: Lab #8 Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo: https://drive.google.com/file/d/13liXcEZtH_GWKboyd6eDvYg7xLJfPieR/view?usp=sharing
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

enum States {Init, Wait, Toggle, ScaleDown, ScaleUp} State;
double notes[] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25};
unsigned char i, on;

void TickFct() {
	switch(State) { // transition
		case Init:
			State = Wait;
			break;
		case Wait:
			if (!(PINA & 0x01) && (PINA & 0x02) && (PINA & 0x04)) { // only A0
				State = ScaleDown;
				if (i > 0) {
					--i;
				}
				set_PWM(notes[i]);
			}
			else if ((PINA & 0x01) && !(PINA & 0x02) && (PINA & 0x04)) { // only A1
				State = Toggle;
				on = !on;
				if (on) { set_PWM(notes[i]); }
				else { set_PWM(0); }
			}
			else if ((PINA & 0x01) && (PINA & 0x02) && !(PINA & 0x04)) { // only A2
				State = ScaleUp;
				if (i < 7) {
					++i;
				}
				set_PWM(notes[i]);
			}
			else {
				State = Wait;
			}
			break;
		case Toggle:
			if (PINA & 0x02) { //!A1
				State = Wait;
			}
			else {
				State = Toggle;
			}
			break;
		case ScaleDown:
			if (PINA & 0x01) { //!A0
				State = Wait;
			}
			else {
				State = ScaleDown;
			}
			break;
		case ScaleUp:
			if (PINA & 0x04) { //!A2
				State = Wait;
			}
			else {
				State = ScaleUp;
			}
			break;
		default:
			State = Init;
			break;
	}

	switch(State) { // action
		case Init:
			i = 0;
			on = 0;
			PORTB = 0x40;
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
