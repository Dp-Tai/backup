


#include <avr/io.h>
#include "Gpio.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define TRUE true
#define FALSE false
// Initialize for gpio module

// definition type of port (input or output)
// input: port number, pin, type : 1: output, 0: input
// output: always true
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

