#include "parser.hpp"

namespace devilution {
GetFieldResult HandleRecordSeparator(const char *begin, const char *end)
{
	if (begin == end) {
		return { end, GetFieldResult::Status::EndOfFile };
	}

	if (*begin == '\r') {
		++begin;
		if (begin == end) {
			return { end, GetFieldResult::Status::FileTruncated };
		}
		// carriage returns should be followed by a newline, so let's let the following checks handle it
	}
	if (*begin == '\n') {
		return { begin + 1, GetFieldResult::Status::EndOfRecord };
	}

	return { begin, GetFieldResult::Status::BadRecordSeparator };
}

GetFieldResult DiscardMultipleFields(const char *begin, const char *end, unsigned skipLength)
{
	GetFieldResult result { begin };
	while (skipLength > 0) {
		result = DiscardField(result.next, end);
		if (result.endOfRecord()) {
			// Found the end of record early, we can reuse the error code so just return it
			return result;
		}
		--skipLength;
	}
	return result;
}

GetFieldResult DiscardMultipleRecords(const char *begin, const char *end, unsigned skipLength)
{
	GetFieldResult result { begin };
	while (skipLength > 0) {
		result = DiscardRemainingFields(result.next, end);
		if (result.endOfFile()) {
			// Found the end of file early, we can reuse the error code so just return it
			return result;
		}
		--skipLength;
	}
	return result;
}
} // namespace devilution
