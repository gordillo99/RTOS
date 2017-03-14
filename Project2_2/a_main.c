#include <avr/io.h>
#include <util/delay.h>
#include "os.h"
#include "./tests/TEST_too_many_tasks.h"

void ASDF1();
void ASDF2();
void ASDF3();
void ASDF4();
void ASDF5();

/***** Core System Tasks *****/
void ASDF1()
{
	for (;;) {
		//pin 25
		PORTA |= (1<<PA3);
		PORTA &= ~(1<<PA3);
		Task_Next();
	}
}

void ASDF2()
{
	for (;;) {
		//pin 26
		PORTA |= (1<<PA4);
		PORTA &= ~(1<<PA4);
		Task_Next();
	}
	
}

void ASDF3()
{
	for (;;) {
		//pin 27
		PORTA |= (1<<PA5);
		PORTA &= ~(1<<PA5);
		Task_Next();
	}
}

void ASDF4()
{
	for(;;){
		//pin 28
		PORTA |= (1<<PA6);
		PORTA &= ~(1<<PA6);
	}
}

void ASDF5()
{
	Chan_Init();
	int x = 0;
	for(;;x++)
	{
		//pin 29
		PORTA |= (1<<PA7);
		PORTA &= ~(1<<PA7);
		if (x == 1000) {
			Send(1, 69);
		}
	}
}

void a_main()
{
	
    // Initialize tasks
	Task_Create_Period(ASDF1,1, 10, 9, 0);
	Task_Create_Period(ASDF2,2, 10, 9 ,1);
	Task_Create_Period(ASDF3,2, 10, 9 ,2);
	Task_Create_RR(ASDF4,5);
	Task_Create_System(ASDF5,6);
	//Task_Create_System(ASDF3,3);
	//Task_Create_RR(ASDF4,4);

    // Kill the initialization task
    Task_Terminate();
}
