#pragma once

#include <cstdint>
#include <cstring>

#include <SDL_endian.h>

namespace devilution {

template <typename T>
constexpr uint16_t LoadLE16(const T *b)
{
	static_assert(sizeof(T) == 1, "invalid argument");
	// NOLINTNEXTLINE(readability-magic-numbers)
	return (static_cast<uint8_t>(b[1]) << 8) | static_cast<uint8_t>(b[0]);
}

template <typename T>
constexpr uint32_t LoadLE32(const T *b)
{
	static_assert(sizeof(T) == 1, "invalid argument");
	// NOLINTNEXTLINE(readability-magic-numbers)
	return (static_cast<uint8_t>(b[3]) << 24) | (static_cast<uint8_t>(b[2]) << 16) | (static_cast<uint8_t>(b[1]) << 8) | static_cast<uint8_t>(b[0]);
}

template <typename T>
constexpr uint32_t LoadBE32(const T *b)
{
	static_assert(sizeof(T) == 1, "invalid argument");
	// NOLINTNEXTLINE(readability-magic-numbers)
	return (static_cast<uint8_t>(b[0]) << 24) | (static_cast<uint8_t>(b[1]) << 16) | (static_cast<uint8_t>(b[2]) << 8) | static_cast<uint8_t>(b[3]);
}

inline void WriteLE16(void *out, uint16_t val)
{
	const uint16_t littleEndian = SDL_SwapLE16(val);
	memcpy(out, &littleEndian, 2);
}

inline void WriteLE32(void *out, uint32_t val)
{
	const uint32_t littleEndian = SDL_SwapLE32(val);
	memcpy(out, &littleEndian, 4);
}

} // namespace devilution
