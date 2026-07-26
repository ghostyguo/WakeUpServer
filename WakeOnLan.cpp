#include "WakeOnLan.h"

unsigned int WolPort = 9; // WoL usually uses port 7 or 9

EthernetUDP WolUdp;

void WakeOnLan_Init() {
  WolUdp.begin(8888); // Local port to listen/send from
}

void SendWolPacket(byte macAddress[]) {
  byte packet[102];
  
  //  debug info
  Serial.print(F("Send WOL Packet:"));
  for (int i=0; i<6; i++) 
  {
     if (i>0) Serial.print(":");
     Serial.print(macAddress[i]);
     Serial.println();
  }

  // Initialize the first 6 bytes with 0xFF
  for (int i = 0; i < 6; i++) {
    packet[i] = 0xFF;
  }
  
  // Repeat the target MAC address 16 times
  for (int i = 1; i <= 16; i++) {
    for (int j = 0; j < 6; j++) {
      packet[i * 6 + j] = macAddress[j];
    }
  }

  // Send the packet via WolUdp broadcast
  WolUdp.beginPacket(BroadcastIP, WolPort);
  WolUdp.write(packet, sizeof(packet));
  WolUdp.endPacket();
  
  Serial.println("Magic packet sent!");
}