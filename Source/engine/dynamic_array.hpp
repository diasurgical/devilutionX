#pragma once

#include <array>
#include <numeric>
#include <stdexcept>

namespace devilution {
template <typename T, std::size_t max_size_>
class dynamic_array {
public:
	using value_type = T;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	using pointer = value_type *;
	using const_pointer = const value_type *;
	using reference = value_type &;
	using const_reference = const value_type &;

	class const_iterator {
	public:
		using iterator_category = std::random_access_iterator_tag;
		using difference_type = dynamic_array::difference_type;
		using value_type = dynamic_array::value_type;
		using pointer = dynamic_array::const_pointer;
		using reference = dynamic_array::const_reference;

		const_iterator() = default;

		const_iterator(const std::array<dynamic_array::value_type, max_size_> *elements, const std::array<dynamic_array::size_type, max_size_> *indexes, dynamic_array::size_type offset)
		    : elements_(elements)
		    , indexes_(indexes)
		    , offset_(offset)
		{
		}

		constexpr reference operator[](difference_type offset) const
		{
			return (*elements_)[(*indexes_)[this->offset_ + offset]];
		}

		reference operator*() const
		{
			return operator[](0);
		}

		pointer operator->() const
		{
			return &operator*();
		}

		const_iterator &operator++()
		{
			++offset_;
			return *this;
		}

		const_iterator operator++(int)
		{
			auto copy = *this;
			++(*this);
			return copy;
		}

		const_iterator &operator--()
		{
			--offset_;
			return *this;
		}

		const_iterator operator--(int)
		{
			auto copy = *this;
			--(*this);
			return copy;
		}

		const_iterator &operator+=(difference_type offset)
		{
			this->offset_ += offset;
			return *this;
		}

		const_iterator operator+(difference_type offset)
		{
			auto copy = *this;
			operator+=(offset);
			return copy;
		}

		const_iterator &operator-=(difference_type offset)
		{
			operator+=(-offset);
			return *this;
		}

		const_iterator operator-(difference_type offset)
		{
			auto copy = *this;
			operator-=(offset);
			return copy;
		}

		difference_type operator-(const_iterator other)
		{
			return offset_ - other.offset_;
		}

		bool operator==(const_iterator &other) const
		{
			return offset_ == other.offset_;
		}

		bool operator!=(const_iterator &other) const
		{
			return !(*this == other);
		}

		bool operator>(const_iterator &other) const
		{
			return offset_ > other.offset_;
		}

		bool operator<(const_iterator &other) const
		{
			return offset_ < other.offset_;
		}

		bool operator>=(const_iterator &other) const
		{
			return !(*this < other);
		}

		bool operator<=(const_iterator &other) const
		{
			return !(*this > other);
		}

	private:
		const std::array<dynamic_array::value_type, max_size_> *elements_ = nullptr;
		const std::array<dynamic_array::size_type, max_size_> *indexes_ = nullptr;
		dynamic_array::size_type offset_ = 0;
	};

	class iterator : public const_iterator {
	public:
		using pointer = dynamic_array::pointer;
		using reference = dynamic_array::reference;

		iterator() = default;

		iterator(std::array<dynamic_array::value_type, max_size_> *elements, std::array<dynamic_array::size_type, max_size_> *indexes, dynamic_array::size_type offset)
		    : const_iterator(elements, indexes, offset)
		{
		}

		constexpr reference operator[](difference_type offset)
		{
			return const_cast<reference>(const_iterator::operator[](offset));
		}

		reference operator*()
		{
			return const_cast<reference>(const_iterator::operator*());
		}

		pointer operator->()
		{
			return const_cast<pointer>(const_iterator::operator->());
		}
	};

	using const_reverse_iterator = std::reverse_iterator<const_iterator>;
	using reverse_iterator = std::reverse_iterator<iterator>;

	dynamic_array()
	{
		std::iota(indexes_.begin(), indexes_.end(), 0);
	}

	dynamic_array(std::initializer_list<value_type> elements)
	    : size_(elements.size())
	{
		std::iota(indexes_.begin(), indexes_.end(), 0);
		std::move(elements.begin(), elements.end(), elements_.begin());
	}

	constexpr reference operator[](size_type index)
	{
		return elements_[indexes_[index]];
	}

	reference at(size_type index)
	{
		if (index < size()) {
			return elements_[indexes_[index]];
		}
		throw std::out_of_range("index greater than current size");
	}

	const_reference at(size_type index) const
	{
		if (index < size()) {
			return elements_[indexes_[index]];
		}
		throw std::out_of_range("index greater than current size");
	}

	constexpr size_type size() const
	{
		return size_;
	}

	constexpr size_type max_size() const
	{
		return max_size_;
	}

	constexpr size_type capacity() const
	{
		return max_size();
	}

	constexpr bool empty() const
	{
		return size() == 0;
	}

	constexpr bool full() const
	{
		return size() >= max_size();
	}

	iterator begin()
	{
		return { &elements_, &indexes_, 0 };
	}

	const_iterator cbegin()
	{
		return { &elements_, &indexes_, 0 };
	}

	iterator end()
	{
		return { nullptr, nullptr, size() };
	}

	const_iterator cend()
	{
		return { nullptr, nullptr, size() };
	}

	constexpr void clear()
	{
		size_ = 0;
		std::iota(indexes_.begin(), indexes_.end(), 0);
	}

	iterator erase(const_iterator pos)
	{
		if (!empty())
			size_--;

		size_type offset = static_cast<size_type>(pos - begin());
		if (offset < size_) {
			std::swap(indexes_[offset], indexes_[size_]);
			return { &elements_, &indexes_, offset };
		}

		return end();
	}

	iterator erase(const_iterator first, const_iterator last)
	{
		/*if (first >= last) {
			// empty range passed in, no work to do
			return begin() + (last - begin());
		}

		if (last == end()) {
			// special case deleting from the end of the list, no need to move indexes around.
			size_ -= last - first;
			return end();
		}

		while (last > first) {
			last--; // because the iterator implements random_access we can use the guarantee that last - 1 is valid, this lets us remove the range working backwards
			last = erase(last);
		}
		return begin() + (last - begin());*/
		return end();
	}

	constexpr void push_back(const_reference element)
	{
		if (full()) {
			// throw std::length_error;
			return;
		}

		operator[](size_) = element;
		size_++;
	}

	constexpr void push_back(T &&element)
	{
		if (full()) {
			// throw std::length_error;
			return;
		}

		operator[](size_) = std::move(element);
		size_++;
	}

private:
	size_type size_ = 0;
	std::array<size_type, max_size_> indexes_;
	std::array<T, max_size_> elements_;
};
} // namespace devilution
