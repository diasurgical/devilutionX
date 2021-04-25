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

void AnimationInfo::SetNewAnimation(uint8_t *pData, int numberOfFrames, int delayLen, AnimationDistributionParams params /*= AnimationDistributionParams::None*/, int numSkippedFrames /*= 0*/, int distributeFramesBeforeFrame /*= 0*/)
{
	this->pData = pData;
	NumberOfFrames = numberOfFrames;
	CurrentFrame = 1;
	DelayCounter = 0;
	DelayLen = delayLen;
	GameTicksSinceSequenceStarted = 0;
	RelevantFramesForDistributing = 0;
	GameTickModifier = 0.0f;

	if (numSkippedFrames != 0 || params != AnimationDistributionParams::None) {
		// Animation Frames that will be adjusted for the skipped Frames/GameTicks
		int relevantAnimationFramesForDistributing = numberOfFrames;
		if (distributeFramesBeforeFrame != 0) {
			// After an attack hits (_pAFNum or _pSFNum) it can be canceled or another attack can be queued and this means the animation is canceled.
			// In normal attacks frame skipping always happens before the attack actual hit.
			// This has the advantage that the sword or bow always points to the enemy when the hit happens (_pAFNum or _pSFNum).
			// Our distribution logic must also regard this behaviour, so we are not allowed to distribute the skipped animations after the actual hit (_pAnimStopDistributingAfterFrame).
			relevantAnimationFramesForDistributing = distributeFramesBeforeFrame - 1;
		}

		// How many GameTicks are needed to advance one Animation Frame
		int gameTicksPerFrame = (delayLen + 1);

		// GameTicks that will be adjusted for the skipped Frames/GameTicks
		int relevantAnimationGameTicksForDistribution = relevantAnimationFramesForDistributing * gameTicksPerFrame;

		// How many GameTicks will the Animation be really shown (skipped Frames and GameTicks removed)
		int relevantAnimationGameTicksWithSkipping = relevantAnimationGameTicksForDistribution - (numSkippedFrames * gameTicksPerFrame);

		if (params == AnimationDistributionParams::ProcessAnimationPending) {
			// If ProcessAnimation will be called after SetNewAnimation (in same GameTick as NewPlrAnim), we increment the Animation-Counter.
			// If no delay is specified, this will result in complete skipped frame (see ProcessAnimation).
			// But if we have a delay specified, this would only result in a reduced time the first frame is shown (one skipped delay).
			// Because of that, we only the remove one GameTick from the time the Animation is shown
			relevantAnimationGameTicksWithSkipping -= 1;
			// The Animation Distribution Logic needs to account how many GameTicks passed since the Animation started.
			// Because ProcessAnimation will increase this later (in same GameTick as SetNewAnimation), we correct this upfront.
			// This also means Rendering should never hapen with GameTicksSinceSequenceStarted < 0.
			GameTicksSinceSequenceStarted = -1;
		}

		if (params == AnimationDistributionParams::SkipsDelayOfLastFrame) {
			// The logic for player/monster/... (not ProcessAnimation) only checks the frame not the delay.
			// That means if a delay is specified, the last-frame is shown less then the other frames
			// Example:
			// If we have a animation with 3 frames and with a delay of 1 (gameTicksPerFrame = 2).
			// The logic checks "if (frame == 3) { start_new_animation(); }"
			// This will result that frame 4 is the last shown Animation Frame.
			// GameTick		Frame		Cnt
			// 1			1			0
			// 2			1			1
			// 3			2			0
			// 3			2			1
			// 4			3			0
			// 5			-			-
			// in GameTick 5 ProcessPlayer sees Frame = 3 and stops the animation.
			// But Frame 3 is only shown 1 GameTick and all other Frames are shown 2 GameTicks.
			// Thats why we need to remove the Delay of the last Frame from the time (GameTicks) the Animation is shown
			relevantAnimationGameTicksWithSkipping -= delayLen;
		}

		// if we skipped Frames we need to expand the GameTicks to make one GameTick for this Animation "faster"
		float gameTickModifier = (float)relevantAnimationGameTicksForDistribution / (float)relevantAnimationGameTicksWithSkipping;

		// gameTickModifier specifies the Animation fraction per GameTick, so we have to remove the delay from the variable
		gameTickModifier /= gameTicksPerFrame;

		RelevantFramesForDistributing = relevantAnimationFramesForDistributing;
		GameTickModifier = gameTickModifier;
	}
}

void AnimationInfo::ProcessAnimation()
{
	DelayCounter++;
	GameTicksSinceSequenceStarted++;
	if (DelayCounter > DelayLen) {
		DelayCounter = 0;
		CurrentFrame++;
		if (CurrentFrame > NumberOfFrames) {
			CurrentFrame = 1;
			GameTicksSinceSequenceStarted = 0;
		}
	}
}

} // namespace devilution
