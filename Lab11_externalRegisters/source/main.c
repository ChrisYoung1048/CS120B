/*	Author: Christopher Young - cyoun069@ucr.edu
 *  	Partner(s) Name: 
 *	Lab Section: 22
 *	Assignment: Lab #11 Exercise #5
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

void transmit_data1(unsigned char data) {
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

void transmit_data2(unsigned char data) {
    int i;
    for (i = 0; i < 8 ; ++i) {
   	 // Sets SRCLR to 1 allowing data to be set
   	 // Also clears SRCLK in preparation of sending data
   	 PORTD = 0x08;
   	 // set SER = next bit of data to be sent.
   	 PORTD |= ((data >> i) & 0x01);
   	 // set SRCLK = 1. Rising edge shifts next bit of data into the shift register
   	 PORTD |= 0x02;  
    }
    // set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register
    PORTD |= 0x04;
    // clears all lines in preparation of a new transmission
    PORTD = 0x00;
}

typedef struct task {
	int state;                  
	unsigned long period;       
	unsigned long elapsedTime;  
	int (*TickFct)(int);       
} task;


unsigned long int findGCD(unsigned long int a, unsigned long int b) {
	unsigned long int c;
	while(1) {
		c = a%b;
		if (c == 0) { return b;}
		a = b;
		b = c;
	}
	return 0;
}

unsigned short playerPosition = 0x0001;
unsigned char enemyDirection = 0;
unsigned short enemyPosition = 0x8000;
unsigned short toDisplay = 0;

enum Control_States {Control_Init, Control_Wait, Control_PressR, Control_PressJ, Control_PressL, Control_Jumped};
int Tick_Control(int state) {
	char pressA0 = ~PINA & 0x01;
	char pressA1 = ~PINA & 0x02;
	char pressA2 = ~PINA & 0x04;
	switch(state) {
		case Control_Init: state = Control_Wait; break;
		case Control_Wait:
			if (pressA0) {
				state = Control_PressR;
				if (playerPosition <= 0x8000) 
					playerPosition = playerPosition << 1; 
			}
			else if (pressA2) {
				state = Control_PressL;
				if (playerPosition >= 0x0001)
					playerPosition = playerPosition >> 1;
			}
			else if (pressA1) {
				state = Control_PressJ;
			}
			else {
				state = Control_Wait;
			}
			break;
		case Control_PressR:
			if (!pressA0) state = Control_Wait;
			else state = Control_PressR;
			break;
		case Control_PressL:
			if (!pressA2) state = Control_Wait;
			else state = Control_PressL;
			break;
		case Control_PressJ:
			if (!pressA1) state = Control_Wait;
			else if (pressA0) {
				state = Control_Jumped;
				if (playerPosition <= 0x1000) 
					playerPosition = playerPosition << 3; 
			}
			else if (pressA2) {
				state = Control_Jumped;
				if (playerPosition >= 0x0008) 
					playerPosition = playerPosition >> 3; 
			}
			else state = Control_PressJ;
			break;
		case Control_Jumped:
			if (! (pressA0 || pressA1 || pressA2) ) state = Control_Wait;
			else state = Control_Jumped;
			break;
		default: state = Control_Init; 
	}

	switch(state) {
		case Control_Init: playerPosition = 0x0001;  break;
		default: break;
	}

	transmit_data1((char) playerPosition);
	transmit_data2((char) ( playerPosition >> 8));

	return state;
}

enum Enemy_States {Enemy_Init, Enemy_Move}; 
int Tick_Enemy(int state) {
	switch(state) {
		case Enemy_Init: state = Enemy_Move; break;
		case Enemy_Move: state = Enemy_Move; break;
		default: state = Enemy_Init;
	}
	
	switch(state) {
		case Enemy_Init: enemyPosition = 0x8000; break;
		case Enemy_Move:
			if (enemyPosition == 0x8000) enemyDirection = 0;
			if (enemyPosition == 0x0000) enemyDirection = 1;
			
			if (enemyDirection) enemyPosition = enemyPosition >> 1;
			else enemyPosition = enemyPosition << 1;
			break;
	}
	
	return state;
}

enum Display_States { Display_Display };
int Tick_Display(int state) {
	switch(state) {
		case Display_Display: state = Display_Display; break;
		default: state = Display_Display;
	}

	switch(state) {
		case Display_Display: 
			toDisplay = playerPosition | enemyPosition;
	}

	transmit_data1((char) toDisplay);
	transmit_data2((char) (toDisplay >> 8));

	return state;
}

int main(void) {
	/* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRB = 0xFF; PORTB = 0x00; // Configure port B's 8 pins as output initialized as 0x00
  	DDRC = 0xFF; PORTC = 0x00; // Configure port C's 8 pins as output initialized as 0x00
	DDRD = 0xFF; PORTD = 0x00;

	static task task1, task2, task3;
	task *tasks[] = { &task1 };
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	const char start = -1;

	// Task 1 (Control)
	task1.state = start;
	task1.period = 100;
	task1.elapsedTime = task1.period;
	task1.TickFct = &Tick_Control;


	// Task 2 (Enemy)
	task2.state = start;
	task2.period = 100;
	task2.elapsedTime = task2.period;
	task2.TickFct = &Tick_Enemy;


	// Task 3 (Display)
	task3.state = start;
	task3.period = 100;
	task3.elapsedTime = task3.period;
	task3.TickFct = &Tick_Display;

	unsigned short i;
	unsigned long GCD = tasks[0]->period;
	for (i = 0; i < numTasks; i++) {
		GCD = findGCD(GCD, tasks[i]->period);
	}

	const unsigned long timerPeriod = GCD;
	TimerSet(timerPeriod);
	TimerOn();

	while (1) {
		for (i = 0; i < numTasks; i++) { // task scheduler
			if (tasks[i]->elapsedTime == tasks[i]->period) { // if tick
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += timerPeriod;
		}
		while (!TimerFlag) {}
		TimerFlag = 0;
	}
	return 1;
}
