/*	Author: Christopher Young - cyoun069@ucr.edu
 *  	Partner(s) Name: 
 *	Lab Section: 22
 *	Assignment: Lab #14
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://drive.google.com/file/d/1V9qZE_O1E1GvLMKHrOaXDedA-CdC5TFb/view?usp=sharing
 *
 */
#include <avr/io.h>
#include <stdlib.h>
#include "timer.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

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

// game states

// menu
unsigned char menu_position = 0;
unsigned char ingame = 0;
unsigned char gamemode = 0;

// display
unsigned char row[3] = {0x02, 0x08, 0};
unsigned char col[3] = {0x08, 0x0A, 0};

// game mechanics

unsigned char p1_score = 0;
unsigned char p2_score = 0;
// position is LED count from from bottom / left
// direction uses values -1 / 0 / 1 (0 meaning no direction or movement) 
unsigned char p1_pos = 0;
signed char p1_dir = 0;

unsigned char p2_pos = 0;
signed char p2_dir = 0;

unsigned char b_pos_x = 0;
unsigned char b_pos_y = 0;
signed char b_dir_x = 0;
signed char b_dir_y = 0;
unsigned char b_speed_x = 0;
unsigned char b_speed_y = 0;


// sync SMs
enum Game_States {Game_init, Game_menu, Game_countdown, Game_single, Game_multi};
int Game_Tick(int state);

enum Control1_States {Control1_wait, Control1_control} ; // player 1 control
int Control1_Tick(int state);

enum Control2_States {Control2_wait, Control2_control, Control2_AI} ; // AI / player 2 control
int Control2_Tick(int state);

enum Ball_States {Ball_wait, Ball_move}; // moves and updates on collisions
int Ball_Tick(int state);

enum Display_States {Display_init, Display_display};
int Display_Tick(int state);

typedef struct _task {
	signed char state;
	unsigned long int period;
	unsigned long int elapsedTime;
	int (*TickFct) (int);
} task;

