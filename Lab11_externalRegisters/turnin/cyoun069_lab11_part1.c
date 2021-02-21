/*	Author: Christopher Young - cyoun069@ucr.edu
 *  	Partner(s) Name: 
 *	Lab Section: 22
 *	Assignment: Lab #11 Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include "timer.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

void transmit_data(unsigned char data) {
    int i;
    for (i = 0; i < 8 ; ++i) {
   	 // Sets SRCLR to 1 allowing data to be set
   	 // Also clears SRCLK in preparation of sending data
   	 PORTC = 0x08;
   	 // set SER = next bit of data to be sent.
   	 PORTC |= ((data >> i) & 0x01);
   	 // set SRCLK = 1. Rising edge shifts next bit of data into the shift register
   	 PORTC |= 0x02;  
    }
    // set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register
    PORTC |= 0x04;
    // clears all lines in preparation of a new transmission
    PORTC = 0x00;
}

enum States {wait, press1, press2} state;
unsigned char x = 0;
void Tick() {
	char pressA0 = ~PINA & 0x01;
	char pressA1 = ~PINA & 0x02;
	switch(state) {
		case wait: 
			if (pressA0) {
				state = press1;
				if (x > 0x00) x--;
			}
			else if (pressA1) {
				state = press2;
				if (x < 0xFF) x++;
			}
			else state = wait;
			break;
		case press1:
			if (!pressA0) state = wait;
			else if (pressA1) {state = wait; x = 0;}
			else state = press1;
			break;
		case press2:
			if (!pressA1) state = wait;
			else if (pressA0) {state = wait; x = 0;}
			else state = press2;
			break;
		default: state = wait;
	}
	
	switch(state) {
		default: transmit_data(x);
	}
}

int main(void) {
	/* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRB = 0xFF; PORTB = 0x00; // Configure port B's 8 pins as output initialized as 0x00
  	DDRC = 0xFF; PORTC = 0x00; // Configure port C's 8 pins as output initialized as 0x00

	//const unsigned long timerPeriod = 100;
	//TimerSet(timerPeriod);
	//TimerOn();

	while (1) {
		Tick();
		//transmit_data(0x88);
		//while (!TimerFlag) {}
		//TimerFlag = 0;
	}
	return 1;
}
