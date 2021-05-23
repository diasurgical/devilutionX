#include "DiabloUI/text.h"

namespace devilution {

std::size_t GetArtStrWidth(const char *str, std::size_t size)
{
	int strWidth = 0;

	for (size_t i = 0, n = strlen(str); i < n; i++) {
		uint8_t w = FontTables[size][(uint8_t)str[i] + 2];
		strWidth += (w != 0) ? w : FontTables[size][0];
	}

	return strWidth;
}

void WordWrapArtStr(char *text, std::size_t width, std::size_t size)
{
	const std::size_t textLength = strlen(text);
	std::size_t lineStart = 0;
	std::size_t lineWidth = 0;
	for (std::size_t i = 0; i < textLength; i++) {
		if (text[i] == '\n') { // Existing line break, scan next line
			lineStart = i + 1;
			lineWidth = 0;
			continue;
		}

		uint8_t w = FontTables[size][(uint8_t)text[i] + 2];
		lineWidth += (w != 0) ? w : FontTables[size][0];

		if (lineWidth <= width) {
			continue; // String is still within the limit, continue to the next line
		}

		std::size_t j; // Backtrack to the previous space
		for (j = i; j >= lineStart; j--) {
			if (text[j] == ' ') {
				break;
			}
		}

		if (j == lineStart) { // Single word longer than width
			if (i == textLength)
				break;
			j = i;
		}

		// Break line and continue to next line
		i = j;
		text[i] = '\n';
		lineStart = i + 1;
		lineWidth = 0;
	}
}

} // namespace devilution
