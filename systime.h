#ifndef SysTime_h
#define SysTime_h

#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

extern int hour, minute, second;

void systime_init();
void systime_update();
void connectNTP();
unsigned long sendNTPpacket(IPAddress& address);

#endif
