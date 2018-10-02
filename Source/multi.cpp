//HEADER_GOES_HERE

#include "../types.h"

#ifndef NO_GLOBALS
char gbSomebodyWonGameKludge; // weak
char pkdata_6761C0[4100];
char szPlayerDescript[128];
short sgwPackPlrOffsetTbl[MAX_PLRS];
PkPlayerStruct netplr[MAX_PLRS];
char sgbPlayerTurnBitTbl[MAX_PLRS];
char sgbPlayerLeftGameTbl[MAX_PLRS];
int multi_cpp_init_value; // weak
int sgbSentThisCycle; // idb
int dword_678628; // weak
char gbActivePlayers; // weak
char gbGameDestroyed; // weak
char sgbSendDeltaTbl[MAX_PLRS];
_gamedata sgGameInitInfo;
char byte_678640; // weak
int sglTimeoutStart; // weak
int sgdwPlayerLeftReasonTbl[MAX_PLRS];
char pkdata_678658[4100];
unsigned int sgdwGameLoops; // idb
UCHAR gbMaxPlayers; // weak
char sgbTimeout; // weak
char szPlayerName[128];
char gbDeltaSender; // weak
int sgbNetInited; // weak
int player_state[MAX_PLRS];
#endif

const int multi_inf = 0x7F800000; // weak
const int event_types[3] =
{
  EVENT_TYPE_PLAYER_LEAVE_GAME,
  EVENT_TYPE_PLAYER_CREATE_GAME,
  EVENT_TYPE_PLAYER_MESSAGE
};

struct multi_cpp_init
{
	multi_cpp_init()
	{
		multi_cpp_init_value = multi_inf;
	}
} _multi_cpp_init;
// 47F154: using guessed type int multi_inf;
// 678620: using guessed type int multi_cpp_init_value;

void __fastcall multi_msg_add(unsigned char *a1, unsigned char a2)
{
	if ( a1 )
	{
		if ( a2 )
			tmsg_add(a1, a2);
	}
}

void __fastcall NetSendLoPri(unsigned char *pbMsg, unsigned char bLen)
{
	unsigned char *v2; // esi
	unsigned char v3; // bl
	int v4; // edx

	v2 = pbMsg;
	v3 = bLen;
	if ( pbMsg )
	{
		if ( bLen )
		{
			multi_copy_packet(pkdata_678658, pbMsg, bLen);
			_LOBYTE(v4) = v3;
			multi_send_packet(v2, v4);
		}
	}
}

void __fastcall multi_copy_packet(void *a1, void *packet, int size)
{
	int v3; // eax
	int v4; // ebx
	char *v5; // esi

	v3 = *(_DWORD *)a1;
	v4 = *(_DWORD *)a1 + (unsigned char)size;
	if ( (unsigned int)(v4 + 2) <= 0x1000 )
	{
		*(_DWORD *)a1 = v4 + 1;
		*((_BYTE *)a1 + v3 + 4) = size;
		v5 = (char *)a1 + v3 + 5;
		memcpy(v5, packet, (unsigned char)size);
		v5[(unsigned char)size] = 0;
	}
}

void __fastcall multi_send_packet(void *packet, int dwSize)
{
	void *v2; // esi
	unsigned char v3; // bl
	TPkt pkt; // [esp+8h] [ebp-200h]

	v2 = packet;
	v3 = dwSize;
	NetRecvPlrData(&pkt);
	pkt.hdr.wLen = v3 + 19;
	memcpy(pkt.body, v2, v3);
	if ( !SNetSendMessage(myplr, &pkt.hdr, (unsigned short)pkt.hdr.wLen) )
		nthread_terminate_game("SNetSendMessage0");
}

void __fastcall NetRecvPlrData(TPkt *pkt)
{
	pkt->hdr.wCheck = 'ip';
	pkt->hdr.px = plr[myplr].WorldX;
	pkt->hdr.py = plr[myplr].WorldY;
	pkt->hdr.targx = plr[myplr]._ptargx;
	pkt->hdr.targy = plr[myplr]._ptargy;
	pkt->hdr.php = plr[myplr]._pHitPoints;
	pkt->hdr.pmhp = plr[myplr]._pMaxHP;
	pkt->hdr.bstr = plr[myplr]._pBaseStr;
	pkt->hdr.bmag = plr[myplr]._pBaseMag;
	pkt->hdr.bdex = plr[myplr]._pBaseDex;
}

