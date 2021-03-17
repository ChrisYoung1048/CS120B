/*	Author: Christopher Young - cyoun069@ucr.edu
 *  	Partner(s) Name: 
 *	Lab Section: 22
 *	Assignment: Lab #13 Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo: https://drive.google.com/file/d/1xx3vnYz_5DlnoX1t8OhZLkwRnBsrG1Mw/view?usp=sharing
 *
 */
#include <avr/io.h>
#include "timer.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

//unsigned short input = ADC;
void A2D_init() {
      ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	// ADEN: Enables analog-to-digital conversion
	// ADSC: Starts analog-to-digital conversion
	// ADATE: Enables auto-triggering, allowing for constant
	//	    analog to digital conversions.
}

//shared variables
unsigned char row = 0x01;
unsigned char col = 0x80;
unsigned char speed = 20;

enum Display_States {Display_init, Display_display} Display_State;
void Display_Tick();

enum Control_States {Control_init, Control_wait} Control_State;
void Control_Tick();

enum Speed_States {Speed_set} Speed_State;
void Speed_Tick();

int main(void) {
	/* Insert DDR and PORT initializations */
	//DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRB = 0xFF; PORTB = 0x00; // Configure port B's 8 pins as output initialized as 0x00
	DDRC = 0xFF; PORTC = 0x00; // Configure port C's 8 pins as output initialized as 0x00
  	DDRD = 0xFF; PORTD = 0x00; // Configure port D's 8 pins as output initialized as 0x00

	unsigned char timerPeriod = 50;
	TimerSet(timerPeriod);
	TimerOn();

	Display_State = Display_init;
	Control_State = Control_wait;
	Speed_State = Speed_set;

	A2D_init();

	while (1) {
		Display_Tick();
		Speed_Tick();
		Control_Tick();
		while (!TimerFlag) {}
		TimerFlag = 0;
	}
	return 1;
}

void Display_Tick() {
	//static unsigned char display_index = 0;
	switch(Display_State) {
		case Display_init: Display_State = Display_display; break;
		case Display_display: Display_State = Display_display; break;
		default: Display_State = Display_init;
	}

	switch(Display_State) {
		case Display_init:
			col = 0x80;
			row = 0x01;
			break;
		case Display_display:
			PORTC = col;
			PORTD = ~row;
			break;
		default:
			break;
	}

}

void Control_Tick() {
	static unsigned char i = 0;
	switch(Control_State) {
		case Control_wait:
			Control_State = Control_wait;
			break;
		default: Control_State = Control_wait;
	}

	switch(Control_State) {
		case Control_wait: 
			PORTB = 0x00;
			if (ADC > 525) {
				PORTB = 0x01; // debugging LED
				if (i >= speed) {
					if (col > 0x01) col = col >> 1;  
					else col = 0x80;
					i = 0;
				} 
				else i++;
			} else if (ADC < 425) {
				PORTB = 0x04; // debugging LED
				if (i >= speed) {
					if (col < 0x80) col = col << 1; 
					else col = 0x01;
					i = 0;
				}
				else i++;
			} else {
				PORTB = 0x02; // debugging LED
			}
			break;
		default: break;
	}

}

void Speed_Tick() {
	switch(Speed_State) {
		case Speed_set: Speed_State = Speed_set; break;
		default: Speed_State = Speed_set;
	}

	switch(Speed_State) {
		case Speed_set:
			if (ADC > 1000) speed = 2;
			else if (ADC > 800) speed = 5;
			else if (ADC > 600) speed = 10;
			else if (ADC > 525) speed = 20;	
			else if (ADC < 50) speed = 2;
			else if (ADC < 250) speed = 5;
			else if (ADC < 350) speed = 10;
			else if (ADC < 425) speed = 20;
			else speed = 0;
		default: break;
	}
}
