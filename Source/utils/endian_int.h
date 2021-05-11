#pragma once

#include <type_traits>

namespace devilution {

template<typename T>
T SwapLE(T in)
{
	static_assert(std::is_integral<T>::value || std::is_enum<T>::value, "unsupported type");
	static_assert((sizeof(T) == 1) || (sizeof(T) == 2) || (sizeof(T) == 4) || (sizeof(T) == 8), "unsupported type");
	if (sizeof(T) == 1)
		return in;
	if (sizeof(T) == 2)
		return static_cast<T>(SDL_SwapLE16(static_cast<Uint16>(in)));
	if (sizeof(T) == 4)
		return static_cast<T>(SDL_SwapLE32(static_cast<Uint32>(in)));
	if (sizeof(T) == 8)
		return static_cast<T>(SDL_SwapLE64(static_cast<Uint64>(in)));
}

template<typename T>
T SwapBE(T in)
{
	static_assert(std::is_integral<T>::value || std::is_enum<T>::value, "unsupported type");
	static_assert((sizeof(T) == 1) || (sizeof(T) == 2) || (sizeof(T) == 4) || (sizeof(T) == 8), "unsupported type");
	if (sizeof(T) == 1)
		return in;
	if (sizeof(T) == 2)
		return static_cast<T>(SDL_SwapBE16(static_cast<Uint16>(in)));
	if (sizeof(T) == 4)
		return static_cast<T>(SDL_SwapBE32(static_cast<Uint32>(in)));
	if (sizeof(T) == 8)
		return static_cast<T>(SDL_SwapBE64(static_cast<Uint64>(in)));
}

template <typename T, T (*f)(T)>
class endian_int {
	T value;

public:
	endian_int() :
		value(0)
	{
	}

	endian_int(const endian_int<T, f>& in) :
		value(in.value)
	{
	}

	endian_int(const T& in) :
		value(f(in))
	{
	}

	endian_int<T, f>& operator=(const endian_int<T, f>& in)
	{
		value = in.value;
		return *this;
	}

	endian_int<T, f>& operator=(const T& in)
	{
		value = f(in);
		return *this;
	}

	endian_int<T, f>& operator-=(const T& in)
	{
		value = f(f(value) - in);
		return *this;
	}

	endian_int<T, f>& operator+=(const T& in)
	{
		value = f(f(value) + in);
		return *this;
	}

	operator T() const
	{
		return f(value);
	}
};

template<typename N, typename T, T (*f)(T)>
N endian_cast(const endian_int<T, f>& in)
{
	return static_cast<N>(static_cast<T>(in));
}

using int16_be_t = endian_int<int16_t, SwapBE>;
using int32_be_t = endian_int<int32_t, SwapBE>;
using int64_be_t = endian_int<int64_t, SwapBE>;
using uint16_be_t = endian_int<uint16_t, SwapBE>;
using uint32_be_t = endian_int<uint32_t, SwapBE>;
using uint64_be_t = endian_int<uint64_t, SwapBE>;

using int16_le_t = endian_int<int16_t, SwapLE>;
using int32_le_t = endian_int<int32_t, SwapLE>;
using int64_le_t = endian_int<int64_t, SwapLE>;
using uint16_le_t = endian_int<uint16_t, SwapLE>;
using uint32_le_t = endian_int<uint32_t, SwapLE>;
using uint64_le_t = endian_int<uint64_t, SwapLE>;

using uint16_net_t = uint16_be_t;
using uint32_net_t = uint32_be_t;
using uint64_net_t = uint64_be_t;
using int16_net_t = int16_be_t;
using int32_net_t = int32_be_t;
using int64_net_t = int64_be_t;

}
