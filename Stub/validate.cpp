/** @file
 * Perform some basic compile-time validation to make sure things line up.
 */
#include "../types.h"

static_assert(sizeof(plr) == 0x15360u, "sizeof(PlayerStruct) is wrong");
