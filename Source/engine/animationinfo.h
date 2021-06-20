/**
 * @file animationinfo.h
 *
 * Contains the core animation information and related logic
 */
#pragma once

#include <stdint.h>
#include <type_traits>

#include "engine/cel_sprite.hpp"

namespace devilution {

/**
 * @brief Specifies what special logics are applied for a Animation
 */
enum AnimationDistributionFlags : uint8_t {
	None = 0,
	/**
	 * @brief ProcessAnimation will be called after SetNewAnimation (in same game tick as NewPlrAnim)
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

/*
* @brief Contains the core animation information and related logic
*/
class AnimationInfo {
public:
	/**
	 * @brief Pointer to Animation Sprite
	 */
	CelSprite *pCelSprite;
	/**
	 * @brief Additional delay of each animation in the current animation
	 */
	int DelayLen;
	/**
	 * @brief Increases by one each game tick, counting how close we are to DelayLen
	 */
	int DelayCounter;
	/**
	 * @brief Number of frames in current animation
	 */
	int NumberOfFrames;
	/**
	 * @brief Current frame of animation
	 */
	int CurrentFrame;

	/**
	 * @brief Calculates the Frame to use for the Animation rendering
	 * @return The Frame to use for rendering
	 */
	int GetFrameToUseForRendering() const;

	/**
	 * @brief Calculates the progress of the current animation as a fraction (0.0f to 1.0f)
	*/
	float GetAnimationProgress() const;

	/**
	 * @brief Sets the new Animation with all relevant information for rendering
	 * @param pCelSprite Pointer to Animation Sprite
	 * @param numberOfFrames Number of Frames in Animation
	 * @param delayLen Delay after each Animation sequence
	 * @param flags Specifies what special logics are applied to this Animation
	 * @param numSkippedFrames Number of Frames that will be skipped (for example with modifier "faster attack")
	 * @param distributeFramesBeforeFrame Distribute the numSkippedFrames only before this frame
	 */
	void SetNewAnimation(CelSprite *pCelSprite, int numberOfFrames, int delayLen, AnimationDistributionFlags flags = AnimationDistributionFlags::None, int numSkippedFrames = 0, int distributeFramesBeforeFrame = 0);

	/**
	 * @brief Changes the Animation Data on-the-fly. This is needed if a animation is currently in progress and the player changes his gear.
	 * @param pCelSprite Pointer to Animation Sprite
	 * @param numberOfFrames Number of Frames in Animation
	 * @param delayLen Delay after each Animation sequence
	 */
	void ChangeAnimationData(CelSprite *pCelSprite, int numberOfFrames, int delayLen);

	/*
	 * @brief Process the Animation for a game tick (for example advances the frame)
	 * @param reverseAnimation Play the animation backwards (for example is used for "unseen" monster fading)
	 * @param dontProgressAnimation Increase DelayCounter but don't change CurrentFrame
	 */
	void ProcessAnimation(bool reverseAnimation = false, bool dontProgressAnimation = false);

private:
	/**
	 * @brief Specifies how many animations-fractions are displayed between two game ticks. this can be > 0, if animations are skipped or < 0 if the same animation is shown in multiple times (delay specified).
	 */
	float TickModifier;
	/**
	 * @brief Number of game ticks after the current animation sequence started
	 */
	int TicksSinceSequenceStarted;
	/**
	 * @brief Animation Frames that will be adjusted for the skipped Frames/game ticks
	 */
	int RelevantFramesForDistributing;
	/**
	 * @brief Animation Frames that wasn't shown from previous Animation
	 */
	int SkippedFramesFromPreviousAnimation;
};

} // namespace devilution
