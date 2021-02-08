/*	Author: Christopher Young - cyoun069@ucr.edu
 *  	Partner(s) Name: 
 *	Lab Section: 22
 *	Assignment: Lab #7 Exercise #3
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
//#include "timer.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
}

int main(void) {
	/* Insert DDR and PORT initializations */
	DDRB = 0xFF; PORTB = 0x00; // Configure port A's 8 pins as inputs
	DDRD = 0xFF; PORTD = 0x00; // Configure port B's 8 pins as output initialized as 0x00

	unsigned short max = 960;

	ADC_init();
	while (1) {
		if (ADC >= max/2) {
			PORTB = 0xFF;
			PORTD = 0xFF;
		}
		else {
			PORTB = 0x00;
			PORTD = 0x00;
		}
	}
	return 1;
}
