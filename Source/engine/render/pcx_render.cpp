#include "engine/render/pcx_render.hpp"

#include <algorithm>
#include <cstring>

#include "engine/render/common_impl.h"
#include "utils/log.hpp"

namespace devilution {
namespace {

constexpr uint8_t PcxMaxSinglePixel = 0xBF;
constexpr uint8_t PcxRunLengthMask = 0x3F;

BlitCommand PcxGetBlitCommand(const uint8_t *src)
{
	const uint8_t value = *src++;
	if (value <= PcxMaxSinglePixel)
		return BlitCommand { BlitType::Pixel, src, 1, value };
	const uint8_t runLength = value & PcxRunLengthMask;
	const uint8_t color = *src++;
	return BlitCommand { BlitType::Fill, src, runLength, color };
}

} // namespace

void RenderPcxSprite(const Surface &out, PcxSprite sprite, Point position)
{
	if (sprite.transparentColor()) {
		DoRenderForwards<PcxGetBlitCommand>(out, position, sprite.data(), sprite.width(), sprite.height(), BlitDirect {},
		    TransformBlitCommandTransparentColor { *sprite.transparentColor() });
	} else {
		DoRenderForwards<PcxGetBlitCommand>(out, position, sprite.data(), sprite.width(), sprite.height(), BlitDirect {},
		    TransformBlitCommandNoop);
	}
}

void RenderPcxSpriteWithColorMap(const Surface &out, PcxSprite sprite, Point position, const std::array<uint8_t, 256> &colorMap)
{
	if (sprite.transparentColor()) {
		DoRenderForwards<PcxGetBlitCommand>(out, position, sprite.data(), sprite.width(), sprite.height(), BlitWithMap { colorMap.data() },
		    TransformBlitCommandTransparentColor { *sprite.transparentColor() });
	} else {
		DoRenderForwards<PcxGetBlitCommand>(out, position, sprite.data(), sprite.width(), sprite.height(), BlitWithMap { colorMap.data() },
		    TransformBlitCommandNoop);
	}
}

} // namespace devilution
