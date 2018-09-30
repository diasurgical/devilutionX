//HEADER_GOES_HERE

#include "../types.h"

#ifndef NO_GLOBALS
short sync_word_6AA708[MAXMONSTERS];
int syncmonsters; // weak
short sync_word_6AA89C[MAXMONSTERS];
int syncitems;
int sgnSyncPInv; // weak
#endif

int __fastcall sync_all_monsters(TSyncHeader *packet, int size)
{
	int result; // eax
	TSyncHeader *v3; // esi
	int v4; // ebx
	TSyncMonster *v5; // edi
	unsigned int v6; // [esp+4h] [ebp-4h]

	result = size;
	if ( nummonsters >= 1 && (unsigned int)size >= 0x2B )
	{
		v3 = packet;
		v6 = size - 38;
		v4 = 0;
		packet->bCmd = CMD_SYNCDATA;
		v5 = (TSyncMonster *)(&packet->bPInvId + 1);
		packet->bLevel = currlevel;
		packet->wLen = 0;
		SyncPlrInv(packet);
		sync_one_monster();
		if ( nummonsters > 0 )
		{
			do
			{
				if ( v6 < 5 || (v4 >= 2 || !sync_monster_active2(v5)) && !sync_monster_active(v5) )
					break;
				v3->wLen += 5;
				v6 -= 5;
				++v5;
				++v4;
			}
			while ( v4 < nummonsters );
		}
		result = v6;
	}
	return result;
}

void __cdecl sync_one_monster()
{
	int i; // ebx
	int v1; // edi
	int v2; // esi
	short v3; // bp
	short v4; // ax
	bool v5; // zf
	short *v6; // edx
	short *v7; // eax

	for ( i = 0; i < nummonsters; ++i )
	{
		v1 = monstactive[i];
		v2 = monstactive[i];
		v3 = abs(plr[myplr].WorldY - monster[v2]._my);
		v4 = abs(plr[myplr].WorldX - monster[v2]._mx);
		v5 = monster[v2]._msquelch == 0;
		v6 = &sync_word_6AA708[v1];
		*v6 = v4 + v3;
		if ( v5 )
		{
			*v6 = v4 + v3 + 4096;
		}
		else
		{
			v7 = &sync_word_6AA89C[v1];
			if ( *v7 )
				--*v7;
		}
	}
}

int __fastcall sync_monster_active(TSyncMonster *packet)
{
	unsigned int v1; // ebx
	int v2; // esi
	int v3; // edx
	int v4; // eax

	v1 = -1;
	v2 = 0;
	v3 = -1;
	if ( nummonsters <= 0 )
		return 0;
	do
	{
		v4 = monstactive[v2];
		if ( (unsigned short)sync_word_6AA708[v4] < v1 && (unsigned short)sync_word_6AA89C[v4] < 0xFFFEu )
		{
			v1 = (unsigned short)sync_word_6AA708[v4];
			v3 = monstactive[v2];
		}
		++v2;
	}
	while ( v2 < nummonsters );
	if ( v3 == -1 )
		return 0;
	sync_monster_pos(packet, v3);
	return 1;
}

void __fastcall sync_monster_pos(TSyncMonster *packet, int mon_id)
{
	int v2; // ebx
	TSyncMonster *v3; // esi
	int v4; // edi
	int v5; // eax
	short v6; // cx
	char v7; // cl

	v2 = mon_id;
	v3 = packet;
	v4 = mon_id;
	packet->_mndx = mon_id;
	packet->_mx = monster[mon_id]._mx;
	packet->_my = monster[mon_id]._my;
	packet->_menemy = encode_enemy(mon_id);
	v5 = v2;
	v6 = sync_word_6AA708[v2];
	if ( (unsigned short)v6 > 0xFFu )
		_LOBYTE(v6) = -1;
	v3->_mdelta = v6;
	v7 = monster[v4]._msquelch;
	sync_word_6AA708[v5] = -1;
	sync_word_6AA89C[v5] = -(v7 != 0) - 1;
}

