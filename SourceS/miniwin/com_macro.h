#pragma once

#ifdef SWITCH
#define DSBSTATUS_PLAYING 0x00000001
#endif

#define CS_HREDRAW 0x0001
#define CS_VREDRAW 0x0002

#define IDC_ARROW (LPCSTR)0x1 // Dummy value

#define CSIDL_STARTMENU 0x000b

#define SW_HIDE DVL_SW_HIDE
#define SW_SHOWNORMAL DVL_SW_SHOWNORMAL

#define BLACK_BRUSH 4

#define LR_DEFAULTCOLOR 0x0000

#define IMAGE_ICON 1

#define SM_CXSCREEN DVL_SM_CXSCREEN
#define SM_CYSCREEN DVL_SM_CYSCREEN

#define GW_HWNDNEXT 2

#define E_FAIL DVL_E_FAIL
#define S_OK DVL_S_OK
#define NOERROR DVL_S_OK

#define MAKE_HRESULT DVL_MAKE_HRESULT

#include "miniwin/com/ddraw_macro.inc"
#include "miniwin/com/dsound_macro.inc"
