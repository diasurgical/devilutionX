#pragma once

#include <vector>

#include <SDL.h>

#include "controls/controller_buttons.h"
#include "controls/game_controls.h"

namespace devilution {

class GameController {
	static std::vector<GameController *> controllers_;
	static GameController *currentGameController_;

public:
#ifdef SWAP_CONFIRM_CANCEL_BUTTONS
	static const ControllerButton ControllerButton_CONFIRM = ControllerButton_BUTTON_B; // right button
	static const ControllerButton ControllerButton_CANCEL = ControllerButton_BUTTON_A;  // bottom button
#else
	static const ControllerButton ControllerButton_CONFIRM = ControllerButton_BUTTON_A;
	static const ControllerButton ControllerButton_CANCEL = ControllerButton_BUTTON_B;
#endif

	static void Add(int joystickIndex);
	static void Remove(SDL_JoystickID instanceId);
	static GameController *Get(SDL_JoystickID instanceId);
	static GameController *Get(const SDL_Event &event);
	static GameController *GetCurrentGameController();
	static const std::vector<GameController *> &All();
	static bool IsPressedOnAnyController(ControllerButton button);

	// Must be called exactly once at the start of each SDL input event.
	void UnlockTriggerState();

	ControllerButton ToControllerButton(const SDL_Event &event);

	bool IsPressed(ControllerButton button) const;
	static bool ProcessAxisMotion(const SDL_Event &event);
	bool GetGameAction(const SDL_Event &event, ControllerButtonEvent ctrlEvent, GameAction *action);
	virtual AxisDirection GetMoveDirection() = 0;

	MenuAction GetAButtonMenuAction();
	MenuAction GetBButtonMenuAction();

	virtual bool GetStartModifierLeftCircleMenuHint(CircleMenuHint *hint) = 0;
	virtual bool GetStartModifierRightCircleMenuHint(CircleMenuHint *hint) = 0;
	virtual bool GetSelectModifierLeftCircleMenuHint(CircleMenuHint *hint) = 0;
	virtual bool GetSelectModifierRightCircleMenuHint(CircleMenuHint *hint) = 0;

protected:
	static SDL_GameControllerButton ToSdlGameControllerButton(ControllerButton button);

	uint32_t TranslateControllerButtonToKey(ControllerButton controllerButton);
	virtual bool HandleControllerButtonEvent(const SDL_Event &event, ControllerButtonEvent ctrlEvent, GameAction *action) = 0;

	SDL_GameController *sdl_game_controller_ = NULL;
	SDL_JoystickID instance_id_ = -1;

	ControllerButton trigger_left_state_ = ControllerButton_NONE;
	ControllerButton trigger_right_state_ = ControllerButton_NONE;
	bool trigger_left_is_down_ = false;
	bool trigger_right_is_down_ = false;
	bool start_modifier_active = false;
	bool select_modifier_active = false;
};

} // namespace devilution
