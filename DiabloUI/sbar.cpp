// ref: 0x100099B5
void __cdecl Sbar_cpp_init()
{
	Sbar_cpp_float = Sbar_cpp_float_value;
}
// 1001F448: using guessed type int Sbar_cpp_float_value;
// 1002A338: using guessed type int Sbar_cpp_float;

// ref: 0x100099C0
BOOL __fastcall Sbar_CheckIfNextHero(HWND hWnd)
{
	_uiheroinfo *v1; // eax

	v1 = (_uiheroinfo *)GetWindowLongA(hWnd, -21);
	if ( !v1 || !v1->next )
		return 0;
	v1->next = 0;
	return 1;
}

// ref: 0x100099DC
int __fastcall Sbar_NumScrollLines(HWND hWnd, int width, int height)
{
	_DWORD *v4; // eax
	_DWORD *v5; // esi
	int result; // eax
	signed int v7; // ecx
	LONG v8; // ebx
	LONG v9; // edi
	int v10; // [esp-4h] [ebp-1Ch]
	struct tagPOINT Point; // [esp+Ch] [ebp-Ch]

	Point.x = width;
	Point.y = height;
	if ( !hWnd )
		return 0;
	if ( !IsWindowVisible(hWnd) )
		return 0;
	v4 = (_DWORD *)GetWindowLongA(hWnd, -21);
	v5 = v4;
	if ( !v4 )
		return 0;
	v7 = v4[13];
	if ( v7 <= 1 )
		v8 = 22;
	else
		v8 = v4[14] * (v4[3] - v4[9] - 44) / (v7 - 1) + 22;
	v9 = v8 + v4[9];
	ScreenToClient(hWnd, &Point);
	if ( Point.y >= 22 )
	{
		if ( Point.y >= v8 )
		{
			if ( Point.y >= v9 )
			{
				if ( Point.y >= v5[3] - 22 )
				{
					*v5 = 4;
					v10 = 2;
				}
				else
				{
					*v5 = 8;
					v10 = 4;
				}
			}
			else
			{
				*v5 = 16;
				v10 = 5;
			}
		}
		else
		{
			*v5 = 2;
			v10 = 3;
		}
		result = v10;
	}
	else
	{
		result = 1;
		*v5 = 1;
	}
	return result;
}

// ref: 0x10009A99
void __fastcall Sbar_DrawScrollBar(HWND hWnd, int nIDDlgItem, int width, int height)
{
	HWND v4; // eax
	LONG v5; // eax MAPDST
	bool v7; // zf
	int v8; // eax
	tagRECT DstRect; // [esp+Ch] [ebp-24h]
	tagRECT SrcBuffer; // [esp+1Ch] [ebp-14h]
	HWND hWnda; // [esp+2Ch] [ebp-4h]

	v4 = GetDlgItem(hWnd, nIDDlgItem);
	hWnda = v4;
	if ( v4 )
	{
		v5 = GetWindowLongA(v4, -21);
		if ( v5 )
		{
			if ( *(_DWORD *)(v5 + 4) )
			{
				v7 = *(_DWORD *)(v5 + 16) == 0;
				*(_DWORD *)(v5 + 52) = width;
				*(_DWORD *)(v5 + 56) = height;
				if ( !v7 )
				{
					SrcBuffer.left = 0;
					DstRect.left = 0;
					SrcBuffer.top = 0;
					DstRect.top = 0;
					DstRect.right = *(_DWORD *)(v5 + 8) - 1;
					DstRect.bottom = *(_DWORD *)(v5 + 12) - 1;
					SrcBuffer.right = *(_DWORD *)(v5 + 8) - 1;
					SrcBuffer.bottom = *(_DWORD *)(v5 + 24) - 1;
					SBltROP3Tiled(
						*(void **)(v5 + 4),
						&DstRect,
						*(POINT **)(v5 + 8),
						*(_DWORD *)(v5 + 16),
						&SrcBuffer,
						*(RECT **)(v5 + 20),
						0,
						0,
						0,
						0xCC0020u);
					if ( *(_DWORD *)(v5 + 28) )
					{
						if ( width <= 1 )
							v8 = 22;
						else
							v8 = height * (*(_DWORD *)(v5 + 12) - *(_DWORD *)(v5 + 36) - 44) / (width - 1) + 22;
						SBltROP3(
							(void *)(v8 * *(_DWORD *)(v5 + 8) + *(_DWORD *)(v5 + 4) + 3),
							*(void **)(v5 + 28),
							18,
							*(_DWORD *)(v5 + 36),
							*(_DWORD *)(v5 + 8),
							*(_DWORD *)(v5 + 32),
							0,
							0xCC0020u);
						SBltROP3(
							*(void **)(v5 + 4),
							(void *)(*(_DWORD *)(v5 + 40) + 22 * (~*(_BYTE *)v5 & 1) * *(_DWORD *)(v5 + 44)),
							*(_DWORD *)(v5 + 8),
							22,
							*(_DWORD *)(v5 + 8),
							*(_DWORD *)(v5 + 44),
							0,
							0xCC0020u);
						SBltROP3(
							(void *)(*(_DWORD *)(v5 + 4) + *(_DWORD *)(v5 + 8) * (*(_DWORD *)(v5 + 12) - 22)),
							(void *)(*(_DWORD *)(v5 + 40) + 22 * ((~*(_BYTE *)v5 & 4 | 8u) >> 2) * *(_DWORD *)(v5 + 44)),
							*(_DWORD *)(v5 + 8),
							22,
							*(_DWORD *)(v5 + 8),
							*(_DWORD *)(v5 + 44),
							0,
							0xCC0020u);
						InvalidateRect(hWnda, 0, 0);
					}
				}
			}
		}
	}
}

