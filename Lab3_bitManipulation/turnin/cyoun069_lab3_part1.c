/*	Author: Christopher Young - cyoun069@ucr.edu
 *  	Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #3  Exercise #1
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
	DDRB = 0x00; PORTB = 0xFF; // Configure port B's 8 pins as inputs
	DDRC = 0xFF; PORTC = 0x00; // Configure port C's 8 pins as outputs, initialize to 0s

	/* Insert your solution below */
	unsigned char tmpA, tmpB, count, i;
	while (1) {
		tmpA = PINA;
		tmpB = PINB;
		count = 0x00;
		for (i = 0; i < 8; ++i) {
			if ((tmpA & 0x01) == 1) count++;
			if ((tmpB & 0x01) == 1) count++;
			tmpA = tmpA >> 1;
			tmpB = tmpB >> 1;
		}
		PORTC = count;
	}
	return 1;
}
