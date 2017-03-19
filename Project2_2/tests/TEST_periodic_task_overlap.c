//Tests whether a periodic task overlap causes the OS to abort
#include "TEST_periodic_task_overlap.h"

static char trace[30] = "";
static char temp[5] = "";

static void Periodic_Task_wcet_1(){
	int curret_time = Now();
	while(curret_time+9 > Now());
}

static void Periodic_Task_wcet_2(){
	int curret_time = Now();
	while(curret_time+18 > Now());
}

static void error_check(){
		uart_write("test_periodic_task_overlap FAILED");
}

void test_periodic_task_overlap(void){
	uart_start();
	int test_case = Task_GetArg();
	switch(test_case){
		case 0:
			Task_Create_Period(Periodic_Task_wcet_1, 0, 1, 1, 0);
			Task_Create_Period(Periodic_Task_wcet_1, 0, 1, 1, 0);
			break;
		case 1:
			Task_Create_Period(Periodic_Task_wcet_1, 0, 2, 1, 0);
			Task_Create_Period(Periodic_Task_wcet_1, 0, 1, 1, 0);
			break;
		case 2:
			Task_Create_Period(Periodic_Task_wcet_2, 0, 3, 2, 0);
			Task_Create_Period(Periodic_Task_wcet_1, 0, 2, 1, 1);
			break;
		case 3:
			Task_Create_Period(Periodic_Task_wcet_2, 0, 3, 2, 0);
			Task_Create_Period(Periodic_Task_wcet_1, 0, 3, 1, 1);
			break;
		case 4:
			Task_Create_Period(Periodic_Task_wcet_2, 0, 3, 4, 0);
			break;
		default:
			Task_Terminate();
	}
	Task_Create_Period(error_check, 0, 5, 1, 4);
}

//test two periodic at same time;