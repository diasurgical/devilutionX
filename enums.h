/**
 * @file enums.h
 *
 * Various global enumerators.
 */

#include <stdint.h>

namespace devilution {

typedef enum monster_goal {
	MGOAL_NORMAL    = 1,
	MGOAL_RETREAT   = 2,
	MGOAL_HEALING   = 3,
	MGOAL_MOVE      = 4,
	MGOAL_ATTACK2   = 5,
	MGOAL_INQUIRING = 6,
	MGOAL_TALKING   = 7,
} monster_goal;

typedef enum dflag {
	BFLAG_MISSILE     = 0x01,
	BFLAG_VISIBLE     = 0x02,
	BFLAG_DEAD_PLAYER = 0x04,
	BFLAG_POPULATED   = 0x08,
	BFLAG_MONSTLR     = 0x10,
	BFLAG_PLAYERLR    = 0x20,
	BFLAG_LIT         = 0x40,
	BFLAG_EXPLORED    = 0x80,
} dflag;

typedef enum placeflag {
	PLACE_SCATTER = 1,
	PLACE_SPECIAL = 2,
	PLACE_UNIQUE  = 4,
} placeflag;

typedef enum mienemy_type {
	TARGET_MONSTERS = 0,
	TARGET_PLAYERS  = 1,
	TARGET_BOTH     = 2,
} mienemy_type;

typedef enum dungeon_message {
	DMSG_CATHEDRAL = 1 << 0,
	DMSG_CATACOMBS = 1 << 1,
	DMSG_CAVES     = 1 << 2,
	DMSG_HELL      = 1 << 3,
	DMSG_DIABLO    = 1 << 4,
} dungeon_message;

typedef enum theme_id {
	THEME_BARREL            = 0x0,
	THEME_SHRINE            = 0x1,
	THEME_MONSTPIT          = 0x2,
	THEME_SKELROOM          = 0x3,
	THEME_TREASURE          = 0x4,
	THEME_LIBRARY           = 0x5,
	THEME_TORTURE           = 0x6,
	THEME_BLOODFOUNTAIN     = 0x7,
	THEME_DECAPITATED       = 0x8,
	THEME_PURIFYINGFOUNTAIN = 0x9,
	THEME_ARMORSTAND        = 0xA,
	THEME_GOATSHRINE        = 0xB,
	THEME_CAULDRON          = 0xC,
	THEME_MURKYFOUNTAIN     = 0xD,
	THEME_TEARFOUNTAIN      = 0xE,
	THEME_BRNCROSS          = 0xF,
	THEME_WEAPONRACK        = 0x10,
	THEME_NONE              = -1,
} theme_id;

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

typedef enum _difficulty {
	DIFF_NORMAL      = 0x0,
	DIFF_NIGHTMARE   = 0x1,
	DIFF_HELL        = 0x2,
	NUM_DIFFICULTIES = 0x3,
} _difficulty;

typedef enum MON_ANIM {
	MA_STAND   = 0,
	MA_WALK    = 1,
	MA_ATTACK  = 2,
	MA_GOTHIT  = 3,
	MA_DEATH   = 4,
	MA_SPECIAL = 5,
} MON_ANIM;

typedef enum spell_type {
	RSPLTYPE_SKILL   = 0x0,
	RSPLTYPE_SPELL   = 0x1,
	RSPLTYPE_SCROLL  = 0x2,
	RSPLTYPE_CHARGES = 0x3,
	RSPLTYPE_INVALID = 0x4,
} spell_type;

typedef enum cursor_id {
	CURSOR_NONE        = 0x0,
	CURSOR_HAND        = 0x1,
	CURSOR_IDENTIFY    = 0x2,
	CURSOR_REPAIR      = 0x3,
	CURSOR_RECHARGE    = 0x4,
	CURSOR_DISARM      = 0x5,
	CURSOR_OIL         = 0x6,
	CURSOR_TELEKINESIS = 0x7,
	CURSOR_RESURRECT   = 0x8,
	CURSOR_TELEPORT    = 0x9,
	CURSOR_HEALOTHER   = 0xA,
	CURSOR_HOURGLASS   = 0xB,
	CURSOR_FIRSTITEM   = 0xC,
} cursor_id;

typedef enum direction {
	DIR_S    = 0x0,
	DIR_SW   = 0x1,
	DIR_W    = 0x2,
	DIR_NW   = 0x3,
	DIR_N    = 0x4,
	DIR_NE   = 0x5,
	DIR_E    = 0x6,
	DIR_SE   = 0x7,
	DIR_OMNI = 0x8,
} direction;

typedef enum _scroll_direction {
	SDIR_NONE = 0x0,
	SDIR_N    = 0x1,
	SDIR_NE   = 0x2,
	SDIR_E    = 0x3,
	SDIR_SE   = 0x4,
	SDIR_S    = 0x5,
	SDIR_SW   = 0x6,
	SDIR_W    = 0x7,
	SDIR_NW   = 0x8,
} _scroll_direction;

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

typedef enum lvl_entry {
	ENTRY_MAIN    = 0,
	ENTRY_PREV    = 1,
	ENTRY_SETLVL  = 2,
	ENTRY_RTNLVL  = 3,
	ENTRY_LOAD    = 4,
	ENTRY_WARPLVL = 5,
	ENTRY_TWARPDN = 6,
	ENTRY_TWARPUP = 7,
} lvl_entry;

typedef enum game_info {
	GAMEINFO_NAME         = 1,
	GAMEINFO_PASSWORD     = 2,
	GAMEINFO_STATS        = 3,
	GAMEINFO_MODEFLAG     = 4,
	GAMEINFO_GAMETEMPLATE = 5,
	GAMEINFO_PLAYERS      = 6,
} game_info;

typedef enum _music_id {
	TMUSIC_TOWN,
	TMUSIC_L1,
	TMUSIC_L2,
	TMUSIC_L3,
	TMUSIC_L4,
	TMUSIC_L5,
	TMUSIC_L6,
	TMUSIC_INTRO,
	NUM_MUSIC,
} _music_id;

typedef enum _mainmenu_selections {
	MAINMENU_SINGLE_PLAYER = 1,
	MAINMENU_MULTIPLAYER,
	MAINMENU_REPLAY_INTRO,
	MAINMENU_SHOW_SUPPORT,
	MAINMENU_SHOW_CREDITS,
	MAINMENU_EXIT_DIABLO,
	MAINMENU_ATTRACT_MODE,
} _mainmenu_selections;

typedef enum _selhero_selections {
	SELHERO_NEW_DUNGEON,
	SELHERO_CONTINUE,
	SELHERO_CONNECT,
	SELHERO_PREVIOUS,
} _selhero_selections;

typedef enum panel_button_id {
	PANBTN_CHARINFO  = 0,
	PANBTN_QLOG      = 1,
	PANBTN_AUTOMAP   = 2,
	PANBTN_MAINMENU  = 3,
	PANBTN_INVENTORY = 4,
	PANBTN_SPELLBOOK = 5,
	PANBTN_SENDMSG   = 6,
	PANBTN_FRIENDLY  = 7,
} panel_button_id;

typedef enum attribute_id {
	ATTRIB_STR,
	ATTRIB_MAG,
	ATTRIB_DEX,
	ATTRIB_VIT,
} attribute_id;

typedef enum _item_indexes {
	IDI_GOLD,
	IDI_WARRIOR,
	IDI_WARRSHLD,
	IDI_WARRCLUB,
	IDI_ROGUE,
	IDI_SORCERER,
	IDI_CLEAVER,
	IDI_FIRSTQUEST = IDI_CLEAVER,
	IDI_SKCROWN,
	IDI_INFRARING,
	IDI_ROCK,
	IDI_OPTAMULET,
	IDI_TRING,
	IDI_BANNER,
	IDI_HARCREST,
	IDI_STEELVEIL,
	IDI_GLDNELIX,
	IDI_ANVIL,
	IDI_MUSHROOM,
	IDI_BRAIN,
	IDI_FUNGALTM,
	IDI_SPECELIX,
	IDI_BLDSTONE,
	IDI_MAPOFDOOM,
	IDI_LASTQUEST = IDI_MAPOFDOOM,
	IDI_EAR,
	IDI_HEAL,
	IDI_MANA,
	IDI_IDENTIFY,
	IDI_PORTAL,
	IDI_ARMOFVAL,
	IDI_FULLHEAL,
	IDI_FULLMANA,
	IDI_GRISWOLD,
	IDI_LGTFORGE,
	IDI_LAZSTAFF,
	IDI_RESURRECT,
	IDI_OIL,
	IDI_SHORTSTAFF,
	IDI_BARDSWORD,
	IDI_BARDDAGGER,
	IDI_RUNEBOMB,
	IDI_THEODORE,
	IDI_AURIC,
	IDI_NOTE1,
	IDI_NOTE2,
	IDI_NOTE3,
	IDI_FULLNOTE,
	IDI_BROWNSUIT,
	IDI_GREYSUIT,
} _item_indexes;

typedef enum _setlevels {
	//SL_BUTCHCHAMB = 0x0,
	SL_SKELKING     = 0x1,
	SL_BONECHAMB    = 0x2,
	SL_MAZE         = 0x3,
	SL_POISONWATER  = 0x4,
	SL_VILEBETRAYER = 0x5,
} _setlevels;

typedef enum quest_id {
	Q_ROCK     = 0x00,
	Q_MUSHROOM = 0x01,
	Q_GARBUD   = 0x02,
	Q_ZHAR     = 0x03,
	Q_VEIL     = 0x04,
	Q_DIABLO   = 0x05,
	Q_BUTCHER  = 0x06,
	Q_LTBANNER = 0x07,
	Q_BLIND    = 0x08,
	Q_BLOOD    = 0x09,
	Q_ANVIL    = 0x0A,
	Q_WARLORD  = 0x0B,
	Q_SKELKING = 0x0C,
	Q_PWATER   = 0x0D,
	Q_SCHAMB   = 0x0E,
	Q_BETRAYER = 0x0F,
	Q_GRAVE    = 0x10,
	Q_FARMER   = 0x11,
	Q_GIRL     = 0x12,
	Q_TRADER   = 0x13,
	Q_DEFILER  = 0x14,
	Q_NAKRUL   = 0x15,
	Q_CORNSTN  = 0x16,
	Q_JERSEY   = 0x17,
	Q_INVALID  = -1,
} quest_id;

typedef enum quest_state {
	QUEST_NOTAVAIL = 0, // quest did not spawn this game
	QUEST_INIT     = 1, // quest has spawned, waiting to trigger
	QUEST_ACTIVE   = 2, // quest is currently in progress
	QUEST_DONE     = 3  // quest log closed and finished
} quest_state;

typedef enum quest_gametype {
	QUEST_SINGLE = 0,
	QUEST_ANY    = 1,
	QUEST_MULTI  = 2,
} quest_gametype;

typedef enum quest_mush_state {
	QS_INIT         = 0,
	QS_TOMESPAWNED  = 1,
	QS_TOMEGIVEN    = 2,
	QS_MUSHSPAWNED  = 3,
	QS_MUSHPICKED   = 4,
	QS_MUSHGIVEN    = 5,
	QS_BRAINSPAWNED = 6,
	QS_BRAINGIVEN   = 7,
} quest_mush_state;

typedef enum _unique_items {
	UITEM_CLEAVER      = 0x0,
	UITEM_SKCROWN      = 0x1,
	UITEM_INFRARING    = 0x2,
	UITEM_OPTAMULET    = 0x3,
	UITEM_TRING        = 0x4,
	UITEM_HARCREST     = 0x5,
	UITEM_STEELVEIL    = 0x6,
	UITEM_ARMOFVAL     = 0x7,
	UITEM_GRISWOLD     = 0x8,
	UITEM_BOVINE       = 0x9,
	UITEM_RIFTBOW      = 0xA,
	UITEM_NEEDLER      = 0xB,
	UITEM_CELESTBOW    = 0xC,
	UITEM_DEADLYHUNT   = 0xD,
	UITEM_BOWOFDEAD    = 0xE,
	UITEM_BLKOAKBOW    = 0xF,
	UITEM_FLAMEDART    = 0x10,
	UITEM_FLESHSTING   = 0x11,
	UITEM_WINDFORCE    = 0x12,
	UITEM_EAGLEHORN    = 0x13,
	UITEM_GONNAGALDIRK = 0x14,
	UITEM_DEFENDER     = 0x15,
	UITEM_GRYPHONCLAW  = 0x16,
	UITEM_BLACKRAZOR   = 0x17,
	UITEM_GIBBOUSMOON  = 0x18,
	UITEM_ICESHANK     = 0x19,
	UITEM_EXECUTIONER  = 0x1A,
	UITEM_BONESAW      = 0x1B,
	UITEM_SHADHAWK     = 0x1C,
	UITEM_WIZSPIKE     = 0x1D,
	UITEM_LGTSABRE     = 0x1E,
	UITEM_FALCONTALON  = 0x1F,
	UITEM_INFERNO      = 0x20,
	UITEM_DOOMBRINGER  = 0x21,
	UITEM_GRIZZLY      = 0x22,
	UITEM_GRANDFATHER  = 0x23,
	UITEM_MANGLER      = 0x24,
	UITEM_SHARPBEAK    = 0x25,
	UITEM_BLOODLSLAYER = 0x26,
	UITEM_CELESTAXE    = 0x27,
	UITEM_WICKEDAXE    = 0x28,
	UITEM_STONECLEAV   = 0x29,
	UITEM_AGUHATCHET   = 0x2A,
	UITEM_HELLSLAYER   = 0x2B,
	UITEM_MESSERREAVER = 0x2C,
	UITEM_CRACKRUST    = 0x2D,
	UITEM_JHOLMHAMM    = 0x2E,
	UITEM_CIVERBS      = 0x2F,
	UITEM_CELESTSTAR   = 0x30,
	UITEM_BARANSTAR    = 0x31,
	UITEM_GNARLROOT    = 0x32,
	UITEM_CRANBASH     = 0x33,
	UITEM_SCHAEFHAMM   = 0x34,
	UITEM_DREAMFLANGE  = 0x35,
	UITEM_STAFFOFSHAD  = 0x36,
	UITEM_IMMOLATOR    = 0x37,
	UITEM_STORMSPIRE   = 0x38,
	UITEM_GLEAMSONG    = 0x39,
	UITEM_THUNDERCALL  = 0x3A,
	UITEM_PROTECTOR    = 0x3B,
	UITEM_NAJPUZZLE    = 0x3C,
	UITEM_MINDCRY      = 0x3D,
	UITEM_RODOFONAN    = 0x3E,
	UITEM_SPIRITSHELM  = 0x3F,
	UITEM_THINKINGCAP  = 0x40,
	UITEM_OVERLORDHELM = 0x41,
	UITEM_FOOLSCREST   = 0x42,
	UITEM_GOTTERDAM    = 0x43,
	UITEM_ROYCIRCLET   = 0x44,
	UITEM_TORNFLESH    = 0x45,
	UITEM_GLADBANE     = 0x46,
	UITEM_RAINCLOAK    = 0x47,
	UITEM_LEATHAUT     = 0x48,
	UITEM_WISDWRAP     = 0x49,
	UITEM_SPARKMAIL    = 0x4A,
	UITEM_SCAVCARAP    = 0x4B,
	UITEM_NIGHTSCAPE   = 0x4C,
	UITEM_NAJPLATE     = 0x4D,
	UITEM_DEMONSPIKE   = 0x4E,
	UITEM_DEFLECTOR    = 0x4F,
	UITEM_SKULLSHLD    = 0x50,
	UITEM_DRAGONBRCH   = 0x51,
	UITEM_BLKOAKSHLD   = 0x52,
	UITEM_HOLYDEF      = 0x53,
	UITEM_STORMSHLD    = 0x54,
	UITEM_BRAMBLE      = 0x55,
	UITEM_REGHA        = 0x56,
	UITEM_BLEEDER      = 0x57,
	UITEM_CONSTRICT    = 0x58,
	UITEM_ENGAGE       = 0x59,
	UITEM_INVALID      = 0x5A,
} _unique_items;

typedef enum plr_class {
	PC_WARRIOR,
	PC_ROGUE,
	PC_SORCERER,
	PC_MONK,
	PC_BARD,
	PC_BARBARIAN,
	NUM_CLASSES
} plr_class;

typedef enum _walk_path {
	WALK_NE   = 0x1,
	WALK_NW   = 0x2,
	WALK_SE   = 0x3,
	WALK_SW   = 0x4,
	WALK_N    = 0x5,
	WALK_E    = 0x6,
	WALK_S    = 0x7,
	WALK_W    = 0x8,
	WALK_NONE = -1,
} _walk_path;

// Logical equipment locations
typedef enum inv_body_loc {
	INVLOC_HEAD       = 0,
	INVLOC_RING_LEFT  = 1,
	INVLOC_RING_RIGHT = 2,
	INVLOC_AMULET     = 3,
	INVLOC_HAND_LEFT  = 4,
	INVLOC_HAND_RIGHT = 5,
	INVLOC_CHEST      = 6,
	NUM_INVLOC,
} inv_body_loc;

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

typedef enum dlrg_flag {
	DLRG_HDOOR     = 0x01,
	DLRG_VDOOR     = 0x02,
	DLRG_CHAMBER   = 0x40,
	DLRG_PROTECTED = 0x80,
} dlrg_flag;

typedef enum conn_type {
	SELCONN_ZT,
	SELCONN_TCP,
	SELCONN_LOOPBACK,
} conn_type;

} // namespace devilution
