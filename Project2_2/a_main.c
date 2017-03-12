#include <avr/io.h>
#include <util/delay.h>
#include "os.h"

/***** Core System Tasks *****/
void Task_WriteBluetooth()
{
	Recv(1);
    for(;;){
		DDRA |= (1<<PA4);
		PORTA &= ~(1<<PA4);
        //Task_Next();
		Recv(1);
    }
}


void Task_ReadJoystick()
{
	Chan_Init();
	Recv(1);
	int x = 69;
    for(;;)
    {
        DDRA |= (1<<PA3);
        PORTA |= (1<<PA3);
        //Task_Next();
    }
}

void ASDF()
{
	Recv(1);
	int x = 0;
	for (;;) {
		PORTA |= (1<<PA5);
		PORTA &= ~(1<<PA5);
	}
	Task_Terminate();
}

void ASDF2()
{
	Send(1,69);
	for (;;) {
		PORTA |= (1<<PA4);
		PORTA &= ~(1<<PA4);
	}
	
}

void a_main()
{
	
    // Initialize tasks
	//Task_Create(ASDF, SYSTEM, 0);
    Task_Create(Task_ReadJoystick, RR, 0, -1, -1, -1);
    Task_Create(Task_WriteBluetooth, RR, 0, -1, -1, -1);
	Task_Create(ASDF, RR, 0, -1, -1, -1);
	Task_Create(ASDF2, RR, 0, -1, -1, -1);

    // Kill the initialization task
    Task_Terminate();
}