void __fastcall NetSendHiPri(unsigned char *pbMsg, unsigned char bLen)
{
	unsigned char *v2; // edi
	unsigned char v3; // bl
	int v4; // edx
	unsigned char *v5; // eax
	TSyncHeader *v6; // eax
	int v7; // eax
	int v8; // eax
	TPkt pkt; // [esp+Ch] [ebp-204h]
	int size; // [esp+20Ch] [ebp-4h]

	v2 = pbMsg;
	v3 = bLen;
	if ( pbMsg && bLen )
	{
		multi_copy_packet(pkdata_6761C0, pbMsg, bLen);
		_LOBYTE(v4) = v3;
		multi_send_packet(v2, v4);
	}
	if ( !dword_678628 )
	{
		dword_678628 = 1;
		NetRecvPlrData(&pkt);
		size = gdwNormalMsgSize - 19;
		v5 = multi_recv_packet(pkdata_6761C0, pkt.body, &size);
		v6 = (TSyncHeader *)multi_recv_packet(pkdata_678658, v5, &size);
		v7 = sync_all_monsters(v6, size);
		size = v7;
		v8 = gdwNormalMsgSize - v7;
		pkt.hdr.wLen = v8;
		if ( !SNetSendMessage(-2, &pkt.hdr, v8) )
			nthread_terminate_game("SNetSendMessage");
	}
}
// 678628: using guessed type int dword_678628;
// 679760: using guessed type int gdwNormalMsgSize;

unsigned char *__fastcall multi_recv_packet(void *packet, unsigned char *a2, int *a3)
{
	char *v3; // esi
	unsigned char *result; // eax
	char *v5; // ebx
	size_t v6; // edi
	char *v7; // ebx
	unsigned char *v8; // [esp+4h] [ebp-4h]

	v3 = (char *)packet;
	result = a2;
	v8 = a2;
	if ( *(_DWORD *)packet )
	{
		v5 = (char *)packet + 4;
		while ( *v5 )
		{
			v6 = (unsigned char)*v5;
			if ( v6 > *a3 )
				break;
			v7 = v5 + 1;
			memcpy(v8, v7, v6);
			v8 += v6;
			v5 = &v7[v6];
			*a3 -= v6;
		}
		memcpy(v3 + 4, v5, (size_t)&v3[*(_DWORD *)v3 - (_DWORD)v5 + 5]);
		*(_DWORD *)v3 += v3 - v5 + 4;
		result = v8;
	}
	return result;
}

void __fastcall multi_send_msg_packet(int a1, unsigned char *a2, unsigned char len)
{
	//const void *v3; // edx
	signed int v4; // ebx
	unsigned int v5; // edi
	TPkt pkt; // [esp+Ch] [ebp-204h]
	int v8; // [esp+20Ch] [ebp-4h]

	v8 = a1;
	NetRecvPlrData(&pkt);
	pkt.hdr.wLen = len + 19;
	memcpy(pkt.body, a2, len);
	v4 = 1;
	v5 = 0;
	while ( 1 )
	{
		if ( v4 & v8 )
		{
			if ( !SNetSendMessage(v5, &pkt.hdr, len + 19) && SErrGetLastError() != STORM_ERROR_INVALID_PLAYER )
				break;
		}
		++v5;
		v4 *= 2;
		if ( v5 >= 4 )
			return;
	}
	nthread_terminate_game("SNetSendMessage");
}

void __cdecl multi_msg_countdown()
{
	int v0; // esi

	v0 = 0;
	do
	{
		if ( player_state[v0] & 0x20000 )
		{
			if ( gdwMsgLenTbl[v0] == 4 )
				multi_parse_turn(v0, *(_DWORD *)glpMsgTbl[v0]);
		}
		++v0;
	}
	while ( v0 < MAX_PLRS );
}

void __fastcall multi_parse_turn(int pnum, int turn)
{
	int v2; // esi
	unsigned int v3; // esi

	v2 = turn;
	if ( turn < 0 )
		multi_handle_turn_upper_bit(pnum);
	v3 = v2 & 0x7FFFFFFF;
	if ( sgbSentThisCycle < gdwTurnsInTransit + v3 )
	{
		if ( v3 >= 0x7FFFFFFF )
			v3 = (unsigned short)v3;
		sgbSentThisCycle = v3 + gdwTurnsInTransit;
		sgdwGameLoops = 4 * v3 * (unsigned char)byte_679704;
	}
}
// 679704: using guessed type char byte_679704;
// 679738: using guessed type int gdwTurnsInTransit;

void __fastcall multi_handle_turn_upper_bit(int pnum)
{
	signed int v1; // eax

	v1 = 0;
	do
	{
		if ( player_state[v1] & 0x10000 && v1 != pnum )
			break;
		++v1;
	}
	while ( v1 < MAX_PLRS );
	if ( myplr == v1 )
	{
		sgbSendDeltaTbl[pnum] = 1;
	}
	else if ( myplr == pnum )
	{
		gbDeltaSender = v1;
	}
}
// 6796E4: using guessed type char gbDeltaSender;

void __fastcall multi_player_left(int pnum, int reason)
{
	sgbPlayerLeftGameTbl[pnum] = 1;
	sgdwPlayerLeftReasonTbl[pnum] = reason;
	multi_clear_left_tbl();
}

void __cdecl multi_clear_left_tbl()
{
	int v0; // esi

	v0 = 0;
	do
	{
		if ( sgbPlayerLeftGameTbl[v0] )
		{
			if ( gbBufferMsgs == 1 )
				msg_send_drop_pkt(v0, sgdwPlayerLeftReasonTbl[v0]);
			else
				multi_player_left_msg(v0, 1);
			sgbPlayerLeftGameTbl[v0] = 0;
			sgdwPlayerLeftReasonTbl[v0] = 0;
		}
		++v0;
	}
	while ( v0 < MAX_PLRS );
}
// 676194: using guessed type char gbBufferMsgs;

