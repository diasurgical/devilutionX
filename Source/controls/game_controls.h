#pragma once

#include <SDL.h>
#include <cstdint>

#include "./axis_direction.h"
#include "controls/controller.h"

namespace devilution {

enum GameActionType : uint8_t {
	GameActionType_NONE,
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
	Uint32 vk_code;
	bool up;
};

struct GameActionSendMouseClick {
	enum Button : uint8_t {
		LEFT,
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

bool GetGameAction(const SDL_Event &event, ControllerButtonEvent ctrlEvent, GameAction *action);

AxisDirection GetMoveDirection();

extern bool start_modifier_active;
extern bool select_modifier_active;

} // namespace devilution
