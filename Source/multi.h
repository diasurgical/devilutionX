//HEADER_GOES_HERE
#ifndef __MULTI_H__
#define __MULTI_H__

extern BOOLEAN gbSomebodyWonGameKludge; // weak
extern char szPlayerDescript[128];
extern WORD sgwPackPlrOffsetTbl[MAX_PLRS];
extern PkPlayerStruct netplr[MAX_PLRS];
extern BOOL gbShouldValidatePackage;
extern BYTE gbActivePlayers;
extern BOOLEAN gbGameDestroyed; // weak
extern char byte_678640;     // weak
extern BYTE gbMaxPlayers;
extern char szPlayerName[128];
extern BYTE gbDeltaSender; // weak
extern int player_state[MAX_PLRS];

#ifdef _DEBUG
void __cdecl dumphist(const char *pszFmt, ...);
#endif
void multi_msg_add(BYTE *pbMsg, BYTE bLen);
void NetSendLoPri(BYTE *pbMsg, BYTE bLen);
void multi_copy_packet(TBuffer *a1, void *packet, BYTE size);
void multi_send_packet(void *packet, BYTE dwSize);
void NetRecvPlrData(TPkt *pkt);
void NetSendHiPri(BYTE *pbMsg, BYTE bLen);
unsigned char *multi_recv_packet(TBuffer *packet, unsigned char *a2, int *a3);
void multi_send_msg_packet(int a1, BYTE *a2, BYTE len);
void multi_msg_countdown();
void multi_parse_turn(int pnum, int turn);
void multi_handle_turn_upper_bit(int pnum);
void multi_player_left(int pnum, int reason);
void multi_clear_left_tbl();
void multi_player_left_msg(int pnum, int left);
void multi_net_ping();
int multi_handle_delta();
int multi_check_pkt_valid(TBuffer *a1);
void multi_mon_seeds();
void multi_begin_timeout();
void multi_check_drop_player();
void multi_process_network_packets();
void multi_handle_all_packets(int players, TPkt *packet, int a3);
void multi_process_tmsgs();
void multi_send_zero_packet(int pnum, char a2, void *pbSrc, int dwLen);
void NetClose();
char multi_event_handler(int a1);
void __stdcall multi_handle_events(_SNETEVENT *pEvt);
BOOL NetInit(BOOL bSinglePlayer, BOOL *pfExitProgram);
void buffer_init(TBuffer *pBuf);
void multi_send_pinfo(int pnum, char cmd);
int InitNewSeed(int newseed);
void SetupLocalCoords();
BOOL multi_init_single(_SNETPROGRAMDATA *client_info, _SNETPLAYERDATA *user_info, _SNETUIDATA *ui_info);
BOOL multi_init_multi(_SNETPROGRAMDATA *client_info, _SNETPLAYERDATA *user_info, _SNETUIDATA *ui_info, int *pfExitProgram);
BOOL multi_upgrade(int *pfExitProgram);
void recv_plrinfo(int pnum, TCmdPlrInfoHdr *p, BOOL recv);

/* rdata */

extern const int multi_inf; // weak
extern const int event_types[3];

#endif /* __MULTI_H__ */
