//HEADER_GOES_HERE

#include "../types.h"

#ifndef NO_GLOBALS
short automaptype[512];
int AMdword_4B7E40; // weak
int AMdword_4B7E44; // weak
bool automapflag; // idb
char AMbyte_4B7E4C[32];
char automapview[DMAXX][DMAXY];
int AutoMapScale; // idb
int AutoMapXOfs; // weak
int AutoMapYOfs; // weak
int AutoMapPosBits; // weak
int AutoMapXPos; // weak
int AutoMapYPos; // weak
int AMPlayerX; // weak
int AMPlayerY; // weak
#endif

void __cdecl InitAutomapOnce()
{
	automapflag = 0;
	AutoMapScale = 50;
	AutoMapPosBits = 32;
	AutoMapXPos = 16;
	AutoMapYPos = 8;
	AMPlayerX = 4;
	AMPlayerY = 2;
}
// 4B84B8: using guessed type int AutoMapPosBits;
// 4B84BC: using guessed type int AutoMapXPos;
// 4B84C0: using guessed type int AutoMapYPos;
// 4B84C4: using guessed type int AMPlayerX;
// 4B84C8: using guessed type int AMPlayerY;

void __cdecl InitAutomap()
{
	signed int v0; // edi
	signed int v1; // ecx
	int v2; // esi
	char v3; // al
	int v4; // esi
	char v5; // al
	char *v6; // ecx
	unsigned char *v7; // eax
	int v8; // ecx
	unsigned char *v9; // edx
	unsigned int i; // esi
	unsigned char v11; // bl
	_BYTE *v12; // edx
	signed int v13; // ecx
	_BYTE *v14; // eax
	signed int v15; // edx
	int size; // [esp+Ch] [ebp-4h]

	v0 = 50;
	v1 = 0;
	do
	{
		v2 = (v0 << 6) / 100;
		v3 = 2 * (320 / v2);
		v4 = 320 % v2;
		v5 = v3 + 1;
		AMbyte_4B7E4C[v1] = v5;
		if ( v4 )
			AMbyte_4B7E4C[v1] = v5 + 1;
		if ( v4 >= 32 * v0 / 100 )
			++AMbyte_4B7E4C[v1];
		v0 += 5;
		++v1;
	}
	while ( v1 < 31 );
	memset(automaptype, 0, 0x400u);
	switch ( leveltype )
	{
		case DTYPE_CATHEDRAL:
			v6 = "Levels\\L1Data\\L1.AMP";
			break;
		case DTYPE_CATACOMBS:
			v6 = "Levels\\L2Data\\L2.AMP";
			break;
		case DTYPE_CAVES:
			v6 = "Levels\\L3Data\\L3.AMP";
			break;
		case DTYPE_HELL:
			v6 = "Levels\\L4Data\\L4.AMP";
			break;
		default:
			return;
	}
	v7 = LoadFileInMem(v6, &size);
	size = (unsigned int)size >> 1;
	v9 = v7;
	for ( i = 1; i <= size; ++i )
	{
		v11 = *v9;
		v12 = v9 + 1;
		_LOWORD(v0) = v11;
		_LOBYTE(v8) = *v12;
		v9 = v12 + 1;
		_LOWORD(v8) = (unsigned char)v8;
		v8 = v0 + (v8 << 8);
		automaptype[i] = v8;
	}
	mem_free_dbg(v7);
	memset(automapview, 0, 0x640u);
	v13 = 0;
	do
	{
		v14 = (unsigned char *)dFlags + v13;
		v15 = 112;
		do
		{
			*v14 &= 0x7Fu;
			v14 += 112;
			--v15;
		}
		while ( v15 );
		++v13;
	}
	while ( v13 < 112 );
}
// 5BB1ED: using guessed type char leveltype;

void __cdecl StartAutomap()
{
	AutoMapXOfs = 0;
	AutoMapYOfs = 0;
	automapflag = 1;
}
// 4B84B0: using guessed type int AutoMapXOfs;
// 4B84B4: using guessed type int AutoMapYOfs;

void __cdecl AutomapUp()
{
	--AutoMapXOfs;
	--AutoMapYOfs;
}
// 4B84B0: using guessed type int AutoMapXOfs;
// 4B84B4: using guessed type int AutoMapYOfs;

void __cdecl AutomapDown()
{
	++AutoMapXOfs;
	++AutoMapYOfs;
}
// 4B84B0: using guessed type int AutoMapXOfs;
// 4B84B4: using guessed type int AutoMapYOfs;

