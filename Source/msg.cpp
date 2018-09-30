//HEADER_GOES_HERE

#include "../types.h"

#ifndef NO_GLOBALS
int sgdwOwnerWait; // weak
int msg_cpp_init_value; // weak
int sgdwRecvOffset; // idb
int sgnCurrMegaPlayer; // weak
DLevel sgLevels[NUMLEVELS];
char sbLastCmd; // weak
TMegaPkt *sgpCurrPkt;
char sgRecvBuf[4722];
unsigned char sgbRecvCmd; // idb
LocalLevel sgLocals[NUMLEVELS];
DJunk sgJunk;
TMegaPkt *sgpMegaPkt;
char sgbDeltaChanged; // weak
char sgbDeltaChunks; // weak
int deltaload; // weak
char gbBufferMsgs; // weak
int dword_676198; // weak
int msg_err_timer; // weak
#endif

const int msg_inf = 0x7F800000; // weak

struct msg_cpp_init
{
	msg_cpp_init()
	{
		msg_cpp_init_value = msg_inf;
	}
} _msg_cpp_init;
// 47F14C: using guessed type int msg_inf;
// 65AB1C: using guessed type int msg_cpp_init_value;

void __fastcall msg_send_drop_pkt(int pnum, int reason)
{
	TFakeDropPlr cmd; // [esp+0h] [ebp-8h]

	cmd.dwReason = reason;
	cmd.bCmd = FAKE_CMD_DROPID;
	cmd.bPlr = pnum;
	msg_send_packet(pnum, &cmd, 6);
}

void __fastcall msg_send_packet(int pnum, void *packet, int dwSize)
{
	void *v3; // edi
	TMegaPkt *v4; // eax
	TFakeCmdPlr cmd; // [esp+Ah] [ebp-2h]

	v3 = packet;
	if ( pnum != sgnCurrMegaPlayer )
	{
		sgnCurrMegaPlayer = pnum;
		cmd.bCmd = FAKE_CMD_SETID;
		cmd.bPlr = pnum;
		msg_send_packet(pnum, &cmd, 2);
	}
	v4 = sgpCurrPkt;
	if ( sgpCurrPkt->dwSpaceLeft < (unsigned int)dwSize )
	{
		msg_get_next_packet();
		v4 = sgpCurrPkt;
	}
	memcpy((char *)&v4[1] - v4->dwSpaceLeft, v3, dwSize);
	sgpCurrPkt->dwSpaceLeft -= dwSize;
}
// 65AB24: using guessed type int sgnCurrMegaPlayer;

TMegaPkt *__cdecl msg_get_next_packet()
{
	TMegaPkt *v0; // eax
	TMegaPkt *v1; // ecx
	TMegaPkt *result; // eax

	v0 = (TMegaPkt *)DiabloAllocPtr(32008);
	sgpCurrPkt = v0;
	v0->pNext = 0;
	sgpCurrPkt->dwSpaceLeft = 32000;
	v1 = sgpMegaPkt;
	result = (TMegaPkt *)&sgpMegaPkt;
	while ( v1 )
	{
		result = v1;
		v1 = v1->pNext;
	}
	result->pNext = sgpCurrPkt;
	return result;
}

int __cdecl msg_wait_resync()
{
	int v0; // eax

	msg_get_next_packet();
	sgbDeltaChunks = 0;
	sgnCurrMegaPlayer = -1;
	sgbRecvCmd = CMD_DLEVEL_END;
	gbBufferMsgs = 1;
	sgdwOwnerWait = GetTickCount();
	v0 = UiProgressDialog(ghMainWnd, "Waiting for game data...", 1, msg_wait_for_turns, 20);
	gbBufferMsgs = 0;
	if ( !v0 )
		goto LABEL_6;
	if ( gbGameDestroyed )
	{
		DrawDlg("The game ended");
LABEL_6:
		msg_free_packets();
		return 0;
	}
	if ( sgbDeltaChunks != 21 )
	{
		DrawDlg("Unable to get level data");
		goto LABEL_6;
	}
	return 1;
}
// 65AB18: using guessed type int sgdwOwnerWait;
// 65AB24: using guessed type int sgnCurrMegaPlayer;
// 67618D: using guessed type char sgbDeltaChunks;
// 676194: using guessed type char gbBufferMsgs;
// 67862D: using guessed type char gbGameDestroyed;

void __cdecl msg_free_packets()
{
	TMegaPkt *v0; // eax
	TMegaPkt *v1; // ecx

	v0 = sgpMegaPkt;
	while ( v0 )
	{
		v1 = v0->pNext;
		sgpMegaPkt = 0;
		sgpCurrPkt = v1;
		mem_free_dbg(v0);
		v0 = sgpCurrPkt;
		sgpMegaPkt = sgpCurrPkt;
	}
}

int __cdecl msg_wait_for_turns()
{
	//int v0; // eax
	//int v2; // eax
	int recieved; // [esp+0h] [ebp-8h]
	int turns; // [esp+4h] [ebp-4h]

	if ( !sgbDeltaChunks )
	{
		nthread_send_and_recv_turn(0, 0);
		//_LOBYTE(v0) = SNetGetOwnerTurnsWaiting(&turns);
		if ( !SNetGetOwnerTurnsWaiting(&turns) && SErrGetLastError() == STORM_ERROR_NOT_IN_GAME )
			return 100;
		if ( GetTickCount() - sgdwOwnerWait <= 2000 && turns < (unsigned int)gdwTurnsInTransit )
			return 0;
		++sgbDeltaChunks;
	}
	multi_process_network_packets();
	nthread_send_and_recv_turn(0, 0);
	//_LOBYTE(v2) = nthread_has_500ms_passed();
	if ( nthread_has_500ms_passed() )
		nthread_recv_turns(&recieved);
	if ( gbGameDestroyed )
		return 100;
	if ( (unsigned char)gbDeltaSender >= 4u )
	{
		sgbDeltaChunks = 0;
		sgbRecvCmd = CMD_DLEVEL_END;
		gbDeltaSender = myplr;
		nthread_set_turn_upper_bit();
	}
	if ( sgbDeltaChunks == 20 )
	{
		sgbDeltaChunks = 21;
		return 99;
	}
	return 100 * (unsigned char)sgbDeltaChunks / 21;
}
// 65AB18: using guessed type int sgdwOwnerWait;
// 67618D: using guessed type char sgbDeltaChunks;
// 67862D: using guessed type char gbGameDestroyed;
// 6796E4: using guessed type char gbDeltaSender;
// 679738: using guessed type int gdwTurnsInTransit;

void __cdecl msg_process_net_packets()
{
	if ( gbMaxPlayers != 1 )
	{
		gbBufferMsgs = 2;
		msg_pre_packet();
		gbBufferMsgs = 0;
		msg_free_packets();
	}
}
// 676194: using guessed type char gbBufferMsgs;
// 679660: using guessed type char gbMaxPlayers;

void __cdecl msg_pre_packet()
{
	TMegaPkt *v0; // edi
	int i; // ebp
	signed int v2; // ebx
	TFakeCmdPlr *v3; // esi
	TFakeCmdPlr *v4; // eax
	TFakeDropPlr *v5; // eax
	int v6; // eax

	v0 = sgpMegaPkt;
	for ( i = -1; v0; v0 = v0->pNext )
	{
		v2 = 32000;
		v3 = (TFakeCmdPlr *)v0->data;
		while ( v2 != v0->dwSpaceLeft )
		{
			if ( v3->bCmd == FAKE_CMD_SETID )
			{
				v4 = v3;
				++v3;
				i = (unsigned char)v4->bPlr;
				v2 -= 2;
			}
			else if ( v3->bCmd == FAKE_CMD_DROPID )
			{
				v5 = (TFakeDropPlr *)v3;
				v3 += 3;
				v2 -= 6;
				multi_player_left((unsigned char)v5->bPlr, v5->dwReason);
			}
			else
			{
				v6 = ParseCmd(i, (TCmd *)v3);
				v3 = (TFakeCmdPlr *)((char *)v3 + v6);
				v2 -= v6;
			}
		}
	}
}

void __fastcall DeltaExportData(int pnum)
{
	char *v1; // edi
	DObjectStr *v2; // esi
	void *v3; // ebx
	void *v4; // eax
	void *v5; // eax
	void *v6; // eax
	int v7; // eax
	char *v8; // eax
	int v9; // eax
	int player_num; // [esp+0h] [ebp-Ch]
	int v11; // [esp+4h] [ebp-8h]
	char src; // [esp+Bh] [ebp-1h]

	player_num = pnum;
	if ( sgbDeltaChanged )
	{
		v11 = 0;
		v1 = (char *)DiabloAllocPtr(4722);
		v2 = sgLevels[0].object;
		v3 = v1 + 1;
		do
		{
			v4 = DeltaExportItem(v3, &v2[-2794]);
			v5 = DeltaExportObject(v4, v2);
			v6 = DeltaExportMonster(v5, &v2[127]);
			v7 = msg_comp_level(v1, (int)v6);
			dthread_send_delta(player_num, (_BYTE)v11++ + CMD_DLEVEL_0, v1, v7);
			v2 += 4721;
		}
		while ( (signed int)v2 < (signed int)&sgLevels[NUMLEVELS].object );
		v8 = DeltaExportJunk((char *)v3);
		v9 = msg_comp_level(v1, (int)v8);
		dthread_send_delta(player_num, CMD_DLEVEL_JUNK, v1, v9);
		mem_free_dbg(v1);
	}
	src = 0;
	dthread_send_delta(player_num, CMD_DLEVEL_END, &src, 1);
}
// 67618C: using guessed type char sgbDeltaChanged;

void *__fastcall DeltaExportItem(void *dst, void *src)
{
	_BYTE *v2; // edi
	_BYTE *v3; // esi
	signed int v4; // ebx

	v2 = (unsigned char *)src;
	v3 = (unsigned char *)dst;
	v4 = 127;
	do
	{
		if ( *v2 == -1 )
		{
			*v3++ = -1;
		}
		else
		{
			memcpy(v3, v2, 0x16u);
			v3 += 22;
		}
		v2 += 22;
		--v4;
	}
	while ( v4 );
	return v3;
}

void *__fastcall DeltaExportObject(void *dst, void *src)
{
	char *v2; // esi

	v2 = (char *)dst;
	memcpy(dst, src, 0x7Fu);
	return v2 + 127;
}

void *__fastcall DeltaExportMonster(void *dst, void *src)
{
	_BYTE *v2; // edi
	_BYTE *v3; // esi
	signed int v4; // ebx

	v2 = (unsigned char *)src;
	v3 = (unsigned char *)dst;
	v4 = MAXMONSTERS;
	do
	{
		if ( *v2 == -1 )
		{
			*v3++ = -1;
		}
		else
		{
			memcpy(v3, v2, 9u);
			v3 += 9;
		}
		v2 += 9;
		--v4;
	}
	while ( v4 );
	return v3;
}

char *__fastcall DeltaExportJunk(char *a1)
{
	char *v1; // ebx
	DJunk *v2; // edi
	MultiQuests *v3; // esi
	unsigned char *v4; // edi
	int *v5; // ebp

	v1 = a1;
	v2 = &sgJunk;
	v3 = sgJunk.quests;
	do
	{
		if ( v2->portal[0].x == LOBYTE(-1) )
		{
			*v1++ = -1;
		}
		else
		{
			memcpy(v1, v2, 5u);
			v1 += 5;
		}
		v2 = (DJunk *)((char *)v2 + 5);
	}
	while ( (signed int)v2 < (signed int)sgJunk.quests );
	v4 = &quests[0]._qactive;
	v5 = &questlist[0]._qflags;
	do
	{
		if ( *(_BYTE *)v5 & 1 )
		{
			v3->qlog = v4[18];
			v3->qstate = *v4;
			v3->qvar1 = v4[13];
			memcpy(v1, v3, 3u);
			v1 += 3;
			++v3;
		}
		v5 += 5;
		v4 += 24;
	}
	while ( (signed int)v5 < (signed int)&questlist[16]._qflags );
	return v1;
}

int __fastcall msg_comp_level(char *buffer, int size)
{
	char *v2; // esi
	int v3; // edi
	int v4; // eax

	v2 = buffer;
	v3 = size - (_DWORD)buffer - 1;
	v4 = encrypt_compress(buffer + 1, v3);
	*v2 = v3 != v4;
	return v4 + 1;
}

void __cdecl delta_init()
{
	sgbDeltaChanged = 0;
	memset(&sgJunk, 255, 0x20u);
	memset(sgLevels, 255, 0x13981u);
	memset(sgLocals, 0, 0x6A40u);
	deltaload = 0;
}
// 67618C: using guessed type char sgbDeltaChanged;
// 676190: using guessed type int deltaload;

void __fastcall delta_kill_monster(int mi, unsigned char x, unsigned char y, unsigned char bLevel)
{
	DMonsterStr *v4; // eax
	char v5; // cl

	if ( gbMaxPlayers != 1 )
	{
		v4 = &sgLevels[bLevel].monster[mi];
		sgbDeltaChanged = 1;
		v5 = monster[mi]._mdir;
		v4->_mhitpoints = 0;
		v4->_mx = x;
		v4->_mdir = v5;
		v4->_my = y;
	}
}
// 67618C: using guessed type char sgbDeltaChanged;
// 679660: using guessed type char gbMaxPlayers;

void __fastcall delta_monster_hp(int mi, int hp, unsigned char bLevel)
{
	DMonsterStr *v3; // eax

	if ( gbMaxPlayers != 1 )
	{
		sgbDeltaChanged = 1;
		v3 = &sgLevels[bLevel].monster[mi];
		if ( v3->_mhitpoints > hp )
			v3->_mhitpoints = hp;
	}
}
// 67618C: using guessed type char sgbDeltaChanged;
// 679660: using guessed type char gbMaxPlayers;

void __fastcall delta_sync_monster(TCmdLocParam1 *packet, char level)
{
	DMonsterStr *v2; // eax
	char v3; // dl

	if ( gbMaxPlayers != 1 )
	{
		sgbDeltaChanged = 1;
		v2 = &sgLevels[(unsigned char)level].monster[(unsigned char)packet->bCmd];
		if ( v2->_mhitpoints )
		{
			v2->_mx = packet->x;
			v3 = packet->y;
			v2->_mactive = -1;
			v2->_my = v3;
			v2->_menemy = packet->wParam1;
		}
	}
}
// 67618C: using guessed type char sgbDeltaChanged;
// 679660: using guessed type char gbMaxPlayers;

void __fastcall delta_sync_golem(TCmdGolem *pG, int pnum, int bLevel)
{
	DMonsterStr *v3; // eax
	char v4; // dl

	if ( gbMaxPlayers != 1 )
	{
		sgbDeltaChanged = 1;
		v3 = &sgLevels[(unsigned char)bLevel].monster[pnum];
		v3->_mx = pG->_mx;
		v4 = pG->_my;
		v3->_mactive = -1;
		v3->_my = v4;
		v3->_menemy = pG->_menemy;
		v3->_mdir = pG->_mdir;
		v3->_mhitpoints = pG->_mhitpoints;
	}
}
// 67618C: using guessed type char sgbDeltaChanged;
// 679660: using guessed type char gbMaxPlayers;

