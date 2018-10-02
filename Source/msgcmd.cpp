//HEADER_GOES_HERE

#include "../types.h"

/* TODO: decompile and fix, commands are NOT deleted properly */

#ifndef NO_GLOBALS
int msgcmd_cpp_init_value; // weak
ChatCmd sgChat_Cmd;
int sgdwMsgCmdTimer;
#endif

const int msgcmd_inf = 0x7F800000; // weak

struct msgcmd_cpp_init_1
{
	msgcmd_cpp_init_1()
	{
		msgcmd_cpp_init_value = msgcmd_inf;
	}
} _msgcmd_cpp_init_1;
// 47F150: using guessed type int msgcmd_inf;
// 6761A0: using guessed type int msgcmd_cpp_init_value;

struct msgcmd_cpp_init_2
{
	msgcmd_cpp_init_2()
	{
		msgcmd_init_event();
		msgcmd_cleanup_chatcmd_atexit();
	}
} _msgcmd_cpp_init_2;

void __cdecl msgcmd_init_event()
{
	msgcmd_init_chatcmd(&sgChat_Cmd);
}

void __cdecl msgcmd_cleanup_chatcmd_atexit()
{
	atexit(msgcmd_cleanup_chatcmd);
}

void __cdecl msgcmd_cleanup_chatcmd()
{
	msgcmd_cleanup_chatcmd_1(&sgChat_Cmd);
	msgcmd_cleanup_extern_msg(sgChat_Cmd.extern_msgs);
}

void __cdecl msgcmd_cmd_cleanup()
{
	msgcmd_free_event(&sgChat_Cmd);
}

void __cdecl msgcmd_send_chat()
{
	ServerCommand *v0; // esi
	int v1; // eax

	if ( (signed int)sgChat_Cmd.extern_msgs[1] > 0 )
	{
		v0 = sgChat_Cmd.extern_msgs[1];
		v1 = GetTickCount();
		if ( (unsigned int)(v1 - sgdwMsgCmdTimer) >= 2000 )
		{
			sgdwMsgCmdTimer = v1;
			SNetSendServerChatCommand(v0->command);
			msgcmd_delete_server_cmd_W(&sgChat_Cmd, v0);
		}
	}
}

bool __fastcall msgcmd_add_server_cmd_W(char *chat_message)
{
	if ( *chat_message != '/' )
		return 0;
	msgcmd_add_server_cmd(chat_message);
	return 1;
}

void __fastcall msgcmd_add_server_cmd(char *command)
{
	char *v1; // edi
	size_t v2; // eax
	int v3; // edx
	size_t v4; // esi
	ChatCmd *v5; // eax

	v1 = command;
	v2 = strlen(command);
	if ( v2 )
	{
		v4 = v2 + 1;
		if ( v2 + 1 <= 0x80 )
		{
			v5 = msgcmd_alloc_event(&sgChat_Cmd, v3, 2, 0, 0);
			memcpy(&v5->extern_msgs[1], v1, v4);
		}
	}
}

void __fastcall msgcmd_init_chatcmd(ChatCmd *chat_cmd)
{
	ServerCommand **v1; // edx

	v1 = chat_cmd->extern_msgs;
	*v1 = 0;
	v1[1] = 0;
	*v1 = (ServerCommand *)v1;
	chat_cmd->next = 0;
	chat_cmd->extern_msgs[1] = (ServerCommand *)~(unsigned int)chat_cmd->extern_msgs;
}

void __fastcall msgcmd_free_event(ChatCmd *a1)
{
	int v1; // edx
	ChatCmd *v2; // edi
	ChatCmd *v3; // esi

	v2 = a1;
	while ( 1 )
	{
		v3 = (ChatCmd *)v2->extern_msgs[1];
		if ( (signed int)v3 <= 0 )
			break;
		msgcmd_remove_event(v3, v1);
		SMemFree(v3, ".?AUEXTERNMESSAGE@@", -2, 0);
	}
}

