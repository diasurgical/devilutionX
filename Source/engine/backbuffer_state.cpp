#include "engine/backbuffer_state.hpp"

#include <unordered_map>

#include "engine/dx.h"
#include "utils/enum_traits.h"

namespace devilution {
namespace {

struct RedrawState {
	enum {
		RedrawNone,
		RedrawViewportOnly,
		RedrawAll
	} Redraw;
	std::array<bool, enum_size<PanelDrawComponent>::value> redrawComponents;
};

struct BackbufferState {
	RedrawState redrawState;
	DrawnCursor cursor;
};

std::unordered_map<void *, BackbufferState> States;

BackbufferState &GetBackbufferState()
{
	// `PalSurface` is null in headless mode.
	void *ptr = PalSurface != nullptr ? PalSurface->pixels : nullptr;
	auto result = States.emplace(std::piecewise_construct, std::forward_as_tuple(ptr), std::forward_as_tuple());
	BackbufferState &state = result.first->second;
	if (result.second)
		state.redrawState.Redraw = RedrawState::RedrawAll;
	return state;
}

} // namespace

bool IsRedrawEverything()
{
	return GetBackbufferState().redrawState.Redraw == RedrawState::RedrawAll;
}

void RedrawViewport()
{
	for (auto &&it : States) {
		if (it.second.redrawState.Redraw != RedrawState::RedrawAll) {
			it.second.redrawState.Redraw = RedrawState::RedrawViewportOnly;
		}
	}
}

bool IsRedrawViewport()
{
	return GetBackbufferState().redrawState.Redraw == RedrawState::RedrawViewportOnly;
}

void RedrawComplete()
{
	GetBackbufferState().redrawState.Redraw = RedrawState::RedrawNone;
}

void RedrawEverything()
{
	for (auto &&it : States) {
		it.second.redrawState.Redraw = RedrawState::RedrawAll;
	}
}

void InitBackbufferState()
{
	States.clear();
}

void RedrawComponent(PanelDrawComponent component)
{
	for (auto &&it : States) {
		it.second.redrawState.redrawComponents[static_cast<size_t>(component)] = true;
	}
}

bool IsRedrawComponent(PanelDrawComponent component)
{
	return GetBackbufferState().redrawState.redrawComponents[static_cast<size_t>(component)];
}

void RedrawComponentComplete(PanelDrawComponent component)
{
	GetBackbufferState().redrawState.redrawComponents[static_cast<size_t>(component)] = false;
}

DrawnCursor &GetDrawnCursor()
{
	return GetBackbufferState().cursor;
}

} // namespace devilution
