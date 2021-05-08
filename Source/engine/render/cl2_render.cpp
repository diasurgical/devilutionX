/**
 * @file cl2_render.cpp
 *
 * CL2 rendering.
 */
#include "cl2_render.hpp"

#include "engine/render/common_impl.h"
#include "scrollrt.h"

namespace devilution {
namespace {

constexpr std::uint8_t MaxCl2Width = 65;

/**
 * @brief Blit CL2 sprite to the given buffer
 * @param out Target buffer
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param pRLEBytes CL2 pixel stream (run-length encoded)
 * @param nDataSize Size of CL2 in bytes
 * @param nWidth Width of sprite
 */
void Cl2BlitSafe(const CelOutputBuffer &out, int sx, int sy, const byte *pRLEBytes, int nDataSize, int nWidth)
{
	const byte *src = pRLEBytes;
	BYTE *dst = out.at(sx, sy);
	int w = nWidth;

	while (nDataSize > 0) {
		auto width = static_cast<std::int8_t>(*src++);
		nDataSize--;
		if (width < 0) {
			width = -width;
			if (width > MaxCl2Width) {
				width -= MaxCl2Width;
				nDataSize--;
				const auto fill = static_cast<std::uint8_t>(*src++);
				if (dst < out.end() && dst > out.begin()) {
					w -= width;
					while (width > 0) {
						*dst = fill;
						dst++;
						width--;
					}
					if (w == 0) {
						w = nWidth;
						dst -= out.pitch() + w;
					}
					continue;
				}
			} else {
				nDataSize -= width;
				if (dst < out.end() && dst > out.begin()) {
					w -= width;
					while (width > 0) {
						*dst = static_cast<std::uint8_t>(*src);
						src++;
						dst++;
						width--;
					}
					if (w == 0) {
						w = nWidth;
						dst -= out.pitch() + w;
					}
					continue;
				}
				src += width;
			}
		}
		while (width > 0) {
			if (width > w) {
				dst += w;
				width -= w;
				w = 0;
			} else {
				dst += width;
				w -= width;
				width = 0;
			}
			if (w == 0) {
				w = nWidth;
				dst -= out.pitch() + w;
			}
		}
	}
}

/**
 * @brief Blit a solid colder shape one pixel larger then the given sprite shape, to the given buffer
 * @param out Target buffer
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param pRLEBytes CL2 pixel stream (run-length encoded)
 * @param nDataSize Size of CL2 in bytes
 * @param nWidth Width of sprite
 * @param col Color index from current palette
 */
void Cl2BlitOutlineSafe(const CelOutputBuffer &out, int sx, int sy, const byte *pRLEBytes, int nDataSize, int nWidth, uint8_t col)
{
	const byte *src = pRLEBytes;
	BYTE *dst = out.at(sx, sy);
	int w = nWidth;

	while (nDataSize > 0) {
		auto width = static_cast<std::int8_t>(*src++);
		nDataSize--;
		if (width < 0) {
			width = -width;
			if (width > MaxCl2Width) {
				width -= MaxCl2Width;
				nDataSize--;
				if (static_cast<std::uint8_t>(*src++) != 0 && dst < out.end() && dst > out.begin()) {
					w -= width;
					dst[-1] = col;
					dst[width] = col;
					while (width > 0) {
						dst[-out.pitch()] = col;
						dst[out.pitch()] = col;
						dst++;
						width--;
					}
					if (w == 0) {
						w = nWidth;
						dst -= out.pitch() + w;
					}
					continue;
				}
			} else {
				nDataSize -= width;
				if (dst < out.end() && dst > out.begin()) {
					w -= width;
					while (width > 0) {
						if (static_cast<std::uint8_t>(*src) != 0) {
							dst[-1] = col;
							dst[1] = col;
							dst[-out.pitch()] = col;
							// BUGFIX: only set `if (dst+out.pitch() < out.end())`
							dst[out.pitch()] = col;
						}
						src++;
						dst++;
						width--;
					}
					if (w == 0) {
						w = nWidth;
						dst -= out.pitch() + w;
					}
					continue;
				}
				src += width;
			}
		}
		while (width > 0) {
			if (width > w) {
				dst += w;
				width -= w;
				w = 0;
			} else {
				dst += width;
				w -= width;
				width = 0;
			}
			if (w == 0) {
				w = nWidth;
				dst -= out.pitch() + w;
			}
		}
	}
}

/**
 * @brief Blit CL2 sprite, and apply lighting, to the given buffer
 * @param out Target buffer
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param pRLEBytes CL2 pixel stream (run-length encoded)
 * @param nDataSize Size of CL2 in bytes
 * @param nWidth With of CL2 sprite
 * @param pTable Light color table
 */
void Cl2BlitLightSafe(const CelOutputBuffer &out, int sx, int sy, const byte *pRLEBytes, int nDataSize, int nWidth, uint8_t *pTable)
{
	const byte *src = pRLEBytes;
	BYTE *dst = out.at(sx, sy);
	int w = nWidth;

	while (nDataSize > 0) {
		auto width = static_cast<std::int8_t>(*src++);
		nDataSize--;
		if (width < 0) {
			width = -width;
			if (width > MaxCl2Width) {
				width -= MaxCl2Width;
				nDataSize--;
				const uint8_t fill = pTable[static_cast<std::uint8_t>(*src++)];
				if (dst < out.end() && dst > out.begin()) {
					w -= width;
					while (width > 0) {
						*dst = fill;
						dst++;
						width--;
					}
					if (w == 0) {
						w = nWidth;
						dst -= out.pitch() + w;
					}
					continue;
				}
			} else {
				nDataSize -= width;
				if (dst < out.end() && dst > out.begin()) {
					w -= width;
					while (width > 0) {
						*dst = pTable[static_cast<std::uint8_t>(*src)];
						src++;
						dst++;
						width--;
					}
					if (w == 0) {
						w = nWidth;
						dst -= out.pitch() + w;
					}
					continue;
				}
				src += width;
			}
		}
		while (width > 0) {
			if (width > w) {
				dst += w;
				width -= w;
				w = 0;
			} else {
				dst += width;
				w -= width;
				width = 0;
			}
			if (w == 0) {
				w = nWidth;
				dst -= out.pitch() + w;
			}
		}
	}
}

} // namespace

void Cl2ApplyTrans(byte *p, const std::array<uint8_t, 256> &ttbl, int nCel)
{
	assert(p != nullptr);

	for (int i = 1; i <= nCel; i++) {
		int nDataSize;
		byte *dst = CelGetFrame(p, i, &nDataSize) + 10;
		nDataSize -= 10;
		while (nDataSize > 0) {
			auto width = static_cast<std::int8_t>(*dst++);
			nDataSize--;
			assert(nDataSize >= 0);
			if (width < 0) {
				width = -width;
				if (width > MaxCl2Width) {
					nDataSize--;
					assert(nDataSize >= 0);
					*dst = static_cast<byte>(ttbl[static_cast<std::uint8_t>(*dst)]);
					dst++;
				} else {
					nDataSize -= width;
					assert(nDataSize >= 0);
					for (; width > 0; width--) {
						*dst = static_cast<byte>(ttbl[static_cast<std::uint8_t>(*dst)]);
						dst++;
					}
				}
			}
		}
	}
}

void Cl2Draw(const CelOutputBuffer &out, int sx, int sy, const CelSprite &cel, int frame)
{
	assert(frame > 0);

	int nDataSize;
	const byte *pRLEBytes = CelGetFrameClipped(cel.Data(), frame, &nDataSize);

	Cl2BlitSafe(out, sx, sy, pRLEBytes, nDataSize, cel.Width(frame));
}

void Cl2DrawOutline(const CelOutputBuffer &out, uint8_t col, int sx, int sy, const CelSprite &cel, int frame)
{
	assert(frame > 0);

	int nDataSize;
	const byte *pRLEBytes = CelGetFrameClipped(cel.Data(), frame, &nDataSize);

	const CelOutputBuffer &sub = out.subregionY(0, out.h() - 1);
	Cl2BlitOutlineSafe(sub, sx, sy, pRLEBytes, nDataSize, cel.Width(frame), col);
}

void Cl2DrawLightTbl(const CelOutputBuffer &out, int sx, int sy, const CelSprite &cel, int frame, char light)
{
	assert(frame > 0);

	int nDataSize;
	const byte *pRLEBytes = CelGetFrameClipped(cel.Data(), frame, &nDataSize);
	Cl2BlitLightSafe(out, sx, sy, pRLEBytes, nDataSize, cel.Width(frame), GetLightTable(light));
}

void Cl2DrawLight(const CelOutputBuffer &out, int sx, int sy, const CelSprite &cel, int frame)
{
	assert(frame > 0);

	int nDataSize;
	const byte *pRLEBytes = CelGetFrameClipped(cel.Data(), frame, &nDataSize);

	if (light_table_index != 0)
		Cl2BlitLightSafe(out, sx, sy, pRLEBytes, nDataSize, cel.Width(frame), &pLightTbl[light_table_index * 256]);
	else
		Cl2BlitSafe(out, sx, sy, pRLEBytes, nDataSize, cel.Width(frame));
}

} // namespace devilution
