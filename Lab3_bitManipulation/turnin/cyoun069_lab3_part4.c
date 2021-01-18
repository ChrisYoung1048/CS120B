/*	Author: Christopher Young - cyoun069@ucr.edu
 *  	Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #3  Exercise #4
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
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRC = 0xFF; PORTC = 0x00; // Configure port B's 8 pins as outputs, initialize to 0s
	DDRC = 0xFF; PORTC = 0x00; // Configure port C's 8 pins as outputs, initialize to 0s

	/* Insert your solution below */
	unsigned char tmpB, tmpC;
	while (1) {
		tmpB = 0x00;
		tmpC = 0x00;

		tmpB = (tmpB & 0xF0) | (PINA >> 4);
		tmpC = (tmpC & 0x0F) | ((PINA & 0x0F) << 4);

		PORTB = tmpB;
		PORTC = tmpC;
	}
	return 1;
}
