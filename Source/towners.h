/**
 * @file towners.h
 *
 * Interface of functionality for loading and spawning towners.
 */
#ifndef __TOWNERS_H__
#define __TOWNERS_H__

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

typedef struct TNQ {
	Uint8 _qsttype;
	Uint8 _qstmsg;
	bool _qstmsgact;
} TNQ;

typedef struct TownerStruct {
	int _tmode;
	int _ttype;
	int _tx;    // Tile X-position of NPC
	int _ty;    // Tile Y-position of NPC
	int _txoff; // Sprite X-offset (unused)
	int _tyoff; // Sprite Y-offset (unused)
	int _txvel; // X-velocity during movement (unused)
	int _tyvel; // Y-velocity during movement (unused)
	int _tdir;  // Facing of NPC (unused)
	Uint8 *_tAnimData;
	int _tAnimDelay; // Tick length of each frame in the current animation
	int _tAnimCnt;   // Increases by one each game tick, counting how close we are to _pAnimDelay
	int _tAnimLen;   // Number of frames in current animation
	int _tAnimFrame; // Current frame of animation.
	int _tAnimFrameCnt;
	Sint8 _tAnimOrder;
	int _tAnimWidth;
	int _tAnimWidth2;
	int _tTenPer;
	int _teflag;
	int _tbtcnt;
	int _tSelFlag;
	bool _tMsgSaid;
	TNQ qsts[MAXQUESTS];
	int _tSeed;
	int _tVar1;
	int _tVar2;
	int _tVar3;
	int _tVar4;
	char _tName[PLR_NAME_LEN];
	Uint8 *_tNAnim[8];
	int _tNFrames;
	Uint8 *_tNData;
} TownerStruct;

typedef struct QuestTalkData {
	int _qinfra;
	int _qblkm;
	int _qgarb;
	int _qzhar;
	int _qveil;
	int _qmod;
	int _qbutch;
	int _qbol;
	int _qblind;
	int _qblood;
	int _qanvil;
	int _qwarlrd;
	int _qking;
	int _qpw;
	int _qbone;
	int _qvb;
	int _qgrv;
	int _qfarm;
	int _qgirl;
	int _qtrade;
	int _qdefiler;
	int _qnakrul;
	int _qjersy;
	int _qhf8;
} QuestTalkData;

extern TownerStruct towner[NUM_TOWNERS];

void InitTowners();
void FreeTownerGFX();
void ProcessTowners();
ItemStruct *PlrHasItem(int pnum, int item, int *i);
void TalkToTowner(int p, int t);

extern QuestTalkData Qtalklist[];

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __TOWNERS_H__ */
