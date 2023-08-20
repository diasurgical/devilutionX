#pragma once

#include <charconv>

#include "parser.hpp"

namespace devilution {
class FieldsInRecordRange {
	GetFieldResult *state_;
	const char *const end_;

public:
	FieldsInRecordRange(GetFieldResult *state, const char *end)
	    : state_(state)
	    , end_(end)
	{
	}

	class Iterator {
		GetFieldResult *state_;
		const char *const end_;

	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = std::string_view;

		Iterator()
		    : state_(nullptr)
		    , end_(nullptr)
		{
		}

		Iterator(GetFieldResult *state, const char *end)
		    : state_(state)
		    , end_(end)
		{
			state_->status = GetFieldResult::Status::ReadyToRead;
		}

		[[nodiscard]] bool operator==(const Iterator &rhs) const
		{
			if (state_ == nullptr && rhs.state_ == nullptr)
				return true;

			return state_ != nullptr && rhs.state_ != nullptr && state_->next == rhs.state_->next;
		}

		[[nodiscard]] bool operator!=(const Iterator &rhs) const
		{
			return !(*this == rhs);
		}

		/**
		 * Advances to the next field in the current record
		 */
		Iterator &operator++()
		{
			return *this += 1;
		}

		/**
		 * @brief Advances by the specified number of fields
		 *
		 * if a non-zero increment is provided and advancing the iterator causes it to reach the end
		 * of the record the iterator is invalidated. It will compare equal to an end iterator and
		 * cannot be used for value access or any further parsing
		 * @param increment how many fields to advance (can be 0)
		 * @return self-reference
		 */
		Iterator &operator+=(unsigned increment)
		{
			if (state_->status == GetFieldResult::Status::ReadyToRead)
				*state_ = DiscardMultipleFields(state_->next, end_, increment);

			if (state_->endOfRecord()) {
				state_ = nullptr;
			} else {
				// We use Status::ReadyToRead as a marker so we only read the next value on the next call to operator*
				// this allows consumers to read from the input stream without using operator* if they want
				state_->status = GetFieldResult::Status::ReadyToRead;
			}
			return *this;
		}

		/**
		 * @brief Returns a view of the current field
		 *
		 * This method scans the current field if this is the first value access since the last
		 * advance. You can repeatedly call operator* safely but you must not try to use parseInt
		 * after calling this method. If you calling parseInt and get an invalid_argument result
		 * back you can use this method to get the actual field value, but if you received any
		 * other result then parseInt has consumed the field and this method is no longer reliable.
		 * @return The current field value (may be an empty string) or a zero length string_view
		 */
		[[nodiscard]] value_type operator*()
		{
			if (state_->status == GetFieldResult::Status::ReadyToRead) {
				*state_ = GetNextField(state_->next, end_);
			}
			return state_->value;
		}

		/**
		 * @brief Attempts to parse a field as a numeric value using std::from_chars
		 *
		 * This method is NOT repeatable. In addition to the behaviour of std::from_chars please
		 * heed the following warnings. If the field contains a value larger than will fit into the
		 * given type parsing will fail with std::errc::out_of_range and the field will be
		 * discarded. If a field starts with some digits then is followed by other characters the
		 * remainder will also be discarded. You can only get a useful value out of operator* if
		 * the result code is std::errc::invalid_argument.
		 * @tparam T an Integral type supported by std::from_chars
		 * @param destination value to store the result of successful parsing
		 * @return the error code from std::from_chars
		 */
		template <typename T>
		[[nodiscard]] std::errc parseInt(T &destination)
		{
			auto result = std::from_chars(state_->next, end_, destination);
			if (result.ec != std::errc::invalid_argument) {
				// from_chars was able to consume at least one character, so discard the rest of the field
				*state_ = DiscardField(result.ptr, end_);
			}
			return result.ec;
		}

		/**
		 * @brief Checks if this field is the last field in a file, not just in the record
		 *
		 * If you call this method before calling parseInt or operator* then this will trigger a
		 * read, meaning you can no longer use parseInt to try extract a numeric value. You
		 * probably want to use this after calling parseInt.
		 * @return true if this field is the last field in the file/RecordsRange
		 */
		[[nodiscard]] bool endOfFile() const
		{
			if (state_->status == GetFieldResult::Status::ReadyToRead) {
				*state_ = GetNextField(state_->next, end_);
			}
			return state_->endOfFile();
		}
	};

	[[nodiscard]] Iterator begin() const
	{
		return { state_, end_ };
	}

	[[nodiscard]] Iterator end() const
	{
		return {};
	}
};

class RecordsRange {
	const char *const begin_;
	const char *const end_;

public:
	RecordsRange(std::string_view characters)
	    : begin_(characters.data())
	    , end_(characters.data() + characters.size())
	{
	}

	class Iterator {
		GetFieldResult state_;
		const char *const end_;

	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = FieldsInRecordRange;

		Iterator()
		    : state_(nullptr, GetFieldResult::Status::EndOfFile)
		    , end_(nullptr)
		{
		}

		Iterator(const char *begin, const char *end)
		    : state_(begin)
		    , end_(end)
		{
		}

		[[nodiscard]] bool operator==(const Iterator &rhs) const
		{
			return state_.next == rhs.state_.next;
		}

		[[nodiscard]] bool operator!=(const Iterator &rhs) const
		{
			return !(*this == rhs);
		}

		Iterator &operator++()
		{
			return *this += 1;
		}

		Iterator &operator+=(unsigned increment)
		{
			if (!state_.endOfRecord())
				state_ = DiscardRemainingFields(state_.next, end_);

			if (state_.endOfFile()) {
				state_.next = nullptr;
			} else {
				if (increment > 0)
					state_ = DiscardMultipleRecords(state_.next, end_, increment - 1);
				// We use Status::ReadyToRead as a marker in case the Field iterator is never
				//  used, so the next call to operator+= will advance past the current record
				state_.status = GetFieldResult::Status::ReadyToRead;
			}
			return *this;
		}

		[[nodiscard]] value_type operator*()
		{
			return { &state_, end_ };
		}
	};

	[[nodiscard]] Iterator begin() const
	{
		return { begin_, end_ };
	}

	[[nodiscard]] Iterator end() const
	{
		return {};
	}
};
} // namespace devilution
