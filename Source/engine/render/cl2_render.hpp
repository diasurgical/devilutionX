/**
 * @file cl2_render.hpp
 *
 * CL2 rendering.
 */
#pragma once

#include <cstdint>

#include <array>
#include <utility>

#include "engine.h"
#include "engine/cel_sprite.hpp"
#include "engine/point.hpp"
#include "lighting.h"

namespace devilution {

/**
 * @brief Apply the color swaps to a CL2 sprite
 * @param p CL2 buffer
 * @param ttbl Palette translation table
 * @param numFrames Number of frames in the CL2 file
 */
void Cl2ApplyTrans(byte *p, const std::array<uint8_t, 256> &ttbl, int numFrames);

/**
 * @brief Blit CL2 sprite, to the back buffer at the given coordianates
 * @param out Output buffer
 * @param position Target buffer coordinate
 * @param cel CL2 buffer
 * @param frame CL2 frame number
 */
void Cl2Draw(const Surface &out, Point position, CelSprite cel, int frame);

/**
 * @brief Blit a solid colder shape one pixel larger than the given sprite shape, to the given buffer at the given coordinates
 * @param col Color index from current palette
 * @param out Output buffer
 * @param position Target buffer coordinate
 * @param cel CL2 buffer
 * @param frame CL2 frame number
 */
void Cl2DrawOutline(const Surface &out, uint8_t col, Point position, CelSprite cel, int frame);

/**
 * @brief Same as `Cl2DrawOutline` but considers colors with index 0 (usually shadows) to be transparent.
 *
 * @param col Color index from current palette
 * @param out Output buffer
 * @param position Target buffer coordinate
 * @param cel CL2 buffer
 * @param frame CL2 frame number
 */
void Cl2DrawOutlineSkipColorZero(const Surface &out, uint8_t col, Point position, CelSprite cel, int frame);

/**
 * @brief Blit CL2 sprite, and apply given TRN to the given buffer at the given coordinates
 * @param out Output buffer
 * @param position Target buffer coordinate
 * @param cel CL2 buffer
 * @param frame CL2 frame number
 * @param trn TRN to use
 */
void Cl2DrawTRN(const Surface &out, Point position, CelSprite cel, int frame, uint8_t *trn);

void Cl2DrawBlendedTRN(const Surface &out, Point position, CelSprite cel, int frame, uint8_t *trn);

// defined in scrollrt.cpp
extern int LightTableIndex;

/**
 * @brief Blit CL2 sprite, and apply lighting, to the given buffer at the given coordinates
 * @param out Output buffer
 * @param position Target buffer coordinate
 * @param cel CL2 buffer
 * @param frame CL2 frame number
 */
inline void Cl2DrawLight(const Surface &out, Point position, CelSprite cel, int frame)
{
	if (LightTableIndex != 0)
		Cl2DrawTRN(out, position, cel, frame, &LightTables[LightTableIndex * 256]);
	else
		Cl2Draw(out, position, cel, frame);
}

inline void Cl2DrawLightBlended(const Surface &out, Point position, CelSprite cel, int frame)
{
	Cl2DrawBlendedTRN(out, position, cel, frame, &LightTables[LightTableIndex * 256]);
}

/**
 * Returns a pair of X coordinates containing the start (inclusive) and end (exclusive)
 * of fully transparent columns in the sprite.
 */
std::pair<int, int> Cl2MeasureSolidHorizontalBounds(CelSprite cel, int frame = 0);

} // namespace devilution
