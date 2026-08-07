/* 
    WakeUpServer: 定時/遠端喚醒電腦

    by ghostyguo@gmail.com,  2027/07/26
*/
#include <string.h>
#include "NetDef.h"
#include "WebServer.h"
#include "LocalTime.h"
#include "WakeOnLan.h"
#include "WakeupServer.h"
#include "RTOS.h"

#define DEBUG_LEVEL 1


int NumberOfSite;
static Computer SiteInfo[] = {
    //{      "J1900", {0x40,0x16,0x7E,0x2A,0x4E,0xFC}, {21, 30}},
    {"MoneyStudio", {0x7C,0x10,0xC9,0xBA,0xA8,0x75}, { 7, 30}, false},
    {    "Sabre15", {0x80,0xFA,0x5B,0x58,0xFF,0xC9}, { 7, 30}, false}
};


// forward definition required
void WebServerTask();
void LocalTimeTask();
void CheckWakeUpTask();

Task *pWebServerTask;
Task *pLocalTimeTask;
Task *pCheckWakeUpTask;

void setup () 
{
    NumberOfSite = sizeof(SiteInfo)/sizeof(Computer); //each contains 6 bytes of MAC address
   
    Serial.begin(9600); //debug port, init first
    Serial.println(F("--- Wackup Server ---"));

    Net_Init(); //所有網路共用的Init
    WakeOnLan_Init();
    LocalTime_Init();
    WebServer_Init();
    delay(1000);

    // Add tasks to RTOS
    pWebServerTask = RTOS.taskManager.addTask(WebServerTask, "WebServerTask", 100); 
    pLocalTimeTask = RTOS.taskManager.addTask(LocalTimeTask, "LocalTimeTask", 1000); 
    pCheckWakeUpTask= RTOS.taskManager.addTask(CheckWakeUpTask, "CheckWakeUpTask", 1000);
    RTOS.init();

}
 
void loop() 
{
    RTOS.run();   
}


void WebServerTask()
{  
    WebServer_Loop();
}

void LocalTimeTask()
{
    LocalTime_Loop();
}

void CheckWakeUpTask() 
{   
    #if (DEBUG_LEVEL>0)    
    Serial.print("Local Time = ");
    Serial.println(LocalTime_GetDateTimeString());
    #endif

    int wackupCount = 0;
    for (int i=0; i<NumberOfSite; i++)
    {
        #if (DEBUG_LEVEL>1)
        Serial.print(" Check#");
        Serial.print(i);
        Serial.print(" -> ");
        Serial.print(SiteInfo[i].WakeUp.Hour);
        Serial.print(":");
        Serial.println(SiteInfo[i].WakeUp.Minute);
        #endif

        if ((LocalTime_GetHour()== SiteInfo[i].WakeUp.Hour) && 
            (LocalTime_GetMinute()== SiteInfo[i].WakeUp.Minute))
        { 
            if (!SiteInfo[i].isWakeUping)
            {
                SiteInfo[i].isWakeUping = true;
                #if (DEBUG_LEVEL>1)
                Serial.print("***Call WOL ");
                Serial.println(SiteInfo[i].Name);
                #endif
                
                SendWolPacket(SiteInfo[i].IP);
                wackupCount++;
            }
            else
            {
                //已經喚醒過了
            }
        }
        else 
        {            
            SiteInfo[i].isWakeUping = false; //不在同一分鐘就清除
        }
    }
    delay(1000);
}

