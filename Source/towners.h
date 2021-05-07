/**
 * @file towners.h
 *
 * Interface of functionality for loading and spawning towners.
 */
#pragma once

#include <cstdint>
#include <memory>
#include "utils/stdcompat/string_view.hpp"

#include "items.h"
#include "player.h"
#include "quests.h"

namespace devilution {

#define NUM_TOWNERS 16

enum _talker_id : uint8_t {
	TOWN_SMITH,
	TOWN_HEALER,
	TOWN_DEADGUY,
	TOWN_TAVERN,
	TOWN_STORY,
	TOWN_DRUNK,
	TOWN_WITCH,
	TOWN_BMAID,
	TOWN_PEGBOY,
	TOWN_COW,
	TOWN_FARMER,
	TOWN_GIRL,
	TOWN_COWFARM,
	NUM_TOWNER_TYPES,
};

struct TNQ {
	uint8_t _qsttype;
	uint8_t _qstmsg;
	bool _qstmsgact;
};

struct TownerStruct {
	byte *_tNAnim[8];
	std::unique_ptr<byte[]> _tNData;
	byte *_tAnimData;
	int16_t _tSeed;
	/** Tile position of NPC */
	Point position;
	int16_t _tAnimWidth;
	int16_t _tAnimDelay; // Tick length of each frame in the current animation
	int16_t _tAnimCnt;   // Increases by one each game tick, counting how close we are to _pAnimDelay
	uint8_t _tAnimLen;   // Number of frames in current animation
	uint8_t _tAnimFrame; // Current frame of animation.
	uint8_t _tAnimFrameCnt;
	uint8_t _tNFrames;
	string_view _tName;
	TNQ qsts[MAXQUESTS];
	bool _tSelFlag;
	bool _tMsgSaid;
	int8_t _tAnimOrder;
	PlayerStruct *_tTalkingToPlayer;
	void (*talk)(PlayerStruct &player, TownerStruct &barOwner);
	bool _tbtcnt;
	_talker_id _ttype;
};

extern TownerStruct towners[NUM_TOWNERS];

void InitTowners();
void FreeTownerGFX();
void ProcessTowners();
void TalkToTowner(PlayerStruct &player, int t);

extern _speech_id Qtalklist[NUM_TOWNER_TYPES][MAXQUESTS];

} // namespace devilution
