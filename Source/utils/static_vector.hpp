#pragma once

#include <cstddef>
#include <initializer_list>
#include <memory>
#include <utility>

#include "appfat.h"

namespace devilution {

/**
 * @brief A stack-allocated vector with a fixed capacity.
 *
 * @tparam T element type.
 * @tparam N capacity.
 */
template <class T, size_t N>
class StaticVector {
public:
	StaticVector() = default;

	template <typename U>
	StaticVector(std::initializer_list<U> elements)
	{
		for (auto &&element : elements) {
			emplace_back(element);
		}
	}

	[[nodiscard]] const T *begin() const
	{
		return &(*this)[0];
	}

	[[nodiscard]] const T *end() const
	{
		return begin() + size_;
	}

	[[nodiscard]] size_t size() const
	{
		return size_;
	}

	[[nodiscard]] bool empty() const
	{
		return size_ == 0;
	}

	[[nodiscard]] T &back()
	{
		return (*this)[size_ - 1];
	}

	[[nodiscard]] const T &back() const
	{
		return (*this)[size_ - 1];
	}

	template <typename... Args>
	T &emplace_back(Args &&...args) // NOLINT(readability-identifier-naming)
	{
		assert(size_ < N);
		return *::new (&data_[size_++]) T(std::forward<Args>(args)...);
	}

	T &operator[](std::size_t pos)
	{
		return *data_[pos].ptr();
	}

	const T &operator[](std::size_t pos) const
	{
		return *data_[pos].ptr();
	}

	~StaticVector()
	{
		for (std::size_t pos = 0; pos < size_; ++pos) {
			std::destroy_at(data_[pos].ptr());
		}
	}

private:
	struct AlignedStorage {
		alignas(alignof(T)) std::byte data[sizeof(T)];

		const T *ptr() const
		{
			return std::launder(reinterpret_cast<const T *>(data));
		}

		T *ptr()
		{
			return std::launder(reinterpret_cast<T *>(data));
		}
	};
	AlignedStorage data_[N];
	std::size_t size_ = 0;
};

} // namespace devilution