void __cdecl AutomapLeft()
{
	--AutoMapXOfs;
	++AutoMapYOfs;
}
// 4B84B0: using guessed type int AutoMapXOfs;
// 4B84B4: using guessed type int AutoMapYOfs;

void __cdecl AutomapRight()
{
	++AutoMapXOfs;
	--AutoMapYOfs;
}
// 4B84B0: using guessed type int AutoMapXOfs;
// 4B84B4: using guessed type int AutoMapYOfs;

void __cdecl AutomapZoomIn()
{
	if ( AutoMapScale < 200 )
	{
		AutoMapScale += 5;
		AutoMapPosBits = (AutoMapScale << 6) / 100;
		AutoMapXPos = AutoMapPosBits >> 1;
		AutoMapYPos = AutoMapPosBits >> 2;
		AMPlayerX = AutoMapPosBits >> 3;
		AMPlayerY = AutoMapPosBits >> 4;
	}
}
// 4B84B8: using guessed type int AutoMapPosBits;
// 4B84BC: using guessed type int AutoMapXPos;
// 4B84C0: using guessed type int AutoMapYPos;
// 4B84C4: using guessed type int AMPlayerX;
// 4B84C8: using guessed type int AMPlayerY;

void __cdecl AutomapZoomOut()
{
	if ( AutoMapScale > 50 )
	{
		AutoMapScale -= 5;
		AutoMapPosBits = (AutoMapScale << 6) / 100;
		AutoMapXPos = AutoMapPosBits >> 1;
		AutoMapYPos = AutoMapPosBits >> 2;
		AMPlayerX = AutoMapPosBits >> 3;
		AMPlayerY = AutoMapPosBits >> 4;
	}
}
// 4B84B8: using guessed type int AutoMapPosBits;
// 4B84BC: using guessed type int AutoMapXPos;
// 4B84C0: using guessed type int AutoMapYPos;
// 4B84C4: using guessed type int AMPlayerX;
// 4B84C8: using guessed type int AMPlayerY;

