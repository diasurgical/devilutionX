//HEADER_GOES_HERE

#include "../types.h"

#ifndef NO_GLOBALS
int engine_cpp_init_value; // weak

char gbPixelCol; // automap pixel color 8-bit (palette entry)
int dword_52B970; // bool flip - if y < x
int orgseed; // weak
int sgnWidth;
int sglGameSeed; // weak
#endif
STATIC CRITICAL_SECTION sgMemCrit;
#ifndef NO_GLOBALS
int SeedCount; // weak
int dword_52B99C; // bool valid - if x/y are in bounds
#endif

const int engine_inf = 0x7F800000; // weak
const int rand_increment = 1; // unused
const int rand_multiplier = 0x015A4E35; // unused

struct engine_cpp_init_1
{
	engine_cpp_init_1()
	{
		engine_cpp_init_value = engine_inf;
	}
} _engine_cpp_init_1;
// 47A474: using guessed type int engine_inf;
// 52B968: using guessed type int engine_cpp_init_value;

void __fastcall CelDrawDatOnly(char *pDecodeTo, char *pRLEBytes, int dwRLESize, int dwRLEWdt)
{
	char *v4; // esi
	char *v5; // edi
	int v6; // edx
	unsigned int v7; // eax
	unsigned int v8; // ecx
	char v9; // cf
	unsigned int v10; // ecx
	char *v11; // [esp+4h] [ebp-8h]

	v11 = pRLEBytes;
	if ( pDecodeTo && pRLEBytes )
	{
		v4 = pRLEBytes;
		v5 = pDecodeTo;
		do
		{
			v6 = dwRLEWdt;
			do
			{
				while ( 1 )
				{
					v7 = (unsigned char)*v4++;
					if ( (v7 & 0x80u) == 0 )
						break;
					_LOBYTE(v7) = -(char)v7;
					v5 += v7;
					v6 -= v7;
					if ( !v6 )
						goto LABEL_14;
				}
				v6 -= v7;
				v8 = v7 >> 1;
				if ( v7 & 1 )
				{
					*v5++ = *v4++;
					if ( !v8 )
						continue;
				}
				v9 = v8 & 1;
				v10 = v7 >> 2;
				if ( v9 )
				{
					*(_WORD *)v5 = *(_WORD *)v4;
					v4 += 2;
					v5 += 2;
					if ( !v10 )
						continue;
				}
				qmemcpy(v5, v4, 4 * v10);
				v4 += 4 * v10;
				v5 += 4 * v10;
			}
			while ( v6 );
LABEL_14:
			v5 += -dwRLEWdt - 768;
		}
		while ( &v11[dwRLESize] != v4 );
	}
}

void __fastcall CelDecodeOnly(int screen_x, int screen_y, void *pCelBuff, int frame, int frame_width)
{
	if ( gpBuffer )
	{
		if ( pCelBuff )
			CelDrawDatOnly(
				(char *)gpBuffer + screen_y_times_768[screen_y] + screen_x,
				(char *)pCelBuff + *((_DWORD *)pCelBuff + frame),
				*((_DWORD *)pCelBuff + frame + 1) - *((_DWORD *)pCelBuff + frame),
				frame_width);
	}
}

void __fastcall CelDecDatOnly(char *pBuff, char *pCelBuff, int frame, int frame_width)
{
	if ( pCelBuff )
	{
		if ( pBuff )
			CelDrawDatOnly(
				pBuff,
				&pCelBuff[*(_DWORD *)&pCelBuff[4 * frame]],
				*(_DWORD *)&pCelBuff[4 * frame + 4] - *(_DWORD *)&pCelBuff[4 * frame],
				frame_width);
	}
}

void __fastcall CelDrawHdrOnly(int screen_x, int screen_y, char *pCelBuff, int frame, int frame_width, int always_0, int direction)
{
	int v7; // edx
	char *v8; // eax
	int v9; // edi
	int v10; // ecx
	int v11; // [esp+Ch] [ebp-8h]
	int v12; // [esp+10h] [ebp-4h]

	v12 = screen_y;
	v11 = screen_x;
	if ( gpBuffer )
	{
		if ( pCelBuff )
		{
			v7 = *(_DWORD *)&pCelBuff[4 * frame];
			v8 = &pCelBuff[v7];
			v9 = *(unsigned short *)&pCelBuff[v7 + always_0];
			if ( *(_WORD *)&pCelBuff[v7 + always_0] )
			{
				if ( direction != 8 && *(_WORD *)&v8[direction] )
					v10 = *(unsigned short *)&v8[direction] - v9;
				else
					v10 = *(_DWORD *)&pCelBuff[4 * frame + 4] - v7 - v9;
				CelDrawDatOnly(
					(char *)gpBuffer + screen_y_times_768[v12 - 16 * always_0] + v11,
					&v8[v9],
					v10,
					frame_width);
			}
		}
	}
}

void __fastcall CelDecodeHdrOnly(char *pBuff, char *pCelBuff, int frame, int frame_width, int always_0, int direction)
{
	int v6; // esi
	char *v7; // eax
	int v8; // ebx
	int v9; // edx
	int v10; // edx

	if ( pCelBuff )
	{
		if ( pBuff )
		{
			v6 = *(_DWORD *)&pCelBuff[4 * frame];
			v7 = &pCelBuff[v6];
			v8 = *(unsigned short *)&pCelBuff[v6 + always_0];
			if ( *(_WORD *)&pCelBuff[v6 + always_0] )
			{
				v9 = *(_DWORD *)&pCelBuff[4 * frame + 4] - v6;
				if ( direction != 8 && *(_WORD *)&v7[direction] )
					v10 = *(unsigned short *)&v7[direction] - v8;
				else
					v10 = v9 - v8;
				CelDrawDatOnly(pBuff, &v7[v8], v10, frame_width);
			}
		}
	}
}

void __fastcall CelDecDatLightOnly(char *pDecodeTo, char *pRLEBytes, int frame_content_size, int frame_width)
{
	char *v4; // esi
	char *v5; // edi
	char *v6; // ebx
	int v7; // edx
	int v8; // eax
	int v9; // ST00_4
	char *a3; // [esp+Ch] [ebp-10h]

	if ( pDecodeTo && pRLEBytes )
	{
		a3 = &pLightTbl[256 * light_table_index];
		v4 = pRLEBytes;
		v5 = pDecodeTo;
		v6 = &pRLEBytes[frame_content_size];
		do
		{
			v7 = frame_width;
			do
			{
				while ( 1 )
				{
					v8 = (unsigned char)*v4++;
					if ( (v8 & 0x80u) != 0 ) /* check sign */
						break;
					v9 = v7 - v8;
					CelDecDatLightEntry(v8, a3, v5, v4);
					v7 = v9;
					if ( !v9 )
						goto LABEL_9;
				}
				_LOBYTE(v8) = -(char)v8;
				v5 += v8;
				v7 -= v8;
			}
			while ( v7 );
LABEL_9:
			v5 += -frame_width - 768;
		}
		while ( v6 != v4 );
	}
}
// 69BEF8: using guessed type int light_table_index;

void __fastcall CelDecDatLightEntry(unsigned char shift, char *LightIndex, char *&pDecodeTo, char *&pRLEBytes)
{
	char v5; // cf
	unsigned char v6; // cl
	char v7; // cl
	int v8; // eax
	char v9; // ch
	int tmp; // eax
	char v11; // ch
	char v12; // ch
	unsigned char a1;

	v5 = shift & 1;
	v6 = shift >> 1;
	if ( v5 )
	{
		a1 = *pRLEBytes;
		*pDecodeTo = LightIndex[a1];
		++pRLEBytes;
		++pDecodeTo;
	}
	v5 = v6 & 1;
	v7 = v6 >> 1;
	if ( v5 )
	{
		a1 = *pRLEBytes;
		*pDecodeTo = LightIndex[a1];
		a1 = pRLEBytes[1];
		pDecodeTo[1] = LightIndex[a1];
		pRLEBytes += 2;
		pDecodeTo += 2;
	}
	for ( ; v7; --v7 )
	{
		v8 = *(_DWORD *)pRLEBytes;
		pRLEBytes += 4;
		a1 = v8;
		v9 = LightIndex[a1];
		a1 = BYTE1(v8);
		tmp = __ROR4__(v8, 16);
		*pDecodeTo = v9;
		v11 = LightIndex[a1];
		a1 = tmp;
		pDecodeTo[1] = v11;
		v12 = LightIndex[a1];
		a1 = BYTE1(tmp);
		pDecodeTo[2] = v12;
		pDecodeTo[3] = LightIndex[a1];
		pDecodeTo += 4;
	}
}

