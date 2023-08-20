#pragma once

#include <iterator>

namespace devilution {

/**
 * @brief A range of a pair of [begin, end) iterators.
 */
template <typename Iterator>
class IteratorRange {
public:
	using iterator = Iterator;
	using const_iterator = Iterator;
	using value_type = typename std::iterator_traits<Iterator>::value_type;

	IteratorRange() = default;

	IteratorRange(Iterator begin, Iterator end)
	    : begin_(begin)
	    , end_(end)
	{
	}

	Iterator begin()
	{
		return begin_;
	}

	Iterator end()
	{
		return end_;
	}

	bool empty() const
	{
		return begin_ == end_;
	}

private:
	Iterator begin_;
	Iterator end_;
};

} // namespace devilution
