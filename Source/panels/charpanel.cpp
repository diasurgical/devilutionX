#include "panels/charpanel.hpp"

#include <fmt/format.h>
#include <string>

#include "DiabloUI/art.h"
#include "DiabloUI/art_draw.h"

#include "control.h"
#include "engine/render/text_render.hpp"

namespace devilution {

struct colorAndText {
	UiFlags color;
	std::string text;
};

struct panelEntry {
	std::string label;
	Displacement position;
	int length;
	Displacement labelOffset; // label's offset vs the beginning of the stat box
	std::function<colorAndText()> statDisplayFunc; // function responsible for displaying stat
};

auto &myPlayer = Players[MyPlayerId];

panelEntry panelEntries[] = {
	{ "", { 13, 14 }, 134, { 0, 0 },
	    [&]() { return colorAndText { UiFlags::ColorSilver, myPlayer._pName }; } },
	{ "level", { 57, 52 }, 45, { -44, 0 },
	    [&]() { return colorAndText { UiFlags::ColorSilver, fmt::format("{:d}", myPlayer._pLevel) }; } },
	{ "strength", { 88, 138 }, 33, { -44, 0 },
	    [&]() { return colorAndText { (myPlayer.GetMaximumAttributeValue(CharacterAttribute::Strength) == myPlayer._pBaseStr ? UiFlags::ColorGold : UiFlags::ColorSilver), fmt::format("{:d}", myPlayer._pBaseStr) }; } },
	{ "magic", { 88, 166 }, 33, { -44, 0 },
	    [&]() { return colorAndText { (myPlayer.GetMaximumAttributeValue(CharacterAttribute::Magic) == myPlayer._pBaseMag ? UiFlags::ColorGold : UiFlags::ColorSilver), fmt::format("{:d}", myPlayer._pBaseMag) }; } },
	{ "dexterity", { 88, 194 }, 33, { -44, 0 },
	    [&]() { return colorAndText { (myPlayer.GetMaximumAttributeValue(CharacterAttribute::Dexterity) == myPlayer._pBaseDex ? UiFlags::ColorGold : UiFlags::ColorSilver), fmt::format("{:d}", myPlayer._pBaseDex) }; } },
	{ "vitality", { 88, 222 }, 33, { -44, 0 },
	    [&]() { return colorAndText { (myPlayer.GetMaximumAttributeValue(CharacterAttribute::Vitality) == myPlayer._pBaseVit ? UiFlags::ColorGold : UiFlags::ColorSilver), fmt::format("{:d}", myPlayer._pBaseVit) }; } },

			{ "", { 88, 138 }, 33, { -44, 0 },
	    [&]() { return colorAndText { (myPlayer.GetMaximumAttributeValue(CharacterAttribute::Strength) == myPlayer._pBaseStr ? UiFlags::ColorGold : UiFlags::ColorSilver), fmt::format("{:d}", myPlayer._pBaseStr) }; } },
};

Art PanelParts[3];
Art PanelFull;

void DrawThingy(const Surface &out, Point pos, int len)
{
	DrawArt(out, pos.x, pos.y, &PanelParts[0]);
	pos.x += PanelParts[0].w();
	DrawArt(out, pos.x, pos.y, &PanelParts[1], 0, len);
	pos.x += len;
	DrawArt(out, pos.x, pos.y, &PanelParts[2]);
}

void DrawShadowString(const Surface &out, const char *text, Point pos)
{
	int width = GetLineWidth(text);
	DrawString(out, text, { pos + Displacement { -2, 2 }, { width, 12 } }, UiFlags::AlignRight | UiFlags::ColorBlack);
	DrawString(out, text, { pos + Displacement { -1, 2 }, { width, 12 } }, UiFlags::AlignRight | UiFlags::ColorBlack);
	DrawString(out, text, { pos, { width, 12 } }, UiFlags::AlignRight | UiFlags::ColorSilver);
}

void LoadCharPanel()
{
	LoadArt("debugart\\charbg.pcx", &PanelFull);
	LoadArt("debugart\\boxleftend.pcx", &PanelParts[0]);
	LoadArt("debugart\\boxmiddle.pcx", &PanelParts[1]);
	LoadArt("debugart\\boxrightend.pcx", &PanelParts[2]);

	Point pos = GetPanelPosition(UiPanels::Character, { 0, 0 });
	const Surface out(PanelFull.surface.get());

	for (auto& entry : panelEntries) {
		DrawThingy(out, pos + entry.position, entry.length);
		colorAndText tmp = entry.statDisplayFunc();
		DrawString(out, tmp.text.c_str(), { pos + entry.position + Displacement { 7, 17 }, { entry.length, 12 } }, UiFlags::AlignCenter | tmp.color);
		if (entry.label != "")
			DrawShadowString(out, entry.label.c_str(), { pos + entry.position + entry.labelOffset + Displacement { 0, 17 } });
	}

	//DrawThingy(out, { pos.x + 13, pos.y + 14 }, 134); //name

	//DrawThingy(out, { pos.x + 57, pos.y + 52 }, 45); // level
	//DrawShadowText(out, "LEVEL", { pos.x + 54, pos.y + 69 });
	/*
	DrawThingy(out, { pos.x + 88, pos.y + 138 }, 33); //str base
	DrawThingy(out, { pos.x + 88, pos.y + 166 }, 33); // magic base
	DrawThingy(out, { pos.x + 88, pos.y + 194 }, 33); //dex base
	DrawThingy(out, { pos.x + 88, pos.y + 222 }, 33); //vita base
	DrawThingy(out, { pos.x + 88, pos.y + 250 }, 33); //points to distribute
	DrawThingy(out, { pos.x + 88, pos.y + 287 }, 33); //curr life
	DrawThingy(out, { pos.x + 88, pos.y + 315 }, 33); //curr mana

	DrawThingy(out, { pos.x + 135, pos.y + 138 }, 33); // str curr
	DrawThingy(out, { pos.x + 135, pos.y + 166 }, 33); // magic curr
	DrawThingy(out, { pos.x + 135, pos.y + 194 }, 33); // dex curr
	DrawThingy(out, { pos.x + 135, pos.y + 222 }, 33); // vita curr
	DrawThingy(out, { pos.x + 135, pos.y + 287 }, 33); //max life
	DrawThingy(out, { pos.x + 135, pos.y + 315 }, 33); //max mana

	DrawThingy(out, { pos.x + 161, pos.y + 14 }, 134); // class

	DrawThingy(out, { pos.x + 208, pos.y + 52 }, 87);  // exp
	DrawThingy(out, { pos.x + 208, pos.y + 80 }, 87);  // next level
	DrawThingy(out, { pos.x + 208, pos.y + 129 }, 87); // gold

	DrawThingy(out, { pos.x + 250, pos.y + 166 }, 45); // armor
	DrawThingy(out, { pos.x + 250, pos.y + 194 }, 45); // to hit
	DrawThingy(out, { pos.x + 250, pos.y + 222 }, 45); // damage
	DrawThingy(out, { pos.x + 250, pos.y + 259 }, 45); // resist magic
	DrawThingy(out, { pos.x + 250, pos.y + 287 }, 45); // resist fire
	DrawThingy(out, { pos.x + 250, pos.y + 315 }, 45); // resist lightning
	*/
	for (auto &gfx : PanelParts) {
		gfx.Unload();
	}
}

bool CharPanelLoaded = false;

void DrawPcxChr(const Surface &out)
{
	if (!CharPanelLoaded) {
		LoadCharPanel();
		CharPanelLoaded = true;
	}
	Point pos = GetPanelPosition(UiPanels::Character, { 0, 0 });
	DrawArt(out, pos.x, pos.y, &PanelFull);
}

} // namespace devilution
