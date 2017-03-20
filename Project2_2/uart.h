
#ifndef INCFILE1_H_
#define INCFILE1_H_

#include <avr/io.h>
#define F_CPU 16000000UL 
#define BAUDRATE 9600
#define PRESCALE (((F_CPU / (BAUDRATE * 16UL))) - 1)

extern void uart_start(void);

extern void uart_write(char *data);

#endif /* INCFILE1_H_ */