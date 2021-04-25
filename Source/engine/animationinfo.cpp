/**
 * @file animationinfo.cpp
 *
 * Contains the core animation information and related logic
 */

#include "animationinfo.h"
#include "appfat.h"
#include "nthread.h"
#include "utils/log.hpp"

namespace devilution {

int AnimationInfo::GetFrameToUseForRendering()
{
	// Normal logic is used,
	// - if no frame-skipping is required and so we have exactly one Animationframe per GameTick
	// or
	// - if we load from a savegame where the new variables are not stored (we don't want to break savegame compatiblity because of smoother rendering of one animation)
	int relevantAnimationFramesForDistributing = RelevantFramesForDistributing;
	if (relevantAnimationFramesForDistributing <= 0)
		return CurrentFrame;

	if (CurrentFrame > relevantAnimationFramesForDistributing)
		return CurrentFrame;

	assert(GameTicksSinceSequenceStarted >= 0);

	float progressToNextGameTick = gfProgressToNextGameTick;

	// we don't use the processed game ticks alone but also the fragtion of the next game tick (if a rendering happens between game ticks). This helps to smooth the animations.
	float totalGameTicksForCurrentAnimationSequence = progressToNextGameTick + (float)GameTicksSinceSequenceStarted;

	// 1 added for rounding reasons. float to int cast always truncate.
	int absoluteAnimationFrame = 1 + (int)(totalGameTicksForCurrentAnimationSequence * GameTickModifier);
	if (absoluteAnimationFrame > relevantAnimationFramesForDistributing) {
		// this can happen if we are at the last frame and the next game tick is due (nthread_GetProgressToNextGameTick returns 1.0f)
		if (absoluteAnimationFrame > (relevantAnimationFramesForDistributing + 1)) {
			// we should never have +2 frames even if next game tick is due
			Log("GetFrameToUseForRendering: Calculated an invalid Animation Frame (Calculated {} MaxFrame {})", absoluteAnimationFrame, relevantAnimationFramesForDistributing);
		}
		return relevantAnimationFramesForDistributing;
	}
	if (absoluteAnimationFrame <= 0) {
		Log("GetFrameToUseForRendering: Calculated an invalid Animation Frame (Calculated {})", absoluteAnimationFrame);
		return 1;
	}
	return absoluteAnimationFrame;
}

} // namespace devilution
