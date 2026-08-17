/*
   WakeupSercver.h
*/

#if (!defined(WakeupServer_h))
#define WakeupServer_h

struct WakeUpTime {
    bool Day[7]; //Sunday, Monday, ... Saturday
    byte Hour, Minute;
};

struct Computer {
    char Name[16];
    byte IP[6];
    WakeUpTime WakeUp;
    bool isWakeUping; //Flag
};

extern int NumberOfSite;
extern Computer SiteInfo[];




#endif //WakeupServer_h