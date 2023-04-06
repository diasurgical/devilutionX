#pragma once

#include <cstdint>

namespace devilution {

/**
 * @brief A tagged union of a pointer and a value in the space of a single pointer.
 *
 * Requires the type T to have alignment > 1.
 * Internally, uses the last bit to hold the tag:
 * 0 if it is a pointer, guaranteed by T's alignment requirements.
 * 1 if it is a value.
 */
template <typename T>
class PointerOrValue {
	static_assert(alignof(T) > 1, "requires alignof > 1");
	static_assert(sizeof(T) < sizeof(T *), "type too large");

public:
	explicit PointerOrValue(const T *ptr)
	    : repr_(reinterpret_cast<uintptr_t>(ptr))
	{
	}

	explicit PointerOrValue(T val)
	    : repr_((static_cast<uintptr_t>(val) << 1) | 1)
	{
	}

	[[nodiscard]] bool HoldsPointer() const
	{
		return (repr_ & 1) == 0;
	}

	[[nodiscard]] const T *AsPointer() const
	{
		return reinterpret_cast<const T *>(repr_);
	}

	[[nodiscard]] T AsValue() const
	{
		return static_cast<T>(repr_ >> 1);
	}

private:
	uintptr_t repr_;
};

} // namespace devilution