bool __fastcall msgcmd_delete_server_cmd_W(ChatCmd *cmd, ServerCommand *extern_msg)
{
	char *v2; // eax
	ServerCommand *v3; // eax
	bool v4; // si
	ChatCmd *ptr; // [esp+Ch] [ebp+4h]

	v2 = (char *)ptr;
	if ( !ptr )
		v2 = (char *)cmd->extern_msgs;
	v3 = (ServerCommand *)*((_DWORD *)v2 + 1);
	if ( (signed int)v3 > 0 )
		v4 = (char)v3;
	else
		v4 = 0;
	msgcmd_remove_event(ptr, (int)extern_msg);
	SMemFree(ptr, ".?AUEXTERNMESSAGE@@", -2, 0);
	return v4;
}

ChatCmd *__fastcall msgcmd_alloc_event(ChatCmd *a1, int a2, int a3, int a4, int a5)
{
	int v5; // eax
	ChatCmd *v6; // edi
	ChatCmd *v7; // eax
	int v8; // edx
	ChatCmd *v9; // esi

	v5 = a5;
	_LOBYTE(v5) = a5 | 8;
	v6 = a1;
	v7 = (ChatCmd *)SMemAlloc(a4 + 136, ".?AUEXTERNMESSAGE@@", -2, v5);
	if ( v7 )
	{
		v7->next = 0;
		v7->extern_msgs[0] = 0;
		v9 = v7;
	}
	else
	{
		v9 = 0;
	}
	if ( a3 )
		msgcmd_event_type(v6, v8, (int *)v9, a3, 0);
	return v9;
}

void __fastcall msgcmd_remove_event(ChatCmd *a1, int a2)
{
	ServerCommand **v2; // esi

	v2 = (ServerCommand **)a1;
	msgcmd_cleanup_extern_msg((ServerCommand **)a1);
	msgcmd_cleanup_extern_msg(v2);
	if ( a2 & 1 )
	{
		if ( v2 )
			SMemFree(v2, "delete", -1, 0);
	}
}

void __fastcall msgcmd_event_type(ChatCmd *a1, int a2, int *a3, int a4, int a5)
{
	ChatCmd *v5; // edi
	int *v6; // esi
	int *v7; // eax
	int v8; // ecx
	int v9; // edx
	int v10; // ecx
	int v11; // edx

	v5 = a1;
	v6 = a3;
	if ( !a3 )
		v6 = (int *)a1->extern_msgs;
	if ( *v6 )
		msgcmd_cleanup_extern_msg((ServerCommand **)v6);
	v7 = (int *)a5;
	if ( !a5 )
		v7 = (int *)v5->extern_msgs;
	if ( a4 == 1 )
	{
		*v6 = (int)v7;
		v6[1] = v7[1];
		v9 = v7[1];
		v10 = (int)v5->next;
		if ( v9 > 0 )
		{
			if ( v10 < 0 )
				v10 = (int)v7 - *(_DWORD *)(*v7 + 4);
			v11 = v10 + v9;
		}
		else
		{
			v11 = ~v9;
		}
		*(_DWORD *)v11 = (unsigned int)v6;
		v7[1] = (int)a3;
	}
	else if ( a4 == 2 )
	{
		v8 = *v7;
		*v6 = *v7;
		v6[1] = *(_DWORD *)(v8 + 4);
		*(_DWORD *)(v8 + 4) = (unsigned int)a3;
		*v7 = (int)v6;
	}
}

void __fastcall msgcmd_cleanup_chatcmd_1(ChatCmd *a1)
{
	ChatCmd *v1; // esi
	ServerCommand **v2; // ecx

	v1 = a1;
	while ( 1 )
	{
		v2 = (ServerCommand **)v1->extern_msgs[1];
		if ( (signed int)v2 <= 0 )
			break;
		msgcmd_cleanup_extern_msg(v2);
	}
}

void __fastcall msgcmd_cleanup_extern_msg(ServerCommand **extern_msgs)
{
	ServerCommand *v1; // esi
	signed int v2; // edx
	int v3; // edx

	v1 = *extern_msgs;
	if ( *extern_msgs )
	{
		v2 = (signed int)extern_msgs[1];
		if ( v2 > 0 )
			v3 = (int)extern_msgs + v2 - v1->field_4;
		else
			v3 = ~v2;
		*(_DWORD *)v3 = (unsigned int)v1;
		(*extern_msgs)->field_4 = (int)extern_msgs[1];
		*extern_msgs = 0;
		extern_msgs[1] = 0;
	}
}
