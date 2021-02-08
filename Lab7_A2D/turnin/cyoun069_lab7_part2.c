/*	Author: Christopher Young - cyoun069@ucr.edu
 *  	Partner(s) Name: 
 *	Lab Section: 22
 *	Assignment: Lab #7 Exercise #2
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


	// Highest value exposed to flashlight = 11 1000 0000 = 960
	// Lowest value when covered = 00 0000 1111 = 15

	ADC_init();
	while (1) {
		PORTB = (char) ADC;
		PORTD = (char) (ADC >> 8);
	}
	return 1;
}