void __fastcall delta_leave_sync(unsigned char bLevel)
{
	unsigned char v1; // bl
	bool v2; // zf
	int v3; // eax
	int i; // ebp
	int v5; // ecx
	int v6; // esi
	DMonsterStr *v7; // edi

	v1 = bLevel;
	if ( gbMaxPlayers != 1 )
	{
		v2 = currlevel == 0;
		if ( !currlevel )
		{
			v3 = GetRndSeed();
			v2 = currlevel == 0;
			glSeedTbl[0] = v3;
		}
		if ( !v2 )
		{
			for ( i = 0; i < nummonsters; ++i )
			{
				v5 = monstactive[i];
				v6 = monstactive[i];
				if ( monster[v6]._mhitpoints )
				{
					sgbDeltaChanged = 1;
					v7 = &sgLevels[v1].monster[v5];
					v7->_mx = monster[v6]._mx;
					v7->_my = monster[v6]._my;
					v7->_mdir = monster[v6]._mdir;
					v7->_menemy = encode_enemy(v5);
					v7->_mhitpoints = monster[v6]._mhitpoints;
					v7->_mactive = monster[v6]._msquelch;
				}
			}
			memcpy(&sgLocals[v1], automapview, 0x640u);
		}
	}
}
// 43C17D: could not find valid save-restore pair for edi
// 67618C: using guessed type char sgbDeltaChanged;
// 679660: using guessed type char gbMaxPlayers;

bool __fastcall delta_portal_inited(int i)
{
	return sgJunk.portal[i].x == LOBYTE(-1);
}

bool __fastcall delta_quest_inited(int i)
{
	return sgJunk.quests[i].qstate != LOBYTE(-1);
}

void __fastcall DeltaAddItem(int ii)
{
	int v1; // eax
	int v2; // ecx
	signed int v3; // ebp
	DLevel *v4; // edx
	DLevel *v5; // edi
	char v6; // bl
	int v7; // esi
	signed int v8; // esi
	int v9; // eax
	char v10; // cl
	char v11; // cl

	v1 = ii;
	if ( gbMaxPlayers != 1 )
	{
		v2 = currlevel;
		v3 = 0;
		v4 = &sgLevels[v2];
		v5 = &sgLevels[v2];
		while ( 1 )
		{
			v6 = v5->item[0].bCmd;
			if ( v5->item[0].bCmd != -1 )
			{
				v7 = v1;
				if ( (unsigned short)v5->item[0].wIndx == item[v1].IDidx
				  && v5->item[0].wCI == item[v7]._iCreateInfo
				  && v5->item[0].dwSeed == item[v7]._iSeed
				  && (v6 == 1 || !v6) )
				{
					break;
				}
			}
			++v3;
			v5 = (DLevel *)((char *)v5 + 22);
			if ( v3 >= 127 )
			{
				v8 = 0;
				while ( v4->item[0].bCmd != -1 )
				{
					++v8;
					v4 = (DLevel *)((char *)v4 + 22);
					if ( v8 >= 127 )
						return;
				}
				v4->item[0].bCmd = 0;
				v9 = 368 * v1;
				v10 = *((_BYTE *)&item[0]._ix + v9);
				sgbDeltaChanged = 1;
				v4->item[0].x = v10;
				v4->item[0].y = *((_BYTE *)&item[0]._iy + v9);
				v4->item[0].wIndx = *(_WORD *)((char *)&item[0].IDidx + v9);
				v4->item[0].wCI = *(short *)((char *)&item[0]._iCreateInfo + v9);
				v4->item[0].dwSeed = *(int *)((char *)&item[0]._iSeed + v9);
				v4->item[0].bId = *((_BYTE *)&item[0]._iIdentified + v9);
				v4->item[0].bDur = *((_BYTE *)&item[0]._iDurability + v9);
				v4->item[0].bMDur = *((_BYTE *)&item[0]._iMaxDur + v9);
				v4->item[0].bCh = *((_BYTE *)&item[0]._iCharges + v9);
				v11 = *((_BYTE *)&item[0]._iMaxCharges + v9);
				_LOWORD(v9) = *(_WORD *)((char *)&item[0]._ivalue + v9);
				v4->item[0].bMCh = v11;
				v4->item[0].wValue = v9;
				return;
			}
		}
	}
}
// 67618C: using guessed type char sgbDeltaChanged;
// 679660: using guessed type char gbMaxPlayers;

void __cdecl DeltaSaveLevel()
{
	int v0; // eax
	int v1; // edx
	int *v2; // ecx
	unsigned char v3; // cl

	if ( gbMaxPlayers != 1 )
	{
		v0 = myplr;
		v1 = 0;
		v2 = &plr[0]._pGFXLoad;
		do
		{
			if ( v1 != v0 )
				*v2 = 0;
			v2 += 5430;
			++v1;
		}
		while ( (signed int)v2 < (signed int)&plr[4]._pGFXLoad );
		v3 = currlevel;
		plr[v0]._pLvlVisited[currlevel] = 1;
		delta_leave_sync(v3);
	}
}
// 679660: using guessed type char gbMaxPlayers;

void __cdecl DeltaLoadLevel()
{
	int v0; // ebx
	int *v1; // esi
	int v2; // eax
	int v3; // ecx
	int v4; // edx
	int v5; // edi
	char v6; // al
	int v7; // eax
	signed int v8; // esi
	int v9; // eax
	char v10; // cl
	int v11; // eax
	char *v12; // edx
	int v13; // eax
	int v14; // ebx
	int *v15; // edx
	unsigned short v16; // cx
	int v17; // ST1C_4
	int v18; // ST18_4
	int v19; // eax
	int v20; // ecx
	int v21; // edx
	int v22; // eax
	int v23; // eax
	int v24; // esi
	int v25; // edi
	int v26; // eax
	int v27; // eax
	int v28; // esi
	unsigned char v29; // al
	int j; // esi
	int v31; // eax
	signed int v32; // [esp+0h] [ebp-24h]
	int v33; // [esp+4h] [ebp-20h]
	int o2; // [esp+8h] [ebp-1Ch]
	int i; // [esp+Ch] [ebp-18h]
	signed int v36; // [esp+10h] [ebp-14h]
	int v37; // [esp+14h] [ebp-10h]
	signed int v38; // [esp+18h] [ebp-Ch]
	signed int v39; // [esp+1Ch] [ebp-8h]
	int v40; // [esp+20h] [ebp-4h]
	signed int v41; // [esp+20h] [ebp-4h]

	if ( gbMaxPlayers != 1 )
	{
		deltaload = 1;
		if ( currlevel )
		{
			v0 = 0;
			if ( nummonsters > 0 )
			{
				v40 = 0;
				v1 = &monster[0]._mfuty;
				do
				{
					if ( sgLevels[currlevel].monster[v40]._mx != -1 )
					{
						M_ClearSquares(v0);
						v2 = v40 * 9 + 4721 * currlevel;
						v3 = *((unsigned char *)&sgLevels[0].monster[0]._mx + v2);
						v4 = *((unsigned char *)&sgLevels[0].monster[0]._my + v2);
						v5 = *(int *)((char *)&sgLevels[0].monster[0]._mhitpoints + v2);
						*(v1 - 3) = v3;
						*(v1 - 2) = v4;
						v1[1] = v3;
						v1[2] = v4;
						*(v1 - 1) = v3;
						*v1 = v4;
						if ( v5 != -1 )
							v1[26] = v5;
						if ( v5 )
						{
							decode_enemy(v0, *((unsigned char *)&sgLevels[0].monster[0]._menemy + v2));
							v7 = *(v1 - 3);
							if ( v7 && v7 != 1 || *(v1 - 2) )
								dMonster[0][*(v1 - 2) + 112 * v7] = v0 + 1;
							if ( (signed int)v1 >= (signed int)&monster[4]._mfuty )
							{
								M_StartStand(v0, v1[7]);
							}
							else
							{
								MAI_Golum(v0);
								v1[28] |= 0x30u;
							}
							*((_BYTE *)v1 + 116) = sgLevels[currlevel].monster[v40]._mactive;
						}
						else
						{
							v1[1] = v3;
							v1[2] = v4;
							M_ClearSquares(v0);
							if ( *((_BYTE *)v1 + 108) != 27 )
							{
								if ( *((_BYTE *)v1 + 144) )
									v6 = *((_BYTE *)v1 + 146);
								else
									v6 = *(_BYTE *)(v1[44] + 317);
								AddDead(*(v1 - 3), *(v1 - 2), v6, (direction)v1[7]);
							}
							v1[16] = 1;
							M_UpdateLeader(v0);
						}
					}
					++v40;
					++v0;
					v1 += 57;
				}
				while ( v0 < nummonsters );
			}
			memcpy(automapview, &sgLocals[currlevel], 0x640u);
		}
		v8 = 0;
		i = 0;
		v32 = 0;
		do
		{
			v9 = v8 + 4721 * currlevel;
			v10 = *(&sgLevels[0].item[0].bCmd + v9);
			if ( v10 != -1 )
			{
				if ( v10 == 1 )
				{
					v11 = FindGetItem(
						*(unsigned short *)((char *)&sgLevels[0].item[0].wIndx + v9),
						*(short *)((char *)&sgLevels[0].item[0].wCI + v9),
						*(int *)((char *)&sgLevels[0].item[0].dwSeed + v9));
					if ( v11 != -1 )
					{
						v12 = &dItem[item[v11]._ix][item[v11]._iy];
						if ( *v12 == v11 + 1 )
							*v12 = 0;
						DeleteItem(v11, i);
					}
				}
				v13 = v8 + 4721 * currlevel;
				if ( *(&sgLevels[0].item[0].bCmd + v13) == 2 )
				{
					v14 = itemavail[0];
					v33 = itemavail[0];
					v15 = &itemavail[-numitems + 126];
					itemactive[numitems] = itemavail[0];
					v16 = *(short *)((char *)&sgLevels[0].item[0].wIndx + v13);
					itemavail[0] = *v15;
					if ( v16 == IDI_EAR )
					{
						RecreateEar(
							v14,
							*(short *)((char *)&sgLevels[0].item[0].wCI + v13),
							*(int *)((char *)&sgLevels[0].item[0].dwSeed + v13),
							*(&sgLevels[0].item[0].bId + v13),
							*((unsigned char *)&sgLevels[0].item[0].bDur + v13),
							*((unsigned char *)&sgLevels[0].item[0].bMDur + v13),
							*((unsigned char *)&sgLevels[0].item[0].bCh + v13),
							*((unsigned char *)&sgLevels[0].item[0].bMCh + v13),
							*(unsigned short *)((char *)&sgLevels[0].item[0].wValue + v13),
							*(int *)((char *)&sgLevels[0].item[0].dwBuff + v13));
					}
					else
					{
						v17 = *(unsigned short *)((char *)&sgLevels[0].item[0].wValue + v13);
						v18 = *(int *)((char *)&sgLevels[0].item[0].dwSeed + v13);
						_LOWORD(v13) = *(short *)((char *)&sgLevels[0].item[0].wCI + v13);
						RecreateItem(v14, v16, v13, v18, v17);
						v19 = v8 + 4721 * currlevel;
						if ( *(&sgLevels[0].item[0].bId + v19) )
							item[v14]._iIdentified = 1;
						v20 = v14;
						item[v20]._iDurability = *((unsigned char *)&sgLevels[0].item[0].bDur + v19);
						item[v20]._iMaxDur = *((unsigned char *)&sgLevels[0].item[0].bMDur + v19);
						v21 = *((unsigned char *)&sgLevels[0].item[0].bCh + v19);
						v22 = *((unsigned char *)&sgLevels[0].item[0].bMCh + v19);
						item[v20]._iCharges = v21;
						item[v20]._iMaxCharges = v22;
					}
					v23 = v8 + 4721 * currlevel;
					v24 = *((unsigned char *)&sgLevels[0].item[0].x + v23);
					v25 = *((unsigned char *)&sgLevels[0].item[0].y + v23);
					if ( !CanPut(v24, v25) )
					{
						v39 = 0;
						v26 = -1;
						v41 = 1;
						v36 = -1;
						do
						{
							if ( v39 )
								break;
							v37 = v26;
							while ( v26 <= v41 && !v39 )
							{
								o2 = v25 + v37;
								v38 = v36;
								do
								{
									if ( v39 )
										break;
									if ( CanPut(v38 + v24, o2) )
									{
										v25 = o2;
										v39 = 1;
										v24 += v38;
									}
									++v38;
									v14 = v33;
								}
								while ( v38 <= v41 );
								v26 = ++v37;
							}
							++v41;
							v26 = v36-- - 1;
						}
						while ( v36 > -50 );
					}
					v27 = v14;
					item[v27]._ix = v24;
					item[v27]._iy = v25;
					dItem[v24][v25] = v14 + 1;
					RespawnItem(v14, 0);
					++numitems;
					v8 = v32;
				}
			}
			++i;
			v8 += 22;
			v32 = v8;
		}
		while ( v8 < 2794 );
		if ( currlevel )
		{
			v28 = 0;
			do
			{
				v29 = sgLevels[currlevel].object[v28].bCmd;
				if ( v29 >= CMD_OPENDOOR )
				{
					if ( v29 <= CMD_PLROPOBJ )
					{
						SyncOpObject(-1, v29, v28);
					}
					else if ( v29 == CMD_BREAKOBJ )
					{
						SyncBreakObj(-1, v28);
					}
				}
				++v28;
			}
			while ( v28 < 127 );
			for ( j = 0; j < nobjects; ++j )
			{
				v31 = object[objectactive[j]]._otype;
				if ( v31 == OBJ_TRAPL || v31 == OBJ_TRAPR )
					Obj_Trap(objectactive[j]);
			}
		}
		deltaload = 0;
	}
}
// 676190: using guessed type int deltaload;
// 679660: using guessed type char gbMaxPlayers;

void __fastcall NetSendCmd(unsigned char bHiPri, unsigned char bCmd)
{
	TCmd cmd; // [esp+3h] [ebp-1h]

	cmd.bCmd = bCmd;
	if ( bHiPri )
		NetSendHiPri((unsigned char *)&cmd, 1u);
	else
		NetSendLoPri((unsigned char *)&cmd, 1u);
}

void __fastcall NetSendCmdGolem(unsigned char mx, unsigned char my, unsigned char dir, unsigned char menemy, int hp, int cl)
{
	TCmdGolem cmd; // [esp+0h] [ebp-Ch]

	cmd._mx = mx;
	cmd._mdir = dir;
	cmd._menemy = menemy;
	cmd._mhitpoints = hp;
	cmd._my = my;
	cmd.bCmd = CMD_AWAKEGOLEM;
	cmd._currlevel = cl;
	NetSendLoPri((unsigned char *)&cmd, 0xAu);
}

void __fastcall NetSendCmdLoc(unsigned char bHiPri, unsigned char bCmd, unsigned char x, unsigned char y)
{
	TCmdLoc cmd; // [esp+1h] [ebp-3h]

	cmd.bCmd = bCmd;
	cmd.x = x;
	cmd.y = y;
	if ( bHiPri )
		NetSendHiPri((unsigned char *)&cmd, 3u);
	else
		NetSendLoPri((unsigned char *)&cmd, 3u);
}

void __fastcall NetSendCmdLocParam1(unsigned char bHiPri, unsigned char bCmd, unsigned char x, unsigned char y, int wParam1)
{
	TCmdLocParam1 cmd; // [esp+0h] [ebp-8h]

	cmd.bCmd = bCmd;
	cmd.x = x;
	cmd.y = y;
	cmd.wParam1 = wParam1;
	if ( bHiPri )
		NetSendHiPri((unsigned char *)&cmd, 5u);
	else
		NetSendLoPri((unsigned char *)&cmd, 5u);
}

void __fastcall NetSendCmdLocParam2(unsigned char bHiPri, unsigned char bCmd, unsigned char x, unsigned char y, int wParam1, int wParam2)
{
	TCmdLocParam2 cmd; // [esp+0h] [ebp-8h]

	cmd.bCmd = bCmd;
	cmd.x = x;
	cmd.y = y;
	cmd.wParam1 = wParam1;
	cmd.wParam2 = wParam2;
	if ( bHiPri )
		NetSendHiPri((unsigned char *)&cmd, 7u);
	else
		NetSendLoPri((unsigned char *)&cmd, 7u);
}

