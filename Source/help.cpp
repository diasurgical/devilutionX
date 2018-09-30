//HEADER_GOES_HERE

#include "../types.h"

#ifndef NO_GLOBALS
int help_select_line; // weak
int dword_634494; // weak
int helpflag;
int displayinghelp[22]; /* check, does nothing? */
int HelpTop; // weak
#endif

const char gszHelpText[] =
{
	"$Keyboard Shortcuts:|"
	"F1:    Open Help Screen|"
	"Esc:   Display Main Menu|"
	"Tab:   Display Auto-map|"
	"Space: Hide all info screens|"
	"S: Open Speedbook|"
	"B: Open Spellbook|"
	"I: Open Inventory screen|"
	"C: Open Character screen|"
	"Q: Open Quest log|"
	"F: Reduce screen brightness|"
	"G: Increase screen brightness|"
	"Z: Zoom Game Screen|"
	"+ / -: Zoom Automap|"
	"1 - 8: Use Belt item|"
	"F5, F6, F7, F8:     Set hot key for skill or spell|"
	"Shift + Left Click: Attack without moving|"
	"|"
	"$Movement:|"
	"If you hold the mouse button down while moving, the character "
	"will continue to move in that direction.|"
	"|"
	"$Combat:|"
	"Holding down the shift key and then left-clicking allows the "
	"character to attack without moving.|"
	"|"
	"$Auto-map:|"
	"To access the auto-map, click the 'MAP' button on the "
	"Information Bar or press 'TAB' on the keyboard. Zooming in and "
	"out of the map is done with the + and - keys. Scrolling the map "
	"uses the arrow keys.|"
	"|"
	"$Picking up Objects:|"
	"Useable items that are small in size, such as potions or scrolls, "
	"are automatically placed in your 'belt' located at the top of "
	"the Interface bar . When an item is placed in the belt, a small "
	"number appears in that box. Items may be used by either pressing "
	"the corresponding number or right-clicking on the item.|"
	"|"
	"$Gold|"
	"You can select a specific amount of gold to drop by right "
	"clicking on a pile of gold in your inventory.|"
	"|"
	"$Skills & Spells:|"
	"You can access your list of skills and spells by left-clicking on "
	"the 'SPELLS' button in the interface bar. Memorized spells and "
	"those available through staffs are listed here. Left-clicking on "
	"the spell you wish to cast will ready the spell. A readied spell "
	"may be cast by simply right-clicking in the play area.|"
	"|"
	"$Using the Speedbook for Spells|"
	"Left-clicking on the 'readied spell' button will open the 'Speedbook' "
	"which allows you to select a skill or spell for immediate use.  "
	"To use a readied skill or spell, simply right-click in the main play "
	"area.|"
	"|"
	"$Setting Spell Hotkeys|"
	"You can assign up to four Hot Keys for skills, spells or scrolls.  "
	"Start by opening the 'speedbook' as described in the section above. "
	"Press the F5, F6, F7 or F8 keys after highlighting the spell you "
	"wish to assign.|"
	"|"
	"$Spell Books|"
	"Reading more than one book increases your knowledge of that "
	"spell, allowing you to cast the spell more effectively.|"
	"&"
};

void __cdecl InitHelp()
{
	helpflag = 0;
	dword_634494 = 0;
	displayinghelp[0] = 0;
}
// 634494: using guessed type int dword_634494;

