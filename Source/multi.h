//HEADER_GOES_HERE
#ifndef __MULTI_H__
#define __MULTI_H__

extern BOOLEAN gbSomebodyWonGameKludge;
extern char szPlayerDescript[128];
extern WORD sgwPackPlrOffsetTbl[MAX_PLRS];
extern PkPlayerStruct netplr[MAX_PLRS];
extern BOOL gbShouldValidatePackage;
extern BYTE gbActivePlayers;
extern BOOLEAN gbGameDestroyed;
extern BOOLEAN gbSelectProvider;
extern BYTE gbMaxPlayers;
extern char szPlayerName[128];
extern BYTE gbDeltaSender;
extern int player_state[MAX_PLRS];

void multi_msg_add(BYTE *pbMsg, BYTE bLen);
void NetSendLoPri(BYTE *pbMsg, BYTE bLen);
void multi_copy_packet(TBuffer *pBuf, void *packet, BYTE size);
void multi_send_packet(void *packet, BYTE dwSize);
void NetRecvPlrData(TPkt *pkt);
void NetSendHiPri(BYTE *pbMsg, BYTE bLen);
BYTE *multi_recv_packet(TBuffer *pBuf, BYTE *body, int *size);
void multi_send_msg_packet(int pmask, BYTE *src, BYTE len);
void multi_msg_countdown();
void multi_parse_turn(int pnum, int turn);
void multi_handle_turn_upper_bit(int pnum);
void multi_player_left(int pnum, int reason);
void multi_clear_left_tbl();
void multi_player_left_msg(int pnum, int left);
void multi_net_ping();
int multi_handle_delta();
int multi_check_pkt_valid(TBuffer *pBuf);
void multi_mon_seeds();
void multi_begin_timeout();
void multi_check_drop_player();
void multi_process_network_packets();
void multi_handle_all_packets(int pnum, BYTE *pData, int nSize);
void multi_process_tmsgs();
void multi_send_zero_packet(int pnum, BYTE bCmd, BYTE *pbSrc, DWORD dwLen);
void NetClose();
void multi_event_handler(BOOL add);
void multi_handle_events(_SNETEVENT *pEvt);
BOOL NetInit(BOOL bSinglePlayer, BOOL *pfExitProgram);
void buffer_init(TBuffer *pBuf);
void multi_send_pinfo(int pnum, char cmd);
int InitLevelType(int l);
void SetupLocalCoords();
BOOL multi_init_single(_SNETPROGRAMDATA *client_info, _SNETPLAYERDATA *user_info, _SNETUIDATA *ui_info);
BOOL multi_init_multi(_SNETPROGRAMDATA *client_info, _SNETPLAYERDATA *user_info, _SNETUIDATA *ui_info, BOOL *pfExitProgram);
BOOL multi_upgrade(BOOL *pfExitProgram);
void recv_plrinfo(int pnum, TCmdPlrInfoHdr *p, BOOL recv);

/* rdata */

extern const int event_types[3];

#endif /* __MULTI_H__ */
