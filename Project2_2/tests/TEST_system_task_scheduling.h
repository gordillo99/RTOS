//Tests whether tasks with highest SYSTEM priority run from beginning to end without interruption
#ifndef INCFILE3_H_
#define INCFILE3_H_

#include "../uart.h"
#include "../os.h"
#include "TEST_priority.h"

extern void Test_Task2(void);

extern void test_system_task_scheduling(void);

#endif /* INCFILE3_H_ */
