/**
 * @file gendung.h
 *
 * Interface of general dungeon generation code.
 */
#pragma once

#include <cstdint>
#include <memory>

#include "engine.h"
#include "engine/cel_sprite.hpp"
#include "engine/point.hpp"
#include "scrollrt.h"
#include "utils/attributes.h"
#include "utils/enum_traits.h"
#include "utils/stdcompat/optional.hpp"

namespace devilution {

#define DMAXX 40
#define DMAXY 40

#define MAXDUNX (16 + DMAXX * 2 + 16)
#define MAXDUNY (16 + DMAXY * 2 + 16)

#define MAXTHEMES 50
#define MAXTILES 2048

enum _setlevels : int8_t {
	SL_NONE,
	SL_SKELKING,
	SL_BONECHAMB,
	SL_MAZE,
	SL_POISONWATER,
	SL_VILEBETRAYER,

	SL_LAST = SL_VILEBETRAYER,
};

enum dungeon_type : int8_t {
	DTYPE_TOWN,
	DTYPE_CATHEDRAL,
	DTYPE_CATACOMBS,
	DTYPE_CAVES,
	DTYPE_HELL,
	DTYPE_NEST,
	DTYPE_CRYPT,

	DTYPE_LAST = DTYPE_CRYPT,
	DTYPE_NONE = -1,
};

enum lvl_entry : uint8_t {
	ENTRY_MAIN,
	ENTRY_PREV,
	ENTRY_SETLVL,
	ENTRY_RTNLVL,
	ENTRY_LOAD,
	ENTRY_WARPLVL,
	ENTRY_TWARPDN,
	ENTRY_TWARPUP,
};

enum {
	// clang-format off
	DLRG_HDOOR     = 1 << 0,
	DLRG_VDOOR     = 1 << 1,
	DLRG_CHAMBER   = 1 << 6,
	DLRG_PROTECTED = 1 << 7,
	// clang-format on
};

enum class DungeonFlag : uint8_t {
	// clang-format off
	None        = 0, // Only used by lighting/automap
	Missile     = 1 << 0,
	Visible     = 1 << 1,
	DeadPlayer  = 1 << 2,
	Populated   = 1 << 3,
	// 1 << 4 and 1 << 5 were used as workarounds for a bug with horizontal movement (relative to the screen) for monsters and players respectively
	Lit         = 1 << 6,
	Explored    = 1 << 7,
	SavedFlags  = (Populated | Lit | Explored), // ~(Missile | Visible | DeadPlayer)
	LoadedFlags = (Missile | Visible | DeadPlayer | Populated | Lit | Explored)
	// clang-format on
};
use_enum_as_flags(DungeonFlag);

enum _difficulty : uint8_t {
	DIFF_NORMAL,
	DIFF_NIGHTMARE,
	DIFF_HELL,

