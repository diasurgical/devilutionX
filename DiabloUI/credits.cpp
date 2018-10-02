// ref: 0x10005297
void __cdecl credits_cpp_init()
{
	credits_cpp_float = credits_cpp_float_value;
}
// 1001F408: using guessed type int credits_cpp_float_value;
// 100296EC: using guessed type int credits_cpp_float;

// ref: 0x100052A2
BOOL __stdcall UiCreditsDialog(int a1)
{
	int v1; // eax

	v1 = (int)SDrawGetFrameWindow();
	SDlgDialogBoxParam(ghUiInst, "CREDITS_DIALOG", v1, credits_WndProc, 25);
	return 1;
}
// 10010382: using guessed type _DWORD __stdcall SDrawGetFrameWindow();

// ref: 0x100052C7
LRESULT __stdcall credits_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	HWND v5; // eax

	if ( Msg > 0x111 )
	{
		if ( Msg == 275 )
		{
			credits_CalcPosROP3(hWnd);
			return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
		}
		if ( Msg == 513 || Msg == 516 )
			goto LABEL_12;
		if ( Msg != 528 )
		{
			if ( Msg == 2024 )
			{
				if ( !Fade_CheckRange5() )
					Fade_SetFadeTimer((int)hWnd);
				return 0;
			}
			return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
		}
		if ( (_WORD)wParam != 513 && (_WORD)wParam != 516 )
			return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
LABEL_25:
		Title_KillAndFadeDlg(hWnd);
		return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
	}
	if ( Msg == 273 )
		goto LABEL_25;
	if ( Msg != 2 )
	{
		if ( Msg == 135 )
			return 4;
		if ( Msg != 256 )
		{
			if ( Msg > 0x103 )
			{
				if ( Msg <= 0x105 )
				{
					v5 = (HWND)SDrawGetFrameWindow();
					SendMessageA(v5, Msg, wParam, lParam);
				}
				else if ( Msg == 272 )
				{
					credits_LoadImgCreditTxt(hWnd, lParam);
					PostMessageA(hWnd, 0x7E8u, 0, 0);
					return 1;
				}
			}
			return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
		}
		if ( wParam != 32 )
			return 0;
LABEL_12:
		Title_KillAndFadeDlg(hWnd);
		return 0;
	}
	credits_FreeCreditResrc(hWnd);
	return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
}
// 10010382: using guessed type _DWORD __stdcall SDrawGetFrameWindow();

// ref: 0x100053D9
void __fastcall credits_FreeCreditResrc(HWND hWnd)
{
	void **v2; // eax

	if ( creditsobj )
	{
		SGdiDeleteObject(creditsobj);
		creditsobj = 0;
	}
	if ( credit_back_img )
	{
		SMemFree(credit_back_img, "C:\\Src\\Diablo\\DiabloUI\\credits.cpp", 46, 0);
		credit_back_img = 0;
	}
	v2 = (void **)GetWindowLongA(hWnd, -21);
	local_FreeMemPtr(v2);
	if ( credittext_rsrc )
	{
		FreeResource(credittext_rsrc);
		credittext_rsrc = 0;
	}
}

// ref: 0x1000543A
void __fastcall credits_LoadImgCreditTxt(HWND hWnd, LPARAM lParam)
{
	signed int v2; // eax
	HRSRC v3; // eax
	HRSRC v4; // eax
	DWORD *v5; // eax
	DWORD *v6; // esi
	int v7; // ebx
	int v8; // esi
	HFONT v9; // eax MAPDST
	BOOL v11; // ebx
	struct tagRECT Rect; // [esp+Ch] [ebp-18h]
	HWND v13; // [esp+1Ch] [ebp-8h]

	if ( lParam )
		v2 = 1000 / lParam;
	else
		v2 = 50;
	SDlgSetTimer((int)hWnd, 1, v2, 0);
	v3 = FindResourceA(ghUiInst, "IDR_CREDITS", "TEXT_FILES");
	credittext_rsrc = LoadResource(ghUiInst, v3);
	v4 = FindResourceA(ghUiInst, "IDR_CREDITS", "TEXT_FILES");
	credittext_size = SizeofResource(ghUiInst, v4);
	v5 = local_AllocWndLongData();
	v6 = v5;
	if ( v5 )
	{
		SetWindowLongA(hWnd, -21, (LONG)v5);
		local_LoadArtWithPal(hWnd, 0, &nullcharacter, -1, 1, "ui_art\\credits.pcx", (BYTE **)v6, v6 + 1, 0);
		Fade_NoInputAndArt(hWnd, 0);
	}
	v13 = GetDlgItem(hWnd, 1000);
	GetWindowRect(v13, &Rect);
	v7 = Rect.right - Rect.left;
	v8 = Rect.bottom - Rect.top + 60;
	credit_back_img = SMemAlloc((Rect.right - Rect.left) * v8, "C:\\Src\\Diablo\\DiabloUI\\credits.cpp", 122, 0);
	credit_horz_pos = v7;
	credit_vertical_pos1 = v8;
	local_AdjustRectSize(&Rect, 0, 30);
	SDlgSetBitmapI(v13, 0, 0, -1, 1, credit_back_img, (int)&Rect, v7, v8, -1);
	credit_vertical_pos2 = v8 - 30;
	credits_CalcPosROP3(hWnd);
	v9 = CreateFontA(-17, 0, 0, 0, 700, 0, 0, 0, 0, 0, 0, 0, 0x12u, "Times New Roman");
	if ( !v9 || (v11 = SGdiImportFont(v9, (int)&creditsobj), DeleteObject(v9), !v11) )
		Title_KillAndFadeDlg(hWnd);
}
// 100296E8: using guessed type int credittext_size;
// 100296FC: using guessed type int credit_vertical_pos1;

