#pragma once

#include "all.h"

namespace dvl {
namespace GameActionTypeNS {
enum GameActionType {
	NONE = 0,
	USE_HEALTH_POTION,
	USE_MANA_POTION,
	PRIMARY_ACTION,   // Talk to towners, click on inv items, attack, etc.
	SECONDARY_ACTION, // Open chests, doors, pickup items.
	CAST_SPELL,
	TOGGLE_INVENTORY,
	TOGGLE_CHARACTER_INFO,
	TOGGLE_QUICK_SPELL_MENU,
	TOGGLE_SPELL_BOOK,
	TOGGLE_QUEST_LOG,
	SEND_KEY,
	SEND_MOUSE_CLICK,
};
}

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
		type = GameActionTypeNS::NONE;
	}

	GameAction(GameActionTypeNS::GameActionType ptype) {
		type = ptype;
	}

	GameAction(GameActionSendKey skey) {
		type = GameActionTypeNS::SEND_KEY;
		send_key = skey;
	}

	GameAction(GameActionSendMouseClick s_mouse_click) {
		type = GameActionTypeNS::SEND_MOUSE_CLICK;
		send_mouse_click = s_mouse_click;
	}

//private:
	GameActionTypeNS::GameActionType type;
		GameActionSendKey send_key;
		GameActionSendMouseClick send_mouse_click;

};

bool GetGameAction(const SDL_Event &event, GameAction *action);

namespace MoveDirectionXNS {
enum MoveDirectionX {
	NONE = 0,
	LEFT,
	RIGHT
};
}

namespace MoveDirectionYNS {
enum MoveDirectionY {
	NONE = 0,
	UP,
	DOWN
};
}

struct MoveDirection {
	MoveDirection()
	{
		x = MoveDirectionXNS::NONE;
		y = MoveDirectionYNS::NONE;
	}

	MoveDirection(MoveDirectionXNS::MoveDirectionX MX, MoveDirectionYNS::MoveDirectionY MY)
	{
		x = MX;
		y = MY;
	}

	MoveDirectionXNS::MoveDirectionX x;
	MoveDirectionYNS::MoveDirectionY y;
};
MoveDirection GetMoveDirection();

extern bool start_modifier_active;
extern bool select_modifier_active;

} // namespace dvl
