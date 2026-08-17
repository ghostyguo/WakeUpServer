#ifndef LcdManager_h
#define LcdManager_h

#include <stdint.h>
#include "LiquidCrystal_I2C.h"

enum DisplaySource {LCD_None, LCD_SystemInit, LCD_SystemReset, LCD_RTOS, LCD_Net, LCD_NtpSync, LCD_WebServer}; //目前是由誰接管顯示

struct LcdBuffer { //顯示來源
    char Line1[16];
    char Line2[16];
};

//extern LiquidCrystal_I2C LCD; //隱藏在 Manager 中, 不對外開放

void LCD_Init();
void LCD_Clear();
void LCD_SetCursor(uint8_t x, uint8_t y); 
void LCD_Print(char* str);
void LCD_SetSource(DisplaySource source);
void LCD_Loop();
void LCD_Update();
void LCD_Delay(uint16_t time=3000);
DisplaySource LCD_GetSource();

#endif //LcdManager_h