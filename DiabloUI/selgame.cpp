// ref: 0x1000B66A
void UNKCALL SelGame_1000B66A(void *arg) { return; }
/* {
	dword_1002A408 = (int)arg;
} */
// 1002A408: using guessed type int dword_1002A408;

// ref: 0x1000B671
int SelGame_1000B671() { return 0; }
/* {
	return dword_1002A408;
} */
// 1002A408: using guessed type int dword_1002A408;

// ref: 0x1000B677
void UNKCALL SelGame_1000B677(void *arg) { return; }
/* {
	dword_1002A404 = (int)arg;
} */
// 1002A404: using guessed type int dword_1002A404;

// ref: 0x1000B67E
int SelGame_1000B67E() { return 0; }
/* {
	return dword_1002A404;
} */
// 1002A404: using guessed type int dword_1002A404;

// ref: 0x1000B684
int __stdcall UiSelectGame(int a1, _SNETPROGRAMDATA *client_info, _SNETPLAYERDATA *user_info, _SNETUIDATA *ui_info, _SNETVERSIONDATA *file_info, int *a6) { return 0; }
/* {
	int v6; // eax
	CHAR v8; // [esp+4h] [ebp-110h]
	char v9; // [esp+5h] [ebp-10Fh]
	short v10; // [esp+81h] [ebp-93h]
	char v11; // [esp+83h] [ebp-91h]
	CHAR v12; // [esp+84h] [ebp-90h]
	char v13; // [esp+85h] [ebp-8Fh]
	short v14; // [esp+101h] [ebp-13h]
	char v15; // [esp+103h] [ebp-11h]
	int v16; // [esp+104h] [ebp-10h]
	CHAR *v17; // [esp+108h] [ebp-Ch]
	CHAR *v18; // [esp+10Ch] [ebp-8h]

	v12 = byte_10029448;
	memset(&v13, 0, 0x7Cu);
	v14 = 0;
	v15 = 0;
	v8 = byte_10029448;
	memset(&v9, 0, 0x7Cu);
	v10 = 0;
	v11 = 0;
	Connect_10004028((int)&v12, 128, (int)&v8, 128);
	memset(&v16, 0, 0x10u);
	if ( a3 )
		memcpy(&v16, a3, 0x10u);
	v16 = 16;
	v17 = &v12;
	v18 = &v8;
	if ( SelGame_1000B671() )
		return SelIPX_1000C634(a1, a2, (int)&v16, (_DWORD *)a4, a5, a6);
	v6 = SelGame_1000B67E();
	switch ( v6 )
	{
		case 1230002254:
			return SelIPX_1000C634(a1, a2, (int)&v16, (_DWORD *)a4, a5, a6);
		case 1297040461:
			return Modem_10008680(a1, a2, (int)&v16, (_DWORD *)a4, a5, a6);
		case 1396916812:
			return DirLink_10005D05(a1, a2, (int)&v16, (_DWORD *)a4, a5, a6);
	}
	return SNetSelectGame(a1, a2, &v16, a4, a5, a6);
} */
// 10010490: using guessed type int __stdcall SNetSelectGame(_DWORD, _DWORD, _DWORD, _DWORD, _DWORD, _DWORD);

// ref: 0x1000B795
signed int SelGame_1000B795() { return 0; }
/* {
	signed int result; // eax

	result = 2139095040;
	dword_1002A400 = 2139095040;
	return result;
} */
// 1002A400: using guessed type int dword_1002A400;
