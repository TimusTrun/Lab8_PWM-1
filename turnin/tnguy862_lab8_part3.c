/*	Author: Tann Nguyen
 *  Partner(s) Name: 
 *	Lab Section:022
 *	Assignment: Lab #8  Exercise #3
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif
#include "timer.h"

double song[32] = {523.25, 0, 523.35, 0, 523.25, 523.35, 523.25, 523.25, 523.25, 0, 392.00, 392.00, 392.00, 392.00, 392.00, 493.88, 498.88, 498.88, 498.88, 498.88, 523.35, 523.35, 523.35, 493.88, 493.88, 523.25, 523.35, 523.25, 523.25, 523.25, 523.25, 523.25, 523.25, 523.25};

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

enum Sng_States {Sng_init, Sng_off, Sng_rls, Sng_ply}Sng_State;
unsigned char i = 0x00;

void Sng_Fct() {
	unsigned char tmpA0 = ~PINA & 0x01;
	switch(Sng_State) {
		case Sng_init:
			Sng_State = Sng_off;
			i = 0;
			break;
		case Sng_off:
			Sng_State = tmpA0 ? Sng_ply : Sng_off;
			break;
		case Sng_ply:
			if(tmpA0 && i == 37)
				Sng_State = Sng_rls;
			else
				Sng_State = (i == 37) ? Sng_off : Sng_ply;
			break;
		case Sng_rls:
			Sng_State = tmpA0 ? Sng_rls : Sng_off;
			break;
	}
	switch(Sng_State) {
		case Sng_off:
			set_PWM(0);
			i = 0;
			break;
		case Sng_ply:
			set_PWM(song[i]);
			i++;
			break;
		default:
			set_PWM(0);
			break;
	}
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00;	PORTA = 0xFF;
	DDRB = 0xFF;	PORTB = 0x00;
    /* Insert your solution below */
	PWM_on();
	Sng_State = Sng_init;
	TimerSet(100);
	TimerOn();
    while (1) {
	Sng_Fct();
	while(!TimerFlag) {}
	TimerFlag = 0;
    }
    return 1;
}
