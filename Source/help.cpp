/**
 * @file help.cpp
 *
 * Implementation of the in-game help text.
 */
#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

int help_select_line;
int dword_634494;
BOOL helpflag;
int displayinghelp[22]; /* check, does nothing? */
int HelpTop;

const char gszHelpText[] = {
#ifdef SPAWN
	"Shareware Diablo Help|"
	"|"
	"$Keyboard Shortcuts:|"
	"Diablo can be played exclusively by using the mouse controls.  "
	"There are times, however, when you may want to use shortcuts to some "
	"commands by using the keyboard.  These shortcuts are listed below:|"
	"|"
	"F1:    Open the Help Screen|"
	"Esc:    Displays the main menu|"
	"Tab:    Displays the Auto-map|"
	"Space:  Removes any pop-up menus or maps from the play area|"
	"S:  Open Speedbook|"
	"B:  Open Spellbook|"
	"I:  Opens the Inventory screen|"
	"C:  Opens the Character screen|"
	"Z:  Zooms the game screen in and out|"
	"F:  Reduces the brightness of the screen|"
	"G:  Increases the brightness of the screen|"
	"Q:  Opens the Quest log (non-functional in the Shareware version)|"
	"1 - 8:  Use that item from your Belt|"
	"F5, F6, F7, F8:  Sets a hot key for a selected skill or spell|"
	"Shift + Left Click: Use any weapon without moving|"
	"|"
	"|"
	"$Movement:|"
	"Movement is controlled by the mouse.  The gauntlet on the screen is "
	"your cursor.  Use this to indicate the destination of your character "
	"and then left-click to move to that area. "
	"If you hold the mouse button down while moving, the character "
	"will continue to move in that direction.|"
	"|"
	"$Selecting Items:|"
	"What you can interact with within the game is easily identifiable. "
	"Move the cursor over any object or creature. If the object can be "
	"picked up, attacked, activated or used in any way, it will be "
	"immediately outlined. A description of the highlighted object appears "
	"in the text area on the control panel.|"
	"|"
	"Example: If you select a door and then left-click the character will "
	"walk to the door and open it. If you left-click on a highlighted "
	"weapon, the character will walk over to it and put it in his "
	"inventory. If you left-click on a highlighted creature...|"
	"|"
	"$Combat:|"
	"Combat is initiated by left-clicking on a creature that has been "
	"highlighted. If your character is equipped with a melee weapon "
	"(Sword, Mace, Ax, etc.) your character will move to range and attack. "
	"If your character is equipped with a bow, left-clicking will fire an "
	"arrow at the highlighted creature. "
	"Holding down the shift key and then left-clicking allows the "
	"character to attack without moving.|"
	"|"
	"$Picking up Objects:|"
	"If you left-click on an item - such as a weapon, shield, armor or "
	"book - your character will move to that item and add it to his "
	"inventory automatically.|"
	"|"
	"Useable items that are small in size - such as a potion or "
	"scroll - are automatically placed in your 'belt', located at the "
	"top of the Interface bar . When an item is placed in the belt, "
	"a small number appears in that box. Items may be used by either "
	"right-clicking on the item or pressing the corresponding number on "
	"the keyboard.|"
	"|"
	"If you do not have enough room in your inventory or belt for an item "
	"that you try to pick up, it will fall from your grasp. Open your "
	"inventory screen and try re-arranging or removing items to carry "
	"what you really want or need.|"
	"|"
	"$Inventory:|"
	"You can toggle the Inventory screen on and off by clicking the "
	"INV> button on the control panel.  Items may be moved around in "
	"your inventory by selecting them and then left-clicking to pick "
	"them up.  When you pick up an item while in the inventory screen, "
	"your cursor changes into the item. You can then place this item into "
	"empty spaces in your inventory, swap them with other items in your "
	"inventory or equip them.|"
	"|"
	"If you have an item that you no longer wish to carry, simply "
	"grab the item from your inventory and then left-click in the "
	"play area to drop it.|"
	"|"
	"$Equipping Items:|"
	"To equip an item, open the inventory screen and pick up the desired "
	"item, either from play or from your inventory, placing it in the "
	"appropriate box on the figure in the inventory screen. Weapons and "
	"shields go into the large spaces to the  right or left of the figure. "
	"Two-handed weapons such as bows and axes preclude the use of a "
	"shield and will take up both of these large spaces.|"
	"|"
	"Cloaks, robes, capes and all other armor must go in the central "
	"torso slot of the figure. |"
	"|"
	"Helmets and caps go in the box over the head of the character.|"
	"|"
	"Rings go into the small boxes at the hands of the figure.|"
	"|"
	"Amulets go into the small box at the next to the neck of the figure.|"
	"|"
	"To change items that your character has equipped, pick up a new "
	"item and place it on top of the item you wish to remove.  Your "
	"character will automatically swap the items and the cursor will "
	"now change into the item that was in that box.|"
	"|"
	"$Usable Items:|"
	"Potions, elixirs and books are classified as usable items.  These "
	"items can be used by right-clicking on them in the inventory screen.  "
	"Books are too large to be placed in the belt, but any potions or "
	"scrolls that are put there can also be used by pressing the "
	"corresponding number on the keyboard.|"
	"|"
	"$Gold:|"
	"You can select a specific amount of gold to drop by right "
	"clicking on a pile of gold in your inventory. "
	"A dialog will appear that allows you to select a specific amount of "
	"gold to take. When you have entered that number, your cursor will "
	"change into that amount of gold.|"
	"|"
	"$Item Information:|"
	"Many items in Diablo share certain common attributes.  These are "
	"damage, durability, charges and minimum requirements..|"
	"|"
	"Damage: This is represented by a range that indicates the minimum "
	"and maximum damage that item can inflict. A short sword has a (2-6) "
	"after its name, meaning it inflicts a minimum of two damage and a "
	"maximum of six when it hits. Damage can be modified by the quality "
	"of the weapon, the character's strength and magical effects.|"
	"|"
	"Durability: This is the amount of damage that an item can take "
	"before it is rendered useless. Durability is represented by a "
	"ratio of current durability to maximum durability. A shield that "
	"has a durability of 15/20 would still have 15 points of damage it "
	"could take from use before it was rendered useless. Maximum "
	"durability can be affected by the quality of the item, enchantments "
	"or repairs made upon the item. The minimum durability can be raised "
	"by repairing an item.|"
	"|"
	"Charges: Some items have charges associated with them. Charges "
	"indicate how many times that item can be used to cast the spell or "
	"affect indicated in its description.  Charges are represented by "
	"a ratio of charges left to maximum charges. A staff that has charges "
	"listed as 2/5 could be used to cast 2 more spells before it was "
	"rendered powerless. It could still be used to attack with as a "
	"physical weapon, however. Maximum charges can be affected by the "
	"magic or recharges cast upon the item. Minimum charges can be "
	"raised by recharging the item.|"
	"|"
	"Minimum Requirements: These are the minimum requirements that a "
	"character must meet to wield the item. The more powerful an item is, "
	"the higher the minimum requirements will be.  If a character "
	"does not meet these requirements, he will be unable to equip the "
	"item and its name and information will be displayed in red.  "
	"The item artwork will also have a red tint in the Inventory screen.|"
	"|"
	"$Items Classes:|"
	"There are three classes of items in Diablo - Mundane, "
	"Magic and Unique:|"
	"|"
	"Mundane items have no special attributes. Their information is "
	"displayed in white text.|"
	"|"
	"Magic Items are represented by blue names and text descriptions. "
	"Use the Identify spell or speak to Cain in town to determine their "
	"exact properties and attributes.|"
	"|"
	"Unique items are represented by gold names and text descriptions. "
	"Use the Identify spell or speak to Cain in town to determine their "
	"exact properties and attributes.|"
	"|"
	"$Skills & Spells:|"
	"You can access your list of skills and spells by left-clicking on "
	"the SPELLS button in the interface bar. This 'Spellbook' contains all "
	"of the skills and spells that your character knows. Spells "
	"available through staffs are also listed here. Left-clicking on "
	"the Icon of the spell you wish to ready will place it in the "
	"'select current spell' icon/area and set it as the current "
	"readied spell. A readied spell "
	"may be cast by simply right-clicking in the play area.|"
	"|"
	"Left-clicking on the 'select current spell' button will also "
	"open a 'Speedbook' menu that also allows you to ready a skill "
	"or spell for use.  To use a readied skill or spell, simply "
	"right-click in the main play area.|"
	"|"
	"Skills are the innate abilities of your character. These skills "
	"are different depending on what class you choose and require no "
	"mana to use.|"
	"|"
	"Warrior:|"
	"The Warrior has the skill of Repair Items. This allows him to fix "
	"an item that has been worn by use or is damaged in combat. "
	"To accomplish this, select the Repair Skill through the "
	"Spellbook or Speedbook and right-click the mouse as if you were "
	"casting a spell. Your cursor will change into a Hammer Icon "
	"that you will use to select the item to be repaired.  "
	"Although Repairing an item in this way will decrease the "
	"maximum durability of that item, it can be done without leaving "
	"the labyrinth.|"
	"|"
	"The Blacksmith can also repair items for a price. When the "
	"Blacksmith performs this service, it does decrease the maximum "
	"durability of the item.|"
	"|"
	"Rogue:|"
	"The Rogue has the skill of Disarm Traps. This allows her to not only "
	"remove traps, but also acts as a 'sixth sense' that warns her of "
	"where these trapped items are located. To accomplish this, select "
	"the Disarm Trap skill through the Spellbook or Speedbook and "
	"right-click the mouse as if you were casting a spell.  "
	"Your cursor will change into a Targeting Cursor that you will "
	"use to select the item to be disarmed.  The success of this "
	"attempt is based on the level of the Rogue and the expertise of "
	"whomever set the trap.|"
	"|"
	"Sorcerer:|"
	"The Sorcerer has the skill of Recharge Staffs. This allows him to "
	"focus his mana into an staff that has been drained of its magical "
	"energies.  To accomplish this, select the Recharge Staffs skill "
	"through the Spellbook or Speedbook and right-click the mouse as "
	"if you were casting a spell.  Your cursor will change into a "
	"Staff Icon that you will use to select the item to be recharged.  "
	"Although Recharging a staff in this way will decrease its maximum "
	"charges, it can be done without leaving the labyrinth.|"
	"|"
	"The Witch can also recharge staffs for a price. When the Witch "
	"performs this service, it does decrease the maximum charges of the "
	"item.|"
	"|"
	"Spells are magical effects that can be cast from a scroll, "
	"a staff or memorized from a book. Spells may or may not require "
	"mana to use and are available to all classes.|"
	"|"
	"Spells cast from a scroll cost no mana to use, but are limited "
	"to only one charge. Casting a spell from a scroll is accomplished "
	"by either right clicking on the scroll or, if it is located in "
	"our belt, pressing the corresponding number on the keyboard. "
	"Scrolls can also be readied in the Speedbook and are represented "
	"by a red icon/button in the 'select current spell' area.|"
	"|"
	"Spells cast from staffs cost no mana to use, but are limited by "
	"the number of charges available. To cast spells from a staff, "
	"it must first be equipped. The 'select current spell' icon/button "
	"will change to indicate that the spell on the staff is currently "
	"ready to cast. Scrolls can also be readied in the Spellbook or "
	"Speedbook and are represented by an orange icon/button in the "
	"'select current spell' area.|"
	"|"
	"Spells that are memorized cost mana to cast, but they can be used "
	"as long as the character has mana to power them. The Warrior "
	"and Rogue start the game with no memorized spells while "
	"the sorcerer begins with Firebolt.  If the character finds a book "
	"in the labyrinth, he can memorize the spell written in that book "
	"by opening the Inventory screen and right-clicking on the book.  "
	"This will make that spell always available to the character for "
	"casting. Memorized spells can be readied through either the "
	"Spellbook or Speedbook and are represented by a blue icon/button "
	"in the 'select current spell' area.|"
	"|"
	"$Important note on books:|"
	"Reading more than one book increases your knowledge of that spell "
	"and gives you the spell at a higher level. The higher the level "
	"of a spell the more effective it is.|"
	"|"
	"While some spells affect the caster, other spells require a target. "
	"These targeted spells are cast in the direction that you indicate "
	"with your cursor on the play area.  If you highlight a creature, "
	"you will cast that spell at that creature.  Not all items within "
	"the labyrinth can be targeted.|"
	"|"
	"Example: A fireball spell will travel at the creature or to the "
	"location you right-click on.  A Healing spell will simply add "
	"health to your character while diminishing his available mana "
	"and requires no targeting.|"
	"|"
	"You can also set a spell or scroll as a Hot Key position for "
	"instant selection.  Start by opening the pop-up menu as described "
	"in the skill section above.  Assign Hot Keys by hitting the "
	"F5, F6, F7 or F8 keys on your keyboard after scrolling through "
	"the available spells and highlighting the one you wish to assign. |"
	"|"
	"$Health and Mana:|"
	"The two orbs in the Information Bar display your life and mana. "
	"The red sphere of fluid on the left side of the control panel "
	"represents the overall health of your character. When the fluid "
	"is gone - your character is dead.|"
	"|"
	"The blue fluid on the right side of the control panel represents "
	"your character's available mana. Mana is the magical force used by "
	"your character to cast spells.  When the liquid in the sphere is "
	"low or depleted, you may be unable to cast some (or all) of your "
	"spells.|"
	"|"
	"$Information Bar:|"
	"The Information Bar is where you receive detailed information in "
	"Diablo and interact with much of your surroundings.  Here is a "
	"quick run-down of the control panel areas and their use:|"
	"|"
	"CHAR: This button is used to access your Character Statistics screen|"
	"INV:  This button is used to access your Inventory screen|"
	"Quest: This button displays your Quest Log (inactive in "
	"Shareware version)|"
	"Automap: This button activates the mapping overlay|"
	"Menu: This button activates the game menu screen|"
	"Spells: This button is used to access your Spellbook|"
	"Current Spell: This is the spell that has been readied for "
	"immediate casting|"
	"Life Orb: This is the amount of health your character currently has|"
	"Mana Orb: This is the amount of mana your character currently has|"
	"Multiplayer Message: This activates the Message Area|"
	"Description Area: This is where any important information about "
	"creatures or items you can interact with is displayed. "
	"This is also where you will enter the text you wish to send when "
	"sending multiplayer messages.|"
	"|"
	"$Character Info:|"
	"Toggle the Character Statistics Screen on and off by clicking the "
	"<CHAR button on the Information Bar.  This screen shows the "
	"'nuts and bolts' of your character.  There are four major attributes "
	"that dictate how your character progresses in the game.|"
	"|"
	"STRENGTH: This is how physically powerful your character is. This "
	"statistic plays a large part in  determining how much damage you "
	"can do when attacking creatures.|"
	"|"
	"MAGIC: This is how attuned your character is with the arcane powers "
	"of the world. This statistic plays a large part in  determining how "
	"much mana you have available for casting spells.|"
	"|"
	"DEXTERITY: This is how quick of foot and hand your character is. "
	"This statistic plays a large part in  determining the chance you "
	"have to hit creatures in combat.|"
	"|"
	"VITALITY: This is how physically fit your character is. This "
	"statistic plays a large part in  determining how much health your "
	"character has when he is undamaged.|"
	"|"
	"$In Game Menu:|"
	"To activate the Game Menu, click the MENU button on the Information "
	"Bar.  You have three options in this menu:|"
	"|"
	"New Game: This exits you from your current game and returns you to "
	"the Main Menu.|"
	"|"
	"Options: This opens the options menu that allows you to "
	"adjust your music and sound effects settings as well as "
	"the gamma level of the screen.|"
	"|"
	"Quit Diablo: This exits the program. Please note that this "
	"automatically saves your character.|"
	"|"
	"$Auto-map:|"
	"Your character automatically maps where he has been in the "
	"labyrinth.  To access the auto-map, click the MAP button on the "
	"Information Bar.  You can also press TAB on your keyboard to activate "
	"the auto-map. Zooming in and out of the map is done with the + and - "
	"keys while scrolling the map uses the arrow keys.|"
	"&"
#else
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
#endif
};

