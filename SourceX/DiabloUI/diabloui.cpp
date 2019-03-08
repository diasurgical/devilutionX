#include "pch.h"
#include <iconv.h>

TTF_Font *font;
int SelectedItemMin = 1;
int SelectedItemMax = 1;
BYTE *FontTables[4];
Art ArtFonts[4][2];
Art ArtLogos[3];
Art ArtFocus[3];
Art ArtBackground;
Art ArtCursor;
Art ArtHero;

void(__stdcall *gfnSoundFunction)(char *file);
void(__stdcall *gfnListFocus)(int value);
void(__stdcall *gfnListSelect)(int value);
void(__stdcall *gfnListEsc)();
UI_Item *gUiItems;
int gUiItemCnt;
bool UiItemsWraps;
char *UiTextInput;
int UiTextInputLen;

int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT = 480;

int fadeValue = 0;
int SelectedItem = 0;

char *errorTitle[] = {
	"SDL Error",
	"Out of Memory Error",
	"SDL Error",
	"Data File Error",
	"SDL Error",
	"Out of Disk Space",
	"SDL Error",
	"Data File Error",
	"Windows 2000 Restricted User Advisory",
	"Read-Only Directory Error",
};
char *errorMessages[] = {
	"Diablo was unable to properly initialize SDL.\nPlease try the following solutions to correct the problem:\n\n    Install the most recent SDL provided for your distribution.\n\nIf you continue to have problems with SDL, create an issue on GitHub:\n    https://github.com/diasurgical/devilutionX/issues\n\n\nThe error encountered while trying to initialize was:\n\n    %s",
	"Diablo has exhausted all the memory on your system.\nMake sure you have at least 512MB of free system memory\n\nThe error encountered was:\n\n    %s",
	"Diablo was unable to open a required file.\nPlease ensure that the diabdat.mpq is in the same folder as Devilution.\nIf this problem persists, try checking that the md5 of diabdat.mpq is either 011bc6518e6166206231080a4440b373 or 68f049866b44688a7af65ba766bef75a.\n\n\nThe problem occurred while trying to load a file.\n\n    %s",
	"Diablo was unable to find the file \"ddraw.dll\", which is a component of Microsoft DirectX.\nPlease run the program \"SETUP.EXE\" on the Diablo CD-ROM and install Microsoft DirectX.\n\nIf you continue to have problems with DirectX, please contact Microsoft's Technical Support at:\n\n    USA telephone: 1-800-426-9400\n    International telephone: 206-882-8080\n    http://www.microsoft.com\n\n\nThe error encountered while trying to initialize DirectX was:\n\n    %s",
	"Diablo was unable to find the file \"dsound.dll\", which is a component of Microsoft DirectX.\nPlease run the program \"SETUP.EXE\" on the Diablo CD-ROM and install Microsoft DirectX.\n\nIf you continue to have problems with DirectX, please contact Microsoft's Technical Support at:\n\n    USA telephone: 1-800-426-9400\n    International telephone: 206-882-8080\n    http://www.microsoft.com\n\n\nThe error encountered while trying to initialize DirectX was:\n\n    %s",
	"Diablo requires at least 10 megabytes of free disk space to run properly.\nThe disk:\n\n%s\n\nhas less than 10 megabytes of free space left.\n\nPlease free some space on your drive and run Diablo again.",
	"Diablo was unable to switch video modes.\nThis is a common problem for computers with more than one video card.\nTo correct this problem, please set your video resolution to 640 x 480 and try running Diablo again.\n\nFor Windows 95 and Windows NT\n    Select \"Settings - Control Panel\" from the \"Start\" menu\n    Run the \"Display\" control panel applet\n    Select the \"Settings\" tab\n    Set the \"Desktop Area\" to \"640 x 480 pixels\"\n\n\nThe error encountered while trying to switch video modes was:\n\n    %s",
	"Diablo cannot read a required data file.\nYour diabdat.mpq may not be in the Devilution folder and not read-only.\nPlease ensure that the filename is in all lower case and try again.\n    %s",
	"In order to install, play or patch Diablo using the Windows 2000 operating system,\nyou will need to log in as either an Administrator or as a Power User.\n\nUsers, also known as Restricted Users, do not have sufficient access to install or play the game properly.\n\nIf you have further questions regarding User Rights in Windows 2000, please refer to your Windows 2000 documentation or contact your system administrator.",
	"Diablo is being run from:\n\n    %s\n\n\nDiablo or the current user does not seem to have write privilages in this directory. Contact your system administrator.\n\nNote that Windows 2000 Restricted Users can not write to the Windows or Program Files directory hierarchies.",
};

