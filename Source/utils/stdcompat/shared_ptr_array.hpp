#pragma once

#include <cstddef>
#include <memory>

namespace devilution {

// Apple Clang 12 has a buggy implementation that fails to compile `std::shared_ptr<T[]>(new T[size])`.
#if (__cplusplus >= 201611L && (!defined(__clang_major__) || __clang_major__ >= 13)) && !defined(NXDK)
template <typename T>
using ArraySharedPtr = std::shared_ptr<T[]>;

template <typename T>
ArraySharedPtr<T> MakeArraySharedPtr(std::size_t size)
{
	return ArraySharedPtr<T>(new T[size]);
}
#else
template <typename T>
using ArraySharedPtr = std::shared_ptr<T>;

template <typename T>
ArraySharedPtr<T> MakeArraySharedPtr(std::size_t size)
{
	return ArraySharedPtr<T> { new T[size], std::default_delete<T[]>() };
}
#endif

} // namespace devilution
