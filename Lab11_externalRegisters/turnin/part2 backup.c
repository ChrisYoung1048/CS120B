/*	Author: Christopher Young - cyoun069@ucr.edu
 *  	Partner(s) Name: 
 *	Lab Section: 22
 *	Assignment: Lab #11 Exercise #2
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

unsigned char go = 0x00;

enum lights1_states { lights1_init, lights1_display } lights1_state;
unsigned char lights1 = 0x00;
void TickFct_lights1() {
	switch(lights1_state) {
		case lights1_init: lights1_state = lights1_display; break;
		case lights1_display: lights1_state = lights1_display; break;
		default: lights1_state = lights1_init;
	}
	

	switch(lights1_state) {
		case lights1_init: lights1 = 0x55; break;
		case lights1_display: 
			lights1 = ~lights1; 
			if (go == 1) transmit_data(lights1); 
			break;
	}

}

enum lights2_states { lights2_init, lights2_display } lights2_state;
unsigned char lights2 = 0x00;
void TickFct_lights2() {
	unsigned char direction = 0;
	switch(lights2_state) {
		case lights2_init: lights2_state = lights2_display; break;
		case lights2_display: lights2_state = lights2_display; break;
		default: lights2_state = lights2_init;
	}
	
	switch(lights2_state) {
		case lights2_init: lights2 = 0x00; break;
		case lights2_display:
			if (lights2 == 0xFF) direction = 0;
			else if (lights2 == 0x00) direction = 1;
			else if (direction) lights2++;
			else lights2--;
			if (go == 2) transmit_data(lights2); 
			break;
	}
}

enum lights3_states { lights3_init, lights3_display } lights3_state;
unsigned char lights3 = 0x00;
void TickFct_lights3() {
	switch(lights3_state) {
		case lights3_init: lights3_state = lights3_display; break;
		case lights3_display: lights3_state = lights3_display; break;
		default: lights3_state = lights3_init;
	}
	

	switch(lights3_state) {
			case lights3_init: lights3 = 0xFF; break;
			case lights3_display: 
				lights3 = ~lights3;
				if (go == 3) transmit_data(lights3); 
				 break;
	}
}

enum lightControl_states {lightControl_init, lightControl_wait, lightControl_press1, lightControl_press2} lightControl_state;
void TickFct_lightControl() {
	char pressA0 = ~PINA & 0x01;
	char pressA1 = ~PINA & 0x02;
	switch(lightControl_state) {
		case lightControl_init: lightControl_state = lightControl_wait; break;
		case lightControl_wait: 
			if (pressA0) {
				lightControl_state = lightControl_press1;
				if (go > 0) go--;
			}
			else if (pressA1) {
				lightControl_state = lightControl_press2;
				if (go < 3) go++;
			}
			else lightControl_state = lightControl_wait;
			break;
		case lightControl_press1:
			if (!pressA0) lightControl_state = lightControl_wait;
			else if (pressA1) {
				lightControl_state = lightControl_wait; 
				if (go == 0) go = 1;
				else go = 0;
			}
			else lightControl_state = lightControl_press1;
			break;
		case lightControl_press2:
			if (!pressA1) lightControl_state = lightControl_wait;
			else if (pressA0) {
				lightControl_state = lightControl_wait; 
				if (go == 0) go = 1;
				else go = 0;
			}
			else lightControl_state = lightControl_press2;
			break;
		default: lightControl_state = lightControl_init;
	}
	
	switch(lightControl_state) {
		case lightControl_init: go = 1; break;
		default: break;
	}
}



int main(void) {
	/* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRB = 0xFF; PORTB = 0x00; // Configure port B's 8 pins as output initialized as 0x00
  	DDRC = 0xFF; PORTC = 0x00; // Configure port C's 8 pins as output initialized as 0x00

	const unsigned long timerPeriod = 500;
	TimerSet(timerPeriod);
	TimerOn();

	lights1_state = lights1_init;
	lights2_state = lights2_init;
	lights3_state = lights3_init;
	lightControl_state = lightControl_init;

	while (1) {
		TickFct_lightControl();
		TickFct_lights1();
		TickFct_lights2(); 
		TickFct_lights3();
		while (!TimerFlag) {}
		TimerFlag = 0;
	}
	return 1;
}
