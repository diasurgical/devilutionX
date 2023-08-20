#pragma once

#include <algorithm>
#include <string_view>

#include "engine.h" // For IsAnyOf

namespace devilution {

struct GetFieldResult {
	std::string_view value;

	const char *next = nullptr;

	enum class Status {
		ReadyToRead,
		EndOfField,
		EndOfRecord,
		BadRecordSeparator,
		EndOfFile,
		FileTruncated
	} status
	    = Status::ReadyToRead;

	GetFieldResult() = default;

	GetFieldResult(const char *next)
	    : value()
	    , next(next)
	    , status(Status::ReadyToRead)
	{
	}

	GetFieldResult(const char *next, const Status &status)
	    : value()
	    , next(next)
	    , status(status)
	{
	}

	/**
	 * @brief Recreates a GetFieldResult with a new value
	 */
	GetFieldResult(std::string_view value, const GetFieldResult &result)
	    : value(value)
	    , next(result.next)
	    , status(result.status)
	{
	}

	/**
	 * @brief Returns true if the last read reached the end of the current record
	 */
	[[nodiscard]] bool endOfRecord() const
	{
		return IsAnyOf(status, Status::EndOfRecord, Status::BadRecordSeparator) || endOfFile();
	}

	/**
	 * @brief Returns true if the last read reached the end of the file/stream
	 */
	[[nodiscard]] bool endOfFile() const
	{
		return IsAnyOf(status, Status::EndOfFile, Status::FileTruncated);
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
 * @brief Consumes the current record separator sequence and returns a result describing whether at least one more record is available.
 *
 * Assumes that begin points to a record separator (cr, lf, or EOF). If we reached the end of the
 * stream (endOfField == end) then `status` will compare equal to
 * GetFieldResult::Status::EndOfFile. Otherwise if a valid record separator sequence was found (lf
 * or crlf) then the `status` equals GetFieldResult::Status::EndOfRecord. If we found a carriage
 * return (cr) character just before the end of the stream then it's likely the file was
 * truncated, `status` will contain GetFieldResult::Status::FileTruncated. If we found a carriage
 * return that is followed by any character other than a newline (lf), or `begin` didn't point to
 * a record separator, then `status` will be GetFieldResult::Status::BadRecordSeparator and the
 * file has probably been mangled.
 * @param begin start of a stream (expected to be pointing to a record separator)
 * @param end one past the last character of the stream
 * @return a struct containing a pointer to the start of the next record (if more characters are
 *          available) or a copy of end, and optionally a status code describing what type of
 *          separator was found.
 */
GetFieldResult HandleRecordSeparator(const char *begin, const char *end);

/**
 * @brief Consumes the current field (or record) separator and returns a result describing whether at least one more field is available.
 *
 * Assumes that begin points to a field or record separator (tab, cr, lf, or EOF). If there are
 * more fields in the current record then the return value will have a pointer to the start of the
 * next field and `status` will be GetFieldResult::Status::EndOfField. Otherwise refer to
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
		return { begin + 1, GetFieldResult::Status::EndOfField };
	}

	return HandleRecordSeparator(begin, end);
}

/**
 * @brief Advances to the next field separator without saving any characters
 * @param begin first character of the stream
 * @param end one past the last character in the stream
 * @return a GetFieldResult struct containing an empty value, a pointer to the start of the next
 *          field/record, and a status code describing what type of separator was found
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
 * @param skipLength how many fields to skip (specifying 0 will cause the method to return without advancing)
 * @return a GetFieldResult struct containing an empty value, a pointer to the start of the next
 *          field/record, and a status code describing what type of separator was found
 */
GetFieldResult DiscardMultipleFields(const char *begin, const char *end, unsigned skipLength);

/**
 * @brief Advances by the specified number of records or until the end of the file, whichever occurs first
 * @param begin first character of the stream
 * @param end one past the last character in the stream
 * @param skipLength how many records to skip (specifying 0 will cause the method to return without advancing)
 * @return a GetFieldResult struct containing an empty value, a pointer to the start of the next
 *          record, and a status code describing what type of separator was found
 */
GetFieldResult DiscardMultipleRecords(const char *begin, const char *end, unsigned skipLength);

/**
 * @brief Discard any remaining fields in the current record
 * @param begin pointer to the current character in the stream
 * @param end one past the last character in the stream
 * @return a GetFieldResult struct containing an empty value, the start of the next record (or
 *          `end`), and a status describing whether more records are available
 */
inline GetFieldResult DiscardRemainingFields(const char *begin, const char *end)
{
	const char *nextSeparator = std::find_if(begin, end, IsRecordSeparator);

	return HandleRecordSeparator(nextSeparator, end);
}

/**
 * @brief Returns a view of the next field from a tab-delimited stream.
 *
 * Note that the result *always* contains a value after calling this function as a zero-length
 * field is a valid value. This function consumes the field separator whenever possible, the
 * `next` member of the returned type will be either the start of the next field/record or `end`.
 * The `status` member contains additional information to distinguish between the end of a field
 * and the end of a record. If there are additional fields in this record then `status` will be
 * GetFieldResult::Status::EndOfField, otherwise refer to HandleRecordSeparator for the meanings
 * associated with the remaining codes.
 * @param begin first character of the stream
 * @param end one past the last character in the stream
 * @return a GetFieldResult struct containing a string_view of the field, the start of the next
 *          field/record, and a status code describing what type of separator was found
 */
inline GetFieldResult GetNextField(const char *begin, const char *end)
{
	const char *nextSeparator = std::find_if(begin, end, IsFieldSeparator);

	// Can't use the string_view(It, It) constructor since that was only added in C++20...
	return { { begin, static_cast<size_t>(nextSeparator - begin) }, HandleFieldSeparator(nextSeparator, end) };
}
} // namespace devilution
