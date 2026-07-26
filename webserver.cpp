#include <Arduino.h>
#include "WebServer.h"
#include "WakeupServer.h"
#include "WakeOnLan.h"
#include "LocalTime.h"

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

// response packet buffer
#define MaxLineBuffer  256
char LineBuffer[MaxLineBuffer];
char *WolCommand="GET /WOL=";
int  LineBufferPtr=0, WakeUpID=0;

void WebServer_Init() {
  // You can use Ethernet.init(pin) to configure the CS pin
  //Ethernet.init(10);  // Most Arduino shields
  //Ethernet.init(5);   // MKR ETH Shield
  //Ethernet.init(0);   // Teensy 2.0
  //Ethernet.init(20);  // Teensy++ 2.0
  //Ethernet.init(15);  // ESP8266 with Adafruit FeatherWing Ethernet
  //Ethernet.init(33);  // ESP32 with Adafruit FeatherWing Ethernet
  Serial.println("Wackup WebServer");

  // start the server
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}

void showButton(EthernetClient client, Computer site, int ID)
{
  client.print(F("<input type=\"button\" value=\""));
  if (site.WakeUp.Hour<10) client.print("0");
  client.print(site.WakeUp.Hour);
  client.print(":");
  if (site.WakeUp.Minute<10) client.print("0");
  client.print(site.WakeUp.Minute);
  client.print(F("  Wake up ->  "));
  client.print(site.Name);
  client.print(F("  \" onclick=\"location.href='http://"));
  //client.print(MyExtIPAddressString);
  client.print(Ethernet.localIP());
  client.print(F("/WOL="));
  client.print(ID);
  client.println(F("'\"><br />")); 
}

void WebServer_Loop() 
{
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");   
    
    WakeUpID=0;
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        LineBuffer[LineBufferPtr] = c;
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println(F("HTTP/1.1 200 OK"));
          client.println(F("Content-Type: text/html"));
          client.println(F("Connection: close"));  // the connection will be closed after completion of the response
          client.println("Refresh: 10");  // refresh the page automatically every 10 sec
          client.println();
          client.println(F("<!DOCTYPE HTML>"));
          client.println(F("<html>"));
          // output the value of each analog input pin
          
          client.println(F("Wake on Lan Server ("));
          client.println(Ethernet.localIP());
          client.println(F(")<br/><br/>"));
  
          /*
          client.print("Auto wake up time is ");
          if (WakeUpHour<10) client.print("0");
          client.print(WakeUpHour);
          client.print(":");
          if (WakeUpMinute<10) client.print("0");
          client.print(WakeUpMinute);
          client.print(":00");
          client.println("<br/><br/>");       
          */

          client.print(F("Current time is "));
          client.print(LocalTime_GetTimeString());

          client.println("<br/><br/>");
          
          if (WakeUpID==0) {
            for (int id=0; id<NumberOfSite; id++) {
              showButton(client, SiteInfo[id], id+1);
              client.println("<br/>"); 
            }
            client.print("Use /WOL=n to initiate WOL");
            client.println("<br/>"); 
          }
          else
          {
            client.print("Sent WOL to ");
            //client.print(WakeUpID);
            client.print(SiteInfo[WakeUpID-1].Name);
            client.println("<br />"); 
          }
          /*
          client.print(F("WakeUpID="));
          client.print(WakeUpID);
            //client.print(WakeUpName[WakeUpID-1]);
          client.println(F("<br />")); 
          client.println(F("</html>"));
          */
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          Serial.println(LineBufferPtr);
          if (strncmp(LineBuffer,WolCommand,strlen(WolCommand))==0) {
             WakeUpID=LineBuffer[strlen(WolCommand)]-'0';
             if ((WakeUpID>0) && (WakeUpID<=NumberOfSite)) {
               SendWolPacket(SiteInfo[WakeUpID-1].IP);
             }
             else
             {
               WakeUpID=0;             
             }
             Serial.print("****** WOL #");
             Serial.print(WakeUpID);
             Serial.println(" ******");
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
    Serial.println("client disconnected");
  }
}
