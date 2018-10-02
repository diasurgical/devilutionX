// ref: 0x100028C2
void UNKCALL BNetGW_100028C2(_DWORD *arg) { return; }
/* {
	_DWORD *v1; // esi
	bool v2; // zf
	bool v3; // sf
	int v4; // edi
	int v5; // eax
	int v6; // edi
	int v7; // ST08_4
	int v8; // eax
	signed int v9; // eax
	struct _TIME_ZONE_INFORMATION TimeZoneInformation; // [esp+Ch] [ebp-B8h]
	char *v11; // [esp+B8h] [ebp-Ch]
	int v12; // [esp+BCh] [ebp-8h]
	int v13; // [esp+C0h] [ebp-4h]

	v1 = arg;
	arg[2] = 0;
	arg[3] = 0;
	arg[4] = 0;
	arg[5] = 0;
	arg[6] = 0;
	*(_BYTE *)arg = 0;
	BNetGW_10002C23(arg);
	if ( !v1[4] )
		goto LABEL_15;
	if ( v1[6] < 0x3E8u )
	{
		v1[1] = 0;
		BNetGW_10002A07(v1);
		SRegDeleteValue("Configuration", "Battle.net gateways", 2u);
	}
	if ( !v1[4] )
LABEL_15:
		BNetGW_10002C51(v1);
	if ( v1[6] >= 0x3E8u )
	{
		v2 = v1[5] == 0;
		v3 = v1[5] < 0;
		v4 = v1[4];
		v13 = -2;
		v12 = 0;
		if ( !v3 && !v2 )
		{
			do
			{
				v5 = SStrLen(v4) + 1;
				v12 += v5;
				v4 += v5;
				++v13;
			}
			while ( v12 < v1[5] );
		}
		v6 = v1[4];
		v7 = v1[4];
		v1[2] = v13 / 3;
		v8 = SStrLen(v7);
		v9 = strtoul((const char *)(v6 + v8 + 1), &v11, 10);
		v1[3] = v9;
		v1[1] = 0;
		if ( v9 < 1 || v9 > v1[2] )
		{
			SMemZero(&TimeZoneInformation, 172);
			GetTimeZoneInformation(&TimeZoneInformation);
			BNetGW_100029BF(v1, TimeZoneInformation.Bias);
		}
	}
	else
	{
		v1[2] = 0;
		v1[3] = 0;
	}
} */
// 100103A0: using guessed type int __stdcall SStrLen(_DWORD);
// 100103C4: using guessed type int __stdcall SMemZero(_DWORD, _DWORD);
// 100103CA: using guessed type int __stdcall SRegDeleteValue(const char *, const char *, unsigned int);

// ref: 0x100029BF
void UNKCALL BNetGW_100029BF(_DWORD *arg, int a2) { return; }
/* {
	_DWORD *v2; // esi
	char *v3; // edi
	signed int v4; // ebx
	signed int v5; // ebp
	int v6; // eax
	int v7; // eax

	v2 = arg;
	v3 = (char *)1;
	v4 = 1380;
	v5 = 1;
	if ( arg[2] >= 1 )
	{
		do
		{
			v6 = BNetGW_10002AF0(v2, v3);
			v7 = abs(60 * v6 - a2);
			if ( v7 < v4 )
			{
				v4 = v7;
				v5 = (signed int)v3;
			}
			++v3;
		}
		while ( (signed int)v3 <= v2[2] );
	}
	BNetGW_10002B51(v2, v5);
} */