DWORD FormatMessage(
    DWORD dwFlags,
    LPCVOID lpSource,
    DWORD dwMessageId,
    DWORD dwLanguageId,
    char *lpBuffer,
    DWORD nSize,
    va_list *Arguments)
{
	DUMMY();
	return 0;
}

int MAKEINTRESOURCE(int i)
{
	switch (i) {
	case IDD_DIALOG1:
		return 0;
	case IDD_DIALOG2:
		return 1;
	case IDD_DIALOG3:
		return 2;
	case IDD_DIALOG4:
		return 3;
	case IDD_DIALOG5:
		return 4;
	case IDD_DIALOG7:
		return 5;
	case IDD_DIALOG8:
		return 6;
	case IDD_DIALOG9:
		return 7;
	case IDD_DIALOG10:
		return 8;
	case IDD_DIALOG11:
		return 9;
	}

	return -1;
}

int DialogBoxParam(HINSTANCE hInstance, int msgId, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam)
{
	char text[1024];
	snprintf(text, 1024, errorMessages[msgId], dwInitParam);

	return SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, errorTitle[msgId], text, window) < 0 ? -1 : 0;
}

BOOL SetDlgItemText(HWND hDlg, int nIDDlgItem, LPCSTR lpString)
{
	return FALSE;
}

BOOL EndDialog(HWND hDlg, INT_PTR nResult)
{
	return FALSE;
}

BOOL SetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
{
	SDL_SetWindowPosition(window, X, Y);

	return TRUE;
}

void __cdecl UiDestroy()
{
	DUMMY();
	mem_free_dbg(ArtHero.data);
	ArtHero.data = NULL;

	mem_free_dbg(font);
	font = NULL;
}

void UiInitList(int min, int max, void(__stdcall *fnFocus)(int value), void(__stdcall *fnSelect)(int value), void(__stdcall *fnEsc)(), UI_Item *items, int itemCnt, bool itemsWraps)
{
	SelectedItem = min;
	SelectedItemMin = min;
	SelectedItemMax = max;
	gfnListFocus = fnFocus;
	gfnListSelect = fnSelect;
	gfnListEsc = fnEsc;
	gUiItems = items;
	gUiItemCnt = itemCnt;
	UiItemsWraps = itemsWraps;
	if (fnFocus)
		fnFocus(min);

	SDL_StopTextInput(); // input is enabled by default
	for (int i = 0; i < itemCnt; i++) {
		if (items[i].type == UI_EDIT) {
			SDL_StartTextInput();
			UiTextInput = items[i].caption;
			UiTextInputLen = items[i].value;
		}
	}
}

void UiPlayMoveSound()
{
	if (gfnSoundFunction)
		gfnSoundFunction("sfx\\items\\titlemov.wav");
}

void UiPlaySelectSound() //TODO play this on menu back step
{
	if (gfnSoundFunction)
		gfnSoundFunction("sfx\\items\\titlslct.wav");
}

void UiFocus(int itemIndex, bool wrap = false)
{
	if (!wrap) {
		if (itemIndex < SelectedItemMin) {
			itemIndex = SelectedItemMin;
			return;
		} else if (itemIndex > SelectedItemMax) {
			itemIndex = SelectedItemMax ?: SelectedItemMin;
			return;
		}
	} else if (itemIndex < SelectedItemMin) {
		itemIndex = SelectedItemMax ?: SelectedItemMin;
	} else if (itemIndex > SelectedItemMax) {
		itemIndex = SelectedItemMin;
	}

	if (SelectedItem == itemIndex)
		return;

	SelectedItem = itemIndex;

	UiPlayMoveSound();

	if (gfnListFocus)
		gfnListFocus(itemIndex);
}

