//HEADER_GOES_HERE

#include "../types.h"

#ifndef NO_GLOBALS
int spurtndx; // weak
DeadStruct dead[MAXDEAD];
int stonendx;
#endif

void __cdecl InitDead()
{
	int v0; // ebx
	int *v1; // edx
	int *v2; // eax
	int v3; // ecx
	int v4; // edx
	int v5; // eax
	int v6; // edx
	int v7; // esi
	int *v8; // eax
	int v9; // edx
	CMonster *v10; // ecx
	char *v11; // edi
	int *v12; // ebx
	int mtypes[MAXMONSTERS]; // [esp+Ch] [ebp-330h]
	int *v14; // [esp+32Ch] [ebp-10h]
	int *v15; // [esp+330h] [ebp-Ch]
	int v16; // [esp+334h] [ebp-8h]
	int v17; // [esp+338h] [ebp-4h]

	memset(mtypes, 0, sizeof(mtypes));
	v0 = 0;
	if ( nummtypes > 0 )
	{
		v1 = &dead[0]._deadFrame;
		v2 = &Monsters[0].Anims[4].Rate;
		v17 = nummtypes;
		do
		{
			v15 = &mtypes[*((unsigned char *)v2 - 216)];
			if ( !*v15 )
			{
				qmemcpy(v1 - 8, v2 - 8, 0x20u);
				v3 = *v2;
				*((_BYTE *)v1 + 12) = 0;
				*v1 = v3;
				v1[1] = v2[21];
				v1[2] = v2[22];
				*((_BYTE *)v2 + 101) = ++v0;
				v1 += 12;
				*v15 = v0;
			}
			v2 += 82;
			--v17;
		}
		while ( v17 );
	}
	v16 = 0;
	v4 = v0;
	memset32(&dead[v0], (unsigned int)misfiledata[16].mAnimData[0], 8u);
	_LOBYTE(dead[v4]._deadtrans) = 0;
	dead[v4]._deadFrame = 8;
	v5 = (unsigned int)misfiledata[18].mAnimData[0];
	dead[v4]._deadWidth = 128;
	dead[v4]._deadWidth2 = 32;
	v6 = v0 + 1;
	spurtndx = v0 + 1;
	memset32(&dead[v6], v5, 8u);
	_LOBYTE(dead[v6]._deadtrans) = 0;
	stonendx = v0 + 2;
	v7 = nummonsters;
	dead[v6]._deadFrame = 12;
	dead[v6]._deadWidth = 128;
	dead[v6]._deadWidth2 = 32;
	v17 = v0 + 2;
	if ( v7 > 0 )
	{
		v8 = &dead[v0 + 2]._deadFrame;
		do
		{
			v9 = monstactive[v16];
			if ( monster[v9]._uniqtype )
			{
				v10 = monster[v9].MType;
				v11 = (char *)(v8 - 8);
				v15 = (int *)8;
				v14 = (int *)v10->Anims[4].Frames;
				do
				{
					v12 = v14;
					++v14;
					*(_DWORD *)v11 = *v12;
					v11 += 4;
					v15 = (int *)((char *)v15 - 1);
				}
				while ( v15 );
				*v8 = v10->Anims[4].Rate;
				v8[1] = v10->flags_1;
				v8[2] = v10->flags_2;
				*((_BYTE *)v8 + 12) = monster[v9]._uniqtrans + 4;
				monster[v9]._udeadval = ++v17;
				v8 += 12;
			}
			++v16;
		}
		while ( v16 < v7 );
	}
}
// 4B8CD8: using guessed type int spurtndx;

void __fastcall AddDead(int dx, int dy, char dv, int ddir)
{
	dDead[dx][dy] = (dv & 0x1F) + 32 * ddir;
}

void __cdecl SetDead()
{
	int v0; // eax
	int v1; // esi
	int v2; // ebp
	char (*v3)[112]; // ebx
	int v4; // edi
	int i; // [esp+0h] [ebp-4h]

	v0 = 0;
	for ( i = 0; i < nummonsters; ++i )
	{
		v1 = monstactive[v0];
		if ( monster[v1]._uniqtype )
		{
			v2 = 0;
			v3 = dDead;
			do
			{
				v4 = 0;
				do
				{
					if ( ((*v3)[v4] & 0x1F) == monster[v1]._udeadval )
						ChangeLightXY((unsigned char)monster[v1].mlid, v2, v4);
					++v4;
				}
				while ( v4 < 112 );
				++v3;
				++v2;
			}
			while ( (signed int)v3 < (signed int)&dDead[112][0] );
		}
		v0 = i + 1;
	}
}