void __fastcall CelDecDatLightTrans(char *pDecodeTo, char *pRLEBytes, int frame_content_size, int frame_width)
{
	char *v4; // esi
	int v5; // edi
	char *v6; // ebx
	int v7; // edx
	unsigned int v8; // eax
	unsigned int v10; // ecx
	char v11; // cf
	unsigned int v12; // ecx
	char *v13; // esi
	_BYTE *v14; // edi
	_BYTE *v18; // edi
	unsigned int v21; // ecx
	_BYTE *v25; // edi
	char *v26; // [esp-4h] [ebp-24h]
	char *v27; // [esp+Ch] [ebp-14h]
	int v28; // [esp+14h] [ebp-Ch]
	int _EAX;
	char *_EBX;

	if ( pDecodeTo && pRLEBytes )
	{
		v27 = &pLightTbl[256 * light_table_index];
		v4 = pRLEBytes;
		v5 = (int)pDecodeTo;
		v6 = &pRLEBytes[frame_content_size];
		v28 = (unsigned char)pDecodeTo & 1;
		do
		{
			v7 = frame_width;
			do
			{
				while ( 1 )
				{
					v8 = (unsigned char)*v4++;
					if ( (v8 & 0x80u) != 0 )
						break;
					v26 = v6;
					_EBX = v27;
					v7 -= v8;
					if ( (v5 & 1) == v28 )
					{
						v10 = v8 >> 1;
						if ( !(v8 & 1) )
							goto LABEL_10;
						++v4;
						++v5;
						if ( v10 )
						{
LABEL_17:
							v11 = v10 & 1;
							v21 = v10 >> 1;
							if ( !v11 )
								goto LABEL_26;
							_EAX = *v4;
							ASM_XLAT(_EAX,_EBX);
							*(_BYTE *)v5 = _EAX;
							v4 += 2;
							v5 += 2;
							if ( v21 )
							{
LABEL_26:
								do
								{
									_EAX = *(_DWORD *)v4;
									v4 += 4;
									ASM_XLAT(_EAX,_EBX);
									*(_BYTE *)v5 = _EAX;
									v25 = (_BYTE *)(v5 + 2);
									_EAX = __ROR4__(_EAX, 16);
									ASM_XLAT(_EAX,_EBX);
									*v25 = _EAX;
									v5 = (int)(v25 + 2);
									--v21;
								}
								while ( v21 );
							}
							goto LABEL_20;
						}
					}
					else
					{
						v10 = v8 >> 1;
						if ( !(v8 & 1) )
							goto LABEL_17;
						_EAX = *v4++;
						ASM_XLAT(_EAX,_EBX);
						*(_BYTE *)v5++ = _EAX;
						if ( v10 )
						{
LABEL_10:
							v11 = v10 & 1;
							v12 = v10 >> 1;
							if ( !v11 )
								goto LABEL_27;
							v13 = v4 + 1;
							v14 = (_BYTE *)(v5 + 1);
							_EAX = *v13;
							v4 = v13 + 1;
							ASM_XLAT(_EAX,_EBX);
							*v14 = _EAX;
							v5 = (int)(v14 + 1);
							if ( v12 )
							{
LABEL_27:
								do
								{
									_EAX = *(_DWORD *)v4;
									v4 += 4;
									v18 = (_BYTE *)(v5 + 1);
									_EAX = __ROR4__(_EAX, 8);
									ASM_XLAT(_EAX,_EBX);
									*v18 = _EAX;
									_EAX = __ROR4__(_EAX, 16);
									v18 += 2;
									ASM_XLAT(_EAX,_EBX);
									*v18 = _EAX;
									v5 = (int)(v18 + 1);
									--v12;
								}
								while ( v12 );
							}
							goto LABEL_20;
						}
					}
LABEL_20:
					v6 = v26;
					if ( !v7 )
						goto LABEL_23;
				}
				_LOBYTE(v8) = -(char)v8;
				v5 += v8;
				v7 -= v8;
			}
			while ( v7 );
LABEL_23:
			v5 -= frame_width + 768;
			v28 = ((_BYTE)v28 + 1) & 1;
		}
		while ( v6 != v4 );
	}
}
// 69BEF8: using guessed type int light_table_index;

void __fastcall CelDecodeLightOnly(int screen_x, int screen_y, char *pCelBuff, int frame, int frame_width)
{
	int v5; // ebx
	int v6; // esi
	char *v7; // edx
	char *v8; // ecx
	int v9; // [esp-8h] [ebp-14h]

	v5 = screen_y;
	if ( gpBuffer && pCelBuff )
	{
		v6 = *(_DWORD *)&pCelBuff[4 * frame];
		v7 = &pCelBuff[v6];
		v8 = (char *)gpBuffer + screen_y_times_768[v5] + screen_x;
		v9 = *(_DWORD *)&pCelBuff[4 * frame + 4] - v6;
		if ( light_table_index )
			CelDecDatLightOnly(v8, v7, v9, frame_width);
		else
			CelDrawDatOnly(v8, v7, v9, frame_width);
	}
}
// 69BEF8: using guessed type int light_table_index;

void __fastcall CelDecodeHdrLightOnly(int screen_x, int screen_y, char *pCelBuff, int frame, int frame_width, int always_0, int direction)
{
	int v7; // esi
	char *v8; // ecx
	int v9; // edi
	char *v10; // edx
	int v11; // ebx
	int v12; // ebx
	char *v13; // edx
	char *v14; // ecx
	int v15; // [esp+Ch] [ebp-4h]
	char *cel_buf; // [esp+18h] [ebp+8h]

	v7 = screen_y;
	v15 = screen_x;
	if ( gpBuffer )
	{
		v8 = pCelBuff;
		if ( pCelBuff )
		{
			v9 = *(_DWORD *)&pCelBuff[4 * frame];
			v10 = &pCelBuff[v9];
			v11 = *(unsigned short *)&pCelBuff[v9 + always_0];
			cel_buf = (char *)*(unsigned short *)&pCelBuff[v9 + always_0];
			if ( v11 )
			{
				if ( direction != 8 && *(_WORD *)&v10[direction] )
					v12 = *(unsigned short *)&v10[direction] - (_DWORD)cel_buf;
				else
					v12 = *(_DWORD *)&v8[4 * frame + 4] - v9 - (_DWORD)cel_buf;
				v13 = &v10[(_DWORD)cel_buf];
				v14 = (char *)gpBuffer + screen_y_times_768[v7 - 16 * always_0] + v15;
				if ( light_table_index )
					CelDecDatLightOnly(v14, v13, v12, frame_width);
				else
					CelDrawDatOnly(v14, v13, v12, frame_width);
			}
		}
	}
}
// 69BEF8: using guessed type int light_table_index;

void __fastcall CelDecodeHdrLightTrans(char *pBuff, char *pCelBuff, int frame, int frame_width, int always_0, int direction)
{
	char *v6; // eax
	int v7; // esi
	char *v8; // edx
	int v9; // ebx
	int v10; // eax
	int v11; // eax
	char *v12; // edx

	v6 = pCelBuff;
	if ( pCelBuff )
	{
		if ( pBuff )
		{
			v7 = *(_DWORD *)&pCelBuff[4 * frame];
			v8 = &pCelBuff[v7];
			v9 = *(unsigned short *)&v6[v7 + always_0];
			if ( *(_WORD *)&v6[v7 + always_0] )
			{
				v10 = *(_DWORD *)&v6[4 * frame + 4] - v7;
				if ( direction != 8 && *(_WORD *)&v8[direction] )
					v11 = *(unsigned short *)&v8[direction] - v9;
				else
					v11 = v10 - v9;
				v12 = &v8[v9];
				if ( cel_transparency_active )
				{
					CelDecDatLightTrans(pBuff, v12, v11, frame_width);
				}
				else if ( light_table_index )
				{
					CelDecDatLightOnly(pBuff, v12, v11, frame_width);
				}
				else
				{
					CelDrawDatOnly(pBuff, v12, v11, frame_width);
				}
			}
		}
	}
}
// 69BEF8: using guessed type int light_table_index;
// 69CF94: using guessed type int cel_transparency_active;

void __fastcall CelDrawHdrLightRed(int screen_x, int screen_y, char *pCelBuff, int frame, int frame_width, int always_0, int direction, char always_1)
{
	char *v8; // esi
	int v9; // ebx
	int v10; // eax
	char *v11; // edi
	int v12; // ecx
	int v13; // esi
	int v14; // eax
	int v15; // eax
	_BYTE *v16; // esi
	char *v17; // edi
	int v18; // edx
	int v19; // eax
	int v20; // ecx
	int v21; // [esp+Ch] [ebp-4h]
	char *v22; // [esp+Ch] [ebp-4h]
	char *cel_buf; // [esp+18h] [ebp+8h]
	char *cel_bufa; // [esp+18h] [ebp+8h]
	int framea; // [esp+1Ch] [ebp+Ch]
	int always_0a; // [esp+24h] [ebp+14h]
	int directiona; // [esp+28h] [ebp+18h]

	v21 = screen_x;
	if ( gpBuffer )
	{
		v8 = pCelBuff;
		if ( pCelBuff )
		{
			v9 = *(_DWORD *)&pCelBuff[4 * frame];
			v10 = always_0;
			v11 = &pCelBuff[v9];
			v12 = *(unsigned short *)&pCelBuff[v9 + always_0];
			cel_buf = (char *)*(unsigned short *)&pCelBuff[v9 + always_0];
			if ( v12 )
			{
				v13 = *(_DWORD *)&v8[4 * frame + 4] - v9;
				if ( direction != 8 && *(_WORD *)&v11[direction] )
					always_0a = *(unsigned short *)&v11[direction] - (_DWORD)cel_buf;
				else
					always_0a = v13 - (_DWORD)cel_buf;
				directiona = (int)&v11[(_DWORD)cel_buf];
				cel_bufa = (char *)gpBuffer + screen_y_times_768[screen_y - 16 * v10] + v21;
				v14 = -(light4flag != 0);
				_LOWORD(v14) = v14 & 0xF400;
				v15 = v14 + 4096;
				framea = v15;
				if ( always_1 == 2 )
				{
					v15 += 256;
					framea = v15;
				}
				if ( always_1 >= 4 )
					framea = v15 + (always_1 << 8) - 256;
				v22 = &pLightTbl[framea];
				v16 = (_BYTE *)directiona;
				v17 = cel_bufa;
				do
				{
					v18 = frame_width;
					do
					{
						while ( 1 )
						{
							v19 = (unsigned char)*v16++;
							if ( (v19 & 0x80u) == 0 )
								break;
							_LOBYTE(v19) = -(char)v19;
							v17 += v19;
							v18 -= v19;
							if ( !v18 )
								goto LABEL_20;
						}
						v18 -= v19;
						v20 = v19;
						do
						{
							_LOBYTE(v19) = *v16++;
							*v17 = v22[v19];
							--v20;
							++v17;
						}
						while ( v20 );
					}
					while ( v18 );
LABEL_20:
					v17 += -frame_width - 768;
				}
				while ( (_BYTE *)(directiona + always_0a) != v16 );
			}
		}
	}
}
// 525728: using guessed type int light4flag;

void __fastcall Cel2DecDatOnly(char *pDecodeTo, char *pRLEBytes, int frame_content_size, int frame_width)
{
	char *v4; // esi
	char *v5; // edi
	int v6; // edx
	unsigned int v7; // eax
	unsigned int v8; // ecx
	char v9; // cf
	unsigned int v10; // ecx
	char *v11; // [esp+4h] [ebp-8h]

	v11 = pRLEBytes;
	if ( pDecodeTo && pRLEBytes && gpBuffer )
	{
		v4 = pRLEBytes;
		v5 = pDecodeTo;
		do
		{
			v6 = frame_width;
			do
			{
				while ( 1 )
				{
					v7 = (unsigned char)*v4++;
					if ( (v7 & 0x80u) == 0 )
						break;
					_LOBYTE(v7) = -(char)v7;
					v5 += v7;
					v6 -= v7;
					if ( !v6 )
						goto LABEL_17;
				}
				v6 -= v7;
				if ( v5 < (char *)gpBufEnd )
				{
					v8 = v7 >> 1;
					if ( !(v7 & 1) || (*v5 = *v4, ++v4, ++v5, v8) )
					{
						v9 = v8 & 1;
						v10 = v7 >> 2;
						if ( !v9 || (*(_WORD *)v5 = *(_WORD *)v4, v4 += 2, v5 += 2, v10) )
						{
							qmemcpy(v5, v4, 4 * v10);
							v4 += 4 * v10;
							v5 += 4 * v10;
						}
					}
				}
				else
				{
					v4 += v7;
					v5 += v7;
				}
			}
			while ( v6 );
LABEL_17:
			v5 += -frame_width - 768;
		}
		while ( &v11[frame_content_size] != v4 );
	}
}
// 69CF0C: using guessed type int gpBufEnd;

