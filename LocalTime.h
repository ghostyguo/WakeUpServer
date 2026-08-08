/*
// LocalTime
*/
#if (!defined(LocalTime_h))
#define LocalTime_h

void LocalTime_Init();
void LocalTime_Loop();
int LocalTime_GetDay();
int LocalTime_GetHour();
int LocalTime_GetMinute();
int LocalTime_GetSecond();
void LocalTime_GetDateString();
String LocalTime_GetTimeString();
String LocalTime_GetDateTimeString();

#endif //LocalTime