void __cdecl DrawAutomap()
{
	int v0; // eax
	int v1; // ecx
	int v2; // edx
	int v3; // edx
	int v4; // ecx
	int v5; // eax
	int v6; // esi
	int v7; // edx
	int v8; // edx
	int v9; // esi
	int v10; // ebx
	int v11; // edi
	int v12; // esi
	int v13; // edi
	int v14; // esi
	int v15; // ebp
	short v16; // ax
	int v17; // ebp
	short v18; // ax
	int v19; // [esp+0h] [ebp-18h]
	int screen_x; // [esp+4h] [ebp-14h]
	int screen_xa; // [esp+4h] [ebp-14h]
	int v22; // [esp+8h] [ebp-10h]
	int ty; // [esp+Ch] [ebp-Ch]
	int tya; // [esp+Ch] [ebp-Ch]
	int v25; // [esp+10h] [ebp-8h]
	int screen_y; // [esp+14h] [ebp-4h]

	if ( leveltype != DTYPE_TOWN )
	{
		gpBufEnd = (unsigned char *)&gpBuffer->row[352];
		v0 = AutoMapXOfs;
		v1 = (ViewX - 16) >> 1;
		v2 = AutoMapXOfs + v1;
		if ( AutoMapXOfs + v1 < 0 )
		{
			do
			{
				++v0;
				++v2;
			}
			while ( v2 < 0 );
			AutoMapXOfs = v0;
		}
		v3 = v0 + v1;
		if ( v0 + v1 >= 40 )
		{
			do
			{
				--v0;
				--v3;
			}
			while ( v3 >= 40 );
			AutoMapXOfs = v0;
		}
		v4 = v0 + v1;
		AMdword_4B7E40 = v4;
		v5 = AutoMapYOfs;
		v6 = (ViewY - 16) >> 1;
		v7 = AutoMapYOfs + v6;
		if ( AutoMapYOfs + v6 < 0 )
		{
			do
			{
				++v5;
				++v7;
			}
			while ( v7 < 0 );
			AutoMapYOfs = v5;
		}
		v8 = v5 + v6;
		if ( v5 + v6 >= 40 )
		{
			do
			{
				--v5;
				--v8;
			}
			while ( v8 >= 40 );
			AutoMapYOfs = v5;
		}
		v9 = v5 + v6;
		AMdword_4B7E44 = v9;
		v10 = AMbyte_4B7E4C[(AutoMapScale - 50) / 5];
		if ( ScrollInfo._sxoff + ScrollInfo._syoff )
			++v10;
		v22 = v4 - v10;
		v19 = v9 - 1;
		if ( v10 & 1 )
		{
			v11 = 384 - AutoMapPosBits * ((v10 - 1) >> 1);
			v12 = 336 - AutoMapXPos * ((v10 + 1) >> 1);
		}
		else
		{
			v11 = AutoMapXPos - AutoMapPosBits * (v10 >> 1) + 384;
			v12 = 336 - AutoMapXPos * (v10 >> 1) - AutoMapYPos;
		}
		if ( ViewX & 1 )
		{
			v11 -= AutoMapYPos;
			v12 -= AMPlayerX;
		}
		if ( ViewY & 1 )
		{
			v11 += AutoMapYPos;
			v12 -= AMPlayerX;
		}
		v13 = (AutoMapScale * ScrollInfo._sxoff / 100 >> 1) + v11;
		v14 = (AutoMapScale * ScrollInfo._syoff / 100 >> 1) + v12;
		if ( invflag || sbookflag )
			v13 -= 160;
		if ( chrflag || questlog )
			v13 += 160;
		if ( v10 + 1 >= 0 )
		{
			v25 = v10 + 2;
			do
			{
				v15 = 0;
				screen_x = v13;
				if ( v10 > 0 )
				{
					ty = v19;
					do
					{
						v16 = GetAutomapType(v22 + v15, ty, 1);
						if ( v16 )
							DrawAutomapType(screen_x, v14, v16);
						screen_x += AutoMapPosBits;
						++v15;
						--ty;
					}
					while ( v15 < v10 );
				}
				++v19;
				screen_xa = 0;
				v17 = v13 - AutoMapXPos;
				screen_y = v14 + AutoMapYPos;
				if ( v10 >= 0 )
				{
					tya = v19;
					do
					{
						v18 = GetAutomapType(v22 + screen_xa, tya, 1);
						if ( v18 )
							DrawAutomapType(v17, screen_y, v18);
						v17 += AutoMapPosBits;
						++screen_xa;
						--tya;
					}
					while ( screen_xa <= v10 );
				}
				++v22;
				v14 += AutoMapXPos;
				--v25;
			}
			while ( v25 );
		}
		DrawAutomapPlr();
		DrawAutomapGame();
	}
	else
	{
		DrawAutomapGame();
	}
}
// 4B7E40: using guessed type int AMdword_4B7E40;
// 4B7E44: using guessed type int AMdword_4B7E44;
// 4B84B0: using guessed type int AutoMapXOfs;
// 4B84B4: using guessed type int AutoMapYOfs;
// 4B84B8: using guessed type int AutoMapPosBits;
// 4B84BC: using guessed type int AutoMapXPos;
// 4B84C0: using guessed type int AutoMapYPos;
// 4B84C4: using guessed type int AMPlayerX;
// 4B8968: using guessed type int sbookflag;
// 5BB1ED: using guessed type char leveltype;
// 69BD04: using guessed type int questlog;
// 69CF0C: using guessed type int gpBufEnd;