void __fastcall Cel2DrawHdrOnly(int screen_x, int screen_y, char *pCelBuff, int frame, int frame_width, int a6, int direction)
{
	int v7; // edx
	char *v8; // eax
	int v9; // edi
	int v10; // ecx
	int v11; // [esp+Ch] [ebp-8h]
	int v12; // [esp+10h] [ebp-4h]

	v12 = screen_y;
	v11 = screen_x;
	if ( gpBuffer )
	{
		if ( pCelBuff )
		{
			v7 = *(_DWORD *)&pCelBuff[4 * frame];
			v8 = &pCelBuff[v7];
			v9 = *(unsigned short *)&pCelBuff[v7 + a6];
			if ( *(_WORD *)&pCelBuff[v7 + a6] )
			{
				if ( direction != 8 && *(_WORD *)&v8[direction] )
					v10 = *(unsigned short *)&v8[direction] - v9;
				else
					v10 = *(_DWORD *)&pCelBuff[4 * frame + 4] - v7 - v9;
				Cel2DecDatOnly(
					(char *)gpBuffer + screen_y_times_768[v12 - 16 * a6] + v11,
					&v8[v9],
					v10,
					frame_width);
			}
		}
	}
}

void __fastcall Cel2DecodeHdrOnly(char *pBuff, char *pCelBuff, int frame, int frame_width, int a5, int direction)
{
	int v6; // edi
	char *v7; // esi
	int v8; // ebx
	int v9; // eax
	int v10; // edx
	int v11; // eax

	if ( pCelBuff )
	{
		if ( pBuff )
		{
			v6 = *(_DWORD *)&pCelBuff[4 * frame];
			v7 = &pCelBuff[v6];
			v8 = *(unsigned short *)&pCelBuff[v6 + a5];
			if ( *(_WORD *)&pCelBuff[v6 + a5] )
			{
				v9 = *(_DWORD *)&pCelBuff[4 * frame + 4] - v6;
				v10 = *(unsigned short *)&v7[direction];
				if ( direction == 8 )
					v10 = 0;
				if ( v10 )
					v11 = v10 - v8;
				else
					v11 = v9 - v8;
				Cel2DecDatOnly(pBuff, &v7[v8], v11, frame_width);
			}
		}
	}
}

void __fastcall Cel2DecDatLightOnly(char *pDecodeTo, char *pRLEBytes, int frame_content_size, int frame_width)
{
	char *v4; // esi
	char *v5; // edi
	char *v6; // ebx
	int v7; // edx
	int v8; // eax
	int v9; // ST00_4
	char *a3; // [esp+Ch] [ebp-10h]

	if ( pDecodeTo && pRLEBytes && gpBuffer )
	{
		a3 = &pLightTbl[256 * light_table_index];
		v4 = pRLEBytes;
		v5 = pDecodeTo;
		v6 = &pRLEBytes[frame_content_size];
		do
		{
			v7 = frame_width;
			do
			{
				while ( 1 )
				{
					v8 = (unsigned __int8)*v4++;
					if ( (v8 & 0x80u) == 0 ) /* check sign */
						break;
					_LOBYTE(v8) = -(char)v8;
					v5 += v8;
					v7 -= v8;
					if ( !v7 )
						goto LABEL_13;
				}
				v7 -= v8;
				if ( v5 < (char *)gpBufEnd )
				{
					v9 = v7;
					Cel2DecDatLightEntry(v8, a3, v5, v4);
					v7 = v9;
				}
				else
				{
					v4 += v8;
					v5 += v8;
				}
			}
			while ( v7 );
LABEL_13:
			v5 += -frame_width - 768;
		}
		while ( v6 != v4 );
	}
}
// 69BEF8: using guessed type int light_table_index;
// 69CF0C: using guessed type int gpBufEnd;

void __fastcall Cel2DecDatLightEntry(unsigned char shift, char *LightIndex, char *&pDecodeTo, char *&pRLEBytes)
{
	char v5; // cf
	unsigned char v6; // cl
	char v7; // cl
	int v8; // eax
	char v9; // ch
	int tmp; // eax
	char v11; // ch
	char v12; // ch
	unsigned char a1;

	v5 = shift & 1;
	v6 = shift >> 1;
	if ( v5 )
	{
		a1 = *pRLEBytes;
		*pDecodeTo = LightIndex[a1];
		++pRLEBytes;
		++pDecodeTo;
	}
	v5 = v6 & 1;
	v7 = v6 >> 1;
	if ( v5 )
	{
		a1 = *pRLEBytes;
		*pDecodeTo = LightIndex[a1];
		a1 = pRLEBytes[1];
		pDecodeTo[1] = LightIndex[a1];
		pRLEBytes += 2;
		pDecodeTo += 2;
	}
	for ( ; v7; --v7 )
	{
		v8 = *(_DWORD *)pRLEBytes;
		pRLEBytes += 4;
		a1 = v8;
		v9 = LightIndex[a1];
		a1 = BYTE1(v8);
		tmp = __ROR4__(v8, 16);
		*pDecodeTo = v9;
		v11 = LightIndex[a1];
		a1 = tmp;
		pDecodeTo[1] = v11;
		v12 = LightIndex[a1];
		a1 = BYTE1(tmp);
		pDecodeTo[2] = v12;
		pDecodeTo[3] = LightIndex[a1];
		pDecodeTo += 4;
	}
}

void __fastcall Cel2DecDatLightTrans(char *pDecodeTo, char *pRLEBytes, int frame_content_size, int frame_width)
{
	char *v4; // esi
	unsigned int v5; // edi
	char *v6; // ebx
	int v7; // edx
	unsigned int v8; // eax
	unsigned int v10; // ecx
	char v11; // cf
	unsigned int v12; // ecx
	char *v13; // esi
	_BYTE *v14; // edi
	_BYTE *v18; // edi
	unsigned int v21; // ecx
	_BYTE *v25; // edi
	char *v26; // [esp-4h] [ebp-24h]
	char *v27; // [esp+Ch] [ebp-14h]
	int v28; // [esp+14h] [ebp-Ch]
	int _EAX;
	char *_EBX;

	if ( pDecodeTo && pRLEBytes && gpBuffer )
	{
		v27 = &pLightTbl[256 * light_table_index];
		v4 = pRLEBytes;
		v5 = (unsigned int)pDecodeTo;
		v6 = &pRLEBytes[frame_content_size];
		v28 = (unsigned char)pDecodeTo & 1;
		do
		{
			v7 = frame_width;
			do
			{
				while ( 1 )
				{
					v8 = (unsigned char)*v4++;
					if ( (v8 & 0x80u) != 0 )
						break;
					v26 = v6;
					_EBX = v27;
					v7 -= v8;
					if ( v5 < (unsigned int)gpBufEnd )
					{
						if ( (v5 & 1) == v28 )
						{
							v10 = v8 >> 1;
							if ( !(v8 & 1) )
								goto LABEL_13;
							++v4;
							++v5;
							if ( v10 )
							{
LABEL_20:
								v11 = v10 & 1;
								v21 = v10 >> 1;
								if ( !v11 )
									goto LABEL_29;
								_EAX = *v4;
								ASM_XLAT(_EAX,_EBX);
								*(_BYTE *)v5 = _EAX;
								v4 += 2;
								v5 += 2;
								if ( v21 )
								{
LABEL_29:
									do
									{
										_EAX = *(_DWORD *)v4;
										v4 += 4;
										ASM_XLAT(_EAX,_EBX);
										*(_BYTE *)v5 = _EAX;
										v25 = (_BYTE *)(v5 + 2);
										_EAX = __ROR4__(_EAX, 16);
										ASM_XLAT(_EAX,_EBX);
										*v25 = _EAX;
										v5 = (unsigned int)(v25 + 2);
										--v21;
									}
									while ( v21 );
								}
								goto LABEL_23;
							}
						}
						else
						{
							v10 = v8 >> 1;
							if ( !(v8 & 1) )
								goto LABEL_20;
							_EAX = *v4++;
							ASM_XLAT(_EAX,_EBX);
							*(_BYTE *)v5++ = _EAX;
							if ( v10 )
							{
LABEL_13:
								v11 = v10 & 1;
								v12 = v10 >> 1;
								if ( !v11 )
									goto LABEL_30;
								v13 = v4 + 1;
								v14 = (_BYTE *)(v5 + 1);
								_EAX = *v13;
								v4 = v13 + 1;
								ASM_XLAT(_EAX,_EBX);
								*v14 = _EAX;
								v5 = (unsigned int)(v14 + 1);
								if ( v12 )
								{
LABEL_30:
									do
									{
										_EAX = *(_DWORD *)v4;
										v4 += 4;
										v18 = (_BYTE *)(v5 + 1);
										_EAX = __ROR4__(_EAX, 8);
										ASM_XLAT(_EAX,_EBX);
										*v18 = _EAX;
										_EAX = __ROR4__(_EAX, 16);
										v18 += 2;
										ASM_XLAT(_EAX,_EBX);
										*v18 = _EAX;
										v5 = (unsigned int)(v18 + 1);
										--v12;
									}
									while ( v12 );
								}
								goto LABEL_23;
							}
						}
					}
					else
					{
						v4 += v8;
						v5 += v8;
					}
LABEL_23:
					v6 = v26;
					if ( !v7 )
						goto LABEL_26;
				}
				_LOBYTE(v8) = -(char)v8;
				v5 += v8;
				v7 -= v8;
			}
			while ( v7 );
LABEL_26:
			v5 -= frame_width + 768;
			v28 = ((_BYTE)v28 + 1) & 1;
		}
		while ( v6 != v4 );
	}
}
// 69BEF8: using guessed type int light_table_index;
// 69CF0C: using guessed type int gpBufEnd;

