#include "LcdManager.h"
#include "LocalTime.h"
#include "NetDef.h"
#include "WebServer.h"
#include "WakeupServer.h"
#include "RTOS.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
DisplaySource _source = LCD_None;
long waitMillis;

void LCD_Init()
{
  lcd.init();  
  lcd.backlight();
  lcd.clear();
}

void LCD_Clear()
{
  lcd.clear();
}

void LCD_SetCursor(uint8_t x, uint8_t y)
{
  //lcd.setCursor(x, y);
}

void LCD_Print(char* str)
{
  lcd.print(str);
}

DisplaySource LCD_GetSource()
{
  return _source;
}

void LCD_Delay(uint16_t time=3000)
{
  delay(time); //Let LCD show more time
}

void LCD_SetSource(DisplaySource source)
{
  if ( _source==source) return; //Not changed

  //lcd.clear();
  _source = source;
  
  Serial.print(F("LCD_Source:")); 
  switch((DisplaySource)_source)
  {
    case LCD_SystemInit: //
          {   
            Serial.println(F("SystemInit"));
          }
          break;
    case LCD_SystemReset://
          {
            Serial.println(F("System Reset"));
          }
          break;
    case LCD_RTOS: //
          {   
            Serial.println(F("RTOS"));
            waitMillis = millis()+2000; //切回 RTOS 之前先等待一些時間再顯示
          }
          break;
    case LCD_Net: //
          {     
            Serial.println(F("Net")); 
          }
          break;
    case LCD_NtpSync: //
          {   
            Serial.println(F("NtpSync"));
          }
          break;
    case LCD_WebServer: //
          {  
            Serial.println(F("Web"));
          }
          break;
    defaule: //
          {
            Serial.println(F("Bad"));
          }
          break;
  }
}

void LCD_Update()
{
  switch((DisplaySource)_source)
  {
    case LCD_SystemInit: //
          {   
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print(F("WakeOnLan Server"));
            lcd.setCursor(0,1);
            lcd.print(F("Site Count : "));
            lcd.print(NumberOfSite);
          }
          break;
    case LCD_SystemReset://
          {   
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print(F("WakeOnLan Server"));
            lcd.setCursor(0,1);
            lcd.print(F("System Reset !!!"));
          }
          break;
    case LCD_NtpSync://
          {   
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print(F("WakeOnLan Server"));
            lcd.setCursor(0,1);
            lcd.print(F("  NTP Sync ..."));
          }
          break;
    case LCD_RTOS: //
          {   
            if (millis() < waitMillis)
            {
              break;
            }
            waitMillis = millis();
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print(Ethernet.localIP());
            
            if (WakeUpID>0)
            {
              lcd.setCursor(14, 1);
              lcd.print(F("+"));
              lcd.print(WakeUpID);
            }
            
            if (isServerConnected) 
            {
              lcd.setCursor(14, 0);
              lcd.print(F("*"));
              isServerConnected = false;
            }
            lcd.setCursor(0,1);
            if (LocalTime_IsTimeSet())
            {
              lcd.print(LocalTime_GetDateTimeString());
            }
            else
            {
              lcd.print(F("Wait NTP Sync..."));
            }
          }
          break;
    case LCD_Net: //
          {     
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print(LcdBuffer_Net.Line1);
            lcd.setCursor(0,1);
            lcd.print(LcdBuffer_Net.Line2);      
            waitMillis = millis()+3000; //切回 RTOS 之前先等待一些時間再顯示
          }
          break;
    case LCD_WebServer: //
          {  
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print(LcdBuffer_WebServer.Line1);
            lcd.setCursor(0,1);
            lcd.print(LcdBuffer_WebServer.Line2);      
            waitMillis = millis()+1000; //切回 RTOS 之前先等待一些時間再顯示
          }
          break;
    defaule: //
          {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print(F("Bad LCD Source"));
            RTOS.shutdown();
          }
          break;
  }
}