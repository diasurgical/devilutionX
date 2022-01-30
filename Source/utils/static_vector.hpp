#pragma once

#include <cstddef>
#include <memory>
#include <type_traits>
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
	template <typename... Args>
	T &emplace_back(Args &&...args) // NOLINT(readability-identifier-naming)
	{
		assert(size_ < N);
		::new (&data_[size_]) T(std::forward<Args>(args)...);
#if __cplusplus >= 201703L
		T &result = *std::launder(reinterpret_cast<T *>(&data_[size_]));
#else
		T &result = *reinterpret_cast<T *>(&data_[size_]);
#endif
		++size_;
		return result;
	}

	const T &operator[](std::size_t pos) const
	{
#if __cplusplus >= 201703L
		return *std::launder(reinterpret_cast<const T *>(&data_[pos]));
#else
		return *reinterpret_cast<const T *>(&data_[pos]);
#endif
	}

	~StaticVector()
	{
		for (std::size_t pos = 0; pos < size_; ++pos) {
#if __cplusplus >= 201703L
			std::destroy_at(std::launder(reinterpret_cast<T *>(&data_[pos])));
#else
			reinterpret_cast<T *>(&data_[pos])->~T();
#endif
		}
	}

private:
	std::aligned_storage_t<sizeof(T), alignof(T)> data_[N];
	std::size_t size_ = 0;
};

} // namespace devilution
