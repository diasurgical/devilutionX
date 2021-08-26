/**
 * @file objects.h
 *
 * Interface of object functionality, interaction, spawning, loading, etc.
 */
#pragma once

#include <cstdint>

#include "engine/point.hpp"
#include "engine/rectangle.hpp"
#include "itemdat.h"
#include "monster.h"
#include "objdat.h"
#include "textdat.h"

namespace devilution {

#define MAXOBJECTS 127

struct ObjectStruct {
	_object_id _otype;
	Point position;
	bool _oLight;
	uint32_t _oAnimFlag;
	byte *_oAnimData;
	int _oAnimDelay;      // Tick length of each frame in the current animation
	int _oAnimCnt;        // Increases by one each game tick, counting how close we are to _pAnimDelay
	uint32_t _oAnimLen;   // Number of frames in current animation
	uint32_t _oAnimFrame; // Current frame of animation.
	int _oAnimWidth;
	bool _oDelFlag;
	int8_t _oBreak;
	bool _oSolidFlag;
	bool _oMissFlag;
	uint8_t _oSelFlag;
	bool _oPreFlag;
	bool _oTrapFlag;
	bool _oDoorFlag;
	int _olid;
	/**
	 * Saves the absolute value of the engine state (typically from a call to AdvanceRndSeed()) to later use when spawning items from a container object
	 * This is an unsigned value to avoid implementation defined behaviour when reading from this variable.
	 */
	uint32_t _oRndSeed;
	int _oVar1;
	int _oVar2;
	int _oVar3;
	int _oVar4;
	int _oVar5;
	uint32_t _oVar6;
	/**
	 * @brief ID of a quest message to play when this object is activated.
	 *
	 * Used by spell book objects which trigger quest progress for Halls of the Blind, Valor, or Warlord of Blood
	 */
	_speech_id bookMessage;
	int _oVar8;

	/**
	 * @brief Marks the map region to be refreshed when the player interacts with the object.
	 *
	 * Some objects will cause a map region to change when a player interacts with them (e.g. Skeleton King
	 * antechamber levers). The coordinates used for this region are based on a 40*40 grid overlaying the central
	 * 80*80 region of the dungeon.
	 *
	 * @param topLeftPosition corner of the map region closest to the origin.
	 * @param bottomRightPosition corner of the map region furthest from the origin.
	 */
	constexpr void SetMapRange(Point topLeftPosition, Point bottomRightPosition)
	{
		_oVar1 = topLeftPosition.x;
		_oVar2 = topLeftPosition.y;
		_oVar3 = bottomRightPosition.x;
		_oVar4 = bottomRightPosition.y;
	}

	/**
	 * @brief Convenience function for SetMapRange(Point, Point).
	 * @param mapRange A rectangle defining the top left corner and size of the affected region.
	 */
	constexpr void SetMapRange(Rectangle mapRange)
	{
		SetMapRange(mapRange.position, mapRange.position + Displacement { mapRange.size });
	}

	/**
	 * @brief Sets up a generic quest book which will trigger a change in the map when activated.
	 *
	 * Books of this type use a generic message (see OperateSChambBook()) compared to the more specific quest books
	 * initialized by IntializeQuestBook().
	 *
	 * @param mapRange The region to be updated when this object is activated.
	*/
	constexpr void InitializeBook(Rectangle mapRange)
	{
		SetMapRange(mapRange);
		_oVar6 = _oAnimFrame + 1; // Save the frame number for the open book frame
	}

	/**
	 * @brief Initializes this object as a quest book which will cause further changes and play a message when activated.
	 * @param mapRange The region to be updated when this object is activated.
	 * @param leverID An ID (distinct from the object index) to identify the new objects spawned after updating the map.
	 * @param message The quest text to play when this object is activated.
	 */
	constexpr void InitializeQuestBook(Rectangle mapRange, int leverID, _speech_id message)
	{
		InitializeBook(mapRange);
		_oVar8 = leverID;
		bookMessage = message;
	}

	/**
	 * @brief Marks an object which was spawned from a sublevel in response to a lever activation.
	 * @param mapRange The region which was updated to spawn this object.
	 * @param leverID The id (*not* an object ID/index) of the lever responsible for the map change.
	 */
	constexpr void InitializeLoadedObject(Rectangle mapRange, int leverID)
	{
		SetMapRange(mapRange);
		_oVar8 = leverID;
	}

	/**
	 * @brief Check if this object is a door
	 * @return True if the object is one of the door types (see _object_id)
	 */
	bool IsDoor() const
	{
		return IsAnyOf(_otype, _object_id::OBJ_L1LDOOR, _object_id::OBJ_L1RDOOR, _object_id::OBJ_L2LDOOR, _object_id::OBJ_L2RDOOR, _object_id::OBJ_L3LDOOR, _object_id::OBJ_L3RDOOR);
	}
};

extern ObjectStruct Objects[MAXOBJECTS];
extern int AvailableObjects[MAXOBJECTS];
extern int ActiveObjects[MAXOBJECTS];
extern int ActiveObjectCount;
extern bool ApplyObjectLighting;
extern bool LoadingMapObjects;

void InitObjectGFX();
void FreeObjectGFX();
void AddL1Objs(int x1, int y1, int x2, int y2);
void AddL2Objs(int x1, int y1, int x2, int y2);
void InitObjects();
void SetMapObjects(const uint16_t *dunData, int startx, int starty);
/**
 * @brief Spawns an object of the given type at the map coordinates provided
 * @param objType Type specifier
 * @param objPos tile coordinates
 */
void AddObject(_object_id objType, Point objPos);
void Obj_Trap(int i);
void ProcessObjects();
void RedoPlayerVision();
void MonstCheckDoors(MonsterStruct &monster);
void ObjChangeMap(int x1, int y1, int x2, int y2);
void ObjChangeMapResync(int x1, int y1, int x2, int y2);
void TryDisarm(int pnum, int i);
int ItemMiscIdIdx(item_misc_id imiscid);
void OperateObject(int pnum, int i, bool TeleFlag);
void SyncOpObject(int pnum, int cmd, int i);
void BreakObject(int pnum, int oi);
void SyncBreakObj(int pnum, int oi);
void SyncObjectAnim(ObjectStruct &object);
void GetObjectStr(int i);
void OperateNakrulLever();
void SyncNakrulRoom();
void AddNakrulLeaver();
/**
 * @brief Checks whether the player is activating Na-Krul's spell tomes in the correct order
 *
 * Used as part of the final Diablo: Hellfire quest (from the hints provided to the player in the
 * reconstructed note). This function both updates the state of the variable that tracks progress
 * and also determines whether the spawn conditions are met (i.e. all tomes have been triggered
 * in the correct order).
 *
 * @param s the id of the spell tome
 * @return true if the player has activated all three tomes in the correct order, false otherwise
 */
bool OperateNakrulBook(int s);
bool objectIsDisabled(int i);

} // namespace devilution
