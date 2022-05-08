#include "engine/render/pcx_render.hpp"

#include <algorithm>
#include <cstring>

#include "engine/render/common_impl.h"
#include "utils/log.hpp"

namespace devilution {
namespace {

constexpr uint8_t PcxMaxSinglePixel = 0xBF;
constexpr uint8_t PcxRunLengthMask = 0x3F;

const uint8_t *SkipRestOfPcxLine(const uint8_t *src, unsigned remainingWidth)
{
	while (remainingWidth > 0) {
		const uint8_t value = *src++;
		if (value <= PcxMaxSinglePixel) {
			--remainingWidth;
		} else {
			remainingWidth -= value & PcxRunLengthMask;
			++src;
		}
	}
	return src;
}
template <bool UseColorMap>
void BlitPcxClipY(const Surface &out, Point position, const uint8_t *src, unsigned srcWidth, unsigned srcHeight, const uint8_t *colorMap)
{
	while (position.y < 0 && srcHeight != 0) {
		src = SkipRestOfPcxLine(src, srcWidth);
		++position.y;
		--srcHeight;
	}
	srcHeight = static_cast<unsigned>(std::min<int>(out.h() - position.y, srcHeight));

	const auto dstSkip = static_cast<unsigned>(out.pitch() - srcWidth);
	const unsigned srcSkip = srcWidth % 2;
	uint8_t *dst = &out[position];
	for (unsigned y = 0; y < srcHeight; y++) {
		for (unsigned x = 0; x < srcWidth;) {
			const uint8_t value = *src++;
			if (value <= PcxMaxSinglePixel) {
				*dst++ = UseColorMap ? colorMap[value] : value;
				++x;
			} else {
				const uint8_t runLength = value & PcxRunLengthMask;
				std::memset(dst, UseColorMap ? colorMap[*src++] : *src++, runLength);
				dst += runLength;
				x += runLength;
			}
		}
		dst += dstSkip;
		src += srcSkip;
	}
}

template <bool UseColorMap>
void BlitPcxClipXY(const Surface &out, Point position, const uint8_t *src, unsigned srcWidth, unsigned srcHeight, ClipX clipX, const uint8_t *colorMap)
{
	while (position.y < 0 && srcHeight != 0) {
		src = SkipRestOfPcxLine(src, srcWidth);
		++position.y;
		--srcHeight;
	}
	srcHeight = static_cast<unsigned>(std::min<int>(out.h() - position.y, srcHeight));

	position.x += static_cast<int>(clipX.left);

	const auto dstSkip = static_cast<unsigned>(out.pitch() - clipX.width);
	const unsigned srcSkip = srcWidth % 2;
	uint8_t *dst = &out[position];
	for (unsigned y = 0; y < srcHeight; y++) {
		// Skip initial src if clipping on the left.
		// Handles overshoot, i.e. when the RLE segment goes into the unclipped area.
		auto remainingWidth = clipX.width;
		auto remainingLeftClip = clipX.left;
		while (remainingLeftClip > 0) {
			const uint8_t value = *src++;
			if (value <= PcxMaxSinglePixel) {
				--remainingLeftClip;
			} else {
				const uint8_t runLength = value & PcxRunLengthMask;
				if (runLength > remainingLeftClip) {
					const uint8_t overshoot = runLength - remainingLeftClip;
					if (overshoot > remainingWidth) {
						std::memset(dst, UseColorMap ? colorMap[*src++] : *src++, remainingWidth);
						dst += remainingWidth;
						remainingWidth = 0;
					} else {
						std::memset(dst, UseColorMap ? colorMap[*src++] : *src++, overshoot);
						dst += overshoot;
						remainingWidth -= overshoot;
					}
					remainingLeftClip = 0;
					break;
				} else {
					++src;
					remainingLeftClip -= runLength;
				}
			}
		}

		while (remainingWidth > 0) {
			const uint8_t value = *src++;
			if (value <= PcxMaxSinglePixel) {
				*dst++ = UseColorMap ? colorMap[value] : value;
				--remainingWidth;
				continue;
			}
			const uint8_t runLength = value & PcxRunLengthMask;
			if (runLength > remainingWidth) {
				std::memset(dst, UseColorMap ? colorMap[*src++] : *src++, remainingWidth);
				dst += remainingWidth;
				remainingWidth -= runLength;
				break;
			} else {
				std::memset(dst, UseColorMap ? colorMap[*src++] : *src++, runLength);
			}
			dst += runLength;
			remainingWidth -= runLength;
		}

		src = SkipRestOfPcxLine(src, clipX.right + remainingWidth);

		dst += dstSkip;
		src += srcSkip;
	}
}

template <bool UseColorMap>
void BlitPcxSprite(const Surface &out, Point position, PcxSprite sprite, const uint8_t *colorMap)
{
	const ClipX clipX = CalculateClipX(position.x, sprite.width(), out);
	if (clipX.width <= 0)
		return;
	if (static_cast<unsigned>(clipX.width) == sprite.width()) {
		BlitPcxClipY<UseColorMap>(out, position, sprite.data(), sprite.width(), sprite.height(), colorMap);
	} else {
		BlitPcxClipXY<UseColorMap>(out, position, sprite.data(), sprite.width(), sprite.height(), clipX, colorMap);
	}
}

} // namespace

void RenderPcxSprite(const Surface &out, PcxSprite sprite, Point position)
{
	BlitPcxSprite</*UseColorMap=*/false>(out, position, sprite, /*colorMap=*/nullptr);
}

void RenderPcxSpriteWithColorMap(const Surface &out, PcxSprite sprite, Point position, const std::array<uint8_t, 256> &colorMap)
{
	BlitPcxSprite</*UseColorMap=*/true>(out, position, sprite, colorMap.data());
}

} // namespace devilution
