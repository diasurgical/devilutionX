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
	GameActionSendKey()
	{
	}

	GameActionSendKey(DWORD vkcode, bool bUp)
	{
		vk_code = vkcode;
		up = bUp;
	}

	DWORD vk_code;
	bool up;
};

struct GameActionSendMouseClick {
	enum Button {
		LEFT = 0,
		RIGHT,
	};

	GameActionSendMouseClick() {
	}

	GameActionSendMouseClick(Button pButton, bool bUp) {
		button = pButton;
		up = bUp;
	}

	Button button;
	bool up;
};

struct GameAction {
	GameAction() {
		type = GameActionType_NONE;
	}

	GameAction(GameActionType ptype) {
		type = ptype;
	}

	GameAction(GameActionSendKey skey) {
		type = GameActionType_SEND_KEY;
		send_key = skey;
	}

	GameAction(GameActionSendMouseClick s_mouse_click) {
		type = GameActionType_SEND_MOUSE_CLICK;
		send_mouse_click = s_mouse_click;
	}

//private:
	GameActionType type;
		GameActionSendKey send_key;
		GameActionSendMouseClick send_mouse_click;

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
	MoveDirection()
	{
		x = MoveDirectionX_NONE;
		y = MoveDirectionY_NONE;
	}

	MoveDirection(MoveDirectionX MX, MoveDirectionY MY)
	{
		x = MX;
		y = MY;
	}

	MoveDirectionX x;
	MoveDirectionY y;
};
MoveDirection GetMoveDirection();

extern bool start_modifier_active;
extern bool select_modifier_active;

} // namespace dvl
