//HEADER_GOES_HERE

#include "../types.h"

#ifndef NO_GLOBALS
USHORT automaptype[512];
#endif
static int MapX;
static int MapY;
#ifndef NO_GLOBALS
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

// color used to draw the player's arrow
#define COLOR_PLAYER      (PAL8_ORANGE+1)
// color for bright map lines (doors, stairs etc.)
#define COLOR_BRIGHT      PAL8_YELLOW
// color for dim map lines/dots
#define COLOR_DIM         (PAL16_YELLOW+8)

#define MAPFLAG_TYPE      0x000F
// these are in the second byte
#define MAPFLAG_VERTDOOR   0x01
#define MAPFLAG_HORZDOOR   0x02
#define MAPFLAG_VERTARCH   0x04
#define MAPFLAG_HORZARCH   0x08
#define MAPFLAG_VERTGRATE  0x10
#define MAPFLAG_HORZGRATE  0x20
#define MAPFLAG_SQUARE     0x40
#define MAPFLAG_STAIRS     0x80

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
	memset(automaptype, 0, sizeof(automaptype));
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
	memset(automapview, 0, sizeof(automapview));
	v13 = 0;
	do
	{
		v14 = (unsigned char *)dFlags + v13;
		v15 = 112;
		do
		{
			*v14 &= ~DFLAG_EXPLORED;
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
	int cells;
	int screen_x;
	int screen_y;
	int i, j;
	int mapx, mapy;

	if ( leveltype == DTYPE_TOWN ) {
		DrawAutomapGame();
		return;
	}

	gpBufEnd = (unsigned char *)&gpBuffer->row[352];

	MapX = (ViewX - 16) >> 1;
	while ( MapX + AutoMapXOfs < 0 )
		AutoMapXOfs++;
	while ( MapX + AutoMapXOfs >= DMAXX )
		AutoMapXOfs--;
	MapX += AutoMapXOfs;

	MapY = (ViewY - 16) >> 1;
	while ( MapY + AutoMapYOfs < 0 )
		AutoMapYOfs++;
	while ( MapY + AutoMapYOfs >= DMAXY )
		AutoMapYOfs--;
	MapY += AutoMapYOfs;

	cells = AMbyte_4B7E4C[(AutoMapScale - 50) / 5];
	if ( ScrollInfo._sxoff + ScrollInfo._syoff )
		cells++;
	mapx = MapX - cells;
	mapy = MapY - 1;

	if ( cells & 1 ) {
		screen_x = 384 - AutoMapPosBits * ((cells - 1) >> 1);
		screen_y = 336 - AutoMapXPos * ((cells + 1) >> 1);
	} else {
		screen_x = 384 - AutoMapPosBits * (cells >> 1) + AutoMapXPos;
		screen_y = 336 - AutoMapXPos * (cells >> 1) - AutoMapYPos;
	}
	if ( ViewX & 1 ) {
		screen_x -= AutoMapYPos;
		screen_y -= AMPlayerX;
	}
	if ( ViewY & 1 ) {
		screen_x += AutoMapYPos;
		screen_y -= AMPlayerX;
	}

	screen_x += AutoMapScale * ScrollInfo._sxoff / 100 >> 1;
	screen_y += AutoMapScale * ScrollInfo._syoff / 100 >> 1;
	if ( invflag || sbookflag ) {
		screen_x -= 160;
	}
	if ( chrflag || questlog ) {
		screen_x += 160;
	}

	for ( i = 0; i <= cells + 1; i++ ) {
		int x = screen_x;
		int y;

		for ( j = 0; j < cells; j++ ) {
			USHORT maptype = GetAutomapType(mapx + j, mapy - j, TRUE);
			if ( maptype )
				DrawAutomapType(x, screen_y, maptype);
			x += AutoMapPosBits;
		}
		mapy++;
		x = screen_x - AutoMapXPos;
		y = screen_y + AutoMapYPos;
		for ( j = 0; j <= cells; j++ ) {
			USHORT maptype = GetAutomapType(mapx + j, mapy - j, TRUE);
			if ( maptype )
				DrawAutomapType(x, y, maptype);
			x += AutoMapPosBits;
		}
		mapx++;
		screen_y += AutoMapXPos;
	}
	DrawAutomapPlr();
	DrawAutomapGame();
}
// 4B8968: using guessed type int sbookflag;
// 5BB1ED: using guessed type char leveltype;
// 69BD04: using guessed type int questlog;
// 69CF0C: using guessed type int gpBufEnd;

void __fastcall DrawAutomapType(int screen_x, int screen_y, USHORT automap_type)
{
	BOOL do_vert;
	BOOL do_horz;
	BOOL do_cave_horz;
	BOOL do_cave_vert;
	int x1, y1, x2, y2;

	UCHAR flags = automap_type >> 8;

	if ( flags & MAPFLAG_SQUARE ) {
		ENG_set_pixel(screen_x, screen_y, COLOR_DIM);
		ENG_set_pixel(screen_x - AMPlayerX, screen_y - AMPlayerY, COLOR_DIM);
		ENG_set_pixel(screen_x - AMPlayerX, screen_y + AMPlayerY, COLOR_DIM);
		ENG_set_pixel(screen_x + AMPlayerX, screen_y - AMPlayerY, COLOR_DIM);
		ENG_set_pixel(screen_x + AMPlayerX, screen_y + AMPlayerY, COLOR_DIM);
		ENG_set_pixel(screen_x - AutoMapYPos, screen_y, COLOR_DIM);
		ENG_set_pixel(screen_x + AutoMapYPos, screen_y, COLOR_DIM);
		ENG_set_pixel(screen_x, screen_y - AMPlayerX, COLOR_DIM);
		ENG_set_pixel(screen_x, screen_y + AMPlayerX, COLOR_DIM);
		ENG_set_pixel(screen_x + AMPlayerX - AutoMapXPos, screen_y + AMPlayerY, COLOR_DIM);
		ENG_set_pixel(screen_x - AMPlayerX + AutoMapXPos, screen_y + AMPlayerY, COLOR_DIM);
		ENG_set_pixel(screen_x - AutoMapYPos, screen_y + AMPlayerX, COLOR_DIM);
		ENG_set_pixel(screen_x + AutoMapYPos, screen_y + AMPlayerX, COLOR_DIM);
		ENG_set_pixel(screen_x - AMPlayerX, screen_y + AutoMapYPos - AMPlayerY, COLOR_DIM);
		ENG_set_pixel(screen_x + AMPlayerX, screen_y + AutoMapYPos - AMPlayerY, COLOR_DIM);
		ENG_set_pixel(screen_x, screen_y + AutoMapYPos, COLOR_DIM);
	}

	if ( flags & MAPFLAG_STAIRS ) {
		DrawLine(screen_x - AMPlayerX, screen_y - AMPlayerX - AMPlayerY, screen_x + AMPlayerX + AutoMapYPos, screen_y + AMPlayerY, COLOR_BRIGHT);
		DrawLine(screen_x - AutoMapYPos, screen_y - AMPlayerX, screen_x + AutoMapYPos, screen_y + AMPlayerX, COLOR_BRIGHT);
		DrawLine(screen_x - AutoMapYPos - AMPlayerX, screen_y - AMPlayerY, screen_x + AMPlayerX, screen_y + AMPlayerX + AMPlayerY, COLOR_BRIGHT);
		DrawLine(screen_x - AutoMapXPos, screen_y, screen_x, screen_y + AutoMapYPos, COLOR_BRIGHT);
	}

	do_vert = FALSE;
	do_horz = FALSE;
	do_cave_horz = FALSE;
	do_cave_vert = FALSE;
	switch ( automap_type & MAPFLAG_TYPE ) {
		case 1: // stand-alone column or other unpassable object
			x1 = screen_x - AutoMapYPos;
			y1 = screen_y - AutoMapYPos;
			x2 = x1 + AutoMapXPos;
			y2 = screen_y - AMPlayerX;
			DrawLine(screen_x, y1, x1, y2, COLOR_DIM);
			DrawLine(screen_x, y1, x2, y2, COLOR_DIM);
			DrawLine(screen_x, screen_y, x1, y2, COLOR_DIM);
			DrawLine(screen_x, screen_y, x2, y2, COLOR_DIM);
			return;
		case 2:
		case 5:
			do_vert = TRUE;
			break;
		case 4:
			do_vert = TRUE;
			do_horz = TRUE;
			break;
		case 3:
		case 6:
			do_horz = TRUE;
			break;
		case 8:
			do_vert = TRUE;
			do_cave_horz = TRUE;
			break;
		case 9:
			do_horz = TRUE;
			do_cave_vert = TRUE;
			break;
		case 10:
			do_cave_horz = TRUE;
			break;
		case 11:
			do_cave_vert = TRUE;
			break;
		case 12:
			do_cave_horz = TRUE;
			do_cave_vert = TRUE;
			break;
	}

	if ( do_vert ) { // right-facing obstacle
		if ( flags & MAPFLAG_VERTDOOR ) { // two wall segments with a door in the middle
			x1 = screen_x - AutoMapXPos;
			x2 = screen_x - AutoMapYPos;
			y1 = screen_y - AutoMapYPos;
			y2 = screen_y - AMPlayerX;

			DrawLine(screen_x, y1, screen_x - AMPlayerX, y1 + AMPlayerY, COLOR_DIM);
			DrawLine(x1, screen_y, x1 + AMPlayerX, screen_y - AMPlayerY, COLOR_DIM);
			DrawLine(x2, y1, x1, y2, COLOR_BRIGHT);
			DrawLine(x2, y1, screen_x, y2, COLOR_BRIGHT);
			DrawLine(x2, screen_y, x1, y2, COLOR_BRIGHT);
			DrawLine(x2, screen_y, screen_x, y2, COLOR_BRIGHT);
		}
		if ( flags & MAPFLAG_VERTGRATE ) { // right-facing half-wall
			DrawLine(screen_x - AutoMapYPos, screen_y - AMPlayerX, screen_x - AutoMapXPos, screen_y, COLOR_DIM);
			flags |= MAPFLAG_VERTARCH;
		}
		if ( flags & MAPFLAG_VERTARCH ) { // window or passable column
			x1 = screen_x - AutoMapYPos;
			y1 = screen_y - AutoMapYPos;
			x2 = x1 + AutoMapXPos;
			y2 = screen_y - AMPlayerX;

			DrawLine(screen_x, y1, x1, y2, COLOR_DIM);
			DrawLine(screen_x, y1, x2, y2, COLOR_DIM);
			DrawLine(screen_x, screen_y, x1, y2, COLOR_DIM);
			DrawLine(screen_x, screen_y, x2, y2, COLOR_DIM);
		}
		if ( !( flags & (MAPFLAG_VERTDOOR | MAPFLAG_VERTGRATE | MAPFLAG_VERTARCH )) )
			DrawLine(screen_x, screen_y - AutoMapYPos, screen_x - AutoMapXPos, screen_y, COLOR_DIM);
	}

	if ( do_horz ) { // left-facing obstacle
		if ( flags & MAPFLAG_HORZDOOR ) {
			x1 = screen_x + AutoMapYPos;
			x2 = screen_x + AutoMapXPos;
			y1 = screen_y - AutoMapYPos;
			y2 = screen_y - AMPlayerX;

			DrawLine(screen_x, y1, screen_x + AMPlayerX, y1 + AMPlayerY, COLOR_DIM);
			DrawLine(x2, screen_y, x2 - AMPlayerX, screen_y - AMPlayerY, COLOR_DIM);
			DrawLine(x1, y1, screen_x, y2, COLOR_BRIGHT);
			DrawLine(x1, y1, x2, y2, COLOR_BRIGHT);
			DrawLine(x1, screen_y, screen_x, y2, COLOR_BRIGHT);
			DrawLine(x1, screen_y, x2, y2, COLOR_BRIGHT);
		}
		if ( flags & MAPFLAG_HORZGRATE ) {
			DrawLine(screen_x + AutoMapYPos, screen_y - AMPlayerX, screen_x + AutoMapXPos, screen_y, COLOR_DIM);
			flags |= MAPFLAG_HORZARCH;
		}
		if ( flags & MAPFLAG_HORZARCH ) {
			x1 = screen_x - AutoMapYPos;
			y1 = screen_y - AutoMapYPos;
			x2 = x1 + AutoMapXPos;
			y2 = screen_y - AMPlayerX;

			DrawLine(screen_x, y1, x1, y2, COLOR_DIM);
			DrawLine(screen_x, y1, x2, y2, COLOR_DIM);
			DrawLine(screen_x, screen_y, x1, y2, COLOR_DIM);
			DrawLine(screen_x, screen_y, x2, y2, COLOR_DIM);
		}
		if ( !( flags & ( MAPFLAG_HORZDOOR | MAPFLAG_HORZGRATE | MAPFLAG_HORZARCH )) )
			DrawLine(screen_x, screen_y - AutoMapYPos, screen_x + AutoMapXPos, screen_y, COLOR_DIM);
	}

	// for caves the horz/vert flags are switched
	if ( do_cave_horz ) {
		if ( flags & MAPFLAG_VERTDOOR ) {
			x1 = screen_x - AutoMapXPos;
			x2 = screen_x - AutoMapYPos;
			y1 = screen_y + AutoMapYPos;
			y2 = screen_y + AMPlayerX;

			DrawLine(screen_x, y1, screen_x - AMPlayerX, y1 - AMPlayerY, COLOR_DIM);
			DrawLine(x1, screen_y, x1 + AMPlayerX, screen_y + AMPlayerY, COLOR_DIM);
			DrawLine(x2, y1, x1, y2, COLOR_BRIGHT);
			DrawLine(x2, y1, screen_x, y2, COLOR_BRIGHT);
			DrawLine(x2, screen_y, x1, y2, COLOR_BRIGHT);
			DrawLine(x2, screen_y, screen_x, y2, COLOR_BRIGHT);
		} else
			DrawLine(screen_x, screen_y + AutoMapYPos, screen_x - AutoMapXPos, screen_y, COLOR_DIM);
	}

	if ( do_cave_vert ) {
		if ( flags & MAPFLAG_HORZDOOR ) {
			x1 = screen_x + AutoMapYPos;
			x2 = screen_x + AutoMapXPos;
			y1 = screen_y + AutoMapYPos;
			y2 = screen_y + AMPlayerX;

			DrawLine(screen_x, y1, screen_x + AMPlayerX, y1 - AMPlayerY, COLOR_DIM);
			DrawLine(x2, screen_y, x2 - AMPlayerX, screen_y + AMPlayerY, COLOR_DIM);
			DrawLine(x1, y1, screen_x, y2, COLOR_BRIGHT);
			DrawLine(x1, y1, x2, y2, COLOR_BRIGHT);
			DrawLine(x1, screen_y, screen_x, y2, COLOR_BRIGHT);
			DrawLine(x1, screen_y, x2, y2, COLOR_BRIGHT);
		} else
			DrawLine(screen_x, screen_y + AutoMapYPos, screen_x + AutoMapXPos, screen_y, COLOR_DIM);
	}
}

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

USHORT __fastcall GetAutomapType(int x, int y, BOOL view)
{
	int v3; // edi
	int v4; // esi
	int v6; // eax
	USHORT v7; // bp

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
	if ( v7 == 7 && (GetAutomapType(x - 1, y, 0) >> 8) & 8 )
	{
		if ( (GetAutomapType(v4, v3 - 1, 0) >> 8) & 4 )
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
