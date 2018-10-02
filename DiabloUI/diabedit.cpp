// ref: 0x10005765
void __fastcall DiabEdit_DoPaintBMP(HWND hWnd)
{
	struct tagPAINTSTRUCT Paint; // [esp+4h] [ebp-40h]

	BeginPaint(hWnd, &Paint);
	SDlgDrawBitmap(hWnd, 1, 0, 0, 0, 0, 0);
	EndPaint(hWnd, &Paint);
}

// ref: 0x1000579B
void __cdecl DiabEdit_cpp_init()
{
	DiabEdit_cpp_float = DiabEdit_cpp_float_value;
}
// 1001F40C: using guessed type int DiabEdit_cpp_float_value;
// 1002970C: using guessed type int DiabEdit_cpp_float;

// ref: 0x100057A6
void __cdecl DiabEdit_SetupWindow()
{
	WNDCLASSA WndClass; // [esp+0h] [ebp-28h]

	memset(&WndClass, 0, 0x28u);
	WndClass.style = 64;
	WndClass.lpfnWndProc = DiabEdit_WndProc;
	WndClass.hInstance = GetModuleHandleA(0);
	WndClass.lpszClassName = "DIABLOEDIT";
	RegisterClassA(&WndClass);
}

// ref: 0x100057E8
LRESULT __stdcall DiabEdit_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if ( Msg <= 0x113 )
	{
		if ( Msg == 275 )
		{
			DiabEdit_GetCursorProp(hWnd);
			return 0;
		}
		if ( Msg == 1 )
		{
			DiabEdit_SetRestrictTimer(hWnd);
		}
		else if ( Msg == 2 )
		{
			DiabEdit_RemoveAllProps(hWnd);
		}
		else
		{
			if ( Msg != 7 )
			{
				if ( Msg == 15 )
				{
					DiabEdit_DoPaintBMP(hWnd);
				}
				else
				{
					if ( Msg == 135 )
						return 129;
					if ( Msg != 256 )
					{
						if ( Msg == 258 )
							DiabEdit_RestrictAndLimit(hWnd, wParam, lParam);
						return DefWindowProcA(hWnd, Msg, wParam, lParam);
					}
					DiabEdit_SetTextAndProp(hWnd, wParam, lParam);
				}
				return 0;
			}
			DiabEdit_SendWndCommand(hWnd, 1u);
		}
		return DefWindowProcA(hWnd, Msg, wParam, lParam);
	}
	switch ( Msg )
	{
		case 0x201u:
			SetFocus(hWnd);
			return DefWindowProcA(hWnd, Msg, wParam, lParam);
		case 0x400u:
			SetWindowTextA(hWnd, &nullcharacter);
			DiabEdit_SendWndCommand(hWnd, 3u);
			return 0;
		case 0x401u:
			SetPropA(hWnd, "LIMIT", (HANDLE)wParam);
			return 0;
		case 0x402u:
			return (LRESULT)GetPropA(hWnd, "LIMIT");
	}
	if ( Msg != 1027 )
	{
		if ( Msg == 1028 )
		{
			DiabEdit_SetRestrictString(hWnd, lParam);
			return 0;
		}
		return DefWindowProcA(hWnd, Msg, wParam, lParam);
	}
	return (LRESULT)GetPropA(hWnd, "CURSOR");
}

// ref: 0x1000591C
void __fastcall DiabEdit_SendWndCommand(HWND hWnd, WORD a2)
{
	int v4; // ST08_4
	HWND v5; // eax

	v4 = (a2 << 16) | (unsigned short)GetWindowLongA(hWnd, -12);
	v5 = GetParent(hWnd);
	SendMessageA(v5, 0x111u, v4, (LPARAM)hWnd);
}

// ref: 0x1000594E
void __fastcall DiabEdit_GetCursorProp(HWND hWnd)
{
	size_t v2; // eax
	char *v3; // esi
	char String[256]; // [esp+Ch] [ebp-100h]

	String[0] = nullcharacter;
	memset(&String[1], 0, 0xFCu);
	*(_WORD *)&String[253] = 0;
	String[255] = 0;
	if ( GetPropA(hWnd, "CURSOR") )
	{
		SetPropA(hWnd, "CURSOR", 0);
		DiabEdit_SendWndCommand(hWnd, 3u);
	}
	else
	{
		SetPropA(hWnd, "CURSOR", (void *)HANDLE_FLAG_INHERIT);
		GetWindowTextA(hWnd, String, 255);
		String[254] = 0;
		v2 = strlen(String);
		String[v2 + 1] = 0;
		v3 = &String[v2];
		String[v2] = 124;
		SetWindowTextA(hWnd, String);
		DiabEdit_SendWndCommand(hWnd, 3u);
		*v3 = 0;
		SetWindowTextA(hWnd, String);
	}
}

