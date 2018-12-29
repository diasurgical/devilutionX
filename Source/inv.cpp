//HEADER_GOES_HERE

#include "../types.h"

#ifndef NO_GLOBALS
BOOL invflag;
void *pInvCels;
int drawsbarflag; // idb
int sgdwLastTime; // check name
#endif

const InvXY InvRect[73] =
{
  { 452, 31 },  // helmet
  { 480, 31 },  // helmet
  { 452, 59 },  // helmet
  { 480, 59 },  // helmet
  { 365, 205 }, // left ring
  { 567, 205 }, // right ring
  { 524, 59 },  // amulet
  { 337, 104 }, // left hand
  { 366, 104 }, // left hand
  { 337, 132 }, // left hand
  { 366, 132 }, // left hand
  { 337, 160 }, // left hand
  { 366, 160 }, // left hand
  { 567, 104 }, // right hand
  { 596, 104 }, // right hand
  { 567, 132 }, // right hand
  { 596, 132 }, // right hand
  { 567, 160 }, // right hand
  { 596, 160 }, // right hand
  { 452, 104 }, // chest
  { 480, 104 }, // chest
  { 452, 132 }, // chest
  { 480, 132 }, // chest
  { 452, 160 }, // chest
  { 480, 160 }, // chest
  { 337, 250 }, // inv row 1
  { 366, 250 }, // inv row 1
  { 394, 250 }, // inv row 1
  { 423, 250 }, // inv row 1
  { 451, 250 }, // inv row 1
  { 480, 250 }, // inv row 1
  { 509, 250 }, // inv row 1
  { 538, 250 }, // inv row 1
  { 567, 250 }, // inv row 1
  { 596, 250 }, // inv row 1
  { 337, 279 }, // inv row 2
  { 366, 279 }, // inv row 2
  { 394, 279 }, // inv row 2
  { 423, 279 }, // inv row 2
  { 451, 279 }, // inv row 2
  { 480, 279 }, // inv row 2
  { 509, 279 }, // inv row 2
  { 538, 279 }, // inv row 2
  { 567, 279 }, // inv row 2
  { 596, 279 }, // inv row 2
  { 337, 308 }, // inv row 3
  { 366, 308 }, // inv row 3
  { 394, 308 }, // inv row 3
  { 423, 308 }, // inv row 3
  { 451, 308 }, // inv row 3
  { 480, 308 }, // inv row 3
  { 509, 308 }, // inv row 3
  { 538, 308 }, // inv row 3
  { 567, 308 }, // inv row 3
  { 596, 308 }, // inv row 3
  { 337, 336 }, // inv row 4
  { 366, 336 }, // inv row 4
  { 394, 336 }, // inv row 4
  { 423, 336 }, // inv row 4
  { 451, 336 }, // inv row 4
  { 480, 336 }, // inv row 4
  { 509, 336 }, // inv row 4
  { 538, 336 }, // inv row 4
  { 567, 336 }, // inv row 4
  { 596, 336 }, // inv row 4
  { 205, 385 }, // belt
  { 234, 385 }, // belt
  { 263, 385 }, // belt
  { 292, 385 }, // belt
  { 321, 385 }, // belt
  { 350, 385 }, // belt
  { 379, 385 }, // belt
  { 408, 385 }  // belt
};

/* data */

int AP2x2Tbl[10] = { 8, 28, 6, 26, 4, 24, 2, 22, 0, 20 }; // weak

void __cdecl FreeInvGFX()
{
	void *invCels = pInvCels;

	pInvCels = NULL;
	mem_free_dbg(invCels);
}

void __cdecl InitInv()
{
	if ( plr[myplr]._pClass == PC_WARRIOR )
	{
		pInvCels = LoadFileInMem("Data\\Inv\\Inv.CEL", 0);
	}
	else if ( plr[myplr]._pClass == PC_ROGUE )
	{
		pInvCels = LoadFileInMem("Data\\Inv\\Inv_rog.CEL", 0);
	}
	else if ( plr[myplr]._pClass == PC_SORCERER )
	{
		pInvCels = LoadFileInMem("Data\\Inv\\Inv_Sor.CEL", 0);
	}

	invflag = 0;
	drawsbarflag = 0;
}

void __fastcall InvDrawSlotBack(int X, int Y, int W, int H)
{
	unsigned char *v4; // edi
	int v5; // edx
	int v6; // ecx
	unsigned char v7; // al
	unsigned char v8; // al

	v4 = (unsigned char *)gpBuffer + screen_y_times_768[Y] + X;
	v5 = (unsigned short)H;
	do
	{
		v6 = (unsigned short)W;
		do
		{
			v7 = *v4;
			if ( *v4 < 0xB0u )
				goto LABEL_9;
			if ( v7 > 0xBFu )
			{
				if ( v7 < 0xF0u )
					goto LABEL_9;
				v8 = v7 - 80;
			}
			else
			{
				v8 = v7 - 16;
			}
			*v4 = v8;
LABEL_9:
			++v4;
			--v6;
		}
		while ( v6 );
		v4 = &v4[-(unsigned short)W - 768];
		--v5;
	}
	while ( v5 );
}

void __cdecl DrawInv()
{
	BOOL invtest[40];

	CelDecodeOnly(384, 511, pInvCels, 1, 320);

	if ( plr[myplr].InvBody[INVLOC_HEAD]._itype != ITYPE_NONE )
	{
		InvDrawSlotBack(517, 219, 2 * INV_SLOT_SIZE_PX, 2 * INV_SLOT_SIZE_PX);

		int frame = plr[myplr].InvBody[INVLOC_HEAD]._iCurs + 12;
		int frame_width = InvItemWidth[frame];

		printf("FRAME %d, WIDTH %d", frame, frame_width);

		if ( pcursinvitem == INVITEM_HEAD )
		{
			int colour = 197;
			if ( plr[myplr].InvBody[INVLOC_HEAD]._iMagical )
			{
				colour = 181;
			}
			if ( !plr[myplr].InvBody[INVLOC_HEAD]._iStatFlag )
			{
				colour = 229;
			}
			CelDecodeClr(colour, 517, 219, (char *)pCursCels, frame, frame_width, 0, 8);
		}

		if ( plr[myplr].InvBody[INVLOC_HEAD]._iStatFlag )
		{
			CelDrawHdrOnly(517, 219, (char *)pCursCels, frame, frame_width, 0, 8);
		}
		else
		{
			CelDrawHdrLightRed(517, 219, (char *)pCursCels, frame, frame_width, 0, 8, 1);
		}
	}

	if ( plr[myplr].InvBody[INVLOC_RING_LEFT]._itype != ITYPE_NONE )
	{
		InvDrawSlotBack(432, 365, INV_SLOT_SIZE_PX, INV_SLOT_SIZE_PX);

		int frame = plr[myplr].InvBody[INVLOC_RING_LEFT]._iCurs + 12;
		int frame_width = InvItemWidth[frame];

		if ( pcursinvitem == INVITEM_RING_LEFT )
		{
			int colour = 197;
			if ( plr[myplr].InvBody[INVLOC_RING_LEFT]._iMagical )
			{
				colour = 181;
			}
			if ( !plr[myplr].InvBody[INVLOC_RING_LEFT]._iStatFlag )
			{
				colour = 229;
			}
			CelDecodeClr(colour, 432, 365, (char *)pCursCels, frame, frame_width, 0, 8);
		}

		if ( plr[myplr].InvBody[INVLOC_RING_LEFT]._iStatFlag )
		{
			CelDrawHdrOnly(432, 365, (char *)pCursCels, frame, frame_width, 0, 8);
		}
		else
		{
			CelDrawHdrLightRed(432, 365, (char *)pCursCels, frame, frame_width, 0, 8, 1);
		}
	}

	if ( plr[myplr].InvBody[INVLOC_RING_RIGHT]._itype != ITYPE_NONE )
	{
		InvDrawSlotBack(633, 365, INV_SLOT_SIZE_PX, INV_SLOT_SIZE_PX);

		int frame = plr[myplr].InvBody[INVLOC_RING_RIGHT]._iCurs + 12;
		int frame_width = InvItemWidth[frame];

		if ( pcursinvitem == INVITEM_RING_RIGHT )
		{
			int colour = 197;
			if ( plr[myplr].InvBody[INVLOC_RING_RIGHT]._iMagical )
			{
				colour = 181;
			}
			if ( !plr[myplr].InvBody[INVLOC_RING_RIGHT]._iStatFlag )
			{
				colour = 229;
			}
			CelDecodeClr(colour, 633, 365, (char *)pCursCels, frame, frame_width, 0, 8);
		}

		if ( plr[myplr].InvBody[INVLOC_RING_RIGHT]._iStatFlag )
		{
			CelDrawHdrOnly(633, 365, (char *)pCursCels, frame, frame_width, 0, 8);
		}
		else
		{
			CelDrawHdrLightRed(633, 365, (char *)pCursCels, frame, frame_width, 0, 8, 1);
		}
	}

	if ( plr[myplr].InvBody[INVLOC_AMULET]._itype != ITYPE_NONE )
	{
		InvDrawSlotBack(589, 220, INV_SLOT_SIZE_PX, INV_SLOT_SIZE_PX);

		int frame = plr[myplr].InvBody[INVLOC_AMULET]._iCurs + 12;
		int frame_width = InvItemWidth[frame];

		if ( pcursinvitem == INVITEM_AMULET )
		{
			int colour = 197;
			if ( plr[myplr].InvBody[INVLOC_AMULET]._iMagical )
			{
				colour = 181;
			}
			if ( !plr[myplr].InvBody[INVLOC_AMULET]._iStatFlag )
			{
				colour = 229;
			}
			CelDecodeClr(colour, 589, 220, (char *)pCursCels, frame, frame_width, 0, 8);
		}

		if ( plr[myplr].InvBody[INVLOC_AMULET]._iStatFlag )
		{
			CelDrawHdrOnly(589, 220, (char *)pCursCels, frame, frame_width, 0, 8);
		}
		else
		{
			CelDrawHdrLightRed(589, 220, (char *)pCursCels, frame, frame_width, 0, 8, 1);
		}
	}

	if ( plr[myplr].InvBody[INVLOC_HAND_LEFT]._itype != ITYPE_NONE )
	{
		InvDrawSlotBack(401, 320, 2 * INV_SLOT_SIZE_PX, 3 * INV_SLOT_SIZE_PX);

		int frame = plr[myplr].InvBody[INVLOC_HAND_LEFT]._iCurs + 12;
		int frame_width = InvItemWidth[frame];
		// calc item offsets for weapons smaller than 2x3 slots
		int screen_x = frame_width == INV_SLOT_SIZE_PX ? 415 : 401;
		int screen_y = InvItemHeight[frame] == (3 * INV_SLOT_SIZE_PX) ? 320 : 306;

		if ( pcursinvitem == INVITEM_HAND_LEFT )
		{
			int colour = 197;
			if ( plr[myplr].InvBody[INVLOC_HAND_LEFT]._iMagical )
			{
				colour = 181;
			}
			if ( !plr[myplr].InvBody[INVLOC_HAND_LEFT]._iStatFlag )
			{
				colour = 229;
			}
			CelDecodeClr(colour, screen_x, screen_y, (char *)pCursCels, frame, frame_width, 0, 8);
		}

		if ( plr[myplr].InvBody[INVLOC_HAND_LEFT]._iStatFlag )
		{
			CelDrawHdrOnly(screen_x, screen_y, (char *)pCursCels, frame, frame_width, 0, 8);
		}
		else
		{
			CelDrawHdrLightRed(screen_x, screen_y, (char *)pCursCels, frame, frame_width, 0, 8, 1);
		}

		if ( plr[myplr].InvBody[INVLOC_HAND_LEFT]._iLoc == ILOC_TWOHAND )
		{
			InvDrawSlotBack(631, 320, 2 * INV_SLOT_SIZE_PX, 3 * INV_SLOT_SIZE_PX);
			light_table_index = 0;
			cel_transparency_active = 1;

			CelDecodeHdrLightTrans(
				frame_width == INV_SLOT_SIZE_PX
					? &gpBuffer->row[160].pixels[581]
					: &gpBuffer->row[160].pixels[567],
				(char *)pCursCels, frame, frame_width, 0, 8);

			cel_transparency_active = 0;
		}
	}
	if ( plr[myplr].InvBody[INVLOC_HAND_RIGHT]._itype != ITYPE_NONE )
	{
		InvDrawSlotBack(631, 320, 2 * INV_SLOT_SIZE_PX, 3 * INV_SLOT_SIZE_PX);

		int frame = plr[myplr].InvBody[INVLOC_HAND_RIGHT]._iCurs + 12;
		int frame_width = InvItemWidth[frame];
		// calc item offsets for weapons smaller than 2x3 slots
		int screen_x = frame_width == INV_SLOT_SIZE_PX ? 645 : 633;
		int screen_y = InvItemHeight[frame] == 3 * INV_SLOT_SIZE_PX ? 320 : 306;

		if ( pcursinvitem == INVITEM_HAND_RIGHT )
		{
			int colour = 197;
			if ( plr[myplr].InvBody[INVLOC_HAND_RIGHT]._iMagical )
			{
				colour = 181;
			}
			if ( !plr[myplr].InvBody[INVLOC_HAND_RIGHT]._iStatFlag )
			{
				colour = 229;
			}
			CelDecodeClr(colour, screen_x, screen_y, (char *)pCursCels, frame, frame_width, 0, 8);
		}

		if ( plr[myplr].InvBody[INVLOC_HAND_RIGHT]._iStatFlag )
		{
			CelDrawHdrOnly(screen_x, screen_y, (char *)pCursCels, frame, frame_width, 0, 8);
		}
		else
		{
			CelDrawHdrLightRed(screen_x, screen_y, (char *)pCursCels, frame, frame_width, 0, 8, 1);
		}
	}


	if ( plr[myplr].InvBody[INVLOC_CHEST]._itype != ITYPE_NONE )
	{
		InvDrawSlotBack(517, 320, 2 * INV_SLOT_SIZE_PX, 3 * INV_SLOT_SIZE_PX);

		int frame = plr[myplr].InvBody[INVLOC_CHEST]._iCurs + 12;
		int frame_width = InvItemWidth[frame];
		

		if ( pcursinvitem == INVITEM_CHEST )
		{
			int colour = 197;
			if ( plr[myplr].InvBody[INVLOC_CHEST]._iMagical )
			{
				colour = 181;
			}
			if ( !plr[myplr].InvBody[INVLOC_CHEST]._iStatFlag )
			{
				colour = 229;
			}
			CelDecodeClr(colour, 517, 320, (char *)pCursCels, frame, frame_width, 0, 8);
		}

		if ( plr[myplr].InvBody[INVLOC_CHEST]._iStatFlag )
		{
			CelDrawHdrOnly(517, 320, (char *)pCursCels, frame, frame_width, 0, 8);
		}
		else
		{
			CelDrawHdrLightRed(517, 320, (char *)pCursCels, frame, frame_width, 0, 8, 1);
		}
	}

	for ( int i = 0; i < NUM_INV_GRID_ELEM; i++ )
	{
		if ( plr[myplr].InvGrid[i] != 0 )
		{
			InvDrawSlotBack(
				InvRect[i + SLOTXY_INV_FIRST].X + 64,
				InvRect[i + SLOTXY_INV_FIRST].Y + 159,
				INV_SLOT_SIZE_PX,
				INV_SLOT_SIZE_PX);
		}
	}

	for ( int j = 0; j < NUM_INV_GRID_ELEM; j++ )
	{
		if ( plr[myplr].InvGrid[j] > 0 ) // first slot of an item
		{
			int ii = plr[myplr].InvGrid[j] - 1;

			invtest[j] = TRUE;

			int frame = plr[myplr].InvList[ii]._iCurs + 12;
			int frame_width = InvItemWidth[frame];
			if ( pcursinvitem == ii + INVITEM_INV_FIRST )
			{
				int colour = 197;
				if ( plr[myplr].InvList[ii]._iMagical )
				{
					colour = 181;
				}
				if ( !plr[myplr].InvList[ii]._iStatFlag )
				{
					colour = 229;
				}
				CelDecodeClr(
					colour,
					InvRect[j + SLOTXY_INV_FIRST].X + 64,
					InvRect[j + SLOTXY_INV_FIRST].Y + 159,
					(char *)pCursCels, frame, frame_width, 0, 8);
			}

			if ( plr[myplr].InvList[ii]._iStatFlag )
			{
				CelDrawHdrOnly(
					InvRect[j + SLOTXY_INV_FIRST].X + 64,
					InvRect[j + SLOTXY_INV_FIRST].Y + 159,
					(char *)pCursCels, frame, frame_width, 0, 8);
			}
			else
			{
				CelDrawHdrLightRed(
					InvRect[j + SLOTXY_INV_FIRST].X + 64,
					InvRect[j + SLOTXY_INV_FIRST].Y + 159,
					(char *)pCursCels, frame, frame_width, 0, 8, 1);
			}
		}
	}
}
// 4B8CB8: using guessed type char pcursinvitem;
// 69BEF8: using guessed type int light_table_index;
// 69CF94: using guessed type int cel_transparency_active;
// 41B8C4: using guessed type int var_A0[40];