void __fastcall NetSendCmdLocParam3(unsigned char bHiPri, unsigned char bCmd, unsigned char x, unsigned char y, int wParam1, int wParam2, int wParam3)
{
	TCmdLocParam3 cmd; // [esp+0h] [ebp-Ch]

	cmd.bCmd = bCmd;
	cmd.x = x;
	cmd.y = y;
	cmd.wParam1 = wParam1;
	cmd.wParam2 = wParam2;
	cmd.wParam3 = wParam3;
	if ( bHiPri )
		NetSendHiPri((unsigned char *)&cmd, 9u);
	else
		NetSendLoPri((unsigned char *)&cmd, 9u);
}

void __fastcall NetSendCmdParam1(BOOL bHiPri, unsigned char bCmd, unsigned short wParam1)
{
	TCmdParam1 cmd; // [esp+1h] [ebp-3h]

	cmd.bCmd = bCmd;
	cmd.wParam1 = wParam1;
	if ( bHiPri )
		NetSendHiPri((unsigned char *)&cmd, 3u);
	else
		NetSendLoPri((unsigned char *)&cmd, 3u);
}

void __fastcall NetSendCmdParam2(BOOL bHiPri, unsigned char bCmd, unsigned short wParam1, unsigned short wParam2)
{
	TCmdParam2 cmd; // [esp+0h] [ebp-8h]

	cmd.bCmd = bCmd;
	cmd.wParam1 = wParam1;
	cmd.wParam2 = wParam2;
	if ( bHiPri )
		NetSendHiPri((unsigned char *)&cmd, 5u);
	else
		NetSendLoPri((unsigned char *)&cmd, 5u);
}

void __fastcall NetSendCmdParam3(unsigned char bHiPri, unsigned char bCmd, unsigned short wParam1, unsigned short wParam2, int wParam3)
{
	TCmdParam3 cmd; // [esp+0h] [ebp-8h]

	cmd.bCmd = bCmd;
	cmd.wParam1 = wParam1;
	cmd.wParam2 = wParam2;
	cmd.wParam3 = wParam3;
	if ( bHiPri )
		NetSendHiPri((unsigned char *)&cmd, 7u);
	else
		NetSendLoPri((unsigned char *)&cmd, 7u);
}

void __fastcall NetSendCmdQuest(unsigned char bHiPri, unsigned char q)
{
	int v2; // eax
	char v3; // dl
	TCmdQuest cmd; // [esp+0h] [ebp-8h]

	cmd.q = q;
	cmd.bCmd = CMD_SYNCQUEST;
	v2 = 24 * q;
	cmd.qstate = *(&quests[0]._qactive + v2);
	v3 = *((_BYTE *)&quests[0]._qlog + v2);
	_LOBYTE(v2) = *(&quests[0]._qvar1 + v2);
	cmd.qlog = v3;
	cmd.qvar1 = v2;
	if ( bHiPri )
		NetSendHiPri((unsigned char *)&cmd, 5u);
	else
		NetSendLoPri((unsigned char *)&cmd, 5u);
}

void __fastcall NetSendCmdGItem(unsigned char bHiPri, unsigned char bCmd, unsigned char mast, unsigned char pnum, int ii)
{
	int v5; // eax
	bool v6; // zf
	short v7; // dx
	short v8; // bx
	int v9; // esi
	int v10; // esi
	char v11; // dl
	short v12; // ax
	TCmdGItem cmd; // [esp+4h] [ebp-20h]

	cmd.bCmd = bCmd;
	cmd.bPnum = pnum;
	cmd.bMaster = mast;
	cmd.bLevel = currlevel;
	cmd.bCursitem = ii;
	cmd.dwTime = 0;
	v5 = (unsigned char)ii;
	cmd.x = item[v5]._ix;
	cmd.y = item[v5]._iy;
	v6 = item[v5].IDidx == IDI_EAR;
	cmd.wIndx = item[v5].IDidx;
	if ( v6 )
	{
		_LOBYTE(v7) = 0;
		_HIBYTE(v7) = item[v5]._iName[7];
		_LOBYTE(v8) = 0;
		_HIBYTE(v8) = item[v5]._iName[18];
		v9 = item[v5]._iName[10];
		cmd.wCI = item[v5]._iName[8] | v7;
		cmd.dwSeed = item[v5]._iName[12] | ((item[v5]._iName[11] | ((v9 | (item[v5]._iName[9] << 8)) << 8)) << 8);
		cmd.bId = item[v5]._iName[13];
		cmd.bDur = item[v5]._iName[14];
		cmd.bMDur = item[v5]._iName[15];
		cmd.bCh = item[v5]._iName[16];
		cmd.bMCh = item[v5]._iName[17];
		v10 = item[v5]._iName[20];
		cmd.wValue = _LOWORD(item[v5]._ivalue) | v8 | ((_LOWORD(item[v5]._iCurs) - 19) << 6);
		cmd.dwBuff = item[v5]._iName[22] | ((item[v5]._iName[21] | ((v10 | (item[v5]._iName[19] << 8)) << 8)) << 8);
	}
	else
	{
		cmd.wCI = item[v5]._iCreateInfo;
		cmd.dwSeed = item[v5]._iSeed;
		cmd.bId = item[v5]._iIdentified;
		cmd.bDur = item[v5]._iDurability;
		cmd.bMDur = item[v5]._iMaxDur;
		cmd.bCh = item[v5]._iCharges;
		v11 = item[v5]._iMaxCharges;
		v12 = item[v5]._ivalue;
		cmd.bMCh = v11;
		cmd.wValue = v12;
	}
	if ( bHiPri )
		NetSendHiPri((unsigned char *)&cmd, 0x1Eu);
	else
		NetSendLoPri((unsigned char *)&cmd, 0x1Eu);
}

void __fastcall NetSendCmdGItem2(unsigned char usonly, unsigned char bCmd, unsigned char mast, unsigned char pnum, struct TCmdGItem *p)
{
	unsigned char v5; // bl
	int v6; // esi
	int v7; // eax
	TCmdGItem cmd; // [esp+8h] [ebp-20h]

	v5 = bCmd;
	v6 = usonly;
	memcpy(&cmd, p, 0x1Eu);
	cmd.bPnum = pnum;
	cmd.bCmd = v5;
	cmd.bMaster = mast;
	if ( !v6 )
	{
		cmd.dwTime = 0;
		NetSendHiPri((unsigned char *)&cmd, 0x1Eu);
		return;
	}
	v7 = GetTickCount();
	if ( cmd.dwTime )
	{
		if ( v7 - cmd.dwTime > 5000 )
			return;
	}
	else
	{
		cmd.dwTime = v7;
	}
	multi_msg_add(&cmd.bCmd, 0x1Eu);
}

bool __fastcall NetSendCmdReq2(unsigned char bCmd, unsigned char mast, unsigned char pnum, struct TCmdGItem *p)
{
	unsigned char v4; // bl
	int v5; // eax
	TCmdGItem cmd; // [esp+4h] [ebp-24h]
	unsigned char v8; // [esp+24h] [ebp-4h]

	v4 = mast;
	v8 = bCmd;
	memcpy(&cmd, p, 0x1Eu);
	cmd.bCmd = v8;
	cmd.bPnum = pnum;
	cmd.bMaster = v4;
	v5 = GetTickCount();
	if ( !cmd.dwTime )
	{
		cmd.dwTime = v5;
LABEL_3:
		multi_msg_add(&cmd.bCmd, 0x1Eu);
		return 1;
	}
	if ( v5 - cmd.dwTime <= 5000 )
		goto LABEL_3;
	return 0;
}

void __fastcall NetSendCmdExtra(struct TCmdGItem *p)
{
	TCmdGItem cmd; // [esp+0h] [ebp-20h]

	memcpy(&cmd, p, 0x1Eu);
	cmd.dwTime = 0;
	cmd.bCmd = CMD_ITEMEXTRA;
	NetSendHiPri((unsigned char *)&cmd, 0x1Eu);
}

void __fastcall NetSendCmdPItem(unsigned char bHiPri, unsigned char bCmd, unsigned char x, unsigned char y)
{
	int v4; // eax
	short *v5; // edx
	bool v6; // zf
	short v7; // dx
	short v8; // bx
	int v9; // esi
	int v10; // esi
	char v11; // dl
	short v12; // ax
	TCmdPItem cmd; // [esp+4h] [ebp-18h]

	cmd.bCmd = bCmd;
	cmd.x = x;
	cmd.y = y;
	v4 = myplr;
	v5 = (short *)&plr[myplr].HoldItem.IDidx;
	v6 = *(_DWORD *)v5 == IDI_EAR;
	cmd.wIndx = *v5;
	if ( v6 )
	{
		_LOBYTE(v7) = 0;
		_HIBYTE(v7) = plr[v4].HoldItem._iName[7];
		_LOBYTE(v8) = 0;
		_HIBYTE(v8) = plr[v4].HoldItem._iName[18];
		v9 = plr[v4].HoldItem._iName[10];
		cmd.wCI = plr[v4].HoldItem._iName[8] | v7;
		cmd.dwSeed = plr[v4].HoldItem._iName[12] | ((plr[v4].HoldItem._iName[11] | ((v9 | (plr[v4].HoldItem._iName[9] << 8)) << 8)) << 8);
		cmd.bId = plr[v4].HoldItem._iName[13];
		cmd.bDur = plr[v4].HoldItem._iName[14];
		cmd.bMDur = plr[v4].HoldItem._iName[15];
		cmd.bCh = plr[v4].HoldItem._iName[16];
		cmd.bMCh = plr[v4].HoldItem._iName[17];
		v10 = plr[v4].HoldItem._iName[20];
		cmd.wValue = _LOWORD(plr[v4].HoldItem._ivalue) | v8 | ((_LOWORD(plr[v4].HoldItem._iCurs) - 19) << 6);
		cmd.dwBuff = plr[v4].HoldItem._iName[22] | ((plr[v4].HoldItem._iName[21] | ((v10 | (plr[v4].HoldItem._iName[19] << 8)) << 8)) << 8);
	}
	else
	{
		cmd.wCI = plr[v4].HoldItem._iCreateInfo;
		cmd.dwSeed = plr[v4].HoldItem._iSeed;
		cmd.bId = plr[v4].HoldItem._iIdentified;
		cmd.bDur = plr[v4].HoldItem._iDurability;
		cmd.bMDur = plr[v4].HoldItem._iMaxDur;
		cmd.bCh = plr[v4].HoldItem._iCharges;
		v11 = plr[v4].HoldItem._iMaxCharges;
		v12 = plr[v4].HoldItem._ivalue;
		cmd.bMCh = v11;
		cmd.wValue = v12;
	}
	if ( bHiPri )
		NetSendHiPri((unsigned char *)&cmd, 0x16u);
	else
		NetSendLoPri((unsigned char *)&cmd, 0x16u);
}

void __fastcall NetSendCmdChItem(unsigned char bHiPri, unsigned char bLoc)
{
	short v2; // dx
	char v3; // al
	TCmdChItem cmd; // [esp+0h] [ebp-Ch]

	cmd.bLoc = bLoc;
	v2 = plr[myplr].HoldItem.IDidx;
	cmd.bCmd = CMD_CHANGEPLRITEMS;
	cmd.wIndx = v2;
	cmd.wCI = plr[myplr].HoldItem._iCreateInfo;
	v3 = plr[myplr].HoldItem._iIdentified;
	cmd.dwSeed = plr[myplr].HoldItem._iSeed;
	cmd.bId = v3;
	if ( bHiPri )
		NetSendHiPri((unsigned char *)&cmd, 0xBu);
	else
		NetSendLoPri((unsigned char *)&cmd, 0xBu);
}

void __fastcall NetSendCmdDelItem(BOOL bHiPri, unsigned char bLoc)
{
	TCmdDelItem cmd; // [esp+2h] [ebp-2h]

	cmd.bLoc = bLoc;
	cmd.bCmd = CMD_DELPLRITEMS;
	if ( bHiPri )
		NetSendHiPri((unsigned char *)&cmd, 2u);
	else
		NetSendLoPri((unsigned char *)&cmd, 2u);
}

void __fastcall NetSendCmdDItem(unsigned char bHiPri, int ii)
{
	int v2; // eax
	short *v3; // edx
	bool v4; // zf
	short v5; // dx
	short v6; // bx
	int v7; // esi
	int v8; // esi
	char v9; // dl
	short v10; // ax
	TCmdPItem cmd; // [esp+4h] [ebp-18h]

	v2 = ii;
	cmd.bCmd = CMD_DROPITEM;
	cmd.x = item[ii]._ix;
	cmd.y = item[ii]._iy;
	v3 = (short *)&item[ii].IDidx;
	v4 = *(_DWORD *)v3 == IDI_EAR;
	cmd.wIndx = *v3;
	if ( v4 )
	{
		_LOBYTE(v5) = 0;
		_HIBYTE(v5) = item[v2]._iName[7];
		_LOBYTE(v6) = 0;
		_HIBYTE(v6) = item[v2]._iName[18];
		v7 = item[v2]._iName[10];
		cmd.wCI = item[v2]._iName[8] | v5;
		cmd.dwSeed = item[v2]._iName[12] | ((item[v2]._iName[11] | ((v7 | (item[v2]._iName[9] << 8)) << 8)) << 8);
		cmd.bId = item[v2]._iName[13];
		cmd.bDur = item[v2]._iName[14];
		cmd.bMDur = item[v2]._iName[15];
		cmd.bCh = item[v2]._iName[16];
		cmd.bMCh = item[v2]._iName[17];
		v8 = item[v2]._iName[20];
		cmd.wValue = _LOWORD(item[v2]._ivalue) | v6 | ((_LOWORD(item[v2]._iCurs) - 19) << 6);
		cmd.dwBuff = item[v2]._iName[22] | ((item[v2]._iName[21] | ((v8 | (item[v2]._iName[19] << 8)) << 8)) << 8);
	}
	else
	{
		cmd.wCI = item[v2]._iCreateInfo;
		cmd.dwSeed = item[v2]._iSeed;
		cmd.bId = item[v2]._iIdentified;
		cmd.bDur = item[v2]._iDurability;
		cmd.bMDur = item[v2]._iMaxDur;
		cmd.bCh = item[v2]._iCharges;
		v9 = item[v2]._iMaxCharges;
		v10 = item[v2]._ivalue;
		cmd.bMCh = v9;
		cmd.wValue = v10;
	}
	if ( bHiPri )
		NetSendHiPri((unsigned char *)&cmd, 0x16u);
	else
		NetSendLoPri((unsigned char *)&cmd, 0x16u);
}

void __fastcall NetSendCmdDamage(unsigned char bHiPri, unsigned char bPlr, unsigned int dwDam)
{
	TCmdDamage cmd; // [esp+0h] [ebp-8h]

	cmd.bPlr = bPlr;
	cmd.bCmd = CMD_PLRDAMAGE;
	cmd.dwDam = dwDam;
	if ( bHiPri )
		NetSendHiPri((unsigned char *)&cmd, 6u);
	else
		NetSendLoPri((unsigned char *)&cmd, 6u);
}

void __fastcall NetSendCmdString(int a1, const char *pszStr)
{
	const char *v2; // esi
	int v3; // edi
	char dwStrLen; // bl
	TCmdString cmd; // [esp+Ch] [ebp-54h]

	v2 = pszStr;
	v3 = a1;
	dwStrLen = strlen(pszStr);
	cmd.bCmd = CMD_STRING;
	strcpy(cmd.str, v2);
	multi_send_msg_packet(v3, &cmd.bCmd, dwStrLen + 2);
}