void __fastcall multi_player_left_msg(int pnum, int left)
{
	int v2; // edi
	int v3; // ebx
	int v4; // esi
	char *v5; // eax
	int v6; // edi

	v2 = pnum;
	v3 = left;
	v4 = pnum;
	if ( plr[pnum].plractive )
	{
		RemovePlrFromMap(pnum);
		RemovePortalMissile(v2);
		DeactivatePortal(v2);
		RemovePlrPortal(v2);
		RemovePlrMissiles(v2);
		if ( v3 )
		{
			v5 = "Player '%s' just left the game";
			v6 = sgdwPlayerLeftReasonTbl[v2] - 0x40000004;
			if ( v6 )
			{
				if ( v6 == 2 )
					v5 = "Player '%s' dropped due to timeout";
			}
			else
			{
				v5 = "Player '%s' killed Diablo and left the game!";
				gbSomebodyWonGameKludge = 1;
			}
			EventPlrMsg(v5, plr[v4]._pName);
		}
		plr[v4].plractive = 0;
		plr[v4]._pName[0] = 0;
		--gbActivePlayers;
	}
}
// 6761B8: using guessed type char gbSomebodyWonGameKludge;
// 67862C: using guessed type char gbActivePlayers;

void __cdecl multi_net_ping()
{
	sgbTimeout = 1;
	sglTimeoutStart = GetTickCount();
}
// 678644: using guessed type int sglTimeoutStart;
// 679661: using guessed type char sgbTimeout;

int __cdecl multi_handle_delta()
{
	int v0; // esi
	int recieved; // [esp+4h] [ebp-4h]

	if ( gbGameDestroyed )
	{
		gbRunGame = 0;
		return 0;
	}
	v0 = 0;
	do
	{
		if ( sgbSendDeltaTbl[v0] )
		{
			sgbSendDeltaTbl[v0] = 0;
			DeltaExportData(v0);
		}
		++v0;
	}
	while ( v0 < MAX_PLRS );
	sgbSentThisCycle = nthread_send_and_recv_turn(sgbSentThisCycle, 1);
	if ( !nthread_recv_turns(&recieved) )
	{
		multi_begin_timeout();
		return 0;
	}
	sgbTimeout = 0;
	if ( recieved )
	{
		if ( dword_678628 )
		{
			dword_678628 = 0;
			if ( !multi_check_pkt_valid(pkdata_6761C0) )
				NetSendHiPri(0, 0);
		}
		else
		{
			NetSendHiPri(0, 0);
			dword_678628 = 0;
		}
	}
	multi_mon_seeds();
	return 1;
}
// 525650: using guessed type int gbRunGame;
// 678628: using guessed type int dword_678628;
// 67862D: using guessed type char gbGameDestroyed;
// 679661: using guessed type char sgbTimeout;

// Microsoft VisualC 2-11/net runtime
int __fastcall multi_check_pkt_valid(char *a1)
{
	return *(_DWORD *)a1 == 0;
}

void __cdecl multi_mon_seeds()
{
	unsigned int v0; // eax
	int v1; // edx
	int *v2; // ecx
	int v3; // esi

	v0 = _rotr(++sgdwGameLoops, 8);
	v1 = 0;
	v2 = &monster[0]._mAISeed;
	do
	{
		v3 = v1++ + v0;
		*v2 = v3;
		v2 += 57;
	}
	while ( (signed int)v2 < (signed int)&monster[MAXMONSTERS]._mAISeed );
}

void __cdecl multi_begin_timeout()
{
	unsigned char bGroupPlayers; // bl
	signed int v1; // eax
	signed int nLowestActive; // esi
	signed int nLowestPlayer; // edi
	signed int v4; // eax
	int v5; // edx
	unsigned char v6; // [esp+Fh] [ebp-1h]

	bGroupPlayers = 0;
#ifdef _DEBUG
	if ( sgbTimeout && !debug_mode_key_i )
#else
	if ( sgbTimeout )
#endif
	{
		v1 = GetTickCount() - sglTimeoutStart;
		if ( v1 <= 20000 )
		{
			if ( v1 >= 10000 )
			{
				v6 = 0;
				nLowestActive = -1;
				nLowestPlayer = -1;
				v4 = 0;
				do
				{
					v5 = player_state[v4];
					if ( v5 & 0x10000 )
					{
						if ( nLowestPlayer == -1 )
							nLowestPlayer = v4;
						if ( v5 & 0x40000 )
						{
							++bGroupPlayers;
							if ( nLowestActive == -1 )
								nLowestActive = v4;
						}
						else
						{
							++v6;
						}
					}
					++v4;
				}
				while ( v4 < MAX_PLRS );
				if ( bGroupPlayers >= v6 && (bGroupPlayers != v6 || nLowestPlayer == nLowestActive) )
				{
					if ( nLowestActive == myplr )
						multi_check_drop_player();
				}
				else
				{
					gbGameDestroyed = 1;
				}
			}
		}
		else
		{
			gbRunGame = 0;
		}
	}
}
// 525650: using guessed type int gbRunGame;
// 67862D: using guessed type char gbGameDestroyed;
// 678644: using guessed type int sglTimeoutStart;
// 679661: using guessed type char sgbTimeout;

