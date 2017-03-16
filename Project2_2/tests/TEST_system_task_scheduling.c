//Tests whether tasks with highest SYSTEM priority run from beginning to end without interruption
#include "TEST_system_task_scheduling.h"

static char trace[30] = "";
static char temp[5] = "";

void Test_Task2(){
	int x;
	itoa(Task_GetArg(), temp, 10);
	add_to_trace(trace, temp);
	for(x = 0; x < 32000; x++){}
	for(x = 0; x < 32000; x++){}
	add_to_trace(trace, temp);
}

static void error_check(){
	if(strcmp(trace, "0011223344") == 0)
		uart_write("test_system_task_scheduling PASSED");
	else
		uart_write("test_system_task_scheduling FAILED");
}

void test_system_task_scheduling(void){
	uart_start();
	Task_Create_System(Test_Task2, 0);
	Task_Create_System(Test_Task2, 1);
	Task_Create_System(Test_Task2, 2);
	Task_Create_System(Test_Task2, 3);
	Task_Create_System(Test_Task2, 4);
	Task_Create_RR(error_check, 0);
}
