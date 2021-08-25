#include "utils/sdl_bilinear_scale.hpp"

#include <cstdint>
#include <memory>

// Performs bilinear scaling using fixed-width integer math.

namespace devilution {

namespace {

unsigned Frac(unsigned fixedPoint)
{
	return fixedPoint & 0xffff;
}

unsigned ToInt(unsigned fixedPoint)
{
	return fixedPoint >> 16;
}

std::unique_ptr<unsigned[]> CreateMixFactors(unsigned srcSize, unsigned dstSize)
{
	std::unique_ptr<unsigned[]> result { new unsigned[dstSize + 1] };
	const auto scale = static_cast<unsigned>(65536.0 * static_cast<float>(srcSize - 1) / dstSize);
	unsigned mix = 0;
	for (unsigned i = 0; i <= dstSize; ++i) {
		result[i] = mix;
		mix = Frac(mix) + scale;
	}
	return result;
};

std::uint8_t MixColors(std::uint8_t first, std::uint8_t second, unsigned ratio)
{
	return ToInt((second - first) * ratio) + first;
}

} // namespace

void BilinearScale32(SDL_Surface *src, SDL_Surface *dst)
{
	const std::unique_ptr<unsigned[]> mixXs = CreateMixFactors(src->w, dst->w);
	const std::unique_ptr<unsigned[]> mixYs = CreateMixFactors(src->h, dst->h);

	const unsigned dgap = dst->pitch - dst->w * 4;

	auto *srcPixels = static_cast<std::uint8_t *>(src->pixels);
	auto *dstPixels = static_cast<std::uint8_t *>(dst->pixels);

	unsigned *curMixY = &mixYs[0];
	unsigned srcY = 0;
	for (unsigned y = 0; y < static_cast<unsigned>(dst->h); ++y) {
		std::uint8_t *s[4] = {
			srcPixels,                 // Self
			srcPixels + 4,             // Right
			srcPixels + src->pitch,    // Bottom
			srcPixels + src->pitch + 4 // Bottom right
		};

		unsigned *curMixX = &mixXs[0];
		unsigned srcX = 0;
		for (unsigned x = 0; x < static_cast<unsigned>(dst->w); ++x) {
			const unsigned mixX = Frac(*curMixX);
			const unsigned mixY = Frac(*curMixY);
			for (unsigned channel = 0; channel < 4; ++channel) {
				dstPixels[channel] = MixColors(
				    MixColors(s[0][channel], s[1][channel], mixX),
				    MixColors(s[2][channel], s[3][channel], mixX),
				    mixY);
			}

			++curMixX;
			if (*curMixX > 0) {
				unsigned step = ToInt(*curMixX);
				srcX += step;
				if (srcX <= static_cast<unsigned>(src->w)) {
					step *= 4;
					for (auto &v : s) {
						v += step;
					}
				}
			}

			dstPixels += 4;
		}

		++curMixY;
		if (*curMixY > 0) {
			const unsigned step = ToInt(*curMixY);
			srcY += step;
			if (srcY < static_cast<unsigned>(src->h)) {
				srcPixels += step * src->pitch;
			}
		}

		dstPixels += dgap;
	}
}

} // namespace devilution
