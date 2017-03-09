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


void Idle() {
    for(;;) {}
}

void a_main()
{

    // Initialize tasks
    Task_Create(Task_ReadJoystick, SYSTEM, 0);
    Task_Create(Task_WriteBluetooth, 4, 0);

    //Task_Create(Idle, 10, 0);

    // Kill the initialization task
    Task_Terminate();
}