void __cdecl multi_check_drop_player()
{
	int v0; // esi
	int v1; // eax

	v0 = 0;
	do
	{
		v1 = player_state[v0];
		if ( !(v1 & 0x40000) )
		{
			if ( v1 & 0x10000 )
				SNetDropPlayer(v0, 0x40000006);
		}
		++v0;
	}
	while ( v0 < MAX_PLRS );
}

void __cdecl multi_process_network_packets()
{
	//int v0; // eax
	TPktHdr *v1; // ecx
	TPktHdr *v2; // edi
	int v3; // eax
	bool v4; // zf
	unsigned char *v5; // esi
	int v6; // ebx
	int v7; // eax
	int v8; // ecx
	int v9; // eax
	int v10; // eax
	int v11; // esi
	int v12; // eax
	int v13; // ecx
	int v14; // eax
	//int v15; // eax
	TPktHdr *pkt; // [esp+0h] [ebp-Ch]
	int len; // [esp+4h] [ebp-8h]
	char arglist[4]; // [esp+8h] [ebp-4h] /* fix, int */

	multi_clear_left_tbl();
	multi_process_tmsgs();
	//_LOBYTE(v0) = SNetReceiveMessage((int *)arglist, (char **)&pkt, &len);
	if ( SNetReceiveMessage((int *)arglist, (char **)&pkt, &len) )
	{
		do
		{
			++dword_676198;
			multi_clear_left_tbl();
			v1 = pkt;
			v2 = pkt;
			if ( (unsigned int)len >= 0x13
			  && *(_DWORD *)arglist < 4u
			  && pkt->wCheck == 'ip'
			  && (unsigned short)pkt->wLen == len )
			{
				v3 = *(_DWORD *)arglist;
				v4 = *(_DWORD *)arglist == myplr;
				plr[v3]._pownerx = (unsigned char)pkt->px;
				v5 = &v1->py;
				plr[v3]._pownery = (unsigned char)v1->py;
				if ( !v4 )
				{
					v4 = gbBufferMsgs == 1;
					plr[v3]._pHitPoints = v1->php;
					plr[v3]._pMaxHP = v1->pmhp;
					plr[v3]._pBaseStr = (unsigned char)v1->bstr;
					plr[v3]._pBaseMag = (unsigned char)v1->bmag;
					plr[v3]._pBaseDex = (unsigned char)v1->bdex;
					if ( !v4 && plr[v3].plractive && plr[v3]._pHitPoints )
					{
						if ( currlevel != plr[v3].plrlevel || plr[v3]._pLvlChanging )
						{
							plr[v3].WorldX = (unsigned char)v1->px;
							plr[v3].WorldY = (unsigned char)*v5;
							plr[v3]._px = (unsigned char)v1->px;
							plr[v3]._py = (unsigned char)*v5;
							plr[v3]._ptargx = (unsigned char)v1->targx;
							plr[v3]._ptargy = (unsigned char)v1->targy;
						}
						else
						{
							v6 = abs(plr[v3].WorldX - (unsigned char)v1->px);
							v7 = abs(plr[*(_DWORD *)arglist].WorldY - (unsigned char)*v5);
							if ( (v6 > 3 || v7 > 3) && !dPlayer[(unsigned char)v2->px][(unsigned char)*v5] )
							{
								FixPlrWalkTags(*(int *)arglist);
								v8 = *(_DWORD *)arglist;
								v9 = *(_DWORD *)arglist;
								plr[v9]._poldx = plr[*(_DWORD *)arglist].WorldX;
								plr[v9]._poldy = plr[v9].WorldY;
								FixPlrWalkTags(v8);
								v10 = *(_DWORD *)arglist;
								plr[v10].WorldX = (unsigned char)v2->px;
								plr[v10].WorldY = (unsigned char)*v5;
								plr[v10]._px = (unsigned char)v2->px;
								plr[v10]._py = (unsigned char)*v5;
								dPlayer[plr[v10].WorldX][plr[v10].WorldY] = arglist[0] + 1;
							}
							v11 = abs(plr[*(_DWORD *)arglist]._px - plr[*(_DWORD *)arglist].WorldX);
							v12 = abs(plr[*(_DWORD *)arglist]._py - plr[*(_DWORD *)arglist].WorldY);
							v13 = *(_DWORD *)arglist;
							if ( v11 > 1 || v12 > 1 )
							{
								v14 = *(_DWORD *)arglist;
								plr[v14]._px = plr[*(_DWORD *)arglist].WorldX;
								plr[v14]._py = plr[v13].WorldY;
							}
							MakePlrPath(v13, (unsigned char)v2->targx, (unsigned char)v2->targy, 1u);
						}
					}
				}
				multi_handle_all_packets(*(int *)arglist, (TPkt *)&v2[1], len - 19);
			}
			//_LOBYTE(v15) = SNetReceiveMessage((int *)arglist, (char **)&pkt, &len);
		}
		while ( SNetReceiveMessage((int *)arglist, (char **)&pkt, &len) );
	}
	if ( SErrGetLastError() != STORM_ERROR_NO_MESSAGES_WAITING )
		nthread_terminate_game("SNetReceiveMsg");
}
// 676194: using guessed type char gbBufferMsgs;
// 676198: using guessed type int dword_676198;

