
#include "NetDef.h"
#include "WakeupServer.h" //import LCD definition
#include "RTOS.h"

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

IPAddress StaticIP(192, 168, 88, 99);
IPAddress BroadcastIP(192, 168, 88, 255); 
IPAddress DNS(8, 8, 8, 8); 
IPAddress GateWay(192, 168, 88, 1); 
IPAddress SubNet(255, 255, 255, 0); 

//LCD Buffer
LcdBuffer LcdBuffer_Net;

void Net_Init()
{
  // You can use Ethernet.init(pin) to configure the CS pin
  //Ethernet.init(10);  // Most Arduino shields
  //Ethernet.init(5);   // MKR ETH Shield
  //Ethernet.init(0);   // Teensy 2.0
  //Ethernet.init(20);  // Teensy++ 2.0
  //Ethernet.init(15);  // ESP8266 with Adafruit FeatherWing Ethernet
  //Ethernet.init(33);  // ESP32 with Adafruit FeatherWing Ethernet

  //takeover LCD
  LCD_SetSource(LCD_Net);

  // start Ethernet interface
    Ethernet.begin(mac, StaticIP, DNS, GateWay); //try fixed IP
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println(F("Ethernet shield was not found."));
    Serial.println(F("*** System Halted!"));    
    //使用 LCD Buffer
    strcpy_P(LcdBuffer_Net.Line1,(PGM_P)F("Ether Shield?"));
    strcpy_P(LcdBuffer_Net.Line2,(PGM_P)F("System Halted!"));
    LCD_Update();
    RTOS.shutdown();          
  }
  if (Ethernet.linkStatus() == LinkOFF) //not linked
  {
    Serial.println(F("Static IP failed, Try DHCP..."));  
    //使用 LCD Buffer
    strcpy_P(LcdBuffer_Net.Line1,(PGM_P)F("Not Linked?"));
    strcpy_P(LcdBuffer_Net.Line2,(PGM_P)F("Try DHCP..."));
    LCD_Update();
    LCD_Delay();
    Ethernet.begin(mac);
  }
  if (Ethernet.linkStatus() == LinkOFF) //not linked?
  {
    Serial.println(F("Cable is not connected?"));
    //使用 LCD Buffer
    strcpy_P(LcdBuffer_Net.Line1,(PGM_P)F("Cable Failed?"));
    strcpy_P(LcdBuffer_Net.Line2,(PGM_P)F("System Halted!"));
    LCD_Update();
    RTOS.shutdown();         
  }
}