void selhero_CatToName(char *in_buf, char *out_buf, int cnt)
{
	iconv_t cd = iconv_open("ISO_8859-1//TRANSLIT//IGNORE", "UTF-8");
	if (cd == (iconv_t)-1) {
		TermMsg("Failed to load iconv!");
	}

	size_t in_left = strlen(in_buf);
	char output[SDL_TEXTINPUTEVENT_TEXT_SIZE] = "";
	char *iso_buf = output;
	size_t out_left = sizeof(output) - 1;

	while (in_left && out_left) {
		iconv(cd, &in_buf, &in_left, &iso_buf, &out_left);
	}
	iconv_close(cd);
	strncat(out_buf, output, cnt - strlen(out_buf));
}

bool UiFocusNavigation(SDL_Event *event)
{
	if (event->type == SDL_QUIT)
		exit(0);

	if (event->type == SDL_KEYDOWN) {
		switch (event->key.keysym.sym) {
		case SDLK_UP:
			UiFocus(SelectedItem - 1, UiItemsWraps);
			return true;
		case SDLK_DOWN:
			UiFocus(SelectedItem + 1, UiItemsWraps);
			return true;
		case SDLK_TAB:
			if (SDL_GetModState() & KMOD_SHIFT)
				UiFocus(SelectedItem - 1, UiItemsWraps);
			else
				UiFocus(SelectedItem + 1, UiItemsWraps);
			return true;
		case SDLK_PAGEUP:
			UiFocus(SelectedItemMin);
			return true;
		case SDLK_PAGEDOWN:
			UiFocus(SelectedItemMax);
			return true;
		case SDLK_RETURN:
		case SDLK_KP_ENTER:
		case SDLK_SPACE:
			UiFocusNavigationSelect();
			return true;
		}
	}

	if (SDL_IsTextInputActive()) {
		switch (event->type) {
		case SDL_KEYDOWN:
			switch (event->key.keysym.sym) {
			case SDLK_v:
				if (SDL_GetModState() & KMOD_CTRL) {
					selhero_CatToName(SDL_GetClipboardText(), UiTextInput, UiTextInputLen);
				}
				return true;
			case SDLK_BACKSPACE:
			case SDLK_LEFT:
				int nameLen = strlen(UiTextInput);
				if (nameLen > 0) {
					UiTextInput[nameLen - 1] = '\0';
				}
				return true;
			}
			break;
		case SDL_TEXTINPUT:
			selhero_CatToName(event->text.text, UiTextInput, UiTextInputLen);
			return true;
		}
	}

	if (gUiItems && gUiItemCnt && UiItemMouseEvents(event, gUiItems, gUiItemCnt))
		return true;

	if (gfnListEsc && event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_ESCAPE) {
		UiFocusNavigationEsc();
		return true;
	}

	return false;
}

void UiFocusNavigationSelect()
{
	UiPlaySelectSound();
	if (SDL_IsTextInputActive()) {
		if (strlen(UiTextInput) == 0) {
			return;
		}
		SDL_StopTextInput();
		UiTextInput = NULL;
		UiTextInputLen = 0;
	}
	if (gfnListSelect)
		gfnListSelect(SelectedItem);
}

void UiFocusNavigationEsc()
{
	UiPlaySelectSound();
	if (SDL_IsTextInputActive()) {
		SDL_StopTextInput();
		UiTextInput = NULL;
		UiTextInputLen = 0;
	}
	if (gfnListEsc)
		gfnListEsc();
}

bool IsInsideRect(const SDL_Event *event, const SDL_Rect *rect)
{
	const SDL_Point point = { event->button.x, event->button.y };
	return SDL_PointInRect(&point, rect);
}