void __fastcall multi_handle_all_packets(int players, TPkt *packet, int a3)
{
	TCmd *v3; // esi
	int i; // edi
	int v5; // eax

	v3 = (TCmd *)packet;
	for ( i = players; a3; a3 -= v5 )
	{
		v5 = ParseCmd(i, v3);
		if ( !v5 )
			break;
		v3 += v5;
	}
}

void __cdecl multi_process_tmsgs()
{
	int v0; // eax
	TPkt pkt; // [esp+0h] [ebp-200h]

	while ( 1 )
	{
		v0 = tmsg_get((unsigned char *)&pkt, 512);
		if ( !v0 )
			break;
		multi_handle_all_packets(myplr, &pkt, v0);
	}
}

void __fastcall multi_send_zero_packet(int pnum, char a2, void *pbSrc, int dwLen)
{
	unsigned int v4; // edi
	short v5; // si
	unsigned short dwBody; // ax
	TPkt pkt; // [esp+Ch] [ebp-208h]
	int pnuma; // [esp+20Ch] [ebp-8h]
	int v10; // [esp+210h] [ebp-4h]

	v4 = dwLen;
	_LOBYTE(v10) = a2;
	pnuma = pnum;
	v5 = 0;
	while ( v4 )
	{
		pkt.hdr.wCheck = 'ip';
		pkt.body[0] = v10;
		dwBody = gdwLargestMsgSize - 24;
		pkt.hdr.px = 0;
		pkt.hdr.py = 0;
		pkt.hdr.targx = 0;
		pkt.hdr.targy = 0;
		pkt.hdr.php = 0;
		pkt.hdr.pmhp = 0;
		pkt.hdr.bstr = 0;
		pkt.hdr.bmag = 0;
		pkt.hdr.bdex = 0;
		*(_WORD *)&pkt.body[1] = v5;
		if ( v4 < gdwLargestMsgSize - 24 )
			dwBody = v4;
		*(_WORD *)&pkt.body[3] = dwBody;
		memcpy(&pkt.body[5], pbSrc, dwBody);
		pkt.hdr.wLen = *(_WORD *)&pkt.body[3] + 24;
		if ( !SNetSendMessage(pnuma, &pkt.hdr, *(unsigned short *)&pkt.body[3] + 24) )
		{
			nthread_terminate_game("SNetSendMessage2");
			return;
		}
		pbSrc = (char *)pbSrc + *(unsigned short *)&pkt.body[3];
		v4 -= *(unsigned short *)&pkt.body[3];
		v5 += *(_WORD *)&pkt.body[3];
	}
}
// 67975C: using guessed type int gdwLargestMsgSize;

void __cdecl NetClose()
{
	if ( sgbNetInited )
	{
		sgbNetInited = 0;
		nthread_cleanup();
		dthread_cleanup();
		tmsg_cleanup();
		multi_event_handler(0);
		SNetLeaveGame(3);
		msgcmd_cmd_cleanup();
		if ( (unsigned char)gbMaxPlayers > 1u )
			Sleep(2000);
	}
}
// 679660: using guessed type char gbMaxPlayers;
// 6796E8: using guessed type int sgbNetInited;

char __fastcall multi_event_handler(int a1)
{
	int v1; // edi
	void *(__stdcall *v2)(int, void (__stdcall *)(_SNETEVENT *)); // ebx
	unsigned int v3; // esi
	int v4; // eax
	char *v5; // eax

	v1 = a1;
	v2 = SNetRegisterEventHandler;
	if ( !a1 )
		v2 = SNetUnregisterEventHandler;
	v3 = 0;
	do
	{
		v4 = (int)v2(event_types[v3], multi_handle_events);
		if ( !v4 && v1 )
		{
			v5 = TraceLastError();
			TermMsg("SNetRegisterEventHandler:\n%s", v5);
		}
		++v3;
	}
	while ( v3 < 3 );
	return v4;
}

