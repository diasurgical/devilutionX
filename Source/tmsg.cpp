//HEADER_GOES_HERE

#include "../types.h"

#ifndef NO_GLOBALS
TMsg *sgpTimedMsgHead;
#endif

int __fastcall tmsg_get(unsigned char *pbMsg, unsigned int dwMaxLen)
{
	unsigned char *v2; // ebx
	DWORD v3; // eax
	TMsg *v4; // esi
	size_t v6; // edi

	v2 = pbMsg;
	if ( !sgpTimedMsgHead )
		return 0;
	v3 = GetTickCount();
	v4 = sgpTimedMsgHead;
	if ( (signed int)(sgpTimedMsgHead->hdr.dwTime - v3) >= 0 )
		return 0;
	sgpTimedMsgHead = sgpTimedMsgHead->hdr.pNext;
	v6 = v4->hdr.bLen;
	memcpy(v2, v4->body, v6);
	mem_free_dbg(v4);
	return v6;
}

void __fastcall tmsg_add(unsigned char *pbMsg, unsigned char bLen)
{
	unsigned char v2; // bl
	unsigned char *v3; // ebp
	size_t v4; // edi
	TMsg *v5; // eax
	TMsg *v6; // esi
	DWORD v7; // eax
	TMsg *v8; // ecx
	TMsg **v9; // eax

	v2 = bLen;
	v3 = pbMsg;
	v4 = bLen;
	v5 = (TMsg *)DiabloAllocPtr(bLen + 12);
	v6 = v5;
	v5->hdr.pNext = 0;
	v7 = GetTickCount();
	v6->hdr.bLen = v2;
	v6->hdr.dwTime = v7 + 500;
	memcpy(v6->body, v3, v4);
	v8 = sgpTimedMsgHead;
	v9 = &sgpTimedMsgHead;
	while ( v8 )
	{
		v9 = &v8->hdr.pNext;
		v8 = v8->hdr.pNext;
	}
	*v9 = v6;
}

void __cdecl tmsg_cleanup()
{
	TMsg *v0; // eax
	TMsg *v1; // esi

	v0 = sgpTimedMsgHead;
	if ( sgpTimedMsgHead )
	{
		do
		{
			v1 = v0->hdr.pNext;
			sgpTimedMsgHead = 0;
			mem_free_dbg(v0);
			v0 = v1;
			sgpTimedMsgHead = v1;
		}
		while ( v1 );
	}
}
