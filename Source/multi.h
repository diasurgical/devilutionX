//HEADER_GOES_HERE
#ifndef __MULTI_H__
#define __MULTI_H__

extern char gbSomebodyWonGameKludge; // weak
extern char pkdata_6761C0[4100];
extern char szPlayerDescript[128];
extern short sgwPackPlrOffsetTbl[MAX_PLRS];
extern PkPlayerStruct netplr[MAX_PLRS];
extern char sgbPlayerTurnBitTbl[MAX_PLRS];
extern char sgbPlayerLeftGameTbl[MAX_PLRS];
extern int multi_cpp_init_value; // weak
extern int sgbSentThisCycle; // idb
extern int dword_678628; // weak
extern char gbActivePlayers; // weak
extern char gbGameDestroyed; // weak
extern char sgbSendDeltaTbl[MAX_PLRS];
extern _gamedata sgGameInitInfo;
extern char byte_678640; // weak
extern int sglTimeoutStart; // weak
extern int sgdwPlayerLeftReasonTbl[MAX_PLRS];
extern char pkdata_678658[4100];
extern unsigned int sgdwGameLoops; // idb
extern UCHAR gbMaxPlayers;
extern char sgbTimeout; // weak
extern char szPlayerName[128];
extern char gbDeltaSender; // weak
extern int sgbNetInited; // weak
extern int player_state[MAX_PLRS];

void __cdecl multi_cpp_init();
void __fastcall multi_msg_add(unsigned char *a1, unsigned char a2);
void __fastcall NetSendLoPri(unsigned char *pbMsg, unsigned char bLen);
void __fastcall multi_copy_packet(void *a1, void *packet, int size);
void __fastcall multi_send_packet(void *packet, int dwSize);
void __fastcall NetRecvPlrData(TPkt *pkt);
void __fastcall NetSendHiPri(unsigned char *pbMsg, unsigned char bLen);
unsigned char *__fastcall multi_recv_packet(void *packet, unsigned char *a2, int *a3);
void __fastcall multi_send_msg_packet(int a1, unsigned char *a2, unsigned char len);
void __cdecl multi_msg_countdown();
void __fastcall multi_parse_turn(int pnum, int turn);
void __fastcall multi_handle_turn_upper_bit(int pnum);
void __fastcall multi_player_left(int pnum, int reason);
void __cdecl multi_clear_left_tbl();
void __fastcall multi_player_left_msg(int pnum, int left);
void __cdecl multi_net_ping();
int __cdecl multi_handle_delta();
int __fastcall multi_check_pkt_valid(char *a1);
void __cdecl multi_mon_seeds();
void __cdecl multi_begin_timeout();
void __cdecl multi_check_drop_player();
void __cdecl multi_process_network_packets();
void __fastcall multi_handle_all_packets(int players, TPkt *packet, int a3);
void __cdecl multi_process_tmsgs();
void __fastcall multi_send_zero_packet(int pnum, char a2, void *pbSrc, int dwLen);
void __cdecl NetClose();
char __fastcall multi_event_handler(int a1);
void __stdcall multi_handle_events(_SNETEVENT *pEvt);
int __fastcall NetInit(int bSinglePlayer, int *pfExitProgram);
void __fastcall multi_clear_pkt(char *a1);
void __fastcall multi_send_pinfo(int pnum, char cmd);
int __fastcall InitNewSeed(int newseed);
void __cdecl SetupLocalCoords();
int __fastcall multi_init_single(_SNETPROGRAMDATA *client_info, _SNETPLAYERDATA *user_info, _SNETUIDATA *ui_info);
int __fastcall multi_init_multi(_SNETPROGRAMDATA *client_info, _SNETPLAYERDATA *user_info, _SNETUIDATA *ui_info, int *a4);
int __fastcall multi_upgrade(int *a1);
void __fastcall multi_player_joins(int pnum, TCmdPlrInfoHdr *cmd, int a3);

/* rdata */

extern const int multi_inf; // weak
extern const int event_types[3];

#endif /* __MULTI_H__ */
