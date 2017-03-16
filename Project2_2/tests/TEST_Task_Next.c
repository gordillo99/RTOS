//Tests whether Task_Next() works for all priorities
#include "TEST_Task_Next.h"

static char trace[30] = "";
static char temp[5] = "";

static int rr_num = 0;

static void Periodic_Task();
static void error_check();

static void RR_Task(){
	itoa(Task_GetArg(), temp, 10);
	add_to_trace(trace, temp);
	if(rr_num == 0){
		Task_Create_Period(Periodic_Task, 4, 8, 1, 2);
		Task_Create_Period(Periodic_Task, 5, 8, 1, 4);
		Task_Create_Period(Periodic_Task, 6, 8, 1, 6);
		rr_num++;
		int init_time = Now();
		while(Now() < init_time+160);
		itoa(Task_GetArg(), temp, 10);
		add_to_trace(trace, temp);
		Task_Create_RR(RR_Task, 7);
		Task_Create_RR(RR_Task, 8);
		Task_Create_Period(error_check, 0, 8, 1, 6);
		Task_Terminate();
	}
	Task_Next();
	itoa(Task_GetArg(), temp, 10);
	add_to_trace(trace, temp);
}

static int cnt = 0;

static void System_Task(){
	if(cnt == 0){
		cnt++;
		Task_Next();
	}
	itoa(Task_GetArg(), temp, 10);
	add_to_trace(trace, temp);
	if(cnt == 1){
		cnt++;
		Task_Next();
	}
	itoa(Task_GetArg(), temp, 10);
	add_to_trace(trace, temp);
}

static void Periodic_Task(){
	itoa(Task_GetArg(), temp, 10);
	add_to_trace(trace, temp);
	Task_Next();
	itoa(Task_GetArg(), temp, 10);
	add_to_trace(trace, temp);
}

static void error_check(){
	if(strcmp(trace, "122001345645637878") == 0)
		uart_write("test_Task_Next PASSED");
	else
		uart_write(trace);
}

void test_Task_Next(void){
	uart_start();
	Task_Create_System(System_Task, 0);
	Task_Create_System(System_Task, 1);
	Task_Create_System(System_Task, 2);
	Task_Create_RR(RR_Task, 3);
}