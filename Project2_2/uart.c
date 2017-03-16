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
	UCSR0B |= (1 << RXEN0) | (1 << TXEN0); //transmit side of hardware
	UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01); //receive side of hardware

	UBRR0L = BAUD_PRESCALE; //set the baud to 9600, have to split it into the two registers
	UBRR0H = (BAUD_PRESCALE >> 8); //high end of baud register

	UCSR0B |= (1 << RXCIE0); //recieve data interrupt, makes sure we don't loose data
	Enable_Interrupt();
}

void uart_write(char *data) {
    /*
    Use this to send a string, it will split it up into individual parts
    send those parts, and then send the new line code
    */
	Disable_Interrupt();
    while (*data) {
        while ((UCSR0A & (1 << UDRE0)) == 0);//make sure the data register is cleared
        UDR0 = *data; //goes through and splits the string into individual bits, sends them
        data += 1;//go to new bit in string
    }
    while ((UCSR0A & (1 << UDRE0)) == 0);//make sure the data register is cleared
    UDR0 = '\n';//send a new line just to be sure
	Enable_Interrupt();
}