// ref: 0x10005A0A
void __fastcall DiabEdit_RestrictAndLimit(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	unsigned char v3; // bl
	char *v4; // eax
	char v5; // cl
	signed int v6; // eax
	signed int v7; // esi
	//char v8; // [esp+7h] [ebp-105h]
	char String[256]; // [esp+8h] [ebp-104h]

	String[0] = nullcharacter;
	v3 = wParam;
	memset(&String[1], 0, 0xFCu);
	*(_WORD *)&String[253] = 0;
	String[255] = 0;
	if ( (_BYTE)wParam == 8 )
		goto LABEL_9;
	if ( (unsigned char)wParam < 0x20u || (unsigned char)wParam > 0x7Eu && (unsigned char)wParam < 0xC0u )
		return;
	v4 = (char *)GetPropA(hWnd, "RESTRICTED");
	if ( !v4 || (v5 = *v4) == 0 )
	{
LABEL_9:
		GetWindowTextA(hWnd, String, 255);
		String[254] = 0;
		v6 = strlen(String);
		v7 = v6;
		if ( v3 == 8 )
		{
			if ( v6 )
			{
				String[v6-1] = 0; // *(&v8 + v6) = 0;
				goto LABEL_14;
			}
		}
		else if ( v6 < (signed int)GetPropA(hWnd, "LIMIT") )
		{
			String[v7] = v3;
			String[v7 + 1] = 0;
LABEL_14:
			SetWindowTextA(hWnd, String);
			goto LABEL_15;
		}
LABEL_15:
		DiabEdit_GetCursorProp(hWnd);
		return;
	}
	while ( v3 != v5 )
	{
		v5 = *++v4;
		if ( !*v4 )
			goto LABEL_9;
	}
}

// ref: 0x10005AF4
void __fastcall DiabEdit_SetTextAndProp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	WPARAM v4; // ebx
	size_t v5; // eax
	//char v6; // [esp+Bh] [ebp-101h]
	char String[256]; // [esp+Ch] [ebp-100h]

	String[0] = nullcharacter;
	memset(&String[1], 0, 0xFCu);
	*(_WORD *)&String[253] = 0;
	String[255] = 0;
	v4 = wParam;
	GetWindowTextA(hWnd, String, 255);
	String[254] = 0;
	v5 = strlen(String);
	if ( v4 == 37 )
	{
		if ( v5 )
		{
			String[v5-1] = 0; // *(&v6 + v5) = 0;
			SetWindowTextA(hWnd, String);
		}
		DiabEdit_GetCursorProp(hWnd);
	}
}

// ref: 0x10005B70
void __fastcall DiabEdit_SetRestrictString(HWND hWnd, LPARAM lParam)
{
	const char *v2; // edi
	char *v3; // eax MAPDST

	v2 = (const char *)lParam;
	v3 = (char *)GetPropA(hWnd, "RESTRICTED");
	if ( v3 )
	{
		strncpy(v3, v2, 0xFFu);
		v3[255] = 0;
	}
}

// ref: 0x10005B9F
void __fastcall DiabEdit_SetRestrictTimer(HWND hWnd)
{
	unsigned char *v2; // eax

	SDlgSetTimer((int)hWnd, 1, 500, 0);
	SetPropA(hWnd, "CURSOR", 0);
	v2 = (unsigned char *)SMemAlloc(0x100u, "C:\\Src\\Diablo\\DiabloUI\\DiabEdit.cpp", 185, 0);
	*v2 = 0;
	SetPropA(hWnd, "RESTRICTED", v2);
}

// ref: 0x10005BE7
void __fastcall DiabEdit_RemoveAllProps(HWND hWnd)
{
	HANDLE v2; // eax

	SDlgKillTimer((int)hWnd, 1);
	RemovePropA(hWnd, "LIMIT");
	RemovePropA(hWnd, "CURSOR");
	v2 = RemovePropA(hWnd, "RESTRICTED");
	if ( v2 )
		SMemFree(v2, "C:\\Src\\Diablo\\DiabloUI\\DiabEdit.cpp", 200, 0);
}