void __fastcall RemovePlrPortal(int pnum)
{
	memset(&sgJunk.portal[pnum], 255, 5u);
	sgbDeltaChanged = 1;
}
// 67618C: using guessed type char sgbDeltaChanged;

int __fastcall ParseCmd(int pnum, TCmd *pCmd)
{
	bool v2; // zf
	TCmd *v3; // eax
	char v4; // dl
	unsigned char v5; // bl
	int result; // eax
	TCmd *v7; // esi

	v2 = sgwPackPlrOffsetTbl[pnum] == 0;
	v3 = pCmd;
	v4 = pCmd->bCmd;
	sbLastCmd = v4;
	if ( !v2 && v4 != CMD_ACK_PLRINFO && v4 != CMD_SEND_PLRINFO )
		return 0;
	v5 = v3->bCmd;
	switch ( v3->bCmd )
	{
		case CMD_WALKXY:
			return On_WALKXY((struct TCmdLoc *)v3, pnum);
		case CMD_ACK_PLRINFO:
			return On_ACK_PLRINFO((struct TCmdPlrInfoHdr *)v3, pnum);
		case CMD_ADDSTR:
			return On_ADDSTR((struct TCmdParam1 *)v3, pnum);
		case CMD_ADDMAG:
			return On_ADDMAG((struct TCmdParam1 *)v3, pnum);
		case CMD_ADDDEX:
			return On_ADDDEX((struct TCmdParam1 *)v3, pnum);
		case CMD_ADDVIT:
			return On_ADDVIT((struct TCmdParam1 *)v3, pnum);
		case CMD_SBSPELL:
			return On_SBSPELL((struct TCmdParam1 *)v3, pnum);
		case CMD_GETITEM:
			return On_GETITEM((struct TCmdGItem *)v3, pnum);
		case CMD_AGETITEM:
			return On_AGETITEM((struct TCmdGItem *)v3, pnum);
		case CMD_PUTITEM:
			return On_PUTITEM((struct TCmdPItem *)v3, pnum);
		case CMD_RESPAWNITEM:
			return On_RESPAWNITEM((struct TCmdPItem *)v3, pnum);
		case CMD_ATTACKXY:
			return On_ATTACKXY((struct TCmdLoc *)v3, pnum);
		case CMD_RATTACKXY:
			return On_RATTACKXY((struct TCmdLoc *)v3, pnum);
		case CMD_SPELLXY:
			return On_SPELLXY((struct TCmdLocParam2 *)v3, pnum);
		case CMD_TSPELLXY:
			return On_TSPELLXY((struct TCmdLocParam2 *)v3, pnum);
		case CMD_OPOBJXY:
			return On_OPOBJXY((struct TCmdLocParam1 *)v3, pnum);
		case CMD_DISARMXY:
			return On_DISARMXY((struct TCmdLocParam1 *)v3, pnum);
		case CMD_ATTACKID:
			return On_ATTACKID((struct TCmdParam1 *)v3, pnum);
		case CMD_ATTACKPID:
			return On_ATTACKPID((struct TCmdParam1 *)v3, pnum);
		case CMD_RATTACKID:
			return On_RATTACKID((struct TCmdParam1 *)v3, pnum);
		case CMD_RATTACKPID:
			return On_RATTACKPID((struct TCmdParam1 *)v3, pnum);
		case CMD_SPELLID:
			return On_SPELLID((struct TCmdLocParam2 *)v3, pnum);
		case CMD_SPELLPID:
			return On_SPELLPID((struct TCmdLocParam2 *)v3, pnum);
		case CMD_TSPELLID:
			return On_TSPELLID((struct TCmdLocParam2 *)v3, pnum);
		case CMD_TSPELLPID:
			return On_TSPELLPID((struct TCmdLocParam2 *)v3, pnum);
		case CMD_RESURRECT:
			return On_RESURRECT((struct TCmdParam1 *)v3, pnum);
		case CMD_OPOBJT:
			return On_OPOBJT((struct TCmdParam1 *)v3, pnum);
		case CMD_KNOCKBACK:
			return On_KNOCKBACK((struct TCmdParam1 *)v3, pnum);
		case CMD_TALKXY:
			return On_TALKXY((struct TCmdLocParam1 *)v3, pnum);
		case CMD_NEWLVL:
			return On_NEWLVL((struct TCmdParam2 *)v3, pnum);
		case CMD_WARP:
			return On_WARP((struct TCmdParam1 *)v3, pnum);
#ifdef _DEBUG
		case CMD_CHEAT_EXPERIENCE:
			return On_CHEAT_EXPERIENCE(v3, pnum);
		case CMD_CHEAT_SPELL_LEVEL:
			return On_CHEAT_SPELL_LEVEL(v3, pnum);
#else
		case CMD_CHEAT_EXPERIENCE:
			return On_DEBUG();
		case CMD_CHEAT_SPELL_LEVEL:
			return On_DEBUG();
#endif
		case CMD_DEBUG:
			return On_DEBUG();
		case CMD_SYNCDATA:
			return On_SYNCDATA(v3, pnum);
		case CMD_MONSTDEATH:
			return On_MONSTDEATH((struct TCmdLocParam1 *)v3, pnum);
		case CMD_MONSTDAMAGE:
			return On_MONSTDAMAGE((struct TCmdLocParam1 *)v3, pnum);
		case CMD_PLRDEAD:
			return On_PLRDEAD((struct TCmdParam1 *)v3, pnum);
		case CMD_REQUESTGITEM:
			return On_REQUESTGITEM((struct TCmdGItem *)v3, pnum);
		case CMD_REQUESTAGITEM:
			return On_REQUESTAGITEM((struct TCmdGItem *)v3, pnum);
		case CMD_GOTOGETITEM:
			return On_GOTOGETITEM((struct TCmdLocParam1 *)v3, pnum);
		case CMD_GOTOAGETITEM:
			return On_GOTOAGETITEM((struct TCmdLocParam1 *)v3, pnum);
		case CMD_OPENDOOR:
			return On_OPENDOOR((struct TCmdParam1 *)v3, pnum);
		case CMD_CLOSEDOOR:
			return On_CLOSEDOOR((struct TCmdParam1 *)v3, pnum);
		case CMD_OPERATEOBJ:
			return On_OPERATEOBJ((struct TCmdParam1 *)v3, pnum);
		case CMD_PLROPOBJ:
			return On_PLROPOBJ((struct TCmdParam2 *)v3, pnum);
		case CMD_BREAKOBJ:
			return On_BREAKOBJ((struct TCmdParam2 *)v3, pnum);
		case CMD_CHANGEPLRITEMS:
			return On_CHANGEPLRITEMS((struct TCmdChItem *)v3, pnum);
		case CMD_DELPLRITEMS:
			return On_DELPLRITEMS((struct TCmdDelItem *)v3, pnum);
		case CMD_PLRDAMAGE:
			return On_PLRDAMAGE((struct TCmdDamage *)v3, pnum);
		case CMD_PLRLEVEL:
			return On_PLRLEVEL((struct TCmdParam1 *)v3, pnum);
		case CMD_DROPITEM:
			return On_DROPITEM((struct TCmdPItem *)v3, pnum);
		case CMD_PLAYER_JOINLEVEL:
			return On_PLAYER_JOINLEVEL((struct TCmdLocParam1 *)v3, pnum);
		case CMD_SEND_PLRINFO:
			return On_SEND_PLRINFO((struct TCmdPlrInfoHdr *)v3, pnum);
		case CMD_SATTACKXY:
			return On_SATTACKXY((struct TCmdLoc *)v3, pnum);
		case CMD_ACTIVATEPORTAL:
			return On_ACTIVATEPORTAL((DJunk *)v3, pnum);
		case CMD_DEACTIVATEPORTAL:
			return On_DEACTIVATEPORTAL(v3, pnum);
		case CMD_HEALOTHER:
			return On_HEALOTHER((struct TCmdParam1 *)v3, pnum);
		case CMD_STRING:
			return On_STRING((struct TCmdString *)v3, pnum);
		case CMD_SETSTR:
			return On_SETSTR((struct TCmdParam1 *)v3, pnum);
		case CMD_SETMAG:
			return On_SETMAG((struct TCmdParam1 *)v3, pnum);
		case CMD_SETDEX:
			return On_SETDEX((struct TCmdParam1 *)v3, pnum);
		case CMD_SETVIT:
			return On_SETVIT((struct TCmdParam1 *)v3, pnum);
		case CMD_RETOWN:
			return On_RETOWN(v3, pnum);
		case CMD_SPELLXYD:
			return On_SPELLXYD((struct TCmdLocParam3 *)v3, pnum);
		case CMD_ITEMEXTRA:
			return On_ITEMEXTRA((struct TCmdGItem *)v3, pnum);
		case CMD_SYNCPUTITEM:
			return On_SYNCPUTITEM((struct TCmdPItem *)v3, pnum);
		case CMD_KILLGOLEM:
			return On_KILLGOLEM((struct TCmdLocParam1 *)v3, pnum);
		case CMD_SYNCQUEST:
			return On_SYNCQUEST((struct TCmdQuest *)v3, pnum);
		case CMD_ENDSHIELD:
			return On_ENDSHIELD((int)v3, pnum);
		case CMD_AWAKEGOLEM:
			return On_AWAKEGOLEM((struct TCmdGolem *)v3, pnum);
		case CMD_NOVA:
			return On_NOVA((struct TCmdLoc *)v3, pnum);
		case CMD_SETSHIELD:
			return On_SETSHIELD((int)v3, pnum);
		case CMD_REMSHIELD:
			return On_REMSHIELD((int)v3, pnum);
		default:
			if ( v5 < CMD_DLEVEL_0 || v5 > CMD_DLEVEL_END )
			{
				SNetDropPlayer(pnum, 0x40000006);
				return 0;
			}
			v7 = v3;
			if ( (unsigned char)gbDeltaSender == pnum )
			{
				if ( sgbRecvCmd != CMD_DLEVEL_END )
				{
					if ( sgbRecvCmd == v3->bCmd )
					{
LABEL_99:
						memcpy(&sgRecvBuf[*(unsigned short *)&v7[1].bCmd], &v7[5], *(unsigned short *)&v7[3].bCmd);
						sgdwRecvOffset += *(unsigned short *)&v7[3].bCmd;
						goto LABEL_100;
					}
					DeltaImportData(sgbRecvCmd, sgdwRecvOffset);
					if ( v7->bCmd == CMD_DLEVEL_END )
					{
						sgbDeltaChunks = 20;
						sgbRecvCmd = CMD_DLEVEL_END;
						goto LABEL_100;
					}
					sgdwRecvOffset = 0;
LABEL_98:
					sgbRecvCmd = v7->bCmd;
					goto LABEL_99;
				}
			}
			else
			{
				if ( v3->bCmd != CMD_DLEVEL_END && (v3->bCmd != CMD_DLEVEL_0 || *(_WORD *)&v3[1].bCmd) )
					goto LABEL_100;
				gbDeltaSender = pnum;
				sgbRecvCmd = CMD_DLEVEL_END;
			}
			if ( v3->bCmd == CMD_DLEVEL_END )
			{
				sgbDeltaChunks = 20;
				goto LABEL_100;
			}
			if ( v3->bCmd == CMD_DLEVEL_0 && !*(_WORD *)&v3[1].bCmd )
			{
				sgdwRecvOffset = 0;
				goto LABEL_98;
			}
LABEL_100:
			result = *(unsigned short *)&v7[3].bCmd + 5;
			break;
	}
	return result;
}
// 66E4A9: using guessed type char sbLastCmd;
// 67618D: using guessed type char sgbDeltaChunks;
// 6796E4: using guessed type char gbDeltaSender;

void __fastcall DeltaImportData(unsigned char cmd, int recv_offset)
{
	unsigned char v2; // bl
	int v3; // esi
	void *v4; // eax
	void *v5; // eax

	v2 = cmd;
	if ( sgRecvBuf[0] )
		encrypt_decompress(&sgRecvBuf[1], recv_offset, 4721);
	if ( v2 == CMD_DLEVEL_JUNK )
	{
		DeltaImportJunk((int)&sgRecvBuf[1]);
	}
	else if ( v2 < CMD_DLEVEL_0 || v2 > CMD_DLEVEL_16 )
	{
		TermMsg("msg:1");
	}
	else
	{
		v3 = (unsigned char)(v2 - CMD_DLEVEL_0);
		v4 = DeltaImportItem(&sgRecvBuf[1], &sgLevels[v3]);
		v5 = DeltaImportObject(v4, sgLevels[v3].object);
		DeltaImportMonster(v5, sgLevels[v3].monster);
	}
	++sgbDeltaChunks;
	sgbDeltaChanged = 1;
}
// 67618C: using guessed type char sgbDeltaChanged;
// 67618D: using guessed type char sgbDeltaChunks;

void *__fastcall DeltaImportItem(void *src, void *dst)
{
	char *v2; // edi
	_BYTE *v3; // esi
	signed int v4; // ebx

	v2 = (char *)dst;
	v3 = (unsigned char *)src;
	v4 = 127;
	do
	{
		if ( *v3 == -1 )
		{
			memset(v2, 255, 0x16u);
			++v3;
		}
		else
		{
			memcpy(v2, v3, 0x16u);
			v3 += 22;
		}
		v2 += 22;
		--v4;
	}
	while ( v4 );
	return v3;
}

void *__fastcall DeltaImportObject(void *src, void *dst)
{
	char *v2; // esi

	v2 = (char *)src;
	memcpy(dst, src, 0x7Fu);
	return v2 + 127;
}

void *__fastcall DeltaImportMonster(void *src, void *dst)
{
	char *v2; // edi
	_BYTE *v3; // esi
	signed int v4; // ebx

	v2 = (char *)dst;
	v3 = (unsigned char *)src;
	v4 = MAXMONSTERS;
	do
	{
		if ( *v3 == -1 )
		{
			memset(v2, 255, 9u);
			++v3;
		}
		else
		{
			memcpy(v2, v3, 9u);
			v3 += 9;
		}
		v2 += 9;
		--v4;
	}
	while ( v4 );
	return v3;
}

char __fastcall DeltaImportJunk(int a1)
{
	_BYTE *v1; // ebx
	int v2; // edi
	DJunk *v3; // esi
	char result; // al
	MultiQuests *v5; // esi
	unsigned char *v6; // edi
	int *v7; // ebp

	v1 = (_BYTE *)a1;
	v2 = 0;
	v3 = &sgJunk;
	do
	{
		if ( *v1 == -1 )
		{
			memset(v3, 255, 5u);
			++v1;
			SetPortalStats(v2, 0, 0, 0, 0, 0);
		}
		else
		{
			memcpy(v3, v1, 5u);
			v1 += 5;
			SetPortalStats(
				v2,
				1,
				(unsigned char)v3->portal[0].x,
				(unsigned char)v3->portal[0].y,
				(unsigned char)v3->portal[0].level,
				(unsigned char)v3->portal[0].ltype);
		}
		v3 = (DJunk *)((char *)v3 + 5);
		++v2;
	}
	while ( (signed int)v3 < (signed int)sgJunk.quests );
	v5 = sgJunk.quests;
	v6 = &quests[0]._qactive;
	v7 = &questlist[0]._qflags;
	do
	{
		if ( *(_BYTE *)v7 & 1 )
		{
			memcpy(v5, v1, 3u);
			*(_DWORD *)(v6 + 18) = (unsigned char)v5->qlog;
			*v6 = v5->qstate;
			result = v5->qvar1;
			v1 += 3;
			v6[13] = result;
			++v5;
		}
		v7 += 5;
		v6 += 24;
	}
	while ( (signed int)v7 < (signed int)&questlist[16]._qflags );
	return result;
}

