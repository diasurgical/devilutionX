/**
 * @file animationinfo.h
 *
 * Contains the core animation information and related logic
 */
#pragma once

#include <cstdint>
#include <type_traits>

#include "engine/cel_sprite.hpp"
#include "utils/stdcompat/optional.hpp"

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
	// clang-format off
	// generic getters
	[[nodiscard]] const std::optional<CelSprite>  &getCelSprite() const { return celSprite_; }
	[[nodiscard]] int getCurrentFrame()                           const { return currentFrame_; }
	[[nodiscard]] int getNumberOfFrames()                         const { return numberOfFrames_; }
	[[nodiscard]] int getTickCounterOfCurrentFrame()              const { return tickCounterOfCurrentFrame_; }
	[[nodiscard]] int getTicksPerFrame()                          const { return ticksPerFrame_; }
	[[nodiscard]] int getTriggerFrame()                           const { return triggerFrame_; }

	// helper getters
	[[nodiscard]] bool isLastFrame()         const { return (currentFrame_ == numberOfFrames_ - 1); }
	[[nodiscard]] bool isOver()              const { return (currentFrame_ >= numberOfFrames_ - 1); }
	[[nodiscard]] bool isTriggerFrame()      const { return (currentFrame_ == triggerFrame_); }
	[[nodiscard]] bool isAfterTriggerFrame() const { return (currentFrame_  > triggerFrame_); }

	// generic setters
	void setCelSprite(const std::optional<CelSprite> &celSprite) { celSprite_ = celSprite; }
	void setCurrentFrame(const int frameNumber)                  { currentFrame_ = frameNumber; }
	void setNumberOfFrames(const int numberOfFrames)             { numberOfFrames_ = numberOfFrames; }
	void setTickCounterOfCurrentFrame(const int tickCounter)     { tickCounterOfCurrentFrame_ = tickCounter; }
	void setTicksPerFrame(const int ticksPerFrame)               { ticksPerFrame_ = ticksPerFrame; } 
	void setTriggerFrame(const int triggerFrame)                 { triggerFrame_ = triggerFrame; }

	// helper setters
	void emplaceCelSprite(const OwnedCelSprite &celSprite)  { celSprite_.emplace(celSprite); }
	void setCurrentFrameAsLast()                            { currentFrame_ = numberOfFrames_ - 1; }
	void petrify()                                          { isPetrified_ = true; }
	void unpetrify()                                        { isPetrified_ = false; }
	// clang-format on

	/**
	 * @brief Calculates the Frame to use for the Animation rendering
	 * @return The Frame to use for rendering
	 */
	[[nodiscard]] int getFrameToUseForRendering() const;

	/**
	 * @brief Calculates the progress of the current animation as a fraction (0.0f to 1.0f)
	 */
	[[nodiscard]] float getAnimationProgress() const;

	/**
	 * @brief Sets the new Animation with all relevant information for rendering
	 * @param celSprite Pointer to Animation Sprite
	 * @param numberOfFrames Number of Frames in Animation
	 * @param ticksPerFrame How many game ticks are needed to advance one Animation Frame
	 * @param flags Specifies what special logics are applied to this Animation
	 * @param numSkippedFrames Number of Frames that will be skipped (for example with modifier "faster attack")
	 * @param distributeFramesBeforeFrame Distribute the numSkippedFrames only before this frame
	 * @param previewShownGameTickFragments Defines how long (in game ticks fraction) the preview animation was shown
	 */
	void setNewAnimation(std::optional<CelSprite> celSprite, int numberOfFrames, int ticksPerFrame, AnimationDistributionFlags flags = AnimationDistributionFlags::None, int numSkippedFrames = 0, int distributeFramesBeforeFrame = 0, float previewShownGameTickFragments = 0.F);

	/**
	 * @brief Changes the Animation Data on-the-fly. This is needed if a animation is currently in progress and the player changes his gear.
	 * @param celSprite Pointer to Animation Sprite
	 * @param numberOfFrames Number of Frames in Animation
	 * @param ticksPerFrame How many game ticks are needed to advance one Animation Frame
	 */
	void changeAnimationData(std::optional<CelSprite> celSprite, int numberOfFrames, int ticksPerFrame);

	/**
	 * @brief Process the Animation for a game tick (for example advances the frame)
	 * @param reverseAnimation Play the animation backwards (for example is used for "unseen" monster fading)
	 * @param dontProgressAnimation Increase TickCounterOfCurrentFrame but don't change currentFrame
	 */
	void processAnimation(bool reverseAnimation = false, bool dontProgressAnimation = false);

private:
	/**
	 * @brief Animation sprite
	 */
	std::optional<CelSprite> celSprite_;
	/**
	 * @brief Current frame of animation
	 */
	int currentFrame_;
	/**
	 * @brief Is the animation currently petrified and shouldn't advance with gfProgressToNextGameTick
	 */
	bool isPetrified_;
	/**
	 * @brief Number of frames in current animation
	 */
	int numberOfFrames_;
	/**
	 * @brief Animation Frames that will be adjusted for the skipped Frames/game ticks
	 */
	int relevantFramesForDistributing_;
	/**
	 * @brief Animation Frames that wasn't shown from previous Animation
	 */
	int skippedFramesFromPreviousAnimation_;
	/**
	 * @brief Increases by one each game tick, counting how close we are to TicksPerFrame
	 */
	int tickCounterOfCurrentFrame_;
	/**
	 * @brief Specifies how many animations-fractions are displayed between two game ticks. this can be > 0, if animations are skipped or < 0 if the same animation is shown in multiple times (delay specified).
	 */
	float tickModifier_;
	/**
	 * @brief How many game ticks are needed to advance one Animation Frame
	 */
	int ticksPerFrame_;
	/**
	 * @brief Number of game ticks after the current animation sequence started
	 */
	float ticksSinceSequenceStarted_;
	/**
	 * @brief Frame that triggers additional action
	 */
	int triggerFrame_;
	/**
	 * @brief returns the progress as a fraction (0.0f to 1.0f) in time to the next game tick or 0.0f if the animation is frozen
	 */
	[[nodiscard]] float getProgressToNextGameTick() const;
};

} // namespace devilution
