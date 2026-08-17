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
#include "LcdManager.h"
#include "MemoryFree.h"
#include "RTOS.h"
#include <avr/pgmspace.h> // Needed for strcpy_P and PROGMEM macros

#define DEBUG_LEVEL 1

int NumberOfSite;
Computer SiteInfo[] = {
    //{      "J1900", {0x40,0x16,0x7E,0x2A,0x4E,0xFC}, {21, 30}},
    {"MoneyStudio", {0x7C,0x10,0xC9,0xBA,0xA8,0x75}, { {false, true,  true,  true,  true,  true,  false} ,7, 30}, false},
    {    "Sabre15", {0x80,0xFA,0x5B,0x58,0xFF,0xC9}, { {false, false, false, false, false, false, false} ,7, 30}, false}
};

// forward definition required
void WebServerTask();
void LocalTimeTask();
void LcdUpdateTask();
void CheckWakeUpTask();

Task *pWebServerTask;
Task *pLocalTimeTask;
Task *pLcdUpdateTask;
Task *pCheckWakeUpTask;
void (*SoftReset)(void) = 0;  

void setup () 
{
    NumberOfSite = sizeof(SiteInfo)/sizeof(Computer); //each contains 6 bytes of MAC address
   
    Serial.begin(57600); //debug port, init first, as fast as possible
    LCD_Init();    //Display output

    Serial.println(F("--- WakeUp Server ---"));
    Serial.print(F("# of Site: "));
    Serial.println(NumberOfSite);

    LCD_SetSource(LCD_SystemInit);
    LCD_Update();
    //LCD_Delay(3000);

    Net_Init(); //所有網路共用的Init
    WakeOnLan_Init();
    LocalTime_Init();
    WebServer_Init();
    //delay(2000);

    // Add tasks to RTOS
    char taskName[20]; //To save RAM
    strcpy_P(taskName, (PGM_P)F("WebServerTask"));
    pWebServerTask = RTOS.taskManager.addTask(WebServerTask, taskName, 50); //WebServer, 50ms 回應一次
    strcpy_P(taskName, (PGM_P)F("LocalTimeTask"));
    pLocalTimeTask = RTOS.taskManager.addTask(LocalTimeTask, taskName, 1000); //Local Time Sync
    strcpy_P(taskName, (PGM_P)F("LcdUpdateTask"));
    pLcdUpdateTask = RTOS.taskManager.addTask(LcdUpdateTask, taskName, 1000); //Lcd Update 
    strcpy_P(taskName, (PGM_P)F("CheckWakeUpTask"));
    pCheckWakeUpTask= RTOS.taskManager.addTask(CheckWakeUpTask, taskName, 1000); //WOL Check
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

void LcdUpdateTask()
{  
    LCD_Update(); //先顯示其他人的
    LCD_SetSource(LCD_RTOS);
}

void SelfReset()
{    
    LCD_SetSource(LCD_SystemReset);
    LCD_Update();
    LCD_Delay(5000);
    SoftReset();  
}
void LocalTimeTask()
{
    //每天凌晨 00:00:00 重新啟動, 避免:
    // 1.millis() overflow
    // 2.網卡當機
    if (LocalTime_GetHour()==0 && 
        LocalTime_GetMinute()==0 &&
        LocalTime_GetSecond()<5)
    {   
        SelfReset();
    }
    if (Ethernet.linkStatus() == LinkOFF) //not linked?
    {
        SelfReset();
    }
    if (Ethernet.localIP() == INADDR_NONE) //no IP? something wrong!
    {
        SelfReset();
    }

    LocalTime_Loop();
}

void CheckWakeUpTask() 
{   
    #if (DEBUG_LEVEL>0)    
    Serial.print(F("Time = "));
    Serial.print(LocalTime_GetDateTimeString());
    Serial.print(F(", FreeMem="));
    Serial.println(freeMemory());
    #endif

    int wackupCount = 0;
    for (int i=0; i<NumberOfSite; i++)
    {
        #if (DEBUG_LEVEL>1)
        Serial.print(F(" Check#"));
        Serial.print(i);
        Serial.print(F(" -> ["));
        for (int k=0; k<7; k++) {                        
            Serial.print(F(" "));
            Serial.print(SiteInfo[i].WakeUp.Day[k]);
        }
        
        Serial.print(F("] "));
        Serial.print(SiteInfo[i].WakeUp.Hour);
        Serial.print(F(":"));
        Serial.println(SiteInfo[i].WakeUp.Minute);
        #endif

        for (int k=0; k<7; k++)
        {
            if (SiteInfo[i].WakeUp.Day[LocalTime_GetDay()] && //Sunday~Saturday
                LocalTime_GetHour()== SiteInfo[i].WakeUp.Hour && 
                LocalTime_GetMinute()== SiteInfo[i].WakeUp.Minute
               )
            { 
                if (!SiteInfo[i].isWakeUping)
                {
                    SiteInfo[i].isWakeUping = true;
                    #if (DEBUG_LEVEL>1)
                    Serial.print(F("***Call WOL "));
                    Serial.println(SiteInfo[i].Name);
                    #endif
                
                    SendWolPacket(SiteInfo[i].IP);
                    wackupCount++;
                }
                else
                {
                    //已經喚醒過了
                }
                break;
            }
            else 
            {            
                SiteInfo[i].isWakeUping = false; //不在同一分鐘就清除
            }
        }
    }
    delay(1000);
}

