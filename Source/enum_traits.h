/**
 * @file enum_traits.h
 *
 * Base template for 'enum_traits' which allow us to access static information about an enum.
 */
#pragma once

#include <cstddef>

namespace devilution {

template <typename T>
struct enum_size {
	constexpr static const std::size_t value = static_cast<std::size_t>(T::LAST) + 1;
};

}
