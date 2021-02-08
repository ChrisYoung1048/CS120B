/*	Author: Christopher Young - cyoun069@ucr.edu
 *  	Partner(s) Name: 
 *	Lab Section: 22
 *	Assignment: Lab #7 Exercise #4
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://drive.google.com/file/d/1V-ghJDVcOQ4xKWHzuhDDew_KJ933xhRl/view?usp=sharing
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
	unsigned short threshold = max / 8;

	ADC_init();
	while (1) {
		if (ADC >= threshold * 8) { PORTB = 0xFF; }
		else if (ADC >= threshold * 7) { PORTB = 0x7F; }
		else if (ADC >= threshold * 6) { PORTB = 0x3F; }
		else if (ADC >= threshold * 5) { PORTB = 0x1F; }
		else if (ADC >= threshold * 4) { PORTB = 0x0F; }
		else if (ADC >= threshold * 3) { PORTB = 0x07; }
		else if (ADC >= threshold * 2) { PORTB = 0x03; }
		else if (ADC >= threshold * 1) { PORTB = 0x01; }
		else { PORTB = 0x00; }
	}
	return 1;
}
