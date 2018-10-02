//HEADER_GOES_HERE

#include "../types.h"

#ifdef MINIWIN
#define srand srand_miniwin
#define rand rand_miniwin
#endif

int __fastcall codec_decode(void *pbSrcDst, int size, char *pszPassword)
{
	unsigned int v3; // ebx
	char *v4; // esi
	int v5; // ebx
	signed int v7; // ecx
	int v8; // esi
	char v9[128]; // [esp+8h] [ebp-98h]
	char dst[20]; // [esp+88h] [ebp-18h]
	int v11; // [esp+9Ch] [ebp-4h]
	char *passworda; // [esp+A8h] [ebp+8h]

	v3 = size;
	v4 = (char *)pbSrcDst;
	codec_init_key(0, pszPassword);
	if ( v3 <= 8 )
		return 0;
	v5 = v3 - 8;
	v11 = v5;
	if ( v5 & 0x3F )
		return 0;
	passworda = (char *)v5;
	if ( v5 )
	{
		do
		{
			memcpy(v9, v4, 0x40u);
			SHA1Result(0, dst);
			v7 = 0;
			do
			{
				v9[v7] ^= dst[v7 % 20];
				++v7;
			}
			while ( v7 < 64 );
			SHA1Calculate(0, v9, 0);
			memset(dst, 0, sizeof(dst));
			memcpy(v4, v9, 0x40u);
			v4 += 64;
			passworda -= 64;
		}
		while ( passworda );
		v5 = v11;
	}
	memset(v9, 0, sizeof(v9));
	if ( !v4[4] )
	{
		SHA1Result(0, dst);
		if ( *(_DWORD *)v4 == *(_DWORD *)dst )
		{
			v8 = v5 + (unsigned char)v4[5] - 64;
			goto LABEL_14;
		}
		memset(dst, 0, sizeof(dst));
	}
	v8 = 0;
LABEL_14:
	SHA1Clear();
	return v8;
}
// 4034D9: using guessed type char var_98[128];

void __fastcall codec_init_key(int unused, char *pszPassword)
{
	char *v2; // edi
	char *v3; // esi
	int v4; // eax
	signed int v5; // ecx
	char v6; // dl
	unsigned int v7; // ecx
	signed int v8; // esi
	char v9[136]; // [esp+Ch] [ebp-E0h]
	char v10[64]; // [esp+94h] [ebp-58h]
	char dst[20]; // [esp+D4h] [ebp-18h]
	int v12; // [esp+E8h] [ebp-4h]

	v2 = pszPassword;
	srand(0x7058u);
	v3 = v9;
	v12 = 136;
	do
	{
		*v3++ = rand();
		--v12;
	}
	while ( v12 );
	v4 = 0;
	v5 = 0;
	do
	{
		if ( !v2[v4] )
			v4 = 0;
		v6 = v2[v4++];
		v10[v5++] = v6;
	}
	while ( v5 < 64 );
	SHA1Reset(0);
	SHA1Calculate(0, v10, dst);
	SHA1Clear();
	v7 = 0;
	do
	{
		v9[v7] ^= dst[(signed int)v7 % 20];
		++v7;
	}
	while ( v7 < 0x88 );
	memset(v10, 0, sizeof(v10));
	memset(dst, 0, sizeof(dst));
	v8 = 0;
	do
	{
		SHA1Reset(v8);
		SHA1Calculate(v8++, &v9[72], 0);
	}
	while ( v8 < 3 );
	memset(v9, 0, sizeof(v9));
}
// 4035DB: using guessed type char var_E0[72];
// 4035DB: using guessed type char var_58[64];
// 4035DB: using guessed type char dst[20];

int __fastcall codec_get_encoded_len(int dwSrcBytes)
{
	if ( dwSrcBytes & 0x3F )
		dwSrcBytes += 64 - (dwSrcBytes & 0x3F);
	return dwSrcBytes + 8;
}

void __fastcall codec_encode(void *pbSrcDst, int size, int size_64, char *pszPassword)
{
	char *v4; // esi
	char v5; // bl
	size_t v6; // edi
	signed int v7; // ecx
	char v9[128]; // [esp+8h] [ebp-ACh]
	char v10[20]; // [esp+88h] [ebp-2Ch]
	char dst[20]; // [esp+9Ch] [ebp-18h]
	size_t v12; // [esp+B0h] [ebp-4h]

	v4 = (char *)pbSrcDst;
	v12 = size;
	if ( size_64 != codec_get_encoded_len(size) )
		TermMsg("Invalid encode parameters");
	codec_init_key(1, pszPassword);
	v5 = 0;
	if ( v12 )
	{
		do
		{
			v6 = v12;
			if ( v12 >= 0x40 )
				v6 = 64;
			memcpy(v9, v4, v6);
			if ( v6 < 0x40 )
				memset(&v9[v6], 0, 64 - v6);
			SHA1Result(0, dst);
			SHA1Calculate(0, v9, 0);
			v7 = 0;
			do
			{
				v9[v7] ^= dst[v7 % 20];
				++v7;
			}
			while ( v7 < 64 );
			memset(dst, 0, sizeof(dst));
			memcpy(v4, v9, 0x40u);
			v4 += 64;
			v12 -= v6;
		}
		while ( v12 );
		v5 = v6;
	}
	memset(v9, 0, sizeof(v9));
	SHA1Result(0, v10);
	v4[4] = 0;
	*((_WORD *)v4 + 3) = 0;
	*(_DWORD *)v4 = *(_DWORD *)v10;
	v4[5] = v5;
	SHA1Clear();
}
// 4036BE: using guessed type char var_AC[128];
// 4036BE: using guessed type char dst[20];
