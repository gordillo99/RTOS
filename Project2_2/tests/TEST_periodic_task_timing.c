//Tests whether a periodic task is executed on time
#include "TEST_periodic_task_timing.h"

static char trace[30] = "";
static char temp[5] = "";

static void Periodic_Task(){
	int curret_time = Now();
	itoa(curret_time, temp, 10);
	add_to_trace(trace, temp);
	Task_Next();
	curret_time = Now();
	itoa(curret_time, temp, 10);
	add_to_trace(trace, temp);
	Task_Terminate();
}

static void error_check(){
	int curret_time = Now();
	itoa(curret_time, temp, 10);
	add_to_trace(trace, temp);
	if(strcmp(trace, "60708090140150240250320") == 0)
		uart_write("test_periodic_task_timing PASSED");
	else
		uart_write("test_periodic_task_overlap FAILED");
	Task_Terminate();
}

void test_periodic_task_timing(void){
	uart_start();
	Task_Create_Period(Periodic_Task, 0, 16, 1, 8);
	Task_Create_Period(Periodic_Task, 0, 16, 1, 9);
	Task_Create_Period(Periodic_Task, 0, 8, 1, 6);
	Task_Create_Period(Periodic_Task, 0, 8, 1, 7);
	Task_Create_Period(error_check, 0, 33, 1, 32);
}
