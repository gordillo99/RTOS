#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "os.h"
#include "queue.h"

extern void a_main();

/*===========
  * RTOS Internal
  *===========
  */

/**
  * This internal kernel function is the context switching mechanism.
  * It consists two halves: the top half is called "Exit_Kernel()", and the bottom half is called "Enter_Kernel()".
  * When kernel calls this function, it starts the top half (i.e., exit). Right in
  * the middle, "Cp" is activated; as a result, Cp is running and the kernel is
  * suspended in the middle of this function. When Cp makes a system call,
  * it enters the kernel via the Enter_Kernel() software interrupt into
  * the middle of this function, where the kernel was suspended.
  * After executing the bottom half, the context of Cp is saved and the context
  * of the kernel is restore. Hence, when this function returns, kernel is active
  * again, but Cp is not running any more. 
  * (See file "cswitch.S" for details.)
  */
extern void CSwitch();
extern void Exit_Kernel();    /* this is the same as CSwitch() */

/**
  * Prototypes
  */
void Task_Terminate(void);
static void Dispatch();

/** 
  * Contained in cswitch.S, context switches to the kernel
  */ 
extern void Enter_Kernel();

/**
  * This table contains ALL process descriptors. It doesn't matter what
  * state a task is in.
  */
static PD Process[MAXTHREAD];

/**
  * The process descriptor of the currently RUNNING task.
  */
volatile static PD* Cp; 

/** 
  * Since this is a "full-served" model, the kernel is executing using its own
  * stack. We can allocate a new workspace for this kernel stack, or we can
  * use the stack of the "main()" function, i.e., the initial C runtime stack.
  * (Note: This and the following stack pointers are used primarily by the
  *   context switching code, i.e., CSwitch(), which is written in assembly
  *   language.)
  */         
volatile unsigned char *KernelSp;

/**
  * This is a "shadow" copy of the stack pointer of "Cp", the currently
  * running task. During context switching, we need to save and restore
  * it into the appropriate process descriptor.
  */
volatile unsigned char *CurrentSp;

/** 1 if kernel has been started; 0 otherwise. */
volatile static unsigned int KernelActive;  

/** number of active tasks */
volatile static unsigned int Tasks; 

 /** Keeps track of the process id's. Always goes up. */
volatile static unsigned int pCount;

/** Global tick overflow count */
volatile unsigned int tickOverflowCount = 0;

/** The ReadyQueue for tasks */
volatile PD *ReadyQueue[MAXTHREAD];
volatile int RQCount = 0;

/** The SystemQueue for tasks */
volatile PD *SysQueue[MAXTHREAD];
volatile int SysCount = 0;

/** The PeriodicQueue for tasks */
volatile PD *PeriodicQueue[MAXTHREAD];
volatile int PerioicCount = 0;

/** The RRQueue for tasks */
volatile PD *RRQueue[MAXTHREAD];
volatile int RRCount = 0;

/** The WaitingQueue for tasks */
volatile PD *WaitingQueue[MAXTHREAD];
volatile int WQCount = 0;

/**
 * Sets up a task's stack with Task_Terminate() at the bottom,
 * The return address of the function
 * and dummy data to be popped off when the task first runs
 */
PID Kernel_Create_Task_At( volatile PD *p, voidfuncptr f, PRIORITY priority, int arg ) {   
	unsigned char *sp;

	sp = (unsigned char *) &(p->workSpace[WORKSPACE-1]);

	//Clear the contents of the workspace
	memset(&(p->workSpace),0,WORKSPACE);

	//Notice that we are placing the address (16-bit) of the functions
	//onto the stack in reverse byte order (least significant first, followed
	//by most significant).  This is because the "return" assembly instructions 
	//(rtn and rti) pop addresses off in BIG ENDIAN (most sig. first, least sig. 
	//second), even though the AT90 is LITTLE ENDIAN machine.

	//Store terminate at the bottom of stack to protect against stack underrun.
	*(unsigned char *)sp-- = ((unsigned int)Task_Terminate) & 0xff;
	*(unsigned char *)sp-- = (((unsigned int)Task_Terminate) >> 8) & 0xff;

	//Place return address of function at bottom of stack
	*(unsigned char *)sp-- = ((unsigned int)f) & 0xff;
	*(unsigned char *)sp-- = (((unsigned int)f) >> 8) & 0xff;
	*(unsigned char *)sp-- = 0x00; // Fix 17 bit address problem for PC

	sp = sp - 34;
	  
	p->sp = sp;     /* stack pointer into the "workSpace" */
	p->code = f;        /* function to be executed as a task */
	p->request = NONE;
	p->processID = pCount;
	p->priority = priority;
	p->arg = arg;

	Tasks++;
	pCount++;

	p->state = READY;

	if (priority == SYSTEM) {
		enqueue(&p, &SysQueue, &SysCount);
	} else if (priority == PERIODIC) {
		//enqueue(&p, &ReadyQueue, &RQCount);
	} else if (priority == RR) {
		enqueue(&p, &RRQueue, &RRCount);
	} else { // temporary code
		enqueue(&p, &ReadyQueue, &RQCount);
	}

	return p->processID;
}

