/**
 * @file capture.h
 *
 * Interface of the screenshot function.
 */
#pragma once

namespace devilution {

/**
 * @brief Save the current screen to a screen??.pcx (00-99) in file if available, then make the screen red for 200ms.
 */
void CaptureScreen();

} // namespace devilution
