#include <string.h>

#include "systime.h"
#include "webserver.h"
#include "WakeUpOnLan.h"


int NumberOfSite;
static byte g_TargetMacAddress[][6] = {
                {0x40,0x16,0x7E,0x2A,0x4E,0xFC}, //J1900
                {0x5C,0xD9,0x98,0x0A,0x69,0x32}, //echos
                {0x10,0xBF,0x48,0x45,0x48,0x9E}, //P32V
                {0x00,0x1A,0x92,0x28,0xAC,0x19}, //G1 - SenseStation
                {0x48,0x5B,0x39,0x32,0xC4,0x84}, //N61JV
                {0x00,0x24,0x1D,0x8D,0x22,0x9B} //Oskay-PC
            };
            
static String WakeUpName[]={"J1900", "Echos", "P32V", "G1", "N61JV", "Oskay"};
static int WakeUpHour=19, WakeUpMinute=30;

#define MaxLineBuffer  256
char LineBuffer[MaxLineBuffer];
char *WolCommand="GET /WOL=";
int  LineBufferPtr=0, WakeUpID=0;

void setup () 
{
    NumberOfSite = sizeof(g_TargetMacAddress)/6; //each contains 6 bytes of MAC address
  
    LineBufferPtr=0;
 
    // Open serial communications and wait for port to open:
    Serial.begin(9600);

    webserver_init(); //start web server   
    systime_init();   // setup system time 
}
 
void loop() 
{ 
    // When 'w' is received, send a magic packet to wake the remote machine. 
    if(Serial.available() > 0) {
        if(Serial.read() == 'w') 
        {
            SendWOLMagicPacket(g_TargetMacAddress[1]);
            Serial.println("Magic packet sent");
        }
    }
    // listen for incoming clients

    if (webserver_client_connected()) {
        Serial.println("new client");
        webserver_response();
    }  
  
    // update the Clock Info by Arduino timer
    systime_update();
  
    // update the Clock Info by NTP
    if ((minute==0) && (second<5)) { //connect NTP to adjust colck every hour
        connectNTP(); //adjust clock
        delay(10000); // to prevent NTP twice
    }
    // check everyday wakeup time for house keeping
    if ((hour== WakeUpHour) && (minute== WakeUpMinute) && (second<5))
    {
        SendWOLMagicPacket(g_TargetMacAddress[0]);
        delay(10000); // to prevent Wackup Twice
    }
} //loop()

/*
void showCurrentTime()
{
  Serial.print("Current time is ");
  if (hour<10) Serial.print("0");
  Serial.print(hour);
  Serial.print(":");
  if (minute<10) Serial.print("0");
  Serial.print(minute);
  Serial.print(":");
  if (second<10) Serial.print("0");
  Serial.println(second);
}
*/

