/*	Author: Christopher Young - cyoun069
 *	Partner(s) Name: 
 *	Lab Section: 022
 *	Assignment: Lab #2  Exercise #4
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
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRB = 0x00; PORTB = 0xFF; // Configure port B's 8 pins as inputs
	DDRC = 0x00; PORTC = 0xFF; // Configure port C's 8 pins as inputs
	DDRD = 0xFF; PORTD = 0x00; // Configure port D's 8 pins as outputs, initialize to 0s
	
	unsigned char tmpA, tmpB, tmpC, tmpD; // Temporary variables
	unsigned short totalWeight = 0x00; // can be used to hold total weight	
	unsigned char weightDifference = 0x00;
		
	while (1) {
		tmpA = PINA;
		tmpB = PINB;
		tmpC = PINC;
		tmpD = 0x00;
		
		// total weight can reach 255 x 3 = 765 (10bits needed)
		totalWeight = tmpA + tmpB + tmpC;
		// check for overweight
		if (totalWeight > 140) tmpD = (tmpD | 0x01);
		
		// check for weight imbalance
		weightDifference = tmpA - tmpC; 
		if (weightDifference < 0) weightDifference = -weightDifference;
		if (weightDifference > 80) tmpD = tmpD | 0x02;

		
		totalWeight = totalWeight >> 2; //shifted right by 2 to represent msb
		
		PORTD = tmpD | (totalWeight & 0xFC);
	}
	return 0;
}