int __fastcall sync_monster_active2(TSyncMonster *packet)
{
	int v1; // edx
	unsigned int v2; // ebp
	int v3; // eax
	int v4; // esi
	int v6; // [esp+8h] [ebp-4h]

	v1 = -1;
	v2 = 65534;
	if ( nummonsters <= 0 )
		return 0;
	v3 = syncmonsters;
	v6 = nummonsters;
	do
	{
		if ( v3 >= nummonsters )
			v3 = 0;
		v4 = monstactive[v3];
		if ( (unsigned short)sync_word_6AA89C[v4] < v2 )
		{
			v2 = (unsigned short)sync_word_6AA89C[v4];
			v1 = monstactive[v3];
		}
		++v3;
		--v6;
	}
	while ( v6 );
	syncmonsters = v3;
	if ( v1 == -1 )
		return 0;
	sync_monster_pos(packet, v1);
	return 1;
}
// 6AA898: using guessed type int syncmonsters;

void __fastcall SyncPlrInv(TSyncHeader *pSync)
{
	int v1; // edx
	int v2; // eax
	int v3; // eax
	short v4; // dx
	short v5; // bx
	ItemStruct *pItem; // eax

	if ( numitems <= 0 )
	{
		pSync->bItemI = -1;
	}
	else
	{
		v1 = syncitems;
		if ( syncitems >= numitems )
			v1 = 0;
		v2 = itemactive[v1];
		syncitems = v1 + 1;
		pSync->bItemI = v2;
		v3 = v2;
		pSync->bItemX = item[v3]._ix;
		pSync->bItemY = item[v3]._iy;
		pSync->wItemIndx = item[v3].IDidx;
		if ( item[v3].IDidx == IDI_EAR )
		{
			_LOBYTE(v4) = 0;
			_HIBYTE(v4) = item[v3]._iName[7];
			_LOBYTE(v5) = 0;
			pSync->wItemCI = item[v3]._iName[8] | v4;
			pSync->dwItemSeed = item[v3]._iName[12] | ((item[v3]._iName[11] | ((item[v3]._iName[10] | (item[v3]._iName[9] << 8)) << 8)) << 8);
			pSync->bItemId = item[v3]._iName[13];
			pSync->bItemDur = item[v3]._iName[14];
			pSync->bItemMDur = item[v3]._iName[15];
			pSync->bItemCh = item[v3]._iName[16];
			pSync->bItemMCh = item[v3]._iName[17];
			_HIBYTE(v5) = item[v3]._iName[18];
			pSync->wItemVal = LOWORD(item[v3]._ivalue) | v5 | ((LOWORD(item[v3]._iCurs) - 19) << 6);
			pSync->dwItemBuff = item[v3]._iName[22] | ((item[v3]._iName[21] | ((item[v3]._iName[20] | (item[v3]._iName[19] << 8)) << 8)) << 8);
		}
		else
		{
			pSync->wItemCI = item[v3]._iCreateInfo;
			pSync->dwItemSeed = item[v3]._iSeed;
			pSync->bItemId = item[v3]._iIdentified;
			pSync->bItemDur = item[v3]._iDurability;
			pSync->bItemMDur = item[v3]._iMaxDur;
			pSync->bItemCh = item[v3]._iCharges;
			pSync->bItemMCh = item[v3]._iMaxCharges;
			if ( !item[v3].IDidx )
				pSync->wItemVal = item[v3]._ivalue;
		}
	}
	pItem = &plr[myplr].InvBody[sgnSyncPInv];
	if ( pItem->_itype == -1 )
	{
		pSync->bPInvLoc = -1;
	}
	else
	{
		pSync->bPInvLoc = sgnSyncPInv;
		pSync->wPInvIndx = pItem->IDidx;
		pSync->wPInvCI = pItem->_iCreateInfo;
		pSync->dwPInvSeed = pItem->_iSeed;
		pSync->bPInvId = pItem->_iIdentified;
	}
	if ( ++sgnSyncPInv >= 7 )
		sgnSyncPInv = 0;
}
// 6AAA34: using guessed type int sgnSyncPInv;