// ref: 0x100055C0
void __fastcall credits_CalcPosROP3(HWND hWnd)
{
	_DWORD *v2; // edi
	struct tagRECT Rect; // [esp+Ch] [ebp-14h]
	HWND hWnda; // [esp+1Ch] [ebp-4h]

	hWnda = GetDlgItem(hWnd, 1000);
	v2 = (_DWORD *)GetWindowLongA(hWnd, -21);
	GetWindowRect(hWnda, &Rect);
	ScreenToClient(hWnd, (LPPOINT)&Rect);
	ScreenToClient(hWnd, (LPPOINT)&Rect.right);
	SBltROP3(
		(char *)credit_back_img + 30 * credit_horz_pos,
		(void *)(Rect.left + *v2 + Rect.top * v2[1]),
		credit_horz_pos,
		Rect.bottom - Rect.top,
		credit_horz_pos,
		v2[1],
		0,
		0xCC0020u);
	--credit_vertical_pos2;
	credits_PrintCredLines(hWnd);
	InvalidateRect(hWnda, 0, 0);
	UpdateWindow(hWnda);
}

// ref: 0x10005660
void __fastcall credits_PrintCredLines(HWND hWnd)
{
	char *v1; // esi
	int i; // edi
	int v3; // ebp
	int v4; // ebx

	v1 = (char *)LockResource(credittext_rsrc);
	credit_line_count = credittext_size;
	SGdiSelectObject((int)creditsobj);
	SGdiSetPitch(credit_horz_pos);
	for ( i = credit_vertical_pos2; credit_line_count > 0; v1 = credits_GetAdjustText(v1, v4) )
	{
		v3 = 0;
		while ( *v1 == 9 )
		{
			v3 += 40;
			++v1;
			--credit_line_count;
		}
		v4 = credits_GetCredLineBreak(v1);
		if ( v4 == -1 )
			break;
		if ( i >= 0 )
		{
			if ( i > credit_vertical_pos1 - 30 )
				break;
			if ( v4 )
			{
				SGdiTextOut(credit_back_img, v3 + 2, i + 2, 0x1000000, v1, v4);
				SGdiTextOut(credit_back_img, v3, i, 16777440, v1, v4);
			}
		}
		i += 22;
	}
	if ( i < 0 )
		Title_KillAndFadeDlg(hWnd);
}
// 100103A6: using guessed type int __stdcall SGdiSetPitch(_DWORD);
// 100103AC: using guessed type int __stdcall SGdiSelectObject(_DWORD);
// 100296E8: using guessed type int credittext_size;
// 100296FC: using guessed type int credit_vertical_pos1;
// 10029700: using guessed type int credit_line_count;

// ref: 0x10005736
int __fastcall credits_GetCredLineBreak(char *str)
{
	int result; // eax

	result = 0;
	while ( *str != 13 && *str != 10 )
	{
		++result;
		++str;
		if ( result > credit_line_count )
			return -1;
	}
	return result;
}
// 10029700: using guessed type int credit_line_count;

// ref: 0x10005755
char *__fastcall credits_GetAdjustText(char *str, int len)
{
	credit_line_count += -2 - len;
	return &str[len + 2];
}
// 10029700: using guessed type int credit_line_count;
