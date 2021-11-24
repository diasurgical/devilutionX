/**
 * @file msg.h
 *
 * Interface of function for sending and reciving network messages.
 */
#pragma once

#include <cstdint>

#include "engine/point.hpp"
#include "items.h"
#include "monster.h"
#include "objects.h"
#include "portal.h"
#include "quests.h"

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
	CMD_AWAKEGOLEM,
	CMD_NOVA,
	CMD_SETSHIELD,
	CMD_REMSHIELD,
	CMD_SETREFLECT,
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
	uint8_t x;
	uint8_t y;
};

struct TCmdLocParam1 {
	_cmd_id bCmd;
	uint8_t x;
	uint8_t y;
	uint16_t wParam1;
};

struct TCmdLocParam2 {
	_cmd_id bCmd;
	uint8_t x;
	uint8_t y;
	uint16_t wParam1;
	uint16_t wParam2;
};

struct TCmdLocParam3 {
	_cmd_id bCmd;
	uint8_t x;
	uint8_t y;
	uint16_t wParam1;
	uint16_t wParam2;
	uint16_t wParam3;
};

struct TCmdLocParam4 {
	_cmd_id bCmd;
	uint8_t x;
	uint8_t y;
	uint16_t wParam1;
	uint16_t wParam2;
	uint16_t wParam3;
	uint16_t wParam4;
};

struct TCmdParam1 {
	_cmd_id bCmd;
	uint16_t wParam1;
};

struct TCmdParam2 {
	_cmd_id bCmd;
	uint16_t wParam1;
	uint16_t wParam2;
};

struct TCmdParam3 {
	_cmd_id bCmd;
	uint16_t wParam1;
	uint16_t wParam2;
	uint16_t wParam3;
};

struct TCmdParam4 {
	_cmd_id bCmd;
	uint16_t wParam1;
	uint16_t wParam2;
	uint16_t wParam3;
	uint16_t wParam4;
};

struct TCmdGolem {
	_cmd_id bCmd;
	uint8_t _mx;
	uint8_t _my;
	Direction _mdir;
	int8_t _menemy;
	int32_t _mhitpoints;
	uint8_t _currlevel;
};

struct TCmdQuest {
	_cmd_id bCmd;
	int8_t q;
	quest_state qstate;
	uint8_t qlog;
	uint8_t qvar1;
};

/**
 * Represents an item being picked up from the ground
 */
struct TCmdGItem {
	_cmd_id bCmd;
	uint8_t bMaster;
	uint8_t bPnum;
	uint8_t bCursitem;
	uint8_t bLevel;
	uint8_t x;
	uint8_t y;
	_item_indexes wIndx;
	uint16_t wCI;
	int32_t dwSeed;
	uint8_t bId;
	uint8_t bDur;
	uint8_t bMDur;
	uint8_t bCh;
	uint8_t bMCh;
	uint16_t wValue;
	uint32_t dwBuff;
	int32_t dwTime;
	uint16_t wToHit;
	uint16_t wMaxDam;
	uint8_t bMinStr;
	uint8_t bMinMag;
	uint8_t bMinDex;
	int16_t bAC;
};

/**
 * Represents an item being dropped onto the ground
 */
struct TCmdPItem {
	_cmd_id bCmd;
	uint8_t x;
	uint8_t y;
	_item_indexes wIndx;
	uint16_t wCI;
	/**
	 * Item identifier
	 * @see Item::_iSeed
	 */
	int32_t dwSeed;
	uint8_t bId;
	uint8_t bDur;
	uint8_t bMDur;
	uint8_t bCh;
	uint8_t bMCh;
	uint16_t wValue;
	uint32_t dwBuff;
	uint16_t wToHit;
	uint16_t wMaxDam;
	uint8_t bMinStr;
	uint8_t bMinMag;
	uint8_t bMinDex;
	int16_t bAC;
};

struct TCmdChItem {
	_cmd_id bCmd;
	uint8_t bLoc;
	uint16_t wIndx;
	uint16_t wCI;
	int32_t dwSeed;
	uint8_t bId;
	uint32_t dwBuff;
};

struct TCmdDelItem {
	_cmd_id bCmd;
	uint8_t bLoc;
};

struct TCmdDamage {
	_cmd_id bCmd;
	uint8_t bPlr;
	uint32_t dwDam;
};

struct TCmdMonDamage {
	_cmd_id bCmd;
	uint16_t wMon;
	uint32_t dwDam;
};

struct TCmdPlrInfoHdr {
	_cmd_id bCmd;
	uint16_t wOffset;
	uint16_t wBytes;
};

struct TCmdString {
	_cmd_id bCmd;
	char str[MAX_SEND_STR_LEN];
};

struct TFakeCmdPlr {
	_cmd_id bCmd;
	uint8_t bPlr;
};

struct TFakeDropPlr {
	_cmd_id bCmd;
	uint8_t bPlr;
	uint32_t dwReason;
};

