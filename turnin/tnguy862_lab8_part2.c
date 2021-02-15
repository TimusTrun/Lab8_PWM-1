/*	Author: Tann Nguyen
 *  Partner(s) Name: 
 *	Lab Section:022
 *	Assignment: Lab #8  Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

void set_PWM(double frequency) {
	static double current_frequency;
	if(frequency != current_frequency) {
		if(!frequency)
			TCCR3B &= 0x08;
		else
			TCCR3B |= 0x03;
		if(frequency < 0.954)
			OCR3A = 0xFFFF;
		else if(frequency > 31250)
			OCR3A = 0x0000;
		else
			OCR3A = (short)(8000000 / (128 * frequency)) - 1;
		TCNT3 = 0;
		current_frequency = frequency;
	}
}

void PWM_on() {
	TCCR3A = (1 << COM3A0);
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	set_PWM(0);
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

enum Clk_States {Clk_Start, Clk_Press, Clk_Release}Clk_State;
static unsigned char i = 0x01;

unsigned char click_fct() {
	unsigned char temp = ~PINA;
	switch(Clk_State) {
		case Clk_Start:
			if((temp & 0x01) == 0x01)
				Clk_State = Clk_Press;
			else if((temp & 0x02) == 0x02)
				Clk_State = Clk_Press;
			break;
		case Clk_Press:
			Clk_State = Clk_Release;
			break;
		case Clk_Release:
			if(((temp & 0x01) != 0x01) && ((temp & 0x02) != 0x02))
				Clk_State = Clk_Start;
			break;
		default:
			Clk_State = Clk_Start;
			break;
	}
	switch(Clk_State) {
		case Clk_Press:
			if((temp & 0x01) == 0x01) {
				if(i < 8)
					i++;
			}
			else if((temp & 0x02) == 0x02) {
				if(i > 1)
					i--;
			}
		default:
			break;
	}
	return i;
}

void play(unsigned char i) {
	switch(i) {
		case 0x01:
			set_PWM(261.63);
			break;
		case 0x02:
			set_PWM(293.66);
			break;
		case 0x03:
			set_PWM(329.63);
			break;
		case 0x04:
			set_PWM(349.23);
			break;
		case 0x05:
			set_PWM(392.00);
			break;
		case 0x06:
			set_PWM(440.00);
			break;
		case 0x07:
			set_PWM(493.88);
			break;
		case 0x08:
			set_PWM(523.25);
			break;

	}
}

enum Tgl_States {Tgl_init, Tgl_n1, Tgl_n2, Tgl_prs, Tgl_rls} Tgl_State;
unsigned char prev = 0x00;
void toggle_fct(unsigned char bruh) {
	unsigned char temp = ~PINA;
	switch(Tgl_State) {
		case Tgl_init:
			Tgl_State = Tgl_n1;
			break;
		case Tgl_n1:
			if((temp & 0x04) == 0x04)
				Tgl_State = Tgl_prs;
			prev = Tgl_n1;
			break;
		case Tgl_prs:
			Tgl_State = Tgl_rls;
			break;
		case Tgl_rls:
			if((temp & 0x04) != 0x04) {
				if(prev == Tgl_n1)
					Tgl_State = Tgl_n2;
				if(prev == Tgl_n2)
					Tgl_State = Tgl_init;
			}
			break;
		case Tgl_n2:
			if((temp & 0x04) == 0x04)
				Tgl_State = Tgl_prs;
			prev = Tgl_n2;
			break;
	}
	switch(Tgl_State) {
		case Tgl_n2:
			play(bruh);
			break;
		case Tgl_n1:
			set_PWM(0);
			break;
		default:
			break;
	}
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00;	PORTA = 0xFF;
	DDRB = 0xFF;	PORTB = 0x00;
    /* Insert your solution below */
	PWM_on();
	Clk_State = Clk_Start;
	Tgl_State = Tgl_init;
    while (1) {
	    unsigned char bruh = click_fct();
	toggle_fct(bruh);
    }
    return 1;
}
