/*
 *    FILE: RTOS.cpp
 *  AUTHOR: Ghosty Guo
 */
#include <Arduino.h>
#include "RTOS.h"

// extern
PreemptiveOS RTOS;

/*
 * Task Class
 */
Task::Task()
{
    
}

Task* Task::run()
{
    setState(RUNNING);            
    return this;
}

Task* Task::setState(TaskState taskState)
{
    runningState = taskState;
    executionTick = 0;
    elapsedTick = 0; 
}
 
#if defined(EXTRA_SUPPORT)

Task* Task::elapsedShift(long tickShift)
{
    elapsedTick += tickShift;        
    return this;
}

Task* Task::suspend() 
{
    setState(SUSPEND);      
    return this;
}
#endif

// ---------- Debug fohctions --------
#if (DEBUG_LEVEL>0)

void Task::report()
{  
    Serial.print(F(" Name='"));
    Serial.print(name);     
    Serial.print(F("', Interval=")); 
    Serial.print(tickInterval);
    Serial.print(F("', Start=")); 
    Serial.print(startTick);
    Serial.print(F(", Execution="));
    Serial.print(executionTick);  
    Serial.print(F(", MaxExecution="));
    Serial.print(maxExecutionTick);  
    Serial.print(F(", Elapsed="));
    Serial.print(elapsedTick); 
    Serial.print(F(", maxDelayTick="));
    Serial.print(maxDelayTick); 
    Serial.print(F(", State="));
    switch (runningState) {
        case 0: Serial.println(F("RUNNING")); break;
        case 1: Serial.println(F("SUSPEND")); break;
        default: Serial.println(F("UNDEFINED")); break;
    }
}
#endif // ---------- Debug fohctions --------

/*
 *  TaskManager Class
 */
TaskManager::TaskManager()
{
}

void TaskManager::init()
{    
    lastMillis = millis();
}

void TaskManager::PrepareTask()
{
    long delayTick = taskQueue[activeTaskIndex].elapsedTick - taskQueue[activeTaskIndex].tickInterval;
    if (delayTick > taskQueue[activeTaskIndex].maxDelayTick) {
        taskQueue[activeTaskIndex].maxDelayTick = delayTick; // keep the max record
    }                
    taskQueue[activeTaskIndex].startTick=millis(); 
}

void TaskManager::UpdateTaskStat()
{
    taskQueue[activeTaskIndex].executionTick = millis()-taskQueue[activeTaskIndex].startTick;
    if (taskQueue[activeTaskIndex].executionTick > taskQueue[activeTaskIndex].maxExecutionTick) {
        taskQueue[activeTaskIndex].maxExecutionTick = taskQueue[activeTaskIndex].executionTick; // keep the max record
    }
    
}
        
void TaskManager::TaskSwitching(int algorithm)
{  
    for (int i=0; i<numberOfTask; i++) {
        if (taskQueue[i].runningState==RUNNING) {
            if (taskQueue[i].elapsedTick >= taskQueue[i].tickInterval) {            
                #if (DEBUG_LEVEL>0)
                Serial.print(F("RTOS run task # "));
                Serial.println(i);
                #endif
                activeTaskIndex = i;
                PrepareTask();          
                taskQueue[activeTaskIndex].entry();  //call the task entry                 
                UpdateTaskStat();                               
                 
                taskQueue[activeTaskIndex].elapsedTick =0;
                //taskQueue[activeTaskIndex].elapsedTick -= taskQueue[activeTaskIndex].tickInterval;   

                if (algorithm==PriorityQueue) {
                    break;  // exit for loop to restart the queue
                }
            } else {
                activeTaskIndex = -1;
            }
        } 
        else if (taskQueue[i].runningState==SUSPEND) {
            // Suspended Task
        }
    }
}

void TaskManager::run()
{  
    unsigned long diffMillis = millis()-lastMillis;
    lastMillis = millis();
    switchCount++;
    
    for (int i=0; i<numberOfTask; i++) {
        if (taskQueue[i].runningState==RUNNING) {
            taskQueue[i].elapsedTick += diffMillis;
        } 
        else if (taskQueue[i].runningState==SUSPEND) {
            // Suspended Task
        }
    }
    TaskSwitching();
}

Task* TaskManager::addTask(void (*taskEntry)(), char *taskName, unsigned int tickInterval, TaskState state, int startDelay)
{
    if (numberOfTask<MaxTaskNumber) {
      taskQueue[numberOfTask].entry = taskEntry;
      taskQueue[numberOfTask].name = taskName;
      taskQueue[numberOfTask].tickInterval = tickInterval;
      taskQueue[numberOfTask].runningState = state;;
      taskQueue[numberOfTask].elapsedTick = startDelay;
      taskQueue[numberOfTask].executionTick = 0;
      numberOfTask++;
    } else {
        // Too Many Tasks
        Serial.print(F("RTOS : Too Many Tasks"));
        RTOS.shutdown();
    }
    return (&taskQueue[numberOfTask-1]); //TaskID
}

// ---------- Debug fohctions --------
#if (DEBUG_LEVEL>0)
void TaskManager::taskListReport()
{
    Serial.print(F("*** TaskListReport : SwitchCount = "));
    Serial.print(switchCount);
    Serial.print(F(", NumberOfTask="));
    Serial.print(numberOfTask);    
    Serial.print(F(", ActiveTask#"));
    Serial.println(activeTaskIndex);
    for (int i=0; i<numberOfTask; i++) {      
        Serial.print(F("TASK # "));
        Serial.print(i);    
        taskQueue[i].report();
    }
}

void TaskManager::activeTaskReport()
{
    if (activeTaskIndex>=0) {      
        UpdateTaskStat();
        Serial.print(millis());
        Serial.print(F(": *** TASK : Active Task #"));
        Serial.print(activeTaskIndex);    
        Serial.print(F(", Name='"));
        Serial.print(taskQueue[activeTaskIndex].name);     
        Serial.print(F("', Start="));
        Serial.print(taskQueue[activeTaskIndex].startTick); 
        Serial.print(F(", Execution="));
        Serial.print(taskQueue[activeTaskIndex].executionTick); 
        Serial.print(F(", maxExecution="));
        Serial.print(taskQueue[activeTaskIndex].maxExecutionTick); 
        Serial.print(F(", maxDelayTick="));
        Serial.print(taskQueue[activeTaskIndex].maxDelayTick); 
        if (taskQueue[activeTaskIndex].executionTick <= taskQueue[activeTaskIndex].tickInterval) {
            Serial.println(F(", OK"));
        } else {
            Serial.println(F(", Exhausted"));
        }
    } else {
        Serial.println(F("No Task"));
    }
}
#endif // ---------- Debug fohctions --------

/*
 *  Process Class
 */
 /*
Process::Process()
{
  
}
*/

/*
 *  Thread Class
 */
/*
Thread::Thread()
{
  
}
 */

/*
 *  PreemptiveOS Class
 */
PreemptiveOS::PreemptiveOS()
{
}

void PreemptiveOS::init()
{
    taskManager.init();    
}

void PreemptiveOS::run()
{
    taskManager.run();    
}

void PreemptiveOS::shutdown()
{
    Serial.println(F("\n\n*** RTOS shutdown"));
    while(1) delay(100);   
}



