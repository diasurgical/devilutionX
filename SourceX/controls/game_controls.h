#pragma once

#include "devilution.h"

namespace dvl {

enum class GameActionType {
	NONE = 0,
	USE_HEALTH_POTION,
	USE_MANA_POTION,
	PRIMARY_ACTION,   // Talk to towners, click on inv items, attack, etc.
	SECONDARY_ACTION, // Open chests, doors, pickup items.
	TOGGLE_INVENTORY,
	TOGGLE_CHARACTER_INFO,
	TOGGLE_QUICK_SPELL_MENU,
	SEND_KEY,
	SEND_MOUSE_LEFT_CLICK
};

struct GameActionSendKey {
	DWORD vk_code;
	bool up;
};

struct GameActionSendMouseLeftClick {
	bool up;
};

struct GameAction {
	GameActionType type;

	GameAction()
	    : type(GameActionType::NONE)
	{
	}

	explicit GameAction(GameActionType type)
	    : type(type)
	{
	}

	GameAction(GameActionSendKey send_key)
	    : type(GameActionType::SEND_KEY)
	    , send_key(send_key)
	{
	}

	GameAction(GameActionSendMouseLeftClick send_mouse_left_click)
	    : type(GameActionType::SEND_MOUSE_LEFT_CLICK)
	    , send_mouse_left_click(send_mouse_left_click)
	{
	}

	union {
		GameActionSendKey send_key;
		GameActionSendMouseLeftClick send_mouse_left_click;
	};
};

bool GetGameAction(const SDL_Event &event, GameAction *action);
bool ShouldSkipMovie(const SDL_Event &event);

enum class MoveDirectionX {
	NONE = 0,
	LEFT,
	RIGHT
};
enum class MoveDirectionY {
	NONE = 0,
	UP,
	DOWN
};
struct MoveDirection {
	MoveDirectionX x;
	MoveDirectionY y;
};
MoveDirection GetMoveDirection();

} // namespace dvl
