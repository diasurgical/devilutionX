/**
 * @file enums.h
 *
 * Various global enumerators.
 */

#include <stdint.h>

namespace devilution {

typedef enum dungeon_message {
	DMSG_CATHEDRAL = 1 << 0,
	DMSG_CATACOMBS = 1 << 1,
	DMSG_CAVES     = 1 << 2,
	DMSG_HELL      = 1 << 3,
	DMSG_DIABLO    = 1 << 4,
} dungeon_message;

typedef enum event_type {
	EVENT_TYPE_PLAYER_CREATE_GAME = 1,
	EVENT_TYPE_2                  = 2,
	EVENT_TYPE_PLAYER_LEAVE_GAME  = 3,
	EVENT_TYPE_PLAYER_MESSAGE     = 4,
	EVENT_TYPE_5                  = 5,
	EVENT_TYPE_6                  = 6,
	EVENT_TYPE_7                  = 7,
	EVENT_TYPE_8                  = 8,
	EVENT_TYPE_9                  = 9,
	EVENT_TYPE_10                 = 10,
	EVENT_TYPE_11                 = 11,
	EVENT_TYPE_12                 = 12,
	EVENT_TYPE_13                 = 13,
	EVENT_TYPE_14                 = 14,
	EVENT_TYPE_15                 = 15,
} event_type;

typedef enum interface_mode {
	WM_DIABNEXTLVL  = 0x402, // WM_USER+2
	WM_DIABPREVLVL  = 0x403,
	WM_DIABRTNLVL   = 0x404,
	WM_DIABSETLVL   = 0x405,
	WM_DIABWARPLVL  = 0x406,
	WM_DIABTOWNWARP = 0x407,
	WM_DIABTWARPUP  = 0x408,
	WM_DIABRETOWN   = 0x409,
	WM_DIABNEWGAME  = 0x40A,
	WM_DIABLOADGAME = 0x40B
	// WM_LEIGHSKIP = 0x40C, // psx only
	// WM_DIAVNEWLVL = 0x40D, // psx only
} interface_mode;

typedef enum game_info {
	GAMEINFO_NAME         = 1,
	GAMEINFO_PASSWORD     = 2,
	GAMEINFO_STATS        = 3,
	GAMEINFO_MODEFLAG     = 4,
	GAMEINFO_GAMETEMPLATE = 5,
	GAMEINFO_PLAYERS      = 6,
} game_info;

typedef enum _setlevels {
	//SL_BUTCHCHAMB = 0x0,
	SL_SKELKING     = 0x1,
	SL_BONECHAMB    = 0x2,
	SL_MAZE         = 0x3,
	SL_POISONWATER  = 0x4,
	SL_VILEBETRAYER = 0x5,
} _setlevels;

typedef enum inv_item {
	INVITEM_HEAD       = 0,
	INVITEM_RING_LEFT  = 1,
	INVITEM_RING_RIGHT = 2,
	INVITEM_AMULET     = 3,
	INVITEM_HAND_LEFT  = 4,
	INVITEM_HAND_RIGHT = 5,
	INVITEM_CHEST      = 6,
	INVITEM_INV_FIRST  = 7,
	INVITEM_INV_LAST   = 46,
	INVITEM_BELT_FIRST = 47,
	INVITEM_BELT_LAST  = 54,
	NUM_INVELEM
} inv_item;

// identifiers for each of the inventory squares
// see https://github.com/sanctuary/graphics/blob/master/inventory.png
typedef enum inv_xy_slot {
	SLOTXY_HEAD_FIRST       = 0,
	SLOTXY_HEAD_LAST        = 3,
	SLOTXY_RING_LEFT        = 4,
	SLOTXY_RING_RIGHT       = 5,
	SLOTXY_AMULET           = 6,
	SLOTXY_HAND_LEFT_FIRST  = 7,
	SLOTXY_HAND_LEFT_LAST   = 12,
	SLOTXY_HAND_RIGHT_FIRST = 13,
	SLOTXY_HAND_RIGHT_LAST  = 18,
	SLOTXY_CHEST_FIRST      = 19,
	SLOTXY_CHEST_LAST       = 24,

	// regular inventory
	SLOTXY_INV_FIRST = 25,
	SLOTXY_INV_LAST  = 64,

	// belt items
	SLOTXY_BELT_FIRST = 65,
	SLOTXY_BELT_LAST  = 72,
	NUM_XY_SLOTS      = 73
} inv_xy_slot;

typedef enum player_graphic {
	PFILE_STAND     = 1 << 0,
	PFILE_WALK      = 1 << 1,
	PFILE_ATTACK    = 1 << 2,
	PFILE_HIT       = 1 << 3,
	PFILE_LIGHTNING = 1 << 4,
	PFILE_FIRE      = 1 << 5,
	PFILE_MAGIC     = 1 << 6,
	PFILE_DEATH     = 1 << 7,
	PFILE_BLOCK     = 1 << 8,
	// everything except PFILE_DEATH
	// 0b1_0111_1111
	PFILE_NONDEATH = 0x17F
} player_graphic;

} // namespace devilution
