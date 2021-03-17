/*	Author: Christopher Young - cyoun069@ucr.edu
 *  	Partner(s) Name: 
 *	Lab Section: 22
 *	Assignment: Lab #14
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 */
#include <avr/io.h>
#include "timer.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

void A2D_init();
void Set_A2D_Pin(unsigned char pinNum);
void transmit_data(unsigned char data);
unsigned long int findGCD(unsigned long int a, unsigned long int b);
void set_array(unsigned char cur[], unsigned char new[]);

//shared variables
unsigned char ingame = 0;

unsigned char menu_position = 0;

unsigned char ball_position[2] = {0,0};
unsigned char ball_direction[2] = {0,0};

unsigned char row[3] = {0, 0, 0};
unsigned char col[3] = {0, 0, 0};

//Sync SMs
enum Game_States {Game_init, Game_menu, Game_single, Game_multi};
int Game_Tick(int state);

enum Control_States {Control_init, Control_horizontal, Control_vertical} Control_State;
int Control_Tick(int state);

enum Display_States {Display_init, Display_display} Display_State;
int Display_Tick(int state);

typedef struct _task {
	signed char state;
	unsigned long int period;
	unsigned long int elapsedTime;
	int (*TickFct) (int);
} task;

int main(void) {
	/* Insert DDR and PORT initializations */
	//DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRB = 0xFF; PORTB = 0x00; // Configure port B's 8 pins as output initialized as 0x00
	DDRC = 0xFF; PORTC = 0x00; // Configure port C's 8 pins as output initialized as 0x00
  	DDRD = 0xFF; PORTD = 0x00; // Configure port D's 8 pins as output initialized as 0x00
	
	A2D_init();

	static task task1, task2, task3;
	task *tasks[] = { &task1, &task2, &task3 };
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	const char start = -1;

	// Task 1 (game)
	task1.state = start;
	task1.period = 50;
	task1.elapsedTime = task1.period;
	task1.TickFct = &Game_Tick;

	// Task 2 (control)
	task2.state = start;
	task2.period = 100;
	task2.elapsedTime = task2.period;
	task2.TickFct = &Control_Tick;

	// Task 3 (display)
	task3.state = start;
	task3.period = 1;
	task3.elapsedTime = task3.period;
	task3.TickFct = &Display_Tick;	

	unsigned short i; // iterator
	unsigned long GCD = tasks[0]->period;
	for (i = 0; i < numTasks; i++) {
		GCD = findGCD(GCD, tasks[i]->period);
	}
	
	unsigned char timerPeriod = GCD;
	TimerSet(timerPeriod);
	TimerOn();

	while (1) {
		transmit_data(0x0F);
			PORTD = ~0x02;
		while (!TimerFlag) {}
		TimerFlag = 0;
		
	}
	return 1;
}

int Game_Tick(int state) {
	unsigned char A2 = ~PINA & 0x04; // left button
	unsigned char A3 = ~PINA & 0x08; // right button
	
	switch(state) {
		case Game_init: state = Game_menu; Set_A2D_Pin(0x00); break;
		case Game_menu: 
			if (A2 || A3) { 
				if (menu_position) state = Game_multi;
				else state = Game_single;			
			}
			else state = Game_menu;
			break;
		default: state = Game_init;
	}

	switch(state) {
		case Game_init: break;
		case Game_menu:
			// select gamemode
			if (ADC > 525) menu_position = 0;
			else if (ADC < 425) menu_position = 1;
			
			// set display
			unsigned char new_row[] = {0x02, 0x04, 0};
			unsigned char new_col[] = {0x08, 0x0A, 0};
			
			if (menu_position) col[0] = col[0] | 0x60;
			else col[1] = col[1] | 0x60;

			//set_array(row, new_row);
			//set_array(col, new_col);	
			break;

		default: break;
	}

	return state;
}

int Control_Tick(int state) {
	
	return state;
}

int Display_Tick(int state) {
	static unsigned char i = 0;
	switch(state) {
		case Display_init: state = Display_display;
		case Display_display: state = Display_display;
		default: state = Display_init;
	}

	switch(state) {
		case Display_init: break;
		case Display_display:
			if (i >= 3) i = 0;
			transmit_data(0x0F);
			PORTD = ~0x02;
			i++;
			break;
		default: break;
	}	

	return state;
}

void A2D_init() {
      ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	// ADEN: Enables analog-to-digital conversion
	// ADSC: Starts analog-to-digital conversion
	// ADATE: Enables auto-triggering, allowing for constant
	//	    analog to digital conversions.
}

void Set_A2D_Pin(unsigned char pinNum) {
ADMUX = (pinNum <= 0x07) ? pinNum : ADMUX;
// Allow channel to stabilize
static unsigned char i = 0;
for ( i=0; i<15; i++ ) { asm("nop"); } 
}

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

void set_array(unsigned char cur[], unsigned char new[]) {
	unsigned char i;	
	for (i = 0; i < 3; i++) {
		cur[i] = new[i];
	}
}

