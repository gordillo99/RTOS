#ifndef _OS_H_
#define _OS_H_
   
#define MAXTHREAD     16
#define WORKSPACE     256   // in bytes, per THREAD
#define MAXCHAN       16
#define MSECPERTICK   10   // resolution of a system TICK in milliseconds
#define MINPRIORITY   10   /** 0 is the highest priority, 10 the lowest */

#define SYSTEM 3
#define PERIODIC 2
#define RR 1
#define IDLE 4

#ifndef NULL
#define NULL          0   /* undefined */
#endif
#define TRUE          1
#define FALSE         0

#define Disable_Interrupt()     asm volatile ("cli"::)
#define Enable_Interrupt()      asm volatile ("sei"::)

typedef void (*voidfuncptr) (void);      /** pointer to void f(void) */

typedef unsigned int PRIORITY;

typedef unsigned int PID;        // always non-zero if it is valid
typedef unsigned int CHAN;       // always non-zero if it is valid
typedef unsigned int TICK;       // 1 TICK is defined by MSECPERTICK
typedef unsigned int BOOL;       // TRUE or FALSE


/**
  *  This is the set of states that a task can be in at any given time.
  */
typedef enum process_states {
    DEAD = 0,
    READY,
    RUNNING,
    BLOCKED,
    TERMINATED
} PROCESS_STATES;

/**
  * This is the set of kernel requests
  */
typedef enum kernel_request_type {
    NONE = 0,
    CREATE,
	CREATE_SYS,
	CREATE_RR,
	CREATE_PERIODIC,
    NEXT,
    TERMINATE,
	SEND,
	ASYNC_SEND,
	RECEIVE,
} KERNEL_REQUEST_TYPE;

typedef enum channel_states {
	UNITIALIZED,
	BLOCKING,
	USED,
} CHANNEL_STATES;

/**
  * Each task is represented by a process descriptor, which contains all
  * relevant information about this task. For convenience, we also store
  * the task's stack, i.e., its workspace, in here.
  */
typedef struct ProcessDescriptor {
    PID processID;
    unsigned char *sp;   /* stack pointer into the "workSpace" */
    unsigned char workSpace[WORKSPACE]; 
    PROCESS_STATES state;
    PRIORITY priority;
    int arg;
    voidfuncptr  code;   /* function to be executed as a task */
    KERNEL_REQUEST_TYPE request;
    unsigned int response;
    TICK wakeTickOverflow;
	TICK period, wcet, offset;
	int countdown;
	unsigned int runningTime;
	int retval;
	CHAN senderChannel;
	CHAN receiverChannel;
	int val;
} PD;

typedef struct ChannelDescriptor {
	CHAN channelID;
	PD * sender;
	PD * receivers[MAXTHREAD];
	unsigned int numberReceivers;
	CHANNEL_STATES state;
	unsigned int val;
} CD;

// void OS_Init(void);      redefined as main()
void OS_Abort(void);

PID  Task_Create( void (*f)(void), PRIORITY py, int arg,  int offset,  int wcet,  int period);
void Task_Terminate(void);
void Task_Next(void); // Same as yield
int  Task_GetArg( PID p );
void Task_Suspend( PID p );          
void Task_Resume( PID p );

void Task_Sleep(TICK t);  // sleep time is at least t*MSECPERTICK

/*
 * Scheduling Policy:
 * There are three priority levels:
 *   HIGHEST  -- System tasks,
 *   MEDIUM   -- Periodic tasks,
 *   LOWEST   -- Round-Robin (RR) tasks
 * A ready System task preempts all other lower priority running tasks. Preemption occurs
 * immediately, not until the next TICK.
 * A ready Periodic task preempts all other lower priority running tasks.
 * Periodic tasks must be scheduled conflict-free, i.e., no two periodic tasks should be
 * ready at the same time. It is the application engineer's responsibility to ensure that
 * it is the case. When a timing violation occurs, the RTOS may abort.
 * When a Periodic task is preempted, it is put on hold until all higher priority tasks
 * are no longer ready. However, when it is resumed later, a timing violation occurs if
 * another Periodic becomes ready, i.e., there is a timing conflict. The RTOS may abort.
 * System and RR tasks are first-come-first-served. They run until they terminate, block
 * or yield.
 */

