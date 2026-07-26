/*
// WakeOnLan
*/
#if (!defined(WakeOnLan_h))
#define WakeOnLan_h

#include "NetDef.h"

void WakeOnLan_Init();
void SendWolPacket(byte macAddress[]);

#endif //WakeOnLan

