#pragma once

#include "common.hpp"

namespace devilution {
class RecordsRange {
	const char *const begin_;
	const char *const end_;

public:
	RecordsRange(const std::string_view record)
	    : begin_(record.data())
	    , end_(record.data() + record.size())
	{
	}

	class Iterator {
		GetFieldResult state_;
		const char *const end_;

	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = std::string_view;

		Iterator(const char *end)
		    : state_(nullptr, GetFieldResult::Status::EndOfFile)
		    , end_(end)
		{
		}

		Iterator(const char *begin, const char *end)
		    : state_(GetNextRecord(begin, end))
		    , end_(end)
		{
		}

		bool operator==(const Iterator &rhs)
		{
			return state_.next == rhs.state_.next;
		}

		bool operator!=(const Iterator &rhs)
		{
			return !(*this == rhs);
		}

		Iterator &operator++()
		{
			if (state_.endOfFile()) {
				state_.next = nullptr;
			} else {
				state_ = GetNextRecord(state_.next, end_);
			}
			return *this;
		}

		value_type operator*()
		{
			return state_.value;
		}
	};

	Iterator begin() const
	{
		return { begin_, end_ };
	}

	Iterator end() const
	{
		return { end_ };
	}
};

class FieldsInRecordRange {
	const char *const begin_;
	const char *const end_;

public:
	FieldsInRecordRange(const std::string_view record)
	    : begin_(record.data())
	    , end_(record.data() + record.size())
	{
	}

	class Iterator {
		GetFieldResult state_;
		const char *const end_;

	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = std::string_view;

		Iterator(const char *end)
		    : state_(nullptr, GetFieldResult::Status::EndOfRecord)
		    , end_(end)
		{
		}

		Iterator(const char *begin, const char *end)
		    : state_(GetNextField(begin, end))
		    , end_(end)
		{
		}

		bool operator==(const Iterator &rhs)
		{
			return state_.next == rhs.state_.next;
		}

		bool operator!=(const Iterator &rhs)
		{
			return !(*this == rhs);
		}

		Iterator &operator++()
		{
			if (state_.endOfRecord()) {
				state_.next = nullptr;
			} else {
				state_ = GetNextField(state_.next, end_);
			}
			return *this;
		}

		value_type operator*()
		{
			return state_.value;
		}
	};

	Iterator begin() const
	{
		return { begin_, end_ };
	}

	Iterator end() const
	{
		return { end_ };
	}
};
} // namespace devilution
