//HEADER_GOES_HERE
#ifndef __MSG_H__
#define __MSG_H__

extern int sgdwOwnerWait; // weak
extern int msg_cpp_init_value; // weak
extern int sgdwRecvOffset; // idb
extern int sgnCurrMegaPlayer; // weak
extern DLevel sgLevels[NUMLEVELS];
extern char sbLastCmd; // weak
extern TMegaPkt *sgpCurrPkt;
extern char sgRecvBuf[4722];
extern unsigned char sgbRecvCmd; // idb
extern LocalLevel sgLocals[NUMLEVELS];
extern DJunk sgJunk;
extern TMegaPkt *sgpMegaPkt;
extern char sgbDeltaChanged; // weak
extern char sgbDeltaChunks; // weak
extern int deltaload; // weak
extern char gbBufferMsgs; // weak
extern int dword_676198; // weak
extern int msg_err_timer; // weak

void __cdecl msg_cpp_init();
void __fastcall msg_send_drop_pkt(int pnum, int reason);
void __fastcall msg_send_packet(int pnum, void *packet, int dwSize);
TMegaPkt *__cdecl msg_get_next_packet();
int __cdecl msg_wait_resync();
void __cdecl msg_free_packets();
int __cdecl msg_wait_for_turns();
void __cdecl msg_process_net_packets();
void __cdecl msg_pre_packet();
void __fastcall DeltaExportData(int pnum);
void *__fastcall DeltaExportItem(void *dst, void *src);
void *__fastcall DeltaExportObject(void *dst, void *src);
void *__fastcall DeltaExportMonster(void *dst, void *src);
char *__fastcall DeltaExportJunk(char *a1);
int __fastcall msg_comp_level(char *buffer, int size);
void __cdecl delta_init();
void __fastcall delta_kill_monster(int mi, unsigned char x, unsigned char y, unsigned char bLevel);
void __fastcall delta_monster_hp(int mi, int hp, unsigned char bLevel);
void __fastcall delta_sync_monster(TCmdLocParam1 *packet, char level);
void __fastcall delta_sync_golem(TCmdGolem *pG, int pnum, int bLevel);
void __fastcall delta_leave_sync(unsigned char bLevel);
bool __fastcall delta_portal_inited(int i);
bool __fastcall delta_quest_inited(int i);
void __fastcall DeltaAddItem(int ii);
void __cdecl DeltaSaveLevel();
void __cdecl DeltaLoadLevel();
void __fastcall NetSendCmd(BOOL bHiPri, unsigned char bCmd);
void __fastcall NetSendCmdGolem(unsigned char mx, unsigned char my, unsigned char dir, unsigned char menemy, int hp, int cl);
void __fastcall NetSendCmdLoc(BOOL bHiPri, unsigned char bCmd, unsigned char x, unsigned char y);
void __fastcall NetSendCmdLocParam1(BOOL bHiPri, unsigned char bCmd, unsigned char x, unsigned char y, unsigned short wParam1);
void __fastcall NetSendCmdLocParam2(BOOL bHiPri, unsigned char bCmd, unsigned char x, unsigned char y, unsigned short wParam1, unsigned short wParam2);
void __fastcall NetSendCmdLocParam3(BOOL bHiPri, unsigned char bCmd, unsigned char x, unsigned char y, unsigned short wParam1, unsigned short wParam2, unsigned short wParam3);
void __fastcall NetSendCmdParam1(BOOL bHiPri, unsigned char bCmd, unsigned short wParam1);
void __fastcall NetSendCmdParam2(BOOL bHiPri, unsigned char bCmd, unsigned short wParam1, unsigned short wParam2);
void __fastcall NetSendCmdParam3(BOOL bHiPri, unsigned char bCmd, unsigned short wParam1, unsigned short wParam2, unsigned short wParam3);
void __fastcall NetSendCmdQuest(BOOL bHiPri, unsigned char q);
void __fastcall NetSendCmdGItem(BOOL bHiPri, unsigned char bCmd, unsigned char mast, unsigned char pnum, int ii);
void __fastcall NetSendCmdGItem2(unsigned char usonly, unsigned char bCmd, unsigned char mast, unsigned char pnum, struct TCmdGItem *p);
bool __fastcall NetSendCmdReq2(unsigned char bCmd, unsigned char mast, unsigned char pnum, struct TCmdGItem *p);
void __fastcall NetSendCmdExtra(struct TCmdGItem *p);
void __fastcall NetSendCmdPItem(BOOL bHiPri, unsigned char bCmd, unsigned char x, unsigned char y);
void __fastcall NetSendCmdChItem(BOOL bHiPri, unsigned char bLoc);
void __fastcall NetSendCmdDelItem(BOOL bHiPri, unsigned char bLoc);
void __fastcall NetSendCmdDItem(BOOL bHiPri, int ii);
void __fastcall NetSendCmdDamage(BOOL bHiPri, unsigned char bPlr, unsigned int dwDam);
void __fastcall NetSendCmdString(int a1, const char *pszStr);
void __fastcall RemovePlrPortal(int pnum);
int __fastcall ParseCmd(int pnum, TCmd *pCmd);
void __fastcall DeltaImportData(unsigned char cmd, int recv_offset);
void *__fastcall DeltaImportItem(void *src, void *dst);
void *__fastcall DeltaImportObject(void *src, void *dst);
void *__fastcall DeltaImportMonster(void *src, void *dst);
char __fastcall DeltaImportJunk(int a1);
int __fastcall On_SYNCDATA(void *packet, int pnum);
int __fastcall On_WALKXY(struct TCmdLoc *pCmd, int pnum);
int __fastcall On_ADDSTR(struct TCmdParam1 *pCmd, int pnum);
int __fastcall On_ADDMAG(struct TCmdParam1 *pCmd, int pnum);
int __fastcall On_ADDDEX(struct TCmdParam1 *pCmd, int pnum);
int __fastcall On_ADDVIT(struct TCmdParam1 *pCmd, int pnum);
int __fastcall On_SBSPELL(struct TCmdParam1 *pCmd, int pnum);
void msg_errorf(char *pszFmt, ...);
int __fastcall On_GOTOGETITEM(struct TCmdLocParam1 *pCmd, int pnum);
int __fastcall On_REQUESTGITEM(struct TCmdGItem *pCmd, int pnum);
bool __fastcall i_own_level(int nReqLevel);
int __fastcall On_GETITEM(struct TCmdGItem *pCmd, int pnum);
bool __fastcall delta_get_item(struct TCmdGItem *pI, unsigned char bLevel);
int __fastcall On_GOTOAGETITEM(struct TCmdLocParam1 *pCmd, int pnum);
int __fastcall On_REQUESTAGITEM(struct TCmdGItem *pCmd, int pnum);
int __fastcall On_AGETITEM(struct TCmdGItem *pCmd, int pnum);
int __fastcall On_ITEMEXTRA(struct TCmdGItem *pCmd, int pnum);
int __fastcall On_PUTITEM(struct TCmdPItem *pCmd, int pnum);
void __fastcall delta_put_item(struct TCmdPItem *pI, int x, int y, unsigned char bLevel);
void __fastcall check_update_plr(int pnum);
int __fastcall On_SYNCPUTITEM(struct TCmdPItem *pCmd, int pnum);
int __fastcall On_RESPAWNITEM(struct TCmdPItem *pCmd, int pnum);
int __fastcall On_ATTACKXY(struct TCmdLoc *pCmd, int pnum);
int __fastcall On_SATTACKXY(struct TCmdLoc *pCmd, int pnum);
int __fastcall On_RATTACKXY(struct TCmdLoc *pCmd, int pnum);
int __fastcall On_SPELLXYD(struct TCmdLocParam3 *pCmd, int pnum);
int __fastcall On_SPELLXY(struct TCmdLocParam2 *pCmd, int pnum);
int __fastcall On_TSPELLXY(struct TCmdLocParam2 *pCmd, int pnum);
int __fastcall On_OPOBJXY(struct TCmdLocParam1 *pCmd, int pnum);
int __fastcall On_DISARMXY(struct TCmdLocParam1 *pCmd, int pnum);
int __fastcall On_OPOBJT(struct TCmdParam1 *pCmd, int pnum);
int __fastcall On_ATTACKID(struct TCmdParam1 *pCmd, int pnum);
int __fastcall On_ATTACKPID(struct TCmdParam1 *pCmd, int pnum);
int __fastcall On_RATTACKID(struct TCmdParam1 *pCmd, int pnum);
int __fastcall On_RATTACKPID(struct TCmdParam1 *pCmd, int pnum);
int __fastcall On_SPELLID(struct TCmdLocParam2 *pCmd, int pnum);
int __fastcall On_SPELLPID(struct TCmdLocParam2 *pCmd, int pnum);
int __fastcall On_TSPELLID(struct TCmdLocParam2 *pCmd, int pnum);
int __fastcall On_TSPELLPID(struct TCmdLocParam2 *pCmd, int pnum);
int __fastcall On_KNOCKBACK(struct TCmdParam1 *pCmd, int pnum);
int __fastcall On_RESURRECT(struct TCmdParam1 *pCmd, int pnum);
int __fastcall On_HEALOTHER(struct TCmdParam1 *pCmd, int pnum);
int __fastcall On_TALKXY(struct TCmdLocParam1 *pCmd, int pnum);
int __fastcall On_NEWLVL(struct TCmdParam2 *pCmd, int pnum);
int __fastcall On_WARP(struct TCmdParam1 *pCmd, int pnum);
int __fastcall On_MONSTDEATH(struct TCmdLocParam1 *pCmd, int pnum);
int __fastcall On_KILLGOLEM(struct TCmdLocParam1 *pCmd, int pnum);
int __fastcall On_AWAKEGOLEM(struct TCmdGolem *pCmd, int pnum);
int __fastcall On_MONSTDAMAGE(struct TCmdLocParam1 *pCmd, int pnum);
int __fastcall On_PLRDEAD(struct TCmdParam1 *pCmd, int pnum);
int __fastcall On_PLRDAMAGE(struct TCmdDamage *pCmd, int pnum);
int __fastcall On_OPENDOOR(struct TCmdParam1 *pCmd, int pnum);
void __fastcall delta_sync_object(int oi, unsigned char bCmd, unsigned char bLevel);
int __fastcall On_CLOSEDOOR(struct TCmdParam1 *pCmd, int pnum);
int __fastcall On_OPERATEOBJ(struct TCmdParam1 *pCmd, int pnum);
int __fastcall On_PLROPOBJ(struct TCmdParam2 *pCmd, int pnum);
int __fastcall On_BREAKOBJ(struct TCmdParam2 *pCmd, int pnum);
int __fastcall On_CHANGEPLRITEMS(struct TCmdChItem *pCmd, int pnum);
int __fastcall On_DELPLRITEMS(struct TCmdDelItem *pCmd, int pnum);
int __fastcall On_PLRLEVEL(struct TCmdParam1 *pCmd, int pnum);
int __fastcall On_DROPITEM(struct TCmdPItem *pCmd, int pnum);
int __fastcall On_SEND_PLRINFO(struct TCmdPlrInfoHdr *pCmd, int pnum);
int __fastcall On_ACK_PLRINFO(struct TCmdPlrInfoHdr *pCmd, int pnum);
int __fastcall On_PLAYER_JOINLEVEL(struct TCmdLocParam1 *pCmd, int pnum);
int __fastcall On_ACTIVATEPORTAL(DJunk *pCmd, int pnum);
void __fastcall delta_open_portal(int pnum, int x, int y, int bLevel, int bLType, int bSetLvl);
int __fastcall On_DEACTIVATEPORTAL(struct TCmd *pCmd, int pnum);
int __fastcall On_RETOWN(struct TCmd *pCmd, int pnum);
int __fastcall On_SETSTR(struct TCmdParam1 *pCmd, int pnum);
int __fastcall On_SETDEX(struct TCmdParam1 *pCmd, int pnum);
int __fastcall On_SETMAG(struct TCmdParam1 *pCmd, int pnum);
int __fastcall On_SETVIT(struct TCmdParam1 *pCmd, int pnum);
int __fastcall On_STRING(struct TCmdString *pCmd, int pnum);
int __fastcall On_SYNCQUEST(struct TCmdQuest *pCmd, int pnum);
int __fastcall On_ENDSHIELD(int a1, int pnum);
#ifdef _DEBUG
int __fastcall On_CHEAT_EXPERIENCE(struct TCmd *pCmd, int pnum);
int __fastcall On_CHEAT_SPELL_LEVEL(struct TCmd *pCmd, int pnum);
#endif
int __cdecl On_DEBUG();
int __fastcall On_NOVA(struct TCmdLoc *pCmd, int pnum);
int __fastcall On_SETSHIELD(int unused, int pnum);
int __fastcall On_REMSHIELD(int unused, int pnum);

/* rdata */

extern const int msg_inf; // weak

#endif /* __MSG_H__ */
