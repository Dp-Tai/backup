

#ifdef GPIO

#include <avr/io.h>
// read value from a port
// input: port name , pin number (first pin from 0 ..7) default: 0xFF
// Output: value (1 byte)
 
extern bool Gpio_Read(int* data_read_ptr, unsigned char  port)
{
    switch (port)
    {
    case 'A':
        *data_read_ptr = PORTA;
        break;
    case 'B':
        *data_read_ptr = PORTB;
        break;
    case 'C':
        *data_read_ptr = PORTC;
        break;
    case 'D':
        *data_read_ptr = PORTD;
        break;
    default:
        return FALSE;
        break;
    }
    return TRUE;
}

 bool Gpio_Write(int data_write, unsigned char port)
 {
    switch (port)
    {
    case 'A':
        PORTA = data_write;
        break;
    case 'B':
        PORTB = data_write;
        break;
    case 'C':
        PORTC = data_write;
        break;
    case 'D':
        PORTD = data_write;
        break;
    default:
        return FALSE;
        break;
    }
    return TRUE;
 }

#endif