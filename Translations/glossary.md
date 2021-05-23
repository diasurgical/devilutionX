# Translations glossary

## Variables

There are two types of variables in the text files: %s, %i. All of these will add something within the single string, here are some examples. 

`%s` will always be another word. Example: `%s of %s` refers to an created Item which includes affixes. `Tin Cap of the Fool` would be such an item.
`%i` will always be a number. Example: Fire hit damage: %i-%i` will indicate how much fire damage a weapon will deal

**IMPORTANT**: You have to keep those intact and in the same order. 

### Example

Working:

	`%s' (level %i) just joined the game`
changed to
	`%s' (level %i) is now with you.`

Not working:

	`%s' (level %i) just joined the game`
changed to
	`A level %i Character named % s' is now with you.`

## %s of %s

The rather tricky line `%s of %s` is used to create items with pre- and suffixes as mentioned in aboves examples. If your language has gendered articles it might be advisable to remove the "of" but add a gendered article to each suffix instead. If you do so the line should be changed to `%s %s` and you will have to add the appropriate article to each suffix.

## Adding lines

Unfortunately it is not possible to add new lines for languages outside of quest texts.
	
## Names

Please try to keep original names intact, however it's up to your judgment to change something that sounds completely wrong in the language you are translating to.

## Coherence

Make sure that you use the same terminology where it's applicable. 

Example:

Good:
	`fully recover life`
	`recover life`
	`fully recover mana`
	`recover mana`

Bad:
	`recover all life points`
	`recover partial life`
	`fully recover mana`
	`recover mana`

## References

Don't worry – we all forget details. Here's a list of resources you can use if you are unsure about a thing.

### Creatures
* Diablo Creatures - [Diablo Archive](https://diablo-archive.fandom.com/wiki/Monsters_(Diablo_I))
* Diablo Unique Creatures - [Diablo Archive](https://diablo-archive.fandom.com/wiki/Category:Diablo_I_Unique_Monsters)
* Hellfire Creatures - [Diablo Wiki](https://diablo.fandom.com/wiki/Hellfire_Bestiary)


### Items and Equipment References and Pictures
* Diablo Unique Weapons - [Diablo Archive](https://diablo-archive.fandom.com/wiki/Unique_Weapons_(Diablo_I))
* Diablo Normal Weapons - [Diablo Archive](https://diablo-archive.fandom.com/wiki/Normal_Weapons_(Diablo_I))
* Diablo and Hellfire Unique Armor - [Diablo Wiki](https://diablo.fandom.com/wiki/List_of_Unique_Body_Armor_(Diablo_I))
* Diablo Normal Armor - [Diablo Archive](https://diablo-archive.fandom.com/wiki/Normal_Armor_(Diablo_I))
* Diablo and Hellfire Unique Rings - [Diablo Wiki](https://diablo.fandom.com/wiki/List_of_Unique_Rings_(Diablo_I))
* Diablo and Hellfire Unique Amulets - [Diablo Wiki](https://diablo.fandom.com/wiki/List_of_Unique_Amulets_(Diablo_I))
* Affixes - [diablowiki.net](https://diablo2.diablowiki.net/D1_Modifiers)