void LoadArt(char *pszFile, Art *art, int frames, PALETTEENTRY *pPalette)
{
	if (art == NULL || art->data != NULL)
		return;

	if (!SBmpLoadImage(pszFile, 0, 0, 0, &art->width, &art->height, 0))
		return;

	art->data = (BYTE *)malloc(art->width * art->height);
	if (!SBmpLoadImage(pszFile, pPalette, art->data, art->width * art->height, 0, 0, 0))
		return;

	if (art->data == NULL)
		return;

	art->height /= frames;
}

void LoadMaskedArtFont(char *pszFile, Art *art, int frames, int mask)
{
	LoadArt(pszFile, art, frames);
	art->masked = true;
	art->mask = mask;
}

void LoadArtFont(char *pszFile, int size, int color)
{
	LoadMaskedArtFont(pszFile, &ArtFonts[size][color], 256, 32);
}

void LoadUiGFX()
{
	FontTables[AFT_SMALL] = LoadFileInMem("ui_art\\font16.bin", 0);
	FontTables[AFT_MED] = LoadFileInMem("ui_art\\font24.bin", 0);
	FontTables[AFT_BIG] = LoadFileInMem("ui_art\\font30.bin", 0);
	FontTables[AFT_HUGE] = LoadFileInMem("ui_art\\font42.bin", 0);
	LoadArtFont("ui_art\\font16s.pcx", AFT_SMALL, AFC_SILVER);
	LoadArtFont("ui_art\\font16g.pcx", AFT_SMALL, AFC_GOLD);
	LoadArtFont("ui_art\\font24s.pcx", AFT_MED, AFC_SILVER);
	LoadArtFont("ui_art\\font24g.pcx", AFT_MED, AFC_GOLD);
	LoadArtFont("ui_art\\font30s.pcx", AFT_BIG, AFC_SILVER);
	LoadArtFont("ui_art\\font30g.pcx", AFT_BIG, AFC_GOLD);
	LoadArtFont("ui_art\\font42g.pcx", AFT_HUGE, AFC_GOLD);

	LoadMaskedArtFont("ui_art\\smlogo.pcx", &ArtLogos[LOGO_MED], 15);
	LoadMaskedArtFont("ui_art\\focus16.pcx", &ArtFocus[FOCUS_SMALL], 8);
	LoadMaskedArtFont("ui_art\\focus.pcx", &ArtFocus[FOCUS_MED], 8);
	LoadMaskedArtFont("ui_art\\focus42.pcx", &ArtFocus[FOCUS_BIG], 8);
	LoadMaskedArtFont("ui_art\\cursor.pcx", &ArtCursor, 1, 0);
	LoadArt("ui_art\\heros.pcx", &ArtHero, 4);
}

void InitFont()
{
	if (!TTF_WasInit() && TTF_Init() == -1) {
		printf("TTF_Init: %s\n", TTF_GetError());
		exit(1);
	}
	atexit(TTF_Quit);

	// TODO locate font dynamically, and use fallback font if missing
	font = TTF_OpenFont("/usr/share/fonts/truetype/msttcorefonts/Times_New_Roman_Bold.ttf", 17);
	if (font == NULL) {
		printf("TTF_OpenFont: %s\n", TTF_GetError());
		return;
	}

	TTF_SetFontKerning(font, FALSE);
	TTF_SetFontHinting(font, TTF_HINTING_MONO);
}

void UiInitialize()
{
	LoadUiGFX();
	ShowCursor(FALSE);
	InitFont();
}

int __cdecl UiProfileGetString()
{
	DUMMY();
	return 0;
}

char connect_plrinfostr[128];
char connect_categorystr[128];
void __stdcall UiSetupPlayerInfo(char *infostr, _uiheroinfo *pInfo, DWORD type)
{
	DUMMY();
	SStrCopy(connect_plrinfostr, infostr, 128);
	char format[32] = "";
	strncpy(format, (char *)&type, 4);
	strcat(format, " %d %d %d %d %d %d %d %d %d");

	snprintf(
	    connect_categorystr,
	    128,
	    format,
	    pInfo->level,
	    pInfo->heroclass,
	    pInfo->herorank,
	    pInfo->strength,
	    pInfo->magic,
	    pInfo->dexterity,
	    pInfo->vitality,
	    pInfo->gold,
	    pInfo->spawned);
}

