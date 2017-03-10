#include <avr/io.h>
#include <util/delay.h>
#include "os.h"

/***** Core System Tasks *****/
void Task_WriteBluetooth()
{	int x;
	DDRB |= (1<<PB7);
    for(;;){
		PORTB |= (1<<PB7); //MAKE LED FLASH
		//for(x = 0; x < 32; x++){}
		//DDRA |= (1<<PA3);
		//PORTA &= ~(1<<PA3);
        //Task_Next();
    }
}


void Task_ReadJoystick()
{	int x;
	DDRB |= (1<<PB7);
    for(;;)
    {
		PORTB = (0<<PB7); //TURN LED FLASH OFF
		//for(x = 0; x < 32; x++){}
        //DDRA |= (1<<PA3);
        //PORTA |= (1<<PA3);
       // Task_Next();
    }
}


void Task_Test_Periodic()
{	int x = 0;
	DDRB |= (1<<PB5);
	for(;;)
	{
		if(x%2 == 0){
			PORTB |= (1<<PB5); //TURN LED FLASH OFF
			for(x = 0; x < 3200; x++){}
		}
		else
			PORTB ^= (1<<PB5);
		x++;
		//Task_Next();
	}
}

void ASDF()
{
/*
	int x = 0;
	for(; x < 100000; x++)
	{
		if (x < 10000000/2) {
			
			DDRA |= (1<<PA4);
			PORTA ^= (1<<PA4);
			//Task_Next();
		} else {
			Task_Terminate();
		}

	}
	*/
	PORTA ^= (1<<PA4);
	_delay_ms(2000);
	PORTA ^= (1<<PA4);
	Task_Terminate();
}


void Idle() {
    for(;;) {}
}

void a_main()
{
	
    // Initialize tasks
	//Task_Create(ASDF, SYSTEM, 0);
    Task_Create_Period(Task_ReadJoystick, 0, 5, 1, 0);
    Task_Create_Period(Task_WriteBluetooth, 0, 5, 1, 2);
	//Task_Create_Period(Task_Test_Periodic, 0, 5, 1, 0);
	
    //Task_Create(Idle, 10, 0);

    // Kill the initialization task
    Task_Terminate();
}
