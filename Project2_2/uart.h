/*
 * IncFile1.h
 *
 * Created: 3/11/2017 8:56:21 PM
 *  Author: Cyrus
 */ 

#include <avr/io.h>

#ifndef INCFILE1_H_
#define INCFILE1_H_

#define F_CPU 16000000UL 
#define USART_BAUDRATE 9600
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

extern void uart_start(void);

extern void uart_write(char *data);

#endif /* INCFILE1_H_ */