int main(void) {
	/* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFC; // Configure port A's last 6 pins as inputs
	DDRB = 0xFF; PORTB = 0x00; // Configure port B's 8 pins as output initialized as 0x00
	DDRC = 0xFF; PORTC = 0x00; // Configure port C's 8 pins as output initialized as 0x00
  	DDRD = 0xFF; PORTD = 0x00; // Configure port D's 8 pins as output initialized as 0x00
	
	A2D_init();

	static task task1, task2, task3, task4, task5;
	task *tasks[] = { &task1, &task2, &task4, &task5, &task3 };
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	const char start = -1;

	// Task 1 (game)
	task1.state = start;
	task1.period = 50;
	task1.elapsedTime = task1.period;
	task1.TickFct = &Game_Tick;

	// Task 2 (control1)
	task2.state = start;
	task2.period = 100;
	task2.elapsedTime = task2.period;
	task2.TickFct = &Control1_Tick;

	// Task 3 (display)
	task3.state = start;
	task3.period = 1;
	task3.elapsedTime = task3.period;
	task3.TickFct = &Display_Tick;

	// Task 4 (control2)
	task4.state = start;
	task4.period = 100;
	task4.elapsedTime = task4.period;
	task4.TickFct = &Control2_Tick;	

	// Task 5 (ball)
	task5.state = start;
	task5.period = 50;
	task5.elapsedTime = task5.period;
	task5.TickFct = &Ball_Tick;	

	unsigned short i; // iterator
	unsigned long GCD = tasks[0]->period;
	for (i = 0; i < numTasks; i++) {
		GCD = findGCD(GCD, tasks[i]->period);
	}
	
	unsigned char timerPeriod = GCD;
	TimerSet(timerPeriod);
	TimerOn();

	
	while (1) {
		for (i = 0; i < numTasks; i++) { // scheduler
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

int Game_Tick(int state) {
	unsigned char A3 = ~PINA & 0x04; // start / reset button

	static unsigned char counter = 0;
	unsigned char i;
	
	switch(state) {
		case Game_init: 			
			state = Game_menu;
			break;
		case Game_menu:
			if (A3) { // on any button press
				// set selected game mode
				if (!menu_position) gamemode = 1; // (singleplayer)
				else gamemode = 2; // (multiplayer)
	
				// initialize countdown display
				unsigned char new_row[] = {0x02, 0x04, 0x08};
				unsigned char new_col[] = {0xFF, 0xFF, 0xFF};
				set_array(row, new_row);
				set_array(col, new_col);

				// initialize countdown counter
				counter = 0;

				// reset scores
				p1_score = 0;
				p2_score = 0; 

				
				// set new state
				state = Game_countdown;
			}
			else state = Game_menu;
			break;
		case Game_countdown:
			if (counter >= 20) { // after 1 second
				// enable L/R joystick
				Set_A2D_Pin(0x00);

				// initialize positions
				p1_pos = 3;
				p2_pos = 3;
				b_pos_x = 4;
				b_pos_y = 2;
				b_dir_x = 1;
				b_dir_y = 1;
				b_speed_x = 1;
				b_speed_y = 1;

				// set ingame flag
				ingame = 1;

				// set new state
				if (gamemode == 1) state = Game_single;
				else if (gamemode == 2) state = Game_multi;
			}
			else state = Game_countdown;
			break;
		case Game_single:
			if (ingame) state = Game_single;
			else state = Game_init;

			break;
		case Game_multi:
			if (ingame) state = Game_multi;
			else if (p1_score < 2 && p2_score < 2 && gamemode == 2) {state = Game_countdown; }
			else state = Game_init;

			break;
		default: state = Game_init;
	}

	switch(state) {
		case Game_init: 
			// initialize menu variables
			menu_position = 0;
			ingame = 0;
			gamemode = 0;

			// initialize menu display
			unsigned char new_row[3] = {0x02, 0x08, 0};
			unsigned char new_col[3] = {0x08, 0x0A, 0};
			set_array(row, new_row);
			set_array(col, new_col);

			// enable U/D joystick
			Set_A2D_Pin(0x01);
			break;
		case Game_menu:
			// select gamemode
			if (ADC > 525) menu_position = 0;
			else if (ADC < 425) menu_position = 1;

			// update menu display
			if (menu_position) {
				col[0] = 0x08;
				col[1] = col[1] | 0x60;
			}
			else {
				col[0] = col[0] | 0x60;
				col[1] = 0x0A;
			}
			break;
		case Game_countdown:
			// update countdown display
			for (i=0;i<3;i++) { if(counter%2==0) col[i] = col[i] >> 1; }

			// increment counter
			counter++;
			break;
		case Game_single:
		case Game_multi:
			// check for game end
			if (b_pos_y <= 0 ) {p2_score++; ingame = 0;}
			if (b_pos_y >= 5) {p1_score++; ingame = 0;}
			if (A3) {ingame = 0; gamemode = 0;} // soft reset

			// update ingame display 
			row[0] = 0x10; col[0] = 0xF0 >> (p1_pos-1); 
			row[1] = 0x01; col[1] = 0xF0 >> (p2_pos-1); 
			row[2] = 0x10 >> (b_pos_y-1); col[2] = 0x80 >> (b_pos_x-1); 
			break;
		default: break;
	}

	return state;
}

int Control1_Tick(int state) {
	switch(state) {
		case Control1_wait: 
			if (ingame) state = Control1_control;
			else state = Control1_wait;
			break;
		case Control1_control:
			if (!ingame) state = Control1_wait;
			else state = Control1_control;
			break;
		default: state = Control1_wait;
	}

	switch(state) {
		case Control1_control:
			if (ADC > 525 && p1_pos < 5) {
				p1_dir = 1;
				p1_pos++;
			}
			else if (ADC < 425 && p1_pos > 0) {
				p1_dir = -1;
				p1_pos--;
			}
			else p1_dir = 0;
		default: break;
	}

	return state;
}

int Control2_Tick(int state) {
	unsigned char A4 = ~PINA & 0x08; // left button
	unsigned char A5 = ~PINA & 0x10; // right button

	switch(state) {
		case Control2_wait: 
			if (ingame && gamemode == 1) state = Control2_AI;
			else if (ingame && gamemode == 2) state = Control2_control;
			else state = Control2_wait;
			break;
		case Control2_AI:
			if (!ingame) state = Control2_wait;
			else state = Control2_AI;
			break;
		case Control2_control:
			if (!ingame) state = Control2_wait;
			else state = Control2_control;
			break;
		default: state = Control2_wait;
	}

	switch(state) {
		case Control2_AI:
			if (rand() % 3) { // moves 2/3 of the time
				unsigned char new_pos = (b_pos_x+(1*b_dir_x)); // future x pos of ball
				
				// if player is at the end, attempt a center shot
				if (p1_pos == 1 || p1_pos == 5) {
					if (new_pos > p2_pos+2 && p2_pos < 5) { // move right
						p2_dir = 1;
						p2_pos++;
					}
					else if (new_pos < p2_pos+1 && p2_pos > 0) { // move left
						p2_dir = -1;
						p2_pos--;
					}
					else p2_dir = 0;
				}

				// if player is near center, attempt a corner shot
				if (p1_pos > 1 && p1_pos < 5) {
					if (new_pos > p2_pos+3 && p2_pos < 5) { // move right
						p2_dir = 1;
						p2_pos++;
					}
					else if (new_pos < p2_pos && p2_pos > 0) { // move left
						p2_dir = -1;
						p2_pos--;
					}
					else p2_dir = 0;
				}
			}
			break;
		case Control2_control:
			if (A5 && p2_pos < 5) {
				p2_dir = 1;
				p2_pos++;
			}
			else if (A4 && p2_pos > 0) {
				p2_dir = -1;
				p2_pos--;
			}
			else p1_dir = 0;
			break;
		default: break;
	}

	return state;	
}

int Ball_Tick(int state) {
	static unsigned char counter = 0;
	switch(state) {
		case Ball_wait: 
			if (ingame) {
				// initialize update counter
				counter = 0;
				// set new state
				state = Ball_move;
			}	
			else state = Ball_wait;
			break;
		case Ball_move:
			if (!ingame) state = Ball_wait;
			else state = Ball_move;
			break;
		default: state = Ball_wait;
	}

	switch(state) {
		case Ball_move:
		// speed and counter determine when ball movement updates
		if (counter >= 5) {
			// update direction on wall collision (this must be done before player collision)
			if (b_pos_x - b_speed_x <= 0) b_dir_x = 1;
			else if (b_pos_x + b_speed_x >= 8) b_dir_x = -1;	

			// update direction and speed on player collision
			unsigned char new_pos = (b_pos_x+(1*b_dir_x)); // future x pos of ball
			
			if (b_dir_y == -1 && b_pos_y == 2) { // if ball is approaching P1
				if (new_pos > p1_pos && new_pos < p1_pos+3) { // if ball touches center
					b_dir_y = 1;
					if (!p1_dir) { // if P1 not moving					
						b_speed_x = 1;
						b_speed_y = 1;
					}
					else { // if P1 moving
						if (p1_dir == 1) b_dir_x = -1;
						else if (p1_dir == -1) b_dir_x = 1;
						b_speed_x = 1;
						b_speed_y = 1;
					}
				}
				if (new_pos == p1_pos || new_pos == p1_pos+3) { // if ball touches corner
					b_dir_y = 1;
					if (!p1_dir) { // if P1 not moving					
						b_speed_x = 1;
						b_speed_y = 2;
					}
					else { // if P1 moving
						if (p1_dir == 1) b_dir_x = -1;
						else if (p1_dir == -1) b_dir_x = 1;
						b_speed_x = 2;
						b_speed_y = 1;
					}
				}
			}
			if (b_dir_y == 1 && b_pos_y == 4) { // if ball is approaching P2
				if (new_pos > p2_pos && new_pos < p2_pos+3) { // if ball touches center
					b_dir_y = -1;
					if (!p2_dir) { // if P2 not moving					
						b_speed_x = 1;
						b_speed_y = 1;
					}
					else { // if P2 moving
						if (p2_dir == 1) b_dir_x = -1;
						else if (p1_dir == -1) b_dir_x = 1;
						b_speed_x = 1;
						b_speed_y = 1;
					}
				}
				if (new_pos == p2_pos || new_pos == p2_pos+3) { // if ball touches corner
					b_dir_y = -1;
					if (!p2_dir) { // if P2 not moving					
						b_speed_x = 1;
						b_speed_y = 2;
					}
					else { // if P2 moving
						if (p2_dir == 1) b_dir_x = -1;
						else if (p1_dir == -1) b_dir_x = 1;
						b_speed_x = 2;
						b_speed_y = 1;
					}
				}
			}

			// update position
			b_pos_x += b_speed_x * (b_dir_x);
			
			b_pos_y += b_speed_y * (b_dir_y); 

			// reset counter
			counter = 0;
		}
		else counter++;
		break;
		default: break;
	}

	return state;
}

int Display_Tick(int state) {
	static unsigned char i = 0;
	switch(state) {
		case Display_init: state = Display_display; break;
		case Display_display: state = Display_display; break; 
		default: state = Display_init; break;
	}

	switch(state) {
		case Display_init: break;
		case Display_display:
			if (i >= 3) i = 0;
			// LED Matrix (Menu / Game)
			transmit_data(col[i]);
			PORTD = ~row[i];

			// single LED (Scoreboard)
			switch (p1_score) {
				case 0: PORTB = 0x00; break;
				case 1: PORTB = 0x01; break;
				case 2: PORTB = 0x03; break;
				default: break;
			}
			switch (p2_score) {
				case 0: break;
				case 1: PORTB |= (0x01 << 2); break;
				case 2: PORTB |= (0x03 << 2); break;
				default: break;
			}		

			i++;
			break;
		default: break;
	}	

	return state;
}


