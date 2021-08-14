#ifndef USE_SDL1

#include "controls/touch/event_handlers.h"

#include "control.h"
#include "diablo.h"
#include "engine.h"
#include "gmenu.h"
#include "scrollrt.h"
#include "stores.h"
#include "utils/ui_fwd.h"

namespace devilution {

namespace {

VirtualGamepadEventHandler Handler(&VirtualGamepadState);

Point ScaleToScreenCoordinates(float x, float y)
{
	return Point {
		(int)round(x * gnScreenWidth),
		(int)round(y * gnScreenHeight)
	};
}

void SimulateMouseMovement(const SDL_Event &event)
{
	float x = event.tfinger.x;
	float y = event.tfinger.y;
	MousePosition = ScaleToScreenCoordinates(x, y);
	sgbControllerActive = false;
}

bool HandleGameMenuInteraction(const SDL_Event &event)
{
	if (!gmenu_is_active())
		return false;
	if (event.type == SDL_FINGERDOWN && gmenu_left_mouse(true))
		return true;
	if (event.type == SDL_FINGERMOTION && gmenu_on_mouse_move())
		return true;
	return event.type == SDL_FINGERUP && gmenu_left_mouse(false);
}

bool HandleStoreInteraction(const SDL_Event &event)
{
	if (stextflag == STORE_NONE)
		return false;
	if (event.type == SDL_FINGERDOWN)
		CheckStoreBtn();
	return true;
}

bool HandleSpeedbookInteraction(const SDL_Event &event)
{
	if (!spselflag)
		return false;
	if (event.type == SDL_FINGERUP)
		SetSpell();
	return true;
}

void HandleBottomPanelInteraction(const SDL_Event &event)
{
	ClearPanBtn();

	if (event.type != SDL_FINGERUP) {
		spselflag = true;
		DoPanBtn();
		spselflag = false;
	} else {
		DoPanBtn();
		if (panbtndown)
			CheckBtnUp();
	}
}

} // namespace

void HandleTouchEvent(const SDL_Event &event)
{
	if (Handler.Handle(event))
		return;

	if (!IsAnyOf(event.type, SDL_FINGERDOWN, SDL_FINGERUP, SDL_FINGERMOTION))
		return;

	SimulateMouseMovement(event);

	if (HandleGameMenuInteraction(event))
		return;

	if (HandleStoreInteraction(event))
		return;

	if (HandleSpeedbookInteraction(event))
		return;

	HandleBottomPanelInteraction(event);
}

bool VirtualGamepadEventHandler::Handle(const SDL_Event &event)
{
	if (!IsAnyOf(event.type, SDL_FINGERDOWN, SDL_FINGERUP, SDL_FINGERMOTION)) {
		VirtualGamepadState.primaryActionButton.didStateChange = false;
		VirtualGamepadState.secondaryActionButton.didStateChange = false;
		VirtualGamepadState.spellActionButton.didStateChange = false;
		VirtualGamepadState.cancelButton.didStateChange = false;
		return false;
	}

	if (directionPadEventHandler.Handle(event))
		return true;

	if (primaryActionButtonEventHandler.Handle(event))
		return true;

	if (secondaryActionButtonEventHandler.Handle(event))
		return true;

	if (spellActionButtonEventHandler.Handle(event))
		return true;

	if (cancelButtonEventHandler.Handle(event))
		return true;

	return false;
}

bool VirtualDirectionPadEventHandler::Handle(const SDL_Event &event)
{
	switch (event.type) {
	case SDL_FINGERDOWN:
		return HandleFingerDown(event.tfinger);

	case SDL_FINGERUP:
		return HandleFingerUp(event.tfinger);

	case SDL_FINGERMOTION:
		return HandleFingerMotion(event.tfinger);

	default:
		return false;
	}
}

bool VirtualDirectionPadEventHandler::HandleFingerDown(const SDL_TouchFingerEvent &event)
{
	if (isActive)
		return false;

	float x = event.x;
	float y = event.y;

	Point touchCoordinates = ScaleToScreenCoordinates(x, y);
	if (!virtualDirectionPad->area.Contains(touchCoordinates))
		return false;

	virtualDirectionPad->UpdatePosition(touchCoordinates);
	activeFinger = event.fingerId;
	isActive = true;
	return true;
}

bool VirtualDirectionPadEventHandler::HandleFingerUp(const SDL_TouchFingerEvent &event)
{
	if (!isActive || event.fingerId != activeFinger)
		return false;

	Point position = virtualDirectionPad->area.position;
	virtualDirectionPad->UpdatePosition(position);
	isActive = false;
	return true;
}

bool VirtualDirectionPadEventHandler::HandleFingerMotion(const SDL_TouchFingerEvent &event)
{
	if (!isActive || event.fingerId != activeFinger)
		return false;

	float x = event.x;
	float y = event.y;

	Point touchCoordinates = ScaleToScreenCoordinates(x, y);
	virtualDirectionPad->UpdatePosition(touchCoordinates);
	return true;
}

bool VirtualPadButtonEventHandler::Handle(const SDL_Event &event)
{
	virtualPadButton->didStateChange = false;

	switch (event.type) {
	case SDL_FINGERDOWN:
		return HandleFingerDown(event.tfinger);

	case SDL_FINGERUP:
		return HandleFingerUp(event.tfinger);

	case SDL_FINGERMOTION:
		return HandleFingerMotion(event.tfinger);

	default:
		return false;
	}
}

bool VirtualPadButtonEventHandler::HandleFingerDown(const SDL_TouchFingerEvent &event)
{
	if (isActive)
		return false;

	float x = event.x;
	float y = event.y;

	Point touchCoordinates = ScaleToScreenCoordinates(x, y);
	if (!virtualPadButton->area.Contains(touchCoordinates))
		return false;

	virtualPadButton->isHeld = true;
	virtualPadButton->didStateChange = true;
	activeFinger = event.fingerId;
	isActive = true;
	return true;
}

bool VirtualPadButtonEventHandler::HandleFingerUp(const SDL_TouchFingerEvent &event)
{
	if (!isActive || event.fingerId != activeFinger)
		return false;

	if (virtualPadButton->isHeld)
		virtualPadButton->didStateChange = true;

	virtualPadButton->isHeld = false;
	isActive = false;
	return true;
}

bool VirtualPadButtonEventHandler::HandleFingerMotion(const SDL_TouchFingerEvent &event)
{
	if (!isActive || event.fingerId != activeFinger)
		return false;

	float x = event.x;
	float y = event.y;
	Point touchCoordinates = ScaleToScreenCoordinates(x, y);

	bool wasHeld = virtualPadButton->isHeld;
	virtualPadButton->isHeld = virtualPadButton->area.Contains(touchCoordinates);
	virtualPadButton->didStateChange = virtualPadButton->isHeld != wasHeld;

	return true;
}

} // namespace devilution

#endif
