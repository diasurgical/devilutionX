#pragma once

#include <cstdint>

namespace devilution {

extern const std::uint8_t ButtonPcxDefault[];
extern const std::uint8_t ButtonPcxPressed[];

#if DEVILUTIONX_EMBEDDED_ERROR_DIALOG_BACKGROUND
extern const std::uint8_t PopupData[];
#endif

} // namespace devilution
