#pragma once

#include <SDL.h>
#include <cstdint>

#include "./axis_direction.h"
#include "controls/controller.h"

namespace devilution {

enum class GamepadLayout : uint8_t {
	Generic,
	Nintendo,
	PlayStation,
	Xbox,
};

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
};

struct GameActionSendKey {
	uint32_t vk_code;
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

	union {
		GameActionSendKey send_key;
	};
};

bool GetGameAction(const SDL_Event &event, ControllerButtonEvent ctrlEvent, GameAction *action);

bool IsSimulatedMouseClickBinding(ControllerButtonEvent ctrlEvent);

AxisDirection GetMoveDirection();

extern bool start_modifier_active;
extern bool select_modifier_active;
extern const ControllerButton ControllerButtonPrimary;
extern const ControllerButton ControllerButtonSecondary;
extern const ControllerButton ControllerButtonTertiary;

} // namespace devilution
