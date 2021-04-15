#include "DiabloUI/text.h"

namespace devilution {

std::size_t GetArtStrWidth(const char *str, std::size_t size)
{
	int strWidth = 0;

	for (size_t i = 0, n = strlen(str); i < n; i++) {
		BYTE w = FontTables[size][*(BYTE *)&str[i] + 2];
		if (w)
			strWidth += w;
		else
			strWidth += FontTables[size][0];
	}

	return strWidth;
}

void WordWrapArtStr(char *text, std::size_t width)
{
	const std::size_t len = strlen(text);
	std::size_t lineStart = 0;
	for (std::size_t i = 0; i <= len; i++) {
		if (text[i] == '\n') {
			lineStart = i + 1;
			continue;
		} else if (text[i] != ' ' && i != len) {
			continue;
		}

		if (i != len)
			text[i] = '\0';
		if (GetArtStrWidth(&text[lineStart], AFT_SMALL) <= width) {
			if (i != len)
				text[i] = ' ';
			continue;
		}

		std::size_t j;
		for (j = i; j >= lineStart; j--) {
			if (text[j] == ' ') {
				break; // Scan for previous space
			}
		}

		if (j == lineStart) { // Single word longer then width
			if (i == len)
				break;
			j = i;
		}

		if (i != len)
			text[i] = ' ';
		text[j] = '\n';
		lineStart = j + 1;
	}
}

} // namespace devilution
