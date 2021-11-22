#include "utils/utf8.hpp"

#include <cstddef>
#include <cstring>

#include <hoehrmann_utf8.h>

namespace devilution {

namespace {

/** Truncates `str` to at most `len` at a code point boundary. */
string_view TruncateUtf8(string_view str, std::size_t len)
{
	if (str.size() > len) {
		str.remove_suffix(str.size() - len);
		while (!str.empty() && !IsTrailUtf8CodeUnit(str.back()))
			str.remove_suffix(1);
	}
	return str;
}

} // namespace

char32_t DecodeFirstUtf8CodePoint(string_view input, uint8_t *len)
{
	uint32_t codepoint = 0;
	uint32_t state = UTF8_ACCEPT;
	for (std::size_t i = 0; i < input.size(); ++i) {
		state = utf8_decode_step(state, static_cast<uint8_t>(input[i]), &codepoint);
		if (state == UTF8_ACCEPT) {
			*len = i + 1;
			return codepoint;
		}
		if (state == UTF8_REJECT) {
			*len = i + 1;
			return Utf8DecodeError;
		}
	}
	*len = input.size();
	return Utf8DecodeError;
}

void CopyUtf8(char *dest, string_view source, std::size_t bytes)
{
	source = TruncateUtf8(source, bytes - 1);
	std::memcpy(dest, source.data(), source.size());
	dest[source.size()] = '\0';
}

} // namespace devilution
