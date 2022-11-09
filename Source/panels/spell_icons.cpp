#include "panels/spell_icons.hpp"

#include "engine/load_cel.hpp"
#include "engine/load_clx.hpp"
#include "engine/palette.h"
#include "engine/render/clx_render.hpp"
#include "init.h"
#include "utils/stdcompat/optional.hpp"

namespace devilution {

namespace {
#ifdef UNPACKED_MPQS
OptionalOwnedClxSpriteList pSpellIconsBackground;
OptionalOwnedClxSpriteList pSpellIconsForeground;
#else
OptionalOwnedClxSpriteList pSpellCels;
#endif

uint8_t SplTransTbl[256];
} // namespace

const char SpellITbl[] = {
	26,
	0,
	1,
	2,
	3,
	4,
	5,
	6,
	7,
	8,
	27,
	12,
	11,
	17,
	15,
	13,
	17,
	18,
	10,
	19,
	14,
	20,
	22,
	23,
	24,
	21,
	25,
	28,
	36,
	37,
	38,
	41,
	40,
	39,
	9,
	35,
	29,
	50,
	50,
	49,
	45,
	46,
	42,
	44,
	47,
	48,
	43,
	34,
	34,
	34,
	34,
	34,
};

void LoadSpellIcons()
{
	if (!gbIsHellfire) {
#ifdef UNPACKED_MPQS
		pSpellIconsForeground = LoadClx("ctrlpan\\spelicon_fg.clx");
		pSpellIconsBackground = LoadClx("ctrlpan\\spelicon_bg.clx");
#else
		pSpellCels = LoadCel("ctrlpan\\spelicon", SPLICONLENGTH);
#endif
	} else {
#ifdef UNPACKED_MPQS
		pSpellIconsForeground = LoadClx("data\\spelicon_fg.clx");
		pSpellIconsBackground = LoadClx("data\\spelicon_bg.clx");
#else
		pSpellCels = LoadCel("data\\spelicon", SPLICONLENGTH);
#endif
	}
	SetSpellTrans(RSPLTYPE_SKILL);
}

void FreeSpellIcons()
{
#ifdef UNPACKED_MPQS
	pSpellIconsBackground = std::nullopt;
	pSpellIconsForeground = std::nullopt;
#else
	pSpellCels = std::nullopt;
#endif
}

void DrawSpellCel(const Surface &out, Point position, int nCel)
{
#ifdef UNPACKED_MPQS
	DrawSpellCel(out, position, (*pSpellIconsForeground)[nCel], (*pSpellIconsBackground)[0]);
#else
	DrawSpellCel(out, position, (*pSpellCels)[nCel]);
#endif
}

#ifdef UNPACKED_MPQS
void DrawSpellCel(const Surface &out, Point position, ClxSprite sprite, ClxSprite background)
{
	ClxDrawTRN(out, position, background, SplTransTbl);
	ClxDrawTRN(out, position, sprite, SplTransTbl);
}
#else
void DrawSpellCel(const Surface &out, Point position, ClxSprite sprite)
{
	ClxDrawTRN(out, position, sprite, SplTransTbl);
}
#endif

void DrawSpellBorder(const Surface &out, Point position, ClxSprite sprite)
{
	const uint8_t color = SplTransTbl[PAL8_YELLOW + 2];
	const size_t width = sprite.width();
	const size_t height = sprite.height();
	position.y -= static_cast<int>(height);
	uint8_t *buf = &out[position];
	std::memset(buf, color, width);
	buf += out.pitch();
	std::memset(buf, color, width);
	for (size_t i = 4; i < sprite.height(); ++i) {
		buf[0] = buf[1] = color;
		buf[width - 2] = buf[width - 1] = color;
		buf += out.pitch();
	}
	std::memset(buf, color, width);
	buf += out.pitch();
	std::memset(buf, color, width);
}

void SetSpellTrans(spell_type t)
{
	if (t == RSPLTYPE_SKILL) {
		for (int i = 0; i < 128; i++)
			SplTransTbl[i] = i;
	}
	for (int i = 128; i < 256; i++)
		SplTransTbl[i] = i;
	SplTransTbl[255] = 0;

	switch (t) {
	case RSPLTYPE_SPELL:
		SplTransTbl[PAL8_YELLOW] = PAL16_BLUE + 1;
		SplTransTbl[PAL8_YELLOW + 1] = PAL16_BLUE + 3;
		SplTransTbl[PAL8_YELLOW + 2] = PAL16_BLUE + 5;
		for (int i = PAL16_BLUE; i < PAL16_BLUE + 16; i++) {
			SplTransTbl[PAL16_BEIGE - PAL16_BLUE + i] = i;
			SplTransTbl[PAL16_YELLOW - PAL16_BLUE + i] = i;
			SplTransTbl[PAL16_ORANGE - PAL16_BLUE + i] = i;
		}
		break;
	case RSPLTYPE_SCROLL:
		SplTransTbl[PAL8_YELLOW] = PAL16_BEIGE + 1;
		SplTransTbl[PAL8_YELLOW + 1] = PAL16_BEIGE + 3;
		SplTransTbl[PAL8_YELLOW + 2] = PAL16_BEIGE + 5;
		for (int i = PAL16_BEIGE; i < PAL16_BEIGE + 16; i++) {
			SplTransTbl[PAL16_YELLOW - PAL16_BEIGE + i] = i;
			SplTransTbl[PAL16_ORANGE - PAL16_BEIGE + i] = i;
		}
		break;
	case RSPLTYPE_CHARGES:
		SplTransTbl[PAL8_YELLOW] = PAL16_ORANGE + 1;
		SplTransTbl[PAL8_YELLOW + 1] = PAL16_ORANGE + 3;
		SplTransTbl[PAL8_YELLOW + 2] = PAL16_ORANGE + 5;
		for (int i = PAL16_ORANGE; i < PAL16_ORANGE + 16; i++) {
			SplTransTbl[PAL16_BEIGE - PAL16_ORANGE + i] = i;
			SplTransTbl[PAL16_YELLOW - PAL16_ORANGE + i] = i;
		}
		break;
	case RSPLTYPE_INVALID:
		SplTransTbl[PAL8_YELLOW] = PAL16_GRAY + 1;
		SplTransTbl[PAL8_YELLOW + 1] = PAL16_GRAY + 3;
		SplTransTbl[PAL8_YELLOW + 2] = PAL16_GRAY + 5;
		for (int i = PAL16_GRAY; i < PAL16_GRAY + 15; i++) {
			SplTransTbl[PAL16_BEIGE - PAL16_GRAY + i] = i;
			SplTransTbl[PAL16_YELLOW - PAL16_GRAY + i] = i;
			SplTransTbl[PAL16_ORANGE - PAL16_GRAY + i] = i;
		}
		SplTransTbl[PAL16_BEIGE + 15] = 0;
		SplTransTbl[PAL16_YELLOW + 15] = 0;
		SplTransTbl[PAL16_ORANGE + 15] = 0;
		break;
	case RSPLTYPE_SKILL:
		break;
	}
}

} // namespace devilution
