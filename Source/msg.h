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

#pragma pack(push, 1)
typedef struct TCmd {
	Uint8 bCmd;
} TCmd;

typedef struct TCmdLoc {
	Uint8 bCmd;
	Uint8 x;
	Uint8 y;
} TCmdLoc;

typedef struct TCmdLocParam1 {
	Uint8 bCmd;
	Uint8 x;
	Uint8 y;
	Uint16 wParam1;
} TCmdLocParam1;

typedef struct TCmdLocParam2 {
	Uint8 bCmd;
	Uint8 x;
	Uint8 y;
	Uint16 wParam1;
	Uint16 wParam2;
} TCmdLocParam2;

typedef struct TCmdLocParam3 {
	Uint8 bCmd;
	Uint8 x;
	Uint8 y;
	Uint16 wParam1;
	Uint16 wParam2;
	Uint16 wParam3;
} TCmdLocParam3;

typedef struct TCmdParam1 {
	Uint8 bCmd;
	Uint16 wParam1;
} TCmdParam1;

typedef struct TCmdParam2 {
	Uint8 bCmd;
	Uint16 wParam1;
	Uint16 wParam2;
} TCmdParam2;

typedef struct TCmdParam3 {
	Uint8 bCmd;
	Uint16 wParam1;
	Uint16 wParam2;
	Uint16 wParam3;
} TCmdParam3;

typedef struct TCmdGolem {
	Uint8 bCmd;
	Uint8 _mx;
	Uint8 _my;
	Uint8 _mdir;
	Sint8 _menemy;
	Sint32 _mhitpoints;
	Uint8 _currlevel;
} TCmdGolem;

typedef struct TCmdQuest {
	Uint8 bCmd;
	Uint8 q;
	Uint8 qstate;
	Uint8 qlog;
	Uint8 qvar1;
} TCmdQuest;

typedef struct TCmdGItem {
	Uint8 bCmd;
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
	Uint8 bAC;
} TCmdGItem;

typedef struct TCmdPItem {
	Uint8 bCmd;
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
	Uint8 bAC;
} TCmdPItem;

typedef struct TCmdChItem {
	Uint8 bCmd;
	Uint8 bLoc;
	Uint16 wIndx;
	Uint16 wCI;
	Sint32 dwSeed;
	Uint8 bId;
	Uint32 dwBuff;
} TCmdChItem;

typedef struct TCmdDelItem {
	Uint8 bCmd;
	Uint8 bLoc;
} TCmdDelItem;

typedef struct TCmdDamage {
	Uint8 bCmd;
	Uint8 bPlr;
	Uint32 dwDam;
} TCmdDamage;

typedef struct TCmdMonDamage {
	Uint8 bCmd;
	Uint16 wMon;
	Uint32 dwDam;
} TCmdMonDamage;

typedef struct TCmdPlrInfoHdr {
	Uint8 bCmd;
	Uint16 wOffset;
	Uint16 wBytes;
} TCmdPlrInfoHdr;

typedef struct TCmdString {
	Uint8 bCmd;
	char str[MAX_SEND_STR_LEN];
} TCmdString;

typedef struct TFakeCmdPlr {
	Uint8 bCmd;
	Uint8 bPlr;
} TFakeCmdPlr;

typedef struct TFakeDropPlr {
	Uint8 bCmd;
	Uint8 bPlr;
	Uint32 dwReason;
} TFakeDropPlr;

typedef struct TSyncHeader {
	Uint8 bCmd;
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
} TSyncHeader;

typedef struct TSyncMonster {
	Uint8 _mndx;
	Uint8 _mx;
	Uint8 _my;
	Uint8 _menemy;
	Uint8 _mdelta;
} TSyncMonster;

typedef struct TPktHdr {
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
} TPktHdr;

typedef struct TPkt {
	TPktHdr hdr;
	Uint8 body[493];
} TPkt;

typedef struct DMonsterStr {
	Uint8 _mx;
	Uint8 _my;
	Uint8 _mdir;
	Uint8 _menemy;
	Uint8 _mactive;
	Sint32 _mhitpoints;
} DMonsterStr;

typedef struct DObjectStr {
	Uint8 bCmd;
} DObjectStr;

typedef struct DLevel {
	TCmdPItem item[MAXITEMS];
	DObjectStr object[MAXOBJECTS];
	DMonsterStr monster[MAXMONSTERS];
} DLevel;

typedef struct LocalLevel {
	Uint8 automapsv[DMAXX][DMAXY];
} LocalLevel;

typedef struct DPortal {
	Uint8 x;
	Uint8 y;
	Uint8 level;
	Uint8 ltype;
	Uint8 setlvl;
} DPortal;

typedef struct MultiQuests {
	Uint8 qstate;
	Uint8 qlog;
	Uint8 qvar1;
} MultiQuests;

typedef struct DJunk {
	DPortal portal[MAXPORTAL];
	MultiQuests quests[MAXMULTIQUESTS];
} DJunk;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct TMegaPkt {
	struct TMegaPkt *pNext;
	Uint32 dwSpaceLeft;
	Uint8 data[32000];
} TMegaPkt;
#pragma pack(pop)

typedef struct TBuffer {
	Uint32 dwNextWriteOffset;
	Uint8 bData[4096];
} TBuffer;

extern BOOL deltaload;
extern BYTE gbBufferMsgs;
extern int dwRecCount;

void msg_send_drop_pkt(int pnum, int reason);
BOOL msg_wait_resync();
void run_delta_info();
void DeltaExportData(int pnum);
void delta_init();
void delta_kill_monster(int mi, BYTE x, BYTE y, BYTE bLevel);
void delta_monster_hp(int mi, int hp, BYTE bLevel);
void delta_sync_monster(const TSyncMonster *pSync, BYTE bLevel);
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
void NetSendCmdPItem(BOOL bHiPri, BYTE bCmd, BYTE x, BYTE y);
void NetSendCmdChItem(BOOL bHiPri, BYTE bLoc);
void NetSendCmdDelItem(BOOL bHiPri, BYTE bLoc);
void NetSendCmdDItem(BOOL bHiPri, int ii);
void NetSendCmdDamage(BOOL bHiPri, BYTE bPlr, DWORD dwDam);
void NetSendCmdMonDmg(BOOL bHiPri, WORD bMon, DWORD dwDam);
void NetSendCmdString(int pmask, const char *pszStr);
void delta_close_portal(int pnum);
DWORD ParseCmd(int pnum, TCmd *pCmd);

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __MSG_H__ */
