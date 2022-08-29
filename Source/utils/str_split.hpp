#pragma once

#include <iterator>
#include <type_traits>

#include "utils/stdcompat/string_view.hpp"

namespace devilution {
class SplitByCharIterator {
public:
	using iterator_category = std::forward_iterator_tag;
	using value_type = string_view;
	using reference = std::add_lvalue_reference<value_type>::type;
	using pointer = std::add_pointer<value_type>::type;

	static SplitByCharIterator begin(string_view text, char split_by) // NOLINT(readability-identifier-naming)
	{
		return SplitByCharIterator(split_by, text, text.substr(0, text.find(split_by)));
	}

	static SplitByCharIterator end(string_view text, char split_by) // NOLINT(readability-identifier-naming)
	{
		return SplitByCharIterator(split_by, text, text.substr(text.size()));
	}

	[[nodiscard]] string_view operator*() const
	{
		return slice_;
	}

	[[nodiscard]] const string_view *operator->() const
	{
		return &slice_;
	}

	SplitByCharIterator &operator++()
	{
		slice_ = text_.substr(slice_.data() - text_.data() + slice_.size());
		if (!slice_.empty())
			slice_.remove_prefix(1); // skip the split_by char
		slice_ = slice_.substr(0, slice_.find(split_by_));
		return *this;
	}

	SplitByCharIterator operator++(int)
	{
		auto copy = *this;
		++(*this);
		return copy;
	}

	bool operator==(const SplitByCharIterator &rhs) const
	{
		return slice_.data() == rhs.slice_.data();
	}

	bool operator!=(const SplitByCharIterator &rhs) const
	{
		return !(*this == rhs);
	}

private:
	SplitByCharIterator(char split_by, string_view text, string_view slice)
	    : split_by_(split_by)
	    , text_(text)
	    , slice_(slice)
	{
	}

	const char split_by_;
	const string_view text_;
	string_view slice_;
};

class SplitByChar {
public:
	explicit SplitByChar(string_view text, char split_by)
	    : text_(text)
	    , split_by_(split_by)
	{
	}

	[[nodiscard]] SplitByCharIterator begin() const // NOLINT(readability-identifier-naming)
	{
		return SplitByCharIterator::begin(text_, split_by_);
	}

	[[nodiscard]] SplitByCharIterator end() const // NOLINT(readability-identifier-naming)
	{
		return SplitByCharIterator::end(text_, split_by_);
	}

private:
	const string_view text_;
	const char split_by_;
};

} // namespace devilution