int __fastcall On_SYNCDATA(void *packet, int pnum)
{
	return SyncData(pnum, (TSyncHeader *)packet);
}

int __fastcall On_WALKXY(struct TCmdLoc *pCmd, int pnum)
{
	int v2; // ebx
	struct TCmdLoc *v3; // edi
	int v4; // esi

	v2 = pnum;
	v3 = pCmd;
	if ( gbBufferMsgs != 1 )
	{
		v4 = pnum;
		if ( currlevel == plr[pnum].plrlevel )
		{
			ClrPlrPath(pnum);
			MakePlrPath(v2, (unsigned char)v3->x, (unsigned char)v3->y, 1u);
			plr[v4].destAction = -1;
		}
	}
	return 3;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_ADDSTR(struct TCmdParam1 *pCmd, int pnum)
{
	unsigned short v2; // cx

	if ( gbBufferMsgs == 1 )
	{
		msg_send_packet(pnum, pCmd, 3);
	}
	else
	{
		v2 = pCmd->wParam1;
		if ( v2 <= 0x100u )
			ModifyPlrStr(pnum, v2);
	}
	return 3;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_ADDMAG(struct TCmdParam1 *pCmd, int pnum)
{
	unsigned short v2; // cx

	if ( gbBufferMsgs == 1 )
	{
		msg_send_packet(pnum, pCmd, 3);
	}
	else
	{
		v2 = pCmd->wParam1;
		if ( v2 <= 0x100u )
			ModifyPlrMag(pnum, v2);
	}
	return 3;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_ADDDEX(struct TCmdParam1 *pCmd, int pnum)
{
	unsigned short v2; // cx

	if ( gbBufferMsgs == 1 )
	{
		msg_send_packet(pnum, pCmd, 3);
	}
	else
	{
		v2 = pCmd->wParam1;
		if ( v2 <= 0x100u )
			ModifyPlrDex(pnum, v2);
	}
	return 3;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_ADDVIT(struct TCmdParam1 *pCmd, int pnum)
{
	unsigned short v2; // cx

	if ( gbBufferMsgs == 1 )
	{
		msg_send_packet(pnum, pCmd, 3);
	}
	else
	{
		v2 = pCmd->wParam1;
		if ( v2 <= 0x100u )
			ModifyPlrVit(pnum, v2);
	}
	return 3;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_SBSPELL(struct TCmdParam1 *pCmd, int pnum)
{
	int v2; // eax

	if ( gbBufferMsgs != 1 )
	{
		if ( currlevel || *(_DWORD *)&spelldata[(unsigned short)pCmd->wParam1].sTownSpell )
		{
			v2 = pnum;
			plr[v2]._pSpell = (unsigned short)pCmd->wParam1;
			plr[v2]._pSplType = plr[v2]._pSBkSplType;
			plr[v2]._pSplFrom = 1;
			plr[v2].destAction = 12;
		}
		else
		{
			msg_errorf("%s has cast an illegal spell.", plr[pnum]._pName);
		}
	}
	return 3;
}
// 676194: using guessed type char gbBufferMsgs;

void msg_errorf(char *pszFmt, ...)
{
	DWORD v1; // eax
	char v2[256]; // [esp+0h] [ebp-100h]
	va_list va; // [esp+10Ch] [ebp+Ch]

	va_start(va, pszFmt);
	v1 = GetTickCount();
	if ( v1 - msg_err_timer >= 5000 )
	{
		msg_err_timer = v1;
		vsprintf(v2, pszFmt, va);
		ErrorPlrMsg(v2);
	}
}
// 67619C: using guessed type int msg_err_timer;

int __fastcall On_GOTOGETITEM(struct TCmdLocParam1 *pCmd, int pnum)
{
	struct TCmdLocParam1 *v2; // edi
	int v3; // esi

	v2 = pCmd;
	if ( gbBufferMsgs != 1 )
	{
		v3 = pnum;
		if ( currlevel == plr[pnum].plrlevel )
		{
			MakePlrPath(pnum, (unsigned char)pCmd->x, (unsigned char)pCmd->y, 0);
			plr[v3].destAction = 15;
			plr[v3].destParam1 = (unsigned short)v2->wParam1;
		}
	}
	return 5;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_REQUESTGITEM(struct TCmdGItem *pCmd, int pnum)
{
	struct TCmdGItem *v2; // esi
	int v4; // edx
	int v5; // edx
	int v7; // edi
	unsigned char v8; // al
	int v9; // edx

	v2 = pCmd;
	if ( gbBufferMsgs != 1 )
	{
		if ( i_own_level(plr[pnum].plrlevel) )
		{
			_LOWORD(v4) = v2->wCI;
			if ( GetItemRecord(v2->dwSeed, v4, (unsigned short)v2->wIndx) )
			{
				_LOWORD(v5) = v2->wCI;
				v7 = FindGetItem((unsigned short)v2->wIndx, v5, v2->dwSeed);
				v8 = v2->bPnum;
				if ( v7 == -1 )
				{
					if ( !NetSendCmdReq2(CMD_REQUESTGITEM, myplr, v8, v2) )
						NetSendCmdExtra(v2);
				}
				else
				{
					NetSendCmdGItem2(0, CMD_GETITEM, myplr, v8, v2);
					if ( (unsigned char)v2->bPnum == myplr )
						InvGetItem(myplr, v7);
					else
						SyncGetItem(
							(unsigned char)v2->x,
							(unsigned char)v2->y,
							(unsigned short)v2->wIndx,
							v2->wCI,
							v2->dwSeed);
					_LOWORD(v9) = v2->wCI;
					SetItemRecord(v2->dwSeed, v9, (unsigned short)v2->wIndx);
				}
			}
		}
	}
	return 30;
}
// 676194: using guessed type char gbBufferMsgs;

bool __fastcall i_own_level(int nReqLevel)
{
	int v1; // edx
	unsigned char *v2; // eax

	v1 = 0;
	v2 = &plr[0]._pLvlChanging;
	do
	{
		if ( *(v2 - 290) && !*v2 && *(_DWORD *)(v2 - 267) == nReqLevel && (v1 != myplr || !gbBufferMsgs) )
			break;
		v2 += 21720;
		++v1;
	}
	while ( (signed int)v2 < (signed int)&plr[4]._pLvlChanging );
	return v1 == myplr;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_GETITEM(struct TCmdGItem *pCmd, int pnum)
{
	struct TCmdGItem *v2; // esi
	int v4; // edi
	char v6; // al
	int v7; // ecx
	int v8; // edx
	int v9; // eax
	int v10; // edx

	v2 = pCmd;
	if ( gbBufferMsgs == 1 )
	{
		msg_send_packet((unsigned short)pnum, pCmd, 30);
	}
	else
	{
		v4 = FindGetItem((unsigned short)pCmd->wIndx, pCmd->wCI, pCmd->dwSeed);
		if ( !delta_get_item(v2, v2->bLevel) )
		{
			NetSendCmdGItem2(1u, CMD_GETITEM, v2->bMaster, v2->bPnum, v2);
			return 30;
		}
		v6 = v2->bLevel;
		v7 = myplr;
		if ( (currlevel == v6 || (unsigned char)v2->bPnum == myplr) && (unsigned char)v2->bMaster != myplr )
		{
			if ( (unsigned char)v2->bPnum != myplr )
			{
				SyncGetItem(
					(unsigned char)v2->x,
					(unsigned char)v2->y,
					(unsigned short)v2->wIndx,
					v2->wCI,
					v2->dwSeed);
				return 30;
			}
			if ( currlevel == v6 )
			{
				v10 = v4;
			}
			else
			{
				v8 = (unsigned char)v2->bId;
				_LOWORD(v8) = v2->wCI;
				v9 = SyncPutItem(
						 myplr,
						 plr[myplr].WorldX,
						 plr[myplr].WorldY,
						 (unsigned short)v2->wIndx,
						 v8,
						 v2->dwSeed,
						 (unsigned char)v2->bId,
						 (unsigned char)v2->bDur,
						 (unsigned char)v2->bMDur,
						 (unsigned char)v2->bCh,
						 (unsigned char)v2->bMCh,
						 (unsigned short)v2->wValue,
						 v2->dwBuff);
				if ( v9 == -1 )
					return 30;
				v7 = myplr;
				v10 = v9;
			}
			InvGetItem(v7, v10);
			return 30;
		}
	}
	return 30;
}
// 676194: using guessed type char gbBufferMsgs;

bool __fastcall delta_get_item(struct TCmdGItem *pI, unsigned char bLevel)
{
	struct TCmdGItem *v2; // esi
	signed int v3; // ecx
	DLevel *v4; // edi
	DLevel *v5; // eax
	char v6; // cl
	DLevel *v8; // eax
	signed int v9; // ecx

	v2 = pI;
	if ( gbMaxPlayers != 1 )
	{
		v3 = 0;
		v4 = &sgLevels[bLevel];
		v5 = &sgLevels[bLevel];
		while ( v5->item[0].bCmd == -1
			 || v5->item[0].wIndx != v2->wIndx
			 || v5->item[0].wCI != v2->wCI
			 || v5->item[0].dwSeed != v2->dwSeed )
		{
			++v3;
			v5 = (DLevel *)((char *)v5 + 22);
			if ( v3 >= 127 )
				goto LABEL_15;
		}
		v6 = v5->item[0].bCmd;
		if ( v5->item[0].bCmd == 1 )
			return 1;
		if ( !v6 )
		{
			sgbDeltaChanged = 1;
			v5->item[0].bCmd = 1;
			return 1;
		}
		if ( v6 == 2 )
		{
			v5->item[0].bCmd = -1;
			sgbDeltaChanged = 1;
			return 1;
		}
		TermMsg("delta:1");
LABEL_15:
		if ( v2->wCI >= 0 )
			return 0;
		v8 = v4;
		v9 = 0;
		while ( v8->item[0].bCmd != -1 )
		{
			++v9;
			v8 = (DLevel *)((char *)v8 + 22);
			if ( v9 >= 127 )
				return 1;
		}
		sgbDeltaChanged = 1;
		v8->item[0].bCmd = 1;
		v8->item[0].x = v2->x;
		v8->item[0].y = v2->y;
		v8->item[0].wIndx = v2->wIndx;
		v8->item[0].wCI = v2->wCI;
		v8->item[0].dwSeed = v2->dwSeed;
		v8->item[0].bId = v2->bId;
		v8->item[0].bDur = v2->bDur;
		v8->item[0].bMDur = v2->bMDur;
		v8->item[0].bCh = v2->bCh;
		v8->item[0].bMCh = v2->bMCh;
		v8->item[0].wValue = v2->wValue;
		v8->item[0].dwBuff = v2->dwBuff;
	}
	return 1;
}
// 67618C: using guessed type char sgbDeltaChanged;
// 679660: using guessed type char gbMaxPlayers;

int __fastcall On_GOTOAGETITEM(struct TCmdLocParam1 *pCmd, int pnum)
{
	struct TCmdLocParam1 *v2; // edi
	int v3; // esi

	v2 = pCmd;
	if ( gbBufferMsgs != 1 )
	{
		v3 = pnum;
		if ( currlevel == plr[pnum].plrlevel )
		{
			MakePlrPath(pnum, (unsigned char)pCmd->x, (unsigned char)pCmd->y, 0);
			plr[v3].destAction = 16;
			plr[v3].destParam1 = (unsigned short)v2->wParam1;
		}
	}
	return 5;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_REQUESTAGITEM(struct TCmdGItem *pCmd, int pnum)
{
	struct TCmdGItem *v2; // esi
	int v4; // edx
	int v5; // edx
	int v7; // zf
	unsigned char v8; // al
	int v9; // edx

	v2 = pCmd;
	if ( gbBufferMsgs != 1 )
	{
		if ( i_own_level(plr[pnum].plrlevel) )
		{
			_LOWORD(v4) = v2->wCI;
			if ( GetItemRecord(v2->dwSeed, v4, (unsigned short)v2->wIndx) )
			{
				_LOWORD(v5) = v2->wCI;
				v7 = FindGetItem((unsigned short)v2->wIndx, v5, v2->dwSeed);
				v8 = v2->bPnum;
				if ( v7 == -1 )
				{
					if ( !NetSendCmdReq2(CMD_REQUESTAGITEM, myplr, v8, v2) )
						NetSendCmdExtra(v2);
				}
				else
				{
					NetSendCmdGItem2(0, CMD_AGETITEM, myplr, v8, v2);
					if ( (unsigned char)v2->bPnum == myplr )
						AutoGetItem(myplr, (unsigned char)v2->bCursitem);
					else
						SyncGetItem(
							(unsigned char)v2->x,
							(unsigned char)v2->y,
							(unsigned short)v2->wIndx,
							v2->wCI,
							v2->dwSeed);
					_LOWORD(v9) = v2->wCI;
					SetItemRecord(v2->dwSeed, v9, (unsigned short)v2->wIndx);
				}
			}
		}
	}
	return 30;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_AGETITEM(struct TCmdGItem *pCmd, int pnum)
{
	struct TCmdGItem *v2; // esi
	char v4; // al
	int v5; // ecx
	int v6; // edx
	int v7; // eax
	int v8; // edx

	v2 = pCmd;
	if ( gbBufferMsgs == 1 )
	{
		msg_send_packet((unsigned short)pnum, pCmd, 30);
	}
	else
	{
		FindGetItem((unsigned short)pCmd->wIndx, pCmd->wCI, pCmd->dwSeed);
		if ( !delta_get_item(v2, v2->bLevel) )
		{
			NetSendCmdGItem2(1u, CMD_AGETITEM, v2->bMaster, v2->bPnum, v2);
			return 30;
		}
		v4 = v2->bLevel;
		v5 = myplr;
		if ( (currlevel == v4 || (unsigned char)v2->bPnum == myplr) && (unsigned char)v2->bMaster != myplr )
		{
			if ( (unsigned char)v2->bPnum != myplr )
			{
				SyncGetItem(
					(unsigned char)v2->x,
					(unsigned char)v2->y,
					(unsigned short)v2->wIndx,
					v2->wCI,
					v2->dwSeed);
				return 30;
			}
			if ( currlevel == v4 )
			{
				v8 = (unsigned char)v2->bCursitem;
			}
			else
			{
				v6 = (unsigned char)v2->bId;
				_LOWORD(v6) = v2->wCI;
				v7 = SyncPutItem(
						 myplr,
						 plr[myplr].WorldX,
						 plr[myplr].WorldY,
						 (unsigned short)v2->wIndx,
						 v6,
						 v2->dwSeed,
						 (unsigned char)v2->bId,
						 (unsigned char)v2->bDur,
						 (unsigned char)v2->bMDur,
						 (unsigned char)v2->bCh,
						 (unsigned char)v2->bMCh,
						 (unsigned short)v2->wValue,
						 v2->dwBuff);
				if ( v7 == -1 )
					return 30;
				v5 = myplr;
				v8 = v7;
			}
			AutoGetItem(v5, v8);
			return 30;
		}
	}
	return 30;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_ITEMEXTRA(struct TCmdGItem *pCmd, int pnum)
{
	int v2; // edi
	struct TCmdGItem *v3; // esi

	v2 = pnum;
	v3 = pCmd;
	if ( gbBufferMsgs == 1 )
	{
		msg_send_packet(pnum, pCmd, 30);
	}
	else
	{
		delta_get_item(pCmd, pCmd->bLevel);
		if ( currlevel == plr[v2].plrlevel )
			SyncGetItem(
				(unsigned char)v3->x,
				(unsigned char)v3->y,
				(unsigned short)v3->wIndx,
				v3->wCI,
				v3->dwSeed);
	}
	return 30;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_PUTITEM(struct TCmdPItem *pCmd, int pnum)
{
	int v2; // edi
	struct TCmdPItem *v3; // esi
	unsigned char *v4; // ebx
	int v5; // edx
	int v6; // eax
	int v7; // edx
	int v8; // ebp

	v2 = pnum;
	v3 = pCmd;
	if ( gbBufferMsgs == 1 )
	{
		msg_send_packet(pnum, pCmd, 22);
		return 22;
	}
	v4 = (unsigned char *)&plr[pnum].plrlevel;
	if ( currlevel != *(_DWORD *)v4 )
	{
		_LOWORD(pnum) = pCmd->wCI;
		PutItemRecord(pCmd->dwSeed, pnum, (unsigned short)pCmd->wIndx);
		delta_put_item(v3, (unsigned char)v3->x, (unsigned char)v3->y, *v4);
		check_update_plr(v2);
		return 22;
	}
	v5 = (unsigned char)pCmd->x;
	if ( v2 == myplr )
		v6 = InvPutItem(v2, v5, (unsigned char)pCmd->y);
	else
		v6 = SyncPutItem(
				 v2,
				 v5,
				 (unsigned char)pCmd->y,
				 (unsigned short)pCmd->wIndx,
				 (unsigned short)pCmd->wCI,
				 pCmd->dwSeed,
				 (unsigned char)pCmd->bId,
				 (unsigned char)pCmd->bDur,
				 (unsigned char)pCmd->bMDur,
				 (unsigned char)pCmd->bCh,
				 (unsigned char)pCmd->bMCh,
				 (unsigned short)pCmd->wValue,
				 pCmd->dwBuff);
	v8 = v6;
	if ( v6 != -1 )
	{
		_LOWORD(v7) = v3->wCI;
		PutItemRecord(v3->dwSeed, v7, (unsigned short)v3->wIndx);
		delta_put_item(v3, item[v8]._ix, item[v8]._iy, *v4);
		check_update_plr(v2);
	}
	return 22;
}
// 676194: using guessed type char gbBufferMsgs;

void __fastcall delta_put_item(struct TCmdPItem *pI, int x, int y, unsigned char bLevel)
{
	struct TCmdPItem *v4; // ebx
	int v5; // eax
	DLevel *v6; // esi
	DLevel *v7; // edi
	char v8; // al
	signed int v9; // eax
	char v10; // [esp+Ch] [ebp-4h]
	signed int bLevela; // [esp+1Ch] [ebp+Ch]

	v10 = x;
	v4 = pI;
	if ( gbMaxPlayers != 1 )
	{
		v5 = bLevel;
		bLevela = 0;
		v6 = &sgLevels[v5];
		v7 = &sgLevels[v5];
		do
		{
			v8 = v7->item[0].bCmd;
			if ( v7->item[0].bCmd != 1
			  && v8 != -1
			  && v7->item[0].wIndx == v4->wIndx
			  && v7->item[0].wCI == v4->wCI
			  && v7->item[0].dwSeed == v4->dwSeed )
			{
				if ( v8 == 2 )
					return;
				TermMsg("Trying to drop a floor item?");
			}
			++bLevela;
			v7 = (DLevel *)((char *)v7 + 22);
		}
		while ( bLevela < 127 );
		v9 = 0;
		while ( v6->item[0].bCmd != -1 )
		{
			++v9;
			v6 = (DLevel *)((char *)v6 + 22);
			if ( v9 >= 127 )
				return;
		}
		sgbDeltaChanged = 1;
		memcpy(v6, v4, 0x16u);
		v6->item[0].x = v10;
		v6->item[0].bCmd = 2;
		v6->item[0].y = y;
	}
}
// 67618C: using guessed type char sgbDeltaChanged;
// 679660: using guessed type char gbMaxPlayers;

void __fastcall check_update_plr(int pnum)
{
	if ( gbMaxPlayers != 1 && pnum == myplr )
		pfile_update(1);
}
// 679660: using guessed type char gbMaxPlayers;

int __fastcall On_SYNCPUTITEM(struct TCmdPItem *pCmd, int pnum)
{
	int v2; // ebx
	struct TCmdPItem *v3; // esi
	unsigned char *v4; // edi
	int v5; // edx
	int v6; // ebp

	v2 = pnum;
	v3 = pCmd;
	if ( gbBufferMsgs == 1 )
	{
		msg_send_packet(pnum, pCmd, 22);
		return 22;
	}
	v4 = (unsigned char *)&plr[pnum].plrlevel;
	if ( currlevel != *(_DWORD *)v4 )
	{
		_LOWORD(pnum) = pCmd->wCI;
		PutItemRecord(pCmd->dwSeed, pnum, (unsigned short)pCmd->wIndx);
		delta_put_item(v3, (unsigned char)v3->x, (unsigned char)v3->y, *v4);
		check_update_plr(v2);
		return 22;
	}
	v6 = SyncPutItem(
			 pnum,
			 (unsigned char)pCmd->x,
			 (unsigned char)pCmd->y,
			 (unsigned short)pCmd->wIndx,
			 (unsigned short)pCmd->wCI,
			 pCmd->dwSeed,
			 (unsigned char)pCmd->bId,
			 (unsigned char)pCmd->bDur,
			 (unsigned char)pCmd->bMDur,
			 (unsigned char)pCmd->bCh,
			 (unsigned char)pCmd->bMCh,
			 (unsigned short)pCmd->wValue,
			 pCmd->dwBuff);
	if ( v6 != -1 )
	{
		_LOWORD(v5) = v3->wCI;
		PutItemRecord(v3->dwSeed, v5, (unsigned short)v3->wIndx);
		delta_put_item(v3, item[v6]._ix, item[v6]._iy, *v4);
		check_update_plr(v2);
	}
	return 22;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_RESPAWNITEM(struct TCmdPItem *pCmd, int pnum)
{
	struct TCmdPItem *v2; // esi
	unsigned char *v3; // edi

	v2 = pCmd;
	if ( gbBufferMsgs == 1 )
	{
		msg_send_packet(pnum, pCmd, 22);
	}
	else
	{
		v3 = (unsigned char *)&plr[pnum].plrlevel;
		if ( currlevel == *(_DWORD *)v3 && pnum != myplr )
			SyncPutItem(
				pnum,
				(unsigned char)pCmd->x,
				(unsigned char)pCmd->y,
				(unsigned short)pCmd->wIndx,
				(unsigned short)pCmd->wCI,
				pCmd->dwSeed,
				(unsigned char)pCmd->bId,
				(unsigned char)pCmd->bDur,
				(unsigned char)pCmd->bMDur,
				(unsigned char)pCmd->bCh,
				(unsigned char)pCmd->bMCh,
				(unsigned short)pCmd->wValue,
				pCmd->dwBuff);
		_LOWORD(pnum) = v2->wCI;
		PutItemRecord(v2->dwSeed, pnum, (unsigned short)v2->wIndx);
		delta_put_item(v2, (unsigned char)v2->x, (unsigned char)v2->y, *v3);
	}
	return 22;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_ATTACKXY(struct TCmdLoc *pCmd, int pnum)
{
	struct TCmdLoc *v2; // edi
	int v3; // esi

	v2 = pCmd;
	if ( gbBufferMsgs != 1 )
	{
		v3 = pnum;
		if ( currlevel == plr[pnum].plrlevel )
		{
			MakePlrPath(pnum, (unsigned char)pCmd->x, (unsigned char)pCmd->y, 0);
			plr[v3].destAction = 9;
			plr[v3].destParam1 = (unsigned char)v2->x;
			plr[v3].destParam2 = (unsigned char)v2->y;
		}
	}
	return 3;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_SATTACKXY(struct TCmdLoc *pCmd, int pnum)
{
	struct TCmdLoc *v2; // edi
	int v3; // esi

	v2 = pCmd;
	if ( gbBufferMsgs != 1 )
	{
		v3 = pnum;
		if ( currlevel == plr[pnum].plrlevel )
		{
			ClrPlrPath(pnum);
			plr[v3].destAction = 9;
			plr[v3].destParam1 = (unsigned char)v2->x;
			plr[v3].destParam2 = (unsigned char)v2->y;
		}
	}
	return 3;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_RATTACKXY(struct TCmdLoc *pCmd, int pnum)
{
	struct TCmdLoc *v2; // edi
	int v3; // esi

	v2 = pCmd;
	if ( gbBufferMsgs != 1 )
	{
		v3 = pnum;
		if ( currlevel == plr[pnum].plrlevel )
		{
			ClrPlrPath(pnum);
			plr[v3].destAction = 10;
			plr[v3].destParam1 = (unsigned char)v2->x;
			plr[v3].destParam2 = (unsigned char)v2->y;
		}
	}
	return 3;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_SPELLXYD(struct TCmdLocParam3 *pCmd, int pnum)
{
	struct TCmdLocParam3 *v2; // edi
	int v3; // esi
	int v4; // eax

	v2 = pCmd;
	if ( gbBufferMsgs != 1 )
	{
		v3 = pnum;
		if ( currlevel == plr[pnum].plrlevel )
		{
			if ( currlevel || *(_DWORD *)&spelldata[(unsigned short)pCmd->wParam1].sTownSpell )
			{
				ClrPlrPath(pnum);
				plr[v3].destAction = 26;
				plr[v3].destParam1 = (unsigned char)v2->x;
				plr[v3].destParam2 = (unsigned char)v2->y;
				plr[v3].destParam3 = (unsigned short)v2->wParam2;
				plr[v3].destParam4 = (unsigned short)v2->wParam3;
				v4 = (unsigned short)v2->wParam1;
				plr[v3]._pSplFrom = 0;
				plr[v3]._pSpell = v4;
				plr[v3]._pSplType = plr[v3]._pRSplType;
			}
			else
			{
				msg_errorf("%s has cast an illegal spell.", plr[v3]._pName);
			}
		}
	}
	return 9;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_SPELLXY(struct TCmdLocParam2 *pCmd, int pnum)
{
	struct TCmdLocParam2 *v2; // edi
	int v3; // esi
	int v4; // eax

	v2 = pCmd;
	if ( gbBufferMsgs != 1 )
	{
		v3 = pnum;
		if ( currlevel == plr[pnum].plrlevel )
		{
			if ( currlevel || *(_DWORD *)&spelldata[(unsigned short)pCmd->wParam1].sTownSpell )
			{
				ClrPlrPath(pnum);
				plr[v3].destAction = 12;
				plr[v3].destParam1 = (unsigned char)v2->x;
				plr[v3].destParam2 = (unsigned char)v2->y;
				plr[v3].destParam3 = (unsigned short)v2->wParam2;
				v4 = (unsigned short)v2->wParam1;
				plr[v3]._pSplFrom = 0;
				plr[v3]._pSpell = v4;
				plr[v3]._pSplType = plr[v3]._pRSplType;
			}
			else
			{
				msg_errorf("%s has cast an illegal spell.", plr[v3]._pName);
			}
		}
	}
	return 7;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_TSPELLXY(struct TCmdLocParam2 *pCmd, int pnum)
{
	struct TCmdLocParam2 *v2; // edi
	int v3; // esi

	v2 = pCmd;
	if ( gbBufferMsgs != 1 )
	{
		v3 = pnum;
		if ( currlevel == plr[pnum].plrlevel )
		{
			if ( currlevel || *(_DWORD *)&spelldata[(unsigned short)pCmd->wParam1].sTownSpell )
			{
				ClrPlrPath(pnum);
				plr[v3].destAction = 12;
				plr[v3].destParam1 = (unsigned char)v2->x;
				plr[v3].destParam2 = (unsigned char)v2->y;
				plr[v3].destParam3 = (unsigned short)v2->wParam2;
				plr[v3]._pSpell = (unsigned short)v2->wParam1;
				plr[v3]._pSplType = plr[v3]._pTSplType;
				plr[v3]._pSplFrom = 2;
			}
			else
			{
				msg_errorf("%s has cast an illegal spell.", plr[v3]._pName);
			}
		}
	}
	return 7;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_OPOBJXY(struct TCmdLocParam1 *pCmd, int pnum)
{
	struct TCmdLocParam1 *v2; // esi
	int v3; // edi
	int v4; // eax

	v2 = pCmd;
	if ( gbBufferMsgs != 1 )
	{
		v3 = pnum;
		if ( currlevel == plr[pnum].plrlevel )
		{
			v4 = (unsigned short)pCmd->wParam1;
			if ( object[v4]._oSolidFlag || object[v4]._oDoorFlag )
				MakePlrPath(pnum, (unsigned char)pCmd->x, (unsigned char)pCmd->y, 0);
			else
				MakePlrPath(pnum, (unsigned char)pCmd->x, (unsigned char)pCmd->y, 1u);
			plr[v3].destAction = 13;
			plr[v3].destParam1 = (unsigned short)v2->wParam1;
		}
	}
	return 5;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_DISARMXY(struct TCmdLocParam1 *pCmd, int pnum)
{
	struct TCmdLocParam1 *v2; // esi
	int v3; // edi
	int v4; // eax

	v2 = pCmd;
	if ( gbBufferMsgs != 1 )
	{
		v3 = pnum;
		if ( currlevel == plr[pnum].plrlevel )
		{
			v4 = (unsigned short)pCmd->wParam1;
			if ( object[v4]._oSolidFlag || object[v4]._oDoorFlag )
				MakePlrPath(pnum, (unsigned char)pCmd->x, (unsigned char)pCmd->y, 0);
			else
				MakePlrPath(pnum, (unsigned char)pCmd->x, (unsigned char)pCmd->y, 1u);
			plr[v3].destAction = 14;
			plr[v3].destParam1 = (unsigned short)v2->wParam1;
		}
	}
	return 5;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_OPOBJT(struct TCmdParam1 *pCmd, int pnum)
{
	int v2; // eax

	if ( gbBufferMsgs != 1 )
	{
		v2 = pnum;
		if ( currlevel == plr[pnum].plrlevel )
		{
			plr[v2].destAction = 18;
			plr[v2].destParam1 = (unsigned short)pCmd->wParam1;
		}
	}
	return 3;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_ATTACKID(struct TCmdParam1 *pCmd, int pnum)
{
	int v2; // ebp
	struct TCmdParam1 *v3; // edi
	int v4; // esi
	int v5; // ebx
	int v6; // eax

	v2 = pnum;
	v3 = pCmd;
	if ( gbBufferMsgs != 1 )
	{
		v4 = pnum;
		if ( currlevel == plr[pnum].plrlevel )
		{
			v5 = abs(plr[v4].WorldX - monster[(unsigned short)pCmd->wParam1]._mfutx);
			v6 = abs(plr[v4].WorldY - monster[(unsigned short)v3->wParam1]._mfuty);
			if ( v5 > 1 || v6 > 1 )
				MakePlrPath(
					v2,
					monster[(unsigned short)v3->wParam1]._mfutx,
					monster[(unsigned short)v3->wParam1]._mfuty,
					0);
			plr[v4].destAction = 20;
			plr[v4].destParam1 = (unsigned short)v3->wParam1;
		}
	}
	return 3;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_ATTACKPID(struct TCmdParam1 *pCmd, int pnum)
{
	struct TCmdParam1 *v2; // edi
	int v3; // esi

	v2 = pCmd;
	if ( gbBufferMsgs != 1 )
	{
		v3 = pnum;
		if ( currlevel == plr[pnum].plrlevel )
		{
			MakePlrPath(pnum, plr[(unsigned short)pCmd->wParam1]._px, plr[(unsigned short)pCmd->wParam1]._py, 0);
			plr[v3].destAction = 21;
			plr[v3].destParam1 = (unsigned short)v2->wParam1;
		}
	}
	return 3;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_RATTACKID(struct TCmdParam1 *pCmd, int pnum)
{
	struct TCmdParam1 *v2; // edi
	int v3; // esi

	v2 = pCmd;
	if ( gbBufferMsgs != 1 )
	{
		v3 = pnum;
		if ( currlevel == plr[pnum].plrlevel )
		{
			ClrPlrPath(pnum);
			plr[v3].destAction = 22;
			plr[v3].destParam1 = (unsigned short)v2->wParam1;
		}
	}
	return 3;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_RATTACKPID(struct TCmdParam1 *pCmd, int pnum)
{
	struct TCmdParam1 *v2; // edi
	int v3; // esi

	v2 = pCmd;
	if ( gbBufferMsgs != 1 )
	{
		v3 = pnum;
		if ( currlevel == plr[pnum].plrlevel )
		{
			ClrPlrPath(pnum);
			plr[v3].destAction = 23;
			plr[v3].destParam1 = (unsigned short)v2->wParam1;
		}
	}
	return 3;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_SPELLID(struct TCmdLocParam2 *pCmd, int pnum)
{
	struct TCmdLocParam2 *v2; // edi
	int v3; // esi
	int v4; // eax

	v2 = pCmd;
	if ( gbBufferMsgs != 1 )
	{
		v3 = pnum;
		if ( currlevel == plr[pnum].plrlevel )
		{
			if ( currlevel || *(_DWORD *)&spelldata[(unsigned short)pCmd->wParam1].sTownSpell )
			{
				ClrPlrPath(pnum);
				plr[v3].destAction = 24;
				plr[v3].destParam1 = *(unsigned short *)&v2->x;
				plr[v3].destParam2 = (unsigned short)v2->wParam2;
				v4 = (unsigned short)v2->wParam1;
				plr[v3]._pSplFrom = 0;
				plr[v3]._pSpell = v4;
				plr[v3]._pSplType = plr[v3]._pRSplType;
			}
			else
			{
				msg_errorf("%s has cast an illegal spell.", plr[v3]._pName);
			}
		}
	}
	return 7;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_SPELLPID(struct TCmdLocParam2 *pCmd, int pnum)
{
	struct TCmdLocParam2 *v2; // edi
	int v3; // esi
	int v4; // eax

	v2 = pCmd;
	if ( gbBufferMsgs != 1 )
	{
		v3 = pnum;
		if ( currlevel == plr[pnum].plrlevel )
		{
			if ( currlevel || *(_DWORD *)&spelldata[(unsigned short)pCmd->wParam1].sTownSpell )
			{
				ClrPlrPath(pnum);
				plr[v3].destAction = 25;
				plr[v3].destParam1 = *(unsigned short *)&v2->x;
				plr[v3].destParam2 = (unsigned short)v2->wParam2;
				v4 = (unsigned short)v2->wParam1;
				plr[v3]._pSplFrom = 0;
				plr[v3]._pSpell = v4;
				plr[v3]._pSplType = plr[v3]._pRSplType;
			}
			else
			{
				msg_errorf("%s has cast an illegal spell.", plr[v3]._pName);
			}
		}
	}
	return 7;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_TSPELLID(struct TCmdLocParam2 *pCmd, int pnum)
{
	struct TCmdLocParam2 *v2; // edi
	int v3; // esi

	v2 = pCmd;
	if ( gbBufferMsgs != 1 )
	{
		v3 = pnum;
		if ( currlevel == plr[pnum].plrlevel )
		{
			if ( currlevel || *(_DWORD *)&spelldata[(unsigned short)pCmd->wParam1].sTownSpell )
			{
				ClrPlrPath(pnum);
				plr[v3].destAction = 24;
				plr[v3].destParam1 = *(unsigned short *)&v2->x;
				plr[v3].destParam2 = (unsigned short)v2->wParam2;
				plr[v3]._pSpell = (unsigned short)v2->wParam1;
				plr[v3]._pSplType = plr[v3]._pTSplType;
				plr[v3]._pSplFrom = 2;
			}
			else
			{
				msg_errorf("%s has cast an illegal spell.", plr[v3]._pName);
			}
		}
	}
	return 7;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_TSPELLPID(struct TCmdLocParam2 *pCmd, int pnum)
{
	struct TCmdLocParam2 *v2; // edi
	int v3; // esi

	v2 = pCmd;
	if ( gbBufferMsgs != 1 )
	{
		v3 = pnum;
		if ( currlevel == plr[pnum].plrlevel )
		{
			if ( currlevel || *(_DWORD *)&spelldata[(unsigned short)pCmd->wParam1].sTownSpell )
			{
				ClrPlrPath(pnum);
				plr[v3].destAction = 25;
				plr[v3].destParam1 = *(unsigned short *)&v2->x;
				plr[v3].destParam2 = (unsigned short)v2->wParam2;
				plr[v3]._pSpell = (unsigned short)v2->wParam1;
				plr[v3]._pSplType = plr[v3]._pTSplType;
				plr[v3]._pSplFrom = 2;
			}
			else
			{
				msg_errorf("%s has cast an illegal spell.", plr[v3]._pName);
			}
		}
	}
	return 7;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_KNOCKBACK(struct TCmdParam1 *pCmd, int pnum)
{
	int v2; // edi
	struct TCmdParam1 *v3; // esi

	v2 = pnum;
	v3 = pCmd;
	if ( gbBufferMsgs != 1 && currlevel == plr[pnum].plrlevel )
	{
		M_GetKnockback((unsigned short)pCmd->wParam1);
		M_StartHit((unsigned short)v3->wParam1, v2, 0);
	}
	return 3;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_RESURRECT(struct TCmdParam1 *pCmd, int pnum)
{
	int v2; // esi

	v2 = pnum;
	if ( gbBufferMsgs == 1 )
	{
		msg_send_packet(pnum, pCmd, 3);
	}
	else
	{
		DoResurrect(pnum, (unsigned short)pCmd->wParam1);
		check_update_plr(v2);
	}
	return 3;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_HEALOTHER(struct TCmdParam1 *pCmd, int pnum)
{
	if ( gbBufferMsgs != 1 && currlevel == plr[pnum].plrlevel )
		DoHealOther(pnum, (unsigned short)pCmd->wParam1);
	return 3;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_TALKXY(struct TCmdLocParam1 *pCmd, int pnum)
{
	struct TCmdLocParam1 *v2; // edi
	int v3; // esi

	v2 = pCmd;
	if ( gbBufferMsgs != 1 )
	{
		v3 = pnum;
		if ( currlevel == plr[pnum].plrlevel )
		{
			MakePlrPath(pnum, (unsigned char)pCmd->x, (unsigned char)pCmd->y, 0);
			plr[v3].destAction = 17;
			plr[v3].destParam1 = (unsigned short)v2->wParam1;
		}
	}
	return 5;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_NEWLVL(struct TCmdParam2 *pCmd, int pnum)
{
	if ( gbBufferMsgs == 1 )
	{
		msg_send_packet(pnum, pCmd, 5);
	}
	else if ( pnum != myplr )
	{
		StartNewLvl(pnum, (unsigned short)pCmd->wParam1, (unsigned short)pCmd->wParam2);
	}
	return 5;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_WARP(struct TCmdParam1 *pCmd, int pnum)
{
	int v2; // esi

	v2 = pnum;
	if ( gbBufferMsgs == 1 )
	{
		msg_send_packet(pnum, pCmd, 3);
	}
	else
	{
		StartWarpLvl(pnum, (unsigned short)pCmd->wParam1);
		if ( v2 == myplr && pcurs >= CURSOR_FIRSTITEM )
		{
			qmemcpy(&item[127], &plr[myplr].HoldItem, sizeof(ItemStruct));
			AutoGetItem(myplr, 127);
		}
	}
	return 3;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_MONSTDEATH(struct TCmdLocParam1 *pCmd, int pnum)
{
	struct TCmdLocParam1 *v2; // esi
	unsigned char *v3; // edi

	v2 = pCmd;
	if ( gbBufferMsgs == 1 )
	{
		msg_send_packet(pnum, pCmd, 5);
	}
	else if ( pnum != myplr )
	{
		v3 = (unsigned char *)&plr[pnum].plrlevel;
		if ( currlevel == *(_DWORD *)v3 )
			M_SyncStartKill((unsigned short)pCmd->wParam1, (unsigned char)pCmd->x, (unsigned char)pCmd->y, pnum);
		delta_kill_monster((unsigned short)v2->wParam1, v2->x, v2->y, *v3);
	}
	return 5;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_KILLGOLEM(struct TCmdLocParam1 *pCmd, int pnum)
{
	int v2; // edi
	struct TCmdLocParam1 *v3; // esi

	v2 = pnum;
	v3 = pCmd;
	if ( gbBufferMsgs == 1 )
	{
		msg_send_packet(pnum, pCmd, 5);
	}
	else if ( pnum != myplr )
	{
		if ( currlevel == pCmd->wParam1 )
			M_SyncStartKill(pnum, (unsigned char)pCmd->x, (unsigned char)pCmd->y, pnum);
		delta_kill_monster(v2, v3->x, v3->y, plr[v2].plrlevel);
	}
	return 5;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_AWAKEGOLEM(struct TCmdGolem *pCmd, int pnum)
{
	int v2; // esi
	int v3; // eax
	signed int v4; // ebp
	int v5; // edi
	int v6; // edx

	v2 = pnum;
	if ( gbBufferMsgs == 1 )
	{
		msg_send_packet(pnum, pCmd, 10);
	}
	else
	{
		v3 = 21720 * pnum;
		if ( currlevel == plr[pnum].plrlevel )
		{
			if ( pnum != myplr )
			{
				v4 = 1;
				v5 = 0;
				if ( nummissiles <= 0 )
					goto LABEL_16;
				do
				{
					v6 = missileactive[v5];
					if ( missile[v6]._mitype == MIS_GOLEM && missile[v6]._misource == v2 )
						v4 = 0;
					++v5;
				}
				while ( v5 < nummissiles );
				if ( v4 )
LABEL_16:
					AddMissile(
						*(int *)((char *)&plr[0].WorldX + v3),
						*(int *)((char *)&plr[0].WorldY + v3),
						(unsigned char)pCmd->_mx,
						(unsigned char)pCmd->_my,
						(unsigned char)pCmd->_mdir,
						MIS_GOLEM,
						0,
						v2,
						0,
						1);
			}
		}
		else
		{
			_LOBYTE(v3) = pCmd->_currlevel;
			delta_sync_golem(pCmd, pnum, v3);
		}
	}
	return 10;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_MONSTDAMAGE(struct TCmdLocParam1 *pCmd, int pnum)
{
	int v2; // edi
	struct TCmdLocParam1 *v3; // edx
	unsigned char *v4; // ebx
	char *v5; // esi
	int *v6; // ecx
	int *v7; // eax

	v2 = pnum;
	v3 = pCmd;
	if ( gbBufferMsgs == 1 )
	{
		msg_send_packet(v2, pCmd, 5);
	}
	else if ( v2 != myplr )
	{
		v4 = (unsigned char *)&plr[v2].plrlevel;
		if ( currlevel == *(_DWORD *)v4 )
		{
			v5 = &monster[*(unsigned short *)&pCmd->x].mWhoHit;
			*v5 |= 1 << v2;
			v6 = &monster[*(unsigned short *)&pCmd->x]._mhitpoints;
			if ( *v6 )
			{
				*v6 -= (unsigned short)v3->wParam1;
				v7 = &monster[*(unsigned short *)&v3->x]._mhitpoints;
				if ( (signed int)(*v7 & 0xFFFFFFC0) < 64 )
					*v7 = 64;
				delta_monster_hp(*(unsigned short *)&v3->x, monster[*(unsigned short *)&v3->x]._mhitpoints, *v4);
			}
		}
	}
	return 5;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_PLRDEAD(struct TCmdParam1 *pCmd, int pnum)
{
	if ( gbBufferMsgs == 1 )
	{
		msg_send_packet(pnum, pCmd, 3);
	}
	else if ( pnum == myplr )
	{
		check_update_plr(pnum);
	}
	else
	{
		StartPlayerKill(pnum, (unsigned short)pCmd->wParam1);
	}
	return 3;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_PLRDAMAGE(struct TCmdDamage *pCmd, int pnum)
{
	int v2; // edi
	int v3; // eax
	int v4; // esi
	int *v5; // esi
	int v6; // ecx

	v2 = myplr;
	if ( (unsigned char)pCmd->bPlr == myplr )
	{
		if ( currlevel )
		{
			if ( gbBufferMsgs != 1 && currlevel == plr[pnum].plrlevel && pCmd->dwDam <= 0x2EE00u )
			{
				v3 = myplr;
				v4 = plr[myplr]._pHitPoints;
				if ( (signed int)(v4 & 0xFFFFFFC0) > 0 )
				{
					drawhpflag = 1;
					plr[v3]._pHitPoints = v4 - pCmd->dwDam;
					v5 = &plr[v3]._pHPBase;
					*v5 -= pCmd->dwDam;
					v6 = plr[v3]._pMaxHP;
					if ( plr[v3]._pHitPoints > v6 )
					{
						plr[v3]._pHitPoints = v6;
						*v5 = plr[v3]._pMaxHPBase;
					}
					if ( (signed int)(plr[v3]._pHitPoints & 0xFFFFFFC0) <= 0 )
						SyncPlrKill(v2, 1);
				}
			}
		}
	}
	return 6;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_OPENDOOR(struct TCmdParam1 *pCmd, int pnum)
{
	struct TCmdParam1 *v2; // edi
	unsigned char *v3; // esi

	v2 = pCmd;
	if ( gbBufferMsgs == 1 )
	{
		msg_send_packet(pnum, pCmd, 3);
	}
	else
	{
		v3 = (unsigned char *)&plr[pnum].plrlevel;
		if ( currlevel == *(_DWORD *)v3 )
			SyncOpObject(pnum, CMD_OPENDOOR, (unsigned short)pCmd->wParam1);
		delta_sync_object((unsigned short)v2->wParam1, 0x2Bu, *v3);
	}
	return 3;
}
// 676194: using guessed type char gbBufferMsgs;

void __fastcall delta_sync_object(int oi, unsigned char bCmd, unsigned char bLevel)
{
	if ( gbMaxPlayers != 1 )
	{
		sgbDeltaChanged = 1;
		sgLevels[bLevel].object[oi].bCmd = bCmd;
	}
}
// 67618C: using guessed type char sgbDeltaChanged;
// 679660: using guessed type char gbMaxPlayers;

int __fastcall On_CLOSEDOOR(struct TCmdParam1 *pCmd, int pnum)
{
	struct TCmdParam1 *v2; // edi
	unsigned char *v3; // esi

	v2 = pCmd;
	if ( gbBufferMsgs == 1 )
	{
		msg_send_packet(pnum, pCmd, 3);
	}
	else
	{
		v3 = (unsigned char *)&plr[pnum].plrlevel;
		if ( currlevel == *(_DWORD *)v3 )
			SyncOpObject(pnum, CMD_CLOSEDOOR, (unsigned short)pCmd->wParam1);
		delta_sync_object((unsigned short)v2->wParam1, 0x2Cu, *v3);
	}
	return 3;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_OPERATEOBJ(struct TCmdParam1 *pCmd, int pnum)
{
	struct TCmdParam1 *v2; // edi
	unsigned char *v3; // esi

	v2 = pCmd;
	if ( gbBufferMsgs == 1 )
	{
		msg_send_packet(pnum, pCmd, 3);
	}
	else
	{
		v3 = (unsigned char *)&plr[pnum].plrlevel;
		if ( currlevel == *(_DWORD *)v3 )
			SyncOpObject(pnum, CMD_OPERATEOBJ, (unsigned short)pCmd->wParam1);
		delta_sync_object((unsigned short)v2->wParam1, 0x2Du, *v3);
	}
	return 3;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_PLROPOBJ(struct TCmdParam2 *pCmd, int pnum)
{
	struct TCmdParam2 *v2; // esi
	unsigned char *v3; // edi

	v2 = pCmd;
	if ( gbBufferMsgs == 1 )
	{
		msg_send_packet(pnum, pCmd, 5);
	}
	else
	{
		v3 = (unsigned char *)&plr[pnum].plrlevel;
		if ( currlevel == *(_DWORD *)v3 )
			SyncOpObject((unsigned short)pCmd->wParam1, CMD_PLROPOBJ, (unsigned short)pCmd->wParam2);
		delta_sync_object((unsigned short)v2->wParam2, 0x2Eu, *v3);
	}
	return 5;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_BREAKOBJ(struct TCmdParam2 *pCmd, int pnum)
{
	struct TCmdParam2 *v2; // esi
	unsigned char *v3; // edi

	v2 = pCmd;
	if ( gbBufferMsgs == 1 )
	{
		msg_send_packet(pnum, pCmd, 5);
	}
	else
	{
		v3 = (unsigned char *)&plr[pnum].plrlevel;
		if ( currlevel == *(_DWORD *)v3 )
			SyncBreakObj((unsigned short)pCmd->wParam1, (unsigned short)pCmd->wParam2);
		delta_sync_object((unsigned short)v2->wParam2, 0x2Fu, *v3);
	}
	return 5;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_CHANGEPLRITEMS(struct TCmdChItem *pCmd, int pnum)
{
	int v2; // eax
	int v3; // edx
	int v4; // ST04_4
	int v5; // edx

	v2 = pnum;
	if ( gbBufferMsgs == 1 )
	{
		msg_send_packet(pnum, pCmd, 11);
	}
	else if ( pnum != myplr )
	{
		v3 = (unsigned char)pCmd->bId;
		_LOWORD(v3) = pCmd->wCI;
		v4 = v3;
		v5 = (unsigned short)pCmd->wIndx;
		_LOBYTE(v5) = pCmd->bLoc;
		CheckInvSwap(v2, v5, (unsigned short)pCmd->wIndx, v4, pCmd->dwSeed, (unsigned char)pCmd->bId);
	}
	return 11;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_DELPLRITEMS(struct TCmdDelItem *pCmd, int pnum)
{
	int v2; // eax

	v2 = pnum;
	if ( gbBufferMsgs == 1 )
	{
		msg_send_packet(pnum, pCmd, 2);
	}
	else if ( pnum != myplr )
	{
		_LOBYTE(pnum) = pCmd->bLoc;
		inv_update_rem_item(v2, pnum);
	}
	return 2;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_PLRLEVEL(struct TCmdParam1 *pCmd, int pnum)
{
	if ( gbBufferMsgs == 1 )
	{
		msg_send_packet(pnum, pCmd, 3);
	}
	else if ( pCmd->wParam1 <= 0x33u && pnum != myplr )
	{
		plr[pnum]._pLevel = pCmd->wParam1;
	}
	return 3;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_DROPITEM(struct TCmdPItem *pCmd, int pnum)
{
	if ( gbBufferMsgs == 1 )
		msg_send_packet(pnum, pCmd, 22);
	else
		delta_put_item(pCmd, (unsigned char)pCmd->x, (unsigned char)pCmd->y, plr[pnum].plrlevel);
	return 22;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_SEND_PLRINFO(struct TCmdPlrInfoHdr *pCmd, int pnum)
{
	struct TCmdPlrInfoHdr *v2; // esi

	v2 = pCmd;
	if ( gbBufferMsgs == 1 )
		msg_send_packet(pnum, pCmd, (unsigned short)pCmd->wBytes + 5);
	else
		multi_player_joins(pnum, pCmd, pCmd->bCmd == 2);
	return (unsigned short)v2->wBytes + 5;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_ACK_PLRINFO(struct TCmdPlrInfoHdr *pCmd, int pnum)
{
  return On_SEND_PLRINFO(pCmd, pnum);
}

int __fastcall On_PLAYER_JOINLEVEL(struct TCmdLocParam1 *pCmd, int pnum)
{
	int v2; // ebx
	struct TCmdLocParam1 *v3; // edi
	int v4; // esi
	int v5; // ecx
	int v6; // ST08_4
	unsigned char *v7; // edx
	int v8; // eax
	int v9; // ecx
	int v10; // eax
	int v11; // eax

	v2 = pnum;
	v3 = pCmd;
	if ( gbBufferMsgs == 1 )
	{
		msg_send_packet(pnum, pCmd, 5);
	}
	else
	{
		v4 = pnum;
		plr[pnum]._pLvlChanging = 0;
		if ( plr[pnum]._pName[0] && !plr[v4].plractive )
		{
			plr[v4].plractive = 1;
			++gbActivePlayers;
			EventPlrMsg("Player '%s' (level %d) just joined the game", plr[pnum]._pName, plr[v4]._pLevel);
		}
		if ( plr[v4].plractive )
		{
			if ( myplr != v2 )
			{
				plr[v4].WorldX = (unsigned char)v3->x;
				plr[v4].WorldY = (unsigned char)v3->y;
				v5 = (unsigned short)v3->wParam1;
				plr[v4]._pGFXLoad = 0;
				plr[v4].plrlevel = v5;
				if ( currlevel == plr[v4].plrlevel )
				{
					LoadPlrGFX(v2, PFILE_STAND);
					SyncInitPlr(v2);
					if ( (signed int)(plr[v4]._pHitPoints & 0xFFFFFFC0) <= 0 )
					{
						plr[v4]._pgfxnum = 0;
						LoadPlrGFX(v2, PFILE_DEATH);
						v6 = plr[v4]._pDWidth;
						v7 = plr[v4]._pDAnim[0];
						plr[v4]._pmode = PM_DEATH;
						NewPlrAnim(v2, v7, plr[v4]._pDFrames, 1, v6);
						v8 = plr[v4]._pAnimLen;
						v9 = v8 - 1;
						plr[v4]._pVar8 = 2 * v8;
						v10 = plr[v4].WorldX;
						plr[v4]._pAnimFrame = v9;
						dFlags[v10][plr[v4].WorldY] |= 4u;
					}
					else
					{
						StartStand(v2, 0);
					}
					v11 = AddVision(plr[v4].WorldX, plr[v4].WorldY, plr[v4]._pLightRad, v2 == myplr);
					plr[v4]._plid = -1;
					plr[v4]._pvid = v11;
				}
			}
		}
	}
	return 5;
}
// 676194: using guessed type char gbBufferMsgs;
// 67862C: using guessed type char gbActivePlayers;

int __fastcall On_ACTIVATEPORTAL(DJunk *pCmd, int pnum)
{
	signed int v2; // ebx
	int v3; // edi
	DJunk *v4; // esi
	int v5; // eax
	int v6; // edx
	int v7; // ecx
	int v8; // ST0C_4
	int v9; // ST08_4
	int v10; // ST04_4

	v2 = 1;
	v3 = pnum;
	v4 = pCmd;
	if ( gbBufferMsgs == 1 )
	{
		msg_send_packet(pnum, pCmd, 9);
	}
	else
	{
		ActivatePortal(
			pnum,
			(unsigned char)pCmd->portal[0].y,
			(unsigned char)pCmd->portal[0].level,
			*(unsigned short *)&pCmd->portal[0].ltype,
			*(unsigned short *)&pCmd->portal[1].x,
			*(unsigned short *)&pCmd->portal[1].level);
		if ( v3 != myplr )
		{
			if ( currlevel )
			{
				if ( currlevel == plr[v3].plrlevel )
				{
					v6 = nummissiles;
					v7 = 0;
					if ( nummissiles <= 0 )
						goto LABEL_19;
					do
					{
						v5 = 176 * missileactive[v7];
						if ( *(int *)((char *)&missile[0]._mitype + v5) == MIS_TOWN
						  && *(int *)((char *)&missile[0]._misource + v5) == v3 )
						{
							v2 = 0;
						}
						++v7;
					}
					while ( v7 < nummissiles );
					if ( v2 )
LABEL_19:
						AddWarpMissile(v3, (unsigned char)v4->portal[0].y, (unsigned char)v4->portal[0].level);
				}
				else
				{
					RemovePortalMissile(v3);
				}
			}
			else
			{
				AddInTownPortal(v3);
			}
		}
		_LOBYTE(v5) = v4->portal[1].level;
		_LOBYTE(v6) = v4->portal[0].y;
		v8 = v5;
		_LOBYTE(v5) = v4->portal[1].x;
		v9 = v5;
		_LOBYTE(v5) = v4->portal[0].ltype;
		v10 = v5;
		_LOBYTE(v5) = v4->portal[0].level;
		delta_open_portal(v3, v6, v5, v10, v9, v8);
	}
	return 9;
}
// 676194: using guessed type char gbBufferMsgs;

void __fastcall delta_open_portal(int pnum, int x, int y, int bLevel, int bLType, int bSetLvl)
{
	int v6; // eax

	v6 = pnum;
	sgbDeltaChanged = 1;
	sgJunk.portal[v6].y = y;
	sgJunk.portal[v6].level = bLevel;
	sgJunk.portal[v6].ltype = bLType;
	sgJunk.portal[v6].x = x;
	sgJunk.portal[v6].setlvl = bSetLvl;
}
// 67618C: using guessed type char sgbDeltaChanged;

int __fastcall On_DEACTIVATEPORTAL(struct TCmd *pCmd, int pnum)
{
	int v2; // esi

	v2 = pnum;
	if ( gbBufferMsgs == 1 )
	{
		msg_send_packet(pnum, pCmd, 1);
	}
	else
	{
		if ( PortalOnLevel(pnum) )
			RemovePortalMissile(v2);
		DeactivatePortal(v2);
		RemovePlrPortal(v2);
	}
	return 1;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_RETOWN(struct TCmd *pCmd, int pnum)
{
	int v2; // esi

	v2 = pnum;
	if ( gbBufferMsgs == 1 )
	{
		msg_send_packet(pnum, pCmd, 1);
	}
	else
	{
		if ( pnum == myplr )
		{
			deathflag = 0;
			gamemenu_off();
		}
		RestartTownLvl(v2);
	}
	return 1;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_SETSTR(struct TCmdParam1 *pCmd, int pnum)
{
	unsigned short v2; // cx

	if ( gbBufferMsgs == 1 )
	{
		msg_send_packet(pnum, pCmd, 3);
	}
	else
	{
		v2 = pCmd->wParam1;
		if ( v2 <= 0x2EEu && pnum != myplr )
			SetPlrStr(pnum, v2);
	}
	return 3;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_SETDEX(struct TCmdParam1 *pCmd, int pnum)
{
	unsigned short v2; // cx

	if ( gbBufferMsgs == 1 )
	{
		msg_send_packet(pnum, pCmd, 3);
	}
	else
	{
		v2 = pCmd->wParam1;
		if ( v2 <= 0x2EEu && pnum != myplr )
			SetPlrDex(pnum, v2);
	}
	return 3;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_SETMAG(struct TCmdParam1 *pCmd, int pnum)
{
	unsigned short v2; // cx

	if ( gbBufferMsgs == 1 )
	{
		msg_send_packet(pnum, pCmd, 3);
	}
	else
	{
		v2 = pCmd->wParam1;
		if ( v2 <= 0x2EEu && pnum != myplr )
			SetPlrMag(pnum, v2);
	}
	return 3;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_SETVIT(struct TCmdParam1 *pCmd, int pnum)
{
	unsigned short v2; // cx

	if ( gbBufferMsgs == 1 )
	{
		msg_send_packet(pnum, pCmd, 3);
	}
	else
	{
		v2 = pCmd->wParam1;
		if ( v2 <= 0x2EEu && pnum != myplr )
			SetPlrVit(pnum, v2);
	}
	return 3;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_STRING(struct TCmdString *pCmd, int pnum)
{
	const char *v2; // esi
	int v3; // edi
	size_t v4; // ebx

	v2 = pCmd->str;
	v3 = pnum;
	v4 = strlen(pCmd->str);
	if ( !gbBufferMsgs )
		SendPlrMsg(v3, v2);
	return v4 + 2;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_SYNCQUEST(struct TCmdQuest *pCmd, int pnum)
{
	if ( gbBufferMsgs == 1 )
	{
		msg_send_packet(pnum, pCmd, 5);
	}
	else
	{
		if ( pnum != myplr )
			SetMultiQuest(
				(unsigned char)pCmd->q,
				(unsigned char)pCmd->qstate,
				pCmd->qlog,
				(unsigned char)pCmd->qvar1);
		sgbDeltaChanged = 1;
	}
	return 5;
}
// 67618C: using guessed type char sgbDeltaChanged;
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_ENDSHIELD(int a1, int pnum)
{
	int v2; // ebx
	int i; // esi
	int v4; // edi
	int v5; // eax

	v2 = pnum;
	if ( gbBufferMsgs != 1 && pnum != myplr && currlevel == plr[pnum].plrlevel )
	{
		for ( i = 0; i < nummissiles; ++i )
		{
			v4 = missileactive[i];
			v5 = missileactive[i];
			if ( missile[v5]._mitype == MIS_MANASHIELD && missile[v5]._misource == v2 )
			{
				ClearMissileSpot(missileactive[i]);
				DeleteMissile(v4, i);
			}
		}
	}
	return 1;
}
// 676194: using guessed type char gbBufferMsgs;

#ifdef _DEBUG
int __fastcall On_CHEAT_EXPERIENCE(struct TCmd *pCmd, int pnum)
{
	if ( gbBufferMsgs == 1 )
	{
		msg_send_packet(pnum, pCmd, 1);
	}
	else if ( plr[pnum]._pLevel < 50 )
	{
		plr[pnum]._pExperience = plr[pnum]._pNextExper;
		NextPlrLevel(pnum);
	}
	return 1;
}

int __fastcall On_CHEAT_SPELL_LEVEL(struct TCmd *pCmd, int pnum)
{
	if ( gbBufferMsgs == 1 )
		msg_send_packet(pnum, pCmd, 1);
	else
		plr[pnum]._pSplLvl[plr[pnum]._pRSpell]++;

	return 1;
}
#endif

int __cdecl On_DEBUG()
{
	return 1;
}

int __fastcall On_NOVA(struct TCmdLoc *pCmd, int pnum)
{
	struct TCmdLoc *v2; // edi
	int v3; // esi

	v2 = pCmd;
	if ( gbBufferMsgs != 1 )
	{
		v3 = pnum;
		if ( currlevel == plr[pnum].plrlevel && pnum != myplr )
		{
			ClrPlrPath(pnum);
			plr[v3]._pSpell = SPL_NOVA;
			plr[v3]._pSplType = 4;
			plr[v3]._pSplFrom = 3;
			plr[v3].destAction = 12;
			plr[v3].destParam1 = (unsigned char)v2->x;
			plr[v3].destParam2 = (unsigned char)v2->y;
		}
	}
	return 3;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_SETSHIELD(int unused, int pnum)
{
	int result; // eax

	result = 1;
	if ( gbBufferMsgs != 1 )
		plr[pnum].pManaShield = 1;
	return result;
}
// 676194: using guessed type char gbBufferMsgs;

int __fastcall On_REMSHIELD(int unused, int pnum)
{
	int result; // eax

	result = 1;
	if ( gbBufferMsgs != 1 )
		plr[pnum].pManaShield = 0;
	return result;
}
// 676194: using guessed type char gbBufferMsgs;
