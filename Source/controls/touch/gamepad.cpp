#include <SDL.h>

#include "controls/touch/gamepad.h"
#include "diablo.h"
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
	int directionPadSize = screenPixels / 4;
	int padButtonSize = round(1.1 * screenPixels / 10);
	int padButtonSpacing = inputMargin / 3;

	float hdpi;
	float vdpi;
	int displayIndex = SDL_GetWindowDisplayIndex(ghMainWnd);
	if (SDL_GetDisplayDPI(displayIndex, nullptr, &hdpi, &vdpi) == 0) {
		int clientWidth;
		int clientHeight;
		SDL_GetWindowSize(ghMainWnd, &clientWidth, &clientHeight);
		hdpi *= static_cast<float>(gnScreenWidth) / clientWidth;
		vdpi *= static_cast<float>(gnScreenHeight) / clientHeight;

		float dpi = std::min(hdpi, vdpi);
		inputMargin = round(0.25 * dpi);
		directionPadSize = round(dpi);
		padButtonSize = round(0.3 * dpi);
		padButtonSpacing = round(0.1 * dpi);
	}

	int padButtonAreaWidth = round(std::sqrt(2) * (padButtonSize + padButtonSpacing));

	int padButtonRight = gnScreenWidth - inputMargin - padButtonSize / 2;
	int padButtonLeft = padButtonRight - padButtonAreaWidth;
	int padButtonBottom = gnScreenHeight - inputMargin - padButtonSize / 2;
	int padButtonTop = padButtonBottom - padButtonAreaWidth;

	VirtualDirectionPad &directionPad = VirtualGamepadState.directionPad;
	directionPad.area.position.x = inputMargin + directionPadSize / 2;
	directionPad.area.position.y = gnScreenHeight - inputMargin - directionPadSize / 2;
	directionPad.area.radius = directionPadSize / 2;
	directionPad.position = directionPad.area.position;

	VirtualPadButton &primaryActionButton = VirtualGamepadState.primaryActionButton;
	primaryActionButton.area.position.x = padButtonRight;
	primaryActionButton.area.position.y = (padButtonTop + padButtonBottom) / 2;
	primaryActionButton.area.radius = padButtonSize / 2;

	VirtualPadButton &secondaryActionButton = VirtualGamepadState.secondaryActionButton;
	secondaryActionButton.area.position.x = (padButtonLeft + padButtonRight) / 2;
	secondaryActionButton.area.position.y = padButtonTop;
	secondaryActionButton.area.radius = padButtonSize / 2;

	VirtualPadButton &spellActionButton = VirtualGamepadState.spellActionButton;
	spellActionButton.area.position.x = padButtonLeft;
	spellActionButton.area.position.y = (padButtonTop + padButtonBottom) / 2;
	spellActionButton.area.radius = padButtonSize / 2;

	VirtualPadButton &cancelButton = VirtualGamepadState.cancelButton;
	cancelButton.area.position.x = (padButtonLeft + padButtonRight) / 2;
	cancelButton.area.position.y = padButtonBottom;
	cancelButton.area.radius = padButtonSize / 2;
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

} // namespace devilution
