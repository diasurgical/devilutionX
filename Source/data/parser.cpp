#include "parser.hpp"

namespace devilution {
GetFieldResult HandleRecordTerminator(const char *begin, const char *end)
{
	if (begin == end) {
		return { end, GetFieldResult::Status::NoFinalTerminator };
	}

	if (*begin == '\r') {
		++begin;
		if (begin == end) {
			return { end, GetFieldResult::Status::FileTruncated };
		}
		// carriage returns should be followed by a newline, so let's let the following checks handle it
	}
	if (*begin == '\n') {
		++begin;
		if (begin == end) {
			return { end, GetFieldResult::Status::EndOfFile };
		}

		return { begin, GetFieldResult::Status::EndOfRecord };
	}

	return { begin, GetFieldResult::Status::BadRecordTerminator };
}

GetFieldResult DiscardMultipleFields(const char *begin, const char *end, unsigned skipLength, unsigned *fieldsSkipped)
{
	GetFieldResult result { begin };
	unsigned skipCount = 0;
	while (skipCount < skipLength) {
		++skipCount;
		result = DiscardField(result.next, end);
		if (result.endOfRecord()) {
			// Found the end of record early
			break;
		}
	}
	if (fieldsSkipped != nullptr) {
		*fieldsSkipped = skipCount;
	}
	return result;
}

GetFieldResult DiscardMultipleRecords(const char *begin, const char *end, unsigned skipLength, unsigned *recordsSkipped)
{
	GetFieldResult result { begin };
	unsigned skipCount = 0;
	while (skipCount < skipLength) {
		++skipCount;
		result = DiscardRemainingFields(result.next, end);
		if (result.endOfFile()) {
			// Found the end of file early
			break;
		}
	}
	if (recordsSkipped != nullptr) {
		*recordsSkipped = skipCount;
	}
	return result;
}
} // namespace devilution
