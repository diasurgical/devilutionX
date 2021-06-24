#pragma once

#include <cstddef> // IWYU pragma: export

#if defined(__cplusplus) && __cplusplus >= 201703L
namespace devilution {
using byte = std::byte;
} // namespace devilution
#else
#include <cstdint>
namespace devilution {
using byte = std::uint8_t;
} // namespace devilution
#endif
