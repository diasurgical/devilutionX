#include <SDL.h>

#include "control.h"
#include "controls/touch/gamepad.h"
#include "diablo.h"
#include "quests.h"
#include "utils/display.h"
#include "utils/ui_fwd.h"

namespace devilution {

VirtualGamepad VirtualGamepadState;

namespace {

constexpr double Pi = 3.141592653589793;

constexpr bool PointsUp(double angle)
{
	constexpr double UpAngle = Pi / 2;
	constexpr double MinAngle = UpAngle - 3 * Pi / 8;
	constexpr double MaxAngle = UpAngle + 3 * Pi / 8;
	return MinAngle <= angle && angle <= MaxAngle;
}

constexpr bool PointsDown(double angle)
{
	constexpr double DownAngle = -Pi / 2;
	constexpr double MinAngle = DownAngle - 3 * Pi / 8;
	constexpr double MaxAngle = DownAngle + 3 * Pi / 8;
	return MinAngle <= angle && angle <= MaxAngle;
}

constexpr bool PointsLeft(double angle)
{
	constexpr double MaxAngle = Pi - 3 * Pi / 8;
	constexpr double MinAngle = -Pi + 3 * Pi / 8;
	return !(MinAngle < angle && angle < MaxAngle);
}

constexpr bool PointsRight(double angle)
{
	constexpr double MinAngle = -3 * Pi / 8;
	constexpr double MaxAngle = 3 * Pi / 8;
	return MinAngle <= angle && angle <= MaxAngle;
}

} // namespace

void InitializeVirtualGamepad()
{
	int screenPixels = std::min(gnScreenWidth, gnScreenHeight);
	int inputMargin = screenPixels / 10;
	int menuButtonWidth = screenPixels / 10;
	int directionPadSize = screenPixels / 4;
	int padButtonSize = round(1.1 * screenPixels / 10);
	int padButtonSpacing = inputMargin / 3;

	float hdpi;
	float vdpi;
	int displayIndex = SDL_GetWindowDisplayIndex(ghMainWnd);
	if (SDL_GetDisplayDPI(displayIndex, nullptr, &hdpi, &vdpi) == 0) {
		int clientWidth;
		int clientHeight;
		if (renderer != nullptr)
			SDL_GetRendererOutputSize(renderer, &clientWidth, &clientHeight);
		else
			SDL_GetWindowSize(ghMainWnd, &clientWidth, &clientHeight);

		hdpi *= static_cast<float>(gnScreenWidth) / clientWidth;
		vdpi *= static_cast<float>(gnScreenHeight) / clientHeight;

		float dpi = std::min(hdpi, vdpi);
		inputMargin = round(0.25 * dpi);
		menuButtonWidth = round(0.2 * dpi);
		directionPadSize = round(dpi);
		padButtonSize = round(0.3 * dpi);
		padButtonSpacing = round(0.1 * dpi);
	}

	int menuPanelTopMargin = 30;
	int menuPanelButtonSpacing = 4;
	Size menuPanelButtonSize = { 64, 62 };
	int rightMarginMenuButton4 = menuPanelButtonSpacing + menuPanelButtonSize.width;
	int rightMarginMenuButton3 = rightMarginMenuButton4 + menuPanelButtonSpacing + menuPanelButtonSize.width;
	int rightMarginMenuButton2 = rightMarginMenuButton3 + menuPanelButtonSpacing + menuPanelButtonSize.width;
	int rightMarginMenuButton1 = rightMarginMenuButton2 + menuPanelButtonSpacing + menuPanelButtonSize.width;

	int padButtonAreaWidth = round(std::sqrt(2) * (padButtonSize + padButtonSpacing));

	int padButtonRight = gnScreenWidth - inputMargin - padButtonSize / 2;
	int padButtonLeft = padButtonRight - padButtonAreaWidth;
	int padButtonBottom = gnScreenHeight - inputMargin - padButtonSize / 2;
	int padButtonTop = padButtonBottom - padButtonAreaWidth;

	Rectangle &charButtonArea = VirtualGamepadState.menuPanel.charButton.area;
	charButtonArea.position.x = gnScreenWidth - rightMarginMenuButton1 * menuButtonWidth / menuPanelButtonSize.width;
	charButtonArea.position.y = menuPanelTopMargin * menuButtonWidth / menuPanelButtonSize.width;
	charButtonArea.size.width = menuButtonWidth;
	charButtonArea.size.height = menuPanelButtonSize.height * menuButtonWidth / menuPanelButtonSize.width;

	Rectangle &questsButtonArea = VirtualGamepadState.menuPanel.questsButton.area;
	questsButtonArea.position.x = gnScreenWidth - rightMarginMenuButton2 * menuButtonWidth / menuPanelButtonSize.width;
	questsButtonArea.position.y = menuPanelTopMargin * menuButtonWidth / menuPanelButtonSize.width;
	questsButtonArea.size.width = menuButtonWidth;
	questsButtonArea.size.height = menuPanelButtonSize.height * menuButtonWidth / menuPanelButtonSize.width;

	Rectangle &inventoryButtonArea = VirtualGamepadState.menuPanel.inventoryButton.area;
	inventoryButtonArea.position.x = gnScreenWidth - rightMarginMenuButton3 * menuButtonWidth / menuPanelButtonSize.width;
	inventoryButtonArea.position.y = menuPanelTopMargin * menuButtonWidth / menuPanelButtonSize.width;
	inventoryButtonArea.size.width = menuButtonWidth;
	inventoryButtonArea.size.height = menuPanelButtonSize.height * menuButtonWidth / menuPanelButtonSize.width;

	Rectangle &mapButtonArea = VirtualGamepadState.menuPanel.mapButton.area;
	mapButtonArea.position.x = gnScreenWidth - rightMarginMenuButton4 * menuButtonWidth / menuPanelButtonSize.width;
	mapButtonArea.position.y = menuPanelTopMargin * menuButtonWidth / menuPanelButtonSize.width;
	mapButtonArea.size.width = menuButtonWidth;
	mapButtonArea.size.height = menuPanelButtonSize.height * menuButtonWidth / menuPanelButtonSize.width;

	Rectangle &menuPanelArea = VirtualGamepadState.menuPanel.area;
	menuPanelArea.position.x = gnScreenWidth - 399 * menuButtonWidth / menuPanelButtonSize.width;
	menuPanelArea.position.y = 0;
	menuPanelArea.size.width = 399 * menuButtonWidth / menuPanelButtonSize.width;
	menuPanelArea.size.height = 162 * menuButtonWidth / menuPanelButtonSize.width;

	VirtualDirectionPad &directionPad = VirtualGamepadState.directionPad;
	Circle &directionPadArea = directionPad.area;
	directionPadArea.position.x = inputMargin + directionPadSize / 2;
	directionPadArea.position.y = gnScreenHeight - inputMargin - directionPadSize / 2;
	directionPadArea.radius = directionPadSize / 2;
	directionPad.position = directionPadArea.position;

	int standButtonDiagonalOffset = directionPadArea.radius + padButtonSpacing / 2 + padButtonSize / 2;
	int standButtonOffset = round(standButtonDiagonalOffset / std::sqrt(2));
	Circle &standButtonArea = VirtualGamepadState.standButton.area;
	standButtonArea.position.x = directionPadArea.position.x - standButtonOffset;
	standButtonArea.position.y = directionPadArea.position.y + standButtonOffset;
	standButtonArea.radius = padButtonSize / 2;

	Circle &primaryActionButtonArea = VirtualGamepadState.primaryActionButton.area;
	primaryActionButtonArea.position.x = padButtonRight;
	primaryActionButtonArea.position.y = (padButtonTop + padButtonBottom) / 2;
	primaryActionButtonArea.radius = padButtonSize / 2;

	Circle &secondaryActionButtonArea = VirtualGamepadState.secondaryActionButton.area;
	secondaryActionButtonArea.position.x = (padButtonLeft + padButtonRight) / 2;
	secondaryActionButtonArea.position.y = padButtonTop;
	secondaryActionButtonArea.radius = padButtonSize / 2;

	Circle &spellActionButtonArea = VirtualGamepadState.spellActionButton.area;
	spellActionButtonArea.position.x = padButtonLeft;
	spellActionButtonArea.position.y = (padButtonTop + padButtonBottom) / 2;
	spellActionButtonArea.radius = padButtonSize / 2;

	Circle &cancelButtonArea = VirtualGamepadState.cancelButton.area;
	cancelButtonArea.position.x = (padButtonLeft + padButtonRight) / 2;
	cancelButtonArea.position.y = padButtonBottom;
	cancelButtonArea.radius = padButtonSize / 2;

	VirtualPadButton &healthButton = VirtualGamepadState.healthButton;
	Circle &healthButtonArea = healthButton.area;
	healthButtonArea.position.x = directionPad.area.position.x - (padButtonSize + padButtonSpacing) / 2;
	healthButtonArea.position.y = directionPad.area.position.y - (directionPadSize + padButtonSize + padButtonSpacing) / 2;
	healthButtonArea.radius = padButtonSize / 2;
	healthButton.isUsable = []() { return !chrflag && !QuestLogIsOpen; };

	VirtualPadButton &manaButton = VirtualGamepadState.manaButton;
	Circle &manaButtonArea = manaButton.area;
	manaButtonArea.position.x = directionPad.area.position.x + (padButtonSize + padButtonSpacing) / 2;
	manaButtonArea.position.y = directionPad.area.position.y - (directionPadSize + padButtonSize + padButtonSpacing) / 2;
	manaButtonArea.radius = padButtonSize / 2;
	manaButton.isUsable = []() { return !chrflag && !QuestLogIsOpen; };

	VirtualGamepadState.isActive = false;
}

void ActivateVirtualGamepad()
{
	VirtualGamepadState.isActive = true;
}

void DeactivateVirtualGamepad()
{
	VirtualGamepadState.Deactivate();
}

void VirtualGamepad::Deactivate()
{
	isActive = false;

	menuPanel.Deactivate();
	directionPad.Deactivate();
	standButton.Deactivate();

	primaryActionButton.Deactivate();
	secondaryActionButton.Deactivate();
	spellActionButton.Deactivate();
	cancelButton.Deactivate();

	healthButton.Deactivate();
	manaButton.Deactivate();
}

void VirtualMenuPanel::Deactivate()
{
	charButton.Deactivate();
	questsButton.Deactivate();
	inventoryButton.Deactivate();
	mapButton.Deactivate();
}

void VirtualDirectionPad::UpdatePosition(Point touchCoordinates)
{
	position = touchCoordinates;

	Displacement diff = position - area.position;
	if (diff == Displacement { 0, 0 }) {
		isUpPressed = false;
		isDownPressed = false;
		isLeftPressed = false;
		isRightPressed = false;
		return;
	}

	if (!area.Contains(position)) {
		int x = diff.deltaX;
		int y = diff.deltaY;
		double dist = sqrt(x * x + y * y);
		x = round(x * area.radius / dist);
		y = round(y * area.radius / dist);
		position.x = area.position.x + x;
		position.y = area.position.y + y;
	}

	double angle = atan2(-diff.deltaY, diff.deltaX);

	isUpPressed = PointsUp(angle);
	isDownPressed = PointsDown(angle);
	isLeftPressed = PointsLeft(angle);
	isRightPressed = PointsRight(angle);
}

void VirtualDirectionPad::Deactivate()
{
	position = area.position;
	isUpPressed = false;
	isDownPressed = false;
	isLeftPressed = false;
	isRightPressed = false;
}

void VirtualButton::Deactivate()
{
	isHeld = false;
	didStateChange = false;
}

} // namespace devilution
