#pragma once

#include <memory>

#include "engine.h"
#include "utils/stdcompat/algorithm.hpp"
#include "utils/stdcompat/cstddef.hpp"
#include "utils/stdcompat/string_view.hpp"

namespace devilution {
enum class DataFileError {
	NotFound = 1,
	ReadError
};

struct LoadDataResult {
	std::unique_ptr<char[]> start;
	size_t size;
	DataFileError ec;

	LoadDataResult() = delete;

	LoadDataResult(std::unique_ptr<char[]> &&start, size_t size)
	    : start(std::move(start))
	    , size(size)
	    , ec()
	{
	}

	LoadDataResult(DataFileError ec)
	    : start(nullptr)
	    , size(0)
	    , ec(ec)
	{
	}

	[[nodiscard]] bool hasError()
	{
		return ec != DataFileError();
	}

	[[nodiscard]] const char *begin()
	{
		if (start != nullptr)
			return start.get();
		return nullptr;
	}

	[[nodiscard]] const char *end()
	{
		if (start != nullptr)
			return start.get() + size;
		return nullptr;
	}
};

/**
 * @brief Attempts to load a data file (using the same mechanism as other runtime assets)
 *
 * On success `ec` will be value initialised and `begin()`/`end()` can be used to stream through the file.
 * @param path file to load including the /Data/ prefix
 * @return a result containing an owned pointer to an in-memory copy of the file, or with `ec` set
 *          to an error code describing the reason for failure.
 */
LoadDataResult LoadDataFile(string_view path);

enum class DataFieldError {
	EndOfRecord = 1,
	EndOfFile,
	FileTruncated,
	BadRecordSeparator
};

struct GetFieldResult {
	const char *ptr = nullptr;
	DataFieldError ec = DataFieldError();

	GetFieldResult() = default;

	GetFieldResult(const char *ptr)
	    : ptr(ptr)
	    , ec()
	{
	}

	GetFieldResult(const char *ptr, const DataFieldError &ec)
	    : ptr(ptr)
	    , ec(ec)
	{
	}

	[[nodiscard]] bool ok()
	{
		return ec == DataFieldError();
	}

	/**
	 * @brief Returns true if the last read reached the end of the file/stream
	 */
	[[nodiscard]] bool eof()
	{
		return IsAnyOf(ec, DataFieldError::EndOfFile, DataFieldError::FileTruncated);
	}
};

/**
 * @brief Checks if this character is potentially part of a record separator sequence
 * @param c character to check
 * @return true if it's a record separator (lf) or carriage return (cr, accepted as the start of a crlf pair)
 */
constexpr bool IsRecordSeparator(char c)
{
	return c == '\r' || c == '\n';
}

/**
 * @brief Checks if this character is a field separator
 *
 * Note that record separator sequences also act to delimit fields
 * @param c character to check
 * @return true if it's a field separator (tab) or part of a record separator sequence
 */
constexpr bool IsFieldSeparator(char c)
{
	return c == '\t' || IsRecordSeparator(c);
}

/**
 * @brief Assumes that begin points to a record separator (cr, lf, or EOF) and returns a result describing whether at least one more record is available.
 *
 * If we reached the end of the stream (endOfField == end) then `ec` will compare equal to
 * DataFieldError::EndOfFile. Otherwise if a valid record separator sequence was found (lf or
 * crlf) then the `ec` equals DataFieldError::EndOfRecord. If we found a carriage return (cr)
 * character just before the end of the stream then it's likely the file was truncated, `ec` will
 * contain DataFieldError::FileTruncated. If we found a carriage return that is followed by any
 * character other than a newline (lf), or `begin` didn't point to a record separator, then `ec`
 * will be DataFieldError::BadRecordSeparator and the file has probably been mangled.
 * @param begin start of a stream (expected to be pointing to a record separator)
 * @param end one past the last character of the stream
 * @return a struct containing a pointer to the start of the next record (if more characters are
 *          available) or a copy of end, and optionally an error code describing what type of
 *          separator was found.
 */
GetFieldResult HandleRecordSeparator(const char *begin, const char *end);

/**
 * @brief Assumes that begin points to a field or record separator (tab, cr, lf, or EOF) and returns a result describing whether at least one more field is available.
 *
 * If there are more fields in the current record then the return value will have a pointer to the
 * start of the next record and `ec` will be value initialised. Otherwise refer to
 * HandleRecordSeparator for a description of the different codes.
 * @param begin start of a stream (expected to be pointing to a record separator)
 * @param end one past the last character of the stream
 * @return a struct containing a pointer to the start of the next field (if more characters are
 *          available) or a copy of end, and optionally an error code describing what type of
 *          separator was found.
 */
inline GetFieldResult HandleFieldSeparator(const char *begin, const char *end)
{
	if (begin != end && *begin == '\t') {
		return { begin + 1 };
	}

	return HandleRecordSeparator(begin, end);
}

/**
 * @brief Advances to the next field separator without saving any characters
 * @param begin first character of the stream
 * @param end one past the last character in the stream
 * @return a GetFieldResult struct containing the start of the next field/record and an error code if the last field in the record was found
 */
inline GetFieldResult DiscardField(const char *begin, const char *end)
{
	const char *nextSeparator = std::find_if(begin, end, IsFieldSeparator);

	return HandleFieldSeparator(nextSeparator, end);
}

/**
 * @brief Advances by the specified number of fields or until the end of the record, whichever occurs first
 * @param begin first character of the stream
 * @param end one past the last character in the stream
 * @param skipLength how many records to skip (specifying 0 will cause the method to return without advancing)
 * @return a GetFieldResult struct containing the start of the next field/record and an error code if the last field in the record was found
 */
GetFieldResult DiscardMultipleFields(const char *begin, const char *end, unsigned skipLength);

/**
 * @brief Reads a sequence of character bytes into the provided buffer from a tab-delimited stream.
 *
 * Note that buffer is *always* modified by this function as a zero-length field is a valid value.
 * This function consumes the field separator whenever possible, the `ptr` member of the returned
 * type will be either the start of the next field/record or `end`. The `ec` member contains
 * additional information to distinguish between the end of a field and the end of a record. If
 * there are additional fields in this record then `ec` will be value initialised
 * (`ec` == DataFieldError()), otherwise refer to HandleRecordSeparator for the meanings associated
 * with different codes.
 * @param begin first character of the stream
 * @param end one past the last character in the stream
 * @param buffer output for the field contents, will be filled with a copy of the characters between [begin, fieldEnd)
 * @return a GetFieldResult struct containing the start of the next field/record and an error code if the last field in the record was found
 */
inline GetFieldResult GetNextField(const char *begin, const char *end, std::string &buffer)
{
	const char *nextSeparator = std::find_if(begin, end, IsFieldSeparator);

	buffer.assign(begin, nextSeparator);

	return HandleFieldSeparator(nextSeparator, end);
}

/**
 * @brief Discard any remaining fields in the current record
 * @param begin pointer to the current character in the stream
 * @param end one past the last character in the stream
 * @return a GetFieldResult struct containing the start of the next record and optionally an error
 *          code if the end of the file or a bad record separator sequence was found
 */
inline GetFieldResult DiscardRemainingFields(const char *begin, const char *end)
{
	const char *nextSeparator = std::find_if(begin, end, IsRecordSeparator);

	return HandleRecordSeparator(nextSeparator, end);
}
} // namespace devilution
