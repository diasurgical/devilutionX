#include "utils/utf8.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>

#include <hoehrmann_utf8.h>

namespace devilution {

std::u32string ConvertUtf8ToUtf32(std::string_view input)
{
    std::u32string result;
    std::size_t len = 0;

    while (!input.empty()) {
        char32_t codepoint = DecodeFirstUtf8CodePoint(input, &len);
        if (codepoint == Utf8DecodeError) {
            break;
        }
        result.push_back(codepoint);
        input.remove_prefix(len);
    }

    return result;
}

std::string ConvertUtf32ToUtf8(std::u32string_view input)
{
    std::string result;
    for (char32_t codepoint : input) {
        AppendUtf8(codepoint, result);
    }
    return result;
}

char32_t DecodeFirstUtf8CodePoint(std::string_view input, std::size_t *len)
{
	uint32_t codepoint = 0;
	uint8_t state = UTF8_ACCEPT;
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

std::string_view TruncateUtf8(std::string_view str, std::size_t len)
{
	if (str.size() > len) {
		std::size_t truncIndex = len;
		while (truncIndex > 0 && IsTrailUtf8CodeUnit(str[truncIndex]))
			truncIndex--;
		str.remove_suffix(str.size() - truncIndex);
	}
	return str;
}

void CopyUtf8(char *dest, std::string_view source, std::size_t bytes)
{
	source = TruncateUtf8(source, bytes - 1);
	std::memcpy(dest, source.data(), source.size());
	dest[source.size()] = '\0';
}

void AppendUtf8(char32_t codepoint, std::string &out)
{
	if (codepoint <= 0x7F) {
		out += static_cast<char>(codepoint);
		return;
	}

	char buf[4];
	if (codepoint <= 0x7FF) {
		buf[0] = static_cast<char>(0xC0 | (codepoint >> 6 & 0x3F));
		buf[1] = static_cast<char>(0x80 | (codepoint & 0x3F));
		out.append(buf, 2);
	} else if (codepoint <= 0xFFFF) {
		buf[0] = static_cast<char>(0xE0 | (codepoint >> 12 & 0x3F));
		buf[1] = static_cast<char>(0x80 | (codepoint >> 6 & 0x3F));
		buf[2] = static_cast<char>(0x80 | (codepoint & 0x3F));
		out.append(buf, 3);
	} else {
		buf[0] = static_cast<char>(0xF0 | (codepoint >> 18 & 0x3F));
		buf[1] = static_cast<char>(0x80 | (codepoint >> 12 & 0x3F));
		buf[2] = static_cast<char>(0x80 | (codepoint >> 6 & 0x3F));
		buf[3] = static_cast<char>(0x80 | (codepoint & 0x3F));
		out.append(buf, 4);
	}
}

} // namespace devilution
