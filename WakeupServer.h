/*
   WakeupSercver.h
*/

#if (!defined(WakeupSercve_h))
#define WakeupSercve_h

struct WakeUpTime {
    bool Day[7]; //Sunday, Monday, ... Saturday
    byte Hour, Minute;
};

struct Computer {
    char Name[20];
    byte IP[6];
    WakeUpTime WakeUp;
    bool isWakeUping; //旗標: 是否正在喚醒
};

extern int NumberOfSite;
extern Computer SiteInfo[];



#endif //WakeupSercve_h