BOOL __stdcall UiCopyProtError(int *pdwResult)
{
	UNIMPLEMENTED();
}

void __stdcall UiAppActivate(BOOL bActive)
{
	DUMMY();
}

BOOL __fastcall UiValidPlayerName(char *name)
{
	if (!strlen(name))
		return FALSE;

	if (strpbrk(name, ",<>%&\\\"?*#/:") || strpbrk(name, " "))
		return FALSE;

	for (char *letter = name; *letter; letter++)
		if (*letter < 0x20 || *letter > 0x7E && *letter < 0xC0)
			return FALSE;

	return TRUE;
}

void __cdecl UiProfileCallback()
{
	UNIMPLEMENTED();
}

void __cdecl UiProfileDraw()
{
	UNIMPLEMENTED();
}

BOOL __stdcall UiCategoryCallback(int a1, int a2, int a3, int a4, int a5, _DWORD *a6, _DWORD *a7)
{
	UNIMPLEMENTED();
}

BOOL __stdcall UiGetDataCallback(int game_type, int data_code, void *a3, int a4, int a5)
{
	UNIMPLEMENTED();
}

BOOL __stdcall UiAuthCallback(int a1, char *a2, char *a3, char a4, char *a5, LPSTR lpBuffer, int cchBufferMax)
{
	UNIMPLEMENTED();
}

BOOL __stdcall UiSoundCallback(int a1, int type, int a3)
{
	UNIMPLEMENTED();
}

void __stdcall UiMessageBoxCallback(HWND hWnd, char *lpText, LPCSTR lpCaption, UINT uType)
{
	UNIMPLEMENTED();
}

BOOL __stdcall UiDrawDescCallback(int game_type, COLORREF color, LPCSTR lpString, char *a4, int a5, UINT align, time_t a7,
    HDC *a8)
{
	UNIMPLEMENTED();
}

BOOL __stdcall UiCreateGameCallback(int a1, int a2, int a3, int a4, int a5, int a6)
{
	UNIMPLEMENTED();
}

BOOL __stdcall UiArtCallback(int game_type, unsigned int art_code, PALETTEENTRY *pPalette, void *pBuffer,
    DWORD dwBuffersize, DWORD *pdwWidth, DWORD *pdwHeight, DWORD *pdwBpp)
{
	UNIMPLEMENTED();
}

BOOL __stdcall UiCreatePlayerDescription(_uiheroinfo *info, DWORD mode, char *desc)
{
	char format[32] = "";
	strncpy(format, (char *)&mode, 4);
	strcat(format, " %d %d %d %d %d %d %d %d %d");

	snprintf(
	    desc,
	    128,
	    format,
	    info->level,
	    info->heroclass,
	    info->herorank,
	    info->strength,
	    info->magic,
	    info->dexterity,
	    info->vitality,
	    info->gold,
	    info->spawned);

	return TRUE;
}

void DrawArt(int screenX, int screenY, Art *art, int nFrame, int drawW)
{
	BYTE *src = (BYTE *)art->data + (art->width * art->height * nFrame);
	BYTE *dst = (BYTE *)&gpBuffer->row[screenY].pixels[screenX];
	drawW = drawW ?: art->width;

	for (int i = 0; i < art->height && i + screenY < SCREEN_HEIGHT; i++, src += art->width, dst += ROW_PITCH) {
		for (int j = 0; j < art->width && j + screenX < SCREEN_WIDTH; j++) {
			if (j < drawW && (!art->masked || src[j] != art->mask))
				dst[j] = src[j];
		}
	}
}

int GetCenterOffset(int w, int bw)
{
	if (bw == 0) {
		bw = SCREEN_WIDTH;
	}

	return bw / 2 - w / 2;
}

