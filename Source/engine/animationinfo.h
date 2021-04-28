/**
 * @file animationinfo.h
 *
 * Contains the core animation information and related logic
 */
#pragma once

#include <stdint.h>

namespace devilution {

/*
* @brief Contains the core animation information and related logic
*/
class AnimationInfo {
public:
	/*
	* @brief Pointer to Animation Data
	*/
	uint8_t *_pAnimData;
	/*
	* @brief Additional delay of each animation in the current animation
	*/
	int _pAnimDelay;
	/*
	* @brief Increases by one each game tick, counting how close we are to _pAnimDelay
	*/
	int _pAnimCnt;
	/*
	* @brief Number of frames in current animation
	*/
	int _pAnimLen;
	/*
	* @brief Current frame of animation
	*/
	int _pAnimFrame;
	/*
	* @brief Specifies how many animations-fractions are displayed between two gameticks. this can be > 0, if animations are skipped or < 0 if the same animation is shown in multiple times (delay specified).
	*/
	float _pAnimGameTickModifier;
	/*
	* @brief Number of GameTicks after the current animation sequence started
	*/
	int _pAnimGameTicksSinceSequenceStarted;
	/*
	* @brief Animation Frames that will be adjusted for the skipped Frames/GameTicks
	*/
	int _pAnimRelevantAnimationFramesForDistributing;
};

} // namespace devilution
