#pragma once

#include <cstdint>

#include "engine/rectangle.hpp"
#include "engine/surface.hpp"

namespace devilution {

enum class PanelDrawComponent {
	Health,
	Mana,
	ControlButtons,
	Belt,

	FIRST = Health,
	LAST = Belt
};

struct DrawnCursor {
	Rectangle rect;
	uint8_t behindBuffer[8192];
};

void InitBackbufferState();

void RedrawEverything();
bool IsRedrawEverything();
void RedrawViewport();
bool IsRedrawViewport();
void RedrawComplete();

void RedrawComponent(PanelDrawComponent component);
bool IsRedrawComponent(PanelDrawComponent component);
void RedrawComponentComplete(PanelDrawComponent component);

DrawnCursor &GetDrawnCursor();

} // namespace devilution