void __cdecl DrawInvBelt()
{
	int v0; // ebx
	signed int v1; // esi
	int v2; // ecx
	int v3; // eax
	int v4; // edi
	char v5; // cl
	int v6; // edx
	bool v7; // zf
	int v8; // ecx
	int v9; // eax
	unsigned char v10; // edx
	signed int v11; // [esp+4h] [ebp-Ch]
	int frame_width; // [esp+8h] [ebp-8h]
	int v13; // [esp+Ch] [ebp-4h]

	v0 = 0;
	if ( !talkflag )
	{
		DrawPanelBox(205, 21, 0xE8u, 0x1Cu, 269, 517);
		v11 = 0;
		v13 = 0;
		do
		{
			if ( *(int *)((char *)&plr[myplr].SpdList[0]._itype + v0) != -1 )
			{
				v1 = v11;
				InvDrawSlotBack(InvRect[v11 + 65].X + 64, InvRect[v11 + 65].Y + 159, 28, 28);
				v2 = myplr;
				v3 = v0 + 21720 * myplr;
				v4 = *(int *)((char *)&plr[0].SpdList[0]._iCurs + v3) + 12;
				frame_width = InvItemWidth[v4];
				if ( pcursinvitem == v11 + 47 )
				{
					v5 = -59;
					if ( *(&plr[0].SpdList[0]._iMagical + v3) )
						v5 = -75;
					if ( !*(int *)((char *)&plr[0].SpdList[0]._iStatFlag + v3) )
						v5 = -27;
					CelDecodeClr(
						v5,
						InvRect[v1 + 65].X + 64,
						InvRect[v1 + 65].Y + 159,
						(char *)pCursCels,
						v4,
						frame_width,
						0,
						8);
					v2 = myplr;
				}
				v0 = v13;
				v6 = InvRect[v1 + 65].Y + 159;
				v7 = *(int *)((char *)&plr[v2].SpdList[0]._iStatFlag + v13) == 0;
				v8 = InvRect[v1 + 65].X;
				if ( v7 )
					CelDrawHdrLightRed(v8 + 64, v6, (char *)pCursCels, v4, frame_width, 0, 8, 1);
				else
					CelDrawHdrOnly(v8 + 64, v6, (char *)pCursCels, v4, frame_width, 0, 8);
				v9 = v13 + 21720 * myplr;
				if ( AllItemsList[*(int *)((char *)&plr[0].SpdList[0].IDidx + v9)].iUsable
				  && *(int *)((char *)&plr[0].SpdList[0]._iStatFlag + v9)
				  && *(int *)((char *)&plr[0].SpdList[0]._itype + v9) != 11 )
				{
					v10 = fontframe[fontidx[(unsigned char)(v11 + 49)]];
					CPrintString(
						screen_y_times_768[InvRect[v1 + 65].Y + 159]
					  - fontkern[v10]
					  + InvRect[v1 + 65].X
					  + 92,
						v10,
						0);
				}
			}
			++v11;
			v0 += 368;
			v13 = v0;
		}
		while ( v11 < MAXBELTITEMS );
	}
}
// 4B8960: using guessed type int talkflag;
// 4B8CB8: using guessed type char pcursinvitem;

int __fastcall AutoPlace(int pnum, int ii, int sx, int sy, int saveflag)
{
	__int64 v5; // rax
	int v6; // ebx
	signed int v7; // edx
	signed int v8; // eax
	signed int v9; // esi
	int j; // edi
	int v11; // eax
	signed int v12; // esi
	signed int v13; // ecx
	int v14; // edi
	char *v15; // ecx
	char v16; // dl
	signed int v18; // [esp+Ch] [ebp-Ch]
	int p; // [esp+10h] [ebp-8h]
	int v20; // [esp+14h] [ebp-4h]
	int i; // [esp+14h] [ebp-4h]

	p = pnum;
	v5 = ii;
	v6 = 1;
	v18 = v5 % 10;
	v7 = 10 * (unsigned __int64)(v5 / 10);
	v8 = v7;
	if ( v7 < 0 )
		v8 = 0;
	v20 = 0;
	if ( sy <= 0 )
	{
LABEL_16:
		if ( saveflag )
		{
			v11 = pnum;
			qmemcpy(
				&plr[pnum].InvList[plr[pnum]._pNumInv],
				&plr[pnum].HoldItem,
				sizeof(plr[pnum].InvList[plr[pnum]._pNumInv]));
			++plr[v11]._pNumInv;
			v12 = v7;
			if ( v7 < 0 )
				v12 = 0;
			for ( i = 0; i < sy; ++i )
			{
				v13 = v18;
				if ( v18 < 0 )
					v13 = 0;
				v14 = 0;
				if ( sx > 0 )
				{
					v15 = &plr[v11].InvGrid[v13 + v12];
					do
					{
						if ( v14 || i != sy - 1 )
							v16 = -_LOBYTE(plr[v11]._pNumInv);
						else
							v16 = plr[v11]._pNumInv;
						*v15++ = v16;
						++v14;
					}
					while ( v14 < sx );
				}
				v12 += 10;
			}
			CalcPlrScrolls(p);
		}
	}
	else
	{
		while ( v6 )
		{
			if ( v8 >= 40 )
				v6 = 0;
			v9 = v18;
			if ( v18 < 0 )
				v9 = 0;
			for ( j = 0; j < sx; ++j )
			{
				if ( !v6 )
					break;
				v6 = 0;
				if ( v9 < 10 )
					_LOBYTE(v6) = plr[pnum].InvGrid[v9 + v8] == 0;
				++v9;
			}
			v8 += 10;
			if ( ++v20 >= sy )
			{
				if ( !v6 )
					return v6;
				goto LABEL_16;
			}
		}
	}
	return v6;
}

int __fastcall SpecialAutoPlace(int pnum, int ii, int sx, int sy, int saveflag)
{
	__int64 v5; // rax
	int v6; // ebx
	signed int v7; // edx
	signed int v8; // eax
	signed int v9; // esi
	int j; // edi
	signed int v11; // ecx
	int *v12; // eax
	int v13; // eax
	signed int v14; // esi
	signed int v15; // ecx
	int v16; // edi
	char *v17; // ecx
	char v18; // dl
	signed int v20; // [esp+Ch] [ebp-Ch]
	int p; // [esp+10h] [ebp-8h]
	int v22; // [esp+14h] [ebp-4h]
	int i; // [esp+14h] [ebp-4h]

	p = pnum;
	v5 = ii;
	v6 = 1;
	v20 = v5 % 10;
	v7 = 10 * (unsigned __int64)(v5 / 10);
	v8 = v7;
	if ( v7 < 0 )
		v8 = 0;
	v22 = 0;
	if ( sy <= 0 )
	{
LABEL_25:
		if ( saveflag )
		{
			v13 = p;
			qmemcpy(&plr[p].InvList[plr[p]._pNumInv], &plr[p].HoldItem, sizeof(plr[p].InvList[plr[p]._pNumInv]));
			++plr[v13]._pNumInv;
			v14 = v7;
			if ( v7 < 0 )
				v14 = 0;
			for ( i = 0; i < sy; ++i )
			{
				v15 = v20;
				if ( v20 < 0 )
					v15 = 0;
				v16 = 0;
				if ( sx > 0 )
				{
					v17 = &plr[v13].InvGrid[v15 + v14];
					do
					{
						if ( v16 || i != sy - 1 )
							v18 = -_LOBYTE(plr[v13]._pNumInv);
						else
							v18 = plr[v13]._pNumInv;
						*v17++ = v18;
						++v16;
					}
					while ( v16 < sx );
				}
				v14 += 10;
			}
			CalcPlrScrolls(p);
		}
		return v6;
	}
	while ( v6 )
	{
		if ( v8 >= 40 )
			v6 = 0;
		v9 = v20;
		if ( v20 < 0 )
			v9 = 0;
		for ( j = 0; j < sx; ++j )
		{
			if ( !v6 )
				break;
			v6 = 0;
			if ( v9 < 10 )
				_LOBYTE(v6) = plr[pnum].InvGrid[v9 + v8] == 0;
			++v9;
		}
		v8 += 10;
		if ( ++v22 >= sy )
		{
			if ( v6 )
				goto LABEL_25;
			break;
		}
	}
	if ( sx <= 1 && sy <= 1 )
	{
		v11 = 0;
		v12 = &plr[p].SpdList[0]._itype;
		while ( *v12 != -1 )
		{
			++v11;
			v12 += 92;
			if ( v11 >= MAXBELTITEMS )
				goto LABEL_24;
		}
		v6 = 1;
		goto LABEL_25;
	}
	v6 = 0;
LABEL_24:
	if ( v6 )
		goto LABEL_25;
	return v6;
}