// ref: 0x10002A07
void *UNKCALL BNetGW_10002A07(_DWORD *arg) { return 0; }
/* {
	_DWORD *v1; // esi
	int v2; // edi
	_BYTE *v3; // ecx
	const char *v4; // eax
	int result; // eax

	v1 = arg;
	if ( arg[1] )
	{
		v2 = arg[4];
		if ( v2 )
		{
			v3 = (_BYTE *)(v2 + SStrLen(arg[4]) + 1);
			*v3 = v1[3] / 10 + 48;
			v4 = "Override Battle.net gateways";
			v3[1] = v1[3] % 10 + 48;
			if ( !v1[7] )
				v4 = "Battle.net gateways";
			SRegSaveData("Configuration", v4, 0x82u, (void *)v1[4], v1[5]);
		}
	}
	result = v1[4];
	if ( result )
	{
		result = SMemFree(result, "C:\\Src\\Diablo\\DiabloUI\\BNetGW.cpp", 152, 0);
		v1[4] = 0;
		v1[5] = 0;
	}
	return result;
} */
// 10010340: using guessed type int __stdcall SMemFree(_DWORD, _DWORD, _DWORD, _DWORD);
// 100103A0: using guessed type int __stdcall SStrLen(_DWORD);
// 100103D0: using guessed type int __stdcall SRegSaveData(const char *, const char *, unsigned int, void *, unsigned int);

// ref: 0x10002A84
_DWORD *UNKCALL BNetGW_10002A84(_DWORD *arg, signed int a2) { return 0; }
/* {
	signed int v2; // eax
	signed int v3; // ebx
	int v4; // esi
	int v5; // edi
	bool v6; // sf
	unsigned char v7; // of
	int v8; // eax
	_DWORD *result; // eax
	_DWORD *v10; // [esp+8h] [ebp-4h]
	_DWORD *v11; // [esp+14h] [ebp+8h]

	v10 = arg;
	if ( !arg[4] )
		return arg;
	v2 = a2;
	v3 = 1;
	if ( a2 < 1 || a2 > arg[2] )
		return arg;
	v4 = 3 * a2;
	v5 = 0;
	v11 = (_DWORD *)arg[4];
	if ( 3 * v2 <= 1 )
	{
LABEL_7:
		v7 = __OFSUB__(v5, arg[5]);
		v6 = v5 - arg[5] < 0;
	}
	else
	{
		while ( 1 )
		{
			v7 = __OFSUB__(v5, arg[5]);
			v6 = v5 - arg[5] < 0;
			if ( v5 >= arg[5] )
				break;
			v8 = SStrLen(v11);
			arg = v10;
			v11 = (_DWORD *)((char *)v11 + ++v8);
			v5 += v8;
			if ( ++v3 >= v4 )
				goto LABEL_7;
		}
	}
	if ( !(v6 ^ v7) )
		return arg;
	result = v11;
	if ( v3 > v4 )
		return arg;
	return result;
} */
// 100103A0: using guessed type int __stdcall SStrLen(_DWORD);

// ref: 0x10002AE5
signed int BNetGW_10002AE5() { return 0; }
/* {
	signed int result; // eax

	result = 2139095040;
	dword_10029478 = 2139095040;
	return result;
} */
// 10029478: using guessed type int dword_10029478;

// ref: 0x10002AF0
int UNKCALL BNetGW_10002AF0(_DWORD *arg, char *a2) { return 0; }
/* {
	const char *v2; // eax
	const char *v3; // esi

	v2 = (const char *)BNetGW_10002A84(arg, (signed int)a2);
	v3 = v2;
	if ( *v2 )
		v3 = &v2[SStrLen(v2) + 1];
	return strtol(v3, &a2, 10);
} */
// 100103A0: using guessed type int __stdcall SStrLen(_DWORD);

// ref: 0x10002B21
_BYTE *UNKCALL BNetGW_10002B21(_DWORD *arg, signed int a2) { return 0; }
/* {
	_DWORD *v2; // eax
	_BYTE *v3; // esi

	v2 = BNetGW_10002A84(arg, a2);
	v3 = v2;
	if ( *(_BYTE *)v2 )
	{
		v3 = (char *)v2 + SStrLen(v2) + 1;
		if ( *v3 )
			v3 += SStrLen(v3) + 1;
	}
	return v3;
} */
// 100103A0: using guessed type int __stdcall SStrLen(_DWORD);

