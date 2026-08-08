/*
    NTPClient v3.2.1: from Arduino Library by Fabrice Weinberg

    source: https://github.com/arduino-libraries/NTPClient

    modified by ghostyguo@gmail.com
    
*/


#include "NetDef.h"
#include "NTPClient.h"

EthernetUDP ntpUDP;
NTPClient timeClient(ntpUDP);

unsigned long oldMillis;

void LocalTime_Init()
{
  timeClient.begin();
  timeClient.setTimeOffset(8*3600); //UTC+8
  oldMillis = millis();
}

void LocalTime_Loop() {
  if (millis()-oldMillis>1000) {
    timeClient.update();
    oldMillis = millis();
  }
}

int LocalTime_GetDay()
{
  //return (timeClient.getHours()+8)%24; //UTC+8
  int day=timeClient.getDay();
  return (day>=0 && day<7) ? timeClient.getDay() : -1;
}

int LocalTime_GetHour()
{
  //return (timeClient.getHours()+8)%24; //UTC+8
  return timeClient.getHours();
}

int LocalTime_GetMinute()
{
  return timeClient.getMinutes();
}

int LocalTime_GetSecond()
{
  return timeClient.getSeconds();
}

void LocalTime_GetDateString()
{
  /*
  time_t utcCalc = timeClient.getEpochTime(); 

  Serial.print( year(utcCalc )) 
  Serial.print( month(utcCalc )) ;
  Serial.print( day(utcCalc )) ;
  Serial.print( hour(utcCalc )) ;
  Serial.print( minute(utcCalc ) );
  */
}

String LocalTime_GetTimeString()
{
   return timeClient.getFormattedTime();
}


String LocalTime_GetDateTimeString()
{
   return timeClient.getFormattedDateTime();
}
