/*	Author: Christopher Young - cyoun069
 *	Partner(s) Name: 
 *	Lab Section: 022
 *	Assignment: Lab #2  Exercise #3
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
	DDRB = 0xFF; PORTB = 0x00; // Configure port B's 8 pins as outputs, initialize to 0s
	
	unsigned char cntavail = 0x00;
	//unsigned char tmpB = 0x00; // Temporary variable to hold the value of B
	//unsigned char tmpA = 0x00; // Temporary variable to hold the value of A
	
	while (1) {
		cntavail = 0x00;
		if (!(PINA & 0x01)) cntavail++;
		if (!(PINA & 0x02)) cntavail++;
		if (!(PINA & 0x04)) cntavail++;
		if (!(PINA & 0x08)) cntavail++;
		cntavail = cntavail & 0x0F;
		if ((PINA & 0x0F) == 0x0F) cntavail = (cntavail | 0x80);
		PORTC = cntavail;
	}
	return 0;
}