void __stdcall multi_handle_events(_SNETEVENT *pEvt)
{
	int v1; // ecx
	int *v2; // eax
	int *v3; // eax

	switch ( pEvt->eventid )
	{
		case EVENT_TYPE_PLAYER_CREATE_GAME:
			v3 = (int *)pEvt->data;
			sgGameInitInfo.dwSeed = *v3;
			_LOBYTE(sgGameInitInfo.bDiff) = *((_BYTE *)v3 + 4);
			sgbPlayerTurnBitTbl[pEvt->playerid] = 1;
			break;
		case EVENT_TYPE_PLAYER_LEAVE_GAME:
			v1 = 0;
			sgbPlayerLeftGameTbl[pEvt->playerid] = 1;
			sgbPlayerTurnBitTbl[pEvt->playerid] = 0;
			v2 = (int *)pEvt->data;
			if ( v2 && pEvt->databytes >= 4u )
				v1 = *v2;
			sgdwPlayerLeftReasonTbl[pEvt->playerid] = v1;
			if ( v1 == 0x40000004 )
				gbSomebodyWonGameKludge = 1;
			sgbSendDeltaTbl[pEvt->playerid] = 0;
			dthread_remove_player(pEvt->playerid);
			if ( (unsigned char)gbDeltaSender == pEvt->playerid )
				gbDeltaSender = 4;
			break;
		case EVENT_TYPE_PLAYER_MESSAGE:
			ErrorPlrMsg((char *)pEvt->data);
			break;
	}
}
// 6761B8: using guessed type char gbSomebodyWonGameKludge;
// 6796E4: using guessed type char gbDeltaSender;

int __fastcall NetInit(int bSinglePlayer, int *pfExitProgram)
{
	int v2; // ebx
	int v4; // eax
	//int v5; // ecx
	bool v7; // zf
	//int v9; // eax
	//int v10; // eax
	_SNETPROGRAMDATA ProgramData; // [esp+8h] [ebp-A8h]
	_SNETUIDATA UiData; // [esp+44h] [ebp-6Ch]
	_SNETPLAYERDATA a2; // [esp+94h] [ebp-1Ch]
	int v14; // [esp+A4h] [ebp-Ch]
	unsigned int len; // [esp+A8h] [ebp-8h]
	int *a4; // [esp+ACh] [ebp-4h]

	a4 = pfExitProgram;
	v14 = bSinglePlayer;
	v2 = 0;
	while ( 1 )
	{
		*a4 = 0;
		SetRndSeed(0);
		sgGameInitInfo.dwSeed = time(NULL);
		_LOBYTE(sgGameInitInfo.bDiff) = gnDifficulty;
		memset(&ProgramData, 0, 0x3Cu);
		ProgramData.size = 60;
		ProgramData.programname = "Diablo Retail";
		ProgramData.programdescription = gszVersionNumber;
		ProgramData.programid = 'DRTL';
		ProgramData.versionid = 42;
		ProgramData.maxplayers = MAX_PLRS;
		ProgramData.initdata = &sgGameInitInfo;
		ProgramData.initdatabytes = 8;
		ProgramData.optcategorybits = 15;
		ProgramData.lcid = 1033; /* LANG_ENGLISH */
		memset(&a2, 0, 0x10u);
		a2.size = 16;
		memset(&UiData, 0, 0x50u);
		UiData.size = 80;
		UiData.parentwindow = SDrawGetFrameWindow(0);
		UiData.artcallback = (void (__cdecl *)())UiArtCallback;
		UiData.createcallback = (void (__cdecl *)())UiCreateGameCallback;
		UiData.drawdesccallback = (void (__cdecl *)())UiDrawDescCallback;
		UiData.messageboxcallback = (void (__cdecl *)())UiMessageBoxCallback;
		UiData.soundcallback = (void (__cdecl *)())UiSoundCallback;
		UiData.authcallback = (void (__cdecl *)())UiAuthCallback;
		UiData.getdatacallback = (void (__cdecl *)())UiGetDataCallback;
		UiData.categorycallback = (void (__cdecl *)())UiCategoryCallback;
		UiData.selectnamecallback = mainmenu_select_hero_dialog;
		UiData.changenamecallback = (void (__cdecl *)())mainmenu_create_hero;
		UiData.profilebitmapcallback = UiProfileDraw;
		UiData.profilecallback = UiProfileCallback;
		UiData.profilefields = UiProfileGetString();
		memset(sgbPlayerTurnBitTbl, 0, 4u);
		gbGameDestroyed = 0;
		memset(sgbPlayerLeftGameTbl, 0, 4u);
		memset(sgdwPlayerLeftReasonTbl, 0, 0x10u);
		memset(sgbSendDeltaTbl, 0, 4u);
		memset(plr, 0, 0x15360u);
		memset(sgwPackPlrOffsetTbl, 0, 8u);
		SNetSetBasePlayer(0);
		if ( v14 )
			v4 = multi_init_single(&ProgramData, &a2, &UiData);
		else
			v4 = multi_init_multi(&ProgramData, &a2, &UiData, a4);
		if ( !v4 )
			return 0;
		sgbNetInited = 1;
		sgbTimeout = 0;
		delta_init();
		InitPlrMsg();
		multi_clear_pkt(pkdata_6761C0);
		multi_clear_pkt(pkdata_678658);
		dword_678628 = 0;
		sync_clear_pkt();
		nthread_start(sgbPlayerTurnBitTbl[myplr]);
		dthread_start();
		MI_Dummy(0); /* v5 */
		sgdwGameLoops = 0;
		sgbSentThisCycle = 0;
		gbDeltaSender = myplr;
		gbSomebodyWonGameKludge = 0;
		nthread_send_and_recv_turn(0, 0);
		SetupLocalCoords();
		multi_send_pinfo(-2, CMD_SEND_PLRINFO);
		gbActivePlayers = 1;
		v7 = sgbPlayerTurnBitTbl[myplr] == 0;
		plr[myplr].plractive = 1;
		if ( v7 || msg_wait_resync() )
			break;
		NetClose();
		byte_678640 = 0;
	}
	gnDifficulty = _LOBYTE(sgGameInitInfo.bDiff);
	SetRndSeed(sgGameInitInfo.dwSeed);
	do
	{
		glSeedTbl[v2] = GetRndSeed();
		gnLevelTypeTbl[v2] = InitNewSeed(v2);
		++v2;
	}
	while ( v2 < 17 );
	//_LOBYTE(v9) = SNetGetGameInfo(GAMEINFO_NAME, szPlayerName, 128, len);
	if ( !SNetGetGameInfo(GAMEINFO_NAME, szPlayerName, 128, &len) )
		nthread_terminate_game("SNetGetGameInfo1");
	//_LOBYTE(v10) = SNetGetGameInfo(GAMEINFO_PASSWORD, szPlayerDescript, 128, len);
	if ( !SNetGetGameInfo(GAMEINFO_PASSWORD, szPlayerDescript, 128, &len) )
		nthread_terminate_game("SNetGetGameInfo2");
	return 1;
}
// 6761B8: using guessed type char gbSomebodyWonGameKludge;
// 678628: using guessed type int dword_678628;
// 67862C: using guessed type char gbActivePlayers;
// 67862D: using guessed type char gbGameDestroyed;
// 678640: using guessed type char byte_678640;
// 679661: using guessed type char sgbTimeout;
// 6796E4: using guessed type char gbDeltaSender;
// 6796E8: using guessed type int sgbNetInited;