void __fastcall Cel2DecodeHdrLight(int screen_x, int screen_y, char *pCelBuff, int frame, int frame_width, int a6, int direction)
{
	int v7; // esi
	char *v8; // eax
	int v9; // edi
	char *v10; // edx
	int v11; // ebx
	int v12; // eax
	int v13; // edi
	int v14; // eax
	char *v15; // edx
	char *v16; // ecx
	char *cel_buf; // [esp+18h] [ebp+8h]

	v7 = screen_y;
	if ( gpBuffer )
	{
		v8 = pCelBuff;
		if ( pCelBuff )
		{
			v9 = *(_DWORD *)&pCelBuff[4 * frame];
			v10 = &pCelBuff[v9];
			v11 = *(unsigned short *)&pCelBuff[v9 + a6];
			cel_buf = (char *)*(unsigned short *)&pCelBuff[v9 + a6];
			if ( v11 )
			{
				v12 = *(_DWORD *)&v8[4 * frame + 4] - v9;
				v13 = *(unsigned short *)&v10[direction];
				if ( direction == 8 )
					v13 = 0;
				if ( v13 )
					v14 = v13 - (_DWORD)cel_buf;
				else
					v14 = v12 - (_DWORD)cel_buf;
				v15 = &v10[(_DWORD)cel_buf];
				v16 = (char *)gpBuffer + screen_y_times_768[v7 - 16 * a6] + screen_x;
				if ( light_table_index )
					Cel2DecDatLightOnly(v16, v15, v14, frame_width);
				else
					Cel2DecDatOnly(v16, v15, v14, frame_width);
			}
		}
	}
}
// 69BEF8: using guessed type int light_table_index;

void __fastcall Cel2DecodeLightTrans(char *dst_buf, char *pCelBuff, int frame, int frame_width, int a5, int direction)
{
	char *v6; // eax
	int v7; // esi
	char *v8; // edx
	int v9; // ebx
	int v10; // eax
	int v11; // esi
	int v12; // eax
	char *v13; // edx

	v6 = pCelBuff;
	if ( pCelBuff )
	{
		v7 = *(_DWORD *)&pCelBuff[4 * frame];
		v8 = &pCelBuff[v7];
		v9 = *(unsigned short *)&v6[v7 + a5];
		if ( *(_WORD *)&v6[v7 + a5] )
		{
			v10 = *(_DWORD *)&v6[4 * frame + 4] - v7;
			v11 = *(unsigned short *)&v8[direction];
			if ( direction == 8 )
				v11 = 0;
			if ( v11 )
				v12 = v11 - v9;
			else
				v12 = v10 - v9;
			v13 = &v8[v9];
			if ( cel_transparency_active )
			{
				Cel2DecDatLightTrans(dst_buf, v13, v12, frame_width);
			}
			else if ( light_table_index )
			{
				Cel2DecDatLightOnly(dst_buf, v13, v12, frame_width);
			}
			else
			{
				Cel2DecDatOnly(dst_buf, v13, v12, frame_width);
			}
		}
	}
}
// 69BEF8: using guessed type int light_table_index;
// 69CF94: using guessed type int cel_transparency_active;

void __fastcall Cel2DrawHdrLightRed(int screen_x, int screen_y, char *pCelBuff, int frame, int frame_width, int always_0, int direction, char always_1)
{
	char *v8; // esi
	int v9; // ebx
	char *v10; // edi
	int v11; // ecx
	int v12; // esi
	int v13; // eax
	int v14; // eax
	_BYTE *v15; // esi
	_BYTE *v16; // edi
	int v17; // ecx
	int v18; // edx
	int v19; // ecx
	int v20; // eax
	_BYTE *v21; // [esp-4h] [ebp-14h]
	int v22; // [esp+Ch] [ebp-4h]
	char *cel_buf; // [esp+18h] [ebp+8h]
	char *cel_bufa; // [esp+18h] [ebp+8h]
	int framea; // [esp+1Ch] [ebp+Ch]
	char *always_0a; // [esp+24h] [ebp+14h]
	int directiona; // [esp+28h] [ebp+18h]

	v22 = screen_x;
	if ( gpBuffer )
	{
		v8 = pCelBuff;
		if ( pCelBuff )
		{
			v9 = *(_DWORD *)&pCelBuff[4 * frame];
			v10 = &pCelBuff[v9];
			v11 = *(unsigned short *)&pCelBuff[v9 + always_0];
			cel_buf = (char *)*(unsigned short *)&pCelBuff[v9 + always_0];
			if ( v11 )
			{
				v12 = *(_DWORD *)&v8[4 * frame + 4] - v9;
				if ( direction != 8 && *(_WORD *)&v10[direction] )
					framea = *(unsigned short *)&v10[direction] - (_DWORD)cel_buf;
				else
					framea = v12 - (_DWORD)cel_buf;
				directiona = (int)&v10[(_DWORD)cel_buf];
				always_0a = (char *)gpBuffer + screen_y_times_768[screen_y - 16 * always_0] + v22;
				v13 = -(light4flag != 0);
				_LOWORD(v13) = v13 & 0xF400;
				v14 = v13 + 4096;
				if ( always_1 == 2 )
					v14 += 256;
				if ( always_1 >= 4 )
					v14 = v14 + (always_1 << 8) - 256;
				cel_bufa = &pLightTbl[v14];
				v15 = (_BYTE *)directiona;
				v16 = (unsigned char *)always_0a;
				v17 = directiona + framea;
				do
				{
					v21 = (_BYTE *)v17;
					v18 = frame_width;
					v19 = 0;
					do
					{
						while ( 1 )
						{
							v20 = (unsigned char)*v15++;
							if ( (v20 & 0x80u) == 0 )
								break;
							_LOBYTE(v20) = -(char)v20;
							v16 += v20;
							v18 -= v20;
							if ( !v18 )
								goto LABEL_21;
						}
						v18 -= v20;
						if ( v16 < gpBufEnd )
						{
							do
							{
								_LOBYTE(v19) = *v15++;
								*v16 = cel_bufa[v19];
								--v20;
								++v16;
							}
							while ( v20 );
						}
						else
						{
							v15 += v20;
							v16 += v20;
						}
					}
					while ( v18 );
LABEL_21:
					v17 = (int)v21;
					v16 += -frame_width - 768;
				}
				while ( v21 != v15 );
			}
		}
	}
}
// 525728: using guessed type int light4flag;
// 69CF0C: using guessed type int gpBufEnd;

void __fastcall CelDecodeRect(char *pBuff, int always_0, int dst_height, int dst_width, char *pCelBuff, int frame, int frame_width)
{
	char *v7; // ebx
	char *v8; // esi
	char *v9; // edi
	int v10; // ebx
	int v11; // edx
	unsigned int v12; // eax
	unsigned int v13; // ecx
	char v14; // cf
	unsigned int v15; // ecx
	int dst_widtha; // [esp+14h] [ebp+Ch]

	if ( pCelBuff && pBuff )
	{
		v7 = &pCelBuff[4 * frame];
		v8 = &pCelBuff[*(_DWORD *)v7];
		v9 = &pBuff[dst_width * dst_height + always_0];
		dst_widtha = frame_width + dst_width;
		v10 = (int)&v8[*((_DWORD *)v7 + 1) - *(_DWORD *)v7];
		do
		{
			v11 = frame_width;
			do
			{
				while ( 1 )
				{
					v12 = (unsigned char)*v8++;
					if ( (v12 & 0x80u) == 0 )
						break;
					_LOBYTE(v12) = -(char)v12;
					v9 += v12;
					v11 -= v12;
					if ( !v11 )
						goto LABEL_14;
				}
				v11 -= v12;
				v13 = v12 >> 1;
				if ( v12 & 1 )
				{
					*v9++ = *v8++;
					if ( !v13 )
						continue;
				}
				v14 = v13 & 1;
				v15 = v12 >> 2;
				if ( v14 )
				{
					*(_WORD *)v9 = *(_WORD *)v8;
					v8 += 2;
					v9 += 2;
					if ( !v15 )
						continue;
				}
				qmemcpy(v9, v8, 4 * v15);
				v8 += 4 * v15;
				v9 += 4 * v15;
			}
			while ( v11 );
LABEL_14:
			v9 -= dst_widtha;
		}
		while ( (char *)v10 != v8 );
	}
}

void __fastcall CelDecodeClr(BYTE colour, int screen_x, int screen_y, char *pCelBuff, int frame, int frame_width, int a7, int direction)
{
	char *v8; // ebx
	int v9; // eax
	char *v10; // esi
	char *v11; // edi
	int v12; // edx
	int v13; // eax
	int v14; // ecx
	char v15; // al
	int v16; // [esp+Ch] [ebp-10h]
	char *v17; // [esp+10h] [ebp-Ch]
	int v18; // [esp+14h] [ebp-8h]
	char v19; // [esp+18h] [ebp-4h]

	v19 = colour;
	if ( pCelBuff )
	{
		if ( gpBuffer )
		{
			v8 = &pCelBuff[4 * frame];
			v17 = &pCelBuff[*(_DWORD *)v8];
			v16 = *(unsigned short *)&v17[a7];
			if ( *(_WORD *)&v17[a7] )
			{
				if ( direction == 8 )
					v9 = 0;
				else
					v9 = *(unsigned short *)&v17[direction];
				if ( v9 )
					v18 = v9 - v16;
				else
					v18 = *((_DWORD *)v8 + 1) - *(_DWORD *)v8 - v16;
				v10 = &v17[v16];
				v11 = (char *)gpBuffer + screen_y_times_768[screen_y - 16 * a7] + screen_x;
				do
				{
					v12 = frame_width;
					do
					{
						while ( 1 )
						{
							v13 = (unsigned char)*v10++;
							if ( (v13 & 0x80u) == 0 )
								break;
							_LOBYTE(v13) = -(char)v13;
							v11 += v13;
							v12 -= v13;
							if ( !v12 )
								goto LABEL_20;
						}
						v12 -= v13;
						v14 = v13;
						do
						{
							v15 = *v10++;
							if ( v15 )
							{
								*(v11 - 768) = v19;
								*(v11 - 1) = v19;
								v11[1] = v19;
								v11[768] = v19;
							}
							++v11;
							--v14;
						}
						while ( v14 );
					}
					while ( v12 );
LABEL_20:
					v11 += -frame_width - 768;
				}
				while ( &v17[v16 + v18] != v10 );
			}
		}
	}
}

