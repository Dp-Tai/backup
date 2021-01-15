
// definition for GPIO module
#ifdef GPIO

 bool Gpio_init(unsigned char port, int type);
 bool Gpio_Read(int* data_read_ptr, unsigned char  port);
 bool Gpio_Write(int data_write, unsigned char  port);

#endif