void __fastcall multi_clear_pkt(char *a1)
{
	*(_DWORD *)a1 = 0;
	a1[4] = 0;
}

void __fastcall multi_send_pinfo(int pnum, char cmd)
{
	char v2; // bl
	int v3; // esi
	PkPlayerStruct pkplr; // [esp+8h] [ebp-4F4h]

	v2 = cmd;
	v3 = pnum;
	PackPlayer(&pkplr, myplr, 1);
	dthread_send_delta(v3, v2, &pkplr, 1266);
}

int __fastcall InitNewSeed(int newseed)
{
	int result; // eax

	result = 0;
	if ( newseed )
	{
		result = 1;
		if ( newseed < 1 || newseed > 4 )
		{
			if ( newseed < 5 || newseed > 8 )
			{
				if ( newseed < 9 || newseed > 12 )
					result = 4;
				else
					result = 3;
			}
			else
			{
				result = 2;
			}
		}
	}
	return result;
}

void __cdecl SetupLocalCoords()
{
	int x; // ecx
	int y; // edx

	if ( !leveldebug || (unsigned char)gbMaxPlayers > 1u )
	{
		currlevel = 0;
		leveltype = 0;
		setlevel = 0;
	}
	x = 75;
	y = 68;
#ifdef _DEBUG
	if ( debug_mode_key_inverted_v || debug_mode_key_d )
	{
		x = 49;
		y = 23;
	}
#endif
	x += plrxoff[myplr];
	y += plryoff[myplr];
	plr[myplr].WorldX = x;
	plr[myplr].WorldY = y;
	plr[myplr]._px = x;
	plr[myplr]._py = y;
	plr[myplr]._ptargx = x;
	plr[myplr]._ptargy = y;
	plr[myplr].plrlevel = currlevel;
	plr[myplr]._pLvlChanging = 1;
	plr[myplr].pLvlLoad = 0;
	plr[myplr]._pmode = PM_NEWLVL;
	plr[myplr].destAction = ACTION_NONE;
}
// 52572C: using guessed type int leveldebug;
// 5BB1ED: using guessed type char leveltype;
// 5CF31D: using guessed type char setlevel;
// 679660: using guessed type char gbMaxPlayers;

int __fastcall multi_init_single(_SNETPROGRAMDATA *client_info, _SNETPLAYERDATA *user_info, _SNETUIDATA *ui_info)
{
	//int v3; // eax
	int result; // eax
	//int v5; // eax
	char *v6; // eax

	//_LOBYTE(v3) = SNetInitializeProvider(0, client_info, user_info, ui_info, &fileinfo);
	if ( SNetInitializeProvider(0, client_info, user_info, ui_info, &fileinfo) )
	{
		ui_info = 0;
		//_LOBYTE(v5) = SNetCreateGame("local", "local", "local", 0, (char *)&sgGameInitInfo.dwSeed, 8, 1, "local", "local", (int *)&ui_info);
		if ( !SNetCreateGame("local", "local", "local", 0, (char *)&sgGameInitInfo.dwSeed, 8, 1, "local", "local", (int *)&ui_info) )
		{
			v6 = TraceLastError();
			TermMsg("SNetCreateGame1:\n%s", v6);
		}
		myplr = 0;
		gbMaxPlayers = 1;
		result = 1;
	}
	else
	{
		SErrGetLastError();
		result = 0;
	}
	return result;
}
// 679660: using guessed type char gbMaxPlayers;