int __fastcall GoldAutoPlace(int pnum)
{
	int v1; // ebp
	int v2; // edi
	int v3; // ecx
	int *v4; // esi
	int v5; // eax
	int v6; // edi
	int *v7; // esi
	int v8; // eax
	signed int v9; // ebx
	char *v10; // edx
	int v11; // eax
	int v12; // ecx
	int pnuma; // [esp+10h] [ebp-4h]

	pnuma = pnum;
	v1 = pnum;
	v2 = 0;
	v3 = 0;
	if ( plr[v1]._pNumInv <= 0 )
	{
LABEL_14:
		v6 = 0;
		if ( plr[v1]._pNumInv <= 0 )
		{
LABEL_28:
			v9 = 39;
			do
			{
				if ( v3 )
					break;
				v10 = &plr[0].InvGrid[10 * (v9 / 10) + v1 * 21720 + v9 % 10];
				if ( !*v10 )
				{
					v11 = v1 * 21720 + 368 * plr[v1]._pNumInv;
					qmemcpy((char *)plr[0].InvList + v11, &plr[v1].HoldItem, 0x170u);
					++plr[v1]._pNumInv;
					*v10 = plr[v1]._pNumInv;
					v12 = plr[v1].HoldItem._ivalue;
					if ( v12 < 2500 )
					{
						if ( v12 > 1000 )
							*(int *)((char *)&plr[0].InvList[0]._iCurs + v11) = 5;
						else
							*(int *)((char *)&plr[0].InvList[0]._iCurs + v11) = 4;
					}
					else
					{
						*(int *)((char *)&plr[0].InvList[0]._iCurs + v11) = 6;
					}
					plr[v1]._pGold = CalculateGold(pnuma);
					v3 = 1;
				}
				--v9;
			}
			while ( v9 >= 0 );
		}
		else
		{
			v7 = &plr[v1].InvList[0]._ivalue;
			while ( !v3 )
			{
				if ( *(v7 - 47) == 11 && *v7 < 5000 )
				{
					v8 = plr[v1].HoldItem._ivalue + *v7;
					if ( v8 <= 5000 )
					{
						*v7 = v8;
						if ( v8 < 2500 )
						{
							if ( v8 > 1000 )
								*(v7 - 1) = 5;
							else
								*(v7 - 1) = 4;
						}
						else
						{
							*(v7 - 1) = 6;
						}
						plr[v1]._pGold = CalculateGold(pnuma);
						v3 = 1;
					}
				}
				++v6;
				v7 += 92;
				if ( v6 >= plr[v1]._pNumInv )
				{
					if ( v3 )
						return v3;
					goto LABEL_28;
				}
			}
		}
	}
	else
	{
		v4 = &plr[v1].InvList[0]._ivalue;
		while ( !v3 )
		{
			if ( *(v4 - 47) == 11 )
			{
				v5 = *v4 + plr[v1].HoldItem._ivalue;
				if ( v5 <= 5000 )
				{
					*v4 = v5;
					if ( v5 < 2500 )
					{
						if ( v5 > 1000 )
							*(v4 - 1) = 5;
						else
							*(v4 - 1) = 4;
					}
					else
					{
						*(v4 - 1) = 6;
					}
					plr[v1]._pGold = CalculateGold(pnuma);
					v3 = 1;
				}
			}
			++v2;
			v4 += 92;
			if ( v2 >= plr[v1]._pNumInv )
			{
				if ( v3 )
					return v3;
				goto LABEL_14;
			}
		}
	}
	return v3;
}

int __fastcall WeaponAutoPlace(int pnum)
{
	int v1; // edi
	int v2; // eax
	int v3; // ecx
	ItemStruct *v4; // esi
	ItemStruct *v5; // edi
	int result; // eax

	v1 = pnum;
	if ( plr[pnum].HoldItem._iLoc == ILOC_TWOHAND )
	{
		if ( plr[v1].InvBody[4]._itype != -1 || plr[v1].InvBody[5]._itype != -1 )
			return 0;
LABEL_12:
		NetSendCmdChItem(1u, 4u);
		v4 = &plr[v1].HoldItem;
		v5 = &plr[v1].InvBody[4];
		goto LABEL_13;
	}
	v2 = plr[v1].InvBody[4]._itype;
	if ( v2 != -1 && plr[v1].InvBody[4]._iClass == 1 )
		return 0;
	v3 = plr[v1].InvBody[5]._itype;
	if ( v3 != -1 && plr[v1].InvBody[5]._iClass == 1 )
		return 0;
	if ( v2 == -1 )
		goto LABEL_12;
	if ( v3 == -1 && plr[v1].InvBody[4]._iLoc != ILOC_TWOHAND )
	{
		NetSendCmdChItem(1u, 5u);
		v4 = &plr[v1].HoldItem;
		v5 = &plr[v1].InvBody[5];
LABEL_13:
		result = 1;
		qmemcpy(v5, v4, sizeof(ItemStruct));
		return result;
	}
	return 0;
}

int __fastcall SwapItem(ItemStruct *a, ItemStruct *b)
{
	int v2; // eax
	ItemStruct h; // [esp+8h] [ebp-170h]

	qmemcpy(&h, a, sizeof(h));
	v2 = h._iCurs;
	qmemcpy(a, b, sizeof(ItemStruct));
	qmemcpy(b, &h, sizeof(ItemStruct));
	return v2 + 12;
}

