/*	Author: Christopher Young - cyoun069@ucr.edu
 *  	Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #3  Exercise #5
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

int main(void) {
	/* Insert DDR and PORT initializations */
	DDRD = 0x00; PORTD = 0xFF; // Configure port D's 8 pins as inputs
	DDRB = 0x00; PORTB = 0x01; // Configure port B's 1st pin as inputs

	/* Insert your solution below */
	unsigned char tmpB;
	unsigned int weight;
	while (1) {
		tmpB = 0x00;

		weight = 0x00;
		weight = weight + PIND; // add the d input to total weight
		weight = weight << 1; // shift the input left by 1
		weight = weight | (PORTB & 0x01); // add the b0 input to the lsb

		if (weight >= 70) tmpB = tmpB | 0x02;
		else if (weight > 5) tmpB = tmpB | 0x04;
		
		PORTB = tmpB;
	}
	return 1;
}
