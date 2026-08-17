
#if (!defined(WebServer_h))
#define WebServer_h

#include <stdint.h>
#include "NetDef.h"
#include "LcdManager.h"


extern bool isServerConnected;
extern uint8_t WakeUpID;
extern LcdBuffer LcdBuffer_WebServer;

void WebServer_Init();
void WebServer_Loop();

#endif //WebServer_h