/**
 * @file msg.h
 *
 * Interface of function for sending and reciving network messages.
 */
#pragma once

#include <stdint.h>

#include "quests.h"
#include "objects.h"
#include "monster.h"
#include "portal.h"
#include "items.h"

namespace devilution {

#define MAX_SEND_STR_LEN 80
#define MAXMULTIQUESTS 10

enum _cmd_id : uint8_t {
	CMD_STAND,
	CMD_WALKXY,
	CMD_ACK_PLRINFO,
	CMD_ADDSTR,
	CMD_ADDMAG,
	CMD_ADDDEX,
	CMD_ADDVIT,
	CMD_SBSPELL,
	CMD_GETITEM,
	CMD_AGETITEM,
	CMD_PUTITEM,
	CMD_RESPAWNITEM,
	CMD_ATTACKXY,
	CMD_RATTACKXY,
	CMD_SPELLXY,
	CMD_TSPELLXY,
	CMD_OPOBJXY,
	CMD_DISARMXY,
	CMD_ATTACKID,
	CMD_ATTACKPID,
	CMD_RATTACKID,
	CMD_RATTACKPID,
	CMD_SPELLID,
	CMD_SPELLPID,
	CMD_TSPELLID,
	CMD_TSPELLPID,
	CMD_RESURRECT,
	CMD_OPOBJT,
	CMD_KNOCKBACK,
	CMD_TALKXY,
	CMD_NEWLVL,
	CMD_WARP,
	CMD_CHEAT_EXPERIENCE,
	CMD_CHEAT_SPELL_LEVEL,
	CMD_DEBUG,
	CMD_SYNCDATA,
	CMD_MONSTDEATH,
	CMD_MONSTDAMAGE,
	CMD_PLRDEAD,
	CMD_REQUESTGITEM,
	CMD_REQUESTAGITEM,
	CMD_GOTOGETITEM,
	CMD_GOTOAGETITEM,
	CMD_OPENDOOR,
	CMD_CLOSEDOOR,
	CMD_OPERATEOBJ,
	CMD_PLROPOBJ,
	CMD_BREAKOBJ,
	CMD_CHANGEPLRITEMS,
	CMD_DELPLRITEMS,
	CMD_PLRDAMAGE,
	CMD_PLRLEVEL,
	CMD_DROPITEM,
	CMD_PLAYER_JOINLEVEL,
	CMD_SEND_PLRINFO,
	CMD_SATTACKXY,
	CMD_ACTIVATEPORTAL,
	CMD_DEACTIVATEPORTAL,
	CMD_DLEVEL_0,
	CMD_DLEVEL_1,
	CMD_DLEVEL_2,
	CMD_DLEVEL_3,
	CMD_DLEVEL_4,
	CMD_DLEVEL_5,
	CMD_DLEVEL_6,
	CMD_DLEVEL_7,
	CMD_DLEVEL_8,
	CMD_DLEVEL_9,
	CMD_DLEVEL_10,
	CMD_DLEVEL_11,
	CMD_DLEVEL_12,
	CMD_DLEVEL_13,
	CMD_DLEVEL_14,
	CMD_DLEVEL_15,
	CMD_DLEVEL_16,
	CMD_DLEVEL_17,
	CMD_DLEVEL_18,
	CMD_DLEVEL_19,
	CMD_DLEVEL_20,
	CMD_DLEVEL_21,
	CMD_DLEVEL_22,
	CMD_DLEVEL_23,
	CMD_DLEVEL_24,
	CMD_DLEVEL_JUNK,
	CMD_DLEVEL_END,
	CMD_HEALOTHER,
	CMD_STRING,
	CMD_SETSTR,
	CMD_SETMAG,
	CMD_SETDEX,
	CMD_SETVIT,
	CMD_RETOWN,
	CMD_SPELLXYD,
	CMD_ITEMEXTRA,
	CMD_SYNCPUTITEM,
	CMD_KILLGOLEM,
	CMD_SYNCQUEST,
	CMD_ENDSHIELD,
	CMD_AWAKEGOLEM,
	CMD_NOVA,
	CMD_SETSHIELD,
	CMD_REMSHIELD,
	CMD_REFLECT,
	CMD_NAKRUL,
	CMD_OPENHIVE,
	CMD_OPENCRYPT,
	FAKE_CMD_SETID,
	FAKE_CMD_DROPID,
	NUM_CMDS,
	CMD_INVALID = 0xFF,
};

#pragma pack(push, 1)
struct TCmd {
	_cmd_id bCmd;
};

struct TCmdLoc {
	_cmd_id bCmd;
	Uint8 x;
	Uint8 y;
};

struct TCmdLocParam1 {
	_cmd_id bCmd;
	Uint8 x;
	Uint8 y;
	Uint16 wParam1;
};

struct TCmdLocParam2 {
	_cmd_id bCmd;
	Uint8 x;
	Uint8 y;
	Uint16 wParam1;
	Uint16 wParam2;
};

struct TCmdLocParam3 {
	_cmd_id bCmd;
	Uint8 x;
	Uint8 y;
	Uint16 wParam1;
	Uint16 wParam2;
	Uint16 wParam3;
};

struct TCmdParam1 {
	_cmd_id bCmd;
	Uint16 wParam1;
};

struct TCmdParam2 {
	_cmd_id bCmd;
	Uint16 wParam1;
	Uint16 wParam2;
};

struct TCmdParam3 {
	_cmd_id bCmd;
	Uint16 wParam1;
	Uint16 wParam2;
	Uint16 wParam3;
};

struct TCmdGolem {
	_cmd_id bCmd;
	Uint8 _mx;
	Uint8 _my;
	direction _mdir;
	Sint8 _menemy;
	Sint32 _mhitpoints;
	Uint8 _currlevel;
};

struct TCmdQuest {
	_cmd_id bCmd;
	Uint8 q;
	quest_state qstate;
	Uint8 qlog;
	Uint8 qvar1;
};

struct TCmdGItem {
	_cmd_id bCmd;
	Uint8 bMaster;
	Uint8 bPnum;
	Uint8 bCursitem;
	Uint8 bLevel;
	Uint8 x;
	Uint8 y;
	Uint16 wIndx;
	Uint16 wCI;
	Sint32 dwSeed;
	Uint8 bId;
	Uint8 bDur;
	Uint8 bMDur;
	Uint8 bCh;
	Uint8 bMCh;
	Uint16 wValue;
	Uint32 dwBuff;
	Sint32 dwTime;
	Uint16 wToHit;
	Uint16 wMaxDam;
	Uint8 bMinStr;
	Uint8 bMinMag;
	Uint8 bMinDex;
	Sint16 bAC;
};

struct TCmdPItem {
	_cmd_id bCmd;
	Uint8 x;
	Uint8 y;
	Uint16 wIndx;
	Uint16 wCI;
	Sint32 dwSeed;
	Uint8 bId;
	Uint8 bDur;
	Uint8 bMDur;
	Uint8 bCh;
	Uint8 bMCh;
	Uint16 wValue;
	Uint32 dwBuff;
	Uint16 wToHit;
	Uint16 wMaxDam;
	Uint8 bMinStr;
	Uint8 bMinMag;
	Uint8 bMinDex;
	Sint16 bAC;
};

struct TCmdChItem {
	_cmd_id bCmd;
	Uint8 bLoc;
	Uint16 wIndx;
	Uint16 wCI;
	Sint32 dwSeed;
	Uint8 bId;
	Uint32 dwBuff;
};

struct TCmdDelItem {
	_cmd_id bCmd;
	Uint8 bLoc;
};

struct TCmdDamage {
	_cmd_id bCmd;
	Uint8 bPlr;
	Uint32 dwDam;
};

struct TCmdMonDamage {
	_cmd_id bCmd;
	Uint16 wMon;
	Uint32 dwDam;
};

struct TCmdPlrInfoHdr {
	_cmd_id bCmd;
	Uint16 wOffset;
	Uint16 wBytes;
};

struct TCmdString {
	_cmd_id bCmd;
	char str[MAX_SEND_STR_LEN];
};

struct TFakeCmdPlr {
	_cmd_id bCmd;
	Uint8 bPlr;
};

struct TFakeDropPlr {
	_cmd_id bCmd;
	Uint8 bPlr;
	Uint32 dwReason;
};

struct TSyncHeader {
	_cmd_id bCmd;
	Uint8 bLevel;
	Uint16 wLen;
	Uint8 bObjId;
	Uint8 bObjCmd;
	Uint8 bItemI;
	Uint8 bItemX;
	Uint8 bItemY;
	Uint16 wItemIndx;
	Uint16 wItemCI;
	Uint32 dwItemSeed;
	Uint8 bItemId;
	Uint8 bItemDur;
	Uint8 bItemMDur;
	Uint8 bItemCh;
	Uint8 bItemMCh;
	Uint16 wItemVal;
	Uint32 dwItemBuff;
	Uint8 bPInvLoc;
	Uint16 wPInvIndx;
	Uint16 wPInvCI;
	Uint32 dwPInvSeed;
	Uint8 bPInvId;
	Uint16 wToHit;
	Uint16 wMaxDam;
	Uint8 bMinStr;
	Uint8 bMinMag;
	Uint8 bMinDex;
	Uint8 bAC;
};

struct TSyncMonster {
	Uint8 _mndx;
	Uint8 _mx;
	Uint8 _my;
	Uint8 _menemy;
	Uint8 _mdelta;
};

struct TPktHdr {
	Uint8 px;
	Uint8 py;
	Uint8 targx;
	Uint8 targy;
	Sint32 php;
	Sint32 pmhp;
	Uint8 bstr;
	Uint8 bmag;
	Uint8 bdex;
	Uint16 wCheck;
	Uint16 wLen;
};

struct TPkt {
	TPktHdr hdr;
	Uint8 body[493];
};

struct DMonsterStr {
	Uint8 _mx;
	Uint8 _my;
	direction _mdir;
	Uint8 _menemy;
	Uint8 _mactive;
	Sint32 _mhitpoints;
};

struct DObjectStr {
	_cmd_id bCmd;
};

struct DLevel {
	TCmdPItem item[MAXITEMS];
	DObjectStr object[MAXOBJECTS];
	DMonsterStr monster[MAXMONSTERS];
};

struct LocalLevel {
	Uint8 automapsv[DMAXX][DMAXY];
};

struct DPortal {
	Uint8 x;
	Uint8 y;
	Uint8 level;
	Uint8 ltype;
	Uint8 setlvl;
};

struct MultiQuests {
	quest_state qstate;
	Uint8 qlog;
	Uint8 qvar1;
};

struct DJunk {
	DPortal portal[MAXPORTAL];
	MultiQuests quests[MAXMULTIQUESTS];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct TMegaPkt {
	struct TMegaPkt *pNext;
	Uint32 dwSpaceLeft;
	Uint8 data[32000];
};
#pragma pack(pop)

struct TBuffer {
	Uint32 dwNextWriteOffset;
	Uint8 bData[4096];
};

extern bool deltaload;
extern BYTE gbBufferMsgs;
extern int dwRecCount;

void msg_send_drop_pkt(int pnum, int reason);
bool msg_wait_resync();
void run_delta_info();
void DeltaExportData(int pnum);
void delta_init();
void delta_kill_monster(int mi, BYTE x, BYTE y, BYTE bLevel);
void delta_monster_hp(int mi, int hp, BYTE bLevel);
void delta_sync_monster(const TSyncMonster *pSync, BYTE bLevel);
bool delta_portal_inited(int i);
bool delta_quest_inited(int i);
void DeltaAddItem(int ii);
void DeltaSaveLevel();
void DeltaLoadLevel();
void NetSendCmd(bool bHiPri, _cmd_id bCmd);
void NetSendCmdGolem(BYTE mx, BYTE my, direction dir, BYTE menemy, int hp, BYTE cl);
void NetSendCmdLoc(int playerId, bool bHiPri, _cmd_id bCmd, BYTE x, BYTE y);
void NetSendCmdLocParam1(bool bHiPri, _cmd_id bCmd, BYTE x, BYTE y, WORD wParam1);
void NetSendCmdLocParam2(bool bHiPri, _cmd_id bCmd, BYTE x, BYTE y, WORD wParam1, WORD wParam2);
void NetSendCmdLocParam3(bool bHiPri, _cmd_id bCmd, BYTE x, BYTE y, WORD wParam1, WORD wParam2, WORD wParam3);
void NetSendCmdParam1(bool bHiPri, _cmd_id bCmd, WORD wParam1);
void NetSendCmdParam2(bool bHiPri, _cmd_id bCmd, WORD wParam1, WORD wParam2);
void NetSendCmdParam3(bool bHiPri, _cmd_id bCmd, WORD wParam1, WORD wParam2, WORD wParam3);
void NetSendCmdQuest(bool bHiPri, BYTE q);
void NetSendCmdGItem(bool bHiPri, _cmd_id bCmd, BYTE mast, BYTE pnum, BYTE ii);
void NetSendCmdPItem(bool bHiPri, _cmd_id bCmd, BYTE x, BYTE y);
void NetSendCmdChItem(bool bHiPri, BYTE bLoc);
void NetSendCmdDelItem(bool bHiPri, BYTE bLoc);
void NetSendCmdDItem(bool bHiPri, int ii);
void NetSendCmdDamage(bool bHiPri, BYTE bPlr, DWORD dwDam);
void NetSendCmdMonDmg(bool bHiPri, WORD bMon, DWORD dwDam);
void NetSendCmdString(uint32_t pmask, const char *pszStr);
void delta_close_portal(int pnum);
DWORD ParseCmd(int pnum, TCmd *pCmd);

} // namespace devilution
