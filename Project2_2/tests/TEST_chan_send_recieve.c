//Tests whether sending and recieving on a channel works
#include "TEST_chan_send_recieve.h"

static char trace[30] = "";
static char temp[5] = "";
static CHAN c = -1;
static int test;
static int sent_value;

static void System_Task_1(){
	itoa(Task_GetArg(), temp, 10);
	add_to_trace(trace, temp);
	if(test == 0)
		c = Chan_Init();
	sent_value = Task_GetArg();
	Send(c, Task_GetArg());
	itoa(Task_GetArg(), temp, 10);
	add_to_trace(trace, temp);
}

static void System_Task_2(){
	itoa(Task_GetArg(), temp, 10);
	add_to_trace(trace, temp);
	if(test == 1 || test == 2 || (test == 3 && c == -1) || (test == 4 && c == -1))
		c = Chan_Init();
	int k = Recv(c);
	if(k == sent_value){
		itoa(Task_GetArg(), temp, 10);
		add_to_trace(trace, temp);
	}
}

static void Periodic_Task(){
	itoa(Task_GetArg(), temp, 10);
	add_to_trace(trace, temp);
	sent_value = Task_GetArg();
	Write(c, Task_GetArg());
	itoa(Task_GetArg(), temp, 10);
	add_to_trace(trace, temp);
}

static void RR_Task_1(){
	itoa(Task_GetArg(), temp, 10);
	add_to_trace(trace, temp);
	sent_value = Task_GetArg();
	Send(c, Task_GetArg());
	itoa(Task_GetArg(), temp, 10);
	add_to_trace(trace, temp);
}

static void RR_Task_2(){
	itoa(Task_GetArg(), temp, 10);
	add_to_trace(trace, temp);
	if(test == 1 || test == 2 || (test == 3 && c == -1))
		c = Chan_Init();
	int k = Recv(c);
	if(k == sent_value){
		itoa(Task_GetArg(), temp, 10);
		add_to_trace(trace, temp);
	}
}

static void error_check(){
	switch(test){
		case 0:
			if(strcmp(trace, "1221") == 0)
				uart_write("test_chan_send_recieve PASSED");
			else 
				uart_write("test_chan_send_recieve FAILED");
			break;
		case 1:
			if(strcmp(trace, "3443") == 0)
				uart_write("test_chan_send_recieve PASSED");
			else
				uart_write("test_chan_send_recieve FAILED");
			break;
		case 2:
			if(strcmp(trace, "5665") == 0)
				uart_write("test_chan_send_recieve PASSED");
			else
				uart_write("test_chan_send_recieve FAILED");
			break;
		case 3:
			if(strcmp(trace, "67899678") == 0)
				uart_write("test_chan_send_recieve PASSED");
			else
				uart_write("test_chan_send_recieve FAILED");
			break;
		default:
			if(strcmp(trace, "10111213141410111213") == 0)
				uart_write("test_chan_send_recieve PASSED");
			else
				uart_write("test_chan_send_recieve FAILED");
			break;
	}
}

void test_chan_send_recieve(void){
	uart_start();
	test = Task_GetArg();
	switch(test){
		case 0: //One system task sends a value on a channel then waits. Another receives the value and both continue;
			Task_Create_System(System_Task_1, 1);
			Task_Create_System(System_Task_2, 2);
			Task_Create_RR(error_check, 0);
			break;
		case 1:	//One system task receives on a channel then waits. Another sends on the channel and both continue;
			Task_Create_System(System_Task_2, 3);
			Task_Create_System(System_Task_1, 4);
			Task_Create_RR(error_check, 0);
			break;
		case 2:	//One system task receives on a channel then waits. A RR sends on the channel and both continue;
			Task_Create_System(System_Task_2, 5);
			Task_Create_RR(RR_Task_1, 6);
			Task_Create_RR(error_check, 0);
			break;
		case 3:	//Multiple system tasks receive on a channel then wait. A system task sends on a channel and all continue;
			Task_Create_System(System_Task_2, 6);
			Task_Create_System(System_Task_2, 7);
			Task_Create_System(System_Task_2, 8);
			Task_Create_System(System_Task_1, 9);
			Task_Create_RR(error_check, 0);
			break;
		default: //Create two system tasks and two RR tasks. Make them wait on a channel. Create a periodic task to wake all of them up with write();
			Task_Create_System(System_Task_2, 10);
			Task_Create_System(System_Task_2, 11);
			Task_Create_RR(RR_Task_2, 12);
			Task_Create_RR(RR_Task_2, 13);
			Task_Create_Period(Periodic_Task, 14, 5, 2, 3);
			Task_Create_Period(error_check, 0, 10, 1, 9);
			break;
	}
	
}
