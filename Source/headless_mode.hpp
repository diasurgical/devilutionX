#pragma once

#include "utils/attributes.h"

namespace devilution {

/**
 * @brief Don't load UI or show Messageboxes or other user-interaction. Needed for unit tests.
 */
extern DVL_API_FOR_TEST bool HeadlessMode;

} // namespace devilution
