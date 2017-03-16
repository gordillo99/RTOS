//Tests whether tasks with highest priority run first when created
#include "TEST_priority.h"

static char trace[30] = "";
static char temp[5] = "";

void add_to_trace(char* a_string, char* addition){
	Disable_Interrupt();
	strcat(a_string, addition);
	Enable_Interrupt();
}

void Test_System_Task(){
	itoa(Task_GetArg(), temp, 10);
	add_to_trace(trace, temp);
	add_to_trace(trace, temp);
}

void Test_RR_Task(){
	itoa(Task_GetArg(), temp, 10);
	add_to_trace(trace, temp);
	
	Task_Create_System(Test_System_Task, 3);
	Task_Create_Period(Test_Periodic_Task, 4, 2, 1, 0);
	
	itoa(Task_GetArg(), temp, 10);
	add_to_trace(trace, temp);
	if(strcmp(trace, "0011233442") == 0)
		uart_write("test_priority PASSED");
	else
		uart_write("test_priority FAILED");
}

void Test_Periodic_Task(){
	itoa(Task_GetArg(), temp, 10);
	add_to_trace(trace, temp);
	add_to_trace(trace, temp);
}

void test_priority(void){
	uart_start();
	Task_Create_Period(Test_Periodic_Task, 1, 2, 1, 0);
	Task_Create_System(Test_System_Task, 0);
	Task_Create_RR(Test_RR_Task, 2);
}