	DIFF_LAST = DIFF_HELL,
};

struct ScrollStruct {
	/** @brief Tile offset of camera. */
	Point tile;
	/** @brief Pixel offset of camera. */
	Displacement offset;
	/** @brief Move direction of camera. */
	ScrollDirection _sdir;
};

struct THEME_LOC {
	int16_t x;
	int16_t y;
	int16_t ttval;
	int16_t width;
	int16_t height;
};

struct MegaTile {
	uint16_t micro1;
	uint16_t micro2;
	uint16_t micro3;
	uint16_t micro4;
};

struct MICROS {
	uint16_t mt[16];
};

struct ShadowStruct {
	uint8_t strig;
	uint8_t s1;
	uint8_t s2;
	uint8_t s3;
	uint8_t nv1;
	uint8_t nv2;
	uint8_t nv3;
};

/** Contains the tile IDs of the map. */
extern uint8_t dungeon[DMAXX][DMAXY];
/** Contains a backup of the tile IDs of the map. */
extern uint8_t pdungeon[DMAXX][DMAXY];
extern uint8_t dflags[DMAXX][DMAXY];
/** Specifies the active set level X-coordinate of the map. */
extern int setpc_x;
/** Specifies the active set level Y-coordinate of the map. */
extern int setpc_y;
/** Specifies the width of the active set level of the map. */
extern int setpc_w;
/** Specifies the height of the active set level of the map. */
extern int setpc_h;
/** Contains the contents of the single player quest DUN file. */
extern std::unique_ptr<uint16_t[]> pSetPiece;
/** Specifies whether a single player quest DUN has been loaded. */
extern bool setloadflag;
extern std::optional<CelSprite> pSpecialCels;
/** Specifies the tile definitions of the active dungeon type; (e.g. levels/l1data/l1.til). */
extern std::unique_ptr<MegaTile[]> pMegaTiles;
extern std::unique_ptr<uint16_t[]> pLevelPieces;
extern std::unique_ptr<byte[]> pDungeonCels;
/**
 * List of transparancy masks to use for dPieces
 */
extern std::array<uint8_t, MAXTILES + 1> block_lvid;
/**
 * List of light blocking dPieces
 */
extern std::array<bool, MAXTILES + 1> nBlockTable;
/**
 * List of path blocking dPieces
 */
extern DVL_API_FOR_TEST std::array<bool, MAXTILES + 1> nSolidTable;
/**
 * List of transparent dPieces
 */
extern std::array<bool, MAXTILES + 1> nTransTable;
/**
 * List of missile blocking dPieces
 */
extern std::array<bool, MAXTILES + 1> nMissileTable;
extern std::array<bool, MAXTILES + 1> nTrapTable;
/** Specifies the minimum X,Y-coordinates of the map. */
extern Point dminPosition;
/** Specifies the maximum X,Y-coordinates of the map. */
extern Point dmaxPosition;
/** Specifies the active dungeon type of the current game. */
extern DVL_API_FOR_TEST dungeon_type leveltype;
/** Specifies the active dungeon level of the current game. */
extern uint8_t currlevel;
extern bool setlevel;
/** Specifies the active quest level of the current game. */
extern _setlevels setlvlnum;
/** Specifies the player viewpoint X-coordinate of the map. */
extern dungeon_type setlvltype;
/** Specifies the player viewpoint X,Y-coordinates of the map. */
extern Point ViewPosition;
extern ScrollStruct ScrollInfo;
extern int MicroTileLen;
extern char TransVal;
/** Specifies the active transparency indices. */
extern bool TransList[256];
/** Contains the piece IDs of each tile on the map. */
extern DVL_API_FOR_TEST int dPiece[MAXDUNX][MAXDUNY];
/** Specifies the dungeon piece information for a given coordinate and block number. */
extern MICROS dpiece_defs_map_2[MAXDUNX][MAXDUNY];
/** Specifies the transparency at each coordinate of the map. */
extern int8_t dTransVal[MAXDUNX][MAXDUNY];
extern DVL_API_FOR_TEST char dLight[MAXDUNX][MAXDUNY];
extern char dPreLight[MAXDUNX][MAXDUNY];
/** Holds various information about dungeon tiles, @see DungeonFlag */
extern DungeonFlag dFlags[MAXDUNX][MAXDUNY];

/** Contains the player numbers (players array indices) of the map. */
extern int8_t dPlayer[MAXDUNX][MAXDUNY];
/**
 * Contains the NPC numbers of the map. The NPC number represents a
 * towner number (towners array index) in Tristram and a monster number
 * (monsters array index) in the dungeon.
 */
extern int16_t dMonster[MAXDUNX][MAXDUNY];
/**
 * Contains the dead numbers (deads array indices) and dead direction of
 * the map, encoded as specified by the pseudo-code below.
 * dDead[x][y] & 0x1F - index of dead
 * dDead[x][y] >> 0x5 - direction
 */
extern DVL_API_FOR_TEST int8_t dCorpse[MAXDUNX][MAXDUNY];
/** Contains the object numbers (objects array indices) of the map. */
extern DVL_API_FOR_TEST int8_t dObject[MAXDUNX][MAXDUNY];
/** Contains the item numbers (items array indices) of the map. */
extern int8_t dItem[MAXDUNX][MAXDUNY];
/**
 * Contains the arch frame numbers of the map from the special tileset
 * (e.g. "levels/l1data/l1s.cel"). Note, the special tileset of Tristram (i.e.
 * "levels/towndata/towns.cel") contains trees rather than arches.
 */
extern char dSpecial[MAXDUNX][MAXDUNY];
extern int themeCount;
extern THEME_LOC themeLoc[MAXTHEMES];

constexpr bool InDungeonBounds(Point position)
{
	return position.x >= 0 && position.x < MAXDUNX && position.y >= 0 && position.y < MAXDUNY;
}

/**
 * @brief Checks if a given tile contains at least one missile
 * @param position Coordinates of the dungeon tile to check
 * @return true if a missile exists at this position
 */
constexpr bool TileContainsMissile(Point position)
{
	return InDungeonBounds(position) && HasAnyOf(dFlags[position.x][position.y], DungeonFlag::Missile);
}

/**
 * @brief Checks if a given tile contains a player corpse
 * @param position Coordinates of the dungeon tile to check
 * @return true if a dead player exists at this position
 */
constexpr bool TileContainsDeadPlayer(Point position)
{
	return InDungeonBounds(position) && HasAnyOf(dFlags[position.x][position.y], DungeonFlag::DeadPlayer);
}

/**
 * @brief Check if a given tile contains a decorative object (or similar non-pathable set piece)
 *
 * This appears to include stairs so that monsters do not spawn or path onto them, but players can path to them to navigate between layers
 *
 * @param position Coordinates of the dungeon tile to check
 * @return true if a set piece was spawned at this position
 */
constexpr bool TileContainsSetPiece(Point position)
{
	return InDungeonBounds(position) && HasAnyOf(dFlags[position.x][position.y], DungeonFlag::Populated);
}

/**
 * @brief Checks if any player can currently see this tile
 *
 * Currently only used by monster AI routines so basic monsters out of sight can be ignored until they're likely to interact with the player
 *
 * @param position Coordinates of the dungeon tile to check
 * @return true if the tile is within at least one players vision
 */
constexpr bool IsTileVisible(Point position)
{
	return InDungeonBounds(position) && HasAnyOf(dFlags[position.x][position.y], DungeonFlag::Visible);
}

/**
 * @brief Checks if a light source is illuminating this tile
 * @param position Coordinates of the dungeon tile to check
 * @return true if the tile is within the radius of at least one light source
 */
constexpr bool IsTileLit(Point position)
{
	return InDungeonBounds(position) && HasAnyOf(dFlags[position.x][position.y], DungeonFlag::Lit);
}

void FillSolidBlockTbls();
void SetDungeonMicros();
void DRLG_InitTrans();
void DRLG_MRectTrans(int x1, int y1, int x2, int y2);
void DRLG_RectTrans(int x1, int y1, int x2, int y2);
void DRLG_CopyTrans(int sx, int sy, int dx, int dy);
void DRLG_ListTrans(int num, BYTE *List);
void DRLG_AreaTrans(int num, BYTE *List);
void DRLG_InitSetPC();
void DRLG_SetPC();
void Make_SetPC(int x, int y, int w, int h);
void DRLG_PlaceThemeRooms(int minSize, int maxSize, int floor, int freq, bool rndSize);
void DRLG_HoldThemeRooms();
void DRLG_LPass3(int lv);
void DRLG_Init_Globals();
bool SkipThemeRoom(int x, int y);
void InitLevels();
void FloodTransparencyValues(uint8_t floorID);

} // namespace devilution
