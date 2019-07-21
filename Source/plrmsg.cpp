#include "diablo.h"

DEVILUTION_BEGIN_NAMESPACE

static BYTE plr_msg_slot;
_plrmsg plr_msgs[PMSG_COUNT];

const char text_color_from_player_num[MAX_PLRS + 1] = { COL_WHITE, COL_WHITE, COL_WHITE, COL_WHITE, COL_GOLD };

void plrmsg_delay(BOOL delay)
{
	int i;
	_plrmsg *pMsg;
	static DWORD plrmsg_ticks;

	if (delay) {
		plrmsg_ticks = -GetTickCount();
		return;
	}

	plrmsg_ticks += GetTickCount();
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
	pMsg->time = GetTickCount();
	result = strncpy(pMsg->str, pszMsg, sizeof(pMsg->str));
	pMsg->str[sizeof(pMsg->str) - 1] = '\0';
	return result;
}

size_t __cdecl EventPlrMsg(const char *pszFmt, ...)
{
	_plrmsg *pMsg;
	va_list va;

	va_start(va, pszFmt);
	pMsg = &plr_msgs[plr_msg_slot];
	plr_msg_slot = (plr_msg_slot + 1) & (PMSG_COUNT - 1);
	pMsg->player = MAX_PLRS;
	pMsg->time = GetTickCount();
	vsprintf(pMsg->str, pszFmt, va);
	va_end(va);
	return strlen(pMsg->str);
}

void SendPlrMsg(int pnum, const char *pszStr)
{
	_plrmsg *pMsg = &plr_msgs[plr_msg_slot];
	plr_msg_slot = (plr_msg_slot + 1) & (PMSG_COUNT - 1);
	pMsg->player = pnum;
	pMsg->time = GetTickCount();
	strlen(plr[pnum]._pName); /* these are used in debug */
	strlen(pszStr);
	sprintf(pMsg->str, "%s (lvl %d): %s", plr[pnum]._pName, plr[pnum]._pLevel, pszStr);
}

void ClearPlrMsg()
{
	int i;
	_plrmsg *pMsg = plr_msgs;
	DWORD tick = GetTickCount();

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
	DWORD x = 74;
	DWORD y = 230;
	DWORD width = 620;
	_plrmsg *pMsg;

	if (chrflag || questlog) {
		if (invflag || sbookflag)
			return;
		x = 394;
		width = 300;
	} else if (invflag || sbookflag)
		width = 300;

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
		int screen = PitchTbl[y] + x;
		const char *sstr = str;
		DWORD len = 0;
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
				CPrintString(screen, c, col);
			screen += fontkern[c] + 1;
		}

		y += 10;
		line++;
		if (line == 3)
			break;
	}
}

DEVILUTION_END_NAMESPACE
