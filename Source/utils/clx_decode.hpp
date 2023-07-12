#pragma once

#include "engine/render/blit_impl.hpp"

namespace devilution {

[[nodiscard]] constexpr bool IsClxOpaque(uint8_t control)
{
	constexpr uint8_t ClxOpaqueMin = 0x80;
	return control >= ClxOpaqueMin;
}

[[nodiscard]] constexpr uint8_t GetClxOpaquePixelsWidth(uint8_t control)
{
	return -static_cast<std::int8_t>(control);
}

[[nodiscard]] constexpr bool IsClxOpaqueFill(uint8_t control)
{
	constexpr uint8_t ClxFillMax = 0xBE;
	return control <= ClxFillMax;
}

[[nodiscard]] constexpr uint8_t GetClxOpaqueFillWidth(uint8_t control)
{
	constexpr uint8_t ClxFillEnd = 0xBF;
	return static_cast<int_fast16_t>(ClxFillEnd - control);
}

[[nodiscard]] constexpr BlitCommand ClxGetBlitCommand(const uint8_t *src)
{
	const uint8_t control = *src++;
	if (!IsClxOpaque(control))
		return BlitCommand { BlitType::Transparent, src, control, 0 };
	if (IsClxOpaqueFill(control)) {
		const uint8_t width = GetClxOpaqueFillWidth(control);
		const uint8_t color = *src++;
		return BlitCommand { BlitType::Fill, src, width, color };
	}
	const uint8_t width = GetClxOpaquePixelsWidth(control);
	return BlitCommand { BlitType::Pixels, src + width, width, 0 };
}

} // namespace devilution