void __fastcall CheckInvPaste(int pnum, int mx, int my)
{
	int v3; // ebx
	int v4; // edi
	int v5; // eax
	int v6; // esi
	signed int v7; // edi
	int v8; // edx
	int v9; // edx
	signed int v10; // edi
	char v11; // al
	signed int v12; // ecx
	int v13; // eax
	int v14; // eax
	char *v15; // edi
	int v16; // esi
	int v17; // ecx
	int v18; // edx
	char v19; // al
	int v20; // ecx
	int v21; // esi
	ItemStruct *v22; // edi
	ItemStruct *v23; // ecx
	int v24; // eax
	int v25; // eax
	int v26; // edx
	ItemStruct *v27; // esi
	int v28; // eax
	int v29; // ecx
	int v30; // esi
	int v31; // eax
	int v32; // eax
	int v33; // ecx
	int v34; // eax
	int v35; // ecx
	char *v36; // eax
	int v37; // edx
	int v38; // ecx
	int v39; // edi
	int v40; // esi
	int v41; // ebx
	int v42; // edx
	int v43; // eax
	int v44; // eax
	signed int v45; // ecx
	int v46; // edx
	char *v47; // eax
	int v48; // edi
	int v49; // eax
	int v50; // ecx
	char *v51; // esi
	char v52; // cl
	int v53; // ecx
	int v54; // eax
	int v55; // edi
	int v56; // edx
	int v57; // esi
	int v58; // ebx
	int v59; // eax
	int v60; // esi
	ItemStruct tempitem; // [esp+Ch] [ebp-190h]
	int v62; // [esp+17Ch] [ebp-20h]
	int p; // [esp+180h] [ebp-1Ch]
	int v64; // [esp+184h] [ebp-18h]
	int v65; // [esp+188h] [ebp-14h]
	int v66; // [esp+18Ch] [ebp-10h]
	int v67; // [esp+190h] [ebp-Ch]
	int v68; // [esp+194h] [ebp-8h]
	int v69; // [esp+198h] [ebp-4h]
	int cursor_id; // [esp+1A4h] [ebp+8h]
	int cursor_ida; // [esp+1A4h] [ebp+8h]

	p = pnum;
	v3 = pnum;
	v4 = mx;
	SetICursor(plr[pnum].HoldItem._iCurs + 12);
	v5 = my + (icursH >> 1);
	v6 = v4 + (icursW >> 1);
	v64 = icursW28;
	v7 = 0;
	v67 = icursH28;
	v68 = 0;
	do
	{
		if ( v7 )
			goto LABEL_18;
		v8 = InvRect[v68].X;
		if ( v6 >= v8 && v6 < v8 + 28 )
		{
			v9 = InvRect[v68].Y;
			if ( v5 >= v9 - 29 && v5 < v9 )
			{
				v7 = 1;
				--v68;
			}
		}
		if ( v68 != 24 )
			goto LABEL_13;
		if ( !(v64 & 1) )
			v6 -= 14;
		if ( !(v67 & 1) )
		{
			v5 -= 14;
LABEL_13:
			if ( v68 == 64 && !(v67 & 1) )
				v5 += 14;
		}
		++v68;
	}
	while ( (unsigned int)v68 < 0x49 );
	if ( !v7 )
		return;
LABEL_18:
	v10 = v68;
	v69 = ILOC_UNEQUIPABLE;
	if ( v68 >= 0 && v68 <= ILOC_ARMOR )
		v69 = ILOC_HELM;
	if ( v68 >= ILOC_HELM && v68 <= ILOC_RING )
		v69 = ILOC_RING;
	if ( v68 == ILOC_AMULET )
		v69 = ILOC_AMULET;
	if ( v68 >= ILOC_UNEQUIPABLE && v68 <= 18 )
		v69 = ILOC_ONEHAND;
	if ( v68 >= 19 && v68 <= 24 )
		v69 = ILOC_ARMOR;
	if ( v68 >= 65 && v68 <= 72 )
		v69 = ILOC_BELT;
	v11 = plr[v3].HoldItem._iLoc;
	v12 = 0;
	if ( (char)v11 == v69 )
		v12 = 1;
	if ( v69 == 1 && v11 == ILOC_TWOHAND )
	{
		v69 = ILOC_TWOHAND;
		v12 = 1;
	}
	if ( v11 != 7 || v69 != ILOC_BELT )
	{
LABEL_50:
		if ( v69 != ILOC_UNEQUIPABLE )
			goto LABEL_81;
		v66 = 0;
		cursor_id = 1;
		v13 = (v68 - 25) / 10;
		if ( plr[v3].HoldItem._itype == ITYPE_GOLD )
		{
			_LOBYTE(v13) = plr[0].InvGrid[10 * v13 + v3 * 21720 + (v68 - 25) % 10];
			if ( !(_BYTE)v13 )
				goto LABEL_93;
			v13 = (char)v13;
			if ( (char)v13 <= 0 )
			{
				v13 = -v13;
			}
			else if ( *(int *)((char *)&plr[0].InvBody[v13 + 6]._itype + v3 * 21720) == ITYPE_GOLD )
			{
				goto LABEL_93;
			}
			v66 = v13;
LABEL_93:
			v21 = p;
			if ( p == myplr )
			{
				PlaySFX(ItemInvSnds[ItemCAnimTbl[plr[v3].HoldItem._iCurs]]);
				v10 = v68;
			}
			cursor_ida = 1;
			switch ( v69 )
			{
				case ILOC_ONEHAND:
					if ( v10 > 12 )
					{
						if ( plr[v3].InvBody[5]._itype == ITYPE_NONE )
						{
							v25 = plr[v3].InvBody[4]._itype;
							if ( v25 == ITYPE_NONE )
								goto LABEL_232;
							if ( plr[v3].InvBody[4]._iLoc == ILOC_TWOHAND )
							{
								NetSendCmdDelItem(0, 4u);
								NetSendCmdChItem(0, 5u);
								SwapItem(&plr[v3].InvBody[5], &plr[v3].InvBody[4]);
								v23 = &plr[v3].InvBody[5];
LABEL_99:
								v24 = SwapItem(v23, &plr[v3].HoldItem);
LABEL_172:
								cursor_ida = v24;
								goto LABEL_226;
							}
							if ( v25 == ITYPE_NONE || plr[v3].InvBody[4]._iClass != plr[v3].HoldItem._iClass )
							{
LABEL_232:
								NetSendCmdChItem(0, 5u);
								v22 = &plr[v3].InvBody[5];
LABEL_158:
								qmemcpy(v22, &plr[v3].HoldItem, sizeof(ItemStruct));
								goto LABEL_226;
							}
						}
						else if ( plr[v3].InvBody[4]._itype == ITYPE_NONE
							   || plr[v3].InvBody[4]._iClass != plr[v3].HoldItem._iClass )
						{
							goto LABEL_114;
						}
					}
					else
					{
						if ( plr[v3].InvBody[4]._itype == ITYPE_NONE )
						{
							if ( plr[v3].InvBody[5]._itype != ITYPE_NONE
							  && plr[v3].InvBody[5]._iClass == plr[v3].HoldItem._iClass )
							{
LABEL_114:
								NetSendCmdChItem(0, 5u);
								v23 = &plr[v3].InvBody[5];
								goto LABEL_99;
							}
							NetSendCmdChItem(0, 4u);
							v22 = &plr[v3].InvBody[4];
							goto LABEL_158;
						}
						if ( plr[v3].InvBody[5]._itype != ITYPE_NONE
						  && plr[v3].InvBody[5]._iClass == plr[v3].HoldItem._iClass )
						{
							goto LABEL_114;
						}
					}
					NetSendCmdChItem(0, 4u);
					v23 = &plr[v3].InvBody[4];
					goto LABEL_99;
				case ILOC_TWOHAND:
					NetSendCmdDelItem(0, 5u);
					if ( plr[v3].InvBody[4]._itype == ITYPE_NONE )
						goto LABEL_147;
					v26 = plr[v3].InvBody[5]._itype;
					if ( v26 == -1 )
						goto LABEL_146;
					qmemcpy(&tempitem, &plr[v3].HoldItem, sizeof(tempitem));
					v27 = &plr[v3].InvBody[5];
					if ( v26 != 5 )
						v27 = &plr[v3].InvBody[4];
					v28 = p;
					qmemcpy(&plr[v3].HoldItem, v27, sizeof(plr[v3].HoldItem));
					v29 = plr[v3].HoldItem._iCurs + 12;
					if ( v28 == myplr )
						SetCursor(v29);
					else
						SetICursor(v29);
					v67 = 0;
					v30 = 0;
					do
					{
						if ( v67 )
							break;
						v31 = AutoPlace(p, v30++, icursW28, icursH28, 1);
						v67 = v31;
					}
					while ( v30 < 40 );
					v32 = p;
					qmemcpy(&plr[v3].HoldItem, &tempitem, sizeof(plr[v3].HoldItem));
					v33 = plr[v3].HoldItem._iCurs + 12;
					if ( v32 == myplr )
						SetCursor(v33);
					else
						SetICursor(v33);
					if ( !v67 )
						return;
					if ( plr[v3].InvBody[5]._itype == ITYPE_SHIELD )
						plr[v3].InvBody[5]._itype = ITYPE_NONE;
					else
						plr[v3].InvBody[4]._itype = ITYPE_NONE;
LABEL_146:
					if ( plr[v3].InvBody[4]._itype != ITYPE_NONE )
						goto LABEL_149;
LABEL_147:
					if ( plr[v3].InvBody[5]._itype == ITYPE_NONE )
					{
						NetSendCmdChItem(0, 4u);
						qmemcpy(&plr[v3].InvBody[4], &plr[v3].HoldItem, sizeof(plr[v3].InvBody[4]));
					}
					else
					{
LABEL_149:
						NetSendCmdChItem(0, 4u);
						if ( plr[v3].InvBody[4]._itype == ITYPE_NONE )
							SwapItem(&plr[v3].InvBody[4], &plr[v3].InvBody[5]);
						cursor_ida = SwapItem(&plr[v3].InvBody[4], &plr[v3].HoldItem);
					}
					if ( plr[v3].InvBody[4]._itype == ITYPE_STAFF )
					{
						v34 = plr[v3].InvBody[4]._iSpell;
						if ( v34 )
						{
							if ( plr[v3].InvBody[4]._iCharges > 0 )
							{
								plr[v3]._pRSpell = v34;
								_LOBYTE(plr[v3]._pRSplType) = 3;
								drawpanflag = 255;
							}
						}
					}
					goto LABEL_226;
				case ILOC_ARMOR:
					NetSendCmdChItem(0, 6u);
					if ( plr[v3].InvBody[6]._itype == ITYPE_NONE )
					{
						v22 = &plr[v3].InvBody[6];
						goto LABEL_158;
					}
					v23 = &plr[v3].InvBody[6];
					goto LABEL_99;
				case ILOC_HELM:
					NetSendCmdChItem(0, 0);
					if ( plr[v3].InvBody[0]._itype == ITYPE_NONE )
					{
						v22 = plr[v3].InvBody;
						goto LABEL_158;
					}
					v23 = plr[v3].InvBody;
					goto LABEL_99;
				case ILOC_RING:
					if ( v10 == 4 )
					{
						NetSendCmdChItem(0, 1u);
						if ( plr[v3].InvBody[1]._itype == ITYPE_NONE )
						{
							v22 = &plr[v3].InvBody[1];
							goto LABEL_158;
						}
						v23 = &plr[v3].InvBody[1];
					}
					else
					{
						NetSendCmdChItem(0, 2u);
						if ( plr[v3].InvBody[2]._itype == ITYPE_NONE )
						{
							v22 = &plr[v3].InvBody[2];
							goto LABEL_158;
						}
						v23 = &plr[v3].InvBody[2];
					}
					goto LABEL_99;
				case ILOC_AMULET:
					NetSendCmdChItem(0, 3u);
					if ( plr[v3].InvBody[3]._itype == ITYPE_NONE )
					{
						v22 = &plr[v3].InvBody[3];
						goto LABEL_158;
					}
					v23 = &plr[v3].InvBody[3];
					goto LABEL_99;
				case ILOC_UNEQUIPABLE:
					v35 = plr[v3].HoldItem._itype;
					if ( v35 == 11 )
					{
						if ( !v66 )
						{
							v36 = &plr[0].InvGrid[10 * ((v68 - 25) / 10) + v3 * 21720 + (v68 - 25) % 10];
							if ( *v36 <= 0 )
							{
								v42 = 368 * plr[v3]._pNumInv + v3 * 21720;
								qmemcpy((char *)plr[0].InvList + v42, &plr[v3].HoldItem, 0x170u);
								++plr[v3]._pNumInv;
								*v36 = plr[v3]._pNumInv;
								v43 = plr[v3].HoldItem._ivalue;
								plr[v3]._pGold += v43;
								if ( v43 <= 5000 )
								{
									if ( v43 < 2500 )
									{
										if ( v43 > 1000 )
											*(int *)((char *)&plr[0].InvList[0]._iCurs + v42) = 5;
										else
											*(int *)((char *)&plr[0].InvList[0]._iCurs + v42) = 4;
									}
									else
									{
										*(int *)((char *)&plr[0].InvList[0]._iCurs + v42) = 6;
									}
								}
								goto LABEL_226;
							}
							v37 = plr[v3].HoldItem._ivalue;
							v38 = 368 * (*v36 - 1) + v3 * 21720;
							v39 = *(int *)((char *)&plr[0].InvList[0]._ivalue + v38);
							v40 = v37 + v39;
							if ( v37 + v39 <= 5000 )
							{
								*(int *)((char *)&plr[0].InvList[0]._ivalue + v38) = v40;
								plr[v3]._pGold += plr[v3].HoldItem._ivalue;
								if ( v40 < 2500 )
								{
									if ( v40 > 1000 )
										*(int *)((char *)&plr[0].InvList[0]._iCurs + v38) = 5;
									else
										*(int *)((char *)&plr[0].InvList[0]._iCurs + v38) = 4;
								}
								else
								{
									*(int *)((char *)&plr[0].InvList[0]._iCurs + v38) = 6;
								}
								goto LABEL_226;
							}
							plr[v3]._pGold += 5000 - v39;
							plr[v3].HoldItem._ivalue = v37 - (5000 - v39);
							*(int *)((char *)&plr[0].InvList[0]._ivalue + v38) = 5000;
							*(int *)((char *)&plr[0].InvList[0]._iCurs + v38) = 6;
							v41 = plr[v3].HoldItem._ivalue;
							if ( v41 >= 2500 )
							{
								cursor_ida = 18;
								goto LABEL_226;
							}
							v24 = (v41 > 1000) + 16;
							goto LABEL_172;
						}
					}
					else if ( !v66 )
					{
						qmemcpy((char *)&plr[0].InvList[plr[v3]._pNumInv++] + v3 * 21720, &plr[v3].HoldItem, 0x170u);
						v66 = plr[v3]._pNumInv;
LABEL_191:
						v48 = v67;
						v49 = 10 * ((v68 - 25) / 10 - ((v67 - 1) >> 1));
						if ( v49 < 0 )
							v49 = 0;
						v65 = 0;
						if ( v67 > 0 )
						{
							v69 = (v68 - 25) % 10 - ((v64 - 1) >> 1);
							do
							{
								v50 = v69;
								if ( v69 < 0 )
									v50 = 0;
								v67 = 0;
								if ( v64 > 0 )
								{
									v51 = &plr[v3].InvGrid[v50 + v49];
									do
									{
										if ( v67 || v65 != v48 - 1 )
											v52 = -(char)v66;
										else
											v52 = v66;
										*v51++ = v52;
										++v67;
									}
									while ( v67 < v64 );
								}
								v49 += 10;
								++v65;
							}
							while ( v65 < v48 );
						}
						goto LABEL_226;
					}
					v44 = v66 - 1;
					if ( v35 == 11 )
						plr[v3]._pGold += plr[v3].HoldItem._ivalue;
					cursor_ida = SwapItem((ItemStruct *)((char *)&plr[0].InvList[v44] + v3 * 21720), &plr[v3].HoldItem);
					if ( plr[v3].HoldItem._itype == ITYPE_GOLD )
						plr[v3]._pGold = CalculateGold(v21);
					v45 = 0;
					v46 = -v66;
					do
					{
						v47 = &plr[v3].InvGrid[v45];
						if ( *v47 == v66 )
							*v47 = 0;
						if ( *v47 == v46 )
							*v47 = 0;
						++v45;
					}
					while ( v45 < 40 );
					goto LABEL_191;
				case ILOC_BELT:
					v53 = v3 * 21720 + 368 * (v68 - 65);
					if ( plr[v3].HoldItem._itype != ITYPE_GOLD )
					{
						if ( *(int *)((char *)&plr[0].SpdList[0]._itype + v53) == ITYPE_NONE )
						{
							qmemcpy((char *)plr[0].SpdList + v53, &plr[v3].HoldItem, 0x170u);
						}
						else
						{
							cursor_ida = SwapItem((ItemStruct *)((char *)plr[0].SpdList + v53), &plr[v3].HoldItem);
							if ( plr[v3].HoldItem._itype == ITYPE_GOLD )
								plr[v3]._pGold = CalculateGold(p);
						}
						goto LABEL_225;
					}
					v54 = *(int *)((char *)&plr[0].SpdList[0]._itype + v53);
					if ( v54 != -1 )
					{
						if ( v54 == 11 )
						{
							v55 = *(int *)((char *)&plr[0].SpdList[0]._ivalue + v53);
							v56 = plr[v3].HoldItem._ivalue;
							v57 = v55 + v56;
							if ( v55 + v56 <= 5000 )
							{
								*(int *)((char *)&plr[0].SpdList[0]._ivalue + v53) = v57;
								plr[v3]._pGold += plr[v3].HoldItem._ivalue;
								if ( v57 < 2500 )
								{
									if ( v57 > 1000 )
										*(int *)((char *)&plr[0].SpdList[0]._iCurs + v53) = 5;
									else
										*(int *)((char *)&plr[0].SpdList[0]._iCurs + v53) = 4;
								}
								else
								{
									*(int *)((char *)&plr[0].SpdList[0]._iCurs + v53) = 6;
								}
								goto LABEL_225;
							}
							plr[v3]._pGold += 5000 - v55;
							plr[v3].HoldItem._ivalue = v56 - (5000 - v55);
							*(int *)((char *)&plr[0].SpdList[0]._ivalue + v53) = 5000;
							*(int *)((char *)&plr[0].SpdList[0]._iCurs + v53) = 6;
							v58 = plr[v3].HoldItem._ivalue;
							if ( v58 >= 2500 )
							{
								cursor_ida = 18;
								goto LABEL_225;
							}
							v59 = (v58 > 1000) + 16;
						}
						else
						{
							plr[v3]._pGold += plr[v3].HoldItem._ivalue;
							v59 = SwapItem((ItemStruct *)((char *)plr[0].SpdList + v53), &plr[v3].HoldItem);
						}
						cursor_ida = v59;
						goto LABEL_225;
					}
					qmemcpy((char *)plr[0].SpdList + v53, &plr[v3].HoldItem, 0x170u);
					plr[v3]._pGold += plr[v3].HoldItem._ivalue;
LABEL_225:
					drawsbarflag = 1;
LABEL_226:
					v60 = p;
					CalcPlrInv(p, 1u);
					if ( v60 == myplr )
					{
						if ( cursor_ida == 1 )
							SetCursorPos(MouseX + (cursW >> 1), MouseY + (cursH >> 1));
						SetCursor(cursor_ida);
					}
					return;
				default:
					goto LABEL_226;
			}
		}
		v62 = (v68 - 25) % 10;
		v14 = 10 * (v13 - ((v67 - 1) >> 1));
		if ( v14 < 0 )
			v14 = 0;
		v65 = 0;
		if ( v67 <= 0 )
			goto LABEL_93;
		v15 = &plr[v3].InvGrid[v14];
		while ( 1 )
		{
			if ( cursor_id == CURSOR_NONE )
				return;
			if ( v14 >= 40 )
				cursor_id = 0;
			v16 = v62 - ((v64 - 1) >> 1);
			if ( v16 < 0 )
				v16 = 0;
			v17 = 0;
			if ( v64 > 0 )
				break;
LABEL_79:
			v14 += 10;
			v15 += 10;
			if ( ++v65 >= v67 )
			{
				v12 = cursor_id;
				v10 = v68;
				goto LABEL_81;
			}
		}
		while ( 1 )
		{
			if ( cursor_id == CURSOR_NONE )
				goto LABEL_79;
			if ( v16 >= 10 )
				goto LABEL_233;
			_LOBYTE(v18) = v15[v16];
			if ( (_BYTE)v18 )
			{
				v18 = (char)v18;
				if ( (v18 & 0x80u) != 0 )
					v18 = -v18;
				if ( !v66 )
				{
					v66 = v18;
					goto LABEL_78;
				}
				if ( v66 != v18 )
LABEL_233:
					cursor_id = 0;
			}
LABEL_78:
			++v16;
			if ( ++v17 >= v64 )
				goto LABEL_79;
		}
	}
	if ( v64 == 1 && v67 == 1 )
	{
		v12 = 1;
		if ( !AllItemsList[plr[v3].HoldItem.IDidx].iUsable )
			v12 = 0;
		if ( !plr[v3].HoldItem._iStatFlag )
			v12 = 0;
		if ( plr[v3].HoldItem._itype == ITYPE_GOLD )
		{
			v12 = 0;
			goto LABEL_50;
		}
	}
LABEL_81:
	if ( !v12 )
		return;
	if ( v69 == ILOC_UNEQUIPABLE || v69 == ILOC_BELT || plr[v3].HoldItem._iStatFlag )
		goto LABEL_92;
	v19 = plr[v3]._pClass;
	if ( !v19 )
	{
		v20 = PS_WARR13;
		goto LABEL_89;
	}
	if ( v19 != 1 )
	{
		if ( v19 != 2 )
			return;
		PlaySFX(PS_MAGE13);
		v12 = 0;
		v10 = v68;
LABEL_92:
		if ( !v12 )
			return;
		goto LABEL_93;
	}
	v20 = PS_ROGUE13;
LABEL_89:
	PlaySFX(v20);
}
// 4B8C9C: using guessed type int cursH;
// 4B8CB4: using guessed type int icursH;
// 4B8CBC: using guessed type int icursW;
// 52571C: using guessed type int drawpanflag;