void __fastcall DrawAutomapType(int screen_x, int screen_y, short automap_type)
{
	short v3; // al
	int v4; // ebx
	int v5; // edi
	int a3; // ST2C_4
	int a1; // ST28_4
	int a2; // ST24_4
	int v9; // edx
	int v10; // ST28_4
	int v11; // ST2C_4
	int v12; // ST24_4
	int v13; // ST2C_4
	int v14; // ST28_4
	int v15; // ST24_4
	int v16; // ST28_4
	int v17; // ST24_4
	int v18; // ST2C_4
	int v19; // ST2C_4
	int v20; // ST28_4
	int v21; // ST24_4
	int v22; // ST28_4
	int v23; // ST2C_4
	int v24; // ST24_4
	int v25; // ST28_4
	int v26; // ST24_4
	int v27; // ST2C_4
	int v28; // [esp-Ch] [ebp-34h]
	int v29; // [esp-8h] [ebp-30h]
	signed int v30; // [esp+Ch] [ebp-1Ch]
	signed int v31; // [esp+10h] [ebp-18h]
	signed int v32; // [esp+14h] [ebp-14h]
	char v33; // [esp+27h] [ebp-1h]
	int automap_typea; // [esp+30h] [ebp+8h]
	int automap_typeb; // [esp+30h] [ebp+8h]
	int automap_typec; // [esp+30h] [ebp+8h]
	int automap_typed; // [esp+30h] [ebp+8h]
	int automap_typee; // [esp+30h] [ebp+8h]
	int automap_typef; // [esp+30h] [ebp+8h]
	int automap_typeg; // [esp+30h] [ebp+8h]

	v3 = automap_type;
	v4 = screen_x;
	v5 = screen_y;
	v33 = _HIBYTE(automap_type);
	if ( automap_type & 0x4000 )
	{
		ENG_set_pixel(screen_x, screen_y, 200);
		ENG_set_pixel(v4 - AMPlayerX, v5 - AMPlayerY, 200);
		ENG_set_pixel(v4 - AMPlayerX, AMPlayerY + v5, 200);
		ENG_set_pixel(AMPlayerX + v4, v5 - AMPlayerY, 200);
		ENG_set_pixel(AMPlayerX + v4, AMPlayerY + v5, 200);
		ENG_set_pixel(v4 - AutoMapYPos, v5, 200);
		ENG_set_pixel(AutoMapYPos + v4, v5, 200);
		ENG_set_pixel(v4, v5 - AMPlayerX, 200);
		ENG_set_pixel(v4, AMPlayerX + v5, 200);
		ENG_set_pixel(v4 + AMPlayerX - AutoMapXPos, AMPlayerY + v5, 200);
		ENG_set_pixel(v4 + AutoMapXPos - AMPlayerX, AMPlayerY + v5, 200);
		ENG_set_pixel(v4 - AutoMapYPos, AMPlayerX + v5, 200);
		ENG_set_pixel(AutoMapYPos + v4, AMPlayerX + v5, 200);
		ENG_set_pixel(v4 - AMPlayerX, v5 + AutoMapYPos - AMPlayerY, 200);
		ENG_set_pixel(AMPlayerX + v4, v5 + AutoMapYPos - AMPlayerY, 200);
		ENG_set_pixel(v4, AutoMapYPos + v5, 200);
		v3 = automap_type;
	}
	if ( automap_type < 0 )
	{
		DrawLine(v4 - AMPlayerX, v5 - AMPlayerX - AMPlayerY, v4 + AMPlayerX + AutoMapYPos, AMPlayerY + v5, 144);
		DrawLine(v4 - AutoMapYPos, v5 - AMPlayerX, AutoMapYPos + v4, AMPlayerX + v5, 144);
		DrawLine(v4 - AutoMapYPos - AMPlayerX, v5 - AMPlayerY, AMPlayerX + v4, v5 + AMPlayerX + AMPlayerY, 144);
		DrawLine(v4 - AutoMapXPos, v5, v4, v5 + AutoMapYPos, 144);
		v3 = automap_type;
	}
	v31 = 0;
	v30 = 0;
	v32 = 0;
	switch ( v3 & 0xF )
	{
		case 1:
			a3 = v4 - AutoMapYPos + AutoMapXPos;
			a1 = v4 - AutoMapYPos;
			a2 = v5 - AutoMapYPos;
			automap_typea = v5 - AMPlayerX;
			DrawLine(v4, v5 - AutoMapYPos, v4 - AutoMapYPos, v5 - AMPlayerX, 200);
			DrawLine(v4, a2, a3, automap_typea, 200);
			DrawLine(v4, v5, a1, automap_typea, 200);
			v9 = v5;
			v29 = automap_typea;
			v28 = a3;
			goto LABEL_36;
		case 2:
		case 5:
			goto LABEL_8;
		case 3:
		case 6:
			goto LABEL_17;
		case 4:
			v31 = 1;
			goto LABEL_8;
		case 7:
			goto LABEL_25;
		case 8:
			v30 = 1;
LABEL_8:
			if ( automap_type & 0x100 )
			{
				v10 = v4 - AutoMapXPos;
				v11 = v4 - AutoMapYPos;
				v12 = v5 - AutoMapYPos;
				automap_typeb = v5 - AMPlayerX;
				DrawLine(v4, v5 - AutoMapYPos, v4 - AMPlayerX, v5 - AutoMapYPos + AMPlayerY, 200);
				DrawLine(v10, v5, v10 + AMPlayerX, v5 - AMPlayerY, 200);
				DrawLine(v11, v12, v10, automap_typeb, 144);
				DrawLine(v11, v12, v4, automap_typeb, 144);
				DrawLine(v11, v5, v10, automap_typeb, 144);
				DrawLine(v11, v5, v4, automap_typeb, 144);
			}
			if ( v33 & 0x10 )
			{
				DrawLine(v4 - AutoMapYPos, v5 - AMPlayerX, v4 - AutoMapXPos, v5, 200);
				v33 |= 4u;
			}
			if ( v33 & 4 )
			{
				v13 = v4 - AutoMapYPos + AutoMapXPos;
				v14 = v4 - AutoMapYPos;
				v15 = v5 - AutoMapYPos;
				automap_typec = v5 - AMPlayerX;
				DrawLine(v4, v5 - AutoMapYPos, v4 - AutoMapYPos, v5 - AMPlayerX, 200);
				DrawLine(v4, v15, v13, automap_typec, 200);
				DrawLine(v4, v5, v14, automap_typec, 200);
				DrawLine(v4, v5, v13, automap_typec, 200);
			}
			if ( !(v33 & 0x15) )
				DrawLine(v4, v5 - AutoMapYPos, v4 - AutoMapXPos, v5, 200);
			if ( v31 )
				goto LABEL_17;
			goto LABEL_25;
		case 9:
			v32 = 1;
LABEL_17:
			if ( v33 & 2 )
			{
				v16 = AutoMapYPos + v4;
				v17 = v5 - AutoMapYPos;
				v18 = v4 + AutoMapXPos;
				automap_typed = v5 - AMPlayerX;
				DrawLine(v4, v5 - AutoMapYPos, v4 + AMPlayerX, v5 - AutoMapYPos + AMPlayerY, 200);
				DrawLine(v18, v5, v18 - AMPlayerX, v5 - AMPlayerY, 200);
				DrawLine(v16, v17, v4, automap_typed, 144);
				DrawLine(v16, v17, v18, automap_typed, 144);
				DrawLine(v16, v5, v4, automap_typed, 144);
				DrawLine(v16, v5, v18, automap_typed, 144);
			}
			if ( v33 & 0x20 )
			{
				DrawLine(AutoMapYPos + v4, v5 - AMPlayerX, v4 + AutoMapXPos, v5, 200);
				v33 |= 8u;
			}
			if ( v33 & 8 )
			{
				v19 = v4 - AutoMapYPos + AutoMapXPos;
				v20 = v4 - AutoMapYPos;
				v21 = v5 - AutoMapYPos;
				automap_typee = v5 - AMPlayerX;
				DrawLine(v4, v5 - AutoMapYPos, v4 - AutoMapYPos, v5 - AMPlayerX, 200);
				DrawLine(v4, v21, v19, automap_typee, 200);
				DrawLine(v4, v5, v20, automap_typee, 200);
				DrawLine(v4, v5, v19, automap_typee, 200);
			}
			if ( !(v33 & 0x2A) )
				DrawLine(v4, v5 - AutoMapYPos, v4 + AutoMapXPos, v5, 200);
LABEL_25:
			if ( v30 )
				goto LABEL_26;
			goto LABEL_32;
		case 0xA:
			goto LABEL_26;
		case 0xB:
			goto LABEL_33;
		case 0xC:
			v32 = 1;
LABEL_26:
			if ( v33 & 1 )
			{
				v22 = v4 - AutoMapXPos;
				v23 = v4 - AutoMapYPos;
				v24 = AutoMapYPos + v5;
				automap_typef = AMPlayerX + v5;
				DrawLine(v4, AutoMapYPos + v5, v4 - AMPlayerX, AutoMapYPos + v5 - AMPlayerY, 200);
				DrawLine(v22, v5, v22 + AMPlayerX, v5 + AMPlayerY, 200);
				DrawLine(v23, v24, v22, automap_typef, 144);
				DrawLine(v23, v24, v4, automap_typef, 144);
				DrawLine(v23, v5, v22, automap_typef, 144);
				DrawLine(v23, v5, v4, automap_typef, 144);
			}
			else
			{
				DrawLine(v4, AutoMapYPos + v5, v4 - AutoMapXPos, v5, 200);
			}
LABEL_32:
			if ( v32 )
			{
LABEL_33:
				if ( v33 & 2 )
				{
					v25 = AutoMapYPos + v4;
					v26 = AutoMapYPos + v5;
					v27 = v4 + AutoMapXPos;
					automap_typeg = AMPlayerX + v5;
					DrawLine(v4, AutoMapYPos + v5, v4 + AMPlayerX, AutoMapYPos + v5 - AMPlayerY, 200);
					DrawLine(v27, v5, v27 - AMPlayerX, v5 + AMPlayerY, 200);
					DrawLine(v25, v26, v4, automap_typeg, 144);
					DrawLine(v25, v26, v27, automap_typeg, 144);
					DrawLine(v25, v5, v4, automap_typeg, 144);
					DrawLine(v25, v5, v27, automap_typeg, 144);
				}
				else
				{
					v29 = v5;
					v28 = v4 + AutoMapXPos;
					v9 = AutoMapYPos + v5;
LABEL_36:
					DrawLine(v4, v9, v28, v29, 200);
				}
			}
			break;
		default:
			return;
	}
}
// 4B84BC: using guessed type int AutoMapXPos;
// 4B84C0: using guessed type int AutoMapYPos;
// 4B84C4: using guessed type int AMPlayerX;
// 4B84C8: using guessed type int AMPlayerY;

