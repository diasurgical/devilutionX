/**
 * @file msg.h
 *
 * Interface of function for sending and reciving network messages.
 */
#ifndef __MSG_H__
#define __MSG_H__

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

extern BOOL deltaload;
extern BYTE gbBufferMsgs;
extern int dwRecCount;

void msg_send_drop_pkt(int pnum, int reason);
void msg_send_packet(int pnum, const void *packet, DWORD dwSize);
void msg_get_next_packet();
BOOL msg_wait_resync();
void msg_free_packets();
int msg_wait_for_turns();
void run_delta_info();
void msg_pre_packet();
void DeltaExportData(int pnum);
BYTE *DeltaExportItem(BYTE *dst, TCmdPItem *src);
BYTE *DeltaExportObject(BYTE *dst, DObjectStr *src);
BYTE *DeltaExportMonster(BYTE *dst, DMonsterStr *src);
BYTE *DeltaExportJunk(BYTE *dst);
int msg_comp_level(BYTE *buffer, BYTE *end);
void delta_init();
void delta_kill_monster(int mi, BYTE x, BYTE y, BYTE bLevel);
void delta_monster_hp(int mi, int hp, BYTE bLevel);
void delta_sync_monster(const TSyncMonster *pSync, BYTE bLevel);
void delta_sync_golem(TCmdGolem *pG, int pnum, BYTE bLevel);
void delta_leave_sync(BYTE bLevel);
BOOL delta_portal_inited(int i);
BOOL delta_quest_inited(int i);
void DeltaAddItem(int ii);
void DeltaSaveLevel();
void DeltaLoadLevel();
void NetSendCmd(BOOL bHiPri, BYTE bCmd);
void NetSendCmdGolem(BYTE mx, BYTE my, BYTE dir, BYTE menemy, int hp, BYTE cl);
void NetSendCmdLoc(BOOL bHiPri, BYTE bCmd, BYTE x, BYTE y);
void NetSendCmdLocParam1(BOOL bHiPri, BYTE bCmd, BYTE x, BYTE y, WORD wParam1);
void NetSendCmdLocParam2(BOOL bHiPri, BYTE bCmd, BYTE x, BYTE y, WORD wParam1, WORD wParam2);
void NetSendCmdLocParam3(BOOL bHiPri, BYTE bCmd, BYTE x, BYTE y, WORD wParam1, WORD wParam2, WORD wParam3);
void NetSendCmdParam1(BOOL bHiPri, BYTE bCmd, WORD wParam1);
void NetSendCmdParam2(BOOL bHiPri, BYTE bCmd, WORD wParam1, WORD wParam2);
void NetSendCmdParam3(BOOL bHiPri, BYTE bCmd, WORD wParam1, WORD wParam2, WORD wParam3);
void NetSendCmdQuest(BOOL bHiPri, BYTE q);
void NetSendCmdGItem(BOOL bHiPri, BYTE bCmd, BYTE mast, BYTE pnum, BYTE ii);
void NetSendCmdGItem2(BOOL usonly, BYTE bCmd, BYTE mast, BYTE pnum, TCmdGItem *p);
BOOL NetSendCmdReq2(BYTE bCmd, BYTE mast, BYTE pnum, TCmdGItem *p);
void NetSendCmdExtra(TCmdGItem *p);
void NetSendCmdPItem(BOOL bHiPri, BYTE bCmd, BYTE x, BYTE y);
void NetSendCmdChItem(BOOL bHiPri, BYTE bLoc);
void NetSendCmdDelItem(BOOL bHiPri, BYTE bLoc);
void NetSendCmdDItem(BOOL bHiPri, int ii);
void NetSendCmdDamage(BOOL bHiPri, BYTE bPlr, DWORD dwDam);
void NetSendCmdString(int pmask, const char *pszStr);
void delta_close_portal(int pnum);
DWORD ParseCmd(int pnum, TCmd *pCmd);
DWORD On_DLEVEL(int pnum, TCmd *pCmd);
void DeltaImportData(BYTE cmd, DWORD recv_offset);
BYTE *DeltaImportItem(BYTE *src, TCmdPItem *dst);
BYTE *DeltaImportObject(BYTE *src, DObjectStr *dst);
BYTE *DeltaImportMonster(BYTE *src, DMonsterStr *dst);
void DeltaImportJunk(BYTE *src);
DWORD On_SYNCDATA(TCmd *pCmd, int pnum);
DWORD On_WALKXY(TCmd *pCmd, int pnum);
DWORD On_ADDSTR(TCmd *pCmd, int pnum);
DWORD On_ADDMAG(TCmd *pCmd, int pnum);
DWORD On_ADDDEX(TCmd *pCmd, int pnum);
DWORD On_ADDVIT(TCmd *pCmd, int pnum);
DWORD On_SBSPELL(TCmd *pCmd, int pnum);
void msg_errorf(const char *pszFmt, ...);
DWORD On_GOTOGETITEM(TCmd *pCmd, int pnum);
DWORD On_REQUESTGITEM(TCmd *pCmd, int pnum);
BOOL i_own_level(int nReqLevel);
DWORD On_GETITEM(TCmd *pCmd, int pnum);
BOOL delta_get_item(TCmdGItem *pI, BYTE bLevel);
DWORD On_GOTOAGETITEM(TCmd *pCmd, int pnum);
DWORD On_REQUESTAGITEM(TCmd *pCmd, int pnum);
DWORD On_AGETITEM(TCmd *pCmd, int pnum);
DWORD On_ITEMEXTRA(TCmd *pCmd, int pnum);
DWORD On_PUTITEM(TCmd *pCmd, int pnum);
void delta_put_item(TCmdPItem *pI, int x, int y, BYTE bLevel);
void check_update_plr(int pnum);
DWORD On_SYNCPUTITEM(TCmd *pCmd, int pnum);
DWORD On_RESPAWNITEM(TCmd *pCmd, int pnum);
DWORD On_ATTACKXY(TCmd *pCmd, int pnum);
DWORD On_SATTACKXY(TCmd *pCmd, int pnum);
DWORD On_RATTACKXY(TCmd *pCmd, int pnum);
DWORD On_SPELLXYD(TCmd *pCmd, int pnum);
DWORD On_SPELLXY(TCmd *pCmd, int pnum);
DWORD On_TSPELLXY(TCmd *pCmd, int pnum);
DWORD On_OPOBJXY(TCmd *pCmd, int pnum);
DWORD On_DISARMXY(TCmd *pCmd, int pnum);
DWORD On_OPOBJT(TCmd *pCmd, int pnum);
DWORD On_ATTACKID(TCmd *pCmd, int pnum);
DWORD On_ATTACKPID(TCmd *pCmd, int pnum);
DWORD On_RATTACKID(TCmd *pCmd, int pnum);
DWORD On_RATTACKPID(TCmd *pCmd, int pnum);
DWORD On_SPELLID(TCmd *pCmd, int pnum);
DWORD On_SPELLPID(TCmd *pCmd, int pnum);
DWORD On_TSPELLID(TCmd *pCmd, int pnum);
DWORD On_TSPELLPID(TCmd *pCmd, int pnum);
DWORD On_KNOCKBACK(TCmd *pCmd, int pnum);
DWORD On_RESURRECT(TCmd *pCmd, int pnum);
DWORD On_HEALOTHER(TCmd *pCmd, int pnum);
DWORD On_TALKXY(TCmd *pCmd, int pnum);
DWORD On_NEWLVL(TCmd *pCmd, int pnum);
DWORD On_WARP(TCmd *pCmd, int pnum);
DWORD On_MONSTDEATH(TCmd *pCmd, int pnum);
DWORD On_KILLGOLEM(TCmd *pCmd, int pnum);
DWORD On_AWAKEGOLEM(TCmd *pCmd, int pnum);
DWORD On_MONSTDAMAGE(TCmd *pCmd, int pnum);
DWORD On_PLRDEAD(TCmd *pCmd, int pnum);
DWORD On_PLRDAMAGE(TCmd *pCmd, int pnum);
DWORD On_OPENDOOR(TCmd *pCmd, int pnum);
void delta_sync_object(int oi, BYTE bCmd, BYTE bLevel);
DWORD On_CLOSEDOOR(TCmd *pCmd, int pnum);
DWORD On_OPERATEOBJ(TCmd *pCmd, int pnum);
DWORD On_PLROPOBJ(TCmd *pCmd, int pnum);
DWORD On_BREAKOBJ(TCmd *pCmd, int pnum);
DWORD On_CHANGEPLRITEMS(TCmd *pCmd, int pnum);
DWORD On_DELPLRITEMS(TCmd *pCmd, int pnum);
DWORD On_PLRLEVEL(TCmd *pCmd, int pnum);
DWORD On_DROPITEM(TCmd *pCmd, int pnum);
DWORD On_SEND_PLRINFO(TCmd *pCmd, int pnum);
DWORD On_ACK_PLRINFO(TCmd *pCmd, int pnum);
DWORD On_PLAYER_JOINLEVEL(TCmd *pCmd, int pnum);
DWORD On_ACTIVATEPORTAL(TCmd *pCmd, int pnum);
void delta_open_portal(int pnum, BYTE x, BYTE y, BYTE bLevel, BYTE bLType, BYTE bSetLvl);
DWORD On_DEACTIVATEPORTAL(TCmd *pCmd, int pnum);
DWORD On_RETOWN(TCmd *pCmd, int pnum);
DWORD On_SETSTR(TCmd *pCmd, int pnum);
DWORD On_SETDEX(TCmd *pCmd, int pnum);
DWORD On_SETMAG(TCmd *pCmd, int pnum);
DWORD On_SETVIT(TCmd *pCmd, int pnum);
DWORD On_STRING(TCmd *pCmd, int pnum);
DWORD On_STRING2(int pnum, TCmd *pCmd);
DWORD On_SYNCQUEST(TCmd *pCmd, int pnum);
DWORD On_ENDSHIELD(TCmd *pCmd, int pnum);
DWORD On_CHEAT_EXPERIENCE(TCmd *pCmd, int pnum);
DWORD On_CHEAT_SPELL_LEVEL(TCmd *pCmd, int pnum);
DWORD On_DEBUG(TCmd *pCmd, int pnum);
DWORD On_NOVA(TCmd *pCmd, int pnum);
DWORD On_SETSHIELD(TCmd *pCmd, int pnum);
DWORD On_REMSHIELD(TCmd *pCmd, int pnum);

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __MSG_H__ */
