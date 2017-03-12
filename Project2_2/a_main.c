#include <avr/io.h>
#include <util/delay.h>
#include "os.h"
#include "./tests/TEST_too_many_tasks.h"

/***** Core System Tasks *****/
void ASDF1()
{
	for (;;) {
		//pin 25
		PORTA |= (1<<PA3);
		PORTA &= ~(1<<PA3);
	}
}

void ASDF2()
{
	for (;;) {
		//pin 26
		PORTA |= (1<<PA4);
		PORTA &= ~(1<<PA4);
	}
	
}

void ASDF3()
{
	for (;;) {
		//pin 27
		PORTA |= (1<<PA5);
		PORTA &= ~(1<<PA5);
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
	for(;;)
	{
		PORTA |= (1<<PA7);
		PORTA &= ~(1<<PA7);
	}
}

void a_main()
{
	
    // Initialize tasks
	//Task_Create(ASDF, SYSTEM, 0);
    //Task_Create(ASDF1, PERIODIC, 0, 0, 2, 10);
    //Task_Create(ASDF2, PERIODIC, 0, 4, 1, 10);
	//Task_Create(ASDF3, PERIODIC, 0, 8, 1, 10);
	//Task_Create(ASDF4, PERIODIC, 0, -1, -1, -1);
	//Task_Create(ASDF5, RR, 0, -1, -1, -1);
	Task_Create_System(test_too_many_tasks,0);
    // Kill the initialization task
    Task_Terminate();
}