PID   Task_Create_System(void (*f)(void), int arg);
PID   Task_Create_RR(    void (*f)(void), int arg);

 /**
   * f a parameterless function to be created as a process instance
   * arg an integer argument to be assigned to this process instanace
   * period its execution period in multiples of TICKs
   * wcet its worst-case execution time in TICKs, must be less than "period"
   * offset its start time in TICKs
   * returns 0 if not successful; otherwise a non-zero PID.
   */
PID   Task_Create_Period(void (*f)(void), int arg, TICK period, TICK wcet, TICK offset);

// NOTE: When a task function returns, it terminates automatically!!

// When a Periodic ask calls Task_Next(), it will resume at the beginning of its next period.
// When a RR or System task calls Task_Next(), it voluntarily gives up execution and 
// re-enters the ready state. All RR and Systems tasks are first-come-first-served.
//   
void Task_Next(void);

/*
 * A CHAN is a one-way communication channel between at least two tasks. It must be
 * initialized before its use. Chan_Init() returns a CHAN if successful; otherwise
 * it returns NULL.
 */

CHAN Chan_Init();

/*
 * A CHAN has no buffer. It is a mechanism allowing a sender and one or more receiver
 * to synchronize on communication. A sender calling Send() must block if there are no
 * receivers waiting; one or more receiver calling Recv() must block if there is no
 * sender waiting. When a sender and one or more receiver are ready to communicate,
 * the value "v" from the sender is returned to each waiting receiver. That is, the
 * communication occurs when both sender and the receiver(s) are ready.
 * Thus, communication is synchronous. Mutliple receivers can receive from the same 
 * CHAN. When a sender is ready to communicate with multiple receivers, all receivers
 * will receive the same value at the same time, i.e., a Send() is a multi-cast operation
 * when multiple receivers are waiting. However, when a sender is waiting, then the next
 * receiver will communicate with the waiting sender only.
 *
 * It is an error if multiple senders send on the same CHAN. 
 * As a result, the RTOS may abort when this occurs.
 * Periodic tasks are NOT allowed to use blocking Send() or Recv().
 * 
 */
void Send( CHAN ch, int v );  // blocking send on CHAN
int Recv( CHAN ch );          // blocking receive on CHAN

/*
 * A sender may not be willing to wait for one or more receiver to communicate.
 * A sender calling Write() on a CHAN will resume one or more receiver if they are waiting,
 * and as a result, they all receive the same value "v". However, if there are no receivers
 * waiting, then the sender continues without blocking. This allows a sender to communicate
 * with multiple receivers without blocking. Thus, the data value "v" may be lost forever.
 * A Write() is essentially the same as Send() except that the sender is not blocking.
 * Periodic tasks may use Write() to communicate with other tasks.
 *
 * Note: It is possible that an ISR may use Write() to resume a waiting receiving task.
 */
void Write( CHAN ch, int v );   // non-blocking send on CHAN


/**  
  * Returns the number of milliseconds since OS_Init(). Note that this number
  * wraps around after it overflows as an unsigned integer. The arithmetic
  * of 2's complement will take care of this wrap-around behaviour if you use
  * this number correctly.
  * Let  T = Now() and we want to know when Now() reaches T+1000.
  * Now() is always increasing. Even if Now() wraps around, (Now() - T) always
  * >= 0. As long as the duration of interest is less than the wrap-around time,
  * then (Now() - T >= 1000) would mean we have reached T+1000.
  * However, we cannot compare Now() against T directly due to this wrap-around
  * behaviour.
  * Now() will wrap around every 65536 milliseconds. Therefore, for measurement
  * purposes, it should be used for durations less than 65 seconds.
  */
unsigned int Now();  // number of milliseconds since the RTOS boots.


/**
 * Booting:
 *  The RTOS and the main application are compiled into a single executable binary, but
 *  otherwise they are totally independent.
 *  There is a single "main()" function, where the combined executable starts. This "main()"
 *  is implemented inside the RTOS, which initializes the RTOS and creates a first application
 *  System task "a_main()".
 *
 *  "a_main()" is a parameterless function defined by the application, which will create all other
 *  application tasks as necessary.
 */

#endif /* _OS_H_ */
