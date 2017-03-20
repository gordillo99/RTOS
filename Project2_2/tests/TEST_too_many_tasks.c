//Test whether too many tasks can be created;
#include "TEST_too_many_tasks.h"

void Test_Task(){
	Task_Terminate();
}

void test_too_many_tasks(void){
	uart_start();
	int x; 
	PID k;
	for(x = 0; x < MAXTHREAD*2 ; x++){
		k = Task_Create_System(Test_Task, x);
		if(x >= MAXTHREAD && k != NULL){
			uart_write("Too many tasks created!");
			uart_write("test_too_many_tasks FAILED");
			Task_Terminate();
		}
	}
	Task_Next();
	for(x = 0; x < MAXTHREAD*2 ; x++){
		k = Task_Create_System(Test_Task, x);
		if(x >= MAXTHREAD && k != NULL){
			uart_write("Too many tasks created!");
			uart_write("test_too_many_tasks FAILED");
			Task_Terminate();
		}
	}
	Task_Next();
	for(x = 0; x < MAXTHREAD*2 ; x++){
		if(x%2 == 0)
			k = Task_Create_Period(Test_Task, x, 15, 1, x);
		else
			k = Task_Create_RR(Test_Task, x);
		if(x >= MAXTHREAD && k != NULL){
			uart_write("Too many tasks created!");
			uart_write("test_too_many_tasks FAILED");
			Task_Terminate();
		}
	}
	uart_write("test_too_many_tasks PASSED");
}