int __fastcall SyncData(int pnum, TSyncHeader *packet)
{
	TSyncHeader *v2; // esi
	TSyncMonster *v3; // edi
	int v4; // ebp
	unsigned short v5; // ax
	unsigned int v6; // ebx

	v2 = packet;
	v3 = (TSyncMonster *)(&packet->bPInvId + 1);
	v4 = pnum;
	if ( packet->bCmd != CMD_SYNCDATA )
		TermMsg("bad sync command");
	if ( gbBufferMsgs != 1 && v4 != myplr )
	{
		v5 = v2->wLen;
		if ( v5 >= 5u )
		{
			v6 = v5 / 5u;
			do
			{
				if ( currlevel == v2->bLevel )
					sync_monster_data(v4, v3);
				delta_sync_monster((TCmdLocParam1 *)v3, v2->bLevel);
				++v3;
				--v6;
			}
			while ( v6 );
		}
	}
	return v2->wLen + 38;
}
// 676194: using guessed type char gbBufferMsgs;

void __fastcall sync_monster_data(int pnum, TSyncMonster *packet)
{
	TSyncMonster *v2; // edi
	int v3; // ecx
	int v4; // ebx
	int v5; // esi
	int v6; // ST18_4
	unsigned int v7; // ecx
	unsigned int v8; // eax
	int v9; // eax
	int v10; // ecx
	signed int v11; // ST14_4
	int v12; // eax
	int v13; // eax
	int v14; // eax
	//int v15; // eax
	int v16; // eax
	int md; // [esp+Ch] [ebp-8h]
	int mda; // [esp+Ch] [ebp-8h]

	v2 = packet;
	md = pnum;
	v3 = 0;
	v4 = (unsigned char)packet->_mndx;
	v5 = v4;
	if ( monster[v5]._mhitpoints )
	{
		if ( nummonsters > 0 )
		{
			do
			{
				if ( monstactive[v3] == v4 )
					break;
				++v3;
			}
			while ( v3 < nummonsters );
		}
		v6 = abs(plr[myplr].WorldY - monster[v5]._my);
		v7 = abs(plr[myplr].WorldX - monster[v5]._mx) + v6;
		if ( v7 > 0xFF )
			v7 = 255;
		v8 = (unsigned char)v2->_mdelta;
		if ( v7 >= v8 && (v7 != v8 || md <= myplr) )
		{
			v9 = (unsigned char)v2->_mx;
			if ( monster[v5]._mfutx != v9 || monster[v5]._mfuty != (unsigned char)v2->_my )
			{
				v10 = monster[v5]._mmode;
				if ( v10 != MM_CHARGE && v10 != MM_STONE )
				{
					v11 = abs(monster[v5]._mx - v9);
					v12 = abs(monster[v5]._my - (unsigned char)v2->_my);
					if ( v11 > 2 || v12 > 2 )
					{
						if ( dMonster[0][(unsigned char)v2->_my + 112 * (unsigned char)v2->_mx] )
						{
LABEL_23:
							decode_enemy(v4, (unsigned char)v2->_menemy);
							return;
						}
						M_ClearSquares(v4);
						dMonster[0][(unsigned char)v2->_my + 112 * (unsigned char)v2->_mx] = v4 + 1;
						monster[v5]._mx = (unsigned char)v2->_mx;
						monster[v5]._my = (unsigned char)v2->_my;
						decode_enemy(v4, (unsigned char)v2->_menemy);
						v16 = GetDirection(
								  (unsigned char)v2->_mx,
								  (unsigned char)v2->_my,
								  (unsigned char)monster[v5]._menemyx,
								  (unsigned char)monster[v5]._menemyy);
						M_StartStand(v4, v16);
					}
					else
					{
						v13 = monster[v5]._mmode;
						if ( v13 >= MM_WALK && v13 <= MM_WALK3 )
							goto LABEL_23;
						v14 = GetDirection(
								  monster[v5]._mx,
								  monster[v5]._my,
								  (unsigned char)v2->_mx,
								  (unsigned char)v2->_my);
						mda = v14;
						//_LOBYTE(v15) = DirOK(v4, v14);
						if ( !DirOK(v4, v14) )
							goto LABEL_23;
						M_ClearSquares(v4);
						dMonster[0][monster[v5]._my + 112 * monster[v5]._mx] = v4 + 1;
						M_WalkDir(v4, mda);
					}
					monster[v5]._msquelch = -1;
					goto LABEL_23;
				}
			}
		}
	}
}

void __cdecl sync_clear_pkt()
{
	syncmonsters = 16 * myplr;
	memset(sync_word_6AA89C, 255, 0x190u);
}
// 6AA898: using guessed type int syncmonsters;
