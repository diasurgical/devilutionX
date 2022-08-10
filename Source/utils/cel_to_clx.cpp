#include "utils/cel_to_clx.hpp"

#include <cstring>

#include <array>
#include <vector>

#ifdef DEBUG_CEL_TO_CL2_SIZE
#include <iomanip>
#include <iostream>
#endif

#include "appfat.h"
#include "utils/clx_write.hpp"
#include "utils/endian.hpp"
#include "utils/str_cat.hpp"

#ifdef USE_SDL1
#include "utils/sdl2_to_1_2_backports.h"
#endif

namespace devilution {

namespace {

constexpr bool IsCelTransparent(uint8_t control)
{
	constexpr uint8_t CelTransparentMin = 0x80;
	return control >= CelTransparentMin;
}

constexpr uint8_t GetCelTransparentWidth(uint8_t control)
{
	return -static_cast<int8_t>(control);
}

void ReadLE32s(SDL_RWops *rwops, size_t n, uint32_t *out)
{
	const size_t numRead = SDL_RWread(rwops, out, 4, n);
	if (numRead != n)
		app_fatal(StrCat("Read failed: ", SDL_GetError()));
#if SDL_BYTEORDER != SDL_LIL_ENDIAN
	for (size_t i = 0; i < n; ++i)
		out[i] = SDL_SwapLE32(out[i]);
#endif
}

void ReadUint8s(SDL_RWops *rwops, size_t n, uint8_t *out)
{
	const size_t numRead = SDL_RWread(rwops, out, 1, n);
	if (numRead != n)
		app_fatal(StrCat("Read failed: ", SDL_GetError()));
}

uint32_t ReadLE32(SDL_RWops *rwops)
{
	uint8_t buf[4];
	const size_t numRead = SDL_RWread(rwops, buf, 4, 1);
	if (numRead == 0)
		app_fatal(StrCat("Read failed: ", SDL_GetError()));
	return LoadLE32(buf);
}

void Seek(SDL_RWops *rwops, Sint64 offset, int whence)
{
	if (SDL_RWseek(rwops, offset, whence) == -1)
		app_fatal(StrCat("Seek failed: ", SDL_GetError()));
}

uint32_t GetMaxSizeFromOffsets(uint32_t *offsets, size_t size)
{
	uint32_t max = 0;
	uint32_t prev = *offsets++;
	while (--size > 0) {
		uint32_t cur = *offsets++;
		max = std::max(max, cur - prev);
		prev = cur;
	}
	return max;
}

} // namespace

OwnedClxSpriteListOrSheet CelToClx(SDL_RWops *rwops, PointerOrValue<uint16_t> widthOrWidths)
{
	// A CEL file either begins with:
	// 1. A CEL header.
	// 2. A list of offsets to frame groups (each group is a CEL file).
	size_t groupsHeaderSize = 0;
	uint32_t numGroups = 1;

	const Sint64 sizeResult = SDL_RWsize(rwops);
	if (sizeResult == -1)
		app_fatal(StrCat("SDL_RWsize failed: ", SDL_GetError()));
	const auto size = static_cast<size_t>(sizeResult);
	const uint32_t maybeNumFrames = ReadLE32(rwops);

	std::vector<uint8_t> cl2Data;

	// Most files become smaller with CL2. Allocate exactly enough bytes to avoid reallocation.
	// The only file that becomes larger is Data\hf_logo3.CEL, by exactly 4445 bytes.
	cl2Data.reserve(size + 4445);

	// If it is a number of frames, then the last frame offset will be equal to the size of the file.
	Seek(rwops, maybeNumFrames * 4 + 4, RW_SEEK_SET);
	if (ReadLE32(rwops) != size) {
		// maybeNumFrames is the address of the first group, right after
		// the list of group offsets.
		numGroups = maybeNumFrames / 4;
		groupsHeaderSize = maybeNumFrames;
		cl2Data.resize(groupsHeaderSize);
	}

	std::unique_ptr<uint32_t[]> offsets;
	std::unique_ptr<uint8_t[]> frameBuf;
	std::unique_ptr<uint32_t[]> groupNumFrames;
	std::unique_ptr<uint32_t[]> groupOffsets;

	Seek(rwops, 4, RW_SEEK_SET);
	if (numGroups == 1) {
		offsets.reset(new uint32_t[maybeNumFrames + 1]);
		ReadLE32s(rwops, maybeNumFrames + 1, offsets.get());
		frameBuf.reset(new uint8_t[GetMaxSizeFromOffsets(offsets.get(), maybeNumFrames + 1)]);
	} else {
		groupOffsets.reset(new uint32_t[numGroups]);
		groupNumFrames.reset(new uint32_t[numGroups]);

		groupOffsets[0] = maybeNumFrames;
		ReadLE32s(rwops, numGroups - 1, groupOffsets.get() + 1);

		uint32_t maxNumFrames = 0;
		for (size_t group = 0; group < numGroups; ++group) {
			Seek(rwops, groupOffsets[group], RW_SEEK_SET);
			groupNumFrames[group] = ReadLE32(rwops);
			if (groupNumFrames[group] > maxNumFrames)
				maxNumFrames = groupNumFrames[group];
		}
		offsets.reset(new uint32_t[maxNumFrames + 1]);

		uint32_t maxFrameSize = 0;
		for (size_t group = numGroups; group-- > 0;) {
			const uint32_t numFrames = groupNumFrames[group];
			Seek(rwops, groupOffsets[group] + 4, RW_SEEK_SET);
			ReadLE32s(rwops, numFrames + 1, offsets.get());
			maxFrameSize = std::max(maxFrameSize, GetMaxSizeFromOffsets(offsets.get(), numFrames + 1));
		}
		frameBuf.reset(new uint8_t[maxFrameSize]);
	}

	for (size_t group = 0; group < numGroups; ++group) {
		uint32_t numFrames;
		if (numGroups == 1) {
			numFrames = maybeNumFrames;
		} else {
			numFrames = groupNumFrames[group];
			if (group == 0) {
				// `offsets` already contains the offsets.
				Seek(rwops, groupOffsets[group] + 4 * (numFrames + 2), RW_SEEK_SET);
			} else {
				Seek(rwops, groupOffsets[group] + 4, RW_SEEK_SET);
				ReadLE32s(rwops, numFrames + 1, offsets.get());
			}
			WriteLE32(&cl2Data[4 * group], cl2Data.size());
		}

		// CLX header: frame count, frame offset for each frame, file size
		const size_t cl2DataOffset = cl2Data.size();
		cl2Data.resize(cl2Data.size() + 4 * (2 + static_cast<size_t>(numFrames)));
		WriteLE32(&cl2Data[cl2DataOffset], numFrames);

		for (size_t frame = 1; frame <= numFrames; ++frame) {
			const uint32_t frameSize = offsets[frame] - offsets[frame - 1];
			ReadUint8s(rwops, frameSize, frameBuf.get());
			const uint8_t *src = frameBuf.get();
			const uint8_t *srcEnd = src + frameSize;

			WriteLE32(&cl2Data[cl2DataOffset + 4 * frame], static_cast<uint32_t>(cl2Data.size() - cl2DataOffset));

			// Skip CEL frame header if there is one.
			constexpr size_t CelFrameHeaderSize = 10;
			const bool celFrameHasHeader = LoadLE16(src) == CelFrameHeaderSize;
			if (celFrameHasHeader)
				src += CelFrameHeaderSize;

			const unsigned frameWidth = widthOrWidths.HoldsPointer() ? widthOrWidths.AsPointer()[frame - 1] : widthOrWidths.AsValue();

			// CLX frame header.
			const size_t frameHeaderPos = cl2Data.size();
			constexpr size_t FrameHeaderSize = 10;
			cl2Data.resize(cl2Data.size() + FrameHeaderSize);
			WriteLE16(&cl2Data[frameHeaderPos], FrameHeaderSize);
			WriteLE16(&cl2Data[frameHeaderPos + 2], frameWidth);

			unsigned transparentRunWidth = 0;
			size_t frameHeight = 0;
			while (src != srcEnd) {
				// Process line:
				for (unsigned remainingCelWidth = frameWidth; remainingCelWidth != 0;) {
					uint8_t val = *src++;
					if (IsCelTransparent(val)) {
						val = GetCelTransparentWidth(val);
						transparentRunWidth += val;
					} else {
						AppendCl2TransparentRun(transparentRunWidth, cl2Data);
						transparentRunWidth = 0;
						AppendCl2PixelsOrFillRun(src, val, cl2Data);
						src += val;
					}
					remainingCelWidth -= val;
				}
				++frameHeight;
			}
			WriteLE16(&cl2Data[frameHeaderPos + 4], frameHeight);
			memset(&cl2Data[frameHeaderPos + 6], 0, 4);
			AppendCl2TransparentRun(transparentRunWidth, cl2Data);
		}

		WriteLE32(&cl2Data[cl2DataOffset + 4 * (1 + static_cast<size_t>(numFrames))], static_cast<uint32_t>(cl2Data.size() - cl2DataOffset));
	}

	auto out = std::unique_ptr<uint8_t[]>(new uint8_t[cl2Data.size()]);
	memcpy(&out[0], cl2Data.data(), cl2Data.size());
#ifdef DEBUG_CEL_TO_CL2_SIZE
	std::cout << "\t" << size << "\t" << cl2Data.size() << "\t" << std::setprecision(1) << std::fixed << (static_cast<int>(cl2Data.size()) - static_cast<int>(size)) / ((float)size) * 100 << "%" << std::endl;
#endif
	return OwnedClxSpriteListOrSheet { std::move(out), static_cast<uint16_t>(numGroups == 1 ? 0 : numGroups) };
}

} // namespace devilution
