#pragma once

#include <iterator>

#include "point.hpp"
#include "rectangle.hpp"

namespace devilution {

class PointsInRectangleIteratorBase {
public:
	using iterator_category = std::random_access_iterator_tag;
	using difference_type = int;
	using value_type = Point;
	using pointer = void;
	using reference = value_type;

protected:
	constexpr PointsInRectangleIteratorBase(Point origin, int majorDimension, int majorIndex, int minorIndex)
	    : origin(origin)
	    , majorDimension(majorDimension)
	    , majorIndex(majorIndex)
	    , minorIndex(minorIndex)
	{
	}

	explicit constexpr PointsInRectangleIteratorBase(Point origin, int majorDimension, int index = 0)
	    : PointsInRectangleIteratorBase(origin, majorDimension, index / majorDimension, index % majorDimension)
	{
	}

	void Increment()
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

	Point origin;

	int majorDimension;

	int majorIndex;
	int minorIndex;
};

class PointsInRectangleRange {
public:
	using const_iterator = class PointsInRectangleIterator : public PointsInRectangleIteratorBase {
	public:
		constexpr PointsInRectangleIterator(Rectangle region, int index = 0)
		    : PointsInRectangleIteratorBase(region.position, region.size.width, index)
		{
		}

		value_type operator*() const
		{
			// Row-major iteration e.g. {0, 0}, {1, 0}, {2, 0}, {0, 1}, {1, 1}, ...
			return origin + Displacement { minorIndex, majorIndex };
		}

		// Equality comparable concepts
		bool operator==(const PointsInRectangleIterator &rhs) const
		{
			return this->majorIndex == rhs.majorIndex && this->minorIndex == rhs.minorIndex;
		}

		bool operator!=(const PointsInRectangleIterator &rhs) const
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
			return (this->majorIndex - rhs.majorIndex) * majorDimension + (this->minorIndex - rhs.minorIndex);
		}

		// Forward concepts
		PointsInRectangleIterator &operator++()
		{
			Increment();
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
			Decrement();
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
			Offset(delta);
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

	constexpr PointsInRectangleRange(Rectangle region)
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
	Rectangle region;
};

class PointsInRectangleRangeColMajor {
public:
	using const_iterator = class PointsInRectangleIteratorColMajor : public PointsInRectangleIteratorBase {
	public:
		constexpr PointsInRectangleIteratorColMajor(Rectangle region, int index = 0)
		    : PointsInRectangleIteratorBase(region.position, region.size.height, index)
		{
		}

		value_type operator*() const
		{
			// Col-major iteration e.g. {0, 0}, {0, 1}, {0, 2}, {1, 0}, {1, 1}, ...
			return origin + Displacement { majorIndex, minorIndex };
		}

		// Equality comparable concepts
		bool operator==(const PointsInRectangleIteratorColMajor &rhs) const
		{
			return this->majorIndex == rhs.majorIndex && this->minorIndex == rhs.minorIndex;
		}

		bool operator!=(const PointsInRectangleIteratorColMajor &rhs) const
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
			return (this->majorIndex - rhs.majorIndex) * majorDimension + (this->minorIndex - rhs.minorIndex);
		}

		// Forward concepts
		PointsInRectangleIteratorColMajor &operator++()
		{
			Increment();
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
			Decrement();
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
			Offset(delta);
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
	constexpr PointsInRectangleRangeColMajor(Rectangle region)
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
	Rectangle region;
};

} // namespace devilution
