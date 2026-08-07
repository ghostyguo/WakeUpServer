/*
 *    Module: Modbus Slave   
 *    Author: Jinn-Kwei Guo
 *      Date: 2015/10/29
 */

#ifndef RTOS_h
#define RTOS_h

typedef enum  {RUNNING, SUSPEND} TaskState;
typedef enum  {RoundRobin, PriorityQueue} SwitchingAlgorithm;


#define   RTOS_VERSION    "0.1"
#define   MaxTaskNumber   3
#define   DefaultSwitchingAlgorithm    PriorityQueue

class Task {
    public:  
        long tickInterval;
        int elapsedTick;
        long executionTick;
        long maxExecutionTick=0;        
        long maxDelayTick=0;
        unsigned long startTick;
        TaskState runningState;
        char *name; 

        Task(); //Constructor
        void (*entry)();
        Task* elapsedShift(long tickShift);
        Task* setState(TaskState taskState);
        Task* suspend();
        Task* run();
        void report();
};

class TaskManager {
    public:
        TaskManager(); //constructor
        void init();
        void run();
        Task* addTask(void (*taskEntry)(), char *taskName, unsigned int tickInterval = 1000, TaskState state = RUNNING, int startDelay=0);
        void debug();
        void activeTaskReport();
        void taskListReport();
        
    private:
        Task taskQueue[MaxTaskNumber];
        unsigned long lastMillis;   
        unsigned long switchCount=0;
        int activeTaskIndex=-1;
        int numberOfTask=0;

        void TaskSwitching(int algorithm=DefaultSwitchingAlgorithm);
        void PrepareTask();
        void UpdateTaskStat();
};

class Process : Task {
    // Not implemented yet   
    public:
        Process();   //constructor
};

class Thread : Task {
    // Not implemented yet   
    public:
        Task *parent;        
        Thread();   //constructor
};

class PreemptiveOS {
    // Other features, blocked I/O, semaphore, 
    // Not implemented yet   
    public:
        TaskManager taskManager;
        
        PreemptiveOS();   //constructor
        void init();
        void run();
        void shutdown();
        
    private:        
};

extern PreemptiveOS RTOS;

#endif //RTOS_h
//
// END OF FILE
//



