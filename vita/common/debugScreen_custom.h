#ifndef DEBUG_SCREEN_CUSTOM_H
#define DEBUG_SCREEN_CUSTOM_H

//#define SCREEN_TAB_SIZE (8)

// backward compatibility for sources based on older Vita SDK versions
//#define DEBUG_SCREEN_CODE_INCLUDE // not recommended for your own projects, but for sake of backward compatibility
#define psvDebugScreenSetFgColor(rgb) psvDebugScreenPrintf("\e[38;2;%lu;%lu;%lum", ((uint32_t)(rgb)>>16)&0xFF, ((uint32_t)(rgb)>>8)&0xFF, (uint32_t)(rgb)&0xFF)
#define psvDebugScreenSetBgColor(rgb) psvDebugScreenPrintf("\e[48;2;%lu;%lu;%lum", ((uint32_t)(rgb)>>16)&0xFF, ((uint32_t)(rgb)>>8)&0xFF, (uint32_t)(rgb)&0xFF)
#define psvDebugScreenClear(rgb) psvDebugScreenSetBgColor(rgb); psvDebugScreenPuts("\e[H\e[2J")

// custom changes for non-Vita builds
#ifndef __vita__
#define psvDebugScreenInitReplacement(...) setvbuf(stdout,NULL,_IONBF,0)
#endif

#endif
