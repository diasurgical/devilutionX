//HEADER_GOES_HERE
#ifndef __MSGCMD_H__
#define __MSGCMD_H__

extern int msgcmd_cpp_init_value; // weak
extern ChatCmd sgChat_Cmd;
extern int sgdwMsgCmdTimer;

void __cdecl msgcmd_cpp_init_1();
void __cdecl msgcmd_cpp_init_2();
void __cdecl msgcmd_init_event();
void __cdecl msgcmd_cleanup_chatcmd_atexit();
void __cdecl msgcmd_cleanup_chatcmd();
void __cdecl msgcmd_cmd_cleanup();
void __cdecl msgcmd_send_chat();
bool __fastcall msgcmd_add_server_cmd_W(char *chat_message);
void __fastcall msgcmd_add_server_cmd(char *command);
void __fastcall msgcmd_init_chatcmd(ChatCmd *chat_cmd);
void __fastcall msgcmd_free_event(ChatCmd *a1);
bool __fastcall msgcmd_delete_server_cmd_W(ChatCmd *cmd, ServerCommand *extern_msg);
ChatCmd *__fastcall msgcmd_alloc_event(ChatCmd *a1, int a2, int a3, int a4, int a5);
void __fastcall msgcmd_remove_event(ChatCmd *a1, int a2);
void __fastcall msgcmd_event_type(ChatCmd *a1, int a2, int *a3, int a4, int a5);
void __fastcall msgcmd_cleanup_chatcmd_1(ChatCmd *a1);
void __fastcall msgcmd_cleanup_extern_msg(ServerCommand **extern_msgs);

/* rdata */

extern const int msgcmd_inf; // weak

#endif /* __MSGCMD_H__ */
