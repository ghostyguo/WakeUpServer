
#include "NetDef.h"

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

IPAddress LocalIP(192, 168, 88, 99);
IPAddress BroadcastIP(192, 168, 88, 255); 
IPAddress DNS(8, 8, 8, 8); 
IPAddress GateWay(192, 168, 88, 1); 
IPAddress SubNet(255, 255, 255, 0); 

void Net_Init()
{
  // You can use Ethernet.init(pin) to configure the CS pin
  //Ethernet.init(10);  // Most Arduino shields
  //Ethernet.init(5);   // MKR ETH Shield
  //Ethernet.init(0);   // Teensy 2.0
  //Ethernet.init(20);  // Teensy++ 2.0
  //Ethernet.init(15);  // ESP8266 with Adafruit FeatherWing Ethernet
  //Ethernet.init(33);  // ESP32 with Adafruit FeatherWing Ethernet

  // start Ethernet interface
  Ethernet.begin(mac, LocalIP, DNS, GateWay); //try fixed IP
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println(F("Ethernet shield was not found."));
    Serial.println(F("*** System Halted!"));
    while (true) {
      delay(1);
    } 
  }
  if (Ethernet.linkStatus() == LinkOFF) //not linked
  {
      Serial.println(F("Ethernet not linked, Try DHCP..."));
      Ethernet.begin(mac);
  }
  if (Ethernet.linkStatus() == LinkOFF) //not linked?
  {
      Serial.println(F("Cable is not connected?"));
  }
}