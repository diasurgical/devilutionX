/**
 * @file animationinfo.cpp
 *
 * Contains the core animation information and related logic.
 */

#include "animationinfo.h"

#include <algorithm>
#include <cstdint>

#include "appfat.h"
#include "nthread.h"
#include "utils/log.hpp"

namespace devilution 
{

int8_t AnimationInfo::getFrameToUseForRendering() const
{
	// If no frame-skipping is required (exactly one Animation frame per game tick),
	// or if loading from a save game where the new variables are not stored, use normal logic.
	if (relevantFramesForDistributing_ <= 0)
		return std::max<int8_t>(0, currentFrame);

	// Handle cases where the current frame exceeds the number of relevant frames.
	if (currentFrame >= relevantFramesForDistributing_)
		return currentFrame;

	// Ensure ticksSinceSequenceStarted_ is non-negative for calculations.
	int16_t ticksSinceSequenceStarted = std::max<int16_t>(0, ticksSinceSequenceStarted_);
	if (ticksSinceSequenceStarted_ < 0)
		Log("getFrameToUseForRendering: Invalid ticksSinceSequenceStarted_ {}", ticksSinceSequenceStarted_);

	// Calculate total ticks considering progress to next game tick for smoother rendering.
	int32_t totalTicksForCurrentAnimationSequence = getProgressToNextGameTick() + ticksSinceSequenceStarted;

	// Compute the absolute frame based on timing and modifiers.
	int8_t absoluteAnimationFrame = static_cast<int8_t>((totalTicksForCurrentAnimationSequence * tickModifier_) / (baseValueFraction * baseValueFraction));

	// Adjust for skipped frames from previous animations.
	if (skippedFramesFromPreviousAnimation_ > 0) 
	{
		absoluteAnimationFrame -= skippedFramesFromPreviousAnimation_;
		if (absoluteAnimationFrame < 0) 
		{
			absoluteAnimationFrame = numberOfFrames + absoluteAnimationFrame;
		}
	} 
	else if (absoluteAnimationFrame >= relevantFramesForDistributing_) 
	{
		// Handle edge case where the calculated frame exceeds limits.
		if (absoluteAnimationFrame >= (relevantFramesForDistributing_ + 1)) 
		{
			Log("getFrameToUseForRendering: Invalid Animation Frame (Calculated {} MaxFrame {})", absoluteAnimationFrame, relevantFramesForDistributing_);
		}
		
		return relevantFramesForDistributing_ - 1;
	}

	// Ensure the calculated frame is within valid bounds.
	if (absoluteAnimationFrame < 0) 
	{
		Log("getFrameToUseForRendering: Invalid Animation Frame (Calculated {})", absoluteAnimationFrame);

		return 0;
	}

	return absoluteAnimationFrame;
}

uint8_t AnimationInfo::getAnimationProgress() const
{
	// Ensure ticksSinceSequenceStarted_ is non-negative for calculations.
	int16_t ticksSinceSequenceStarted = std::max<int16_t>(0, ticksSinceSequenceStarted_);
	int32_t tickModifier = tickModifier_;

	// Handle case where animation distribution is not active.
	if (relevantFramesForDistributing_ <= 0) 
	{
		if (ticksPerFrame <= 0) 
		{
			Log("getAnimationProgress: Invalid ticksPerFrame {}", ticksPerFrame);
			return 0;
		}

		ticksSinceSequenceStarted = ((currentFrame * ticksPerFrame) + tickCounterOfCurrentFrame) * baseValueFraction;
		tickModifier = baseValueFraction / ticksPerFrame;
	}

	int32_t totalTicksForCurrentAnimationSequence = getProgressToNextGameTick() + ticksSinceSequenceStarted;
	int32_t progressInAnimationFrames = totalTicksForCurrentAnimationSequence * tickModifier;
	int32_t animationFraction = progressInAnimationFrames / (numberOfFrames * baseValueFraction);

	assert(animationFraction <= baseValueFraction);
	
	return static_cast<uint8_t>(animationFraction);
}

void AnimationInfo::setNewAnimation(OptionalClxSpriteList celSprite, int8_t numberOfFrames,	int8_t ticksPerFrame, AnimationDistributionFlags flags /*= AnimationDistributionFlags::None*/, int8_t numSkippedFrames /*= 0*/,	int8_t distributeFramesBeforeFrame /*= 0*/,	uint8_t previewShownGameTickFragments /*= 0*/) 
{
	if ((flags & AnimationDistributionFlags::RepeatedAction) == AnimationDistributionFlags::RepeatedAction && distributeFramesBeforeFrame != 0 && this->numberOfFrames == numberOfFrames && currentFrame + 1 >= distributeFramesBeforeFrame && currentFrame != this->numberOfFrames - 1)
	{
		// Restore truncated animation frames.
		skippedFramesFromPreviousAnimation_ = this->numberOfFrames - currentFrame - 1;
	} 
	else 
	{
		skippedFramesFromPreviousAnimation_ = 0;
	}

	// Ensure ticksPerFrame is valid.
	if (ticksPerFrame <= 0) 
	{
		Log("setNewAnimation: Invalid ticksPerFrame {}", ticksPerFrame);
		ticksPerFrame = 1;
	}

	// Initialize animation state.
	this->sprites = celSprite;
	this->numberOfFrames = numberOfFrames;
	currentFrame = numSkippedFrames;
	tickCounterOfCurrentFrame = 0;
	this->ticksPerFrame = ticksPerFrame;
	ticksSinceSequenceStarted_ = 0;
	relevantFramesForDistributing_ = 0;
	tickModifier_ = 0;
	isPetrified = false;

	// Only modify distribution if frames were skipped or specific flags are set.
	if (numSkippedFrames != 0 || flags != AnimationDistributionFlags::None) 
	{
		configureFrameDistribution(flags, numberOfFrames, distributeFramesBeforeFrame, numSkippedFrames, ticksPerFrame, previewShownGameTickFragments);
	}
}

void AnimationInfo::configureFrameDistribution(AnimationDistributionFlags flags, int8_t numberOfFrames,	int8_t distributeFramesBeforeFrame,	int8_t numSkippedFrames, int8_t ticksPerFrame, uint8_t previewShownGameTickFragments) 
{
	int8_t relevantAnimationFramesForDistributing = numberOfFrames;

	// Adjust relevant frames based on pre-frame distribution.
	if (distributeFramesBeforeFrame != 0) 
	{
		relevantAnimationFramesForDistributing = distributeFramesBeforeFrame - 1;
	}

	int32_t relevantAnimationTicksForDistribution = relevantAnimationFramesForDistributing * ticksPerFrame;
	int32_t relevantAnimationTicksWithSkipping = relevantAnimationTicksForDistribution - (numSkippedFrames * ticksPerFrame);

	if (flags & AnimationDistributionFlags::ProcessAnimationPending) 
	{
		relevantAnimationTicksWithSkipping -= 1;
		ticksSinceSequenceStarted_ = -baseValueFraction;
	}

	if (flags & AnimationDistributionFlags::SkipsDelayOfLastFrame) 
	{
		relevantAnimationTicksWithSkipping -= (ticksPerFrame - 1);
	}

	relevantAnimationTicksForDistribution += (skippedFramesFromPreviousAnimation_ * ticksPerFrame);
	relevantAnimationTicksForDistribution *= baseValueFraction;
	relevantAnimationTicksWithSkipping *= baseValueFraction;

	ticksSinceSequenceStarted_ += previewShownGameTickFragments;
	relevantAnimationTicksWithSkipping += previewShownGameTickFragments;

	// Calculate tick modifier for frame distribution.
	int32_t tickModifier = 0;
	if (relevantAnimationTicksWithSkipping != 0) 
	{
		tickModifier = (baseValueFraction * relevantAnimationTicksForDistribution) / relevantAnimationTicksWithSkipping;
	}

	tickModifier /= ticksPerFrame;

	// Set up calculated values for frame distribution.
	relevantFramesForDistributing_ = relevantAnimationFramesForDistributing;
	tickModifier_ = static_cast<uint16_t>(tickModifier);
}

void AnimationInfo::changeAnimationData(OptionalClxSpriteList celSprite, int8_t numberOfFrames, int8_t ticksPerFrame)
{
	if (numberOfFrames != this->numberOfFrames || ticksPerFrame != this->ticksPerFrame) 
	{
		// Ensure current frame is valid and disable Animation Distribution Logic.
		if (numberOfFrames >= 1) 
		{
			currentFrame = std::clamp<int8_t>(currentFrame, 0, numberOfFrames - 1);
		} 
		else 
		{
			currentFrame = -1;
		}

		this->numberOfFrames = numberOfFrames;
		this->ticksPerFrame = ticksPerFrame;
		ticksSinceSequenceStarted_ = 0;
		relevantFramesForDistributing_ = 0;
		tickModifier_ = 0;
	}

	this->sprites = celSprite;
}

void AnimationInfo::processAnimation(bool reverseAnimation /*= false*/)
{
	tickCounterOfCurrentFrame++;
	ticksSinceSequenceStarted_ += baseValueFraction;

	if (tickCounterOfCurrentFrame >= ticksPerFrame) 
	{
		tickCounterOfCurrentFrame = 0;
		if (reverseAnimation) 
		{
			--currentFrame;
			if (currentFrame < 0) {
				currentFrame = numberOfFrames - 1;
				ticksSinceSequenceStarted_ = 0;
			}
		} 
		else 
		{
			++currentFrame;
			if (currentFrame >= numberOfFrames) 
			{
				currentFrame = 0;
				ticksSinceSequenceStarted_ = 0;
			}
		}
	}
}

uint8_t AnimationInfo::getProgressToNextGameTick() const
{
	return isPetrified ? 0 : ProgressToNextGameTick;
}

} // namespace devilution