// ref: 0x10009BF1
void __fastcall Sbar_LoadScrBarGFX(HWND hWnd, int nIDDlgItem)
{
	DWORD *v2; // eax MAPDST
	void *v4; // eax
	struct tagRECT Rect; // [esp+Ch] [ebp-14h]
	HWND hWnda; // [esp+1Ch] [ebp-4h]

	hWnda = GetDlgItem(hWnd, nIDDlgItem);
	if ( hWnda )
	{
		v2 = (DWORD *)SMemAlloc(0x3Cu, "C:\\Src\\Diablo\\DiabloUI\\Sbar.cpp", 221, 0);
		if ( v2 )
		{
			SetWindowLongA(hWnda, -21, (LONG)v2);
			*v2 = 0;
			GetClientRect(hWnda, &Rect);
			v2[2] = Rect.right;
			v2[3] = Rect.bottom;
			v4 = SMemAlloc(Rect.right * Rect.bottom, "C:\\Src\\Diablo\\DiabloUI\\Sbar.cpp", 230, 0);
			v2[1] = (DWORD)v4;
			if ( v4 )
			{
				SDlgSetBitmapI(hWnda, 0, &nullcharacter, -1, 1, v4, 0, v2[2], v2[3], -1);
				local_LoadArtImage("ui_art\\sb_bg.pcx", (BYTE **)v2 + 4, v2 + 5);
				local_LoadArtImage("ui_art\\sb_thumb.pcx", (BYTE **)v2 + 7, v2 + 8);
				local_LoadArtImage("ui_art\\sb_arrow.pcx", (BYTE **)v2 + 10, v2 + 11);
			}
		}
	}
}

// ref: 0x10009CC7
void __cdecl Sbar_cpp_init2()
{
	Sbar_cpp_float2 = Sbar_cpp_float_value2;
}
// 1001F44C: using guessed type int Sbar_cpp_float_value2;
// 1002A344: using guessed type int Sbar_cpp_float2;

// ref: 0x10009CD2
void __fastcall Sbar_FreeScrollBar(HWND hWnd, int nIDDlgItem)
{
	HWND v2; // eax MAPDST
	_DWORD *v4; // eax MAPDST
	void *v6; // eax
	void *v7; // eax
	void *v8; // eax
	void *v9; // eax

	v2 = GetDlgItem(hWnd, nIDDlgItem);
	if ( v2 )
	{
		v4 = (_DWORD *)GetWindowLongA(v2, -21);
		if ( v4 )
		{
			v6 = (void *)v4[1];
			if ( v6 )
				SMemFree(v6, "C:\\Src\\Diablo\\DiabloUI\\Sbar.cpp", 267, 0);
			v7 = (void *)v4[4];
			if ( v7 )
				SMemFree(v7, "C:\\Src\\Diablo\\DiabloUI\\Sbar.cpp", 269, 0);
			v8 = (void *)v4[7];
			if ( v8 )
				SMemFree(v8, "C:\\Src\\Diablo\\DiabloUI\\Sbar.cpp", 271, 0);
			v9 = (void *)v4[10];
			if ( v9 )
				SMemFree(v9, "C:\\Src\\Diablo\\DiabloUI\\Sbar.cpp", 273, 0);
			SMemFree(v4, "C:\\Src\\Diablo\\DiabloUI\\Sbar.cpp", 275, 0);
			SetWindowLongA(v2, -21, 0);
		}
	}
}
