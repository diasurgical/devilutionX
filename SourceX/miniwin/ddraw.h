#include "devilution.h"
#include <SDL.h>

namespace dvl {
	/* START ANDROID INPUT FUNKTIONS */
	void DisplayJoyStick(int x , int y, int h, int w, SDL_Surface * SrcSurface, SDL_Surface * DstSurface);
	void LoadAndroidImages();
	void DrawJoyStick(int MouseX, int MouseY, bool flag);
	void walkInDir(int dir);
	void __fastcall checkTownersNearby(bool interact);
	void DrawAndroidUI();
	bool __fastcall checkMonstersNearby(bool attack, bool spellcast);
	void convert_touch_xy_to_game_xy(float touch_x, float touch_y, int *game_x, int *game_y);
	//void __fastcall checkItemsNearby(bool interact);
	void AutoPickGold(int pnum);
	void AutoPickItem(int pnum);
	void ActivateObject(bool interact);
	void __fastcall checkItemsNearby(int pnum);
	void useBeltPotion(bool mana);

	extern bool showJoystick;
	extern int TouchX;
	extern int TouchY;

	extern SDL_Surface * JoyStickS;
	extern SDL_Texture * JoyStickT;
	extern SDL_Surface * AJoyStickS;
	extern SDL_Texture * AJoyStickT;
	extern SDL_Surface * ShiftStickS;
	extern SDL_Texture * ShiftStickT;
	extern SDL_Surface * DemoSqS;
	extern SDL_Texture * DemoSqT;
	extern SDL_Surface * Fog ;
	extern SDL_Texture * gFog ;
	extern bool ShiftButtonPressed ;
	extern bool AttackButtonPressed;
	extern bool CastButtonPressed;

	typedef struct coords {
	int x;
	int y;
} coords;
extern coords speedspellscoords[50];

extern SDL_Rect Arect;
extern SDL_Rect Shiftrect;
extern SDL_Rect LGameUIMenu;
extern SDL_Rect RGameUIMenu;
extern SDL_Rect PotGameUIMenu;
extern SDL_Rect Crect;

extern SDL_Rect DemoN;
extern SDL_Rect DemoS;
extern SDL_Rect DemoE;
extern SDL_Rect DemoW;

extern SDL_Rect DemoNW;
extern SDL_Rect DemoNE;
extern SDL_Rect DemoSW;
extern SDL_Rect DemoSE;

extern SDL_Rect DemonHealth;
extern SDL_Rect AngelMana;

extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern SDL_Texture *texture;

extern SDL_Surface *surface;
extern SDL_Palette *palette;
extern SDL_Surface *pal_surface;
extern bool bufferUpdated;

struct StubDraw : public IDirectDraw {
public:
	virtual ULONG Release();
	HRESULT CreatePalette(DWORD dwFlags, LPPALETTEENTRY lpColorTable, LPDIRECTDRAWPALETTE *lplpDDPalette, IUnknown *pUnkOuter);
	HRESULT CreateSurface(LPDDSURFACEDESC lpDDSurfaceDesc, LPDIRECTDRAWSURFACE *lplpDDSurface, IUnknown *pUnkOuter);
	HRESULT WaitForVerticalBlank(DWORD dwFlags, HANDLE hEvent);
};

struct StubSurface : public IDirectDrawSurface {
public:
	StubSurface(LPDDSURFACEDESC lpDDSurfaceDesc);
	~StubSurface();
	virtual ULONG Release();
	HRESULT BltFast(DWORD dwX, DWORD dwY, LPDIRECTDRAWSURFACE lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwTrans);
	HRESULT GetCaps(LPDDSCAPS lpDDSCaps);
	HRESULT GetDC(HDC *lphDC);
	HRESULT GetPixelFormat(LPDDPIXELFORMAT lpDDPixelFormat);
	HRESULT IsLost();
	HRESULT Lock(LPRECT lpDestRect, LPDDSURFACEDESC lpDDSurfaceDesc, DWORD dwFlags, HANDLE hEvent);
	HRESULT ReleaseDC(HDC hDC);
	HRESULT Restore();
	HRESULT SetPalette(LPDIRECTDRAWPALETTE lpDDPalette);
	HRESULT Unlock(LPVOID lpSurfaceData);
};

struct StubPalette : public IDirectDrawPalette {
public:
	virtual ULONG Release();
	HRESULT GetCaps(LPDWORD lpdwCaps);
	HRESULT GetEntries(DWORD dwFlags, DWORD dwBase, DWORD dwNumEntries, LPPALETTEENTRY lpEntries);
	HRESULT SetEntries(DWORD dwFlags, DWORD dwStartingEntry, DWORD dwCount, LPPALETTEENTRY lpEntries);
};

} // namespace dvl