void __fastcall CheckInvSwap(int pnum, BYTE bLoc, int idx, WORD wCI, int seed, BOOL bId)
{
	RecreateItem(MAXITEMS, idx, wCI, seed, 0);

	PlayerStruct *p = &plr[pnum];
	p->HoldItem = item[MAXITEMS];

	if ( bId )
	{
		p->HoldItem._iIdentified = TRUE;
	}

	if ( bLoc < NUM_INVLOC )
	{
		p->InvBody[bLoc] = p->HoldItem;

		if ( bLoc == INVLOC_HAND_LEFT && p->HoldItem._iLoc == ILOC_TWOHAND )
		{
			p->InvBody[INVLOC_HAND_RIGHT]._itype = ITYPE_NONE;
		}
		else if ( bLoc == INVLOC_HAND_RIGHT && p->HoldItem._iLoc == ILOC_TWOHAND )
		{
			p->InvBody[INVLOC_HAND_LEFT]._itype = ITYPE_NONE;
		}
	}

	CalcPlrInv(pnum, TRUE);
}

void __fastcall CheckInvCut(int pnum, int mx, int my)
{
	if ( plr[pnum]._pmode > PM_WALK3 )
	{
		return;
	}

	if ( dropGoldFlag )
	{
		dropGoldFlag = 0;
		dropGoldValue = 0;
	}

	int r;
	BOOL done = FALSE;

	// TODO: this loop is compiled differently (via InvRect pointers)
	for ( r = 0; (DWORD)r < NUM_XY_SLOTS && !done; r++ )
	{
		// check which inventory rectangle the mouse is in, if any
		if ( mx >= InvRect[r].X
			&& mx < InvRect[r].X + (INV_SLOT_SIZE_PX + 1)
			&& my >= InvRect[r].Y - (INV_SLOT_SIZE_PX + 1)
			&& my < InvRect[r].Y )
		{
			done = TRUE;
			r--;
		}
	}

	if ( !done )
	{
		// not on an inventory slot rectangle
		return;
	}

	plr[pnum].HoldItem._itype = ITYPE_NONE;

	if (
		r >= SLOTXY_HEAD_FIRST
		&& r <= SLOTXY_HEAD_LAST
		&& plr[pnum].InvBody[INVLOC_HEAD]._itype != ITYPE_NONE )
	{
		NetSendCmdDelItem(FALSE, INVLOC_HEAD);
		plr[pnum].HoldItem = plr[pnum].InvBody[INVLOC_HEAD];
		plr[pnum].InvBody[INVLOC_HEAD]._itype = ITYPE_NONE;
	}

	if (
		r == SLOTXY_RING_LEFT
		&& plr[pnum].InvBody[INVLOC_RING_LEFT]._itype != ITYPE_NONE )
	{
		NetSendCmdDelItem(FALSE, INVLOC_RING_LEFT);
		plr[pnum].HoldItem = plr[pnum].InvBody[INVLOC_RING_LEFT];
		plr[pnum].InvBody[INVLOC_RING_LEFT]._itype = ITYPE_NONE;
	}

	if (
		r == SLOTXY_RING_RIGHT
		&& plr[pnum].InvBody[INVLOC_RING_RIGHT]._itype != ITYPE_NONE )
	{
		NetSendCmdDelItem(FALSE, INVLOC_RING_RIGHT);
		plr[pnum].HoldItem = plr[pnum].InvBody[INVLOC_RING_RIGHT];
		plr[pnum].InvBody[INVLOC_RING_RIGHT]._itype = ITYPE_NONE;
	}

	if (
		r == SLOTXY_AMULET
		&& plr[pnum].InvBody[INVLOC_AMULET]._itype != ITYPE_NONE )
	{
		NetSendCmdDelItem(FALSE, INVLOC_AMULET);
		plr[pnum].HoldItem = plr[pnum].InvBody[INVLOC_AMULET];
		plr[pnum].InvBody[INVLOC_AMULET]._itype = ITYPE_NONE;
	}

	if (
		r >= SLOTXY_HAND_LEFT_FIRST
		&& r <= SLOTXY_HAND_LEFT_LAST
		&& plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype != ITYPE_NONE )
	{
		NetSendCmdDelItem(FALSE, INVLOC_HAND_LEFT);
		plr[pnum].HoldItem = plr[pnum].InvBody[INVLOC_HAND_LEFT];
		plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype = ITYPE_NONE;
	}

	if (
		r >= SLOTXY_HAND_RIGHT_FIRST
		&& r <= SLOTXY_HAND_RIGHT_LAST
		&& plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype != ITYPE_NONE )
	{
		NetSendCmdDelItem(FALSE, INVLOC_HAND_RIGHT);
		plr[pnum].HoldItem = plr[pnum].InvBody[INVLOC_HAND_RIGHT];
		plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype = ITYPE_NONE;
	}

	if (
		r >= SLOTXY_CHEST_FIRST
		&& r <= SLOTXY_CHEST_LAST
		&& plr[pnum].InvBody[INVLOC_CHEST]._itype != ITYPE_NONE )
	{
		NetSendCmdDelItem(FALSE, INVLOC_CHEST);
		plr[pnum].HoldItem = plr[pnum].InvBody[INVLOC_CHEST];
		plr[pnum].InvBody[INVLOC_CHEST]._itype = ITYPE_NONE;
	}

	if ( r >= SLOTXY_INV_FIRST && r <= SLOTXY_INV_LAST )
	{
		char ii = plr[pnum].InvGrid[r - SLOTXY_INV_FIRST];
		if ( ii )
		{
			int iv = ii;
			if ( ii <= 0 )
			{
				iv = -ii;
			}

			for ( int i = 0; i < NUM_INV_GRID_ELEM; i++ )
			{
				if ( plr[pnum].InvGrid[i] == iv || plr[pnum].InvGrid[i] == -iv )
				{
					plr[pnum].InvGrid[i] = 0;
				}
			}

			iv--;

			plr[pnum].HoldItem = plr[pnum].InvList[iv];
			plr[pnum]._pNumInv--;

			if ( plr[pnum]._pNumInv > 0 && plr[pnum]._pNumInv != iv )
			{
				plr[pnum].InvList[iv] = plr[pnum].InvList[plr[pnum]._pNumInv];

				for ( int j = 0; j < NUM_INV_GRID_ELEM; j++ )
				{
					if ( plr[pnum].InvGrid[j] == plr[pnum]._pNumInv + 1 )
					{
						plr[pnum].InvGrid[j] = iv + 1;
					}
					if ( plr[pnum].InvGrid[j] == -(plr[pnum]._pNumInv + 1) )
					{
						plr[pnum].InvGrid[j] = -iv - 1;
					}
				}
			}
		}
	}

	if ( r >= SLOTXY_BELT_FIRST )
	{
		int offs = r - SLOTXY_BELT_FIRST;
		if ( plr[pnum].SpdList[offs]._itype != ITYPE_NONE )
		{
			plr[pnum].HoldItem = plr[pnum].SpdList[offs];
			plr[pnum].SpdList[offs]._itype = ITYPE_NONE;
			drawsbarflag = 1;
		}
	}


	if ( plr[pnum].HoldItem._itype != ITYPE_NONE )
	{
		if ( plr[pnum].HoldItem._itype == ITYPE_GOLD )
		{
			plr[pnum]._pGold = CalculateGold(pnum);
		}

		CalcPlrInv(pnum, TRUE);
		CheckItemStats(pnum);

		if ( pnum == myplr )
		{
			PlaySFX(IS_IGRAB);
			SetCursor(plr[pnum].HoldItem._iCurs + CURSOR_FIRSTITEM);
			SetCursorPos(mx - (cursW >> 1), MouseY - (cursH >> 1));
		}
	}
}

void __fastcall inv_update_rem_item(int pnum, BYTE iv)
{
	if ( iv < NUM_INVLOC )
	{
		plr[pnum].InvBody[iv]._itype = ITYPE_NONE;
	}

	BOOL Loadgfx = FALSE;

	if ( plr[pnum]._pmode != PM_DEATH )
	{
		Loadgfx = TRUE;
	}

	CalcPlrInv(pnum, Loadgfx);
}

void __fastcall RemoveInvItem(int pnum, int iv)
{
	iv++;

	for ( int i = 0; i < NUM_INV_GRID_ELEM; i++ )
	{
		if ( plr[pnum].InvGrid[i] == iv || plr[pnum].InvGrid[i] == -iv )
		{
			plr[pnum].InvGrid[i] = 0;
		}
	}

	iv--;
	plr[pnum]._pNumInv--;

	if ( plr[pnum]._pNumInv > 0 && plr[pnum]._pNumInv != iv )
	{
		plr[pnum].InvList[iv] = plr[pnum].InvList[plr[pnum]._pNumInv];

		for ( int j = 0; j < NUM_INV_GRID_ELEM; j++ )
		{
			if ( plr[pnum].InvGrid[j] == plr[pnum]._pNumInv + 1 )
			{
				plr[pnum].InvGrid[j] = iv + 1;
			}
			if ( plr[pnum].InvGrid[j] == -(plr[pnum]._pNumInv + 1) )
			{
				plr[pnum].InvGrid[j] = -(iv + 1);
			}
		}
	}

	CalcPlrScrolls(pnum);

	if ( plr[pnum]._pRSplType == RSPLTYPE_SCROLL )
	{
		if ( plr[pnum]._pRSpell != SPL_INVALID )
		{
			// BUGFIX: Cast the literal `1` to `unsigned __int64` to make that bitshift 64bit
			// this causes the last 4 skills to not reset correctly after use
			if ( !(
				plr[pnum]._pScrlSpells64
				& (1 << (plr[pnum]._pRSpell - 1))) )
			{
				plr[pnum]._pRSpell = SPL_INVALID;
			}

			drawpanflag = 255;
		}
	}
}

void __fastcall RemoveSpdBarItem(int pnum, int iv)
{
	plr[pnum].SpdList[iv]._itype = ITYPE_NONE;

	CalcPlrScrolls(pnum);

	if ( plr[pnum]._pRSplType == RSPLTYPE_SCROLL )
	{
		if ( plr[pnum]._pRSpell != SPL_INVALID )
		{
			// BUGFIX: Cast the literal `1` to `unsigned __int64` to make that bitshift 64bit
			// this causes the last 4 skills to not reset correctly after use
			if ( !(
				plr[pnum]._pScrlSpells64
				& (1 << (plr[pnum]._pRSpell - 1))) )
			{
				plr[pnum]._pRSpell = SPL_INVALID;
			}

		}
	}
	drawpanflag = 255;
}

void __cdecl CheckInvItem()
{
	if ( pcurs >= CURSOR_FIRSTITEM )
	{
		CheckInvPaste(myplr, MouseX, MouseY);
	}
	else
	{
		CheckInvCut(myplr, MouseX, MouseY);
	}
}

void __cdecl CheckInvScrn()
{
	if ( MouseX > 190 && MouseX < 437
		&& MouseY > 352 && MouseY < 385 )
	{
		CheckInvItem();
	}
}

void __fastcall CheckItemStats(int pnum)
{
	PlayerStruct *p = &plr[pnum];

	p->HoldItem._iStatFlag = FALSE;

	if ( p->_pStrength >= p->HoldItem._iMinStr
		&& p->_pMagic >= p->HoldItem._iMinMag
		&& p->_pDexterity >= p->HoldItem._iMinDex )
	{
		p->HoldItem._iStatFlag = TRUE;
	}
}

void __fastcall CheckBookLevel(int pnum)
{
	int v1; // ecx
	int v2; // eax
	unsigned char v3; // bl
	int v4; // edi

	v1 = pnum;
	if ( plr[v1].HoldItem._iMiscId == IMISC_BOOK )
	{
		v2 = plr[v1].HoldItem._iSpell;
		v3 = spelldata[plr[v1].HoldItem._iSpell].sMinInt;
		plr[v1].HoldItem._iMinMag = v3;
		v4 = plr[0]._pSplLvl[v2 + v1 * 21720];
		if ( plr[0]._pSplLvl[v2 + v1 * 21720] )
		{
			do
			{
				v3 += 20 * v3 / 100;
				--v4;
				if ( v3 + 20 * v3 / 100 > 255 )
				{
					v3 = -1;
					v4 = 0;
				}
			}
			while ( v4 );
			plr[v1].HoldItem._iMinMag = v3;
		}
	}
}

