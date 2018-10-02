// ref: 0x10007458
void __fastcall Focus_CheckPlayMove(LPARAM lParam)
{
	if ( sgbSpinnersLoaded && lParam != dword_10029CA8 )
	{
		if ( dword_10029CAC )
			TitleSnd_PlayMoveSound();
		dword_10029CA8 = lParam;
	}
}
// 10029CA4: using guessed type int sgbSpinnersLoaded;
// 10029CA8: using guessed type int dword_10029CA8;
// 10029CAC: using guessed type int dword_10029CAC;

// ref: 0x10007482
int __cdecl Focus_GetSpinWidthOrZero()
{
	return sgbSpinnersLoaded != 0 ? focus_spin_width : 0;
}
// 10029CA4: using guessed type int sgbSpinnersLoaded;

// ref: 0x10007492
void __fastcall Focus_BlitSpinner(HWND hWnd1, HWND hWnd2)
{
	_DWORD *v2; // edi
	LONG v3; // eax MAPDST
	int v5; // eax MAPDST
	int v7; // eax
	struct tagRECT Rect; // [esp+8h] [ebp-18h]
	char *v9; // [esp+18h] [ebp-8h]

	v9 = (char *)hWnd1;
	v2 = (_DWORD *)GetWindowLongA(hWnd1, -21);
	v3 = GetWindowLongA(hWnd2, -21);
	if ( v2 && v3 && *v2 )
	{
		if ( *(_DWORD *)v3 )
		{
			GetWindowRect(hWnd2, &Rect);
			ScreenToClient((HWND)v9, (LPPOINT)&Rect);
			ScreenToClient((HWND)v9, (LPPOINT)&Rect.right);
			SBltROP3(
				*(void **)v3,
				(void *)(Rect.left + *v2 + Rect.top * v2[1]),
				focus_spin_width,
				*(_DWORD *)(v3 + 8),
				*(_DWORD *)(v3 + 4),
				v2[1],
				0,
				0xCC0020u);
			v5 = v2[1];
			v7 = *v2 + Rect.top * v5;
			v9 = *(char **)(v3 + 4);
			SBltROP3(
				&v9[*(_DWORD *)v3 - focus_spin_width],
				&v9[v7 - focus_spin_width + Rect.left],
				focus_spin_width,
				*(_DWORD *)(v3 + 8),
				(int)v9,
				v5,
				0,
				0xCC0020u);
			Focus_CenterSpinFromSide(hWnd2);
		}
	}
}

// ref: 0x10007566
void __fastcall Focus_CenterSpinFromSide(HWND hWnd)
{
	struct tagRECT Rect; // [esp+8h] [ebp-10h]

	GetClientRect(hWnd, &Rect);
	--Rect.bottom;
	Rect.left = --Rect.right - focus_spin_width;
	InvalidateRect(hWnd, &Rect, 0);
	Rect.left = 0;
	Rect.right = focus_spin_width - 1;
	InvalidateRect(hWnd, &Rect, 0);
}

// ref: 0x100075B7
void __fastcall Focus_GetAndBlitSpin(HWND hWnd, LPARAM lParam)
{
	HWND v2; // esi

	v2 = (HWND)lParam;
	GetWindowLongA((HWND)lParam, -12);
	Focus_BlitSpinner(hWnd, v2);
	dword_10029CAC = 1;
}
// 10029CAC: using guessed type int dword_10029CAC;

// ref: 0x100075DC
BOOL __fastcall Focus_DoBlitSpinIncFrame(HWND hWnd1, HWND hWnd2)
{
	void **v4; // eax
	void **v5; // esi
	int v7; // edi
	struct tagRECT Rect; // [esp+Ch] [ebp-14h]

	if ( !sgbSpinnersLoaded )
		return 0;
	if ( !hWnd2 )
		return 0;
	GetWindowLongA(hWnd2, -12);
	if ( hWnd1 != GetParent(hWnd2) )
		return 0;
	Focus_BlitSpinner(hWnd1, hWnd2);
	v4 = (void **)GetWindowLongA(hWnd2, -21);
	v5 = v4;
	if ( !v4 || !*v4 )
		return 0;
	GetWindowRect(hWnd2, &Rect);
	ScreenToClient(hWnd1, (LPPOINT)&Rect);
	ScreenToClient(hWnd1, (LPPOINT)&Rect.right);
	if ( SpinnerTransOut[sgnSpinnerFrame] )
	{
		v7 = ((signed int)v5[2] - focus_spin_height) / 2;
		STransBlt(*v5, 0, v7, (int)v5[1], SpinnerTransOut[sgnSpinnerFrame]);
		STransBlt(*v5, (int)v5[1] - focus_spin_width, v7, (int)v5[1], SpinnerTransOut[sgnSpinnerFrame]);
		Focus_CenterSpinFromSide(hWnd2);
	}
	if ( ++sgnSpinnerFrame >= 8 )
		sgnSpinnerFrame = 0;
	return 1;
}
// 10029C9C: using guessed type int focus_spin_height;
// 10029CA4: using guessed type int sgbSpinnersLoaded;
// 10029CB0: using guessed type int sgnSpinnerFrame;

