#pragma once

#include <cstddef>
#ifdef __has_include
#if defined(__cplusplus) && __cplusplus >= 201703L
namespace devilution {
using byte = std::byte;
}
#else
#include <cstdint>
namespace devilution {
using byte = std::uint8_t;
}
#endif
#endif