/**
  *  Create a new task
  */
static PID Kernel_Create_Task(voidfuncptr f, PRIORITY priority, int arg ) {
	int x;

	if (Tasks == MAXTHREAD) return;  /* Too many task! */

	/* find a DEAD PD that we can use  */
	for (x = 0; x < MAXTHREAD; x++) {
		if (Process[x].state == DEAD) break;
	}

	unsigned int p = Kernel_Create_Task_At( &(Process[x]), f, priority, arg );

	return p;
}

/**
  *  Terminate a task
  */
static void Kernel_Terminate_Task() {
	Cp->state = DEAD;
	Cp->processID = 0;
	Tasks--;
}

/**
  * This internal kernel function is the "scheduler". It chooses the 
  * next task to run, i.e., Cp.
  */
static void Dispatch() {
	Cp = dequeue(&SysQueue, &SysCount);

	if(Cp == NULL) {
		Cp = dequeue(&RRQueue, &RRCount);
	}

	if(Cp == NULL) {
		Cp = dequeue(&ReadyQueue, &RQCount); // original code (temporary)
	}

	if (Cp == NULL) {
		OS_Abort();
	}

	CurrentSp = Cp->sp;
	Cp->state = RUNNING;
}

/**
  * This internal kernel function is the "main" driving loop of this full-served
  * model architecture. Basically, on OS_Start(), the kernel repeatedly
  * requests the next user task's next system call and then invokes the
  * corresponding kernel function on its behalf.
  *
  * This is the main loop of our kernel, called by OS_Start().
  */
static void Next_Kernel_Request() {
	Dispatch();  /* select a new task to run */

	while(1) {
		Cp->request = NONE; /* clear its request */

		/* activate this newly selected task */
		CurrentSp = Cp->sp;

		Exit_Kernel();    /* or CSwitch() */

		/* if this task makes a system call, it will return to here! */

		/* save the Cp's stack pointer */
		Cp->sp = CurrentSp;

		switch(Cp->request){
		case CREATE:
			Cp->response = Kernel_Create_Task( Cp->code, Cp->priority, Cp->arg );
			break;
		case CREATE_SYS:
			Cp->response = Kernel_Create_Task( Cp->code, SYSTEM, Cp->arg );
			break;
		case CREATE_PERIODIC:
			Cp->response = Kernel_Create_Task( Cp->code, PERIODIC, Cp->arg );
			break;
		case CREATE_RR:
			Cp->response = Kernel_Create_Task( Cp->code, RR, Cp->arg );
			break;
		case NEXT:
		case NONE:
			Cp->state = READY;
			if (Cp->priority == SYSTEM) {
				enqueue(&Cp, &SysQueue, &SysCount);
			} else if (Cp->priority == PERIODIC) {

			} else if (Cp->priority == RR) {
				enqueue(&Cp, &RRQueue, &RRCount);
			} else {
				enqueue(&Cp, &ReadyQueue, &RQCount); // original code (temporary)
			}

			Dispatch();
			break;
		case TERMINATE:
			/* deallocate all resources used by this task */
			Kernel_Terminate_Task();
			Dispatch();
			break;

		default:
			/* Houston! we have a problem! */
			break;
		}
	} 
}

/*================
  * RTOS  API  and Stubs
  *================
  */

/**
  * This function initializes the RTOS and must be called first
  */
