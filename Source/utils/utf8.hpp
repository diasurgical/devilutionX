#pragma once

#include <cstdint>
#include <string>
#include <utility>

#include "utils/stdcompat/string_view.hpp"

namespace devilution {

constexpr char32_t Utf8DecodeError = 0xD83F;

/**
 * Decodes the first code point from UTF8-encoded input.
 *
 * Sets `len` to the length of the code point in bytes.
 * Returns `Utf8DecodeError` on error.
 */
char32_t DecodeFirstUtf8CodePoint(string_view input, uint8_t *len);

/**
 * Decodes and removes the first code point from UTF8-encoded input.
 */
inline char32_t ConsumeFirstUtf8CodePoint(string_view *input)
{
	uint8_t len;
	const char32_t result = DecodeFirstUtf8CodePoint(*input, &len);
	input->remove_prefix(len);
	return result;
}

/**
 * Returns true if this is a trailing byte in a UTF-8 code point encoding.
 *
 * A trailing byte is any byte that is not the heading byte.
 */
inline bool IsTrailUtf8CodeUnit(char x)
{
	return static_cast<signed char>(x) < -0x40;
}

/**
 * Returns the start byte index of the last code point in a UTF-8 string.
 */
inline std::size_t FindLastUtf8Symbols(string_view input)
{
	if (input.empty())
		return 0;

	std::size_t pos = input.size() - 1;
	while (pos > 0 && IsTrailUtf8CodeUnit(input[pos]))
		--pos;
	return pos;
}

/**
 * @brief Copy up to a given number of bytes from a UTF8 string, and zero terminate string
 * @param bytes Max number of bytes to copy
 */
void CopyUtf8(char *dest, string_view source, std::size_t bytes);

} // namespace devilution
