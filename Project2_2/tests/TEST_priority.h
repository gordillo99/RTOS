//Tests whether tasks with highest priority run first when created
#ifndef INCFILE2_H_
#define INCFILE2_H_

#include "../uart.h"
#include "../os.h"

extern void Test_System_Task();
extern void Test_RR_Task();
extern void Test_Periodic_Task();

extern void test_priority(void);

#endif /* INCFILE1_H_ */