void __cdecl DrawAutomapPlr()
{
	int v0; // ebx
	int v1; // eax
	int v2; // ecx
	int v3; // esi
	int v4; // edi
	int v5; // edx
	int v6; // ecx
	int v7; // eax
	int v8; // ecx
	int v9; // [esp-Ch] [ebp-20h]
	int v10; // [esp-8h] [ebp-1Ch]
	int v11; // [esp+Ch] [ebp-8h]
	int v12; // [esp+10h] [ebp-4h]

	v0 = myplr;
	if ( plr[myplr]._pmode == PM_WALK3 )
	{
		v1 = plr[v0]._px;
		v2 = plr[v0]._py;
		if ( plr[v0]._pdir == 2 )
			++v1;
		else
			++v2;
	}
	else
	{
		v1 = plr[v0].WorldX;
		v2 = plr[v0].WorldY;
	}
	v11 = v1 - 2 * AutoMapXOfs - ViewX;
	v12 = v2 - 2 * AutoMapYOfs - ViewY;
	v3 = (AutoMapScale * ScrollInfo._sxoff / 100 >> 1)
	   + (AutoMapScale * plr[v0]._pxoff / 100 >> 1)
	   + AutoMapYPos * (v11 - v12)
	   + 384;
	if ( invflag || sbookflag )
		v3 = (AutoMapScale * ScrollInfo._sxoff / 100 >> 1)
		   + (AutoMapScale * plr[v0]._pxoff / 100 >> 1)
		   + AutoMapYPos * (v11 - v12)
		   + 224;
	if ( chrflag || questlog )
		v3 += 160;
	v4 = AMPlayerX * (v12 + v11)
	   + (AutoMapScale * ScrollInfo._syoff / 100 >> 1)
	   + (AutoMapScale * plr[v0]._pyoff / 100 >> 1)
	   + 336
	   - AMPlayerX;
	switch ( plr[v0]._pdir )
	{
		case DIR_S:
			DrawLine(v3, v4, v3, v4 + AutoMapYPos, 153);
			DrawLine(v3, AutoMapYPos + v4, v3 + AMPlayerY, v4 + AMPlayerX, 153);
			v10 = v4 + AMPlayerX;
			v9 = v3 - AMPlayerY;
			v5 = AutoMapYPos + v4;
			goto LABEL_19;
		case DIR_SW:
			DrawLine(
				v3,
				AMPlayerX * (v12 + v11)
			  + (AutoMapScale * ScrollInfo._syoff / 100 >> 1)
			  + (AutoMapScale * plr[v0]._pyoff / 100 >> 1)
			  + 336
			  - AMPlayerX,
				v3 - AutoMapYPos,
				AMPlayerX * (v12 + v11)
			  + (AutoMapScale * ScrollInfo._syoff / 100 >> 1)
			  + (AutoMapScale * plr[v0]._pyoff / 100 >> 1)
			  + 336,
				153);
			DrawLine(v3 - AutoMapYPos, AMPlayerX + v4, v3 - AMPlayerY - AMPlayerX, v4, 153);
			v7 = AMPlayerX;
			v8 = v3;
			v5 = AMPlayerX + v4;
			v10 = AMPlayerX + v4;
			goto LABEL_23;
		case DIR_W:
			DrawLine(v3, v4, v3 - AutoMapYPos, v4, 153);
			DrawLine(v3 - AutoMapYPos, v4, v3 - AMPlayerX, v4 - AMPlayerY, 153);
			v5 = v4;
			v10 = v4 + AMPlayerY;
			v9 = v3 - AMPlayerX;
			goto LABEL_24;
		case DIR_NW:
			DrawLine(v3, v4, v3 - AutoMapYPos, v4 - AMPlayerX, 153);
			DrawLine(v3 - AutoMapYPos, v4 - AMPlayerX, v3 - AMPlayerX, v4 - AMPlayerX, 153);
			v7 = AMPlayerX;
			v8 = v3 - AMPlayerY;
			v10 = v4;
			v5 = v4 - AMPlayerX;
LABEL_23:
			v9 = v8 - v7;
LABEL_24:
			v6 = v3 - AutoMapYPos;
			goto LABEL_25;
		case DIR_N:
			DrawLine(v3, v4, v3, v4 - AutoMapYPos, 153);
			DrawLine(v3, v4 - AutoMapYPos, v3 - AMPlayerY, v4 - AMPlayerX, 153);
			v10 = v4 - AMPlayerX;
			v5 = v4 - AutoMapYPos;
			v9 = v3 + AMPlayerY;
LABEL_19:
			v6 = v3;
			goto LABEL_25;
		case DIR_NE:
			DrawLine(v3, v4, v3 + AutoMapYPos, v4 - AMPlayerX, 153);
			DrawLine(AutoMapYPos + v3, v4 - AMPlayerX, v3 + AMPlayerX, v4 - AMPlayerX, 153);
			v10 = v4;
			v9 = v3 + AMPlayerX + AMPlayerY;
			v5 = v4 - AMPlayerX;
			goto LABEL_17;
		case DIR_E:
			DrawLine(v3, v4, v3 + AutoMapYPos, v4, 153);
			DrawLine(AutoMapYPos + v3, v4, v3 + AMPlayerX, v4 - AMPlayerY, 153);
			DrawLine(AutoMapYPos + v3, v4, v3 + AMPlayerX, v4 + AMPlayerY, 153);
			break;
		case DIR_SE:
			DrawLine(
				v3,
				AMPlayerX * (v12 + v11)
			  + (AutoMapScale * ScrollInfo._syoff / 100 >> 1)
			  + (AutoMapScale * plr[v0]._pyoff / 100 >> 1)
			  + 336
			  - AMPlayerX,
				v3 + AutoMapYPos,
				AMPlayerX * (v12 + v11)
			  + (AutoMapScale * ScrollInfo._syoff / 100 >> 1)
			  + (AutoMapScale * plr[v0]._pyoff / 100 >> 1)
			  + 336,
				153);
			DrawLine(AutoMapYPos + v3, AMPlayerX + v4, v3 + AMPlayerX + AMPlayerY, v4, 153);
			v5 = AMPlayerX + v4;
			v10 = AMPlayerX + v4;
			v9 = v3 + AMPlayerX;
LABEL_17:
			v6 = AutoMapYPos + v3;
LABEL_25:
			DrawLine(v6, v5, v9, v10, 153);
			break;
		default:
			return;
	}
}
// 4B84B0: using guessed type int AutoMapXOfs;
// 4B84B4: using guessed type int AutoMapYOfs;
// 4B84C0: using guessed type int AutoMapYPos;
// 4B84C4: using guessed type int AMPlayerX;
// 4B84C8: using guessed type int AMPlayerY;
// 4B8968: using guessed type int sbookflag;
// 69BD04: using guessed type int questlog;