int __fastcall multi_init_multi(_SNETPROGRAMDATA *client_info, _SNETPLAYERDATA *user_info, _SNETUIDATA *ui_info, int *a4)
{
	_SNETPLAYERDATA *v4; // ebx
	signed int i; // edi
	int a6; // [esp+Ch] [ebp-Ch]
	int a2; // [esp+10h] [ebp-8h]
	int type; // [esp+14h] [ebp-4h]

	v4 = user_info;
	a2 = (int)client_info;
	for ( i = 1; ; i = 0 )
	{
		type = 0;
		if ( byte_678640 )
		{
			if ( !UiSelectProvider(0, (_SNETPROGRAMDATA *)a2, v4, ui_info, &fileinfo, &type)
			  && (!i || SErrGetLastError() != STORM_ERROR_REQUIRES_UPGRADE || !multi_upgrade(a4)) )
			{
				return 0;
			}
			if ( type == 'BNET' )
				plr[0].pBattleNet = 1;
		}
		multi_event_handler(1);
		if ( UiSelectGame(1, (_SNETPROGRAMDATA *)a2, v4, ui_info, &fileinfo, &a6) )
			break;
		byte_678640 = 1;
	}
	if ( (unsigned int)a6 >= MAX_PLRS )
		return 0;
	myplr = a6;
	gbMaxPlayers = MAX_PLRS;
	pfile_read_player_from_save();
	if ( type == 'BNET' )
		plr[myplr].pBattleNet = 1;
	return 1;
}
// 678640: using guessed type char byte_678640;
// 679660: using guessed type char gbMaxPlayers;

int __fastcall multi_upgrade(int *a1)
{
	int *v1; // esi
	int result; // eax
	int status; // [esp+4h] [ebp-4h]

	v1 = a1;
	SNetPerformUpgrade((unsigned long *)&status);
	result = 1;
	if ( status && status != 1 )
	{
		if ( status == 2 )
		{
			*v1 = 1;
		}
		else if ( status == -1 )
		{
			DrawDlg("Network upgrade failed");
		}
		result = 0;
	}
	return result;
}

void __fastcall multi_player_joins(int pnum, TCmdPlrInfoHdr *cmd, int a3)
{
	int v3; // ebx
	TCmdPlrInfoHdr *v4; // edi
	short *v5; // esi
	int v6; // esi
	bool v7; // zf
	char *v8; // eax
	int v9; // ST08_4
	unsigned char *v10; // edx
	int v11; // eax
	int v12; // ecx
	int v13; // eax

	v3 = pnum;
	v4 = cmd;
	if ( myplr != pnum )
	{
		v5 = &sgwPackPlrOffsetTbl[pnum];
		if ( *v5 == cmd->wOffset || (*v5 = 0, !cmd->wOffset) )
		{
			if ( !a3 && !*v5 )
			{
				multi_send_pinfo(pnum, CMD_ACK_PLRINFO);
			}
			memcpy((char *)&netplr[v3] + (unsigned short)v4->wOffset, &v4[1], (unsigned short)v4->wBytes);
			*v5 += v4->wBytes;
			if ( *v5 == 1266 )
			{
				*v5 = 0;
				multi_player_left_msg(v3, 0);
				v6 = v3;
				plr[v3]._pGFXLoad = 0;
				UnPackPlayer(&netplr[v3], v3, 1);
				if ( a3 )
				{
					++gbActivePlayers;
					v7 = sgbPlayerTurnBitTbl[v3] == 0;
					plr[v6].plractive = 1;
					v8 = "Player '%s' (level %d) just joined the game";
					if ( v7 )
						v8 = "Player '%s' (level %d) is already in the game";
					EventPlrMsg(v8, plr[v6]._pName, plr[v6]._pLevel);
					LoadPlrGFX(v3, PFILE_STAND);
					SyncInitPlr(v3);
					if ( plr[v6].plrlevel == currlevel )
					{
						if ( (signed int)(plr[v6]._pHitPoints & 0xFFFFFFC0) <= 0 )
						{
							plr[v6]._pgfxnum = 0;
							LoadPlrGFX(v3, PFILE_DEATH);
							v9 = plr[v6]._pDWidth;
							v10 = plr[v6]._pDAnim[0];
							plr[v6]._pmode = 8;
							NewPlrAnim(v3, v10, plr[v6]._pDFrames, 1, v9);
							v11 = plr[v6]._pAnimLen;
							v12 = v11 - 1;
							plr[v6]._pVar8 = 2 * v11;
							v13 = plr[v6].WorldX;
							plr[v6]._pAnimFrame = v12;
							dFlags[v13][plr[v6].WorldY] |= DFLAG_DEAD_PLAYER;
						}
						else
						{
							StartStand(v3, 0);
						}
					}
				}
			}
		}
	}
}
// 67862C: using guessed type char gbActivePlayers;