void __fastcall CelDrawHdrClrHL(char colour, int screen_x, int screen_y, char *pCelBuff, int frame, int frame_width, int a7, int direction)
{
	char *v8; // ebx
	int v9; // eax
	char *v10; // esi
	char *v11; // edi
	int v12; // edx
	int v13; // eax
	int v14; // ecx
	char v15; // al
	int v16; // ecx
	char v17; // al
	int v18; // [esp+Ch] [ebp-10h]
	char *v19; // [esp+10h] [ebp-Ch]
	int v20; // [esp+14h] [ebp-8h]
	char v21; // [esp+18h] [ebp-4h]

	v21 = colour;
	if ( pCelBuff )
	{
		if ( gpBuffer )
		{
			v8 = &pCelBuff[4 * frame];
			v19 = &pCelBuff[*(_DWORD *)v8];
			v18 = *(unsigned short *)&v19[a7];
			if ( *(_WORD *)&v19[a7] )
			{
				if ( direction == 8 )
					v9 = 0;
				else
					v9 = *(unsigned short *)&v19[direction];
				if ( v9 )
					v20 = v9 - v18;
				else
					v20 = *((_DWORD *)v8 + 1) - *(_DWORD *)v8 - v18;
				v10 = &v19[v18];
				v11 = (char *)gpBuffer + screen_y_times_768[screen_y - 16 * a7] + screen_x;
				do
				{
					v12 = frame_width;
					do
					{
						while ( 1 )
						{
							v13 = (unsigned char)*v10++;
							if ( (v13 & 0x80u) == 0 )
								break;
							_LOBYTE(v13) = -(char)v13;
							v11 += v13;
							v12 -= v13;
							if ( !v12 )
								goto LABEL_28;
						}
						v12 -= v13;
						if ( v11 < (char *)gpBufEnd )
						{
							if ( v11 >= (char *)gpBufEnd - 768 )
							{
								v16 = v13;
								do
								{
									v17 = *v10++;
									if ( v17 )
									{
										*(v11 - 768) = v21;
										*(v11 - 1) = v21;
										v11[1] = v21;
									}
									++v11;
									--v16;
								}
								while ( v16 );
							}
							else
							{
								v14 = v13;
								do
								{
									v15 = *v10++;
									if ( v15 )
									{
										*(v11 - 768) = v21;
										*(v11 - 1) = v21;
										v11[1] = v21;
										v11[768] = v21;
									}
									++v11;
									--v14;
								}
								while ( v14 );
							}
						}
						else
						{
							v10 += v13;
							v11 += v13;
						}
					}
					while ( v12 );
LABEL_28:
					v11 += -frame_width - 768;
				}
				while ( &v19[v18 + v20] != v10 );
			}
		}
	}
}
// 69CF0C: using guessed type int gpBufEnd;

void __fastcall ENG_set_pixel(int screen_x, int screen_y, UCHAR pixel)
{
	UCHAR *v3; // edi

	if ( screen_y >= 0 && screen_y < 640 && screen_x >= 64 && screen_x < 704 )
	{
		v3 = (UCHAR *)gpBuffer + screen_y_times_768[screen_y] + screen_x;
		if ( v3 < gpBufEnd )
			*v3 = pixel;
	}
}
// 69CF0C: using guessed type int gpBufEnd;

void __fastcall engine_draw_pixel(int x, int y)
{
	_BYTE *v2; // eax

	if ( dword_52B970 )
	{
		if ( !dword_52B99C || x >= 0 && x < 640 && y >= 64 && y < 704 )
		{
			v2 = (unsigned char *)gpBuffer + screen_y_times_768[x] + y;
			goto LABEL_14;
		}
	}
	else if ( !dword_52B99C || y >= 0 && y < 640 && x >= 64 && x < 704 )
	{
		v2 = (unsigned char *)gpBuffer + screen_y_times_768[y] + x;
LABEL_14:
		if ( v2 < gpBufEnd )
			*v2 = gbPixelCol;
		return;
	}
}
// 52B96C: using guessed type char gbPixelCol;
// 52B970: using guessed type int dword_52B970;
// 52B99C: using guessed type int dword_52B99C;
// 69CF0C: using guessed type int gpBufEnd;

void __fastcall DrawLine(int x0, int y0, int x1, int y1, UCHAR col)
{
	int v5; // ST18_4
	int v6; // ST2C_4
	int v7; // ST20_4
	int v8; // [esp+Ch] [ebp-48h]
	int v9; // [esp+10h] [ebp-44h]
	int v10; // [esp+14h] [ebp-40h]
	int v11; // [esp+18h] [ebp-3Ch]
	signed int v12; // [esp+1Ch] [ebp-38h]
	int v13; // [esp+20h] [ebp-34h]
	int v14; // [esp+24h] [ebp-30h]
	int v15; // [esp+28h] [ebp-2Ch]
	int y; // [esp+2Ch] [ebp-28h]
	int ya; // [esp+2Ch] [ebp-28h]
	int yb; // [esp+2Ch] [ebp-28h]
	int yc; // [esp+2Ch] [ebp-28h]
	int j; // [esp+30h] [ebp-24h]
	int i; // [esp+30h] [ebp-24h]
	int x; // [esp+34h] [ebp-20h]
	int xa; // [esp+34h] [ebp-20h]
	int xb; // [esp+34h] [ebp-20h]
	int xc; // [esp+34h] [ebp-20h]
	int xd; // [esp+34h] [ebp-20h]
	int xe; // [esp+34h] [ebp-20h]
	int xf; // [esp+34h] [ebp-20h]
	int xg; // [esp+34h] [ebp-20h]
	int xh; // [esp+34h] [ebp-20h]
	int v31; // [esp+38h] [ebp-1Ch]
	int v32; // [esp+3Ch] [ebp-18h]
	int v33; // [esp+3Ch] [ebp-18h]
	int v34; // [esp+3Ch] [ebp-18h]
	signed int v35; // [esp+40h] [ebp-14h]
	signed int v36; // [esp+44h] [ebp-10h]
	int v37; // [esp+48h] [ebp-Ch]
	int v38; // [esp+48h] [ebp-Ch]
	int v39; // [esp+4Ch] [ebp-8h]
	int v40; // [esp+4Ch] [ebp-8h]
	int v41; // [esp+50h] [ebp-4h]
	int x2a; // [esp+5Ch] [ebp+8h]

	v8 = y0;
	v9 = x0;
	gbPixelCol = col;
	dword_52B99C = 0;
	if ( x0 < 64 || x0 >= 704 )
		dword_52B99C = 1;
	if ( x1 < 64 || x1 >= 704 )
		dword_52B99C = 1;
	if ( y0 < 160 || y0 >= 512 )
		dword_52B99C = 1;
	if ( y1 < 160 || y1 >= 512 )
		dword_52B99C = 1;
	if ( x1 - x0 < 0 )
		v36 = -1;
	else
		v36 = 1;
	v11 = v36 * (x1 - x0);
	if ( y1 - y0 < 0 )
		v35 = -1;
	else
		v35 = 1;
	v10 = v35 * (y1 - y0);
	if ( v35 == v36 )
		v12 = 1;
	else
		v12 = -1;
	if ( v11 >= v10 )
	{
		dword_52B970 = 0;
	}
	else
	{
		v8 = y0 ^ x0 ^ y0;
		v9 = v8 ^ y0 ^ x0;
		x2a = y1 ^ x1;
		y1 ^= x2a;
		x1 = y1 ^ x2a;
		v5 = v10 ^ v11;
		v10 ^= v5;
		v11 = v10 ^ v5;
		dword_52B970 = 1;
	}
	if ( x1 >= v9 )
	{
		x = v9;
		y = v8;
		v32 = x1;
		v13 = y1;
	}
	else
	{
		x = x1;
		y = y1;
		v32 = v9;
		v13 = v8;
	}
	v31 = (v11 - 1) / 4;
	v41 = (v11 - 1) % 4; /* (((v11 - 1) >> 31) ^ abs(v11 - 1) & 3) - ((v11 - 1) >> 31) */
	engine_draw_pixel(x, y);
	engine_draw_pixel(v32, v13);
	v14 = 4 * v10 - 2 * v11;
	if ( v14 >= 0 )
	{
		v40 = 2 * (v10 - v11);
		v15 = 4 * (v10 - v11);
		v38 = v15 + v11;
		for ( i = 0; i < v31; ++i )
		{
			xe = x + 1;
			v34 = v32 - 1;
			if ( v38 <= 0 )
			{
				if ( v40 <= v38 )
				{
					y += v12;
					engine_draw_pixel(xe, y);
					x = xe + 1;
					engine_draw_pixel(x, y);
					v13 -= v12;
					engine_draw_pixel(v34, v13);
				}
				else
				{
					engine_draw_pixel(xe, y);
					y += v12;
					x = xe + 1;
					engine_draw_pixel(x, y);
					engine_draw_pixel(v34, v13);
					v13 -= v12;
				}
				v32 = v34 - 1;
				engine_draw_pixel(v32, v13);
				v38 += v14;
			}
			else
			{
				v6 = v12 + y;
				engine_draw_pixel(xe, v6);
				y = v12 + v6;
				x = xe + 1;
				engine_draw_pixel(x, y);
				v7 = v13 - v12;
				engine_draw_pixel(v34, v7);
				v13 = v7 - v12;
				v32 = v34 - 1;
				engine_draw_pixel(v32, v13);
				v38 += v15;
			}
		}
		if ( v41 )
		{
			if ( v38 <= 0 )
			{
				if ( v40 <= v38 )
				{
					yc = v12 + y;
					xh = x + 1;
					engine_draw_pixel(xh, yc);
					if ( v41 > 1 )
						engine_draw_pixel(xh + 1, yc);
					if ( v41 > 2 )
					{
						if ( v40 >= v38 )
							engine_draw_pixel(v32 - 1, v13);
						else
							engine_draw_pixel(v32 - 1, v13 - v12);
					}
				}
				else
				{
					xg = x + 1;
					engine_draw_pixel(xg, y);
					if ( v41 > 1 )
						engine_draw_pixel(xg + 1, v12 + y);
					if ( v41 > 2 )
						engine_draw_pixel(v32 - 1, v13);
				}
			}
			else
			{
				yb = v12 + y;
				xf = x + 1;
				engine_draw_pixel(xf, yb);
				if ( v41 > 1 )
					engine_draw_pixel(xf + 1, v12 + yb);
				if ( v41 > 2 )
					engine_draw_pixel(v32 - 1, v13 - v12);
			}
		}
	}
	else
	{
		v39 = 2 * v10;
		v37 = 4 * v10 - v11;
		for ( j = 0; j < v31; ++j )
		{
			xa = x + 1;
			v33 = v32 - 1;
			if ( v37 >= 0 )
			{
				if ( v39 <= v37 )
				{
					y += v12;
					engine_draw_pixel(xa, y);
					x = xa + 1;
					engine_draw_pixel(x, y);
					v13 -= v12;
					engine_draw_pixel(v33, v13);
				}
				else
				{
					engine_draw_pixel(xa, y);
					y += v12;
					x = xa + 1;
					engine_draw_pixel(x, y);
					engine_draw_pixel(v33, v13);
					v13 -= v12;
				}
				v32 = v33 - 1;
				engine_draw_pixel(v32, v13);
				v37 += v14;
			}
			else
			{
				engine_draw_pixel(xa, y);
				x = xa + 1;
				engine_draw_pixel(x, y);
				engine_draw_pixel(v33, v13);
				v32 = v33 - 1;
				engine_draw_pixel(v32, v13);
				v37 += 4 * v10;
			}
		}
		if ( v41 )
		{
			if ( v37 >= 0 )
			{
				if ( v39 <= v37 )
				{
					ya = v12 + y;
					xd = x + 1;
					engine_draw_pixel(xd, ya);
					if ( v41 > 1 )
						engine_draw_pixel(xd + 1, ya);
					if ( v41 > 2 )
						engine_draw_pixel(v32 - 1, v13 - v12);
				}
				else
				{
					xc = x + 1;
					engine_draw_pixel(xc, y);
					if ( v41 > 1 )
						engine_draw_pixel(xc + 1, v12 + y);
					if ( v41 > 2 )
						engine_draw_pixel(v32 - 1, v13);
				}
			}
			else
			{
				xb = x + 1;
				engine_draw_pixel(xb, y);
				if ( v41 > 1 )
					engine_draw_pixel(xb + 1, y);
				if ( v41 > 2 )
					engine_draw_pixel(v32 - 1, v13);
			}
		}
	}
}
// 52B96C: using guessed type char gbPixelCol;
// 52B970: using guessed type int dword_52B970;
// 52B99C: using guessed type int dword_52B99C;

