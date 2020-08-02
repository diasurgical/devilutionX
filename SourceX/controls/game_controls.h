#pragma once

#include "all.h"

namespace dvl {

enum GameActionType {
	GameActionType_NONE = 0,
	GameActionType_USE_HEALTH_POTION,
	GameActionType_USE_MANA_POTION,
	GameActionType_PRIMARY_ACTION,   // Talk to towners, click on inv items, attack, etc.
	GameActionType_SECONDARY_ACTION, // Open chests, doors, pickup items.
	GameActionType_CAST_SPELL,
	GameActionType_TOGGLE_INVENTORY,
	GameActionType_TOGGLE_CHARACTER_INFO,
	GameActionType_TOGGLE_QUICK_SPELL_MENU,
	GameActionType_TOGGLE_SPELL_BOOK,
	GameActionType_TOGGLE_QUEST_LOG,
	GameActionType_SEND_KEY,
	GameActionType_SEND_MOUSE_CLICK,
};

struct GameActionSendKey {
	DWORD vk_code;
	bool up;
};

struct GameActionSendMouseClick {
	enum Button {
		LEFT = 0,
		RIGHT,
	};
	Button button;
	bool up;
};

struct GameAction {
	GameActionType type;

	GameAction()
	    : type(GameActionType_NONE)
	{
	}

	explicit GameAction(GameActionType type)
	    : type(type)
	{
	}

	GameAction(GameActionSendKey send_key)
	    : type(GameActionType_SEND_KEY)
	    , send_key(send_key)
	{
	}

	GameAction(GameActionSendMouseClick send_mouse_click)
	    : type(GameActionType_SEND_MOUSE_CLICK)
	    , send_mouse_click(send_mouse_click)
	{
	}

	union {
		GameActionSendKey send_key;
		GameActionSendMouseClick send_mouse_click;
	};
};

bool GetGameAction(const SDL_Event &event, GameAction *action);

enum MoveDirectionX {
	MoveDirectionX_NONE = 0,
	MoveDirectionX_LEFT,
	MoveDirectionX_RIGHT
};
enum MoveDirectionY {
	MoveDirectionY_NONE = 0,
	MoveDirectionY_UP,
	MoveDirectionY_DOWN
};
struct MoveDirection {
	MoveDirectionX x;
	MoveDirectionY y;
};
MoveDirection GetMoveDirection();

extern bool start_modifier_active;
extern bool select_modifier_active;

} // namespace dvl
