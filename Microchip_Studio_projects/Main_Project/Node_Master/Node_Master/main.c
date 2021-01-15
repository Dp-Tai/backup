/*
 * Node_Master.c
 *
 * Created: 1/12/2021 9:11:06 PM
 * Author : DVT
 */ 

#define F_CPU 12000000 // define external 12mhz
#define CKSEL 0xF
#define __Enable_All_Interrupt__ sei();

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void init();
void uart_init();
void gpio_init();


void uart_char_tx(unsigned char chr);

void Uart_receive_interrupt_func();

volatile unsigned char uart_data;

int main(void){
	
	__Enable_All_Interrupt__
	init();
	while(1)
	{
		 // uart_char_tx(uart_data);
		//  _delay_ms(1000); 
	}
}

void init()
{
	gpio_init();
	uart_init();
}

void gpio_init()
{
	DDRC = 3;
}

void uart_init()
{
	//set baud, 57.6k ung voi f=8Mhz, xem bang 70 trang 165, Atmega32 datasheet
	UBRRH=0;
	UBRRL=47;
	//set khung truyen va kich hoat bo nhan du lieu
	UCSRA=0x00;
	UCSRC=(1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);
	UCSRB=(1<<RXEN)|(1<<TXEN)|(1<<RXCIE);//cho phep ca 2 qua trinh nhan va//truyen, va cho  phep ngat sau khi nhan xong	
}

//chuong trinh con phat du lieu
void uart_char_tx(unsigned char chr){
	while (bit_is_clear(UCSRA,UDRE)) {}; //cho den khi bit UDRE=1
	UDR=chr;
}


void Uart_receive_interrupt_func()
{
		uart_data=UDR;		
		PORTC ^= 0x0f;
}

ISR(USART_RXC_vect){
	Uart_receive_interrupt_func();
	uart_char_tx(uart_data);
}