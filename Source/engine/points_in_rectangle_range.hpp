#pragma once

#include <iterator>

#include "point.hpp"
#include "rectangle.hpp"

namespace devilution {

template <typename CoordT>
class PointsInRectangleIteratorBase {
public:
	using iterator_category = std::random_access_iterator_tag;
	using difference_type = CoordT;
	using value_type = PointOf<CoordT>;
	using pointer = void;
	using reference = value_type;

protected:
	constexpr PointsInRectangleIteratorBase(PointOf<CoordT> origin, int majorDimension, int majorIndex, int minorIndex)
	    : origin(origin)
	    , majorDimension(majorDimension)
	    , majorIndex(majorIndex)
	    , minorIndex(minorIndex)
	{
	}

	explicit constexpr PointsInRectangleIteratorBase(PointOf<CoordT> origin, int majorDimension, int index = 0)
	    : PointsInRectangleIteratorBase(origin, majorDimension, index / majorDimension, index % majorDimension)
	{
	}

	DVL_ALWAYS_INLINE void Increment()
	{
		++minorIndex;
		if (minorIndex >= majorDimension) {
			++majorIndex;
			minorIndex -= majorDimension;
		}
	}

	void Decrement()
	{
		if (minorIndex <= 0) {
			--majorIndex;
			minorIndex += majorDimension;
		}
		--minorIndex;
	}

	void Offset(difference_type delta)
	{
		majorIndex += (minorIndex + delta) / majorDimension;
		minorIndex = (minorIndex + delta) % majorDimension;
		if (minorIndex < 0) {
			minorIndex += majorDimension;
			--majorIndex;
		}
	}

	PointOf<CoordT> origin;

	int majorDimension;

	int majorIndex;
	int minorIndex;
};

template <typename CoordT>
class PointsInRectangle {
public:
	using const_iterator = class PointsInRectangleIterator : public PointsInRectangleIteratorBase<CoordT> {
	public:
		using iterator_category = typename PointsInRectangleIteratorBase<CoordT>::iterator_category;
		using difference_type = typename PointsInRectangleIteratorBase<CoordT>::difference_type;
		using value_type = typename PointsInRectangleIteratorBase<CoordT>::value_type;
		using pointer = typename PointsInRectangleIteratorBase<CoordT>::pointer;
		using reference = typename PointsInRectangleIteratorBase<CoordT>::reference;

		constexpr PointsInRectangleIterator(RectangleOf<CoordT> region, int index = 0)
		    : PointsInRectangleIteratorBase<CoordT>(region.position, region.size.width, index)
		{
		}

		DVL_ALWAYS_INLINE value_type operator*() const
		{
			// Row-major iteration e.g. {0, 0}, {1, 0}, {2, 0}, {0, 1}, {1, 1}, ...
			return this->origin + Displacement { this->minorIndex, this->majorIndex };
		}

		// Equality comparable concepts
		DVL_ALWAYS_INLINE bool operator==(const PointsInRectangleIterator &rhs) const
		{
			return this->majorIndex == rhs.majorIndex && this->minorIndex == rhs.minorIndex;
		}

		DVL_ALWAYS_INLINE bool operator!=(const PointsInRectangleIterator &rhs) const
		{
			return !(*this == rhs);
		}

		// Partially ordered concepts
		bool operator>=(const PointsInRectangleIterator &rhs) const
		{
			return this->majorIndex > rhs.majorIndex || (this->majorIndex == rhs.majorIndex && this->minorIndex >= rhs.minorIndex);
		}

		bool operator<(const PointsInRectangleIterator &rhs) const
		{
			return !(*this >= rhs);
		}

		bool operator<=(const PointsInRectangleIterator &rhs) const
		{
			return this->majorIndex < rhs.majorIndex || (this->majorIndex == rhs.majorIndex && this->minorIndex <= rhs.minorIndex);
		}

		bool operator>(const PointsInRectangleIterator &rhs) const
		{
			return !(*this <= rhs);
		}

		difference_type operator-(const PointsInRectangleIterator &rhs) const
		{
			return (this->majorIndex - rhs.majorIndex) * this->majorDimension + (this->minorIndex - rhs.minorIndex);
		}

		// Forward concepts
		DVL_ALWAYS_INLINE PointsInRectangleIterator &operator++()
		{
			this->Increment();
			return *this;
		}

		PointsInRectangleIterator operator++(int)
		{
			auto copy = *this;
			++(*this);
			return copy;
		}

		// Bidirectional concepts
		PointsInRectangleIterator &operator--()
		{
			this->Decrement();
			return *this;
		}

		PointsInRectangleIterator operator--(int)
		{
			auto copy = *this;
			--(*this);
			return copy;
		}

		// Random access concepts
		PointsInRectangleIterator operator+(difference_type delta) const
		{
			auto copy = *this;
			return copy += delta;
		}

		PointsInRectangleIterator &operator+=(difference_type delta)
		{
			this->Offset(delta);
			return *this;
		}

		friend PointsInRectangleIterator operator+(difference_type delta, const PointsInRectangleIterator &it)
		{
			return it + delta;
		}

		PointsInRectangleIterator &operator-=(difference_type delta)
		{
			return *this += -delta;
		}

		PointsInRectangleIterator operator-(difference_type delta) const
		{
			auto copy = *this;
			return copy -= delta;
		}

		value_type operator[](difference_type offset) const
		{
			return **(this + offset);
		}
	};

	constexpr PointsInRectangle(RectangleOf<CoordT> region)
	    : region(region)
	{
	}

	[[nodiscard]] const_iterator cbegin() const
	{
		return region;
	}

	[[nodiscard]] const_iterator begin() const
	{
		return cbegin();
	}