int __fastcall GetDirection(int x1, int y1, int x2, int y2)
{
	int v4; // esi
	int v5; // ecx
	int v6; // edx
	int result; // eax
	int v8; // esi
	int v9; // edx

	v4 = x2 - x1;
	v5 = y2 - y1;
	if ( v4 < 0 )
	{
		v8 = -v4;
		v9 = 2 * v8;
		if ( v5 < 0 )
		{
			v5 = -v5;
			result = 4;
			if ( v9 < v5 )
				result = 5;
		}
		else
		{
			result = 2;
			if ( v9 < v5 )
				result = 1;
		}
		if ( 2 * v5 < v8 )
			return 3;
	}
	else
	{
		v6 = 2 * v4;
		if ( v5 < 0 )
		{
			v5 = -v5;
			result = 6;
			if ( v6 < v5 )
				result = 5;
		}
		else
		{
			result = 0;
			if ( v6 < v5 )
				result = 1;
		}
		if ( 2 * v5 < v4 )
			return 7;
	}
	return result;
}

void __fastcall SetRndSeed(int s)
{
	SeedCount = 0;
	sglGameSeed = s;
	orgseed = s;
}
// 52B974: using guessed type int orgseed;
// 52B97C: using guessed type int sglGameSeed;
// 52B998: using guessed type int SeedCount;

int __cdecl GetRndSeed()
{
	++SeedCount;
	sglGameSeed = 0x015A4E35 * sglGameSeed + 1;
	return abs(sglGameSeed);
}
// 52B97C: using guessed type int sglGameSeed;
// 52B998: using guessed type int SeedCount;

int __fastcall random(BYTE idx, int v)
{
	if ( v <= 0 )
		return 0;
	if ( v >= 0xFFFF )
		return GetRndSeed() % v;
	return (GetRndSeed() >> 16) % v;
}

#ifndef MINIWIN
struct engine_cpp_init_2
{
	engine_cpp_init_2()
	{
		mem_init_mutex();
		mem_atexit_mutex();
	}
} _engine_cpp_init_2;
#endif

void __cdecl mem_init_mutex()
{
	InitializeCriticalSection(&sgMemCrit);
}

void __cdecl mem_atexit_mutex()
{
	atexit(mem_free_mutex);
}

void __cdecl mem_free_mutex()
{
	DeleteCriticalSection(&sgMemCrit);
}

unsigned char *__fastcall DiabloAllocPtr(int dwBytes)
{
	int v1; // ebx
	unsigned char *v2; // ebx
	int v3; // eax

	v1 = dwBytes;
	EnterCriticalSection(&sgMemCrit);
	v2 = (unsigned char *)SMemAlloc(v1, "C:\\Src\\Diablo\\Source\\ENGINE.CPP", 2236, 0);
	LeaveCriticalSection(&sgMemCrit);
	if ( !v2 )
	{
		v3 = GetLastError();
		ErrDlg(IDD_DIALOG2, v3, "C:\\Src\\Diablo\\Source\\ENGINE.CPP", 2269);
	}
	return v2;
}

void __fastcall mem_free_dbg(void *p)
{
	void *v1; // edi

	v1 = p;
	if ( p )
	{
		EnterCriticalSection(&sgMemCrit);
		SMemFree(v1, "C:\\Src\\Diablo\\Source\\ENGINE.CPP", 2317, 0);
		LeaveCriticalSection(&sgMemCrit);
	}
}

unsigned char *__fastcall LoadFileInMem(char *pszName, int *pdwFileLen)
{
	int *v2; // edi
	char *v3; // ebx
	int v4; // eax
	int v5; // esi
	char *v6; // edi
	void *a1; // [esp+Ch] [ebp-4h]

	v2 = pdwFileLen;
	v3 = pszName;
	WOpenFile(pszName, &a1, 0);
	v4 = WGetFileSize(a1, 0);
	v5 = v4;
	if ( v2 )
		*v2 = v4;
	if ( !v4 )
		TermMsg("Zero length SFILE:\n%s", v3);
	v6 = (char *)DiabloAllocPtr(v5);
	WReadFile(a1, v6, v5);
	WCloseFile(a1);
	return (unsigned char *)v6;
}

void __fastcall LoadFileWithMem(char *pszName, void *buf)
{
	char *v2; // ebx
	char *v3; // edi
	int v4; // esi
	void *a1; // [esp+Ch] [ebp-4h]

	v2 = (char *)buf;
	v3 = pszName;
	if ( !buf )
		TermMsg("LoadFileWithMem(NULL):\n%s", pszName);
	WOpenFile(v3, &a1, 0);
	v4 = WGetFileSize(a1, 0);
	if ( !v4 )
		TermMsg("Zero length SFILE:\n%s", v3);
	WReadFile(a1, v2, v4);
	WCloseFile(a1);
}

void __fastcall Cl2ApplyTrans(unsigned char *p, unsigned char *ttbl, int last_frame)
{
	int v3; // eax
	int v4; // edi
	int v5; // esi
	unsigned char *v6; // eax
	char v7; // bl
	unsigned char v8; // bl
	int v9; // edi
	int i; // [esp+0h] [ebp-4h]

	v3 = 1;
	for ( i = 1; i <= last_frame; ++i )
	{
		v4 = *(_DWORD *)&p[4 * v3];
		v5 = *(_DWORD *)&p[4 * v3 + 4] - v4 - 10;
		v6 = &p[v4 + 10];
		while ( v5 )
		{
			v7 = *v6++;
			--v5;
			if ( v7 < 0 )
			{
				v8 = -v7;
				if ( (char)v8 <= 65 )
				{
					v5 -= (char)v8;
					if ( v8 )
					{
						v9 = v8;
						do
						{
							*v6 = ttbl[*v6];
							++v6;
							--v9;
						}
						while ( v9 );
					}
				}
				else
				{
					--v5;
					*v6 = ttbl[*v6];
					++v6;
				}
			}
		}
		v3 = i + 1;
	}
}

void __fastcall Cl2DecodeFrm1(int x, int y, char *pCelBuff, int nCel, int width, int dir1, int dir2)
{
	char *v8; // edx
	char *v9; // ecx
	int v10; // ecx
	int v11; // eax
	char *pCelBuffa; // [esp+18h] [ebp+8h]

	if ( gpBuffer )
	{
		v8 = pCelBuff;
		if ( pCelBuff )
		{
			if ( nCel > 0 )
			{
				v9 = *(char **)&pCelBuff[4 * nCel];
				pCelBuffa = v9;
				v10 = (int)&v9[(_DWORD)v8];
				if ( *(_WORD *)(v10 + dir1) )
				{
					if ( dir2 == 8 || (v11 = *(unsigned short *)(v10 + dir2), !*(_WORD *)(v10 + dir2)) )
						v11 = *((_DWORD *)v8 + nCel + 1) - (_DWORD)pCelBuffa;
					Cl2DecDatFrm1(
						(char *)gpBuffer + screen_y_times_768[y - 16 * dir1] + x,
						(char *)(*(unsigned short *)(v10 + dir1) + v10),
						v11 - *(unsigned short *)(v10 + dir1),
						width);
				}
			}
		}
	}
}

void __fastcall Cl2DecDatFrm1(char *buffer, char *frame_content, int a3, int width) /* fix */
{
	char *v4; // esi
	char *v5; // edi
	int v6; // eax
	int v7; // ebx
	int v8; // ecx
	char v9; // dl
	char v10; // dl
	int v11; // edx

	v4 = frame_content;
	v5 = buffer;
	v6 = 0;
	v7 = width;
	v8 = a3;
	do
	{
		_LOBYTE(v6) = *v4++;
		--v8;
		if ( (v6 & 0x80u) == 0 )
		{
			do
			{
				if ( v6 <= v7 )
				{
					v11 = v6;
					v5 += v6;
					v6 = 0;
				}
				else
				{
					v11 = v7;
					v5 += v7;
					v6 -= v7;
				}
				v7 -= v11;
				if ( !v7 )
				{
					v7 = width;
					v5 = &v5[-width - 768];
				}
			}
			while ( v6 );
		}
		else
		{
			_LOBYTE(v6) = -(char)v6;
			if ( (char)v6 <= 65 )
			{
				v8 -= v6;
				v7 -= v6;
				do
				{
					v10 = *v4++;
					*v5 = v10;
					--v6;
					++v5;
				}
				while ( v6 );
			}
			else
			{
				_LOBYTE(v6) = v6 - 65;
				--v8;
				v9 = *v4++;
				v7 -= v6;
				do
				{
					*v5 = v9;
					--v6;
					++v5;
				}
				while ( v6 );
			}
			if ( !v7 )
			{
				v7 = width;
				v5 = &v5[-width - 768];
			}
		}
	}
	while ( v8 );
}