void __fastcall CheckQuestItem(int pnum)
{
	int v1; // ecx
	int v2; // esi
	char v3; // cl
	char v4; // cl
	char v5; // cl
	char v6; // cl
	char v7; // al

	v1 = pnum;
	v2 = plr[v1].HoldItem.IDidx;
	if ( v2 == IDI_OPTAMULET )
		quests[QTYPE_BLIND]._qactive = 3;
	if ( v2 == IDI_MUSHROOM && quests[QTYPE_BLKM]._qactive == 2 && quests[QTYPE_BLKM]._qvar1 == QS_MUSHSPAWNED )
	{
		v3 = plr[v1]._pClass;
		sfxdelay = IDI_OPTAMULET;
		if ( v3 )
		{
			if ( v3 == 1 )
			{
				sfxdnum = PS_ROGUE95;
			}
			else if ( v3 == 2 )
			{
				sfxdnum = PS_MAGE95;
			}
		}
		else
		{
			sfxdnum = PS_WARR95;
		}
		quests[QTYPE_BLKM]._qvar1 = QS_MUSHPICKED;
	}
	if ( v2 == IDI_ANVIL )
	{
		if ( quests[QTYPE_ANVIL]._qactive == 1 )
		{
			quests[QTYPE_ANVIL]._qactive = 2;
			quests[QTYPE_ANVIL]._qvar1 = 1;
		}
		if ( quests[QTYPE_ANVIL]._qlog == 1 )
		{
			sfxdelay = IDI_OPTAMULET;
			v4 = plr[myplr]._pClass;
			if ( v4 )
			{
				if ( v4 == 1 )
				{
					sfxdnum = PS_ROGUE89;
				}
				else if ( v4 == 2 )
				{
					sfxdnum = PS_MAGE89;
				}
			}
			else
			{
				sfxdnum = PS_WARR89;
			}
		}
	}
	if ( v2 == IDI_GLDNELIX )
	{
		sfxdelay = 30;
		v5 = plr[myplr]._pClass;
		if ( v5 )
		{
			if ( v5 == 1 )
			{
				sfxdnum = PS_ROGUE88;
			}
			else if ( v5 == 2 )
			{
				sfxdnum = PS_MAGE88;
			}
		}
		else
		{
			sfxdnum = PS_WARR88;
		}
	}
	if ( v2 == IDI_ROCK )
	{
		if ( quests[QTYPE_INFRA]._qactive == 1 )
		{
			quests[QTYPE_INFRA]._qactive = 2;
			quests[QTYPE_INFRA]._qvar1 = 1;
		}
		if ( quests[QTYPE_INFRA]._qlog == 1 )
		{
			sfxdelay = IDI_OPTAMULET;
			v6 = plr[myplr]._pClass;
			if ( v6 )
			{
				if ( v6 == 1 )
				{
					sfxdnum = PS_ROGUE87;
				}
				else if ( v6 == 2 )
				{
					sfxdnum = PS_MAGE87;
				}
			}
			else
			{
				sfxdnum = PS_WARR87;
			}
		}
	}
	if ( v2 == IDI_ARMOFVAL )
	{
		quests[QTYPE_BLOOD]._qactive = 3;
		sfxdelay = 20;
		v7 = plr[myplr]._pClass;
		if ( v7 )
		{
			if ( v7 == 1 )
			{
				sfxdnum = PS_ROGUE91;
			}
			else if ( v7 == 2 )
			{
				sfxdnum = PS_MAGE91;
			}
		}
		else
		{
			sfxdnum = PS_WARR91;
		}
	}
}
// 52A554: using guessed type int sfxdelay;

void __fastcall InvGetItem(int pnum, int ii)
{
	int v2; // ebp
	int v3; // edx
	int v4; // ecx
	int v5; // ecx
	int pnuma; // [esp+4h] [ebp-8h]
	int v7; // [esp+8h] [ebp-4h]

	v7 = ii;
	pnuma = pnum;
	if ( dropGoldFlag )
	{
		dropGoldFlag = 0;
		dropGoldValue = 0;
	}
	v2 = ii;
	if ( dItem[item[ii]._ix][item[ii]._iy] )
	{
		if ( myplr == pnum && pcurs >= CURSOR_FIRSTITEM )
			NetSendCmdPItem(1u, CMD_SYNCPUTITEM, plr[myplr].WorldX, plr[myplr].WorldY);
		_HIBYTE(item[v2]._iCreateInfo) &= 0x7Fu;
		qmemcpy(&plr[pnuma].HoldItem, &item[v2], sizeof(plr[pnuma].HoldItem));
		CheckQuestItem(pnuma);
		CheckBookLevel(pnuma);
		CheckItemStats(pnuma);
		v3 = 0;
		dItem[item[v2]._ix][item[v2]._iy] = 0;
		while ( v3 < numitems )
		{
			v4 = itemactive[v3];
			if ( v4 == v7 )
			{
				DeleteItem(v4, v3);
				v3 = 0;
			}
			else
			{
				++v3;
			}
		}
		v5 = plr[pnuma].HoldItem._iCurs;
		pcursitem = -1;
		SetCursor(v5 + 12);
	}
}
// 4B84DC: using guessed type int dropGoldFlag;
// 4B8CC0: using guessed type char pcursitem;

void __fastcall AutoGetItem(int pnum, int ii)
{
	int v2; // ebx
	int v3; // ebp
	int v4; // eax
	int v5; // ecx
	int v6; // edi
	int v7; // edi
	int v8; // edi
	int v9; // edi
	int v10; // edx
	int v11; // ecx
	char v12; // al
	int v13; // ecx
	int iia; // [esp+10h] [ebp-18h]
	signed int iib; // [esp+10h] [ebp-18h]
	signed int iic; // [esp+10h] [ebp-18h]
	signed int iid; // [esp+10h] [ebp-18h]
	signed int iie; // [esp+10h] [ebp-18h]
	signed int iif; // [esp+10h] [ebp-18h]
	signed int iig; // [esp+10h] [ebp-18h]
	signed int iih; // [esp+10h] [ebp-18h]
	signed int iii; // [esp+10h] [ebp-18h]
	signed int iij; // [esp+10h] [ebp-18h]
	ItemStruct *v24; // [esp+14h] [ebp-14h]
	int *v25; // [esp+14h] [ebp-14h]
	int v26; // [esp+18h] [ebp-10h]
	int i; // [esp+1Ch] [ebp-Ch]
	int v28; // [esp+20h] [ebp-8h]
	int v29; // [esp+24h] [ebp-4h]

	v2 = pnum;
	i = ii;
	if ( dropGoldFlag )
	{
		dropGoldFlag = 0;
		dropGoldValue = 0;
	}
	if ( ii == 127 || dItem[item[ii]._ix][item[ii]._iy] )
	{
		v3 = pnum;
		_HIBYTE(item[ii]._iCreateInfo) &= 0x7Fu;
		v28 = ii;
		qmemcpy(&plr[pnum].HoldItem, &item[ii], sizeof(plr[pnum].HoldItem));
		CheckQuestItem(pnum);
		CheckBookLevel(v2);
		CheckItemStats(v2);
		SetICursor(plr[v2].HoldItem._iCurs + 12);
		if ( plr[v2].HoldItem._itype == ITYPE_GOLD )
		{
			v4 = GoldAutoPlace(v2);
		}
		else
		{
			v4 = 0;
			if ( (!(plr[v3]._pgfxnum & 0xF) || (plr[v3]._pgfxnum & 0xF) == 1) && plr[v3]._pmode <= PM_WALK3 )
			{
				if ( plr[v3].HoldItem._iStatFlag )
				{
					if ( plr[v3].HoldItem._iClass == 1 )
					{
						v4 = WeaponAutoPlace(v2);
						if ( v4 )
						{
							CalcPlrInv(v2, 1u);
							goto LABEL_71;
						}
					}
				}
			}
			v5 = icursW28;
			v29 = icursW28;
			v26 = icursH28;
			if ( icursW28 == 1 )
			{
				if ( icursH28 == 1 )
				{
					if ( plr[v3].HoldItem._iStatFlag && AllItemsList[plr[v3].HoldItem.IDidx].iUsable )
					{
						iia = 0;
						v24 = plr[v3].SpdList;
						do
						{
							if ( v4 )
								break;
							if ( v24->_itype == -1 )
							{
								qmemcpy(v24, &plr[v3].HoldItem, sizeof(ItemStruct));
								CalcPlrScrolls(v2);
								v4 = 1;
								drawsbarflag = 1;
							}
							++iia;
							++v24;
						}
						while ( iia < 8 );
					}
					v6 = 30;
					do
					{
						if ( v4 )
							break;
						v4 = AutoPlace(v2, v6++, 1, 1, 1);
					}
					while ( v6 <= 39 );
					v7 = 20;
					do
					{
						if ( v4 )
							break;
						v4 = AutoPlace(v2, v7++, 1, 1, 1);
					}
					while ( v7 <= 29 );
					v8 = 10;
					do
					{
						if ( v4 )
							break;
						v4 = AutoPlace(v2, v8++, 1, 1, 1);
					}
					while ( v8 <= 19 );
					v9 = 0;
					while ( !v4 )
					{
						v4 = AutoPlace(v2, v9++, 1, 1, 1);
						if ( v9 > 9 )
							goto LABEL_35;
					}
					goto LABEL_71;
				}
LABEL_35:
				if ( v26 == 2 )
				{
					iib = 29;
					do
					{
						if ( v4 )
							break;
						v4 = AutoPlace(v2, iib--, 1, 2, 1);
					}
					while ( iib >= 20 );
					iic = 9;
					do
					{
						if ( v4 )
							break;
						v4 = AutoPlace(v2, iic--, 1, 2, 1);
					}
					while ( iic >= 0 );
					iid = 19;
					while ( !v4 )
					{
						v4 = AutoPlace(v2, iid--, 1, 2, 1);
						if ( iid < 10 )
							goto LABEL_45;
					}
					goto LABEL_71;
				}
LABEL_45:
				if ( v26 == 3 )
				{
					iie = 0;
					while ( !v4 )
					{
						v4 = AutoPlace(v2, iie++, 1, 3, 1);
						if ( iie >= 20 )
							goto LABEL_49;
					}
					goto LABEL_71;
				}
			}
			else
			{
LABEL_49:
				if ( v29 == 2 )
				{
					if ( v26 == 2 )
					{
						v25 = AP2x2Tbl;
						do
						{
							if ( v4 )
								break;
							v4 = AutoPlace(v2, *v25, 2, 2, 1);
							++v25;
						}
						while ( (signed int)v25 < (signed int)&AP2x2Tbl[10] );
						iif = 21;
						do
						{
							if ( v4 )
								break;
							v4 = AutoPlace(v2, iif, 2, 2, 1);
							iif += 2;
						}
						while ( iif < 29 );
						iig = 1;
						do
						{
							if ( v4 )
								break;
							v4 = AutoPlace(v2, iig, 2, 2, 1);
							iig += 2;
						}
						while ( iig < 9 );
						iih = 10;
						while ( !v4 )
						{
							v4 = AutoPlace(v2, iih++, 2, 2, 1);
							if ( iih >= 19 )
								goto LABEL_63;
						}
						goto LABEL_71;
					}
LABEL_63:
					if ( v26 == 3 )
					{
						iii = 0;
						do
						{
							if ( v4 )
								break;
							v4 = AutoPlace(v2, iii++, 2, 3, 1);
						}
						while ( iii < 9 );
						iij = 10;
						while ( !v4 )
						{
							v4 = AutoPlace(v2, iij++, 2, 3, 1);
							if ( iij >= 19 )
								goto LABEL_70;
						}
						goto LABEL_71;
					}
				}
			}
		}
LABEL_70:
		if ( v4 )
		{
LABEL_71:
			v10 = 0;
			dItem[item[v28]._ix][item[v28]._iy] = 0;
			while ( v10 < numitems )
			{
				v11 = itemactive[v10];
				if ( v11 == i )
				{
					DeleteItem(v11, v10);
					v10 = 0;
				}
				else
				{
					++v10;
				}
			}
			return;
		}
		if ( v2 == myplr )
		{
			v12 = plr[v3]._pClass;
			switch ( v12 )
			{
				case UI_WARRIOR:
					v13 = random(0, 3) + PS_WARR14;
LABEL_84:
					PlaySFX(v13);
					break;
				case UI_ROGUE:
					v13 = random(0, 3) + PS_ROGUE14;
					goto LABEL_84;
				case UI_SORCERER:
					v13 = random(0, 3) + PS_MAGE14;
					goto LABEL_84;
			}
		}
		qmemcpy(&plr[v3].HoldItem, &item[v28], sizeof(plr[v3].HoldItem));
		RespawnItem(i, 1);
		NetSendCmdPItem(1u, CMD_RESPAWNITEM, item[v28]._ix, item[v28]._iy);
		plr[v3].HoldItem._itype = ITYPE_NONE;
	}
}
// 48E9A8: using guessed type int AP2x2Tbl[10];
// 4B84DC: using guessed type int dropGoldFlag;