void OS_Init() {
	int x;

	Tasks = 0;
	KernelActive = 0;
	pCount = 0;

	for (x = 0; x < MAXTHREAD; x++) {
		memset(&(Process[x]),0,sizeof(PD));
		Process[x].state = DEAD;
		Process[x].processID = 0;
	}
}

/**
  * This function starts the RTOS after creating a_main
  */
void OS_Start() {   
	if ( (! KernelActive) && (Tasks > 0)) {
		Disable_Interrupt();

		KernelActive = 1;
		Next_Kernel_Request();
		/* SHOULD NEVER GET HERE!!! */
	}
}

/**
  * Just quits
  */
void OS_Abort() {
	exit(1);
}

/**
  * Application or kernel level task create to setup system call
  */
PID Task_Create(voidfuncptr f, PRIORITY priority, int arg){
	unsigned int p;

	if (KernelActive) {
		Disable_Interrupt();
	
		if (priority == SYSTEM) {
			Cp->request = CREATE_SYS;
		} else if (priority == PERIODIC) {

		} else if (priority == RR) {
			Cp->request = CREATE_RR;
		} else {
			Cp->request = CREATE; // original code (temporary)
		}

		Cp->code = f;
		Cp->priority = priority;
		Cp->arg = arg;
		Enter_Kernel();
		p = Cp->response;
	} else { 
	  /* call the RTOS function directly */
	  p = Kernel_Create_Task( f, priority, arg );
	}
	return p;
}

/**
  * Application level task next to setup system call to give up CPU
  */
void Task_Next() {
	if (KernelActive) {
		Disable_Interrupt();
		Cp->request = NEXT;
		Enter_Kernel();
	}
}

/**
  * Application level task terminate to setup system call
  */
void Task_Terminate() {
	if (KernelActive) {
		Disable_Interrupt();
		Cp -> request = TERMINATE;
		Enter_Kernel();
		/* never returns here! */
	}
}

/**
  * Application level task getarg to return intiial arg value
  */
int Task_GetArg(PID p) {
	return (Cp->arg);
}

/**
  * Setup pins and timers
  */
void setup() {


	/** initialize Timer1 16 bit timer */
	Disable_Interrupt();

	/** Timer 1 */
	TCCR1A = 0;                 /** Set TCCR1A register to 0 */
	TCCR1B = 0;                 /** Set TCCR1B register to 0 */

	TCNT1 = 0;                  /** Initialize counter to 0 */

	OCR1A = 624;                /** Compare match register (TOP comparison value) [(16MHz/(100Hz*8)] - 1 */

	TCCR1B |= (1 << WGM12);     /** Turns on CTC mode (TOP is now OCR1A) */

	TCCR1B |= (1 << CS12);      /** Prescaler 256 */

	TIMSK1 |= (1 << OCIE1A);    /** Enable timer compare interrupt */

	/** Timer 3 */
	TCCR3A = 0;                 /** Set TCCR0A register to 0 */
	TCCR3B = 0;                 /** Set TCCR0B register to 0 */

	TCNT3 = 0;                  /** Initialize counter to 0 */

	OCR3A = 62499;              /** Compare match register (TOP comparison value) [(16MHz/(100Hz*8)] - 1 */

	TCCR3B |= (1 << WGM32);     /** Turns on CTC mode (TOP is now OCR1A) */

	TCCR3B |= (1 << CS32);      /** Prescaler 1024 */

	TIMSK3 = (1 << OCIE3A);

	Enable_Interrupt();
}

/**
  * ISR for timer1
  */
ISR(TIMER1_COMPA_vect) { 
	Task_Next();
}

/**
  * ISR for timer3
  */
ISR(TIMER3_COMPA_vect) { // PERIOD: 1 s
	tickOverflowCount += 1;
}

/**
  * This function boots the OS and creates the first task: a_main
  */
void main() {
DDRA |= (1<<PA4);
PORTA &= ~(1<<PA4);

DDRA |= (1<<PA5);
PORTA &= ~(1<<PA5);

DDRA |= (1<<PA3);
PORTA &= ~(1<<PA3);
	setup();

	OS_Init();
	Task_Create(a_main, SYSTEM, 1);
	OS_Start();
}