// ref: 0x10002B51
void UNKCALL BNetGW_10002B51(_DWORD *arg, signed int a2) { return; }
/* {
	signed int v2; // eax

	if ( arg[4] )
	{
		v2 = a2;
		if ( a2 >= 1 && a2 <= arg[2] )
		{
			if ( a2 > 99 )
				v2 = 99;
			arg[3] = v2;
			arg[1] = 1;
		}
	}
} */

// ref: 0x10002B78
char *UNKCALL BNetGW_10002B78(_DWORD *arg, char *a2) { return 0; }
/* {
	_DWORD *v2; // esi
	char *result; // eax
	char *v4; // ST08_4
	void *v5; // eax
	char *v6; // [esp+Ch] [ebp-4h]

	v2 = arg;
	v6 = 0;
	result = (char *)SRegLoadData("Configuration", a2, 0x82u, 0, 0, (unsigned int *)&v6);
	if ( result )
	{
		result = v6;
		if ( v6 )
		{
			v4 = v6;
			v2[5] = v6;
			v5 = (void *)SMemAlloc(v4, "C:\\Src\\Diablo\\DiabloUI\\BNetGW.cpp", 263, 0);
			v2[4] = v5;
			if ( !SRegLoadData("Configuration", a2, 0x82u, v5, v2[5], 0) )
			{
				SMemFree(v2[4], "C:\\Src\\Diablo\\DiabloUI\\BNetGW.cpp", 271, 0);
				v2[4] = 0;
				v2[5] = 0;
			}
			result = (char *)v2[4];
			v2[6] = 0;
			if ( result )
			{
				a2 = result;
				v2[6] = strtoul(result, &a2, 10);
				result = (char *)v2[4];
				if ( result == a2 )
					v2[6] = 0;
			}
		}
	}
	return result;
} */
// 10010340: using guessed type int __stdcall SMemFree(_DWORD, _DWORD, _DWORD, _DWORD);
// 10010364: using guessed type int __stdcall SMemAlloc(_DWORD, _DWORD, _DWORD, _DWORD);
// 100103D6: using guessed type int __stdcall SRegLoadData(const char *, const char *, unsigned int, void *, unsigned int, unsigned int *);

// ref: 0x10002C23
char *UNKCALL BNetGW_10002C23(_DWORD *arg) { return 0; }
/* {
	_DWORD *v1; // esi
	char *result; // eax

	v1 = arg;
	arg[7] = 0;
	result = BNetGW_10002B78(arg, "Override Battle.net gateways");
	if ( !v1[4] )
		return BNetGW_10002B78(v1, "Battle.net gateways");
	v1[7] = 1;
	return result;
} */

