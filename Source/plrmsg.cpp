//HEADER_GOES_HERE

#include "../types.h"

#ifndef NO_GLOBALS
int plrmsg_ticks; // weak
char plr_msg_slot; // weak
_plrmsg plr_msgs[8];
#endif

const char text_color_from_player_num[5] = { COL_WHITE, COL_WHITE, COL_WHITE, COL_WHITE, COL_GOLD };

void __fastcall plrmsg_delay(int a1)
{
	_plrmsg *pMsg; // eax
	signed int v2; // ecx

	if ( a1 )
	{
		plrmsg_ticks = -GetTickCount();
	}
	else
	{
		plrmsg_ticks += GetTickCount();
		pMsg = plr_msgs;
		v2 = 8;
		do
		{
			pMsg->time += plrmsg_ticks;
			++pMsg;
			--v2;
		}
		while ( v2 );
	}
}
// 69B7D0: using guessed type int plrmsg_ticks;

char *__fastcall ErrorPlrMsg(char *pszMsg)
{
	_plrmsg *pMsg; // esi
	char *v2; // edi
	char *result; // eax

	pMsg = &plr_msgs[(unsigned char)plr_msg_slot];
	v2 = pszMsg;
	plr_msg_slot = (plr_msg_slot + 1) & 7;
	pMsg->player = 4;
	pMsg->time = GetTickCount();
	result = strncpy(pMsg->str, v2, 0x90u);
	pMsg->str[143] = 0;
	return result;
}
// 69B7D4: using guessed type char plr_msg_slot;

size_t EventPlrMsg(char *pszFmt, ...)
{
	char *v1; // esi
	va_list va; // [esp+Ch] [ebp+8h]

	va_start(va, pszFmt);
	v1 = (char *)&plr_msgs[(unsigned char)plr_msg_slot];
	plr_msg_slot = (plr_msg_slot + 1) & 7;
	v1[4] = 4;
	*(_DWORD *)v1 = GetTickCount();
	v1 += 5;
	vsprintf(v1, pszFmt, va);
	return strlen(v1);
}
// 69B7D4: using guessed type char plr_msg_slot;

void __fastcall SendPlrMsg(int pnum, const char *pszStr)
{
	_plrmsg *pMsg; // esi
	int v3; // ebx
	const char *v4; // ebp
	int v5; // edi
	const char *v6; // ebx

	pMsg = &plr_msgs[(unsigned char)plr_msg_slot];
	v3 = pnum;
	v4 = pszStr;
	plr_msg_slot = (plr_msg_slot + 1) & 7;
	pMsg->player = pnum;
	pMsg->time = GetTickCount();
	v5 = v3;
	v6 = plr[v3]._pName;
	strlen(v6); /* these are used in debug */
	strlen(v4);
	sprintf(pMsg->str, "%s (lvl %d): %s", v6, plr[v5]._pLevel, v4);
}
// 69B7D4: using guessed type char plr_msg_slot;

void __cdecl ClearPlrMsg()
{
	_plrmsg *pMsg; // esi
	DWORD v1; // eax
	signed int v2; // ecx

	pMsg = plr_msgs;
	v1 = GetTickCount();
	v2 = 8;
	do
	{
		if ( (signed int)(v1 - pMsg->time) > 10000 )
			pMsg->str[0] = 0;
		++pMsg;
		--v2;
	}
	while ( v2 );
}

void __cdecl InitPlrMsg()
{
	memset(plr_msgs, 0, 0x4C0u);
	plr_msg_slot = 0;
}
// 69B7D4: using guessed type char plr_msg_slot;

void __cdecl DrawPlrMsg()
{
	int v0; // ebx
	int v1; // ebp
	int v2; // edi
	char *v3; // esi
	signed int v4; // [esp+Ch] [ebp-4h]

	v0 = 74;
	v1 = 230;
	v2 = 620;
	if ( chrflag || questlog )
	{
		if ( invflag || sbookflag )
			return;
		v0 = 394;
		goto LABEL_9;
	}
	if ( invflag || sbookflag )
LABEL_9:
		v2 = 300;
	v3 = plr_msgs[0].str;
	v4 = 8;
	do
	{
		if ( *v3 )
			PrintPlrMsg(v0, v1, v2, v3, (unsigned char)text_color_from_player_num[(unsigned char)*(v3 - 1)]);
		v3 += 152;
		v1 += 35;
		--v4;
	}
	while ( v4 );
}
// 4B8968: using guessed type int sbookflag;
// 69BD04: using guessed type int questlog;

void __fastcall PrintPlrMsg(int no, int x, int y, char *str, int just)
{
	char *v5; // edi
	int *v6; // edx
	int v7; // esi
	char *v8; // edx
	int v9; // esi
	unsigned int v10; // eax
	unsigned char v11; // cl
	unsigned char v12; // cl
	int v13; // eax
	unsigned char v14; // bl
	int v15; // [esp+Ch] [ebp-Ch]
	int *v16; // [esp+10h] [ebp-8h]
	int v17; // [esp+14h] [ebp-4h]
	char *stra; // [esp+24h] [ebp+Ch]

	v17 = 0;
	v5 = str;
	v15 = no;
	if ( *str )
	{
		v6 = &screen_y_times_768[x];
		v16 = v6;
		do
		{
			v7 = *v6;
			v8 = v5;
			v9 = v15 + v7;
			v10 = 0;
			stra = v5;
			while ( 1 )
			{
				v11 = *v8;
				if ( !*v8 )
					break;
				++v8;
				v12 = fontframe[fontidx[v11]];
				v10 += fontkern[v12] + 1;
				if ( v12 )
				{
					if ( v10 >= y )
						goto LABEL_13;
				}
				else
				{
					stra = v8;
				}
			}
			stra = v8;
LABEL_13:
			while ( v5 < stra )
			{
				v13 = (unsigned char)*v5++;
				v14 = fontframe[fontidx[v13]];
				if ( v14 )
					CPrintString(v9, v14, just);
				v9 += fontkern[v14] + 1;
			}
			v6 = v16 + 10;
			++v17;
			v16 += 10;
		}
		while ( v17 != 3 && *v5 );
	}
}