short __fastcall GetAutomapType(int x, int y, BOOL view)
{
	int v3; // edi
	int v4; // esi
	int v6; // eax
	short v7; // bp

	v3 = y;
	v4 = x;
	if ( view )
	{
		if ( x == -1 && y >= 0 && y < 40 && automapview[0][y] )
		{
			x = 0;
			return ~GetAutomapType(x, y, 0) & 0x4000;
		}
		if ( y == -1 )
		{
			if ( x < 0 )
				return 0;
			if ( x < 40 && automapview[x][0] )
			{
				y = 0;
				return ~GetAutomapType(x, y, 0) & 0x4000;
			}
		}
	}
	if ( x < 0 )
		return 0;
	if ( x >= 40 )
		return 0;
	if ( y < 0 )
		return 0;
	if ( y >= 40 )
		return 0;
	v6 = y + 40 * x;
	if ( !automapview[0][v6] && view )
		return 0;
	v7 = automaptype[(unsigned char)dungeon[0][v6]];
	if ( v7 == 7 && ((unsigned short)GetAutomapType(x - 1, y, 0) >> 8) & 8 )
	{
		if ( ((unsigned short)GetAutomapType(v4, v3 - 1, 0) >> 8) & 4 )
			v7 = 1;
	}
	return v7;
}