// ref: 0x100076C3
void __cdecl Focus_DeleteSpinners()
{
	HANDLE *v0; // esi

	v0 = SpinnerTransOut;
	do
	{
		if ( *v0 )
		{
			STransDelete(*v0);
			*v0 = 0;
		}
		++v0;
	}
	while ( (signed int)v0 < (signed int)&SpinnerTransOut[8] );
	dword_10029CAC = 0;
	dword_10029CA8 = 0;
	sgbSpinnersLoaded = 0;
}
// 10029CA4: using guessed type int sgbSpinnersLoaded;
// 10029CA8: using guessed type int dword_10029CA8;
// 10029CAC: using guessed type int dword_10029CAC;

// ref: 0x100076FA
void __cdecl Focus_ResetSpinToZero()
{
	dword_10029CAC = 0;
	dword_10029CA8 = 0;
}
// 10029CA8: using guessed type int dword_10029CA8;
// 10029CAC: using guessed type int dword_10029CAC;

// ref: 0x1000770E
void __cdecl Focus_cpp_init()
{
	focus_cpp_float = focus_cpp_float_value;
}
// 1001F42C: using guessed type int focus_cpp_float_value;
// 10029CA0: using guessed type int focus_cpp_float;

// ref: 0x10007719
void __fastcall Focus_LoadSpinner(const char *pszFileName)
{
	HANDLE *v1; // esi
	int v2; // ecx
	int v3; // eax
	int v4; // ebx
	int a5[4]; // [esp+4h] [ebp-1Ch]
	DWORD data[2]; // [esp+14h] [ebp-Ch]
	BYTE *pBuffer; // [esp+1Ch] [ebp-4h]

	if ( !sgbSpinnersLoaded )
	{
		pBuffer = 0;
		dword_10029CAC = 0;
		dword_10029CA8 = 0;
		local_LoadArtImage(pszFileName, &pBuffer, data);
		v1 = SpinnerTransOut;
		memset(SpinnerTransOut, 0, 0x20u);
		if ( pBuffer )
		{
			v2 = data[0];
			focus_spin_width = data[0];
			v3 = (signed int)data[1] / 8;
			v4 = 0;
			focus_spin_height = (signed int)data[1] / 8;
			while ( 1 )
			{
				a5[2] = v2 - 1;
				a5[1] = v4 * v3;
				a5[0] = 0;
				a5[3] = v3 + v4 * v3 - 1;
				STransCreateI(pBuffer, v2, v3, 8, (int)a5, 16777466, v1);
				++v1;
				++v4;
				if ( (signed int)v1 >= (signed int)&SpinnerTransOut[8] )
					break;
				v3 = focus_spin_height;
				v2 = focus_spin_width;
			}
			SMemFree(pBuffer, "C:\\Src\\Diablo\\DiabloUI\\Focus.cpp", 246, 0);
		}
		sgnSpinnerFrame = 0;
		sgbSpinnersLoaded = 1;
	}
}
// 10029C9C: using guessed type int focus_spin_height;
// 10029CA4: using guessed type int sgbSpinnersLoaded;
// 10029CA8: using guessed type int dword_10029CA8;
// 10029CAC: using guessed type int dword_10029CAC;
// 10029CB0: using guessed type int sgnSpinnerFrame;

// ref: 0x100077E9
void __fastcall Focus_SetFocusTimer(HWND hWnd, const char *pszFileName)
{
	Focus_LoadSpinner(pszFileName);
	SDlgSetTimer((int)hWnd, 1, 55, Focus_SetFocusAndBlit);
}

// ref: 0x10007804
void __stdcall Focus_SetFocusAndBlit(int hWnd, int a2, int a3, int a4)
{
	HWND v1; // eax

	v1 = GetFocus();
	Focus_DoBlitSpinIncFrame((HWND)hWnd, v1);
}

// ref: 0x10007818
void __fastcall Focus_KillFocusTimer(HWND hWnd)
{
	SDlgKillTimer((int)hWnd, 1);
	Focus_DeleteSpinners();
}