int GetStrWidth(char *str, int size)
{
	int strWidth = 0;

	for (int i = 0; i < strlen((char *)str); i++) {
		BYTE w = FontTables[size][*(BYTE *)&str[i] + 2];
		if (w)
			strWidth += w;
		else
			strWidth += FontTables[size][0];
	}

	return strWidth;
}

int TextAlignment(UI_Item *item, TXT_JUST align, _artFontTables size)
{
	if (align != JustLeft) {
		int w = GetStrWidth(item->caption, size);
		if (align == JustCentre) {
			return GetCenterOffset(w, item->rect.w);
		} else if (align == JustRight) {
			return item->rect.w - w;
		}
	}

	return 0;
}

void WordWrap(UI_Item *item)
{
	int lineStart = 0;
	int len = strlen((char *)item->caption);
	for (int i = 0; i <= len; i++) {
		if (item->caption[i] == '\n') {
			lineStart = i + 1;
			continue;
		} else if (item->caption[i] != ' ' && i != len) {
			continue;
		}

		if (i != len)
			item->caption[i] = '\0';
		if (GetStrWidth(&item->caption[lineStart], AFT_SMALL) <= item->rect.w) {
			if (i != len)
				item->caption[i] = ' ';
			continue;
		}

		int j;
		for (j = i; j >= lineStart; j--) {
			if (item->caption[j] == ' ') {
				break; // Scan for previous space
			}
		}

		if (j == lineStart) { // Single word longer then width
			if (i == len)
				break;
			j = i;
		}

		if (i != len)
			item->caption[i] = ' ';
		item->caption[j] = '\n';
		lineStart = j + 1;
	}
};

void DrawArtStr(UI_Item *item)
{
	_artFontTables size = AFT_SMALL;
	_artFontColors color = item->flags & UIS_GOLD ? AFC_GOLD : AFC_SILVER;
	TXT_JUST align = JustLeft;

	if (item->flags & UIS_MED)
		size = AFT_MED;
	else if (item->flags & UIS_BIG)
		size = AFT_BIG;
	else if (item->flags & UIS_HUGE)
		size = AFT_HUGE;

	if (item->flags & UIS_CENTER)
		align = JustCentre;
	else if (item->flags & UIS_RIGHT)
		align = JustRight;

	int x = item->rect.x + TextAlignment(item, align, size);

	int sx = x;
	int sy = item->rect.y;
	if (item->flags & UIS_VCENTER)
		sy += (item->rect.h - ArtFonts[size][color].height) / 2;

	for (int i = 0; i < strlen((char *)item->caption); i++) {
		if (item->caption[i] == '\n') {
			sx = x;
			sy += ArtFonts[size][color].height;
			continue;
		}
		BYTE w = FontTables[size][*(BYTE *)&item->caption[i] + 2] ?: FontTables[size][0];
		DrawArt(sx, sy, &ArtFonts[size][color], *(BYTE *)&item->caption[i], w);
		sx += w;
	}

	if (item->type == UI_EDIT && GetAnimationFrame(2, 500)) {
		DrawArt(sx, sy, &ArtFonts[size][color], '|');
	}
}

void LoadPalInMem(PALETTEENTRY *pPal)
{
	for (int i = 0; i < 256; i++) {
		orig_palette[i].peFlags = 0;
		orig_palette[i].peRed = pPal[i].peRed;
		orig_palette[i].peGreen = pPal[i].peGreen;
		orig_palette[i].peBlue = pPal[i].peBlue;
	}
}

void LoadBackgroundArt(char *pszFile)
{
	PALETTEENTRY pPal[256];

	fadeValue = 0;
	LoadArt(pszFile, &ArtBackground, 1, pPal);
	LoadPalInMem(pPal);
	ApplyGamma(logical_palette, orig_palette, 256);
}

int GetAnimationFrame(int frames, int fps)
{
	int frame = (SDL_GetTicks() / fps) % frames;

	return frame > frames ? 0 : frame;
}

