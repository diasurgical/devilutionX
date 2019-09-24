#pragma once

namespace dvl {

//
// COM
//
#define DECLARE_INTERFACE_(name, base) struct name : public base
#define THIS_
#define THIS
#define PURE = 0

#define STDMETHOD(name) STDMETHOD_(HRESULT, name)
#define STDMETHOD_(type, name) virtual WINAPI type name

struct IUnknown {
	// clang-format off
	STDMETHOD_(ULONG, Release)(THIS) PURE;
	// clang-format on
};

template<class T, class U, class V> constexpr HRESULT DVL_MAKE_HRESULT(T&& sev, U&& fac, V&& code)
{
        return (((uint32_t)(sev) << 31) | ((uint32_t)(fac) << 16) | ((uint32_t)(code)));
}

typedef struct IDirectDrawPalette *LPDIRECTDRAWPALETTE;
typedef struct IDirectDrawSurface *LPDIRECTDRAWSURFACE;
typedef struct IDirectDraw *LPDIRECTDRAW;

#include "miniwin/com/dsound.inc"

constexpr HRESULT DVL_E_FAIL = 0x80004005L;
constexpr HRESULT DVL_S_OK = 0;

constexpr auto DVL_SW_HIDE = 0;
constexpr auto DVL_SW_SHOWNORMAL = 1;
constexpr auto DVL_SM_CXSCREEN = 0;
constexpr auto DVL_SM_CYSCREEN = 1;

}  // namespace dvl