void __fastcall Cl2DecodeFrm2(char colour, int screen_x, int screen_y, char *pCelBuff, int nCel, int frame_width, int a7, int a8)
{
	int v8; // ebx
	char *v9; // edx
	int v10; // eax
	int v11; // [esp+Ch] [ebp-8h]

	v11 = screen_x;
	if ( gpBuffer )
	{
		if ( pCelBuff )
		{
			if ( nCel > 0 )
			{
				v8 = *(_DWORD *)&pCelBuff[4 * nCel];
				v9 = &pCelBuff[v8];
				if ( *(_WORD *)&pCelBuff[v8 + a7] )
				{
					if ( a8 == 8 || (v10 = *(unsigned short *)&v9[a8], !*(_WORD *)&v9[a8]) )
						v10 = *(_DWORD *)&pCelBuff[4 * nCel + 4] - v8;
					Cl2DecDatFrm2(
						(char *)gpBuffer + screen_y_times_768[screen_y - 16 * a7] + v11,
						&v9[*(unsigned short *)&pCelBuff[v8 + a7]],
						v10 - *(unsigned short *)&pCelBuff[v8 + a7],
						frame_width,
						colour);
				}
			}
		}
	}
}

void __fastcall Cl2DecDatFrm2(char *buffer, char *frame_content, int a3, int frame_width, char colour)
{
	char *v5; // esi
	char *v6; // edi
	int v7; // eax
	int v8; // ebx
	int v9; // ecx
	char v10; // dl
	char v11; // dh
	char v12; // dh
	int v13; // edx

	v5 = frame_content;
	v6 = buffer;
	v7 = 0;
	v8 = frame_width;
	v9 = a3;
	v10 = colour;
	do
	{
		_LOBYTE(v7) = *v5++;
		--v9;
		if ( (v7 & 0x80u) != 0 )
		{
			_LOBYTE(v7) = -(char)v7;
			if ( (char)v7 <= 65 )
			{
				v9 -= v7;
				v8 -= v7;
				do
				{
					v12 = *v5++;
					if ( v12 )
					{
						*(v6 - 1) = v10;
						v6[1] = v10;
						*(v6 - 768) = v10;
						v6[768] = v10;
					}
					--v7;
					++v6;
				}
				while ( v7 );
				goto LABEL_12;
			}
			_LOBYTE(v7) = v7 - 65;
			--v9;
			v11 = *v5++;
			if ( v11 )
			{
				*(v6 - 1) = v10;
				v8 -= v7;
				v6[v7] = v10;
				do
				{
					*(v6 - 768) = v10;
					v6[768] = v10;
					--v7;
					++v6;
				}
				while ( v7 );
LABEL_12:
				if ( !v8 )
				{
					v8 = frame_width;
					v6 = &v6[-frame_width - 768];
				}
				continue;
			}
		}
		do
		{
			if ( v7 <= v8 )
			{
				v13 = v7;
				v6 += v7;
				v7 = 0;
			}
			else
			{
				v13 = v8;
				v6 += v8;
				v7 -= v8;
			}
			v8 -= v13;
			if ( !v8 )
			{
				v8 = frame_width;
				v6 = &v6[-frame_width - 768];
			}
		}
		while ( v7 );
		v10 = colour;
	}
	while ( v9 );
}

void __fastcall Cl2DecodeFrm3(int screen_x, int screen_y, char *pCelBuff, int nCel, int frame_width, int a6, int a7, char a8)
{
	char *v8; // edi
	int v9; // ebx
	char *v10; // esi
	int v11; // eax
	int v12; // eax
	char *v13; // esi
	int v14; // edi
	int v15; // eax
	int v16; // eax
	char *pCelBuffa; // [esp+18h] [ebp+8h]

	if ( gpBuffer )
	{
		v8 = pCelBuff;
		if ( pCelBuff )
		{
			if ( nCel > 0 )
			{
				v9 = *(_DWORD *)&pCelBuff[4 * nCel];
				v10 = &pCelBuff[v9];
				v11 = *(unsigned short *)&pCelBuff[v9 + a6];
				pCelBuffa = (char *)*(unsigned short *)&pCelBuff[v9 + a6];
				if ( v11 )
				{
					if ( a7 == 8 || (v12 = *(unsigned short *)&v10[a7], !*(_WORD *)&v10[a7]) )
						v12 = *(_DWORD *)&v8[4 * nCel + 4] - v9;
					v13 = &v10[(_DWORD)pCelBuffa];
					v14 = v12 - (_DWORD)pCelBuffa;
					v15 = -(light4flag != 0);
					_LOWORD(v15) = v15 & 0xF400;
					v16 = v15 + 4096;
					if ( a8 == 2 )
						v16 += 256;
					if ( a8 >= 4 )
						v16 = v16 + (a8 << 8) - 256;
					Cl2DecDatLightTbl1(
						(char *)gpBuffer + screen_y_times_768[screen_y - 16 * a6] + screen_x,
						v13,
						v14,
						frame_width,
						&pLightTbl[v16]);
				}
			}
		}
	}
}
// 525728: using guessed type int light4flag;

void __fastcall Cl2DecDatLightTbl1(char *a1, char *a2, int a3, int a4, char *unused_lindex) /* check 5th arg */
{
	char *v5; // esi
	char *v6; // edi
	int v7; // ebx
	int v8; // ecx
	int v9; // eax
	int v10; // edx
	char v11; // dl

	v5 = a2;
	v6 = a1;
	v7 = a4;
	v8 = a3;
	sgnWidth = a4;
	v9 = 0;
	v10 = 0;
	do
	{
		_LOBYTE(v9) = *v5++;
		--v8;
		if ( (v9 & 0x80u) == 0 )
		{
			do
			{
				if ( v9 <= v7 )
				{
					v10 = v9;
					v6 += v9;
					v9 = 0;
				}
				else
				{
					v10 = v7;
					v6 += v7;
					v9 -= v7;
				}
				v7 -= v10;
				if ( !v7 )
				{
					v7 = sgnWidth;
					v6 = &v6[-sgnWidth - 768];
				}
			}
			while ( v9 );
		}
		else
		{
			_LOBYTE(v9) = -(char)v9;
			if ( (char)v9 <= 65 )
			{
				v8 -= v9;
				v7 -= v9;
				do
				{
					_LOBYTE(v10) = *v5++;
					*v6 = unused_lindex[v10];
					--v9;
					++v6;
				}
				while ( v9 );
			}
			else
			{
				_LOBYTE(v9) = v9 - 65;
				--v8;
				v7 -= v9;
				_LOBYTE(v10) = *v5++;
				v11 = unused_lindex[v10];
				do
				{
					*v6 = v11;
					--v9;
					++v6;
				}
				while ( v9 );
			}
			if ( !v7 )
			{
				v7 = sgnWidth;
				v6 = &v6[-sgnWidth - 768];
			}
		}
	}
	while ( v8 );
}
// 52B978: using guessed type int sgnWidth;

void __fastcall Cl2DecodeLightTbl(int screen_x, int screen_y, char *pCelBuff, int nCel, int frame_width, int a6, int a7)
{
	int v7; // esi
	char *v8; // edi
	int v9; // ebx
	char *v10; // edx
	int v11; // eax
	int v12; // eax
	int v13; // eax
	char *v14; // edx
	char *v15; // ecx
	char *pCelBuffa; // [esp+18h] [ebp+8h]

	v7 = screen_y;
	if ( gpBuffer )
	{
		v8 = pCelBuff;
		if ( pCelBuff )
		{
			if ( nCel > 0 )
			{
				v9 = *(_DWORD *)&pCelBuff[4 * nCel];
				v10 = &pCelBuff[v9];
				v11 = *(unsigned short *)&pCelBuff[v9 + a6];
				pCelBuffa = (char *)*(unsigned short *)&pCelBuff[v9 + a6];
				if ( v11 )
				{
					if ( a7 == 8 || (v12 = *(unsigned short *)&v10[a7], !*(_WORD *)&v10[a7]) )
						v12 = *(_DWORD *)&v8[4 * nCel + 4] - v9;
					v13 = v12 - (_DWORD)pCelBuffa;
					v14 = &v10[(_DWORD)pCelBuffa];
					v15 = (char *)gpBuffer + screen_y_times_768[v7 - 16 * a6] + screen_x;
					if ( light_table_index )
						Cl2DecDatLightTbl1(
							v15,
							v14,
							v13,
							frame_width,
							&pLightTbl[256 * light_table_index]);
					else
						Cl2DecDatFrm1(v15, v14, v13, frame_width);
				}
			}
		}
	}
}
// 69BEF8: using guessed type int light_table_index;

void __fastcall Cl2DecodeFrm4(int screen_x, int screen_y, char *pCelBuff, int nCel, int frame_width, int a6, int a7)
{
	int v7; // ebx
	char *v8; // edx
	char *v9; // ecx
	int v10; // ecx
	int v11; // eax
	int v12; // [esp+Ch] [ebp-4h]
	char *pCelBuffa; // [esp+18h] [ebp+8h]

	v7 = screen_y;
	v12 = screen_x;
	if ( gpBuffer )
	{
		v8 = pCelBuff;
		if ( pCelBuff )
		{
			if ( nCel > 0 )
			{
				v9 = *(char **)&pCelBuff[4 * nCel];
				pCelBuffa = v9;
				v10 = (int)&v9[(_DWORD)v8];
				if ( *(_WORD *)(v10 + a6) )
				{
					if ( a7 == 8 || (v11 = *(unsigned short *)(v10 + a7), !*(_WORD *)(v10 + a7)) )
						v11 = *(_DWORD *)&v8[4 * nCel + 4] - (_DWORD)pCelBuffa;
					Cl2DecDatFrm4(
						(char *)gpBuffer + screen_y_times_768[v7 - 16 * a6] + v12,
						(char *)(*(unsigned short *)(v10 + a6) + v10),
						v11 - *(unsigned short *)(v10 + a6),
						frame_width);
				}
			}
		}
	}
}

void __fastcall Cl2DecDatFrm4(char *buffer, char *a2, int a3, int frame_width)
{
	char *v4; // esi
	char *v5; // edi
	int v6; // eax
	int v7; // ebx
	int v8; // ecx
	char v9; // dl
	char v10; // dl
	int v11; // edx

	v4 = a2;
	v5 = buffer;
	v6 = 0;
	v7 = frame_width;
	v8 = a3;
	do
	{
		_LOBYTE(v6) = *v4++;
		--v8;
		if ( (v6 & 0x80u) != 0 )
		{
			_LOBYTE(v6) = -(char)v6;
			if ( (char)v6 <= 65 )
			{
				v8 -= v6;
				if ( v5 < (char *)gpBufEnd )
				{
					v7 -= v6;
					do
					{
						v10 = *v4++;
						*v5 = v10;
						--v6;
						++v5;
					}
					while ( v6 );
					goto LABEL_12;
				}
				v4 += v6;
			}
			else
			{
				_LOBYTE(v6) = v6 - 65;
				--v8;
				v9 = *v4++;
				if ( v5 < (char *)gpBufEnd )
				{
					v7 -= v6;
					do
					{
						*v5 = v9;
						--v6;
						++v5;
					}
					while ( v6 );
LABEL_12:
					if ( !v7 )
					{
						v7 = frame_width;
						v5 = &v5[-frame_width - 768];
					}
					continue;
				}
			}
		}
		do
		{
			if ( v6 <= v7 )
			{
				v11 = v6;
				v5 += v6;
				v6 = 0;
			}
			else
			{
				v11 = v7;
				v5 += v7;
				v6 -= v7;
			}
			v7 -= v11;
			if ( !v7 )
			{
				v7 = frame_width;
				v5 = &v5[-frame_width - 768];
			}
		}
		while ( v6 );
	}
	while ( v8 );
}
// 69CF0C: using guessed type int gpBufEnd;

