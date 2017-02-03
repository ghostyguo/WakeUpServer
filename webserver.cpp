#include <SPI.h>
#include <Ethernet.h>
#include <Utility\Socket.h>

// Make up a mac Address & an IP address. Both should be globally unique or at least unique on the local network. 
static byte g_abyMyMacAddress[] = {0x00,0x1A,0x4B,0x38,0x0C,0x5C};
static IPAddress g_MyIPAddress(192,168,2,99);
// External IP, behind router
String MyExtIPAddressString = "114.35.227.44:30080";

// The machine to wake up. WOL should already be configured for the target machine. 
// The free windows program "Wake On LAN Ex 2" by Joseph Cox can be useful for testing the remote
// machine is properly configured. Download it here: http://software.bootblock.co.uk/?id=wakeonlanex2\

EthernetServer server(80); //ghosty

void webserver_init()
{  
    Ethernet.begin(g_abyMyMacAddress, g_MyIPAddress);
    server.begin();
  
    Serial.println("Wake on Lan (WOL) Demo");
    Serial.print("server is at ");
    Serial.println(Ethernet.localIP());
    Serial.println("Send 'w' to initiate WOL"); 
}

int webserver_client_connected()
{
    return server.available();
}

void webserver_response()
{
    EthernetClient client = server.available();
    
    if (client) {      
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
                client.println("HTTP/1.1 200 OK");
                client.println("Content-Type: text/html");
                client.println("Connection: close");  // the connection will be closed after completion of the response
                //client.println("Refresh: 5");  // refresh the page automatically every 5 sec
                client.println();
                client.println("<!DOCTYPE HTML>");
                client.println("<html>");
                // output the value of each analog input pin
          
                client.println("Wake on Lan Server (IP=");
                client.println(Ethernet.localIP());
                client.println(")<br/><br/>");
  
                client.print("Auto wake up time is ");
                if (WakeUpHour<10) client.print("0");
                client.print(WakeUpHour);
                client.print(":");
                if (WakeUpMinute<10) client.print("0");
                client.print(WakeUpMinute);
                client.print(":00");
                client.println("<br/><br/>");       
          
                client.print("Current time is ");
                if (hour<10) client.print("0");
                client.print(hour);
                client.print(":");
                if (minute<10) client.print("0");
                client.print(minute);
                client.print(":");
                if (second<10) client.print("0");
                client.println(second);         
                client.println("<br/><br/>");
          
                if (WakeUpID==0) {
                    for (int id=0; id<NumberOfSite; id++) {
                        showButton(client, WakeUpName[id], id+1);
                        client.println("<br/>"); 
                    }
                    //client.print("Use /WOL=n to initiate WOL");
                    //client.println("<br/>"); 
                }
                else
                {
                    client.print("Sent WOL to ");
                    client.print(WakeUpName[WakeUpID-1]);
                    client.println("<br />"); 
                }
                client.println("</html>");
                break;
            } //if (c == '\n' && currentLineIsBlank)
            if (c == '\n') {
                // you're starting a new line
                //Serial.println(LineBufferPtr);
                if (strncmp(LineBuffer,WolCommand,strlen(WolCommand))==0) {
                    WakeUpID=LineBuffer[strlen(WolCommand)]-'0';
                    if ((WakeUpID>0) && (WakeUpID<NumberOfSite)) {
                    SendWOLMagicPacket(g_TargetMacAddress[WakeUpID-1]);
                }
                else
                {
                    WakeUpID=0;             
                }
                Serial.print("****** WOL #");
                Serial.print(WakeUpID);
                Serial.println(" ******");
            } //(c == '\n')
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
  }  //if (client)
}


void showButton(EthernetClient client, String hostName, int ID)
{
  client.print("<input type=\"button\" value=\"");
  client.print("  Wake up ->  ");
  client.print(hostName);
  client.print("  \" onclick=\"location.href='http://");
  client.print(MyExtIPAddressString);
  client.print("/WOL=");
  client.print(ID);
  client.println("'\"><br />"); 
}
