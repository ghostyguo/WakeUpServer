#include <Arduino.h>
#include "WebServer.h"
#include "WakeupServer.h"
#include "WakeOnLan.h"
#include "LocalTime.h"
#include "LcdManager.h"
#include "MemoryFree.h"

#define DEBUG_LEVEL 0
//#define SHOW_ON_LCD 1 //Disabled, Due to spend much time to display,the web response is slow and ethernet is broken

//EthernetServer
EthernetServer server(80);
bool isServerConnected=false;

// response packet buffer
#define MaxLineBuffer  160
char LineBuffer[MaxLineBuffer]; // use heap to save memory
//char *WolCommand="GET /WOL=";
const char WolCommand[] PROGMEM = "GET /WOL=";
uint16_t LineBufferPtr=0;
uint8_t WakeUpID=0;

//LCD Buffer
LcdBuffer LcdBuffer_WebServer;
#if defined(SHOW_ON_LCD)
const char pgmstrWebServer[] PROGMEM = "Web Server:"; //const string for LCD
const char pgmstrClient[] PROGMEM = "Client"; //const string for LCD
#endif

void WebServer_Init() {
  // You can use Ethernet.init(pin) to configure the CS pin
  //Ethernet.init(10);  // Most Arduino shields
  //Ethernet.init(5);   // MKR ETH Shield
  //Ethernet.init(0);   // Teensy 2.0
  //Ethernet.init(20);  // Teensy++ 2.0
  //Ethernet.init(15);  // ESP8266 with Adafruit FeatherWing Ethernet
  //Ethernet.init(33);  // ESP32 with Adafruit FeatherWing Ethernet

  isServerConnected=false;
  // start the server
  server.begin();
  Serial.print(F("Server:"));
  Serial.println(Ethernet.localIP());
}

void showSiteButton(EthernetClient client, Computer site, int ID)
{
  client.print(F("<input type=\"button\" value=\""));
  //if (site.WakeUp.Hour<10) client.print(F("0"));
  //client.print(site.WakeUp.Hour);
  //client.print(":");
  //if (site.WakeUp.Minute<10) client.print(F("0"));
  //client.print(site.WakeUp.Minute);
  client.print(F("  Wake up #"));
  client.print(ID);
  client.print(F(" -> "));
  client.print(site.Name);
  client.print(F("  \" onclick=\"location.href='http://"));
  //client.print(MyExtIPAddressString);
  client.print(Ethernet.localIP());
  client.print(F("/WOL="));
  client.print(ID);
  client.println(F("'\"><br />")); 
}

void showReturnButton(EthernetClient client, int awakenID)
{
  client.print(F("Already Sent WOL to "));
  client.print(SiteInfo[awakenID].Name);
  client.println(F("<br/>")); 
  client.println(F("<br/>")); 

  client.print(F("<input type=\"button\" value=\""));
  client.print("Return to Main Page");
  client.print(F("\" onclick=\"location.href='http://"));
  client.print(Ethernet.localIP());
  client.println(F("'\"><br />")); 
}

void WebServer_Loop() 
{
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    isServerConnected=true;
    //#if (DEBUG_LEVEL>0)
    Serial.print(F("new client"));   
    Serial.print(F(", FreeMemory="));
    Serial.println(freeMemory());
    //#endif

#if defined(SHOW_ON_LCD)
    //takeover LCD
    LCD_SetSource(LCD_WebServer);
    //LCD message, Due to low memory, the following may to be removed
    strcpy_P(LcdBuffer_WebServer.Line1, pgmstrWebServer);
    strcpy_P(LcdBuffer_WebServer.Line2, pgmstrClient);
    strcat_P(LcdBuffer_WebServer.Line2, (PGM_P)F(" Connected"));
    //LCD_Update();
#endif
    
    WakeUpID=0;
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        LineBuffer[LineBufferPtr] = c;
        
        #if (DEBUG_LEVEL>2)
        Serial.write(c);
        #endif

        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println(F("HTTP/1.1 200 OK"));
          client.println(F("Content-Type: text/html"));
          client.println(F("Connection: close"));  // the connection will be closed after completion of the response          
          bool isRefresh=true;          
          for (int id=0; id<NumberOfSite; id++)
          {
            if (SiteInfo[id].isWakeUping)
            {
              isRefresh = false;
              break;
            }
          }
          if (isRefresh)
          {
            client.println(F("Refresh: 10"));  // refresh the page automatically every 10 secif ()
          }
          client.println();
          client.println(F("<!DOCTYPE HTML>"));
          client.println(F("<html>"));
          // output the value of each analog input pin
          
          client.println(F("Wake on Lan Server ("));
          client.println(Ethernet.localIP());
          client.println(F(")<br/><br/>"));
          client.print(F("Current time is "));
          client.print(LocalTime_GetDateTimeString());

          client.println("<br/><br/>");
          
          if (WakeUpID==0) {
            for (int id=0; id<NumberOfSite; id++) {
              showSiteButton(client, SiteInfo[id], id+1);
              client.println(F("<br/>")); 
            }
            client.print("Use /WOL=n to initiate WOL");
            client.println(F("<br/>")); 
          }
          else if (SiteInfo[WakeUpID-1].isWakeUping)
          {
            SiteInfo[WakeUpID-1].isWakeUping = false;
            showReturnButton(client, WakeUpID-1);
          }
          else
          {
            client.print(F("Sent WOL to "));
            client.print(SiteInfo[WakeUpID-1].Name);
            client.println(F("<br/>")); 
          }
          //Serial.print(F("Web Response, FreeMemory="));
          //Serial.println(freeMemory());
          break; //exit while to close client
        }
        if (c == '\n') {
          // you're starting a new line

          #if (DEBUG_LEVEL>1)
          Serial.println(LineBuffer);
          #endif

          if (strncmp_P(LineBuffer, (PGM_P)WolCommand,strlen(WolCommand))==0) {
            WakeUpID=LineBuffer[strlen(WolCommand)]-'0';
            if ((WakeUpID>0) && (WakeUpID<=NumberOfSite)) {
              if (!SiteInfo[WakeUpID-1].isWakeUping)
              {
                SendWolPacket(SiteInfo[WakeUpID-1].IP);
                SiteInfo[WakeUpID-1].isWakeUping = true;
                //Serial.print(F("Web WakeUp, FreeMemory="));
                //Serial.println(freeMemory());

#if defined(SHOW_ON_LCD)
                // LCD message
                strcpy_P(LcdBuffer_WebServer.Line1, (PGM_P)F("Web WakeUp ID=? "));
                LcdBuffer_WebServer.Line1[14] = '0' + WakeUpID;
                strcpy(LcdBuffer_WebServer.Line2, SiteInfo[WakeUpID-1].Name);
                //LCD_Update();    
#endif                         
              }
            }
            else
            {
              WakeUpID=0;             
            }
            #if (DEBUG_LEVEL>1)
            Serial.print(F("******"))
            Serial.print(F("WOL #"));
            Serial.print(WakeUpID);
            Serial.println(F("******"))
            #endif
          }
          LineBufferPtr=0;
          currentLineIsBlank = true;
        }     
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
          LineBufferPtr++;
        }
      }
    }

    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    
    //#if (DEBUG_LEVEL>0)
    Serial.println(F("client disconnected"));
    //#endif
  }
}
