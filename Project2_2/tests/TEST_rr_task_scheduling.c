//Tests whether tasks with RR priority run in proper order after creation
#include "TEST_rr_task_scheduling.h"

static char trace[30] = "";
static char temp[5] = "";

static void RR_Task(){
	itoa(Task_GetArg(), temp, 10);
	add_to_trace(trace, temp);
	add_to_trace(trace, temp);
}

static void error_check(){
	if(strcmp(trace, "0011223344") == 0)
		uart_write("test_rr_task_scheduling PASSED");
	else
		uart_write("test_rr_task_scheduling FAILED");
}

void test_rr_task_scheduling(void){
	uart_start();
	Task_Create_RR(RR_Task, 0);
	Task_Create_RR(RR_Task, 1);
	Task_Create_RR(RR_Task, 2);
	Task_Create_RR(RR_Task, 3);
	Task_Create_RR(RR_Task, 4);
	Task_Create_RR(error_check, 0);
}
