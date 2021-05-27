#pragma once

#ifdef __has_include
#if __has_include(<cstddef>)
#include <cstddef>
#if defined(__cplusplus) && __cplusplus >= 201703L
namespace devilution {
    using byte = std::byte;
}
#else
namespace devilution {
    using byte = uint8_t;
}
#endif
#endif
#endif