void __fastcall Cl2DecodeClrHL(char colour, int screen_x, int screen_y, char *pCelBuff, int nCel, int frame_width, int a7, int a8)
{
	int v8; // ebx
	char *v9; // edx
	int v10; // ecx
	int v11; // eax
	int v12; // [esp+Ch] [ebp-8h]
	char a5; // [esp+10h] [ebp-4h]

	v12 = screen_x;
	a5 = colour;
	if ( gpBuffer )
	{
		if ( pCelBuff )
		{
			if ( nCel > 0 )
			{
				v8 = *(_DWORD *)&pCelBuff[4 * nCel];
				v9 = &pCelBuff[v8];
				v10 = *(unsigned short *)&pCelBuff[v8 + a7];
				if ( *(_WORD *)&pCelBuff[v8 + a7] )
				{
					if ( a8 == 8 || (v11 = *(unsigned short *)&v9[a8], !*(_WORD *)&v9[a8]) )
						v11 = *(_DWORD *)&pCelBuff[4 * nCel + 4] - v8;
					gpBufEnd -= 768;
					Cl2DecDatClrHL(
						(char *)gpBuffer + screen_y_times_768[screen_y - 16 * a7] + v12,
						&v9[v10],
						v11 - v10,
						frame_width,
						a5);
					gpBufEnd += 768;
				}
			}
		}
	}
}
// 69CF0C: using guessed type int gpBufEnd;

void __fastcall Cl2DecDatClrHL(char *dst_buf, char *frame_content, int a3, int frame_width, char colour)
{
	char *v5; // esi
	char *v6; // edi
	int v7; // eax
	int v8; // ebx
	int v9; // ecx
	char v10; // dl
	char v11; // dh
	char v12; // dh
	int v13; // edx

	v5 = frame_content;
	v6 = dst_buf;
	v7 = 0;
	v8 = frame_width;
	v9 = a3;
	v10 = colour;
	do
	{
		_LOBYTE(v7) = *v5++;
		--v9;
		if ( (v7 & 0x80u) != 0 )
		{
			_LOBYTE(v7) = -(char)v7;
			if ( (char)v7 <= 65 )
			{
				v9 -= v7;
				if ( v6 < (char *)gpBufEnd )
				{
					v8 -= v7;
					do
					{
						v12 = *v5++;
						if ( v12 )
						{
							*(v6 - 1) = v10;
							v6[1] = v10;
							*(v6 - 768) = v10;
							v6[768] = v10;
						}
						--v7;
						++v6;
					}
					while ( v7 );
					goto LABEL_15;
				}
				v5 += v7;
			}
			else
			{
				_LOBYTE(v7) = v7 - 65;
				--v9;
				v11 = *v5++;
				if ( v11 && v6 < (char *)gpBufEnd )
				{
					*(v6 - 1) = v10;
					v8 -= v7;
					v6[v7] = v10;
					do
					{
						*(v6 - 768) = v10;
						v6[768] = v10;
						--v7;
						++v6;
					}
					while ( v7 );
LABEL_15:
					if ( !v8 )
					{
						v8 = frame_width;
						v6 = &v6[-frame_width - 768];
					}
					continue;
				}
			}
		}
		do
		{
			if ( v7 <= v8 )
			{
				v13 = v7;
				v6 += v7;
				v7 = 0;
			}
			else
			{
				v13 = v8;
				v6 += v8;
				v7 -= v8;
			}
			v8 -= v13;
			if ( !v8 )
			{
				v8 = frame_width;
				v6 = &v6[-frame_width - 768];
			}
		}
		while ( v7 );
		v10 = colour;
	}
	while ( v9 );
}
// 69CF0C: using guessed type int gpBufEnd;

void __fastcall Cl2DecodeFrm5(int screen_x, int screen_y, char *pCelBuff, int nCel, int frame_width, int a6, int a7, char a8)
{
	char *v8; // edi
	int v9; // ebx
	char *v10; // esi
	int v11; // eax
	int v12; // eax
	char *v13; // esi
	int v14; // edi
	int v15; // eax
	int v16; // eax
	char *pCelBuffa; // [esp+18h] [ebp+8h]

	if ( gpBuffer )
	{
		v8 = pCelBuff;
		if ( pCelBuff )
		{
			if ( nCel > 0 )
			{
				v9 = *(_DWORD *)&pCelBuff[4 * nCel];
				v10 = &pCelBuff[v9];
				v11 = *(unsigned __int16 *)&pCelBuff[v9 + a6];
				pCelBuffa = (char *)*(unsigned __int16 *)&pCelBuff[v9 + a6];
				if ( v11 )
				{
					if ( a7 == 8 || (v12 = *(unsigned __int16 *)&v10[a7], !*(_WORD *)&v10[a7]) )
						v12 = *(_DWORD *)&v8[4 * nCel + 4] - v9;
					v13 = &v10[(_DWORD)pCelBuffa];
					v14 = v12 - (_DWORD)pCelBuffa;
					v15 = -(light4flag != 0);
					_LOWORD(v15) = v15 & 0xF400;
					v16 = v15 + 4096;
					if ( a8 == 2 )
						v16 += 256;
					if ( a8 >= 4 )
						v16 = v16 + (a8 << 8) - 256;
					Cl2DecDatLightTbl2(
						(char *)gpBuffer + screen_y_times_768[screen_y - 16 * a6] + screen_x,
						v13,
						v14,
						frame_width,
						&pLightTbl[v16]);
				}
			}
		}
	}
}
// 525728: using guessed type int light4flag;

void __fastcall Cl2DecDatLightTbl2(char *dst_buf, char *a2, int a3, int frame_width, char *a5) /* check 5th arg */
{
	char *v5; // esi
	char *v6; // edi
	int v7; // ebx
	int v8; // ecx
	int v9; // eax
	int v10; // edx
	char v11; // dl

	v5 = a2;
	v6 = dst_buf;
	v7 = frame_width;
	v8 = a3;
	sgnWidth = frame_width;
	v9 = 0;
	v10 = 0;
	do
	{
		_LOBYTE(v9) = *v5++;
		--v8;
		if ( (v9 & 0x80u) != 0 )
		{
			_LOBYTE(v9) = -(char)v9;
			if ( (char)v9 <= 65 )
			{
				v8 -= v9;
				if ( v6 < (char *)gpBufEnd )
				{
					v7 -= v9;
					do
					{
						_LOBYTE(v10) = *v5++;
						*v6 = a5[v10];
						--v9;
						++v6;
					}
					while ( v9 );
					goto LABEL_12;
				}
				v5 += v9;
			}
			else
			{
				_LOBYTE(v9) = v9 - 65;
				--v8;
				_LOBYTE(v10) = *v5++;
				v11 = a5[v10];
				if ( v6 < (char *)gpBufEnd )
				{
					v7 -= v9;
					do
					{
						*v6 = v11;
						--v9;
						++v6;
					}
					while ( v9 );
LABEL_12:
					if ( !v7 )
					{
						v7 = sgnWidth;
						v6 = &v6[-sgnWidth - 768];
					}
					continue;
				}
			}
		}
		do
		{
			if ( v9 <= v7 )
			{
				v10 = v9;
				v6 += v9;
				v9 = 0;
			}
			else
			{
				v10 = v7;
				v6 += v7;
				v9 -= v7;
			}
			v7 -= v10;
			if ( !v7 )
			{
				v7 = sgnWidth;
				v6 = &v6[-sgnWidth - 768];
			}
		}
		while ( v9 );
	}
	while ( v8 );
}
// 52B978: using guessed type int sgnWidth;
// 69CF0C: using guessed type int gpBufEnd;

void __fastcall Cl2DecodeFrm6(int screen_x, int screen_y, char *pCelBuff, int nCel, int frame_width, int a6, int a7)
{
	int v7; // esi
	char *v8; // edi
	int v9; // ebx
	char *v10; // edx
	int v11; // eax
	int v12; // eax
	int v13; // eax
	char *v14; // edx
	char *v15; // ecx
	char *pCelBuffa; // [esp+18h] [ebp+8h]

	v7 = screen_y;
	if ( gpBuffer )
	{
		v8 = pCelBuff;
		if ( pCelBuff )
		{
			if ( nCel > 0 )
			{
				v9 = *(_DWORD *)&pCelBuff[4 * nCel];
				v10 = &pCelBuff[v9];
				v11 = *(unsigned short *)&pCelBuff[v9 + a6];
				pCelBuffa = (char *)*(unsigned short *)&pCelBuff[v9 + a6];
				if ( v11 )
				{
					if ( a7 == 8 || (v12 = *(unsigned short *)&v10[a7], !*(_WORD *)&v10[a7]) )
						v12 = *(_DWORD *)&v8[4 * nCel + 4] - v9;
					v13 = v12 - (_DWORD)pCelBuffa;
					v14 = &v10[(_DWORD)pCelBuffa];
					v15 = (char *)gpBuffer + screen_y_times_768[v7 - 16 * a6] + screen_x;
					if ( light_table_index )
						Cl2DecDatLightTbl2(v15, v14, v13, frame_width, &pLightTbl[256 * light_table_index]);
					else
						Cl2DecDatFrm4(v15, v14, v13, frame_width);
				}
			}
		}
	}
}
// 69BEF8: using guessed type int light_table_index;

void __fastcall PlayInGameMovie(char *pszMovie)
{
	char *v1; // esi

	v1 = pszMovie;
	PaletteFadeOut(8);
	play_movie(v1, 0);
	ClearScreenBuffer();
	drawpanflag = 255;
	scrollrt_draw_game_screen(1);
	PaletteFadeIn(8);
	drawpanflag = 255;
}
// 52571C: using guessed type int drawpanflag;
