/**
 * @file restrict.h
 *
 * Interface of functionality for checking if the game will be able run on the system.
 */
#pragma once

namespace devilution {

/**
 * @brief Check that we have write access to the game install folder
 */
void ReadOnlyTest();

} // namespace devilution