int __fastcall FindGetItem(int indx, unsigned short ci, int iseed)
{
	int i; // ebx
	int ii; // esi

	i = 0;
	if ( numitems <= 0 )
		return -1;
	while ( 1 )
	{
		ii = itemactive[i];
		if ( item[ii].IDidx == indx && item[ii]._iSeed == iseed && item[ii]._iCreateInfo == ci )
			break;
		if ( ++i >= numitems )
			return -1;
	}
	return ii;
}

void __fastcall SyncGetItem(int x, int y, int idx, unsigned short ci, int iseed)
{
	char v5; // cl
	int v6; // esi
	int v7; // eax
	int v8; // edx
	int v9; // ecx
	//int v10; // ecx

	v5 = dItem[x][y];
	if ( v5
		&& (v6 = v5 - 1, v7 = v6, item[v7].IDidx == idx)
		&& item[v7]._iSeed == iseed
		&& item[v7]._iCreateInfo == ci )
	{
		FindGetItem(idx, ci, iseed);
	}
	else
	{
		v6 = FindGetItem(idx, ci, iseed);
	}
	if ( v6 != -1 )
	{
		v8 = 0;
		dItem[item[v6]._ix][item[v6]._iy] = 0;
		while ( v8 < numitems )
		{
			v9 = itemactive[v8];
			if ( v9 == v6 )
			{
				DeleteItem(v9, v8);
				FindGetItem(idx, ci, iseed);
				FindGetItem(idx, ci, iseed); /* check idx */
				v8 = 0;
			}
			else
			{
				++v8;
			}
		}
		FindGetItem(idx, ci, iseed);
	}
}

int __fastcall CanPut(int i, int j)
{
	int v2; // ecx
	int v3; // esi
	char v4; // al
	int v5; // eax
	char v6; // al
	bool v7; // sf
	char v8; // al
	char v9; // cl

	v2 = i;
	if ( dItem[v2][j] )
		return 0;
	v3 = v2 * 112 + j;
	if ( nSolidTable[dPiece[0][v3]] )
		return 0;
	v4 = dObject[v2][j];
	if ( v4 )
	{
		v5 = v4 <= 0 ? -1 - v4 : v4 - 1;
		if ( object[v5]._oSolidFlag )
			return 0;
	}
	v6 = dObject[v2 + 1][j + 1];
	v7 = v6 < 0;
	if ( v6 > 0 )
	{
		if ( object[v6 - 1]._oSelFlag ) /* check */
			return 0;
		v7 = v6 < 0;
	}
	if ( v7 && object[-(v6 + 1)]._oSelFlag )
		return 0;
	v8 = dObject[v2 + 1][j];
	if ( v8 > 0 )
	{
		v9 = dObject[v2][j + 1];
		if ( v9 > 0 && object[v8 - 1]._oSelFlag && object[v9 - 1]._oSelFlag )
			return 0;
	}
	if ( !currlevel && (dMonster[0][v3] || dMonster[1][v3 + 1]) )
		return 0;
	return 1;
}

int __cdecl TryInvPut()
{
	int result; // eax
	int v1; // eax
	char v2; // si
	int v3; // edi
	int v4; // ebx
	int v5; // esi

	if ( numitems >= 127 )
		return 0;
	v1 = GetDirection(plr[myplr].WorldX, plr[myplr].WorldY, cursmx, cursmy);
	v2 = v1;
	v3 = plr[myplr].WorldY;
	v4 = plr[myplr].WorldX;
	if ( CanPut(v4 + offset_x[v1], v3 + offset_y[v1])
		|| (v5 = (v2 - 1) & 7, CanPut(v4 + offset_x[v5], v3 + offset_y[v5]))
		|| CanPut(v4 + offset_x[((_BYTE)v5 + 2) & 7], v3 + offset_y[((_BYTE)v5 + 2) & 7]) )
	{
		result = 1;
	}
	else
	{
		result = CanPut(v4, v3);
	}
	return result;
}

void __fastcall DrawInvMsg(char *msg)
{
	char *v1; // esi
	int v2; // eax

	v1 = msg;
	v2 = GetTickCount();
	if ( (unsigned int)(v2 - sgdwLastTime) >= 5000 )
	{
		sgdwLastTime = v2;
		ErrorPlrMsg(v1);
	}
}

int __fastcall InvPutItem(int pnum, int x, int y)
{
	int v3; // edi
	int *v4; // esi
	int v5; // ebx
	int v6; // esi
	int v7; // eax
	int v8; // edi
	int v9; // esi
	int v10; // esi
	int v11; // eax
	int v12; // edx
	int v13; // esi
	int v15; // eax
	int *v16; // edx
	int v17; // edx
	ItemStruct *v18; // [esp+Ch] [ebp-1Ch]
	int v19; // [esp+10h] [ebp-18h]
	signed int v20; // [esp+14h] [ebp-14h]
	int v21; // [esp+18h] [ebp-10h]
	int v22; // [esp+1Ch] [ebp-Ch]
	signed int v23; // [esp+20h] [ebp-8h]
	int xa; // [esp+24h] [ebp-4h]
	int ya; // [esp+30h] [ebp+8h]
	int yb; // [esp+30h] [ebp+8h]
	int yc; // [esp+30h] [ebp+8h]

	xa = x;
	if ( numitems >= 127 )
		return -1;
	v3 = pnum;
	_LOWORD(x) = plr[pnum].HoldItem._iCreateInfo;
	v4 = &plr[pnum].HoldItem._iSeed;
	v18 = &plr[pnum].HoldItem;
	v5 = y;
	if ( FindGetItem(plr[pnum].HoldItem.IDidx, x, plr[pnum].HoldItem._iSeed) != -1 )
	{
		DrawInvMsg("A duplicate item has been detected.  Destroying duplicate...");
		SyncGetItem(xa, y, plr[v3].HoldItem.IDidx, plr[v3].HoldItem._iCreateInfo, *v4);
	}
	ya = GetDirection(plr[v3].WorldX, plr[v3].WorldY, xa, y);
	v6 = v5 - plr[v3].WorldY;
	if ( abs(xa - plr[v3].WorldX) > 1 || abs(v6) > 1 )
	{
		v5 = plr[v3].WorldY + offset_y[ya];
		xa = plr[v3].WorldX + offset_x[ya];
	}
	if ( !CanPut(xa, v5) )
	{
		v7 = plr[v3].WorldX;
		v8 = plr[v3].WorldY;
		v9 = ((_BYTE)ya - 1) & 7;
		v19 = v7;
		v5 = v8 + offset_y[v9];
		xa = v7 + offset_x[v9];
		if ( !CanPut(xa, v8 + offset_y[v9]) )
		{
			v10 = ((_BYTE)v9 + 2) & 7;
			v5 = v8 + offset_y[v10];
			xa = v19 + offset_x[v10];
			if ( !CanPut(xa, v8 + offset_y[v10]) )
			{
				v23 = 0;
				v11 = -1;
				yb = 1;
				v20 = -1;
				while ( !v23 )
				{
					v22 = v11;
					while ( v11 <= yb && !v23 )
					{
						v21 = v20;
						v12 = v8 + v22;
						v13 = v19 + v20;
						do
						{
							if ( v23 )
								break;
							if ( CanPut(v13, v12) )
							{
								v23 = 1;
								xa = v13;
								v5 = v12;
							}
							++v21;
							++v13;
						}
						while ( v21 <= yb );
						v11 = ++v22;
					}
					++yb;
					v11 = v20-- - 1;
					if ( v20 <= -50 )
					{
						if ( v23 )
							break;
						return -1;
					}
				}
			}
		}
	}
	CanPut(xa, v5);
	v15 = itemavail[0];
	dItem[xa][v5] = _LOBYTE(itemavail[0]) + 1;
	yc = v15;
	v16 = &itemavail[MAXITEMS - numitems - 1];
	itemactive[numitems] = v15;
	itemavail[0] = *v16;
	v17 = v15;
	qmemcpy(&item[v15], v18, sizeof(ItemStruct));
	item[v17]._iy = v5;
	item[v17]._ix = xa;
	RespawnItem(v15, 1);
	++numitems;
	SetCursor(CURSOR_HAND);
	return yc;
}

int __fastcall SyncPutItem(int pnum, int x, int y, int idx, int icreateinfo, int iseed, int Id, int dur, int mdur, int ch, int mch, int ivalue, unsigned int ibuff)
{
	int v13; // ebx
	int v14; // edi
	int v15; // esi
	int v17; // edi
	int v18; // ecx
	int v19; // edi
	int v20; // eax
	int v21; // eax
	int v22; // eax
	int v23; // edx
	int v25; // ecx
	int *v26; // edx
	int v27; // eax
	int v28; // eax
	int v29; // [esp+Ch] [ebp-18h]
	int v30; // [esp+Ch] [ebp-18h]
	signed int v31; // [esp+10h] [ebp-14h]
	int v32; // [esp+14h] [ebp-10h]
	int v33; // [esp+18h] [ebp-Ch]
	int o1; // [esp+1Ch] [ebp-8h]
	signed int v35; // [esp+20h] [ebp-4h]
	int i; // [esp+2Ch] [ebp+8h]
	int ia; // [esp+2Ch] [ebp+8h]
	int ib; // [esp+2Ch] [ebp+8h]
	int ic; // [esp+2Ch] [ebp+8h]

	v13 = x;
	v14 = pnum;
	if ( numitems >= 127 )
		return -1;
	v15 = y;
	if ( FindGetItem(idx, icreateinfo, iseed) != -1 )
	{
		DrawInvMsg("A duplicate item has been detected from another player.");
		SyncGetItem(v13, y, idx, icreateinfo, iseed);
	}
	v17 = v14;
	i = GetDirection(plr[v17].WorldX, plr[v17].WorldY, v13, y);
	v29 = v15 - plr[v17].WorldY;
	if ( abs(v13 - plr[v17].WorldX) > 1 || abs(v29) > 1 )
	{
		v13 = plr[v17].WorldX + offset_x[i];
		v15 = plr[v17].WorldY + offset_y[i];
	}
	if ( !CanPut(v13, v15) )
	{
		v18 = plr[v17].WorldX;
		v19 = plr[v17].WorldY;
		v20 = ((_BYTE)i - 1) & 7;
		v30 = v18;
		ia = v20;
		v20 *= 4;
		v13 = v18 + *(int *)((char *)offset_x + v20);
		v15 = v19 + *(int *)((char *)offset_y + v20);
		if ( !CanPut(v18 + *(int *)((char *)offset_x + v20), v19 + *(int *)((char *)offset_y + v20)) )
		{
			v21 = ((_BYTE)ia + 2) & 7;
			v13 = v30 + offset_x[v21];
			v15 = v19 + offset_y[v21];
			if ( !CanPut(v30 + offset_x[v21], v19 + offset_y[v21]) )
			{
				v35 = 0;
				v22 = -1;
				ib = 1;
				v31 = -1;
				while ( !v35 )
				{
					v33 = v22;
					while ( v22 <= ib && !v35 )
					{
						v23 = v19 + v33;
						v32 = v31;
						o1 = v30 + v31;
						do
						{
							if ( v35 )
								break;
							if ( CanPut(o1, v23) )
							{
								v13 = o1;
								v35 = 1;
								v15 = v23;
							}
							++v32;
							++o1;
						}
						while ( v32 <= ib );
						v22 = ++v33;
					}
					++ib;
					v22 = v31-- - 1;
					if ( v31 <= -50 )
					{
						if ( v35 )
							break;
						return -1;
					}
				}
			}
		}
	}
	CanPut(v13, v15);
	v25 = itemavail[0];
	ic = itemavail[0];
	dItem[v13][v15] = _LOBYTE(itemavail[0]) + 1;
	v26 = &itemavail[MAXITEMS - numitems - 1];
	itemactive[numitems] = v25;
	itemavail[0] = *v26;
	if ( idx == IDI_EAR )
	{
		RecreateEar(v25, icreateinfo, iseed, Id, dur, mdur, ch, mch, ivalue, ibuff);
	}
	else
	{
		RecreateItem(v25, idx, icreateinfo, iseed, ivalue);
		if ( Id )
			item[ic]._iIdentified = 1;
		v27 = ic;
		item[v27]._iDurability = dur;
		item[v27]._iMaxDur = mdur;
		item[v27]._iCharges = ch;
		item[v27]._iMaxCharges = mch;
	}
	v28 = ic;
	item[v28]._ix = v13;
	item[v28]._iy = v15;
	RespawnItem(ic, 1);
	++numitems;
	return ic;
}

