/*
 * GPIO.c
 *
 * Created: 1/11/2021 8:09:01 PM
 *  Author: DVT
 */ 


#define GPIO


#include "GPIO.h"
#include <avr/io.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>


#define TRUE true
#define FALSE false
// Initialize for gpio module

// definition type of port (input or output)
// input: port number, pin, type : 1: output, 0: input
// output: always true
bool Gpio_init(int port, int type)
{
	switch (port)
	{
		//  case 'A':
		//       DDRA = type;
		//      break;
		case 11:
		DDRB = type;
		break;
		case 12:
		DDRC = type;
		break;
		case 13:
		DDRD = type;
		break;
		default:
		return FALSE;
		break;
	}
	return TRUE;
};





