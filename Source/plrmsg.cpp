#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

static BYTE plr_msg_slot;
_plrmsg plr_msgs[PMSG_COUNT];

/** Maps from player_num to text colour, as used in chat messages. */
const char text_color_from_player_num[MAX_PLRS + 1] = { COL_WHITE, COL_WHITE, COL_WHITE, COL_WHITE, COL_GOLD };

void plrmsg_delay(BOOL delay)
{
	int i;
	_plrmsg *pMsg;
	static DWORD plrmsg_ticks;

	if (delay) {
		plrmsg_ticks = -SDL_GetTicks();
		return;
	}

	plrmsg_ticks += SDL_GetTicks();
	pMsg = plr_msgs;
	for (i = 0; i < PMSG_COUNT; i++, pMsg++)
		pMsg->time += plrmsg_ticks;
}

char *ErrorPlrMsg(const char *pszMsg)
{
	char *result;
	_plrmsg *pMsg = &plr_msgs[plr_msg_slot];
	plr_msg_slot = (plr_msg_slot + 1) & (PMSG_COUNT - 1);
	pMsg->player = MAX_PLRS;
	pMsg->time = SDL_GetTicks();
	result = strncpy(pMsg->str, pszMsg, sizeof(pMsg->str));
	pMsg->str[sizeof(pMsg->str) - 1] = '\0';
	return result;
}

size_t EventPlrMsg(const char *pszFmt, ...)
{
	_plrmsg *pMsg;
	va_list va;

	va_start(va, pszFmt);
	pMsg = &plr_msgs[plr_msg_slot];
	plr_msg_slot = (plr_msg_slot + 1) & (PMSG_COUNT - 1);
	pMsg->player = MAX_PLRS;
	pMsg->time = SDL_GetTicks();
	vsprintf(pMsg->str, pszFmt, va);
	va_end(va);
	return strlen(pMsg->str);
}

void SendPlrMsg(int pnum, const char *pszStr)
{
	_plrmsg *pMsg = &plr_msgs[plr_msg_slot];
	plr_msg_slot = (plr_msg_slot + 1) & (PMSG_COUNT - 1);
	pMsg->player = pnum;
	pMsg->time = SDL_GetTicks();
	strlen(plr[pnum]._pName); /* these are used in debug */
	strlen(pszStr);
	sprintf(pMsg->str, "%s (lvl %d): %s", plr[pnum]._pName, plr[pnum]._pLevel, pszStr);
}

void ClearPlrMsg()
{
	int i;
	_plrmsg *pMsg = plr_msgs;
	DWORD tick = SDL_GetTicks();

	for (i = 0; i < PMSG_COUNT; i++, pMsg++) {
		if ((int)(tick - pMsg->time) > 10000)
			pMsg->str[0] = '\0';
	}
}

void InitPlrMsg()
{
	memset(plr_msgs, 0, sizeof(plr_msgs));
	plr_msg_slot = 0;
}

void DrawPlrMsg()
{
	int i;
	DWORD x = (10 + SCREEN_X);
	DWORD y = (70 + SCREEN_Y);
	DWORD width = SCREEN_WIDTH - 20;
	_plrmsg *pMsg;

	if (chrflag || questlog) {
		x = (330 + SCREEN_X);
		width -= 300;
	}
	if (invflag || sbookflag)
		width -= 300;

	if (width < 300)
		return;

	pMsg = plr_msgs;
	for (i = 0; i < 8; i++) {
		if (pMsg->str[0])
			PrintPlrMsg(x, y, width, pMsg->str, text_color_from_player_num[pMsg->player]);
		pMsg++;
		y += 35;
	}
}

void PrintPlrMsg(DWORD x, DWORD y, DWORD width, const char *str, BYTE col)
{
	int line = 0;

	while (*str) {
		BYTE c;
		int sx = x;
		DWORD len = 0;
		const char *sstr = str;
		const char *endstr = sstr;

		while (1) {
			if (*sstr) {
				c = gbFontTransTbl[(BYTE)*sstr++];
				c = fontframe[c];
				len += fontkern[c] + 1;
				if (!c) // allow wordwrap on blank glyph
					endstr = sstr;
				else if (len >= width)
					break;
			} else {
				endstr = sstr;
				break;
			}
		}

		while (str < endstr) {
			c = gbFontTransTbl[(BYTE)*str++];
			c = fontframe[c];
			if (c)
				PrintChar(sx, y, c, col);
			sx += fontkern[c] + 1;
		}

		y += 10;
		line++;
		if (line == 3)
			break;
	}
}

DEVILUTION_END_NAMESPACE