struct TSyncHeader {
	_cmd_id bCmd;
	uint8_t bLevel;
	uint16_t wLen;
	uint8_t bItemI;
	uint8_t bItemX;
	uint8_t bItemY;
	uint16_t wItemIndx;
	uint16_t wItemCI;
	uint32_t dwItemSeed;
	uint8_t bItemId;
	uint8_t bItemDur;
	uint8_t bItemMDur;
	uint8_t bItemCh;
	uint8_t bItemMCh;
	uint16_t wItemVal;
	uint32_t dwItemBuff;
	uint8_t bPInvLoc;
	uint16_t wPInvIndx;
	uint16_t wPInvCI;
	uint32_t dwPInvSeed;
	uint8_t bPInvId;
};

struct TSyncMonster {
	uint8_t _mndx;
	uint8_t _mx;
	uint8_t _my;
	uint8_t _menemy;
	uint8_t _mdelta;
	int32_t _mhitpoints;
	int8_t mWhoHit;
};

struct TPktHdr {
	uint8_t px;
	uint8_t py;
	uint8_t targx;
	uint8_t targy;
	int32_t php;
	int32_t pmhp;
	uint8_t bstr;
	uint8_t bmag;
	uint8_t bdex;
	uint16_t wCheck;
	uint16_t wLen;
};

struct TPkt {
	TPktHdr hdr;
	byte body[493];
};

struct DMonsterStr {
	uint8_t _mx;
	uint8_t _my;
	Direction _mdir;
	uint8_t _menemy;
	uint8_t _mactive;
	int32_t _mhitpoints;
	int8_t mWhoHit;
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
	uint8_t automapsv[DMAXX][DMAXY];
};

struct DPortal {
	uint8_t x;
	uint8_t y;
	uint8_t level;
	uint8_t ltype;
	uint8_t setlvl;
};

struct MultiQuests {
	quest_state qstate;
	uint8_t qlog;
	uint8_t qvar1;
};

struct DJunk {
	DPortal portal[MAXPORTAL];
	MultiQuests quests[MAXMULTIQUESTS];
};
#pragma pack(pop)

struct TBuffer {
	uint32_t dwNextWriteOffset;
	byte bData[4096];
};

extern bool deltaload;
extern BYTE gbBufferMsgs;
extern int dwRecCount;

void msg_send_drop_pkt(int pnum, int reason);
bool msg_wait_resync();
void run_delta_info();
void DeltaExportData(int pnum);
void delta_init();
void delta_kill_monster(int mi, Point position, BYTE bLevel);
void delta_monster_hp(int mi, int hp, BYTE bLevel);
void delta_sync_monster(const TSyncMonster &monsterSync, uint8_t level);
bool delta_portal_inited(int i);
bool delta_quest_inited(int i);
void DeltaAddItem(int ii);
void DeltaSaveLevel();
void DeltaLoadLevel();
void NetSendCmd(bool bHiPri, _cmd_id bCmd);
void NetSendCmdGolem(BYTE mx, BYTE my, Direction dir, BYTE menemy, int hp, BYTE cl);
void NetSendCmdLoc(int playerId, bool bHiPri, _cmd_id bCmd, Point position);
void NetSendCmdLocParam1(bool bHiPri, _cmd_id bCmd, Point position, uint16_t wParam1);
void NetSendCmdLocParam2(bool bHiPri, _cmd_id bCmd, Point position, uint16_t wParam1, uint16_t wParam2);
void NetSendCmdLocParam3(bool bHiPri, _cmd_id bCmd, Point position, uint16_t wParam1, uint16_t wParam2, uint16_t wParam3);
void NetSendCmdLocParam4(bool bHiPri, _cmd_id bCmd, Point position, uint16_t wParam1, uint16_t wParam2, uint16_t wParam3, uint16_t wParam4);
void NetSendCmdParam1(bool bHiPri, _cmd_id bCmd, uint16_t wParam1);
void NetSendCmdParam2(bool bHiPri, _cmd_id bCmd, uint16_t wParam1, uint16_t wParam2);
void NetSendCmdParam3(bool bHiPri, _cmd_id bCmd, uint16_t wParam1, uint16_t wParam2, uint16_t wParam3);
void NetSendCmdParam4(bool bHiPri, _cmd_id bCmd, uint16_t wParam1, uint16_t wParam2, uint16_t wParam3, uint16_t wParam4);
void NetSendCmdQuest(bool bHiPri, const Quest &quest);
void NetSendCmdGItem(bool bHiPri, _cmd_id bCmd, BYTE mast, BYTE pnum, BYTE ii);
void NetSendCmdPItem(bool bHiPri, _cmd_id bCmd, Point position);
void NetSendCmdChItem(bool bHiPri, BYTE bLoc);
void NetSendCmdDelItem(bool bHiPri, BYTE bLoc);
void NetSendCmdDItem(bool bHiPri, int ii);
void NetSendCmdDamage(bool bHiPri, uint8_t bPlr, uint32_t dwDam);
void NetSendCmdMonDmg(bool bHiPri, uint16_t wMon, uint32_t dwDam);
void NetSendCmdString(uint32_t pmask, const char *pszStr);
void delta_close_portal(int pnum);
uint32_t ParseCmd(int pnum, const TCmd *pCmd);

} // namespace devilution
