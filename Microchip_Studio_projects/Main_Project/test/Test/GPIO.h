/*
 * GPIO.h
 *
 * Created: 1/11/2021 8:09:14 PM
 *  Author: DVT
 */ 


#ifndef GPIO_H_
#define GPIO_H_

//#ifdef GPIO

#include <stdbool.h>

extern bool Gpio_init(int port, int type);
extern bool Gpio_Read(int* data_read_ptr, unsigned char  port);
extern bool Gpio_Write(int data_write, unsigned char  port);

//#endif



#endif /* GPIO_H_ */