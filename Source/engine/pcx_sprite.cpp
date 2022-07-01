#include "engine/pcx_sprite.hpp"

namespace devilution {

std::unique_ptr<uint32_t[]> OwnedPcxSpriteSheet::calculateFrameOffsets(PcxSprite sprite, uint16_t numFrames)
{
	uint16_t frameHeight = sprite.height() / numFrames;
	std::unique_ptr<uint32_t[]> frameOffsets { new uint32_t[numFrames] };
	frameOffsets[0] = 0;
	const unsigned width = sprite.width();
	const unsigned srcSkip = width % 2;
	const uint8_t *data = sprite.data();
	for (unsigned frame = 1; frame < numFrames; ++frame) {
		for (unsigned y = 0; y < frameHeight; ++y) {
			for (unsigned x = 0; x < width;) {
				constexpr uint8_t PcxMaxSinglePixel = 0xBF;
				const uint8_t byte = *data++;
				if (byte <= PcxMaxSinglePixel) {
					++x;
					continue;
				}
				constexpr uint8_t PcxRunLengthMask = 0x3F;
				const uint8_t runLength = (byte & PcxRunLengthMask);
				++data;
				x += runLength;
			}
			data += srcSkip;
		}
		frameOffsets[frame] = static_cast<uint32_t>(data - sprite.data());
	}
	return frameOffsets;
}

} // namespace devilution
