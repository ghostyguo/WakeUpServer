/*
// NetDef: 所有網路共用的定義Init
*/
#if (!defined(NetDef_h))
#define NetDef_h

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include "LcdManager.h"

//typedef byte unsigned char;

const char timeServer[] = "time.nist.gov"; // time.nist.gov NTP server

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
extern byte mac[];
extern IPAddress StaticIP;
extern IPAddress BroadcastIP;
extern IPAddress DNS;
extern IPAddress GateWay;
extern IPAddress SubNet;

extern LcdBuffer LcdBuffer_Net;  //因為可能會 Halt, 所以不用

void Net_Init();

#endif // NetDef_h