/*
 * Node_Sensor.cpp
 *
 * Created: 1/10/2021 1:56:46 PM
 * Author : DVT
 */ 
#define F_CPU 12000000 // 12mhz
#define CKSEL 0xF
#define GPIO


#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


#include "Gpio.h"

// definition macro here
bool gpio_init(unsigned char  port, int type);
bool timer_init(int timer_number);
volatile long int timer0_cnt; // counter for timer 0 - 8 bit
volatile long int timer1_cnt; // counter for timer 1 - 16 bit

int main(void)
{
    /* Replace with your application code */
	(void)gpio_init('C',0xf);
	(void)Gpio_init('C',0xf);
	timer_init(3);
    while (1) 
    {
    }
}

bool gpio_init(unsigned char  port, int type)
{
	switch (port)
	{
		// case 'A':
		//     DDRA = type;
		//     break;
		case 'B':
		DDRB = type;
		break;
		case 'C':
		DDRC = type;
		break;
		case 'D':
		DDRD = type;
		break;
		default:
		return false;
		break;
	}
	return true;
};

bool timer_init(int timer_number)
{
	if (timer_number == 0)
	{
		TCCR0 = 2; // set prescale 8 , F_CPU = 12mhz
		//TOIE0 |= 1; // enable overflow interrupt
		// initialize counter
		TCNT0 = 0; 
	    // enable overflow interrupt
		TIMSK |= (1 << TOIE0);
	    
		// enable global interrupts
		sei();
	    
		// initialize overflow counter variable
		timer0_cnt = 0;
	}
	else if (timer_number == 1)
	{
		TCCR1B |= 0x2; // prescale  = 8
		TIMSK |= 0x4; // overflow interrupt
		TCNT1 = 0; // initialize value for timer 1
		ICR1 = 50000; // set top - MAX VALUE = 50000, timer counter = 30 ~ 1s 
		
		// enable global interrupts
		sei();
				
		// initialize overflow counter variable
		timer1_cnt = 0;
	}
	else
	{
		TCCR0 = 2; // set prescale 8
		//TOIE0 |= 1; // enable overflow interrupt
		// initialize counter
		TCNT0 = 0;
		// enable overflow interrupt
		TIMSK |= (1 << TOIE0);
		
		// timer 2 setting
		TCCR1B |= 0x2; // prescale  = 8
		TIMSK |= 0x4; // overflow interrupt
		TCNT1 = 0; // initialize value for timer 1
		ICR1 = 50000; // set top = 50000, timer counter = 30 ~ 1s
		
		// enable global interrupts
		sei();
		
		// initialize overflow counter variable
		timer0_cnt = 0;	
		timer1_cnt = 0;
	}
}

// TIMER1 overflow interrupt service routine
// called whenever TCNT1 overflows
ISR(TIMER1_OVF_vect)
{
	// keep a track of number of overflows
	timer1_cnt++;
	if (timer1_cnt == 30)
	{
		PORTC ^= 0x0c;
		timer1_cnt = 0;
		
	}
}
// TIMER0 overflow interrupt service routine
// called whenever TCNT0 overflows
ISR(TIMER0_OVF_vect)
{
	// keep a track of number of overflows
	timer0_cnt++;
	if (timer0_cnt == 3000)
	{
		PORTC ^= 0x03;
		timer0_cnt = 0;
		
	}
}