	[[nodiscard]] const_iterator cend() const
	{
		return { region, region.size.width * region.size.height };
	}

	[[nodiscard]] const_iterator end() const
	{
		return cend();
	}

	[[nodiscard]] auto crbegin() const
	{
		// explicit type needed for older GCC versions
		return std::reverse_iterator<const_iterator>(cend());
	}

	[[nodiscard]] auto rbegin() const
	{
		return crbegin();
	}

	[[nodiscard]] auto crend() const
	{
		// explicit type needed for older GCC versions
		return std::reverse_iterator<const_iterator>(cbegin());
	}

	[[nodiscard]] auto rend() const
	{
		return crend();
	}

protected:
	RectangleOf<CoordT> region;
};

template <typename CoordT>
class PointsInRectangleColMajor {
public:
	using const_iterator = class PointsInRectangleIteratorColMajor : public PointsInRectangleIteratorBase<CoordT> {
	public:
		using iterator_category = typename PointsInRectangleIteratorBase<CoordT>::iterator_category;
		using difference_type = typename PointsInRectangleIteratorBase<CoordT>::difference_type;
		using value_type = typename PointsInRectangleIteratorBase<CoordT>::value_type;
		using pointer = typename PointsInRectangleIteratorBase<CoordT>::pointer;
		using reference = typename PointsInRectangleIteratorBase<CoordT>::reference;

		constexpr PointsInRectangleIteratorColMajor(RectangleOf<CoordT> region, int index = 0)
		    : PointsInRectangleIteratorBase<CoordT>(region.position, region.size.height, index)
		{
		}

		DVL_ALWAYS_INLINE value_type operator*() const
		{
			// Col-major iteration e.g. {0, 0}, {0, 1}, {0, 2}, {1, 0}, {1, 1}, ...
			return this->origin + Displacement { this->majorIndex, this->minorIndex };
		}

		// Equality comparable concepts
		DVL_ALWAYS_INLINE bool operator==(const PointsInRectangleIteratorColMajor &rhs) const
		{
			return this->majorIndex == rhs.majorIndex && this->minorIndex == rhs.minorIndex;
		}

		DVL_ALWAYS_INLINE bool operator!=(const PointsInRectangleIteratorColMajor &rhs) const
		{
			return !(*this == rhs);
		}

		// Partially ordered concepts
		bool operator>=(const PointsInRectangleIteratorColMajor &rhs) const
		{
			return this->majorIndex > rhs.majorIndex || (this->majorIndex == rhs.majorIndex && this->minorIndex >= rhs.minorIndex);
		}

		bool operator<(const PointsInRectangleIteratorColMajor &rhs) const
		{
			return !(*this >= rhs);
		}

		bool operator<=(const PointsInRectangleIteratorColMajor &rhs) const
		{
			return this->majorIndex < rhs.majorIndex || (this->majorIndex == rhs.majorIndex && this->minorIndex <= rhs.minorIndex);
		}

		bool operator>(const PointsInRectangleIteratorColMajor &rhs) const
		{
			return !(*this <= rhs);
		}

		difference_type operator-(const PointsInRectangleIteratorColMajor &rhs) const
		{
			return (this->majorIndex - rhs.majorIndex) * this->majorDimension + (this->minorIndex - rhs.minorIndex);
		}

		// Forward concepts
		DVL_ALWAYS_INLINE PointsInRectangleIteratorColMajor &operator++()
		{
			this->Increment();
			return *this;
		}

		PointsInRectangleIteratorColMajor operator++(int)
		{
			auto copy = *this;
			++(*this);
			return copy;
		}

		// Bidirectional concepts
		PointsInRectangleIteratorColMajor &operator--()
		{
			this->Decrement();
			return *this;
		}

		PointsInRectangleIteratorColMajor operator--(int)
		{
			auto copy = *this;
			--(*this);
			return copy;
		}

		// Random access concepts
		PointsInRectangleIteratorColMajor operator+(difference_type delta) const
		{
			auto copy = *this;
			return copy += delta;
		}

		PointsInRectangleIteratorColMajor &operator+=(difference_type delta)
		{
			this->Offset(delta);
			return *this;
		}

		friend PointsInRectangleIteratorColMajor operator+(difference_type delta, const PointsInRectangleIteratorColMajor &it)
		{
			return it + delta;
		}

		PointsInRectangleIteratorColMajor &operator-=(difference_type delta)
		{
			return *this += -delta;
		}

		PointsInRectangleIteratorColMajor operator-(difference_type delta) const
		{
			auto copy = *this;
			return copy -= delta;
		}

		value_type operator[](difference_type offset) const
		{
			return **(this + offset);
		}
	};

	// gcc6 needs a defined constructor?
	constexpr PointsInRectangleColMajor(RectangleOf<CoordT> region)
	    : region(region)
	{
	}

	[[nodiscard]] const_iterator cbegin() const
	{
		return region;
	}

	[[nodiscard]] const_iterator begin() const
	{
		return cbegin();
	}

	[[nodiscard]] const_iterator cend() const
	{
		return { region, region.size.width * region.size.height };
	}

	[[nodiscard]] const_iterator end() const
	{
		return cend();
	}

	[[nodiscard]] auto crbegin() const
	{
		// explicit type needed for older GCC versions
		return std::reverse_iterator<const_iterator>(cend());
	}

	[[nodiscard]] auto rbegin() const
	{
		return crbegin();
	}

	[[nodiscard]] auto crend() const
	{
		// explicit type needed for older GCC versions
		return std::reverse_iterator<const_iterator>(cbegin());
	}

	[[nodiscard]] auto rend() const
	{
		return crend();
	}

protected:
	RectangleOf<CoordT> region;
};

} // namespace devilution
