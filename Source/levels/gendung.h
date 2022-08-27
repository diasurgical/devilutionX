/**
 * @file gendung.h
 *
 * Interface of general dungeon generation code.
 */
#pragma once

#include <cstdint>
#include <memory>

#include "engine.h"
#include "engine/clx_sprite.hpp"
#include "engine/point.hpp"
#include "engine/rectangle.hpp"
#include "engine/render/scrollrt.h"
#include "utils/attributes.h"
#include "utils/bitset2d.hpp"
#include "utils/enum_traits.h"
#include "utils/stdcompat/optional.hpp"

namespace devilution {

#define DMAXX 40
#define DMAXY 40

#define MAXDUNX (16 + DMAXX * 2 + 16)
#define MAXDUNY (16 + DMAXY * 2 + 16)

#define MAXTHEMES 50
#define MAXTILES 1379

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

enum class TileProperties : uint8_t {
	// clang-format off
	None             = 0,
	Solid            = 1 << 0,
	BlockLight       = 1 << 1,
	BlockMissile     = 1 << 2,
	Transparent      = 1 << 3,
	TransparentLeft  = 1 << 4,
	TransparentRight = 1 << 5,
	Trap             = 1 << 7,
	// clang-format on
};
use_enum_as_flags(TileProperties);

enum _difficulty : uint8_t {
	DIFF_NORMAL,
	DIFF_NIGHTMARE,
	DIFF_HELL,

	DIFF_LAST = DIFF_HELL,
};

struct THEME_LOC {
	Rectangle room;
	int16_t ttval;
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

/** Reprecents what tiles are being utilized in the generated map. */
extern Bitset2d<DMAXX, DMAXY> DungeonMask;
/** Contains the tile IDs of the map. */
extern DVL_API_FOR_TEST uint8_t dungeon[DMAXX][DMAXY];
/** Contains a backup of the tile IDs of the map. */
extern uint8_t pdungeon[DMAXX][DMAXY];
/** Tile that may not be overwritten by the level generator */
extern Bitset2d<DMAXX, DMAXY> Protected;
extern Rectangle SetPieceRoom;
/** Specifies the active set quest piece in coordinate. */
extern Rectangle SetPiece;
/** Contains the contents of the single player quest DUN file. */
extern std::unique_ptr<uint16_t[]> pSetPiece;
extern OptionalOwnedClxSpriteList pSpecialCels;
/** Specifies the tile definitions of the active dungeon type; (e.g. levels/l1data/l1.til). */
extern DVL_API_FOR_TEST std::unique_ptr<MegaTile[]> pMegaTiles;
extern std::unique_ptr<byte[]> pDungeonCels;
/**
 * List tile properties
 */
extern DVL_API_FOR_TEST std::array<TileProperties, MAXTILES> SOLData;
/** Specifies the minimum X,Y-coordinates of the map. */
extern Point dminPosition;
/** Specifies the maximum X,Y-coordinates of the map. */
extern Point dmaxPosition;
/** Specifies the active dungeon type of the current game. */
extern DVL_API_FOR_TEST dungeon_type leveltype;
/** Specifies the active dungeon level of the current game. */
extern DVL_API_FOR_TEST uint8_t currlevel;
extern bool setlevel;
/** Specifies the active quest level of the current game. */
extern _setlevels setlvlnum;
/** Specifies the player viewpoint X-coordinate of the map. */
extern dungeon_type setlvltype;
/** Specifies the player viewpoint X,Y-coordinates of the map. */
extern DVL_API_FOR_TEST Point ViewPosition;
extern int MicroTileLen;
extern char TransVal;
/** Specifies the active transparency indices. */
extern bool TransList[256];
/** Contains the piece IDs of each tile on the map. */
extern DVL_API_FOR_TEST uint16_t dPiece[MAXDUNX][MAXDUNY];
/** Map of micros that comprises a full tile for any given dungeon piece. */
extern MICROS DPieceMicros[MAXTILES];
/** Specifies the transparency at each coordinate of the map. */
extern DVL_API_FOR_TEST int8_t dTransVal[MAXDUNX][MAXDUNY];
extern char dLight[MAXDUNX][MAXDUNY];
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
/**
 * Contains the arch frame numbers of the map from the special tileset
 * (e.g. "levels/l1data/l1s.cel"). Note, the special tileset of Tristram (i.e.
 * "levels/towndata/towns.cel") contains trees rather than arches.
 */
extern char dSpecial[MAXDUNX][MAXDUNY];
extern int themeCount;
extern THEME_LOC themeLoc[MAXTHEMES];

#ifdef BUILD_TESTING
std::optional<Size> GetSizeForThemeRoom();
#endif

dungeon_type GetLevelType(int level);
void CreateDungeon(uint32_t rseed, lvl_entry entry);

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

struct Miniset {
	Size size;
	/* these are indexed as [y][x] */
	uint8_t search[6][6];
	uint8_t replace[6][6];

	/**
	 * @param position Coordinates of the dungeon tile to check
	 * @param respectProtected Match bug from Crypt levels if false
	 */
	bool matches(Point position, bool respectProtected = true) const
	{
		for (int yy = 0; yy < size.height; yy++) {
			for (int xx = 0; xx < size.width; xx++) {
				if (search[yy][xx] != 0 && dungeon[xx + position.x][yy + position.y] != search[yy][xx])
					return false;
				if (respectProtected && Protected.test(xx + position.x, yy + position.y))
					return false;
			}
		}
		return true;
	}

	void place(Point position, bool protect = false) const
	{
		for (int y = 0; y < size.height; y++) {
			for (int x = 0; x < size.width; x++) {
				if (replace[y][x] == 0)
					continue;
				dungeon[x + position.x][y + position.y] = replace[y][x];
				if (protect)
					Protected.set(x + position.x, y + position.y);
			}
		}
	}
};

bool TileHasAny(int tileId, TileProperties property);
void LoadLevelSOLData();
void SetDungeonMicros();
void DRLG_InitTrans();
void DRLG_MRectTrans(Point origin, Point extent);
void DRLG_MRectTrans(Rectangle area);
void DRLG_RectTrans(Rectangle area);
void DRLG_CopyTrans(int sx, int sy, int dx, int dy);
void LoadTransparency(const uint16_t *dunData);
void LoadDungeonBase(const char *path, Point spawn, int floorId, int dirtId);
void Make_SetPC(Rectangle area);
/**
 * @param miniset The miniset to place
 * @param tries Tiles to try, 1600 will scan the full map
 * @param drlg1Quirk Match buggy behaviour of Diablo's Cathedral
 */
std::optional<Point> PlaceMiniSet(const Miniset &miniset, int tries = 199, bool drlg1Quirk = false);
void PlaceDunTiles(const uint16_t *dunData, Point position, int floorId = 0);
void DRLG_PlaceThemeRooms(int minSize, int maxSize, int floor, int freq, bool rndSize);
void DRLG_HoldThemeRooms();
void SetSetPieceRoom(Point position, int floorId);
void FreeQuestSetPieces();
void DRLG_LPass3(int lv);

/**
 * @brief Checks if a theme room is located near the target point
 * @param position Target location in dungeon coordinates
 * @return True if a theme room is near (within 2 tiles of) this point, false if it is free.
 */
bool IsNearThemeRoom(Point position);
void InitLevels();
void FloodTransparencyValues(uint8_t floorID);

} // namespace devilution
