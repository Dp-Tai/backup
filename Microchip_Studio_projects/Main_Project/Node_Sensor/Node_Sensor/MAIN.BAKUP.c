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

#include "Gpio.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>



#define __Interrupt_Enable__ sei()

// definitions init functions
bool gpio_init(unsigned char  port, int type);
bool timer_init(int timer_number);
bool uart_init(int channel);
bool adc_init(int channel, int type);
void ADC_INIT_ORIGINAL();

// APIs definition
void adc_read(int *data_ptr, int channel);
void uart_char_tx(unsigned char chr);
void uart_char_rx(unsigned char *data_ptr);

unsigned char *Uart_data_ptr;
long int *Adc_data_ptr;
volatile long int timer0_cnt; // counter for timer 0 - 8 bit
volatile long int timer1_cnt; // counter for timer 1 - 16 bit

int main(void)
{
	// enable all the interrupt
	__Interrupt_Enable__;
	
    /* Replace with your application code */
	ADC_INIT_ORIGINAL();
	uart_init(0);
	(void)Gpio_init('C',0xf);
	timer_init(3);
	//long int adc_value = 0;
    while (1) 
    {
		*Uart_data_ptr = (unsigned char)(((*Adc_data_ptr) * 500)/1023);
		uart_char_tx(*Uart_data_ptr);
		_delay_ms(100);
    }
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


//initialize UART 
bool uart_init(int channel)
{
	UCSRB |= (1<<RXCIE) | (1<<TXCIE) | (1<<RXEN) | (1<<TXEN); // enable interrupt after finishing transmitting data
	// enable interrupt finishing receiving data
	 // 1: enable interrupt when data buffer is empty
	 // enable receiving data mode
	 //enable transmit mode
	UCSRC |= (1<UPM1) | (1<UPM0) | (1<<UCSZ1);
	 // set odd parity 
	 // set odd parity
	 // 1 stop bit, 1: 2 stop bit selection
	// set data length: 010: data length = 7
	// mode normal: 1: dubble speed mode
//	U2X = 0;
	// set baudrate = 9600 at 12mhz
	UBRRH = 0;
	UBRRL = 0x47;
	
	*Uart_data_ptr = 0;
}

//initialize ADC 
/*
	input: ADC channel: 0..7, type: internal or external trigger: for more information, please refer datasheet
*/
bool adc_init(int channel, int type)
{
	// Bits 7:6 – REFS1:0
	// set vref of adc: 0: internal , 1: external AVCC with cap at VREF, 2: reserved, 3: internal 2.5 vref with external cap at Vref pin
	ADMUX  |= 0b00000000;
	
	// final 5 bit of ADMUX is used to choose adc channel
	ADMUX |= channel;
	
	// enable adc conversion
	ADCSRA |= (1<<ADEN);
	
	if (type == 1)
	{
		ADCSRA  |=  0b00100000;  // enable external source: 0: use cycle convert, 1: external source trigger
		SFIOR   |=  0x4; // enable the external trigger as the overflow of timer 0
	}
	
	ADCSRA |= (1<<ADIE) | (1<<ADPS1) | (1< ADPS0);
	 // enable adc interrupt after finishing conversion
	 // default prescale of adc = f_cpu / 128 = 93.75khz
	 // default prescale of adc = f_cpu / 128 = 93.75khz
}

void ADC_INIT_ORIGINAL()
{
	// AREF = AVcc
	ADMUX = (1<<REFS0);
	
	// ADC Enable and prescaler of 128
	// 16000000/128 = 125000
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
	
	*Adc_data_ptr = 0;
	
	//ch &= 0b00000111;  // AND operation with 7
	ADMUX &= 0b11100000; // clears the bottom 3 bits before ORing
	ADMUX |= 0x07;
}


// APIs definition

// ADC convert api
// ADC start conversion, this is called in interrupt  

/*void adc_read(int *data_ptr)
{
	*data_ptr = ADC;
}*/
void adc_read(long int *data_ptr)
{
	// select the corresponding channel 0~7
	// ANDing with ’7′ will always keep the value
	// of ‘ch’ between 0 and 7
	
	// start single convertion
	// write ’1′ to ADSC
	ADCSRA |= (1<<ADSC);
	
	// wait for conversion to complete
	// ADSC becomes ’0′ again
	// till then, run loop continuously
	while(ADCSRA & (1<<ADSC));
	*data_ptr = ADC;
}

// UART send data API
void uart_char_tx(unsigned char chr){
	while (UDRE) {}; //cho den khi bit UDRE=1
	UDR=chr;
}

// UART read data api (this function is used in enterrupt)
void uart_char_rx(unsigned char *data_ptr)
{
	*data_ptr = UDR;
}


// TIMER1 overflow interrupt service routine
// called whenever TCNT1 overflows
ISR(TIMER1_OVF_vect)
{
	// keep a track of number of overflows
	timer1_cnt++;
	if (timer1_cnt == 30)
	{
		PORTC ^= 0xF;
		timer1_cnt = 0;
		adc_read(Adc_data_ptr);
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
	uart_char_rx(Uart_data_ptr);
}