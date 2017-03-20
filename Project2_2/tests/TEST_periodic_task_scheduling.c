//Tests whether tasks with periodic tasks run in proper order after creation
#include "TEST_periodic_task_scheduling.h"

static char trace[30] = "";
static char temp[5] = "";

static void Periodic_Task(){
	itoa(Task_GetArg(), temp, 10);
	add_to_trace(trace, temp);
	Task_Next();
	itoa(Task_GetArg(), temp, 10);
	add_to_trace(trace, temp);
}

static void error_check(){
	if(strcmp(trace, "0123456701234567") == 0)
		uart_write("test_periodic_task_scheduling PASSED");
	else
		uart_write(trace);
}

void test_periodic_task_scheduling(void){
	uart_start();
	Task_Create_Period(Periodic_Task, 0, 10, 1, 1);
	Task_Create_Period(Periodic_Task, 1, 10, 1, 2);
	Task_Create_Period(Periodic_Task, 2, 10, 1, 3);
	Task_Create_Period(Periodic_Task, 3, 10, 1, 4);
	Task_Create_Period(Periodic_Task, 4, 10, 1, 5);
	Task_Create_Period(Periodic_Task, 5, 10, 1, 6);
	Task_Create_Period(Periodic_Task, 6, 10, 1, 7);
	Task_Create_Period(Periodic_Task, 7, 10, 1, 8);
	Task_Create_Period(error_check, 0, 21, 1, 20);
}
