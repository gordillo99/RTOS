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
	
	int x = 0;
	for (;x < 10000;x++) {
		PORTA |= (1<<PA5);
		PORTA &= ~(1<<PA5);
	}
	Task_Terminate();
}

void ASDF2()
{
	for (;;) {
		PORTA |= (1<<PA4);
		PORTA &= ~(1<<PA4);
	}
	
}

void a_main()
{
	
    // Initialize tasks
	//Task_Create(ASDF, SYSTEM, 0);
    Task_Create(Task_ReadJoystick, PERIODIC, 0, 0, 1, 3);
    Task_Create(Task_WriteBluetooth, PERIODIC, 0, 2, 1, 3);
	Task_Create(ASDF2, RR, 0, -1, -1, -1);
	Task_Create(ASDF, SYSTEM, 0, -1, -1, -1);

    // Kill the initialization task
    Task_Terminate();
}
