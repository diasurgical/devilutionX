#include "panels/spell_icons.hpp"

#include "engine/load_cel.hpp"
#include "engine/render/cel_render.hpp"
#include "init.h"
#include "palette.h"
#include "utils/stdcompat/optional.hpp"

namespace devilution {

namespace {
std::optional<CelSprite> pSpellCels;
uint8_t SplTransTbl[256];
} // namespace

const char SpellITbl[] = {
	27,
	1,
	2,
	3,
	4,
	5,
	6,
	7,
	8,
	9,
	28,
	13,
	12,
	18,
	16,
	14,
	18,
	19,
	11,
	20,
	15,
	21,
	23,
	24,
	25,
	22,
	26,
	29,
	37,
	38,
	39,
	42,
	41,
	40,
	10,
	36,
	30,
	51,
	51,
	50,
	46,
	47,
	43,
	45,
	48,
	49,
	44,
	35,
	35,
	35,
	35,
	35,
};

void LoadSpellIcons()
{
	if (!gbIsHellfire)
		pSpellCels = LoadCel("CtrlPan\\SpelIcon.CEL", SPLICONLENGTH);
	else
		pSpellCels = LoadCel("Data\\SpelIcon.CEL", SPLICONLENGTH);
	SetSpellTrans(RSPLTYPE_SKILL);
}

void FreeSpellIcons()
{
	pSpellCels = std::nullopt;
}

void DrawSpellCel(const Surface &out, Point position, int nCel)
{
	DrawSpellCel(out, position, *pSpellCels, nCel);
}

void DrawSpellCel(const Surface &out, Point position, const CelSprite &sprite, int nCel)
{
	CelDrawLightTo(out, position, sprite, nCel, SplTransTbl);
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
