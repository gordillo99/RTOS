#include <avr/io.h>
#include <util/delay.h>
#include "os.h"

/***** Core System Tasks *****/
void Task_WriteBluetooth()
{
    for(;;){
		DDRA |= (1<<PA3);
		PORTA &= ~(1<<PA3);
        //Task_Next();
    }
}


void Task_ReadJoystick()
{
    for(;;)
    {
        DDRA |= (1<<PA3);
        PORTA |= (1<<PA3);
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
    Task_Create(Task_ReadJoystick, PERIODIC, 0, 0, 1, 3);
    Task_Create(Task_WriteBluetooth, PERIODIC, 0, 1, 1, 3);

    //Task_Create(Idle, 10, 0);

    // Kill the initialization task
    Task_Terminate();
}