void UiFadeIn(int steps)
{
	if (fadeValue < 256) {
		fadeValue += steps;
		if (fadeValue > 256) {
			fadeValue = 256;
		}
	}

	SetFadeLevel(fadeValue);
}

void UiRenderItemDebug(UI_Item item)
{
	item.rect.x += 64; // pal_surface is shifted?
	item.rect.y += 160;
	SDL_FillRect(pal_surface, &item.rect, random(0, 255));
}

void DrawSelector(UI_Item *item = 0)
{
	int size = FOCUS_SMALL;
	if (item->rect.h >= 42)
		size = FOCUS_BIG;
	else if (item->rect.h >= 30)
		size = FOCUS_MED;

	int frame = GetAnimationFrame(8);
	int y = item->rect.y + (item->rect.h - ArtFocus[size].height) / 2; // TODO FOCUS_MED appares higher then the box

	DrawArt(item->rect.x, y, &ArtFocus[size], frame);
	DrawArt(item->rect.x + item->rect.w - ArtFocus[size].width, y, &ArtFocus[size], frame);
}

void DrawEditBox(UI_Item item)
{
	DrawSelector(&item);
	item.rect.x += 43;
	item.rect.y += 1;
	item.rect.w -= 86;
	DrawArtStr(&item);
}

void UiRender()
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		UiFocusNavigation(&event);
	}
	UiRenderItems(gUiItems, gUiItemCnt);
	DrawLogo();
	DrawMouse();
	UiFadeIn();
}

void UiRenderItems(UI_Item *items, int size)
{
	for (int i = 0; i < size; i++) {
		if (items[i].flags & UIS_HIDDEN)
			continue;

		switch (items[i].type) {
		case UI_EDIT:
			DrawEditBox(items[i]);
			break;
		case UI_LIST:
			if (items[i].caption == NULL)
				continue;
			if (SelectedItem == items[i].value)
				DrawSelector(&items[i]);
		case UI_BUTTON:
		case UI_TEXT:
			DrawArtStr(&items[i]);
			break;
		case UI_IMAGE:
			DrawArt(items[i].rect.x, items[i].rect.y, (Art *)items[i].context, items[i].value, items[i].rect.w);
			break;
		default:
			UiRenderItemDebug(items[i]);
			break;
		}
	}
}

bool UiItemMouseEvents(SDL_Event *event, UI_Item *items, int size)
{
	if (event->type != SDL_MOUSEBUTTONDOWN || event->button.button != SDL_BUTTON_LEFT) {
		return false;
	}

	for (int i = 0; i < size; i++) {
		if (!IsInsideRect(event, &items[i].rect)) {
			continue;
		}

		if (items[i].type != UI_BUTTON && items[i].type != UI_LIST) {
			continue;
		}

		if (items[i].type == UI_LIST) {
			if (items[i].caption != NULL && *items[i].caption != '\0') {
				if (gfnListFocus != NULL && SelectedItem != items[i].value) {
					UiFocus(items[i].value);
				} else if (gfnListFocus == NULL || event->button.clicks >= 2) {
					SelectedItem = items[i].value;
					UiFocusNavigationSelect();
				}
			}

			return true;
		}

		if (items[i].context) {
			((void (*)(int value))items[i].context)(items[i].value);
		}
		return true;
	}

	return false;
}

void DrawLogo(int t, int size)
{
	DrawArt(GetCenterOffset(ArtLogos[size].width), t, &ArtLogos[size], GetAnimationFrame(15));
}

void DrawMouse()
{
	SDL_GetMouseState(&MouseX, &MouseY);

	float scaleX;
	SDL_RenderGetScale(renderer, &scaleX, NULL);
	MouseX /= scaleX;
	MouseY /= scaleX;

	SDL_Rect view;
	SDL_RenderGetViewport(renderer, &view);
	MouseX -= view.x;
	MouseY -= view.y;

	DrawArt(MouseX, MouseY, &ArtCursor);
}