int __cdecl CheckInvHLight()
{
	signed int v0; // ebx
	int result; // eax
	ItemStruct *v2; // edi
	PlayerStruct *v3; // esi
	int v4; // eax
	int v5; // ebx
	int v6; // edi
	char *v7; // eax
	char v8; // al
	char v9; // [esp+Fh] [ebp-1h]

	v0 = 0;
	do
	{
		result = InvRect[v0].X;
		if ( MouseX >= result )
		{
			result += 29;
			if ( MouseX < result )
			{
				result = InvRect[v0].Y;
				if ( MouseY >= result - 29 && MouseY < result )
					break;
			}
		}
		++v0;
	}
	while ( (unsigned int)v0 < 0x49 );
	if ( (unsigned int)v0 >= 0x49 )
		goto LABEL_37;
	v9 = -1;
	_LOBYTE(infoclr) = 0;
	v2 = 0;
	v3 = &plr[myplr];
	ClearPanel();
	if ( v0 >= 0 && v0 <= 3 )
	{
		v9 = 0;
		v2 = v3->InvBody;
		goto LABEL_36;
	}
	switch ( v0 )
	{
		case 4:
			v9 = 1;
			v2 = &v3->InvBody[1];
			goto LABEL_36;
		case 5:
			v9 = 2;
			v2 = &v3->InvBody[2];
			goto LABEL_36;
		case 6:
			v9 = 3;
			v2 = &v3->InvBody[3];
			goto LABEL_36;
	}
	if ( v0 >= 7 && v0 <= 12 )
	{
		v9 = 4;
		v2 = &v3->InvBody[4];
		goto LABEL_36;
	}
	if ( v0 < 13 || v0 > 18 )
	{
		if ( v0 >= 19 && v0 <= 24 )
		{
			v9 = 6;
			v2 = &v3->InvBody[6];
			goto LABEL_36;
		}
		if ( v0 < 25 || v0 > 64 )
		{
			if ( v0 < 65 )
				goto LABEL_36;
			v5 = v0 - 65;
			drawsbarflag = 1;
			result = 368 * v5;
			v2 = &v3->SpdList[v5];
			if ( v3->SpdList[v5]._itype != -1 )
			{
				v9 = v5 + 47;
				goto LABEL_36;
			}
		}
		else
		{
			result = abs(v3->InvGrid[v0 - 25]); // abs(*((char *)&v3->InvList[39]._iVAdd2 + v0 + 3)); /* find right address */
			if ( result )
			{
				v4 = result - 1;
				v9 = v4 + 7;
				v2 = &v3->InvList[v4];
				goto LABEL_36;
			}
		}
LABEL_37:
		_LOBYTE(result) = -1;
		return result;
	}
	v2 = &v3->InvBody[4];
	if ( v3->InvBody[4]._itype == -1 || v3->InvBody[4]._iLoc != 2 )
	{
		v9 = 5;
		v2 = &v3->InvBody[5];
	}
	else
	{
		v9 = 4;
	}
LABEL_36:
	result = v2->_itype;
	if ( result == ITYPE_NONE )
		goto LABEL_37;
	if ( result == ITYPE_GOLD )
	{
		v6 = v2->_ivalue;
		v7 = get_pieces_str(v6);
		result = sprintf(infostr, "%i gold %s", v6, v7);
	}
	else
	{
		v8 = v2->_iMagical;
		if ( v8 == 1 )
		{
			_LOBYTE(infoclr) = 1;
		}
		else if ( v8 == 2 )
		{
			_LOBYTE(infoclr) = 3;
		}
		strcpy(infostr, v2->_iName);
		if ( v2->_iIdentified )
		{
			strcpy(infostr, v2->_iIName);
			PrintItemDetails(v2);
		}
		else
		{
			PrintItemDur(v2);
		}
	}
	_LOBYTE(result) = v9;
	return result;
}
// 4B883C: using guessed type int infoclr;

void __fastcall RemoveScroll(int pnum)
{
	int v1; // eax
	int v2; // esi
	int v3; // edx
	int *v4; // ecx
	int v5; // edx
	int *v6; // ecx
	int p; // [esp+Ch] [ebp-4h]

	p = pnum;
	v1 = pnum;
	v2 = plr[pnum]._pNumInv;
	v3 = 0;
	if ( v2 <= 0 )
	{
LABEL_8:
		v5 = 0;
		v6 = &plr[v1].SpdList[0]._iMiscId;
		while ( *(v6 - 53) == -1 || *v6 != IMISC_SCROLL && *v6 != IMISC_SCROLLT || v6[1] != plr[v1]._pSpell )
		{
			++v5;
			v6 += 92;
			if ( v5 >= MAXBELTITEMS )
				return;
		}
		RemoveSpdBarItem(p, v5);
	}
	else
	{
		v4 = &plr[v1].InvList[0]._iMiscId;
		while ( *(v4 - 53) == -1 || *v4 != IMISC_SCROLL && *v4 != IMISC_SCROLLT || v4[1] != plr[v1]._pSpell )
		{
			++v3;
			v4 += 92;
			if ( v3 >= v2 )
				goto LABEL_8;
		}
		RemoveInvItem(p, v3);
	}
	CalcPlrScrolls(p);
}

BOOL __cdecl UseScroll()
{
	int v0; // eax
	int v1; // esi
	int v2; // ecx
	int *v3; // edx
	signed int v4; // esi
	int *v5; // ecx

	if ( pcurs != CURSOR_HAND || leveltype == DTYPE_TOWN && !spelldata[plr[myplr]._pRSpell].sTownSpell )
		return FALSE;
	v0 = myplr;
	v1 = 0;
	v2 = plr[myplr]._pNumInv;
	if ( v2 <= 0 )
	{
LABEL_11:
		v4 = 0;
		v5 = &plr[v0].SpdList[0]._iMiscId;
		while ( *(v5 - 53) == -1 || *v5 != IMISC_SCROLL && *v5 != IMISC_SCROLLT || v5[1] != plr[v0]._pRSpell )
		{
			++v4;
			v5 += 92;
			if ( v4 >= MAXBELTITEMS )
				return FALSE;
		}
	}
	else
	{
		v3 = &plr[v0].InvList[0]._iMiscId;
		while ( *(v3 - 53) == -1 || *v3 != IMISC_SCROLL && *v3 != IMISC_SCROLLT || v3[1] != plr[v0]._pRSpell )
		{
			++v1;
			v3 += 92;
			if ( v1 >= v2 )
				goto LABEL_11;
		}
	}
	return TRUE;
}
// 5BB1ED: using guessed type char leveltype;

void __fastcall UseStaffCharge(int pnum)
{
	int v1; // eax
	int *v2; // eax

	v1 = pnum;
	if ( plr[pnum].InvBody[4]._itype != ITYPE_NONE
		&& plr[v1].InvBody[4]._iMiscId == IMISC_STAFF
		&& plr[v1].InvBody[4]._iSpell == plr[v1]._pRSpell )
	{
		v2 = &plr[v1].InvBody[4]._iCharges;
		if ( *v2 > 0 )
		{
			--*v2;
			CalcPlrStaff(pnum);
		}
	}
}

BOOL __cdecl UseStaff()
{
	int v0; // eax
	BOOL result; // al

	result = FALSE;
	if ( pcurs == CURSOR_HAND )
	{
		v0 = myplr;
		if ( plr[myplr].InvBody[4]._itype != ITYPE_NONE
			&& plr[v0].InvBody[4]._iMiscId == IMISC_STAFF
			&& plr[v0].InvBody[4]._iSpell == plr[v0]._pRSpell
			&& plr[v0].InvBody[4]._iCharges > 0 )
		{
			result = TRUE;
		}
	}
	return result;
}

void __cdecl StartGoldDrop()
{
	int v0; // eax

	initialDropGoldIndex = pcursinvitem;
	if ( pcursinvitem > 46 )
		v0 = plr[myplr].InvBody[pcursinvitem]._iMaxDur;
	else
		v0 = plr[myplr].InvBody[pcursinvitem]._ivalue;
	dropGoldValue = 0;
	initialDropGoldValue = v0;
	dropGoldFlag = 1;
	if ( talkflag )
		control_reset_talk();
}
// 4B84DC: using guessed type int dropGoldFlag;
// 4B8960: using guessed type int talkflag;
// 4B8CB8: using guessed type char pcursinvitem;

int __fastcall UseInvItem(int pnum, int cii)
{
	int v2; // esi
	int result; // eax
	int v4; // ebx
	int v5; // ebp
	_DWORD *v6; // edi
	char v7; // al
	int v8; // ecx
	int v9; // eax
	int v10; // ecx
	char v11; // al
	char v12; // al
	int p; // [esp+10h] [ebp-8h]
	signed int v14; // [esp+14h] [ebp-4h]

	v2 = pnum;
	p = pnum;
	if ( plr[pnum]._pInvincible && !plr[v2]._pHitPoints && pnum == myplr )
		return 1;
	result = 1;
	if ( pcurs == 1 && !stextflag )
	{
		if ( cii <= 5 )
			return 0;
		if ( cii > 46 )
		{
			if ( talkflag )
				return result;
			v4 = cii - 47;
			v14 = 1;
			v5 = 368 * (cii - 47) + v2 * 21720;
			v6 = (_DWORD *)((char *)plr[0].SpdList + v5);
		}
		else
		{
			v4 = cii - 7;
			v14 = 0;
			v5 = 368 * (cii - 7) + v2 * 21720;
			v6 = (_DWORD *)((char *)plr[0].InvList + v5);
		}
		if ( v6[90] == 17 )
		{
			v12 = plr[v2]._pClass;
			sfxdelay = 10;
			if ( v12 )
			{
				if ( v12 == 1 )
				{
					sfxdnum = PS_ROGUE95;
				}
				else if ( v12 == 2 )
				{
					sfxdnum = PS_MAGE95;
				}
			}
			else
			{
				sfxdnum = PS_WARR95;
			}
			return 1;
		}
		if ( v6[90] == 19 )
		{
			PlaySFX(IS_IBOOK);
			v11 = plr[v2]._pClass;
			sfxdelay = 10;
			if ( v11 )
			{
				if ( v11 == 1 )
				{
					sfxdnum = PS_ROGUE29;
				}
				else if ( v11 == 2 )
				{
					sfxdnum = PS_MAGE29;
				}
			}
			else
			{
				sfxdnum = PS_WARR29;
			}
			return 1;
		}
		if ( !AllItemsList[v6[90]].iUsable )
			return 0;
		if ( !v6[89] )
		{
			v7 = plr[v2]._pClass;
			if ( v7 )
			{
				if ( v7 == 1 )
				{
					v8 = PS_ROGUE13;
				}
				else
				{
					if ( v7 != 2 )
						return 1;
					v8 = PS_MAGE13;
				}
			}
			else
			{
				v8 = PS_WARR13;
			}
			PlaySFX(v8);
			return 1;
		}
		v9 = v6[55];
		if ( !v9 && v6[2] == 11 )
		{
			StartGoldDrop();
			return 1;
		}
		if ( dropGoldFlag )
		{
			dropGoldFlag = 0;
			dropGoldValue = 0;
		}
		if ( v9 == 21 && !currlevel && !spelldata[v6[56]].sTownSpell
			|| v9 == 22 && !currlevel && !spelldata[v6[56]].sTownSpell )
		{
			return 1;
		}
		if ( v9 == 24 )
		{
			v10 = 65;
		}
		else
		{
			if ( pnum != myplr )
				goto LABEL_39;
			v10 = ItemInvSnds[ItemCAnimTbl[v6[48]]];
		}
		PlaySFX(v10);
LABEL_39:
		UseItem(p, v6[55], v6[56]);
		if ( v14 )
		{
			RemoveSpdBarItem(p, v4);
		}
		else if ( *(int *)((char *)&plr[0].InvList[0]._iMiscId + v5) != IMISC_MAPOFDOOM )
		{
			RemoveInvItem(p, v4);
		}
		return 1;
	}
	return result;
}
// 4B84DC: using guessed type int dropGoldFlag;
// 4B8960: using guessed type int talkflag;
// 52A554: using guessed type int sfxdelay;
// 6AA705: using guessed type char stextflag;

void __cdecl DoTelekinesis()
{
	if ( pcursobj != -1 )
		NetSendCmdParam1(1u, CMD_OPOBJT, pcursobj);
	if ( pcursitem != -1 )
		NetSendCmdGItem(1u, CMD_REQUESTAGITEM, myplr, myplr, pcursitem);
	if ( pcursmonst != -1 && !M_Talker(pcursmonst) && !monster[pcursmonst].mtalkmsg )
		NetSendCmdParam1(1u, CMD_KNOCKBACK, pcursmonst);
	SetCursor(CURSOR_HAND);
}
// 4B8CC0: using guessed type char pcursitem;
// 4B8CC1: using guessed type char pcursobj;

int __fastcall CalculateGold(int pnum)
{
	int result; // eax
	int v2; // ecx
	int *v3; // edx
	signed int v4; // esi
	int v5; // edx
	int *v6; // ecx

	result = 0;
	v2 = pnum;
	v3 = &plr[v2].SpdList[0]._ivalue;
	v4 = MAXBELTITEMS;
	do
	{
		if ( *(v3 - 47) == 11 )
		{
			result += *v3;
			drawpanflag = 255;
		}
		v3 += 92;
		--v4;
	}
	while ( v4 );
	v5 = plr[v2]._pNumInv;
	if ( v5 > 0 )
	{
		v6 = &plr[v2].InvList[0]._ivalue;
		do
		{
			if ( *(v6 - 47) == 11 )
				result += *v6;
			v6 += 92;
			--v5;
		}
		while ( v5 );
	}
	return result;
}
// 52571C: using guessed type int drawpanflag;

int __cdecl DropItemBeforeTrig()
{
	if ( !TryInvPut() )
		return 0;
	NetSendCmdPItem(1u, CMD_PUTITEM, cursmx, cursmy);
	SetCursor(CURSOR_HAND);
	return 1;
}
