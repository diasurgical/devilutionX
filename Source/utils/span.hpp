#pragma once

#include <cstddef>

namespace devilution {

/**
 * @brief A contiguous span of elements.
 *
 * Similar to std::span in C++20.
 */
template <typename T>
class Span {
public:
	Span(T *begin, T *end)
	    : begin_(begin)
	    , end_(end)
	{
	}

	T *begin() const
	{
		return begin_;
	}

	T *end() const
	{
		return end_;
	}

	size_t size() const
	{
		return end_ - begin_;
	}

	T &operator[](size_t i) const
	{
		return *(begin_ + i);
	}

private:
	T *begin_;
	T *end_;
};

} // namespace devilution
