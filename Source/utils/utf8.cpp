#include "utils/utf8.hpp"

#include <cstddef>

#include <hoehrmann_utf8.h>

namespace devilution {

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

} // namespace devilution
