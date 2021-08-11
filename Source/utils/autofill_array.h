#pragma once

#include <array>

namespace devilution {

template <typename T, std::size_t N>
struct AutofillArray : public std::array<T, N> {
	using std::array<T, N>::array;

	explicit AutofillArray(T x)
	{
		std::array<T, N>::fill(x);
	}

	AutofillArray()
	    : AutofillArray(0)
	{
	}

	template <typename... Xs>
	AutofillArray(const T &x, const Xs &...xs)
	    : std::array<T, N>({ x, static_cast<const T &>(xs)... })
	{
	}
};

} //namespace devilution