// ref: 0x10002C51
int UNKCALL BNetGW_10002C51(_DWORD *arg) { return 0; }
/* {
	int result; // eax
	char *v2; // edi
	char *v3; // esi
	unsigned int v4; // ebx
	char *v5; // esi
	_BYTE *v6; // esi
	char *v7; // eax
	char *v8; // eax
	_BYTE *v9; // esi
	char *v10; // esi
	char v11; // al
	unsigned int v12; // esi
	char *v13; // [esp+4h] [ebp-10h]
	int v14; // [esp+8h] [ebp-Ch]
	int v15; // [esp+Ch] [ebp-8h]
	_DWORD *v16; // [esp+10h] [ebp-4h]

	v15 = 0;
	v16 = arg;
	result = BNetGW_10002DBF(&v15);
	v2 = (char *)result;
	v14 = result;
	if ( result )
	{
		v3 = (char *)SMemAlloc(2 * v15, "C:\\Src\\Diablo\\DiabloUI\\BNetGW.cpp", 358, 0);
		v13 = v3;
		v4 = (unsigned int)&v2[v15];
		sprintf(v3, "%d", 1000);
		v5 = &v3[SStrLen(v3) + 1];
		*v5++ = 48;
		*v5++ = 48;
		*v5 = 0;
		v6 = v5 + 1;
		if ( (unsigned int)v2 < v4 )
		{
			if ( (unsigned int)v2 >= v4 )
				goto LABEL_8;
			do
			{
				if ( *v2 == 35 )
				{
					v7 = BNetGW_10002DEB(v2, v4);
					v8 = BNetGW_10002E0B(v7, v4);
				}
				else
				{
					do
					{
						if ( isspace(*v2) )
							break;
						*v6++ = *v2++;
					}
					while ( (unsigned int)v2 < v4 );
LABEL_8:
					*v6 = 0;
					v9 = v6 + 1;
					if ( (unsigned int)v2 < v4 )
					{
						do
						{
							if ( !isspace(*v2) )
								break;
							++v2;
						}
						while ( (unsigned int)v2 < v4 );
						while ( (unsigned int)v2 < v4 && !isspace(*v2) )
							*v9++ = *v2++;
					}
					*v9 = 0;
					v10 = v9 + 1;
					if ( (unsigned int)v2 < v4 )
					{
						do
						{
							if ( !isspace(*v2) )
								break;
							++v2;
						}
						while ( (unsigned int)v2 < v4 );
						while ( (unsigned int)v2 < v4 )
						{
							v11 = *v2;
							if ( *v2 == 13 || v11 == 10 )
								break;
							*v10++ = v11;
							++v2;
						}
					}
					*v10 = 0;
					v6 = v10 + 1;
					v8 = BNetGW_10002E0B(v2, v4);
				}
				v2 = v8;
			}
			while ( (unsigned int)v8 < v4 );
		}
		*v6 = 0;
		v12 = v6 - v13 + 1;
		SRegSaveData("Configuration", "Battle.net gateways", 0x82u, v13, v12);
		SMemFree(v14, "C:\\Src\\Diablo\\DiabloUI\\BNetGW.cpp", 429, 0);
		result = (int)v16;
		v16[5] = v12;
		*(_DWORD *)(result + 16) = v13;
		*(_DWORD *)(result + 24) = 1000;
	}
	return result;
} */
// 10010340: using guessed type int __stdcall SMemFree(_DWORD, _DWORD, _DWORD, _DWORD);
// 10010364: using guessed type int __stdcall SMemAlloc(_DWORD, _DWORD, _DWORD, _DWORD);
// 100103A0: using guessed type int __stdcall SStrLen(_DWORD);
// 100103D0: using guessed type int __stdcall SRegSaveData(const char *, const char *, unsigned int, void *, unsigned int);

// ref: 0x10002DBF
int UNKCALL BNetGW_10002DBF(_DWORD *arg) { return 0; }
/* {
	_DWORD *v1; // esi
	int v3; // [esp+4h] [ebp-8h]
	int v4; // [esp+8h] [ebp-4h]

	v4 = 0;
	v1 = arg;
	SFileLoadFile("rez\\gateways.txt", &v3, &v4, 0, 0);
	*v1 = v4;
	return v3;
} */
// 100103DC: using guessed type int __stdcall SFileLoadFile(_DWORD, _DWORD, _DWORD, _DWORD, _DWORD);

// ref: 0x10002DEB
char *__stdcall BNetGW_10002DEB(char *a1, unsigned int a2) { return 0; }
/* {
	char *result; // eax
	char v3; // cl

	for ( result = a1; (unsigned int)result < a2; ++result )
	{
		v3 = *result;
		if ( !*result || v3 == 13 || v3 == 10 )
			break;
	}
	return result;
} */

// ref: 0x10002E0B
char *__stdcall BNetGW_10002E0B(char *a1, unsigned int a2) { return 0; }
/* {
	char *result; // eax
	char v3; // cl

	for ( result = a1; (unsigned int)result < a2; ++result )
	{
		v3 = *result;
		if ( !*result || v3 != 13 && v3 != 10 )
			break;
	}
	return result;
} */
