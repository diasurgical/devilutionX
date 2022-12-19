/**
 * @file animationinfo.h
 *
 * Contains the core animation information and related logic
 */
#pragma once

#include <cstdint>
#include <type_traits>

#include "engine/clx_sprite.hpp"

namespace devilution {

/**
 * @brief Specifies what special logics are applied for a Animation
 */
enum AnimationDistributionFlags : uint8_t {
	None = 0,
	/**
	 * @brief processAnimation will be called after setNewAnimation (in same game tick as NewPlrAnim)
	 */
	ProcessAnimationPending = 1 << 0,
	/**
	 * @brief Delay of last Frame is ignored (for example, because only Frame and not delay is checked in game_logic)
	 */
	SkipsDelayOfLastFrame = 1 << 1,
	/**
	 * @brief Repeated Animation (for example same player melee attack, that can be repeated directly after hit frame and doesn't need to show all animation frames)
	 */
	RepeatedAction = 1 << 2,
};

/**
 * @brief Contains the core animation information and related logic
 */
class AnimationInfo {
public:
	/**
	 * @brief Animation sprite
	 */
	OptionalClxSpriteList sprites;
	/**
	 * @brief How many game ticks are needed to advance one Animation Frame
	 */
	int8_t ticksPerFrame;
	/**
	 * @brief Increases by one each game tick, counting how close we are to ticksPerFrame
	 */
	int8_t tickCounterOfCurrentFrame;
	/**
	 * @brief Number of frames in current animation
	 */
	int8_t numberOfFrames;
	/**
	 * @brief Current frame of animation
	 */
	int8_t currentFrame;
	/**
	 * @brief Is the animation currently petrified and shouldn't advance with gfProgressToNextGameTick
	 */
	bool isPetrified;

	[[nodiscard]] ClxSprite currentSprite() const
	{
		return (*sprites)[getFrameToUseForRendering()];
	}

	[[nodiscard]] bool isLastFrame() const
	{
		return currentFrame >= (numberOfFrames - 1);
	}

	/**
	 * @brief Calculates the Frame to use for the Animation rendering
	 * @return The Frame to use for rendering
	 */
	[[nodiscard]] int8_t getFrameToUseForRendering() const;

	/**
	 * @brief Calculates the progress of the current animation as a fraction (see baseValueFraction)
	 */
	[[nodiscard]] uint8_t getAnimationProgress() const;

	/**
	 * @brief Sets the new Animation with all relevant information for rendering
	 * @param sprites Animation sprites
	 * @param numberOfFrames Number of Frames in Animation
	 * @param ticksPerFrame How many game ticks are needed to advance one Animation Frame
	 * @param flags Specifies what special logics are applied to this Animation
	 * @param numSkippedFrames Number of Frames that will be skipped (for example with modifier "faster attack")
	 * @param distributeFramesBeforeFrame Distribute the numSkippedFrames only before this frame
	 * @param previewShownGameTickFragments Defines how long (in game ticks fraction) the preview animation was shown
	 */
	void setNewAnimation(OptionalClxSpriteList sprites, int8_t numberOfFrames, int8_t ticksPerFrame, AnimationDistributionFlags flags = AnimationDistributionFlags::None, int8_t numSkippedFrames = 0, int8_t distributeFramesBeforeFrame = 0, uint8_t previewShownGameTickFragments = 0);

	/**
	 * @brief Changes the Animation Data on-the-fly. This is needed if a animation is currently in progress and the player changes his gear.
	 * @param sprites Animation sprites
	 * @param numberOfFrames Number of Frames in Animation
	 * @param ticksPerFrame How many game ticks are needed to advance one Animation Frame
	 */
	void changeAnimationData(OptionalClxSpriteList sprites, int8_t numberOfFrames, int8_t ticksPerFrame);

	/**
	 * @brief Process the Animation for a game tick (for example advances the frame)
	 * @param reverseAnimation Play the animation backwards (for example is used for "unseen" monster fading)
	 */
	void processAnimation(bool reverseAnimation = false);

	/**
	 * @brief Fractions in AnimationInfo are stored as fixed point (baseValueFraction/128 correspondents to 1/100%).
	 */
	constexpr static uint8_t baseValueFraction = 128;

private:
	/**
	 * @brief returns the progress as a fraction in time to the next game tick or no progress if the animation is frozen (see baseValueFraction)
	 */
	[[nodiscard]] uint8_t getProgressToNextGameTick() const;

	/**
	 * @brief Animation Frames that will be adjusted for the skipped Frames/game ticks
	 */
	int8_t relevantFramesForDistributing_;
	/**
	 * @brief Animation Frames that wasn't shown from previous Animation
	 */
	int8_t skippedFramesFromPreviousAnimation_;
	/**
	 * @brief Specifies how many animations-fractions (see baseValueFraction) are displayed between two game ticks. this can be more than one frame, if animations are skipped or less than one frame if the same animation is shown in multiple times (delay specified).
	 */
	uint16_t tickModifier_;
	/**
	 * @brief Number of game ticks after the current animation sequence started
	 */
	int16_t ticksSinceSequenceStarted_;
};

} // namespace devilution
