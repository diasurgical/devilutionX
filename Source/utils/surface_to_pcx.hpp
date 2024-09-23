#include <cstdio>
#include <string>

#include <expected.hpp>

#include "engine/surface.hpp"

namespace devilution {

tl::expected<void, std::string>
WriteSurfaceToFilePcx(const Surface &buf, FILE *outStream);

} // namespace devilution
