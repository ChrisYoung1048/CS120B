/*	Author: Christopher Young - cyoun069@ucr.edu
 *  	Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #3  Exercise #2
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
	DDRC = 0xFF; PORTC = 0x00; // Configure port C's 8 pins as outputs, initialize to 0s

	/* Insert your solution below */
	unsigned char tmpC, fuelLevel;
	while (1) {
		fuelLevel = (PINA & 0x0F); // fuel level is indicated from PA0 to PA3
		tmpC = 0x3F; // initially set to full indicator ( 0011 1111 )
		
		if (fuelLevel <= 0) tmpC = 0x00; //(0000 0000)
		else if (fuelLevel <= 2) tmpC = 0x20; //(0010 0000)
		else if (fuelLevel <= 4) tmpC = 0x30; //(0011 0000)
		else if (fuelLevel <= 6) tmpC = 0x38; //(0011 1000)
		else if (fuelLevel <= 9) tmpC = 0x3C; //(0011 1100)
		else if (fuelLevel <= 12) tmpC = 0x3E; //(0011 1110)
		else if (fuelLevel <= 15) tmpC = 0x3F; //(0011 1111)

		if (fuelLevel <= 4) tmpC = (tmpC | 0x40);
		
		PORTC = tmpC;
	}
	return 1;
}
