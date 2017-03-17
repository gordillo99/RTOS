/*
 * CFile1.c
 *
 * Created: 3/11/2017 8:58:41 PM
 *  Author: Cyrus
 */ 

#include "uart.h"
#include "os.h"

void uart_start(void) {
	Disable_Interrupt();
	UCSR0B |= (1 << RXEN0) | (1 << TXEN0); 

	UBRR0L = PRESCALE; 
	UBRR0H = (PRESCALE >> 8); 

	UCSR0B |= (1 << RXCIE0); 
	Enable_Interrupt();
}

void uart_write(char *data) {
	Disable_Interrupt();
    while (*data) {
        while ((UCSR0A & (1 << UDRE0)) == 0);
        UDR0 = *data; 
        data += 1;
    }
    while ((UCSR0A & (1 << UDRE0)) == 0);
    UDR0 = '\n';
	Enable_Interrupt();
}