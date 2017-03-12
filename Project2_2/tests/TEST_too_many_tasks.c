#include "TEST_too_many_tasks.h"

void Test_Task(){
	Task_Terminate();
}

void test_too_many_tasks(void){
	uart_start();
	int x; 
	PID k;
	char my_str[50] = "Tasks Created: ";
	char temp[5];
	for(x = 0; x < MAXTHREAD*2 ; x++){
		k = Task_Create_System(Test_Task, x);
		sprintf(temp, "%d", x+1);
		strcat(my_str, temp);
		if(k != NULL)
			uart_write(my_str);
		my_str[15] = '\0';
		if(x >= MAXTHREAD && k != NULL){
			uart_write("Too many tasks created!");
			uart_write("test_too_many_tasks                        FAILED");
			Task_Terminate();
		}
	}
	uart_write("test_too_many_tasks                        PASSED");
	Task_Terminate();
}