void __cdecl DrawAutomapGame()
{
	int v0; // esi
	char *v1; // eax
	char *v2; // eax
	char v3[256]; // [esp+4h] [ebp-100h]

	v0 = 20;
	if ( (unsigned char)gbMaxPlayers > 1u )
	{
		v1 = strcpy(v3, "game: ");
		strcat(v1, szPlayerName);
		PrintGameStr(8, 20, v3, 3);
		v0 = 35;
		if ( szPlayerDescript[0] )
		{
			v2 = strcpy(v3, "password: ");
			strcat(v2, szPlayerDescript);
			PrintGameStr(8, 35, v3, 3);
			v0 = 50;
		}
	}
	if ( setlevel )
	{
		PrintGameStr(8, v0, quest_level_names[(unsigned char)setlvlnum], 3);
	}
	else if ( currlevel )
	{
		sprintf(v3, "Level: %i", currlevel);
		PrintGameStr(8, v0, v3, 3);
	}
}
// 5CCB10: using guessed type char setlvlnum;
// 5CF31D: using guessed type char setlevel;
// 679660: using guessed type char gbMaxPlayers;

void __fastcall SetAutomapView(int x, int y)
{
	int xx = (x - 16) >> 1;
	int yy = (y - 16) >> 1;

	if ( xx < 0 || xx >= DMAXX || yy < 0 || yy >= DMAXY ) {
		return;
	}

	automapview[xx][yy] = 1;

	USHORT maptype = GetAutomapType(xx, yy, FALSE);
	USHORT solid = maptype & 0x4000;

	switch ( maptype & 0xF ) {
		case 2:
			if ( solid ) {
				if ( GetAutomapType(xx, yy + 1, FALSE) == 0x4007 )
					automapview[xx][yy + 1] = 1;
			}
			else if ( GetAutomapType(xx - 1, yy, FALSE) & 0x4000 )
				automapview[xx - 1][yy] = 1;
			break;
		case 3:
			if ( solid ) {
				if ( GetAutomapType(xx + 1, yy, FALSE) == 0x4007 )
					automapview[xx + 1][yy] = 1;
			}
			else if ( GetAutomapType(xx, yy - 1, FALSE) & 0x4000 )
				automapview[xx][yy - 1] = 1;
			break;
		case 4:
			if ( solid ) {
				if ( GetAutomapType(xx, yy + 1, FALSE) == 0x4007 )
					automapview[xx][yy + 1] = 1;
				if ( GetAutomapType(xx + 1, yy, FALSE) == 0x4007 )
					automapview[xx + 1][yy] = 1;
			}
			else {
				if ( GetAutomapType(xx - 1, yy, FALSE) & 0x4000 )
					automapview[xx - 1][yy] = 1;
				if ( GetAutomapType(xx, yy - 1, FALSE) & 0x4000 )
					automapview[xx][yy - 1] = 1;
				if ( GetAutomapType(xx - 1, yy - 1, FALSE) & 0x4000 )
					automapview[xx - 1][yy - 1] = 1;
			}
			break;
		case 5:
			if ( solid ) {
				if ( GetAutomapType(xx, yy - 1, FALSE) & 0x4000 )
					automapview[xx][yy - 1] = 1;
				if ( GetAutomapType(xx, yy + 1, FALSE) == 0x4007 )
					automapview[xx][yy + 1] = 1;
			}
			else if ( GetAutomapType(xx - 1, yy, FALSE) & 0x4000 )
				automapview[xx - 1][yy] = 1;
			break;
		case 6:
			if ( solid ) {
				if ( GetAutomapType(xx - 1, yy, FALSE) & 0x4000 )
					automapview[xx - 1][yy] = 1;
				if ( GetAutomapType(xx + 1, yy, FALSE) == 0x4007 )
					automapview[xx + 1][yy] = 1;
			}
			else if ( GetAutomapType(xx, yy - 1, FALSE) & 0x4000 )
				automapview[xx][yy - 1] = 1;
	}
}

void __cdecl AutomapZoomReset()
{
	AutoMapXOfs = 0;
	AutoMapYOfs = 0;
	AutoMapPosBits = (AutoMapScale << 6) / 100;
	AutoMapXPos = AutoMapPosBits >> 1;
	AutoMapYPos = AutoMapPosBits >> 2;
	AMPlayerX = AutoMapPosBits >> 3;
	AMPlayerY = AutoMapPosBits >> 4;
}
// 4B84B0: using guessed type int AutoMapXOfs;
// 4B84B4: using guessed type int AutoMapYOfs;
// 4B84B8: using guessed type int AutoMapPosBits;
// 4B84BC: using guessed type int AutoMapXPos;
// 4B84C0: using guessed type int AutoMapYPos;
// 4B84C4: using guessed type int AMPlayerX;
// 4B84C8: using guessed type int AMPlayerY;