void InitHelp()
{
	helpflag = FALSE;
	dword_634494 = 0;
	displayinghelp[0] = 0;
}

void DrawHelp()
{
	int i, c, w;
	char col;
	const char *s;

	DrawSTextHelp();
	DrawQTextBack();
	PrintSString(0, 2, TRUE, "Diablo Help", COL_GOLD, 0);
	DrawSLine(5);

	s = &gszHelpText[0];

	for (i = 0; i < help_select_line; i++) {
		c = 0;
		w = 0;
		while (*s == '\0') {
			s++;
		}
		if (*s == '$') {
			s++;
		}
		if (*s == '&') {
			continue;
		}
		while (*s != '|' && w < 577) {
			while (*s == '\0') {
				s++;
			}
			tempstr[c] = *s;
			w += fontkern[fontframe[gbFontTransTbl[(BYTE)tempstr[c]]]] + 1;
			c++;
			s++;
		}
		if (w >= 577) {
			c--;
			while (tempstr[c] != ' ') {
				s--;
				c--;
			}
		}
		if (*s == '|') {
			s++;
		}
	}
	for (i = 7; i < 22; i++) {
		c = 0;
		w = 0;
		while (*s == '\0') {
			s++;
		}
		if (*s == '$') {
			s++;
			col = COL_RED;
		} else {
			col = COL_WHITE;
		}
		if (*s == '&') {
			HelpTop = help_select_line;
			continue;
		}
		while (*s != '|' && w < 577) {
			while (*s == '\0') {
				s++;
			}
			tempstr[c] = *s;
			w += fontkern[fontframe[gbFontTransTbl[(BYTE)tempstr[c]]]] + 1;
			c++;
			s++;
		}
		if (w >= 577) {
			c--;
			while (tempstr[c] != ' ') {
				s--;
				c--;
			}
		}
		if (c != 0) {
			tempstr[c] = '\0';
			DrawHelpLine(0, i, tempstr, col);
		}
		if (*s == '|') {
			s++;
		}
	}

	PrintSString(0, 23, TRUE, "Press ESC to end or the arrow keys to scroll.", COL_GOLD, 0);
}

void DrawHelpLine(int always_0, int help_line_nr, char *text, char color)
{
	int sx, sy, width;
	BYTE c;

	width = 0;
	sx = always_0 + (32 + SCREEN_X) + PANEL_LEFT;
	sy = help_line_nr * 12 + (44 + SCREEN_Y + UI_OFFSET_Y);
	while (*text) {
		c = gbFontTransTbl[(BYTE)*text];
		text++;
		c = fontframe[c];
		width += fontkern[c] + 1;
		if (c) {
			if (width <= 577)
				PrintChar(sx, sy, c, color);
		}
		sx += fontkern[c] + 1;
	}
}

void DisplayHelp()
{
	help_select_line = 0;
	helpflag = TRUE;
	HelpTop = 5000;
}

void HelpScrollUp()
{
	if (help_select_line > 0)
		help_select_line--;
}

void HelpScrollDown()
{
	if (help_select_line < HelpTop)
		help_select_line++;
}

DEVILUTION_END_NAMESPACE
