
// definition for GPIO module
#ifdef GPIO
#include <stdbool.h>

extern bool Gpio_init(unsigned char port, int type);
extern bool Gpio_Read(int* data_read_ptr, unsigned char  port);
extern bool Gpio_Write(int data_write, unsigned char  port);

#endif