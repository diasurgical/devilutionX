#include "utils/cl2_to_clx.hpp"

#include <cstdint>
#include <cstring>

#include <vector>

#include "utils/clx_decode.hpp"
#include "utils/clx_encode.hpp"
#include "utils/endian.hpp"

namespace devilution {

namespace {

constexpr size_t FrameHeaderSize = 10;

struct SkipSize {
	int_fast16_t wholeLines;
	int_fast16_t xOffset;
};
SkipSize GetSkipSize(int_fast16_t overrun, int_fast16_t srcWidth)
{
	SkipSize result;
	result.wholeLines = overrun / srcWidth;
	result.xOffset = overrun - srcWidth * result.wholeLines;
	return result;
}

} // namespace

uint16_t Cl2ToClx(const uint8_t *data, size_t size,
    PointerOrValue<uint16_t> widthOrWidths, std::vector<uint8_t> &clxData)
{
	uint32_t numGroups = 1;
	const uint32_t maybeNumFrames = LoadLE32(data);
	const uint8_t *groupBegin = data;

	// If it is a number of frames, then the last frame offset will be equal to the size of the file.
	if (LoadLE32(&data[maybeNumFrames * 4 + 4]) != size) {
		// maybeNumFrames is the address of the first group, right after
		// the list of group offsets.
		numGroups = maybeNumFrames / 4;
		clxData.resize(maybeNumFrames);
	}

	// Transient buffer for a contiguous run of non-transparent pixels.
	std::vector<uint8_t> pixels;
	pixels.reserve(4096);

	for (size_t group = 0; group < numGroups; ++group) {
		uint32_t numFrames;
		if (numGroups == 1) {
			numFrames = maybeNumFrames;
		} else {
			groupBegin = &data[LoadLE32(&data[group * 4])];
			numFrames = LoadLE32(groupBegin);
			WriteLE32(&clxData[4 * group], clxData.size());
		}

		// CLX header: frame count, frame offset for each frame, file size
		const size_t clxDataOffset = clxData.size();
		clxData.resize(clxData.size() + 4 * (2 + static_cast<size_t>(numFrames)));
		WriteLE32(&clxData[clxDataOffset], numFrames);

		const uint8_t *frameEnd = &groupBegin[LoadLE32(&groupBegin[4])];
		for (size_t frame = 1; frame <= numFrames; ++frame) {
			WriteLE32(&clxData[clxDataOffset + 4 * frame],
			    static_cast<uint32_t>(clxData.size() - clxDataOffset));

			const uint8_t *frameBegin = frameEnd;
			frameEnd = &groupBegin[LoadLE32(&groupBegin[4 * (frame + 1)])];

			const uint16_t frameWidth = widthOrWidths.HoldsPointer() ? widthOrWidths.AsPointer()[frame - 1] : widthOrWidths.AsValue();

			const size_t frameHeaderPos = clxData.size();
			clxData.resize(clxData.size() + FrameHeaderSize);
			WriteLE16(&clxData[frameHeaderPos], FrameHeaderSize);
			WriteLE16(&clxData[frameHeaderPos + 2], frameWidth);

			unsigned transparentRunWidth = 0;
			int_fast16_t xOffset = 0;
			size_t frameHeight = 0;
			const uint8_t *src = frameBegin + FrameHeaderSize;
			while (src != frameEnd) {
				auto remainingWidth = static_cast<int_fast16_t>(frameWidth) - xOffset;
				while (remainingWidth > 0) {
					const BlitCommand cmd = ClxGetBlitCommand(src);
					switch (cmd.type) {
					case BlitType::Transparent:
						if (!pixels.empty()) {
							AppendClxPixelsOrFillRun(pixels.data(), pixels.size(), clxData);
							pixels.clear();
						}

						transparentRunWidth += cmd.length;
						break;
					case BlitType::Fill:
					case BlitType::Pixels:
						AppendClxTransparentRun(transparentRunWidth, clxData);
						transparentRunWidth = 0;

						if (cmd.type == BlitType::Fill) {
							pixels.insert(pixels.end(), cmd.length, cmd.color);
						} else { // BlitType::Pixels
							pixels.insert(pixels.end(), src + 1, cmd.srcEnd);
						}
						break;
					}
					src = cmd.srcEnd;
					remainingWidth -= cmd.length;
				}

				++frameHeight;
				if (remainingWidth < 0) {
					const auto skipSize = GetSkipSize(-remainingWidth, static_cast<int_fast16_t>(frameWidth));
					xOffset = skipSize.xOffset;
					frameHeight += skipSize.wholeLines;
				} else {
					xOffset = 0;
				}
			}
			if (!pixels.empty()) {
				AppendClxPixelsOrFillRun(pixels.data(), pixels.size(), clxData);
				pixels.clear();
			}
			AppendClxTransparentRun(transparentRunWidth, clxData);

			WriteLE16(&clxData[frameHeaderPos + 4], frameHeight);
			memset(&clxData[frameHeaderPos + 6], 0, 4);
		}

		WriteLE32(&clxData[clxDataOffset + 4 * (1 + static_cast<size_t>(numFrames))], static_cast<uint32_t>(clxData.size() - clxDataOffset));
	}
	return numGroups == 1 ? 0 : numGroups;
}

} // namespace devilution
