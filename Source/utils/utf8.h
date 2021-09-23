#pragma once

#include <cstdint>
#include <string>
#include <utility>

/* Branchless UTF-8 decoder
 *
 * This is free and unencumbered software released into the public domain.
 */

/* Decode the next character, C, from BUF, reporting errors in E.
 *
 * Since this is a branchless decoder, four bytes will be read from the
 * buffer regardless of the actual length of the next character. This
 * means the buffer _must_ have at least three bytes of zero padding
 * following the end of the data stream.
 *
 * Errors are reported in E, which will be non-zero if the parsed
 * character was somehow invalid: invalid byte sequence, non-canonical
 * encoding, or a surrogate half.
 *
 * The function returns a pointer to the next character. When an error
 * occurs, this pointer will be a guess that depends on the particular
 * error, but it will always advance at least one byte.
 */
inline const char *utf8_decode(const char *buf, uint32_t *c, int *e)
{
	static const char lengths[] = {
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 4, 0
	};
	static const int masks[] = { 0x00, 0x7f, 0x1f, 0x0f, 0x07 };
	static const uint32_t mins[] = { 4194304, 0, 128, 2048, 65536 };
	static const int shiftc[] = { 0, 18, 12, 6, 0 };
	static const int shifte[] = { 0, 6, 4, 2, 0 };

	auto s = reinterpret_cast<const unsigned char *>(buf);
	int len = lengths[s[0] >> 3];

	/* Compute the pointer to the next character early so that the next
     * iteration can start working on the next character. Neither Clang
     * nor GCC figure out this reordering on their own.
     */
	const unsigned char *next = s + len + !len;

	/* Assume a four-byte character and load four bytes. Unused bits are
     * shifted out.
     */
	*c = (uint32_t)(s[0] & masks[len]) << 18;
	*c |= (uint32_t)(s[1] & 0x3f) << 12;
	*c |= (uint32_t)(s[2] & 0x3f) << 6;
	*c |= (uint32_t)(s[3] & 0x3f) << 0;
	*c >>= shiftc[len];

	/* Accumulate the various error conditions. */
	*e = (*c < mins[len]) << 6;      // non-canonical encoding
	*e |= ((*c >> 11) == 0x1b) << 7; // surrogate half?
	*e |= (*c > 0x10FFFF) << 8;      // out of range?
	*e |= (s[1] & 0xc0) >> 2;
	*e |= (s[2] & 0xc0) >> 4;
	*e |= (s[3]) >> 6;
	*e ^= 0x2a; // top two bits of each tail byte correct?
	*e >>= shifte[len];

	return reinterpret_cast<const char *>(next);
}

inline int FindLastUtf8Symbols(const char *text)
{
	std::string textBuffer(text);
	textBuffer.resize(textBuffer.size() + 4); // Buffer must be padded before calling utf8_decode()
	const char *textData = textBuffer.data();
	const char *previousPosition = textData;

	uint32_t next;
	int error;
	for (; *textData != '\0'; previousPosition = textData) {
		textData = utf8_decode(textData, &next, &error);
		if (*textData == '\0')
			return previousPosition - textBuffer.data();
	}

	return 0;
}