void __cdecl DrawHelp()
{
	int v0; // edi
	const char *v1; // esi
	int v2; // edx
	signed int v3; // ecx
	char v4; // al
	unsigned char v5; // al
	_BYTE *i; // eax
	int v7; // eax
	signed int v8; // edx
	char v9; // cl
	unsigned char v10; // cl
	text_color color; // [esp+Ch] [ebp-8h]
	int help_line_nr; // [esp+10h] [ebp-4h]
	signed int help_line_nra; // [esp+10h] [ebp-4h]

	DrawSTextHelp();
	DrawQTextBack();
	PrintSString(0, 2, 1u, "Diablo Help", COL_GOLD, 0);
	DrawSLine(5);
	v0 = help_select_line;
	v1 = gszHelpText;
	if ( help_select_line > 0 )
	{
		help_line_nr = help_select_line;
		do
		{
			v2 = 0;
			v3 = 0;
			while ( !*v1 )
				++v1;
			if ( *v1 == '$' )
				++v1;
			v4 = *v1;
			if ( *v1 != '&' )
			{
				if ( v4 == ('|') )
					goto LABEL_47;
				while ( v3 < 577 )
				{
					if ( !v4 )
					{
						do
							++v1;
						while ( !*v1 );
					}
					v5 = *v1;
					tempstr[v2++] = *v1++;
					v3 += fontkern[fontframe[fontidx[v5]]] + 1;
					v4 = *v1;
					if ( *v1 == ('|') )
					{
						if ( v3 < 577 )
							goto LABEL_18;
						break;
					}
				}
				for ( i = (unsigned char *)&tempstr[v2]-1; *i != ' '; --i )
					--v1;
LABEL_18:
				if ( *v1 == ('|') )
LABEL_47:
					++v1;
			}
			--help_line_nr;
		}
		while ( help_line_nr );
	}
	help_line_nra = 7;
	do
	{
		v7 = 0;
		v8 = 0;
		while ( !*v1 )
			++v1;
		if ( *v1 == '$' )
		{
			++v1;
			_LOBYTE(color) = COL_RED;
		}
		else
		{
			_LOBYTE(color) = COL_WHITE;
		}
		v9 = *v1;
		if ( *v1 == '&' )
		{
			HelpTop = v0;
		}
		else
		{
			if ( v9 == ('|') )
				goto LABEL_48;
			while ( v8 < 577 )
			{
				if ( !v9 )
				{
					do
						++v1;
					while ( !*v1 );
				}
				v10 = *v1;
				tempstr[v7++] = *v1++;
				v8 += fontkern[fontframe[fontidx[v10]]] + 1;
				v9 = *v1;
				if ( *v1 == ('|') )
				{
					if ( v8 < 577 )
						goto LABEL_39;
					break;
				}
			}
			while ( tempstr[--v7] != ' ' )
				--v1;
LABEL_39:
			if ( v7 )
			{
				tempstr[v7] = 0;
				DrawHelpLine(0, help_line_nra, tempstr, color);
				v0 = help_select_line;
			}
			if ( *v1 == ('|') )
LABEL_48:
				++v1;
		}
		++help_line_nra;
	}
	while ( help_line_nra < 22 );
	PrintSString(0, 23, 1u, "Press ESC to end or the arrow keys to scroll.", COL_GOLD, 0);
}
// 634490: using guessed type int help_select_line;
// 634960: using guessed type int HelpTop;

void __fastcall DrawHelpLine(int always_0, int help_line_nr, char *text, text_color color)
{
	signed int v4; // ebx
	int v5; // edi
	unsigned char i; // al
	unsigned char v7; // al
	int v8; // esi

	v4 = 0;
	v5 = screen_y_times_768[SStringY[help_line_nr] + 204] + always_0 + 96;
	for ( i = *text; *text; i = *text )
	{
		++text;
		v7 = fontframe[fontidx[i]];
		v8 = v7;
		v4 += fontkern[v7] + 1;
		if ( v7 )
		{
			if ( v4 <= 577 )
				CPrintString(v5, v7, color);
		}
		v5 += fontkern[v8] + 1;
	}
}

void __cdecl DisplayHelp()
{
	help_select_line = 0;
	helpflag = 1;
	HelpTop = 5000;
}
// 634490: using guessed type int help_select_line;
// 634960: using guessed type int HelpTop;

void __cdecl HelpScrollUp()
{
	if ( help_select_line > 0 )
		--help_select_line;
}
// 634490: using guessed type int help_select_line;

void __cdecl HelpScrollDown()
{
	if ( help_select_line < HelpTop )
		++help_select_line;
}
// 634490: using guessed type int help_select_line;
// 634960: using guessed type int HelpTop;
