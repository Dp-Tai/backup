/*
 * Node_Sensor.cpp
 *
 * Created: 1/10/2021 1:56:46 PM
 * Author : DVT
 */ 
#define F_CPU 12000000 // 12mhz
#define CKSEL 0xF
#define GPIO

#define FALSE false
#define TRUE true

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>



#define __Interrupt_Enable__ sei()
#define ADC_ARRAY_SIZE 10

// definitions init functions
void init();
bool Gpio_init(unsigned char  port, int type);
bool timer_init(int timer_number);
void adc_init();
void uart_init();

void adc_read(uint8_t ch);
void uart_char_tx(unsigned char data);

// APIs definition


//unsigned char *Uart_data_ptr;
//long int *Adc_data_ptr;
uint16_t ADC_array[ADC_ARRAY_SIZE];
uint16_t ADC_value_raw;
unsigned char ADC_value;
bool uart_send_flag;
volatile long int timer0_cnt; // counter for timer 0 - 8 bit
volatile long int timer1_cnt; // counter for timer 1 - 16 bit 
volatile long int Global_counter_1; // counter 1s

int main(void)
{
	// enable all the interrupt
	__Interrupt_Enable__;
	
    /* Replace with your application code */
	init();	
    while (1) 
    {
		if (uart_send_flag)
		{
			uart_char_tx(ADC_value);
			uart_send_flag = false;
		}
    }
}

void init()
{
	memset(ADC_array,0,ADC_ARRAY_SIZE);
	ADC_value = 0;
	Global_counter_1 = 0;
	uart_send_flag = false;
	Gpio_init('C',0xf);
	timer_init(3);
	adc_init();
	uart_init();
}

// initialize GPIO 
/*
	input: port and type
	port: port name, B or C or D 
	type: set mode working: 0 - input, 1 - output	
*/
bool Gpio_init(unsigned char port, int type)
{
	switch (port)
	{
		//  case 'A':
		//       DDRA = type;
		//      break;
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
		return FALSE;
		break;
	}
	return TRUE;
};

// initialize timer 
/* 
	input: timer number
	0: timer 0 - 8 bit, mode overflow interrupt
	1: timer 1 - 16 bit
	2: timer 0 - 8 bit and timer 1 - 16 bit - mode overflow interrupt
*/
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

// initialize uart
void uart_init()
{
	//set baud, 57.6k ung voi f=8Mhz, xem bang 70 trang 165, Atmega32 datasheet
	UBRRH=0;
	UBRRL=77;
	
	//set khung truyen va kich hoat bo nhan du lieu
	UCSRA=0x00;
	UCSRC=(1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);
	UCSRB=(1<<TXEN);
}
// initialize adc
void adc_init()
{
	// AREF = AVcc
	ADMUX = (1<<REFS0);
	
	// ADC Enable and prescaler of 128
	// 16000000/128 = 125000
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
	
	ADC_value_raw = 0;
}

void adc_read(uint8_t ch)
{
	// select the corresponding channel 0~7
	// ANDing with '7' will always keep the value
	// of 'ch' between 0 and 7
	ch &= 0b00000111;  // AND operation with 7
	ADMUX = (ADMUX & 0xF8)|ch;     // clears the bottom 3 bits before ORing
	
	// start single conversion
	// write '1' to ADSC
	ADCSRA |= (1<<ADSC);
	
	// wait for conversion to complete
	// ADSC becomes '0' again
	// till then, run loop continuously
	while(ADCSRA & (1<<ADSC));
	
	ADC_value_raw = ADC;
}

//UART send data
void uart_char_tx(unsigned char chr){
	while (bit_is_clear(UCSRA,UDRE)) {}; //when bit UDRE=1, buffer is clear
	UDR=chr;
}

// TIMER1 overflow interrupt service routine
// called whenever TCNT1 overflows
ISR(TIMER1_OVF_vect)
{
	// keep a track of number of overflows
	timer1_cnt++;
	
	// sample of ADC: 1 sample in 1 second
	// when timer 1 overflow 30 times ~ 1s, reset counter for 1s
	if (timer1_cnt == 30)
	{
		PORTC ^= 0x1;
		timer1_cnt = 0;
		Global_counter_1++;
		// read value adc port 7 and save to adc_value_raw
		adc_read(7);
		ADC_value = (unsigned char)(ADC_value_raw * 500 / 1023); // temporature
	}
	if (Global_counter_1 == 5)
	{
		uart_send_flag = true;
		Global_counter_1 = 0;
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
		// PORTC ^= 0x03;
		timer0_cnt = 0;
		
	}
}


ISR(USART_RXC_vect)
{
	//uart_char_rx(Uart_data_ptr);
}


// Control ADC buffer
void FIFO_Algorithm(uint16_t *buffer)
{
	
}