/*	Author: Christopher Young - cyoun069@ucr.edu
 *  	Partner(s) Name: 
 *	Lab Section: 22
 *	Assignment: Lab #11 Exercise #3
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo: https://drive.google.com/file/d/12g5SPiBeUl2zOtydrgOpO9r35s7c6UCD/view?usp=sharing
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

void transmit_data22(unsigned char data) {
    int i;
    for (i = 0; i < 8 ; ++i) {
   	 // Sets SRCLR to 1 allowing data to be set
   	 // Also clears SRCLK in preparation of sending data
   	 PORTC = 0x80;
   	 // set SER = next bit of data to be sent.
   	 PORTC |= ((data >> i) & 0x10);
   	 // set SRCLK = 1. Rising edge shifts next bit of data into the shift register
   	 PORTC |= 0x20;  
    }
    // set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register
    PORTC |= 0x40;
    // clears all lines in preparation of a new transmission
    PORTC = 0x00;
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

unsigned char go1 = 0;
unsigned char go2 = 0;

enum L1_States { L1_S1, L1_S2 };
int Tick_Light1(int state) {
	switch(state) {
		case L1_S1: state = L1_S2; break;
		case L1_S2: state = L1_S1; break;
		default: state = L1_S1;
	}

	switch(state) {
		case L1_S1: 
			if(go1 == 1) transmit_data1(0x55);
			if(go2 == 1) transmit_data2(0x55);
			break;
		case L1_S2:
			if(go1 == 1) transmit_data1(0xAA);
			if(go2 == 1) transmit_data2(0xAA);
			break;
	}

	return state;
}

enum L2_States { L2_S1, L2_S2 };
int Tick_Light2(int state) {
	switch(state) {
		case L2_S1: state = L2_S2; break;
		case L2_S2: state = L2_S1; break;
		default: state = L2_S1;
	}

	switch(state) {
		case L2_S1: 
			if(go1 == 2) transmit_data1(0xFF);
			if(go2 == 2) transmit_data2(0xFF);
			break;
		case L2_S2:
			if(go1 == 2) transmit_data1(0x00); 
			if(go2 == 2) transmit_data2(0x00);
			break;
	}

	return state;
}

enum L3_States { L3_S1, L3_S2, L3_S3, L3_S4 };
int Tick_Light3(int state) {
	switch(state) {
		case L3_S1: state = L3_S2; break;
		case L3_S2: state = L3_S3; break;
		case L3_S3: state = L3_S4; break;
		case L3_S4: state = L3_S1; break;
		default: state = L3_S1;
	}

	switch(state) {
		case L3_S1: 
			if(go1 == 3) transmit_data1(0x03);
			if(go2 == 3) transmit_data2(0x03);
			break;
		case L3_S2:
			if(go1 == 3) transmit_data1(0x0C);
			if(go2 == 3) transmit_data2(0x0C);
			break;
		case L3_S3:
			if(go1 == 3) transmit_data1(0x30);
			if(go2 == 3) transmit_data2(0x30);
			break;
		case L3_S4:
			if(go1 == 3) transmit_data1(0xC0);
			if(go2 == 3) transmit_data2(0xC0);
			break;
	}

	return state;
}

enum Control_States {Control_Init, Control_Wait, Control_Press1, Control_Press2, Control_Press3, Control_Press4};
int Tick_Control(int state) {
	char pressA0 = ~PINA & 0x01;
	char pressA1 = ~PINA & 0x02;
	char pressA2 = ~PINA & 0x04;
	char pressA3 = ~PINA & 0x08;
	switch(state) {
		case Control_Init: state = Control_Wait; break;
		case Control_Wait:
			if (pressA0) {
				state = Control_Press1;
				if (go2 > 1 && go2 != 0) go2--;
			}
			else if (pressA1) {
				state = Control_Press2;
				if (go2 < 3 && go2 != 0) go2++;
			}
			else if (pressA2) {
				state = Control_Press3;
				if (go1 > 1 && go1 != 0) go1--;
			}
			else if (pressA3) {
				state = Control_Press4;
				if (go1 < 3 && go1 != 0) go1++;
			}
			else state = Control_Wait;
			break;
		case Control_Press1:
			if (!pressA0) state = Control_Wait;
			else if (pressA1) {
				state = Control_Wait; 
				if (go2 == 0) go2 = 1;
				else { go2 = 0; transmit_data2(0x00); }
			}
			else state = Control_Press1;
			break;
		case Control_Press2:
			if (!pressA1) state = Control_Wait;
			else if (pressA0) {
				state = Control_Wait; 
				if (go2 == 0) go2 = 1;
				else { go2 = 0; transmit_data2(0x00); }
			}
			else state = Control_Press2;
			break;
		case Control_Press3:
			if (!pressA2) state = Control_Wait;
			else if (pressA3) {
				state = Control_Wait; 
				if (go1 == 0) go1 = 1;
				else { go1 = 0; transmit_data1(0x00); }
			}
			else state = Control_Press3;
			break;
		case Control_Press4:
			if (!pressA3) state = Control_Wait;
			else if (pressA2) {
				state = Control_Wait; 
				if (go1 == 0) go1 = 1;
				else { go1 = 0; transmit_data1(0x00); }
			}
			else state = Control_Press4;
			break;
		default: state = Control_Init;
	}

	switch(state) {
		case Control_Init: go1 = 1; go2 = 1; break;
		default: break;
	}

	return state;
}

int main(void) {
	/* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRB = 0xFF; PORTB = 0x00; // Configure port B's 8 pins as output initialized as 0x00
  	DDRC = 0xFF; PORTC = 0x00; // Configure port C's 8 pins as output initialized as 0x00
	DDRD = 0xFF; PORTD = 0x00;

	static task task1, task2, task3, task4;
	task *tasks[] = { &task1, &task2, &task3, &task4 };
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	const char start = -1;

	// Task 1 (Control)
	task1.state = start;
	task1.period = 200;
	task1.elapsedTime = task1.period;
	task1.TickFct = &Tick_Control;

	// Task 2 (L1)
	task2.state = start;
	task2.period = 200;
	task2.elapsedTime = task2.period;
	task2.TickFct = &Tick_Light1;

	// Task 3 (L2)
	task3.state = start;
	task3.period = 200;
	task3.elapsedTime = task3.period;
	task3.TickFct = &Tick_Light2;

	// Task 4 (L3)
	task4.state = start;
	task4.period = 200;
	task4.elapsedTime = task4.period;
	task4.TickFct = &Tick_Light3;

	unsigned short i;
	unsigned long GCD = tasks[0]->period;
	for (i = 0; i < numTasks; i++) {
		GCD = findGCD(GCD, tasks[i]->period);
	}

	const unsigned long timerPeriod = GCD;
	TimerSet(timerPeriod);
	TimerOn();

	while (1) {
		PORTC = 0x80;
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
