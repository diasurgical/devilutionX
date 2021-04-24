/**
 * @file effects.h
 *
 * Interface of functions for loading and playing sounds.
 */
#pragma once

#include <stdint.h>
#include <string_view>

#include "sound.h"

namespace devilution {

enum _sfx_id : int16_t {
	PS_WALK1,
	PS_WALK2,
	PS_WALK3,
	PS_WALK4,
	PS_BFIRE,
	PS_FMAG,
	PS_TMAG,
	PS_LGHIT,
	PS_LGHIT1,
	PS_SWING,
	PS_SWING2,
	PS_DEAD,
	IS_STING1,
	IS_FBALLBOW,
	IS_QUESTDN,
	IS_ARMRFKD,
	IS_BARLFIRE,
	IS_BARREL,
	IS_POPPOP8,
	IS_POPPOP5,
	IS_POPPOP3,
	IS_POPPOP2,
	IS_BHIT,
	IS_BHIT1,
	IS_CHEST,
	IS_DOORCLOS,
	IS_DOOROPEN,
	IS_FANVL,
	IS_FAXE,
	IS_FBLST,
	IS_FBODY,
	IS_FBOOK,
	IS_FBOW,
	IS_FCAP,
	IS_FHARM,
	IS_FLARM,
	IS_FMAG,
	IS_FMAG1,
	IS_FMUSH,
	IS_FPOT,
	IS_FRING,
	IS_FROCK,
	IS_FSCRL,
	IS_FSHLD,
	IS_FSIGN,
	IS_FSTAF,
	IS_FSWOR,
	IS_GOLD,
	IS_HLMTFKD,
	IS_IANVL,
	IS_IAXE,
	IS_IBLST,
	IS_IBODY,
	IS_IBOOK,
	IS_IBOW,
	IS_ICAP,
	IS_IGRAB,
	IS_IHARM,
	IS_ILARM,
	IS_IMUSH,
	IS_IPOT,
	IS_IRING,
	IS_IROCK,
	IS_ISCROL,
	IS_ISHIEL,
	IS_ISIGN,
	IS_ISTAF,
	IS_ISWORD,
	IS_LEVER,
	IS_MAGIC,
	IS_MAGIC1,
	IS_RBOOK,
	IS_SARC,
	IS_SHLDFKD,
	IS_SWRDFKD,
	IS_TITLEMOV,
	IS_TITLSLCT,
	SFX_SILENCE,
	IS_TRAP,
	IS_CAST1,
	IS_CAST10,
	IS_CAST12,
	IS_CAST2,
	IS_CAST3,
	IS_CAST4,
	IS_CAST5,
	IS_CAST6,
	IS_CAST7,
	IS_CAST8,
	IS_CAST9,
	LS_HEALING,
	IS_REPAIR,
	LS_ACID,
	LS_ACIDS,
	LS_APOC,
	LS_ARROWALL,
	LS_BLODBOIL,
	LS_BLODSTAR,
	LS_BLSIMPT,
	LS_BONESP,
	LS_BSIMPCT,
	LS_CALDRON,
	LS_CBOLT,
	LS_CHLTNING,
	LS_DSERP,
	LS_ELECIMP1,
	LS_ELEMENTL,
	LS_ETHEREAL,
	LS_FBALL,
	LS_FBOLT1,
	LS_FBOLT2,
	LS_FIRIMP1,
	LS_FIRIMP2,
	LS_FLAMWAVE,
	LS_FLASH,
	LS_FOUNTAIN,
	LS_GOLUM,
	LS_GOLUMDED,
	LS_GSHRINE,
	LS_GUARD,
	LS_GUARDLAN,
	LS_HOLYBOLT,
	LS_HYPER,
	LS_INFRAVIS,
	LS_INVISIBL,
	LS_INVPOT,
	LS_LNING1,
	LS_LTNING,
	LS_MSHIELD,
	LS_NESTXPLD,
	LS_NOVA,
	LS_PORTAL,
	LS_PUDDLE,
	LS_RESUR,
	LS_SCURSE,
	LS_SCURIMP,
	LS_SENTINEL,
	LS_SHATTER,
	LS_SOULFIRE,
	LS_SPOUTLOP,
	LS_SPOUTSTR,
	LS_STORM,
	LS_TRAPDIS,
	LS_TELEPORT,
	LS_VTHEFT,
	LS_WALLLOOP,
	LS_WALLSTRT,
	LS_LMAG,
	TSFX_BMAID1,
	TSFX_BMAID2,
	TSFX_BMAID3,
	TSFX_BMAID4,
	TSFX_BMAID5,
	TSFX_BMAID6,
	TSFX_BMAID7,
	TSFX_BMAID8,
	TSFX_BMAID9,
	TSFX_BMAID10,
	TSFX_BMAID11,
	TSFX_BMAID12,
	TSFX_BMAID13,
	TSFX_BMAID14,
	TSFX_BMAID15,
	TSFX_BMAID16,
	TSFX_BMAID17,
	TSFX_BMAID18,
	TSFX_BMAID19,
	TSFX_BMAID20,
	TSFX_BMAID21,
	TSFX_BMAID22,
	TSFX_BMAID23,
	TSFX_BMAID24,
	TSFX_BMAID25,
	TSFX_BMAID26,
	TSFX_BMAID27,
	TSFX_BMAID28,
	TSFX_BMAID29,
	TSFX_BMAID30,
	TSFX_BMAID31,
	TSFX_BMAID32,
	TSFX_BMAID33,
	TSFX_BMAID34,
	TSFX_BMAID35,
	TSFX_BMAID36,
	TSFX_BMAID37,
	TSFX_BMAID38,
	TSFX_BMAID39,
	TSFX_BMAID40,
	TSFX_SMITH1,
	TSFX_SMITH2,
	TSFX_SMITH3,
	TSFX_SMITH4,
	TSFX_SMITH5,
	TSFX_SMITH6,
	TSFX_SMITH7,
	TSFX_SMITH8,
	TSFX_SMITH9,
	TSFX_SMITH10,
	TSFX_SMITH11,
	TSFX_SMITH12,
	TSFX_SMITH13,
	TSFX_SMITH14,
	TSFX_SMITH15,
	TSFX_SMITH16,
	TSFX_SMITH17,
	TSFX_SMITH18,
	TSFX_SMITH19,
	TSFX_SMITH20,
	TSFX_SMITH21,
	TSFX_SMITH22,
	TSFX_SMITH23,
	TSFX_SMITH24,
	TSFX_SMITH25,
	TSFX_SMITH26,
	TSFX_SMITH27,
	TSFX_SMITH28,
	TSFX_SMITH29,
	TSFX_SMITH30,
	TSFX_SMITH31,
	TSFX_SMITH32,
	TSFX_SMITH33,
	TSFX_SMITH34,
	TSFX_SMITH35,
	TSFX_SMITH36,
	TSFX_SMITH37,
	TSFX_SMITH38,
	TSFX_SMITH39,
	TSFX_SMITH40,
	TSFX_SMITH41,
	TSFX_SMITH42,
	TSFX_SMITH43,
	TSFX_SMITH44,
	TSFX_SMITH45,
	TSFX_SMITH46,
	TSFX_SMITH47,
	TSFX_SMITH48,
	TSFX_SMITH49,
	TSFX_SMITH50,
	TSFX_SMITH51,
	TSFX_SMITH52,
	TSFX_SMITH53,
	TSFX_SMITH54,
	TSFX_SMITH55,
	TSFX_SMITH56,
	TSFX_COW1,
	TSFX_COW2,
	/*
	TSFX_COW3,
	TSFX_COW4,
	TSFX_COW5,
	TSFX_COW6,
	*/
	TSFX_COW7,
	TSFX_COW8,
	TSFX_DEADGUY,
	TSFX_DRUNK1,
	TSFX_DRUNK2,
	TSFX_DRUNK3,
	TSFX_DRUNK4,
	TSFX_DRUNK5,
	TSFX_DRUNK6,
	TSFX_DRUNK7,
	TSFX_DRUNK8,
	TSFX_DRUNK9,
	TSFX_DRUNK10,
	TSFX_DRUNK11,
	TSFX_DRUNK12,
	TSFX_DRUNK13,
	TSFX_DRUNK14,
	TSFX_DRUNK15,
	TSFX_DRUNK16,
	TSFX_DRUNK17,
	TSFX_DRUNK18,
	TSFX_DRUNK19,
	TSFX_DRUNK20,
	TSFX_DRUNK21,
	TSFX_DRUNK22,
	TSFX_DRUNK23,
	TSFX_DRUNK24,
	TSFX_DRUNK25,
	TSFX_DRUNK26,
	TSFX_DRUNK27,
	TSFX_DRUNK28,
	TSFX_DRUNK29,
	TSFX_DRUNK30,
	TSFX_DRUNK31,
	TSFX_DRUNK32,
	TSFX_DRUNK33,
	TSFX_DRUNK34,
	TSFX_DRUNK35,
	TSFX_HEALER1,
	TSFX_HEALER2,
	TSFX_HEALER3,
	TSFX_HEALER4,
	TSFX_HEALER5,
	TSFX_HEALER6,
	TSFX_HEALER7,
	TSFX_HEALER8,
	TSFX_HEALER9,
	TSFX_HEALER10,
	TSFX_HEALER11,
	TSFX_HEALER12,
	TSFX_HEALER13,
	TSFX_HEALER14,
	TSFX_HEALER15,
	TSFX_HEALER16,
	TSFX_HEALER17,
	TSFX_HEALER18,
	TSFX_HEALER19,
	TSFX_HEALER20,
	TSFX_HEALER21,
	TSFX_HEALER22,
	TSFX_HEALER23,
	TSFX_HEALER24,
	TSFX_HEALER25,
	TSFX_HEALER26,
	TSFX_HEALER27,
	TSFX_HEALER28,
	TSFX_HEALER29,
	TSFX_HEALER30,
	TSFX_HEALER31,
	TSFX_HEALER32,
	TSFX_HEALER33,
	TSFX_HEALER34,
	TSFX_HEALER35,
	TSFX_HEALER36,
	TSFX_HEALER37,
	TSFX_HEALER38,
	TSFX_HEALER39,
	TSFX_HEALER40,
	TSFX_HEALER41,
	TSFX_HEALER42,
	TSFX_HEALER43,
	TSFX_HEALER44,
	TSFX_HEALER45,
	TSFX_HEALER46,
	TSFX_HEALER47,
	TSFX_PEGBOY1,
	TSFX_PEGBOY2,
	TSFX_PEGBOY3,
	TSFX_PEGBOY4,
	TSFX_PEGBOY5,
	TSFX_PEGBOY6,
	TSFX_PEGBOY7,
	TSFX_PEGBOY8,
	TSFX_PEGBOY9,
	TSFX_PEGBOY10,
	TSFX_PEGBOY11,
	TSFX_PEGBOY12,
	TSFX_PEGBOY13,
	TSFX_PEGBOY14,
	TSFX_PEGBOY15,
	TSFX_PEGBOY16,
	TSFX_PEGBOY17,
	TSFX_PEGBOY18,
	TSFX_PEGBOY19,
	TSFX_PEGBOY20,
	TSFX_PEGBOY21,
	TSFX_PEGBOY22,
	TSFX_PEGBOY23,
	TSFX_PEGBOY24,
	TSFX_PEGBOY25,
	TSFX_PEGBOY26,
	TSFX_PEGBOY27,
	TSFX_PEGBOY28,
	TSFX_PEGBOY29,
	TSFX_PEGBOY30,
	TSFX_PEGBOY31,
	TSFX_PEGBOY32,
	TSFX_PEGBOY33,
	TSFX_PEGBOY34,
	TSFX_PEGBOY35,
	TSFX_PEGBOY36,
	TSFX_PEGBOY37,
	TSFX_PEGBOY38,
	TSFX_PEGBOY39,
	TSFX_PEGBOY40,
	TSFX_PEGBOY41,
	TSFX_PEGBOY42,
	TSFX_PEGBOY43,
	TSFX_PRIEST0,
	TSFX_PRIEST1,
	TSFX_PRIEST2,
	TSFX_PRIEST3,
	TSFX_PRIEST4,
	TSFX_PRIEST5,
	TSFX_PRIEST6,
	TSFX_PRIEST7,
	TSFX_STORY0,
	TSFX_STORY1,
	TSFX_STORY2,
	TSFX_STORY3,
	TSFX_STORY4,
	TSFX_STORY5,
	TSFX_STORY6,
	TSFX_STORY7,
	TSFX_STORY8,
	TSFX_STORY9,
	TSFX_STORY10,
	TSFX_STORY11,
	TSFX_STORY12,
	TSFX_STORY13,
	TSFX_STORY14,
	TSFX_STORY15,
	TSFX_STORY16,
	TSFX_STORY17,
	TSFX_STORY18,
	TSFX_STORY19,
	TSFX_STORY20,
	TSFX_STORY21,
	TSFX_STORY22,
	TSFX_STORY23,
	TSFX_STORY24,
	TSFX_STORY25,
	TSFX_STORY26,
	TSFX_STORY27,
	TSFX_STORY28,
	TSFX_STORY29,
	TSFX_STORY30,
	TSFX_STORY31,
	TSFX_STORY32,
	TSFX_STORY33,
	TSFX_STORY34,
	TSFX_STORY35,
	TSFX_STORY36,
	TSFX_STORY37,
	TSFX_STORY38,
	TSFX_TAVERN0,
	TSFX_TAVERN1,
	TSFX_TAVERN2,
	TSFX_TAVERN3,
	TSFX_TAVERN4,
	TSFX_TAVERN5,
	TSFX_TAVERN6,
	TSFX_TAVERN7,
	TSFX_TAVERN8,
	TSFX_TAVERN9,
	TSFX_TAVERN10,
	TSFX_TAVERN11,
	TSFX_TAVERN12,
	TSFX_TAVERN13,
	TSFX_TAVERN14,
	TSFX_TAVERN15,
	TSFX_TAVERN16,
	TSFX_TAVERN17,
	TSFX_TAVERN18,
	TSFX_TAVERN19,
	TSFX_TAVERN20,
	TSFX_TAVERN21,
	TSFX_TAVERN22,
	TSFX_TAVERN23,
	TSFX_TAVERN24,
	TSFX_TAVERN25,
	TSFX_TAVERN26,
	TSFX_TAVERN27,
	TSFX_TAVERN28,
	TSFX_TAVERN29,
	TSFX_TAVERN30,
	TSFX_TAVERN31,
	TSFX_TAVERN32,
	TSFX_TAVERN33,
	TSFX_TAVERN34,
	TSFX_TAVERN35,
	TSFX_TAVERN36,
	TSFX_TAVERN37,
	TSFX_TAVERN38,
	TSFX_TAVERN39,
	TSFX_TAVERN40,
	TSFX_TAVERN41,
	TSFX_TAVERN42,
	TSFX_TAVERN43,
	TSFX_TAVERN44,
	TSFX_TAVERN45,
	TSFX_WITCH1,
	TSFX_WITCH2,
	TSFX_WITCH3,
	TSFX_WITCH4,
	TSFX_WITCH5,
	TSFX_WITCH6,
	TSFX_WITCH7,
	TSFX_WITCH8,
	TSFX_WITCH9,
	TSFX_WITCH10,
	TSFX_WITCH11,
	TSFX_WITCH12,
	TSFX_WITCH13,
	TSFX_WITCH14,
	TSFX_WITCH15,
	TSFX_WITCH16,
	TSFX_WITCH17,
	TSFX_WITCH18,
	TSFX_WITCH19,
	TSFX_WITCH20,
	TSFX_WITCH21,
	TSFX_WITCH22,
	TSFX_WITCH23,
	TSFX_WITCH24,
	TSFX_WITCH25,
	TSFX_WITCH26,
	TSFX_WITCH27,
	TSFX_WITCH28,
	TSFX_WITCH29,
	TSFX_WITCH30,
	TSFX_WITCH31,
	TSFX_WITCH32,
	TSFX_WITCH33,
	TSFX_WITCH34,
	TSFX_WITCH35,
	TSFX_WITCH36,
	TSFX_WITCH37,
	TSFX_WITCH38,
	TSFX_WITCH39,
	TSFX_WITCH40,
	TSFX_WITCH41,
	TSFX_WITCH42,
	TSFX_WITCH43,
	TSFX_WITCH44,
	TSFX_WITCH45,
	TSFX_WITCH46,
	TSFX_WITCH47,
	TSFX_WITCH48,
	TSFX_WITCH49,
	TSFX_WITCH50,
	TSFX_WOUND,
	PS_MAGE1,
	PS_MAGE2,
	PS_MAGE3,
	PS_MAGE4,
	PS_MAGE5,
	PS_MAGE6,
	PS_MAGE7,
	PS_MAGE8,
	PS_MAGE9,
	PS_MAGE10,
	PS_MAGE11,
	PS_MAGE12,
	PS_MAGE13,
	PS_MAGE14,
	PS_MAGE15,
	PS_MAGE16,
	PS_MAGE17,
	PS_MAGE18,
	PS_MAGE19,
	PS_MAGE20,
	PS_MAGE21,
	PS_MAGE22,
	PS_MAGE23,
	PS_MAGE24,
	PS_MAGE25,
	PS_MAGE26,
	PS_MAGE27,
	PS_MAGE28,
	PS_MAGE29,
	PS_MAGE30,
	PS_MAGE31,
	PS_MAGE32,
	PS_MAGE33,
	PS_MAGE34,
	PS_MAGE35,
	PS_MAGE36,
	PS_MAGE37,
	PS_MAGE38,
	PS_MAGE39,
	PS_MAGE40,
	PS_MAGE41,
	PS_MAGE42,
	PS_MAGE43,
	PS_MAGE44,
	PS_MAGE45,
	PS_MAGE46,
	PS_MAGE47,
	PS_MAGE48,
	PS_MAGE49,
	PS_MAGE50,
	PS_MAGE51,
	PS_MAGE52,
	PS_MAGE53,
	PS_MAGE54,
	PS_MAGE55,
	PS_MAGE56,
	PS_MAGE57,
	PS_MAGE58,
	PS_MAGE59,
	PS_MAGE60,
	PS_MAGE61,
	PS_MAGE62,
	PS_MAGE63,
	PS_MAGE64,
	PS_MAGE65,
	PS_MAGE66,
	PS_MAGE67,
	PS_MAGE68,
	PS_MAGE69,
	PS_MAGE69B,
	PS_MAGE70,
	PS_MAGE71,
	PS_MAGE72,
	PS_MAGE73,
	PS_MAGE74,
	PS_MAGE75,
	PS_MAGE76,
	PS_MAGE77,
	PS_MAGE78,
	PS_MAGE79,
	PS_MAGE80,
	PS_MAGE81,
	PS_MAGE82,
	PS_MAGE83,
	PS_MAGE84,
	PS_MAGE85,
	PS_MAGE86,
	PS_MAGE87,
	PS_MAGE88,
	PS_MAGE89,
	PS_MAGE90,
	PS_MAGE91,
	PS_MAGE92,
	PS_MAGE93,
	PS_MAGE94,
	PS_MAGE95,
	PS_MAGE96,
	PS_MAGE97,
	PS_MAGE98,
	PS_MAGE99,
	PS_MAGE100,
	PS_MAGE101,
	PS_MAGE102,
	PS_ROGUE1,
	PS_ROGUE2,
	PS_ROGUE3,
	PS_ROGUE4,
	PS_ROGUE5,
	PS_ROGUE6,
	PS_ROGUE7,
	PS_ROGUE8,
	PS_ROGUE9,
	PS_ROGUE10,
	PS_ROGUE11,
	PS_ROGUE12,
	PS_ROGUE13,
	PS_ROGUE14,
	PS_ROGUE15,
	PS_ROGUE16,
	PS_ROGUE17,
	PS_ROGUE18,
	PS_ROGUE19,
	PS_ROGUE20,
	PS_ROGUE21,
	PS_ROGUE22,
	PS_ROGUE23,
	PS_ROGUE24,
	PS_ROGUE25,
	PS_ROGUE26,
	PS_ROGUE27,
	PS_ROGUE28,
	PS_ROGUE29,
	PS_ROGUE30,
	PS_ROGUE31,
	PS_ROGUE32,
	PS_ROGUE33,
	PS_ROGUE34,
	PS_ROGUE35,
	PS_ROGUE36,
	PS_ROGUE37,
	PS_ROGUE38,
	PS_ROGUE39,
	PS_ROGUE40,
	PS_ROGUE41,
	PS_ROGUE42,
	PS_ROGUE43,
	PS_ROGUE44,
	PS_ROGUE45,
	PS_ROGUE46,
	PS_ROGUE47,
	PS_ROGUE48,
	PS_ROGUE49,
	PS_ROGUE50,
	PS_ROGUE51,
	PS_ROGUE52,
	PS_ROGUE53,
	PS_ROGUE54,
	PS_ROGUE55,
	PS_ROGUE56,
	PS_ROGUE57,
	PS_ROGUE58,
	PS_ROGUE59,
	PS_ROGUE60,
	PS_ROGUE61,
	PS_ROGUE62,
	PS_ROGUE63,
	PS_ROGUE64,
	PS_ROGUE65,
	PS_ROGUE66,
	PS_ROGUE67,
	PS_ROGUE68,
	PS_ROGUE69,
	PS_ROGUE69B,
	PS_ROGUE70,
	PS_ROGUE71,
	PS_ROGUE72,
	PS_ROGUE73,
	PS_ROGUE74,
	PS_ROGUE75,
	PS_ROGUE76,
	PS_ROGUE77,
	PS_ROGUE78,
	PS_ROGUE79,
	PS_ROGUE80,
	PS_ROGUE81,
	PS_ROGUE82,
	PS_ROGUE83,
	PS_ROGUE84,
	PS_ROGUE85,
	PS_ROGUE86,
	PS_ROGUE87,
	PS_ROGUE88,
	PS_ROGUE89,
	PS_ROGUE90,
	PS_ROGUE91,
	PS_ROGUE92,
	PS_ROGUE93,
	PS_ROGUE94,
	PS_ROGUE95,
	PS_ROGUE96,
	PS_ROGUE97,
	PS_ROGUE98,
	PS_ROGUE99,
	PS_ROGUE100,
	PS_ROGUE101,
	PS_ROGUE102,
	PS_WARR1,
	PS_WARR2,
	PS_WARR3,
	PS_WARR4,
	PS_WARR5,
	PS_WARR6,
	PS_WARR7,
	PS_WARR8,
	PS_WARR9,
	PS_WARR10,
	PS_WARR11,
	PS_WARR12,
	PS_WARR13,
	PS_WARR14,
	PS_WARR14B,
	PS_WARR14C,
	PS_WARR15,
	PS_WARR15B,
	PS_WARR15C,
	PS_WARR16,
	PS_WARR16B,
	PS_WARR16C,
	PS_WARR17,
	PS_WARR18,
	PS_WARR19,
	PS_WARR20,
	PS_WARR21,
	PS_WARR22,
	PS_WARR23,
	PS_WARR24,
	PS_WARR25,
	PS_WARR26,
	PS_WARR27,
	PS_WARR28,
	PS_WARR29,
	PS_WARR30,
	PS_WARR31,
	PS_WARR32,
	PS_WARR33,
	PS_WARR34,
	PS_WARR35,
	PS_WARR36,
	PS_WARR37,
	PS_WARR38,
	PS_WARR39,
	PS_WARR40,
	PS_WARR41,
	PS_WARR42,
	PS_WARR43,
	PS_WARR44,
	PS_WARR45,
	PS_WARR46,
	PS_WARR47,
	PS_WARR48,
	PS_WARR49,
	PS_WARR50,
	PS_WARR51,
	PS_WARR52,
	PS_WARR53,
	PS_WARR54,
	PS_WARR55,
	PS_WARR56,
	PS_WARR57,
	PS_WARR58,
	PS_WARR59,
	PS_WARR60,
	PS_WARR61,
	PS_WARR62,
	PS_WARR63,
	PS_WARR64,
	PS_WARR65,
	PS_WARR66,
	PS_WARR67,
	PS_WARR68,
	PS_WARR69,
	PS_WARR69B,
	PS_WARR70,
	PS_WARR71,
	PS_WARR72,
	PS_WARR73,
	PS_WARR74,
	PS_WARR75,
	PS_WARR76,
	PS_WARR77,
	PS_WARR78,
	PS_WARR79,
	PS_WARR80,
	PS_WARR81,
	PS_WARR82,
	PS_WARR83,
	PS_WARR84,
	PS_WARR85,
	PS_WARR86,
	PS_WARR87,
	PS_WARR88,
	PS_WARR89,
	PS_WARR90,
	PS_WARR91,
	PS_WARR92,
	PS_WARR93,
	PS_WARR94,
	PS_WARR95,
	PS_WARR95B,
	PS_WARR95C,
	PS_WARR95D,
	PS_WARR95E,
	PS_WARR95F,
	PS_WARR96B,
	PS_WARR97,
	PS_WARR98,
	PS_WARR99,
	PS_WARR100,
	PS_WARR101,
	PS_WARR102,
	PS_MONK1,
	PS_MONK2,
	PS_MONK3,
	PS_MONK4,
	PS_MONK5,
	PS_MONK6,
	PS_MONK7,
	PS_MONK8,
	PS_MONK9,
	PS_MONK10,
	PS_MONK11,
	PS_MONK12,
	PS_MONK13,
	PS_MONK14,
	PS_MONK15,
	PS_MONK16,
	PS_MONK17,
	PS_MONK18,
	PS_MONK19,
	PS_MONK20,
	PS_MONK21,
	PS_MONK22,
	PS_MONK23,
	PS_MONK24,
	PS_MONK25,
	PS_MONK26,
	PS_MONK27,
	PS_MONK28,
	PS_MONK29,
	PS_MONK30,
	PS_MONK31,
	PS_MONK32,
	PS_MONK33,
	PS_MONK34,
	PS_MONK35,
	PS_MONK36,
	PS_MONK37,
	PS_MONK38,
	PS_MONK39,
	PS_MONK40,
	PS_MONK41,
	PS_MONK42,
	PS_MONK43,
	PS_MONK44,
	PS_MONK45,
	PS_MONK46,
	PS_MONK47,
	PS_MONK48,
	PS_MONK49,
	PS_MONK50,
	PS_MONK51,
	PS_MONK52,
	PS_MONK53,
	PS_MONK54,
	PS_MONK55,
	PS_MONK56,
	PS_MONK57,
	PS_MONK58,
	PS_MONK59,
	PS_MONK60,
	PS_MONK61,
	PS_MONK62,
	PS_MONK63,
	PS_MONK64,
	PS_MONK65,
	PS_MONK66,
	PS_MONK67,
	PS_MONK68,
	PS_MONK69,
	PS_MONK69B,
	PS_MONK70,
	PS_MONK71,
	PS_MONK72,
	PS_MONK73,
	PS_MONK74,
	PS_MONK75,
	PS_MONK76,
	PS_MONK77,
	PS_MONK78,
	PS_MONK79,
	PS_MONK80,
	PS_MONK81,
	PS_MONK82,
	PS_MONK83,
	PS_MONK84,
	PS_MONK85,
	PS_MONK86,
	PS_MONK87,
	PS_MONK88,
	PS_MONK89,
	PS_MONK90,
	PS_MONK91,
	PS_MONK92,
	PS_MONK93,
	PS_MONK94,
	PS_MONK95,
	PS_MONK96,
	PS_MONK97,
	PS_MONK98,
	PS_MONK99,
	PS_MONK100,
	PS_MONK101,
	PS_MONK102,
	PS_NAR1,
	PS_NAR2,
	PS_NAR3,
	PS_NAR4,
	PS_NAR5,
	PS_NAR6,
	PS_NAR7,
	PS_NAR8,
	PS_NAR9,
	PS_DIABLVLINT,
	USFX_CLEAVER,
	USFX_GARBUD1,
	USFX_GARBUD2,
	USFX_GARBUD3,
	USFX_GARBUD4,
	USFX_IZUAL1,
	USFX_LACH1,
	USFX_LACH2,
	USFX_LACH3,
	USFX_LAZ1,
	USFX_LAZ2,
	USFX_SKING1,
	USFX_SNOT1,
	USFX_SNOT2,
	USFX_SNOT3,
	USFX_WARLRD1,
	USFX_WLOCK1,
	USFX_ZHAR1,
	USFX_ZHAR2,
	USFX_DIABLOD,
	TSFX_FARMER1,
	TSFX_FARMER2,
	TSFX_FARMER2A,
	TSFX_FARMER3,
	TSFX_FARMER4,
	TSFX_FARMER5,
	TSFX_FARMER6,
	TSFX_FARMER7,
	TSFX_FARMER8,
	TSFX_FARMER9,
	TSFX_TEDDYBR1,
	TSFX_TEDDYBR2,
	TSFX_TEDDYBR3,
	TSFX_TEDDYBR4,
	USFX_DEFILER1,
	USFX_DEFILER2,
	USFX_DEFILER3,
	USFX_DEFILER4,
	USFX_DEFILER8,
	USFX_DEFILER6,
	USFX_DEFILER7,
	USFX_NAKRUL1,
	USFX_NAKRUL2,
	USFX_NAKRUL3,
	USFX_NAKRUL4,
	USFX_NAKRUL5,
	USFX_NAKRUL6,
	PS_NARATR3,
	TSFX_COWSUT1,
	TSFX_COWSUT2,
	TSFX_COWSUT3,
	TSFX_COWSUT4,
	TSFX_COWSUT4A,
	TSFX_COWSUT5,
	TSFX_COWSUT6,
	TSFX_COWSUT7,
	TSFX_COWSUT8,
	TSFX_COWSUT9,
	TSFX_COWSUT10,
	TSFX_COWSUT11,
	TSFX_COWSUT12,
	USFX_SKLJRN1,
	PS_NARATR6,
	PS_NARATR7,
	PS_NARATR8,
	PS_NARATR5,
	PS_NARATR9,
	PS_NARATR4,
	TSFX_TRADER1,
	IS_CROPEN,
	IS_CRCLOS,
	SFX_NONE = -1,
};

[[maybe_unused]] constexpr std::string_view toString(_sfx_id value)
{
	switch(value) {
	case PS_WALK1:
		return "PlayerSound Walk1";
	case PS_WALK2:
		return "PlayerSound Walk2";
	case PS_WALK3:
		return "PlayerSound Walk3";
	case PS_WALK4:
		return "PlayerSound Walk4";
	case PS_BFIRE:
		return "PlayerSound Bfire";
	case PS_FMAG:
		return "PlayerSound Fmag";
	case PS_TMAG:
		return "PlayerSound Tmag";
	case PS_LGHIT:
		return "PlayerSound Lghit";
	case PS_LGHIT1:
		return "PlayerSound Lghit1";
	case PS_SWING:
		return "PlayerSound Swing";
	case PS_SWING2:
		return "PlayerSound Swing2";
	case PS_DEAD:
		return "PlayerSound Dead";
	case IS_STING1:
		return "ItemSound Sting1";
	case IS_FBALLBOW:
		return "ItemSound Fballbow";
	case IS_QUESTDN:
		return "ItemSound Questdn";
	case IS_ARMRFKD:
		return "ItemSound Armrfkd";
	case IS_BARLFIRE:
		return "ItemSound Barlfire";
	case IS_BARREL:
		return "ItemSound Barrel";
	case IS_POPPOP8:
		return "ItemSound Poppop8";
	case IS_POPPOP5:
		return "ItemSound Poppop5";
	case IS_POPPOP3:
		return "ItemSound Poppop3";
	case IS_POPPOP2:
		return "ItemSound Poppop2";
	case IS_BHIT:
		return "ItemSound Bhit";
	case IS_BHIT1:
		return "ItemSound Bhit1";
	case IS_CHEST:
		return "ItemSound Chest";
	case IS_DOORCLOS:
		return "ItemSound Doorclos";
	case IS_DOOROPEN:
		return "ItemSound Dooropen";
	case IS_FANVL:
		return "ItemSound Fanvl";
	case IS_FAXE:
		return "ItemSound Faxe";
	case IS_FBLST:
		return "ItemSound Fblst";
	case IS_FBODY:
		return "ItemSound Fbody";
	case IS_FBOOK:
		return "ItemSound Fbook";
	case IS_FBOW:
		return "ItemSound Fbow";
	case IS_FCAP:
		return "ItemSound Fcap";
	case IS_FHARM:
		return "ItemSound Fharm";
	case IS_FLARM:
		return "ItemSound Flarm";
	case IS_FMAG:
		return "ItemSound Fmag";
	case IS_FMAG1:
		return "ItemSound Fmag1";
	case IS_FMUSH:
		return "ItemSound Fmush";
	case IS_FPOT:
		return "ItemSound Fpot";
	case IS_FRING:
		return "ItemSound Fring";
	case IS_FROCK:
		return "ItemSound Frock";
	case IS_FSCRL:
		return "ItemSound Fscrl";
	case IS_FSHLD:
		return "ItemSound Fshld";
	case IS_FSIGN:
		return "ItemSound Fsign";
	case IS_FSTAF:
		return "ItemSound Fstaf";
	case IS_FSWOR:
		return "ItemSound Fswor";
	case IS_GOLD:
		return "ItemSound Gold";
	case IS_HLMTFKD:
		return "ItemSound Hlmtfkd";
	case IS_IANVL:
		return "ItemSound Ianvl";
	case IS_IAXE:
		return "ItemSound Iaxe";
	case IS_IBLST:
		return "ItemSound Iblst";
	case IS_IBODY:
		return "ItemSound Ibody";
	case IS_IBOOK:
		return "ItemSound Ibook";
	case IS_IBOW:
		return "ItemSound Ibow";
	case IS_ICAP:
		return "ItemSound Icap";
	case IS_IGRAB:
		return "ItemSound Igrab";
	case IS_IHARM:
		return "ItemSound Iharm";
	case IS_ILARM:
		return "ItemSound Ilarm";
	case IS_IMUSH:
		return "ItemSound Imush";
	case IS_IPOT:
		return "ItemSound Ipot";
	case IS_IRING:
		return "ItemSound Iring";
	case IS_IROCK:
		return "ItemSound Irock";
	case IS_ISCROL:
		return "ItemSound Iscrol";
	case IS_ISHIEL:
		return "ItemSound Ishiel";
	case IS_ISIGN:
		return "ItemSound Isign";
	case IS_ISTAF:
		return "ItemSound Istaf";
	case IS_ISWORD:
		return "ItemSound Isword";
	case IS_LEVER:
		return "ItemSound Lever";
	case IS_MAGIC:
		return "ItemSound Magic";
	case IS_MAGIC1:
		return "ItemSound Magic1";
	case IS_RBOOK:
		return "ItemSound Rbook";
	case IS_SARC:
		return "ItemSound Sarc";
	case IS_SHLDFKD:
		return "ItemSound Shldfkd";
	case IS_SWRDFKD:
		return "ItemSound Swrdfkd";
	case IS_TITLEMOV:
		return "ItemSound Titlemov";
	case IS_TITLSLCT:
		return "ItemSound Titlslct";
	case SFX_SILENCE:
		return "Sfx Silence";
	case IS_TRAP:
		return "ItemSound Trap";
	case IS_CAST1:
		return "ItemSound Cast1";
	case IS_CAST10:
		return "ItemSound Cast10";
	case IS_CAST12:
		return "ItemSound Cast12";
	case IS_CAST2:
		return "ItemSound Cast2";
	case IS_CAST3:
		return "ItemSound Cast3";
	case IS_CAST4:
		return "ItemSound Cast4";
	case IS_CAST5:
		return "ItemSound Cast5";
	case IS_CAST6:
		return "ItemSound Cast6";
	case IS_CAST7:
		return "ItemSound Cast7";
	case IS_CAST8:
		return "ItemSound Cast8";
	case IS_CAST9:
		return "ItemSound Cast9";
	case LS_HEALING:
		return "SpellSound Healing";
	case IS_REPAIR:
		return "ItemSound Repair";
	case LS_ACID:
		return "SpellSound Acid";
	case LS_ACIDS:
		return "SpellSound Acids";
	case LS_APOC:
		return "SpellSound Apoc";
	case LS_ARROWALL:
		return "SpellSound Arrowall";
	case LS_BLODBOIL:
		return "SpellSound Blodboil";
	case LS_BLODSTAR:
		return "SpellSound Blodstar";
	case LS_BLSIMPT:
		return "SpellSound Blsimpt";
	case LS_BONESP:
		return "SpellSound Bonesp";
	case LS_BSIMPCT:
		return "SpellSound Bsimpct";
	case LS_CALDRON:
		return "SpellSound Caldron";
	case LS_CBOLT:
		return "SpellSound Cbolt";
	case LS_CHLTNING:
		return "SpellSound Chltning";
	case LS_DSERP:
		return "SpellSound Dserp";
	case LS_ELECIMP1:
		return "SpellSound Elecimp1";
	case LS_ELEMENTL:
		return "SpellSound Elementl";
	case LS_ETHEREAL:
		return "SpellSound Ethereal";
	case LS_FBALL:
		return "SpellSound Fball";
	case LS_FBOLT1:
		return "SpellSound Fbolt1";
	case LS_FBOLT2:
		return "SpellSound Fbolt2";
	case LS_FIRIMP1:
		return "SpellSound Firimp1";
	case LS_FIRIMP2:
		return "SpellSound Firimp2";
	case LS_FLAMWAVE:
		return "SpellSound Flamwave";
	case LS_FLASH:
		return "SpellSound Flash";
	case LS_FOUNTAIN:
		return "SpellSound Fountain";
	case LS_GOLUM:
		return "SpellSound Golum";
	case LS_GOLUMDED:
		return "SpellSound Golumded";
	case LS_GSHRINE:
		return "SpellSound Gshrine";
	case LS_GUARD:
		return "SpellSound Guard";
	case LS_GUARDLAN:
		return "SpellSound Guardlan";
	case LS_HOLYBOLT:
		return "SpellSound Holybolt";
	case LS_HYPER:
		return "SpellSound Hyper";
	case LS_INFRAVIS:
		return "SpellSound Infravis";
	case LS_INVISIBL:
		return "SpellSound Invisibl";
	case LS_INVPOT:
		return "SpellSound Invpot";
	case LS_LNING1:
		return "SpellSound Lning1";
	case LS_LTNING:
		return "SpellSound Ltning";
	case LS_MSHIELD:
		return "SpellSound Mshield";
	case LS_NESTXPLD:
		return "SpellSound Nestxpld";
	case LS_NOVA:
		return "SpellSound Nova";
	case LS_PORTAL:
		return "SpellSound Portal";
	case LS_PUDDLE:
		return "SpellSound Puddle";
	case LS_RESUR:
		return "SpellSound Resur";
	case LS_SCURSE:
		return "SpellSound Scurse";
	case LS_SCURIMP:
		return "SpellSound Scurimp";
	case LS_SENTINEL:
		return "SpellSound Sentinel";
	case LS_SHATTER:
		return "SpellSound Shatter";
	case LS_SOULFIRE:
		return "SpellSound Soulfire";
	case LS_SPOUTLOP:
		return "SpellSound Spoutlop";
	case LS_SPOUTSTR:
		return "SpellSound Spoutstr";
	case LS_STORM:
		return "SpellSound Storm";
	case LS_TRAPDIS:
		return "SpellSound Trapdis";
	case LS_TELEPORT:
		return "SpellSound Teleport";
	case LS_VTHEFT:
		return "SpellSound Vtheft";
	case LS_WALLLOOP:
		return "SpellSound Wallloop";
	case LS_WALLSTRT:
		return "SpellSound Wallstrt";
	case LS_LMAG:
		return "SpellSound Lmag";
	case TSFX_BMAID1:
		return "TownerSound Bmaid1";
	case TSFX_BMAID2:
		return "TownerSound Bmaid2";
	case TSFX_BMAID3:
		return "TownerSound Bmaid3";
	case TSFX_BMAID4:
		return "TownerSound Bmaid4";
	case TSFX_BMAID5:
		return "TownerSound Bmaid5";
	case TSFX_BMAID6:
		return "TownerSound Bmaid6";
	case TSFX_BMAID7:
		return "TownerSound Bmaid7";
	case TSFX_BMAID8:
		return "TownerSound Bmaid8";
	case TSFX_BMAID9:
		return "TownerSound Bmaid9";
	case TSFX_BMAID10:
		return "TownerSound Bmaid10";
	case TSFX_BMAID11:
		return "TownerSound Bmaid11";
	case TSFX_BMAID12:
		return "TownerSound Bmaid12";
	case TSFX_BMAID13:
		return "TownerSound Bmaid13";
	case TSFX_BMAID14:
		return "TownerSound Bmaid14";
	case TSFX_BMAID15:
		return "TownerSound Bmaid15";
	case TSFX_BMAID16:
		return "TownerSound Bmaid16";
	case TSFX_BMAID17:
		return "TownerSound Bmaid17";
	case TSFX_BMAID18:
		return "TownerSound Bmaid18";
	case TSFX_BMAID19:
		return "TownerSound Bmaid19";
	case TSFX_BMAID20:
		return "TownerSound Bmaid20";
	case TSFX_BMAID21:
		return "TownerSound Bmaid21";
	case TSFX_BMAID22:
		return "TownerSound Bmaid22";
	case TSFX_BMAID23:
		return "TownerSound Bmaid23";
	case TSFX_BMAID24:
		return "TownerSound Bmaid24";
	case TSFX_BMAID25:
		return "TownerSound Bmaid25";
	case TSFX_BMAID26:
		return "TownerSound Bmaid26";
	case TSFX_BMAID27:
		return "TownerSound Bmaid27";
	case TSFX_BMAID28:
		return "TownerSound Bmaid28";
	case TSFX_BMAID29:
		return "TownerSound Bmaid29";
	case TSFX_BMAID30:
		return "TownerSound Bmaid30";
	case TSFX_BMAID31:
		return "TownerSound Bmaid31";
	case TSFX_BMAID32:
		return "TownerSound Bmaid32";
	case TSFX_BMAID33:
		return "TownerSound Bmaid33";
	case TSFX_BMAID34:
		return "TownerSound Bmaid34";
	case TSFX_BMAID35:
		return "TownerSound Bmaid35";
	case TSFX_BMAID36:
		return "TownerSound Bmaid36";
	case TSFX_BMAID37:
		return "TownerSound Bmaid37";
	case TSFX_BMAID38:
		return "TownerSound Bmaid38";
	case TSFX_BMAID39:
		return "TownerSound Bmaid39";
	case TSFX_BMAID40:
		return "TownerSound Bmaid40";
	case TSFX_SMITH1:
		return "TownerSound Smith1";
	case TSFX_SMITH2:
		return "TownerSound Smith2";
	case TSFX_SMITH3:
		return "TownerSound Smith3";
	case TSFX_SMITH4:
		return "TownerSound Smith4";
	case TSFX_SMITH5:
		return "TownerSound Smith5";
	case TSFX_SMITH6:
		return "TownerSound Smith6";
	case TSFX_SMITH7:
		return "TownerSound Smith7";
	case TSFX_SMITH8:
		return "TownerSound Smith8";
	case TSFX_SMITH9:
		return "TownerSound Smith9";
	case TSFX_SMITH10:
		return "TownerSound Smith10";
	case TSFX_SMITH11:
		return "TownerSound Smith11";
	case TSFX_SMITH12:
		return "TownerSound Smith12";
	case TSFX_SMITH13:
		return "TownerSound Smith13";
	case TSFX_SMITH14:
		return "TownerSound Smith14";
	case TSFX_SMITH15:
		return "TownerSound Smith15";
	case TSFX_SMITH16:
		return "TownerSound Smith16";
	case TSFX_SMITH17:
		return "TownerSound Smith17";
	case TSFX_SMITH18:
		return "TownerSound Smith18";
	case TSFX_SMITH19:
		return "TownerSound Smith19";
	case TSFX_SMITH20:
		return "TownerSound Smith20";
	case TSFX_SMITH21:
		return "TownerSound Smith21";
	case TSFX_SMITH22:
		return "TownerSound Smith22";
	case TSFX_SMITH23:
		return "TownerSound Smith23";
	case TSFX_SMITH24:
		return "TownerSound Smith24";
	case TSFX_SMITH25:
		return "TownerSound Smith25";
	case TSFX_SMITH26:
		return "TownerSound Smith26";
	case TSFX_SMITH27:
		return "TownerSound Smith27";
	case TSFX_SMITH28:
		return "TownerSound Smith28";
	case TSFX_SMITH29:
		return "TownerSound Smith29";
	case TSFX_SMITH30:
		return "TownerSound Smith30";
	case TSFX_SMITH31:
		return "TownerSound Smith31";
	case TSFX_SMITH32:
		return "TownerSound Smith32";
	case TSFX_SMITH33:
		return "TownerSound Smith33";
	case TSFX_SMITH34:
		return "TownerSound Smith34";
	case TSFX_SMITH35:
		return "TownerSound Smith35";
	case TSFX_SMITH36:
		return "TownerSound Smith36";
	case TSFX_SMITH37:
		return "TownerSound Smith37";
	case TSFX_SMITH38:
		return "TownerSound Smith38";
	case TSFX_SMITH39:
		return "TownerSound Smith39";
	case TSFX_SMITH40:
		return "TownerSound Smith40";
	case TSFX_SMITH41:
		return "TownerSound Smith41";
	case TSFX_SMITH42:
		return "TownerSound Smith42";
	case TSFX_SMITH43:
		return "TownerSound Smith43";
	case TSFX_SMITH44:
		return "TownerSound Smith44";
	case TSFX_SMITH45:
		return "TownerSound Smith45";
	case TSFX_SMITH46:
		return "TownerSound Smith46";
	case TSFX_SMITH47:
		return "TownerSound Smith47";
	case TSFX_SMITH48:
		return "TownerSound Smith48";
	case TSFX_SMITH49:
		return "TownerSound Smith49";
	case TSFX_SMITH50:
		return "TownerSound Smith50";
	case TSFX_SMITH51:
		return "TownerSound Smith51";
	case TSFX_SMITH52:
		return "TownerSound Smith52";
	case TSFX_SMITH53:
		return "TownerSound Smith53";
	case TSFX_SMITH54:
		return "TownerSound Smith54";
	case TSFX_SMITH55:
		return "TownerSound Smith55";
	case TSFX_SMITH56:
		return "TownerSound Smith56";
	case TSFX_COW1:
		return "TownerSound Cow1";
	case TSFX_COW2:
		return "TownerSound Cow2";
	case TSFX_COW7:
		return "TownerSound Cow7";
	case TSFX_COW8:
		return "TownerSound Cow8";
	case TSFX_DEADGUY:
		return "TownerSound Deadguy";
	case TSFX_DRUNK1:
		return "TownerSound Drunk1";
	case TSFX_DRUNK2:
		return "TownerSound Drunk2";
	case TSFX_DRUNK3:
		return "TownerSound Drunk3";
	case TSFX_DRUNK4:
		return "TownerSound Drunk4";
	case TSFX_DRUNK5:
		return "TownerSound Drunk5";
	case TSFX_DRUNK6:
		return "TownerSound Drunk6";
	case TSFX_DRUNK7:
		return "TownerSound Drunk7";
	case TSFX_DRUNK8:
		return "TownerSound Drunk8";
	case TSFX_DRUNK9:
		return "TownerSound Drunk9";
	case TSFX_DRUNK10:
		return "TownerSound Drunk10";
	case TSFX_DRUNK11:
		return "TownerSound Drunk11";
	case TSFX_DRUNK12:
		return "TownerSound Drunk12";
	case TSFX_DRUNK13:
		return "TownerSound Drunk13";
	case TSFX_DRUNK14:
		return "TownerSound Drunk14";
	case TSFX_DRUNK15:
		return "TownerSound Drunk15";
	case TSFX_DRUNK16:
		return "TownerSound Drunk16";
	case TSFX_DRUNK17:
		return "TownerSound Drunk17";
	case TSFX_DRUNK18:
		return "TownerSound Drunk18";
	case TSFX_DRUNK19:
		return "TownerSound Drunk19";
	case TSFX_DRUNK20:
		return "TownerSound Drunk20";
	case TSFX_DRUNK21:
		return "TownerSound Drunk21";
	case TSFX_DRUNK22:
		return "TownerSound Drunk22";
	case TSFX_DRUNK23:
		return "TownerSound Drunk23";
	case TSFX_DRUNK24:
		return "TownerSound Drunk24";
	case TSFX_DRUNK25:
		return "TownerSound Drunk25";
	case TSFX_DRUNK26:
		return "TownerSound Drunk26";
	case TSFX_DRUNK27:
		return "TownerSound Drunk27";
	case TSFX_DRUNK28:
		return "TownerSound Drunk28";
	case TSFX_DRUNK29:
		return "TownerSound Drunk29";
	case TSFX_DRUNK30:
		return "TownerSound Drunk30";
	case TSFX_DRUNK31:
		return "TownerSound Drunk31";
	case TSFX_DRUNK32:
		return "TownerSound Drunk32";
	case TSFX_DRUNK33:
		return "TownerSound Drunk33";
	case TSFX_DRUNK34:
		return "TownerSound Drunk34";
	case TSFX_DRUNK35:
		return "TownerSound Drunk35";
	case TSFX_HEALER1:
		return "TownerSound Healer1";
	case TSFX_HEALER2:
		return "TownerSound Healer2";
	case TSFX_HEALER3:
		return "TownerSound Healer3";
	case TSFX_HEALER4:
		return "TownerSound Healer4";
	case TSFX_HEALER5:
		return "TownerSound Healer5";
	case TSFX_HEALER6:
		return "TownerSound Healer6";
	case TSFX_HEALER7:
		return "TownerSound Healer7";
	case TSFX_HEALER8:
		return "TownerSound Healer8";
	case TSFX_HEALER9:
		return "TownerSound Healer9";
	case TSFX_HEALER10:
		return "TownerSound Healer10";
	case TSFX_HEALER11:
		return "TownerSound Healer11";
	case TSFX_HEALER12:
		return "TownerSound Healer12";
	case TSFX_HEALER13:
		return "TownerSound Healer13";
	case TSFX_HEALER14:
		return "TownerSound Healer14";
	case TSFX_HEALER15:
		return "TownerSound Healer15";
	case TSFX_HEALER16:
		return "TownerSound Healer16";
	case TSFX_HEALER17:
		return "TownerSound Healer17";
	case TSFX_HEALER18:
		return "TownerSound Healer18";
	case TSFX_HEALER19:
		return "TownerSound Healer19";
	case TSFX_HEALER20:
		return "TownerSound Healer20";
	case TSFX_HEALER21:
		return "TownerSound Healer21";
	case TSFX_HEALER22:
		return "TownerSound Healer22";
	case TSFX_HEALER23:
		return "TownerSound Healer23";
	case TSFX_HEALER24:
		return "TownerSound Healer24";
	case TSFX_HEALER25:
		return "TownerSound Healer25";
	case TSFX_HEALER26:
		return "TownerSound Healer26";
	case TSFX_HEALER27:
		return "TownerSound Healer27";
	case TSFX_HEALER28:
		return "TownerSound Healer28";
	case TSFX_HEALER29:
		return "TownerSound Healer29";
	case TSFX_HEALER30:
		return "TownerSound Healer30";
	case TSFX_HEALER31:
		return "TownerSound Healer31";
	case TSFX_HEALER32:
		return "TownerSound Healer32";
	case TSFX_HEALER33:
		return "TownerSound Healer33";
	case TSFX_HEALER34:
		return "TownerSound Healer34";
	case TSFX_HEALER35:
		return "TownerSound Healer35";
	case TSFX_HEALER36:
		return "TownerSound Healer36";
	case TSFX_HEALER37:
		return "TownerSound Healer37";
	case TSFX_HEALER38:
		return "TownerSound Healer38";
	case TSFX_HEALER39:
		return "TownerSound Healer39";
	case TSFX_HEALER40:
		return "TownerSound Healer40";
	case TSFX_HEALER41:
		return "TownerSound Healer41";
	case TSFX_HEALER42:
		return "TownerSound Healer42";
	case TSFX_HEALER43:
		return "TownerSound Healer43";
	case TSFX_HEALER44:
		return "TownerSound Healer44";
	case TSFX_HEALER45:
		return "TownerSound Healer45";
	case TSFX_HEALER46:
		return "TownerSound Healer46";
	case TSFX_HEALER47:
		return "TownerSound Healer47";
	case TSFX_PEGBOY1:
		return "TownerSound Pegboy1";
	case TSFX_PEGBOY2:
		return "TownerSound Pegboy2";
	case TSFX_PEGBOY3:
		return "TownerSound Pegboy3";
	case TSFX_PEGBOY4:
		return "TownerSound Pegboy4";
	case TSFX_PEGBOY5:
		return "TownerSound Pegboy5";
	case TSFX_PEGBOY6:
		return "TownerSound Pegboy6";
	case TSFX_PEGBOY7:
		return "TownerSound Pegboy7";
	case TSFX_PEGBOY8:
		return "TownerSound Pegboy8";
	case TSFX_PEGBOY9:
		return "TownerSound Pegboy9";
	case TSFX_PEGBOY10:
		return "TownerSound Pegboy10";
	case TSFX_PEGBOY11:
		return "TownerSound Pegboy11";
	case TSFX_PEGBOY12:
		return "TownerSound Pegboy12";
	case TSFX_PEGBOY13:
		return "TownerSound Pegboy13";
	case TSFX_PEGBOY14:
		return "TownerSound Pegboy14";
	case TSFX_PEGBOY15:
		return "TownerSound Pegboy15";
	case TSFX_PEGBOY16:
		return "TownerSound Pegboy16";
	case TSFX_PEGBOY17:
		return "TownerSound Pegboy17";
	case TSFX_PEGBOY18:
		return "TownerSound Pegboy18";
	case TSFX_PEGBOY19:
		return "TownerSound Pegboy19";
	case TSFX_PEGBOY20:
		return "TownerSound Pegboy20";
	case TSFX_PEGBOY21:
		return "TownerSound Pegboy21";
	case TSFX_PEGBOY22:
		return "TownerSound Pegboy22";
	case TSFX_PEGBOY23:
		return "TownerSound Pegboy23";
	case TSFX_PEGBOY24:
		return "TownerSound Pegboy24";
	case TSFX_PEGBOY25:
		return "TownerSound Pegboy25";
	case TSFX_PEGBOY26:
		return "TownerSound Pegboy26";
	case TSFX_PEGBOY27:
		return "TownerSound Pegboy27";
	case TSFX_PEGBOY28:
		return "TownerSound Pegboy28";
	case TSFX_PEGBOY29:
		return "TownerSound Pegboy29";
	case TSFX_PEGBOY30:
		return "TownerSound Pegboy30";
	case TSFX_PEGBOY31:
		return "TownerSound Pegboy31";
	case TSFX_PEGBOY32:
		return "TownerSound Pegboy32";
	case TSFX_PEGBOY33:
		return "TownerSound Pegboy33";
	case TSFX_PEGBOY34:
		return "TownerSound Pegboy34";
	case TSFX_PEGBOY35:
		return "TownerSound Pegboy35";
	case TSFX_PEGBOY36:
		return "TownerSound Pegboy36";
	case TSFX_PEGBOY37:
		return "TownerSound Pegboy37";
	case TSFX_PEGBOY38:
		return "TownerSound Pegboy38";
	case TSFX_PEGBOY39:
		return "TownerSound Pegboy39";
	case TSFX_PEGBOY40:
		return "TownerSound Pegboy40";
	case TSFX_PEGBOY41:
		return "TownerSound Pegboy41";
	case TSFX_PEGBOY42:
		return "TownerSound Pegboy42";
	case TSFX_PEGBOY43:
		return "TownerSound Pegboy43";
	case TSFX_PRIEST0:
		return "TownerSound Priest0";
	case TSFX_PRIEST1:
		return "TownerSound Priest1";
	case TSFX_PRIEST2:
		return "TownerSound Priest2";
	case TSFX_PRIEST3:
		return "TownerSound Priest3";
	case TSFX_PRIEST4:
		return "TownerSound Priest4";
	case TSFX_PRIEST5:
		return "TownerSound Priest5";
	case TSFX_PRIEST6:
		return "TownerSound Priest6";
	case TSFX_PRIEST7:
		return "TownerSound Priest7";
	case TSFX_STORY0:
		return "TownerSound Story0";
	case TSFX_STORY1:
		return "TownerSound Story1";
	case TSFX_STORY2:
		return "TownerSound Story2";
	case TSFX_STORY3:
		return "TownerSound Story3";
	case TSFX_STORY4:
		return "TownerSound Story4";
	case TSFX_STORY5:
		return "TownerSound Story5";
	case TSFX_STORY6:
		return "TownerSound Story6";
	case TSFX_STORY7:
		return "TownerSound Story7";
	case TSFX_STORY8:
		return "TownerSound Story8";
	case TSFX_STORY9:
		return "TownerSound Story9";
	case TSFX_STORY10:
		return "TownerSound Story10";
	case TSFX_STORY11:
		return "TownerSound Story11";
	case TSFX_STORY12:
		return "TownerSound Story12";
	case TSFX_STORY13:
		return "TownerSound Story13";
	case TSFX_STORY14:
		return "TownerSound Story14";
	case TSFX_STORY15:
		return "TownerSound Story15";
	case TSFX_STORY16:
		return "TownerSound Story16";
	case TSFX_STORY17:
		return "TownerSound Story17";
	case TSFX_STORY18:
		return "TownerSound Story18";
	case TSFX_STORY19:
		return "TownerSound Story19";
	case TSFX_STORY20:
		return "TownerSound Story20";
	case TSFX_STORY21:
		return "TownerSound Story21";
	case TSFX_STORY22:
		return "TownerSound Story22";
	case TSFX_STORY23:
		return "TownerSound Story23";
	case TSFX_STORY24:
		return "TownerSound Story24";
	case TSFX_STORY25:
		return "TownerSound Story25";
	case TSFX_STORY26:
		return "TownerSound Story26";
	case TSFX_STORY27:
		return "TownerSound Story27";
	case TSFX_STORY28:
		return "TownerSound Story28";
	case TSFX_STORY29:
		return "TownerSound Story29";
	case TSFX_STORY30:
		return "TownerSound Story30";
	case TSFX_STORY31:
		return "TownerSound Story31";
	case TSFX_STORY32:
		return "TownerSound Story32";
	case TSFX_STORY33:
		return "TownerSound Story33";
	case TSFX_STORY34:
		return "TownerSound Story34";
	case TSFX_STORY35:
		return "TownerSound Story35";
	case TSFX_STORY36:
		return "TownerSound Story36";
	case TSFX_STORY37:
		return "TownerSound Story37";
	case TSFX_STORY38:
		return "TownerSound Story38";
	case TSFX_TAVERN0:
		return "TownerSound Tavern0";
	case TSFX_TAVERN1:
		return "TownerSound Tavern1";
	case TSFX_TAVERN2:
		return "TownerSound Tavern2";
	case TSFX_TAVERN3:
		return "TownerSound Tavern3";
	case TSFX_TAVERN4:
		return "TownerSound Tavern4";
	case TSFX_TAVERN5:
		return "TownerSound Tavern5";
	case TSFX_TAVERN6:
		return "TownerSound Tavern6";
	case TSFX_TAVERN7:
		return "TownerSound Tavern7";
	case TSFX_TAVERN8:
		return "TownerSound Tavern8";
	case TSFX_TAVERN9:
		return "TownerSound Tavern9";
	case TSFX_TAVERN10:
		return "TownerSound Tavern10";
	case TSFX_TAVERN11:
		return "TownerSound Tavern11";
	case TSFX_TAVERN12:
		return "TownerSound Tavern12";
	case TSFX_TAVERN13:
		return "TownerSound Tavern13";
	case TSFX_TAVERN14:
		return "TownerSound Tavern14";
	case TSFX_TAVERN15:
		return "TownerSound Tavern15";
	case TSFX_TAVERN16:
		return "TownerSound Tavern16";
	case TSFX_TAVERN17:
		return "TownerSound Tavern17";
	case TSFX_TAVERN18:
		return "TownerSound Tavern18";
	case TSFX_TAVERN19:
		return "TownerSound Tavern19";
	case TSFX_TAVERN20:
		return "TownerSound Tavern20";
	case TSFX_TAVERN21:
		return "TownerSound Tavern21";
	case TSFX_TAVERN22:
		return "TownerSound Tavern22";
	case TSFX_TAVERN23:
		return "TownerSound Tavern23";
	case TSFX_TAVERN24:
		return "TownerSound Tavern24";
	case TSFX_TAVERN25:
		return "TownerSound Tavern25";
	case TSFX_TAVERN26:
		return "TownerSound Tavern26";
	case TSFX_TAVERN27:
		return "TownerSound Tavern27";
	case TSFX_TAVERN28:
		return "TownerSound Tavern28";
	case TSFX_TAVERN29:
		return "TownerSound Tavern29";
	case TSFX_TAVERN30:
		return "TownerSound Tavern30";
	case TSFX_TAVERN31:
		return "TownerSound Tavern31";
	case TSFX_TAVERN32:
		return "TownerSound Tavern32";
	case TSFX_TAVERN33:
		return "TownerSound Tavern33";
	case TSFX_TAVERN34:
		return "TownerSound Tavern34";
	case TSFX_TAVERN35:
		return "TownerSound Tavern35";
	case TSFX_TAVERN36:
		return "TownerSound Tavern36";
	case TSFX_TAVERN37:
		return "TownerSound Tavern37";
	case TSFX_TAVERN38:
		return "TownerSound Tavern38";
	case TSFX_TAVERN39:
		return "TownerSound Tavern39";
	case TSFX_TAVERN40:
		return "TownerSound Tavern40";
	case TSFX_TAVERN41:
		return "TownerSound Tavern41";
	case TSFX_TAVERN42:
		return "TownerSound Tavern42";
	case TSFX_TAVERN43:
		return "TownerSound Tavern43";
	case TSFX_TAVERN44:
		return "TownerSound Tavern44";
	case TSFX_TAVERN45:
		return "TownerSound Tavern45";
	case TSFX_WITCH1:
		return "TownerSound Witch1";
	case TSFX_WITCH2:
		return "TownerSound Witch2";
	case TSFX_WITCH3:
		return "TownerSound Witch3";
	case TSFX_WITCH4:
		return "TownerSound Witch4";
	case TSFX_WITCH5:
		return "TownerSound Witch5";
	case TSFX_WITCH6:
		return "TownerSound Witch6";
	case TSFX_WITCH7:
		return "TownerSound Witch7";
	case TSFX_WITCH8:
		return "TownerSound Witch8";
	case TSFX_WITCH9:
		return "TownerSound Witch9";
	case TSFX_WITCH10:
		return "TownerSound Witch10";
	case TSFX_WITCH11:
		return "TownerSound Witch11";
	case TSFX_WITCH12:
		return "TownerSound Witch12";
	case TSFX_WITCH13:
		return "TownerSound Witch13";
	case TSFX_WITCH14:
		return "TownerSound Witch14";
	case TSFX_WITCH15:
		return "TownerSound Witch15";
	case TSFX_WITCH16:
		return "TownerSound Witch16";
	case TSFX_WITCH17:
		return "TownerSound Witch17";
	case TSFX_WITCH18:
		return "TownerSound Witch18";
	case TSFX_WITCH19:
		return "TownerSound Witch19";
	case TSFX_WITCH20:
		return "TownerSound Witch20";
	case TSFX_WITCH21:
		return "TownerSound Witch21";
	case TSFX_WITCH22:
		return "TownerSound Witch22";
	case TSFX_WITCH23:
		return "TownerSound Witch23";
	case TSFX_WITCH24:
		return "TownerSound Witch24";
	case TSFX_WITCH25:
		return "TownerSound Witch25";
	case TSFX_WITCH26:
		return "TownerSound Witch26";
	case TSFX_WITCH27:
		return "TownerSound Witch27";
	case TSFX_WITCH28:
		return "TownerSound Witch28";
	case TSFX_WITCH29:
		return "TownerSound Witch29";
	case TSFX_WITCH30:
		return "TownerSound Witch30";
	case TSFX_WITCH31:
		return "TownerSound Witch31";
	case TSFX_WITCH32:
		return "TownerSound Witch32";
	case TSFX_WITCH33:
		return "TownerSound Witch33";
	case TSFX_WITCH34:
		return "TownerSound Witch34";
	case TSFX_WITCH35:
		return "TownerSound Witch35";
	case TSFX_WITCH36:
		return "TownerSound Witch36";
	case TSFX_WITCH37:
		return "TownerSound Witch37";
	case TSFX_WITCH38:
		return "TownerSound Witch38";
	case TSFX_WITCH39:
		return "TownerSound Witch39";
	case TSFX_WITCH40:
		return "TownerSound Witch40";
	case TSFX_WITCH41:
		return "TownerSound Witch41";
	case TSFX_WITCH42:
		return "TownerSound Witch42";
	case TSFX_WITCH43:
		return "TownerSound Witch43";
	case TSFX_WITCH44:
		return "TownerSound Witch44";
	case TSFX_WITCH45:
		return "TownerSound Witch45";
	case TSFX_WITCH46:
		return "TownerSound Witch46";
	case TSFX_WITCH47:
		return "TownerSound Witch47";
	case TSFX_WITCH48:
		return "TownerSound Witch48";
	case TSFX_WITCH49:
		return "TownerSound Witch49";
	case TSFX_WITCH50:
		return "TownerSound Witch50";
	case TSFX_WOUND:
		return "TownerSound Wound";
	case PS_MAGE1:
		return "PlayerSound Mage1";
	case PS_MAGE2:
		return "PlayerSound Mage2";
	case PS_MAGE3:
		return "PlayerSound Mage3";
	case PS_MAGE4:
		return "PlayerSound Mage4";
	case PS_MAGE5:
		return "PlayerSound Mage5";
	case PS_MAGE6:
		return "PlayerSound Mage6";
	case PS_MAGE7:
		return "PlayerSound Mage7";
	case PS_MAGE8:
		return "PlayerSound Mage8";
	case PS_MAGE9:
		return "PlayerSound Mage9";
	case PS_MAGE10:
		return "PlayerSound Mage10";
	case PS_MAGE11:
		return "PlayerSound Mage11";
	case PS_MAGE12:
		return "PlayerSound Mage12";
	case PS_MAGE13:
		return "PlayerSound Mage13";
	case PS_MAGE14:
		return "PlayerSound Mage14";
	case PS_MAGE15:
		return "PlayerSound Mage15";
	case PS_MAGE16:
		return "PlayerSound Mage16";
	case PS_MAGE17:
		return "PlayerSound Mage17";
	case PS_MAGE18:
		return "PlayerSound Mage18";
	case PS_MAGE19:
		return "PlayerSound Mage19";
	case PS_MAGE20:
		return "PlayerSound Mage20";
	case PS_MAGE21:
		return "PlayerSound Mage21";
	case PS_MAGE22:
		return "PlayerSound Mage22";
	case PS_MAGE23:
		return "PlayerSound Mage23";
	case PS_MAGE24:
		return "PlayerSound Mage24";
	case PS_MAGE25:
		return "PlayerSound Mage25";
	case PS_MAGE26:
		return "PlayerSound Mage26";
	case PS_MAGE27:
		return "PlayerSound Mage27";
	case PS_MAGE28:
		return "PlayerSound Mage28";
	case PS_MAGE29:
		return "PlayerSound Mage29";
	case PS_MAGE30:
		return "PlayerSound Mage30";
	case PS_MAGE31:
		return "PlayerSound Mage31";
	case PS_MAGE32:
		return "PlayerSound Mage32";
	case PS_MAGE33:
		return "PlayerSound Mage33";
	case PS_MAGE34:
		return "PlayerSound Mage34";
	case PS_MAGE35:
		return "PlayerSound Mage35";
	case PS_MAGE36:
		return "PlayerSound Mage36";
	case PS_MAGE37:
		return "PlayerSound Mage37";
	case PS_MAGE38:
		return "PlayerSound Mage38";
	case PS_MAGE39:
		return "PlayerSound Mage39";
	case PS_MAGE40:
		return "PlayerSound Mage40";
	case PS_MAGE41:
		return "PlayerSound Mage41";
	case PS_MAGE42:
		return "PlayerSound Mage42";
	case PS_MAGE43:
		return "PlayerSound Mage43";
	case PS_MAGE44:
		return "PlayerSound Mage44";
	case PS_MAGE45:
		return "PlayerSound Mage45";
	case PS_MAGE46:
		return "PlayerSound Mage46";
	case PS_MAGE47:
		return "PlayerSound Mage47";
	case PS_MAGE48:
		return "PlayerSound Mage48";
	case PS_MAGE49:
		return "PlayerSound Mage49";
	case PS_MAGE50:
		return "PlayerSound Mage50";
	case PS_MAGE51:
		return "PlayerSound Mage51";
	case PS_MAGE52:
		return "PlayerSound Mage52";
	case PS_MAGE53:
		return "PlayerSound Mage53";
	case PS_MAGE54:
		return "PlayerSound Mage54";
	case PS_MAGE55:
		return "PlayerSound Mage55";
	case PS_MAGE56:
		return "PlayerSound Mage56";
	case PS_MAGE57:
		return "PlayerSound Mage57";
	case PS_MAGE58:
		return "PlayerSound Mage58";
	case PS_MAGE59:
		return "PlayerSound Mage59";
	case PS_MAGE60:
		return "PlayerSound Mage60";
	case PS_MAGE61:
		return "PlayerSound Mage61";
	case PS_MAGE62:
		return "PlayerSound Mage62";
	case PS_MAGE63:
		return "PlayerSound Mage63";
	case PS_MAGE64:
		return "PlayerSound Mage64";
	case PS_MAGE65:
		return "PlayerSound Mage65";
	case PS_MAGE66:
		return "PlayerSound Mage66";
	case PS_MAGE67:
		return "PlayerSound Mage67";
	case PS_MAGE68:
		return "PlayerSound Mage68";
	case PS_MAGE69:
		return "PlayerSound Mage69";
	case PS_MAGE69B:
		return "PlayerSound Mage69b";
	case PS_MAGE70:
		return "PlayerSound Mage70";
	case PS_MAGE71:
		return "PlayerSound Mage71";
	case PS_MAGE72:
		return "PlayerSound Mage72";
	case PS_MAGE73:
		return "PlayerSound Mage73";
	case PS_MAGE74:
		return "PlayerSound Mage74";
	case PS_MAGE75:
		return "PlayerSound Mage75";
	case PS_MAGE76:
		return "PlayerSound Mage76";
	case PS_MAGE77:
		return "PlayerSound Mage77";
	case PS_MAGE78:
		return "PlayerSound Mage78";
	case PS_MAGE79:
		return "PlayerSound Mage79";
	case PS_MAGE80:
		return "PlayerSound Mage80";
	case PS_MAGE81:
		return "PlayerSound Mage81";
	case PS_MAGE82:
		return "PlayerSound Mage82";
	case PS_MAGE83:
		return "PlayerSound Mage83";
	case PS_MAGE84:
		return "PlayerSound Mage84";
	case PS_MAGE85:
		return "PlayerSound Mage85";
	case PS_MAGE86:
		return "PlayerSound Mage86";
	case PS_MAGE87:
		return "PlayerSound Mage87";
	case PS_MAGE88:
		return "PlayerSound Mage88";
	case PS_MAGE89:
		return "PlayerSound Mage89";
	case PS_MAGE90:
		return "PlayerSound Mage90";
	case PS_MAGE91:
		return "PlayerSound Mage91";
	case PS_MAGE92:
		return "PlayerSound Mage92";
	case PS_MAGE93:
		return "PlayerSound Mage93";
	case PS_MAGE94:
		return "PlayerSound Mage94";
	case PS_MAGE95:
		return "PlayerSound Mage95";
	case PS_MAGE96:
		return "PlayerSound Mage96";
	case PS_MAGE97:
		return "PlayerSound Mage97";
	case PS_MAGE98:
		return "PlayerSound Mage98";
	case PS_MAGE99:
		return "PlayerSound Mage99";
	case PS_MAGE100:
		return "PlayerSound Mage100";
	case PS_MAGE101:
		return "PlayerSound Mage101";
	case PS_MAGE102:
		return "PlayerSound Mage102";
	case PS_ROGUE1:
		return "PlayerSound Rogue1";
	case PS_ROGUE2:
		return "PlayerSound Rogue2";
	case PS_ROGUE3:
		return "PlayerSound Rogue3";
	case PS_ROGUE4:
		return "PlayerSound Rogue4";
	case PS_ROGUE5:
		return "PlayerSound Rogue5";
	case PS_ROGUE6:
		return "PlayerSound Rogue6";
	case PS_ROGUE7:
		return "PlayerSound Rogue7";
	case PS_ROGUE8:
		return "PlayerSound Rogue8";
	case PS_ROGUE9:
		return "PlayerSound Rogue9";
	case PS_ROGUE10:
		return "PlayerSound Rogue10";
	case PS_ROGUE11:
		return "PlayerSound Rogue11";
	case PS_ROGUE12:
		return "PlayerSound Rogue12";
	case PS_ROGUE13:
		return "PlayerSound Rogue13";
	case PS_ROGUE14:
		return "PlayerSound Rogue14";
	case PS_ROGUE15:
		return "PlayerSound Rogue15";
	case PS_ROGUE16:
		return "PlayerSound Rogue16";
	case PS_ROGUE17:
		return "PlayerSound Rogue17";
	case PS_ROGUE18:
		return "PlayerSound Rogue18";
	case PS_ROGUE19:
		return "PlayerSound Rogue19";
	case PS_ROGUE20:
		return "PlayerSound Rogue20";
	case PS_ROGUE21:
		return "PlayerSound Rogue21";
	case PS_ROGUE22:
		return "PlayerSound Rogue22";
	case PS_ROGUE23:
		return "PlayerSound Rogue23";
	case PS_ROGUE24:
		return "PlayerSound Rogue24";
	case PS_ROGUE25:
		return "PlayerSound Rogue25";
	case PS_ROGUE26:
		return "PlayerSound Rogue26";
	case PS_ROGUE27:
		return "PlayerSound Rogue27";
	case PS_ROGUE28:
		return "PlayerSound Rogue28";
	case PS_ROGUE29:
		return "PlayerSound Rogue29";
	case PS_ROGUE30:
		return "PlayerSound Rogue30";
	case PS_ROGUE31:
		return "PlayerSound Rogue31";
	case PS_ROGUE32:
		return "PlayerSound Rogue32";
	case PS_ROGUE33:
		return "PlayerSound Rogue33";
	case PS_ROGUE34:
		return "PlayerSound Rogue34";
	case PS_ROGUE35:
		return "PlayerSound Rogue35";
	case PS_ROGUE36:
		return "PlayerSound Rogue36";
	case PS_ROGUE37:
		return "PlayerSound Rogue37";
	case PS_ROGUE38:
		return "PlayerSound Rogue38";
	case PS_ROGUE39:
		return "PlayerSound Rogue39";
	case PS_ROGUE40:
		return "PlayerSound Rogue40";
	case PS_ROGUE41:
		return "PlayerSound Rogue41";
	case PS_ROGUE42:
		return "PlayerSound Rogue42";
	case PS_ROGUE43:
		return "PlayerSound Rogue43";
	case PS_ROGUE44:
		return "PlayerSound Rogue44";
	case PS_ROGUE45:
		return "PlayerSound Rogue45";
	case PS_ROGUE46:
		return "PlayerSound Rogue46";
	case PS_ROGUE47:
		return "PlayerSound Rogue47";
	case PS_ROGUE48:
		return "PlayerSound Rogue48";
	case PS_ROGUE49:
		return "PlayerSound Rogue49";
	case PS_ROGUE50:
		return "PlayerSound Rogue50";
	case PS_ROGUE51:
		return "PlayerSound Rogue51";
	case PS_ROGUE52:
		return "PlayerSound Rogue52";
	case PS_ROGUE53:
		return "PlayerSound Rogue53";
	case PS_ROGUE54:
		return "PlayerSound Rogue54";
	case PS_ROGUE55:
		return "PlayerSound Rogue55";
	case PS_ROGUE56:
		return "PlayerSound Rogue56";
	case PS_ROGUE57:
		return "PlayerSound Rogue57";
	case PS_ROGUE58:
		return "PlayerSound Rogue58";
	case PS_ROGUE59:
		return "PlayerSound Rogue59";
	case PS_ROGUE60:
		return "PlayerSound Rogue60";
	case PS_ROGUE61:
		return "PlayerSound Rogue61";
	case PS_ROGUE62:
		return "PlayerSound Rogue62";
	case PS_ROGUE63:
		return "PlayerSound Rogue63";
	case PS_ROGUE64:
		return "PlayerSound Rogue64";
	case PS_ROGUE65:
		return "PlayerSound Rogue65";
	case PS_ROGUE66:
		return "PlayerSound Rogue66";
	case PS_ROGUE67:
		return "PlayerSound Rogue67";
	case PS_ROGUE68:
		return "PlayerSound Rogue68";
	case PS_ROGUE69:
		return "PlayerSound Rogue69";
	case PS_ROGUE69B:
		return "PlayerSound Rogue69b";
	case PS_ROGUE70:
		return "PlayerSound Rogue70";
	case PS_ROGUE71:
		return "PlayerSound Rogue71";
	case PS_ROGUE72:
		return "PlayerSound Rogue72";
	case PS_ROGUE73:
		return "PlayerSound Rogue73";
	case PS_ROGUE74:
		return "PlayerSound Rogue74";
	case PS_ROGUE75:
		return "PlayerSound Rogue75";
	case PS_ROGUE76:
		return "PlayerSound Rogue76";
	case PS_ROGUE77:
		return "PlayerSound Rogue77";
	case PS_ROGUE78:
		return "PlayerSound Rogue78";
	case PS_ROGUE79:
		return "PlayerSound Rogue79";
	case PS_ROGUE80:
		return "PlayerSound Rogue80";
	case PS_ROGUE81:
		return "PlayerSound Rogue81";
	case PS_ROGUE82:
		return "PlayerSound Rogue82";
	case PS_ROGUE83:
		return "PlayerSound Rogue83";
	case PS_ROGUE84:
		return "PlayerSound Rogue84";
	case PS_ROGUE85:
		return "PlayerSound Rogue85";
	case PS_ROGUE86:
		return "PlayerSound Rogue86";
	case PS_ROGUE87:
		return "PlayerSound Rogue87";
	case PS_ROGUE88:
		return "PlayerSound Rogue88";
	case PS_ROGUE89:
		return "PlayerSound Rogue89";
	case PS_ROGUE90:
		return "PlayerSound Rogue90";
	case PS_ROGUE91:
		return "PlayerSound Rogue91";
	case PS_ROGUE92:
		return "PlayerSound Rogue92";
	case PS_ROGUE93:
		return "PlayerSound Rogue93";
	case PS_ROGUE94:
		return "PlayerSound Rogue94";
	case PS_ROGUE95:
		return "PlayerSound Rogue95";
	case PS_ROGUE96:
		return "PlayerSound Rogue96";
	case PS_ROGUE97:
		return "PlayerSound Rogue97";
	case PS_ROGUE98:
		return "PlayerSound Rogue98";
	case PS_ROGUE99:
		return "PlayerSound Rogue99";
	case PS_ROGUE100:
		return "PlayerSound Rogue100";
	case PS_ROGUE101:
		return "PlayerSound Rogue101";
	case PS_ROGUE102:
		return "PlayerSound Rogue102";
	case PS_WARR1:
		return "PlayerSound Warr1";
	case PS_WARR2:
		return "PlayerSound Warr2";
	case PS_WARR3:
		return "PlayerSound Warr3";
	case PS_WARR4:
		return "PlayerSound Warr4";
	case PS_WARR5:
		return "PlayerSound Warr5";
	case PS_WARR6:
		return "PlayerSound Warr6";
	case PS_WARR7:
		return "PlayerSound Warr7";
	case PS_WARR8:
		return "PlayerSound Warr8";
	case PS_WARR9:
		return "PlayerSound Warr9";
	case PS_WARR10:
		return "PlayerSound Warr10";
	case PS_WARR11:
		return "PlayerSound Warr11";
	case PS_WARR12:
		return "PlayerSound Warr12";
	case PS_WARR13:
		return "PlayerSound Warr13";
	case PS_WARR14:
		return "PlayerSound Warr14";
	case PS_WARR14B:
		return "PlayerSound Warr14b";
	case PS_WARR14C:
		return "PlayerSound Warr14c";
	case PS_WARR15:
		return "PlayerSound Warr15";
	case PS_WARR15B:
		return "PlayerSound Warr15b";
	case PS_WARR15C:
		return "PlayerSound Warr15c";
	case PS_WARR16:
		return "PlayerSound Warr16";
	case PS_WARR16B:
		return "PlayerSound Warr16b";
	case PS_WARR16C:
		return "PlayerSound Warr16c";
	case PS_WARR17:
		return "PlayerSound Warr17";
	case PS_WARR18:
		return "PlayerSound Warr18";
	case PS_WARR19:
		return "PlayerSound Warr19";
	case PS_WARR20:
		return "PlayerSound Warr20";
	case PS_WARR21:
		return "PlayerSound Warr21";
	case PS_WARR22:
		return "PlayerSound Warr22";
	case PS_WARR23:
		return "PlayerSound Warr23";
	case PS_WARR24:
		return "PlayerSound Warr24";
	case PS_WARR25:
		return "PlayerSound Warr25";
	case PS_WARR26:
		return "PlayerSound Warr26";
	case PS_WARR27:
		return "PlayerSound Warr27";
	case PS_WARR28:
		return "PlayerSound Warr28";
	case PS_WARR29:
		return "PlayerSound Warr29";
	case PS_WARR30:
		return "PlayerSound Warr30";
	case PS_WARR31:
		return "PlayerSound Warr31";
	case PS_WARR32:
		return "PlayerSound Warr32";
	case PS_WARR33:
		return "PlayerSound Warr33";
	case PS_WARR34:
		return "PlayerSound Warr34";
	case PS_WARR35:
		return "PlayerSound Warr35";
	case PS_WARR36:
		return "PlayerSound Warr36";
	case PS_WARR37:
		return "PlayerSound Warr37";
	case PS_WARR38:
		return "PlayerSound Warr38";
	case PS_WARR39:
		return "PlayerSound Warr39";
	case PS_WARR40:
		return "PlayerSound Warr40";
	case PS_WARR41:
		return "PlayerSound Warr41";
	case PS_WARR42:
		return "PlayerSound Warr42";
	case PS_WARR43:
		return "PlayerSound Warr43";
	case PS_WARR44:
		return "PlayerSound Warr44";
	case PS_WARR45:
		return "PlayerSound Warr45";
	case PS_WARR46:
		return "PlayerSound Warr46";
	case PS_WARR47:
		return "PlayerSound Warr47";
	case PS_WARR48:
		return "PlayerSound Warr48";
	case PS_WARR49:
		return "PlayerSound Warr49";
	case PS_WARR50:
		return "PlayerSound Warr50";
	case PS_WARR51:
		return "PlayerSound Warr51";
	case PS_WARR52:
		return "PlayerSound Warr52";
	case PS_WARR53:
		return "PlayerSound Warr53";
	case PS_WARR54:
		return "PlayerSound Warr54";
	case PS_WARR55:
		return "PlayerSound Warr55";
	case PS_WARR56:
		return "PlayerSound Warr56";
	case PS_WARR57:
		return "PlayerSound Warr57";
	case PS_WARR58:
		return "PlayerSound Warr58";
	case PS_WARR59:
		return "PlayerSound Warr59";
	case PS_WARR60:
		return "PlayerSound Warr60";
	case PS_WARR61:
		return "PlayerSound Warr61";
	case PS_WARR62:
		return "PlayerSound Warr62";
	case PS_WARR63:
		return "PlayerSound Warr63";
	case PS_WARR64:
		return "PlayerSound Warr64";
	case PS_WARR65:
		return "PlayerSound Warr65";
	case PS_WARR66:
		return "PlayerSound Warr66";
	case PS_WARR67:
		return "PlayerSound Warr67";
	case PS_WARR68:
		return "PlayerSound Warr68";
	case PS_WARR69:
		return "PlayerSound Warr69";
	case PS_WARR69B:
		return "PlayerSound Warr69b";
	case PS_WARR70:
		return "PlayerSound Warr70";
	case PS_WARR71:
		return "PlayerSound Warr71";
	case PS_WARR72:
		return "PlayerSound Warr72";
	case PS_WARR73:
		return "PlayerSound Warr73";
	case PS_WARR74:
		return "PlayerSound Warr74";
	case PS_WARR75:
		return "PlayerSound Warr75";
	case PS_WARR76:
		return "PlayerSound Warr76";
	case PS_WARR77:
		return "PlayerSound Warr77";
	case PS_WARR78:
		return "PlayerSound Warr78";
	case PS_WARR79:
		return "PlayerSound Warr79";
	case PS_WARR80:
		return "PlayerSound Warr80";
	case PS_WARR81:
		return "PlayerSound Warr81";
	case PS_WARR82:
		return "PlayerSound Warr82";
	case PS_WARR83:
		return "PlayerSound Warr83";
	case PS_WARR84:
		return "PlayerSound Warr84";
	case PS_WARR85:
		return "PlayerSound Warr85";
	case PS_WARR86:
		return "PlayerSound Warr86";
	case PS_WARR87:
		return "PlayerSound Warr87";
	case PS_WARR88:
		return "PlayerSound Warr88";
	case PS_WARR89:
		return "PlayerSound Warr89";
	case PS_WARR90:
		return "PlayerSound Warr90";
	case PS_WARR91:
		return "PlayerSound Warr91";
	case PS_WARR92:
		return "PlayerSound Warr92";
	case PS_WARR93:
		return "PlayerSound Warr93";
	case PS_WARR94:
		return "PlayerSound Warr94";
	case PS_WARR95:
		return "PlayerSound Warr95";
	case PS_WARR95B:
		return "PlayerSound Warr95b";
	case PS_WARR95C:
		return "PlayerSound Warr95c";
	case PS_WARR95D:
		return "PlayerSound Warr95d";
	case PS_WARR95E:
		return "PlayerSound Warr95e";
	case PS_WARR95F:
		return "PlayerSound Warr95f";
	case PS_WARR96B:
		return "PlayerSound Warr96b";
	case PS_WARR97:
		return "PlayerSound Warr97";
	case PS_WARR98:
		return "PlayerSound Warr98";
	case PS_WARR99:
		return "PlayerSound Warr99";
	case PS_WARR100:
		return "PlayerSound Warr100";
	case PS_WARR101:
		return "PlayerSound Warr101";
	case PS_WARR102:
		return "PlayerSound Warr102";
	case PS_MONK1:
		return "PlayerSound Monk1";
	case PS_MONK2:
		return "PlayerSound Monk2";
	case PS_MONK3:
		return "PlayerSound Monk3";
	case PS_MONK4:
		return "PlayerSound Monk4";
	case PS_MONK5:
		return "PlayerSound Monk5";
	case PS_MONK6:
		return "PlayerSound Monk6";
	case PS_MONK7:
		return "PlayerSound Monk7";
	case PS_MONK8:
		return "PlayerSound Monk8";
	case PS_MONK9:
		return "PlayerSound Monk9";
	case PS_MONK10:
		return "PlayerSound Monk10";
	case PS_MONK11:
		return "PlayerSound Monk11";
	case PS_MONK12:
		return "PlayerSound Monk12";
	case PS_MONK13:
		return "PlayerSound Monk13";
	case PS_MONK14:
		return "PlayerSound Monk14";
	case PS_MONK15:
		return "PlayerSound Monk15";
	case PS_MONK16:
		return "PlayerSound Monk16";
	case PS_MONK17:
		return "PlayerSound Monk17";
	case PS_MONK18:
		return "PlayerSound Monk18";
	case PS_MONK19:
		return "PlayerSound Monk19";
	case PS_MONK20:
		return "PlayerSound Monk20";
	case PS_MONK21:
		return "PlayerSound Monk21";
	case PS_MONK22:
		return "PlayerSound Monk22";
	case PS_MONK23:
		return "PlayerSound Monk23";
	case PS_MONK24:
		return "PlayerSound Monk24";
	case PS_MONK25:
		return "PlayerSound Monk25";
	case PS_MONK26:
		return "PlayerSound Monk26";
	case PS_MONK27:
		return "PlayerSound Monk27";
	case PS_MONK28:
		return "PlayerSound Monk28";
	case PS_MONK29:
		return "PlayerSound Monk29";
	case PS_MONK30:
		return "PlayerSound Monk30";
	case PS_MONK31:
		return "PlayerSound Monk31";
	case PS_MONK32:
		return "PlayerSound Monk32";
	case PS_MONK33:
		return "PlayerSound Monk33";
	case PS_MONK34:
		return "PlayerSound Monk34";
	case PS_MONK35:
		return "PlayerSound Monk35";
	case PS_MONK36:
		return "PlayerSound Monk36";
	case PS_MONK37:
		return "PlayerSound Monk37";
	case PS_MONK38:
		return "PlayerSound Monk38";
	case PS_MONK39:
		return "PlayerSound Monk39";
	case PS_MONK40:
		return "PlayerSound Monk40";
	case PS_MONK41:
		return "PlayerSound Monk41";
	case PS_MONK42:
		return "PlayerSound Monk42";
	case PS_MONK43:
		return "PlayerSound Monk43";
	case PS_MONK44:
		return "PlayerSound Monk44";
	case PS_MONK45:
		return "PlayerSound Monk45";
	case PS_MONK46:
		return "PlayerSound Monk46";
	case PS_MONK47:
		return "PlayerSound Monk47";
	case PS_MONK48:
		return "PlayerSound Monk48";
	case PS_MONK49:
		return "PlayerSound Monk49";
	case PS_MONK50:
		return "PlayerSound Monk50";
	case PS_MONK51:
		return "PlayerSound Monk51";
	case PS_MONK52:
		return "PlayerSound Monk52";
	case PS_MONK53:
		return "PlayerSound Monk53";
	case PS_MONK54:
		return "PlayerSound Monk54";
	case PS_MONK55:
		return "PlayerSound Monk55";
	case PS_MONK56:
		return "PlayerSound Monk56";
	case PS_MONK57:
		return "PlayerSound Monk57";
	case PS_MONK58:
		return "PlayerSound Monk58";
	case PS_MONK59:
		return "PlayerSound Monk59";
	case PS_MONK60:
		return "PlayerSound Monk60";
	case PS_MONK61:
		return "PlayerSound Monk61";
	case PS_MONK62:
		return "PlayerSound Monk62";
	case PS_MONK63:
		return "PlayerSound Monk63";
	case PS_MONK64:
		return "PlayerSound Monk64";
	case PS_MONK65:
		return "PlayerSound Monk65";
	case PS_MONK66:
		return "PlayerSound Monk66";
	case PS_MONK67:
		return "PlayerSound Monk67";
	case PS_MONK68:
		return "PlayerSound Monk68";
	case PS_MONK69:
		return "PlayerSound Monk69";
	case PS_MONK69B:
		return "PlayerSound Monk69b";
	case PS_MONK70:
		return "PlayerSound Monk70";
	case PS_MONK71:
		return "PlayerSound Monk71";
	case PS_MONK72:
		return "PlayerSound Monk72";
	case PS_MONK73:
		return "PlayerSound Monk73";
	case PS_MONK74:
		return "PlayerSound Monk74";
	case PS_MONK75:
		return "PlayerSound Monk75";
	case PS_MONK76:
		return "PlayerSound Monk76";
	case PS_MONK77:
		return "PlayerSound Monk77";
	case PS_MONK78:
		return "PlayerSound Monk78";
	case PS_MONK79:
		return "PlayerSound Monk79";
	case PS_MONK80:
		return "PlayerSound Monk80";
	case PS_MONK81:
		return "PlayerSound Monk81";
	case PS_MONK82:
		return "PlayerSound Monk82";
	case PS_MONK83:
		return "PlayerSound Monk83";
	case PS_MONK84:
		return "PlayerSound Monk84";
	case PS_MONK85:
		return "PlayerSound Monk85";
	case PS_MONK86:
		return "PlayerSound Monk86";
	case PS_MONK87:
		return "PlayerSound Monk87";
	case PS_MONK88:
		return "PlayerSound Monk88";
	case PS_MONK89:
		return "PlayerSound Monk89";
	case PS_MONK90:
		return "PlayerSound Monk90";
	case PS_MONK91:
		return "PlayerSound Monk91";
	case PS_MONK92:
		return "PlayerSound Monk92";
	case PS_MONK93:
		return "PlayerSound Monk93";
	case PS_MONK94:
		return "PlayerSound Monk94";
	case PS_MONK95:
		return "PlayerSound Monk95";
	case PS_MONK96:
		return "PlayerSound Monk96";
	case PS_MONK97:
		return "PlayerSound Monk97";
	case PS_MONK98:
		return "PlayerSound Monk98";
	case PS_MONK99:
		return "PlayerSound Monk99";
	case PS_MONK100:
		return "PlayerSound Monk100";
	case PS_MONK101:
		return "PlayerSound Monk101";
	case PS_MONK102:
		return "PlayerSound Monk102";
	case PS_NAR1:
		return "PlayerSound Nar1";
	case PS_NAR2:
		return "PlayerSound Nar2";
	case PS_NAR3:
		return "PlayerSound Nar3";
	case PS_NAR4:
		return "PlayerSound Nar4";
	case PS_NAR5:
		return "PlayerSound Nar5";
	case PS_NAR6:
		return "PlayerSound Nar6";
	case PS_NAR7:
		return "PlayerSound Nar7";
	case PS_NAR8:
		return "PlayerSound Nar8";
	case PS_NAR9:
		return "PlayerSound Nar9";
	case PS_DIABLVLINT:
		return "PlayerSound Diablvlint";
	case USFX_CLEAVER:
		return "MonsterSound Cleaver";
	case USFX_GARBUD1:
		return "MonsterSound Garbud1";
	case USFX_GARBUD2:
		return "MonsterSound Garbud2";
	case USFX_GARBUD3:
		return "MonsterSound Garbud3";
	case USFX_GARBUD4:
		return "MonsterSound Garbud4";
	case USFX_IZUAL1:
		return "MonsterSound Izual1";
	case USFX_LACH1:
		return "MonsterSound Lach1";
	case USFX_LACH2:
		return "MonsterSound Lach2";
	case USFX_LACH3:
		return "MonsterSound Lach3";
	case USFX_LAZ1:
		return "MonsterSound Laz1";
	case USFX_LAZ2:
		return "MonsterSound Laz2";
	case USFX_SKING1:
		return "MonsterSound Sking1";
	case USFX_SNOT1:
		return "MonsterSound Snot1";
	case USFX_SNOT2:
		return "MonsterSound Snot2";
	case USFX_SNOT3:
		return "MonsterSound Snot3";
	case USFX_WARLRD1:
		return "MonsterSound Warlrd1";
	case USFX_WLOCK1:
		return "MonsterSound Wlock1";
	case USFX_ZHAR1:
		return "MonsterSound Zhar1";
	case USFX_ZHAR2:
		return "MonsterSound Zhar2";
	case USFX_DIABLOD:
		return "MonsterSound Diablod";
	case TSFX_FARMER1:
		return "TownerSound Farmer1";
	case TSFX_FARMER2:
		return "TownerSound Farmer2";
	case TSFX_FARMER2A:
		return "TownerSound Farmer2a";
	case TSFX_FARMER3:
		return "TownerSound Farmer3";
	case TSFX_FARMER4:
		return "TownerSound Farmer4";
	case TSFX_FARMER5:
		return "TownerSound Farmer5";
	case TSFX_FARMER6:
		return "TownerSound Farmer6";
	case TSFX_FARMER7:
		return "TownerSound Farmer7";
	case TSFX_FARMER8:
		return "TownerSound Farmer8";
	case TSFX_FARMER9:
		return "TownerSound Farmer9";
	case TSFX_TEDDYBR1:
		return "TownerSound Teddybr1";
	case TSFX_TEDDYBR2:
		return "TownerSound Teddybr2";
	case TSFX_TEDDYBR3:
		return "TownerSound Teddybr3";
	case TSFX_TEDDYBR4:
		return "TownerSound Teddybr4";
	case USFX_DEFILER1:
		return "MonsterSound Defiler1";
	case USFX_DEFILER2:
		return "MonsterSound Defiler2";
	case USFX_DEFILER3:
		return "MonsterSound Defiler3";
	case USFX_DEFILER4:
		return "MonsterSound Defiler4";
	case USFX_DEFILER8:
		return "MonsterSound Defiler8";
	case USFX_DEFILER6:
		return "MonsterSound Defiler6";
	case USFX_DEFILER7:
		return "MonsterSound Defiler7";
	case USFX_NAKRUL1:
		return "MonsterSound Nakrul1";
	case USFX_NAKRUL2:
		return "MonsterSound Nakrul2";
	case USFX_NAKRUL3:
		return "MonsterSound Nakrul3";
	case USFX_NAKRUL4:
		return "MonsterSound Nakrul4";
	case USFX_NAKRUL5:
		return "MonsterSound Nakrul5";
	case USFX_NAKRUL6:
		return "MonsterSound Nakrul6";
	case PS_NARATR3:
		return "PlayerSound Naratr3";
	case TSFX_COWSUT1:
		return "TownerSound Cowsut1";
	case TSFX_COWSUT2:
		return "TownerSound Cowsut2";
	case TSFX_COWSUT3:
		return "TownerSound Cowsut3";
	case TSFX_COWSUT4:
		return "TownerSound Cowsut4";
	case TSFX_COWSUT4A:
		return "TownerSound Cowsut4a";
	case TSFX_COWSUT5:
		return "TownerSound Cowsut5";
	case TSFX_COWSUT6:
		return "TownerSound Cowsut6";
	case TSFX_COWSUT7:
		return "TownerSound Cowsut7";
	case TSFX_COWSUT8:
		return "TownerSound Cowsut8";
	case TSFX_COWSUT9:
		return "TownerSound Cowsut9";
	case TSFX_COWSUT10:
		return "TownerSound Cowsut10";
	case TSFX_COWSUT11:
		return "TownerSound Cowsut11";
	case TSFX_COWSUT12:
		return "TownerSound Cowsut12";
	case USFX_SKLJRN1:
		return "MonsterSound Skljrn1";
	case PS_NARATR6:
		return "PlayerSound Naratr6";
	case PS_NARATR7:
		return "PlayerSound Naratr7";
	case PS_NARATR8:
		return "PlayerSound Naratr8";
	case PS_NARATR5:
		return "PlayerSound Naratr5";
	case PS_NARATR9:
		return "PlayerSound Naratr9";
	case PS_NARATR4:
		return "PlayerSound Naratr4";
	case TSFX_TRADER1:
		return "TownerSound Trader1";
	case IS_CROPEN:
		return "ItemSound Cropen";
	case IS_CRCLOS:
		return "ItemSound Crclos";
	case SFX_NONE:
		return "None";
	}
}

enum sfx_flag : uint8_t {
	// clang-format off
	sfx_STREAM   = 1 << 0,
	sfx_MISC     = 1 << 1,
	sfx_UI       = 1 << 2,
	sfx_MONK     = 1 << 3,
	sfx_ROGUE    = 1 << 4,
	sfx_WARRIOR  = 1 << 5,
	sfx_SORCERER = 1 << 6,
	sfx_HELLFIRE = 1 << 7,
	// clang-format on
};

struct TSFX {
	Uint8 bFlags;
	const char *pszName;
	TSnd *pSnd;
};

extern int sfxdelay;
extern _sfx_id sfxdnum;

bool effect_is_playing(int nSFX);
void stream_stop();
void InitMonsterSND(int monst);
void FreeMonsterSnd();
bool calc_snd_position(int x, int y, int *plVolume, int *plPan);
void PlayEffect(int i, int mode);
void PlaySFX(_sfx_id psfx);
void PlaySfxLoc(_sfx_id psfx, int x, int y, bool randomizeByCategory = true);
void sound_stop();
void sound_update();
void effects_cleanup_sfx();
void sound_init();
void ui_sound_init();
void effects_play_sound(const char *snd_file);
int GetSFXLength(int nSFX);

} // namespace devilution
