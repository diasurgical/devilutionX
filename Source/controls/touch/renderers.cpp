#include "controls/touch/renderers.h"
#include "engine.h"

namespace devilution {

namespace {

VirtualGamepadRenderer Renderer(&VirtualGamepadState);

} // namespace

void DrawVirtualGamepad(const Surface &out)
{
	Renderer.Render(out);
}

void VirtualGamepadRenderer::Render(const Surface &out)
{
	directionPadRenderer.Render(out);
	primaryActionButtonRenderer.Render(out);
	secondaryActionButtonRenderer.Render(out);
	spellActionButtonRenderer.Render(out);
	cancelButtonRenderer.Render(out);
}

void VirtualDirectionPadRenderer::Render(const Surface &out)
{
	auto center = virtualDirectionPad->area.position;
	auto radius = virtualDirectionPad->area.radius;
	int diameter = 2 * radius;

	int x = center.x - radius;
	int y = center.y - radius;
	int width = diameter;
	int height = diameter;
	SDL_Rect rect { x, y, width, height };

	auto pixelFormat = out.surface->format;
	auto color = SDL_MapRGBA(pixelFormat, 0, 255, 0, 255);
	SDL_FillRect(out.surface, &rect, color);
}

void VirtualPadButtonRenderer::Render(const Surface &out)
{
	auto center = virtualPadButton->area.position;
	auto radius = virtualPadButton->area.radius;
	int diameter = 2 * radius;

	int x = center.x - radius;
	int y = center.y - radius;
	int width = diameter;
	int height = diameter;
	SDL_Rect rect { x, y, width, height };

	auto pixelFormat = out.surface->format;
	auto color = SDL_MapRGBA(pixelFormat, 0, 255, 0, 255);
	SDL_FillRect(out.surface, &rect, color);
}

} // namespace devilution
