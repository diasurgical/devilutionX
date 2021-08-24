/**
 * @file effects.cpp
 *
 * Implementation of functions for loading and playing sounds.
 */
#include "effects.h"

#include "engine/random.hpp"
#include "init.h"
#include "player.h"
#include "sound.h"
#include "utils/stdcompat/algorithm.hpp"

namespace devilution {

int sfxdelay;
_sfx_id sfxdnum = SFX_NONE;

namespace {

#ifndef DISABLE_STREAMING_SOUNDS
constexpr bool AllowStreaming = true;
#else
constexpr bool AllowStreaming = false;
#endif

/** Specifies the sound file and the playback state of the current sound effect. */
TSFX *sgpStreamSFX = nullptr;

/**
 * Monster sound type prefix
 * a: Attack
 * h: Hit
 * d: Death
 * s: Special
 */
const char MonstSndChar[] = { 'a', 'h', 'd', 's' };

/* data */
/** List of all sounds, except monsters and music */
TSFX sgSFX[] = {
	// clang-format off
	// bFlags,                   pszName,                       pSnd
	{ sfx_MISC,                  "Sfx\\Misc\\Walk1.wav",        nullptr },// PS_WALK1
	{ sfx_MISC,                  "Sfx\\Misc\\Walk2.wav",        nullptr },// PS_WALK2
	{ sfx_MISC,                  "Sfx\\Misc\\Walk3.wav",        nullptr },// PS_WALK3
	{ sfx_MISC,                  "Sfx\\Misc\\Walk4.wav",        nullptr },// PS_WALK4
	{ sfx_MISC,                  "Sfx\\Misc\\BFire.wav",        nullptr },// PS_BFIRE
	{ sfx_MISC,                  "Sfx\\Misc\\Fmag.wav",         nullptr },// PS_FMAG
	{ sfx_MISC,                  "Sfx\\Misc\\Tmag.wav",         nullptr },// PS_TMAG
	{ sfx_MISC,                  "Sfx\\Misc\\Lghit.wav",        nullptr },// PS_LGHIT
	{ sfx_MISC,                  "Sfx\\Misc\\Lghit1.wav",       nullptr },// PS_LGHIT1
	{ sfx_MISC,                  "Sfx\\Misc\\Swing.wav",        nullptr },// PS_SWING
	{ sfx_MISC,                  "Sfx\\Misc\\Swing2.wav",       nullptr },// PS_SWING2
	{ sfx_MISC,                  "Sfx\\Misc\\Dead.wav",         nullptr },// PS_DEAD
	{ sfx_MISC | sfx_HELLFIRE,   "Sfx\\Misc\\Sting1.wav",       nullptr },// IS_STING1
	{ sfx_MISC | sfx_HELLFIRE,   "Sfx\\Misc\\FBallBow.wav",     nullptr },// IS_FBALLBOW
	{ sfx_STREAM,                "Sfx\\Misc\\Questdon.wav",     nullptr },// IS_QUESTDN
	{ sfx_MISC,                  "Sfx\\Items\\Armrfkd.wav",     nullptr },// IS_ARMRFKD
	{ sfx_MISC,                  "Sfx\\Items\\Barlfire.wav",    nullptr },// IS_BARLFIRE
	{ sfx_MISC,                  "Sfx\\Items\\Barrel.wav",      nullptr },// IS_BARREL
	{ sfx_MISC | sfx_HELLFIRE,   "Sfx\\Items\\PodPop8.wav",     nullptr },// IS_POPPOP8
	{ sfx_MISC | sfx_HELLFIRE,   "Sfx\\Items\\PodPop5.wav",     nullptr },// IS_POPPOP5
	{ sfx_MISC | sfx_HELLFIRE,   "Sfx\\Items\\UrnPop3.wav",     nullptr },// IS_POPPOP3
	{ sfx_MISC | sfx_HELLFIRE,   "Sfx\\Items\\UrnPop2.wav",     nullptr },// IS_POPPOP2
	{ sfx_MISC,                  "Sfx\\Items\\Bhit.wav",        nullptr },// IS_BHIT
	{ sfx_MISC,                  "Sfx\\Items\\Bhit1.wav",       nullptr },// IS_BHIT1
	{ sfx_MISC,                  "Sfx\\Items\\Chest.wav",       nullptr },// IS_CHEST
	{ sfx_MISC,                  "Sfx\\Items\\Doorclos.wav",    nullptr },// IS_DOORCLOS
	{ sfx_MISC,                  "Sfx\\Items\\Dooropen.wav",    nullptr },// IS_DOOROPEN
	{ sfx_MISC,                  "Sfx\\Items\\Flipanvl.wav",    nullptr },// IS_FANVL
	{ sfx_MISC,                  "Sfx\\Items\\Flipaxe.wav",     nullptr },// IS_FAXE
	{ sfx_MISC,                  "Sfx\\Items\\Flipblst.wav",    nullptr },// IS_FBLST
	{ sfx_MISC,                  "Sfx\\Items\\Flipbody.wav",    nullptr },// IS_FBODY
	{ sfx_MISC,                  "Sfx\\Items\\Flipbook.wav",    nullptr },// IS_FBOOK
	{ sfx_MISC,                  "Sfx\\Items\\Flipbow.wav",     nullptr },// IS_FBOW
	{ sfx_MISC,                  "Sfx\\Items\\Flipcap.wav",     nullptr },// IS_FCAP
	{ sfx_MISC,                  "Sfx\\Items\\Flipharm.wav",    nullptr },// IS_FHARM
	{ sfx_MISC,                  "Sfx\\Items\\Fliplarm.wav",    nullptr },// IS_FLARM
	{ sfx_MISC,                  "Sfx\\Items\\Flipmag.wav",     nullptr },// IS_FMAG
	{ sfx_MISC,                  "Sfx\\Items\\Flipmag1.wav",    nullptr },// IS_FMAG1
	{ sfx_MISC,                  "Sfx\\Items\\Flipmush.wav",    nullptr },// IS_FMUSH
	{ sfx_MISC,                  "Sfx\\Items\\Flippot.wav",     nullptr },// IS_FPOT
	{ sfx_MISC,                  "Sfx\\Items\\Flipring.wav",    nullptr },// IS_FRING
	{ sfx_MISC,                  "Sfx\\Items\\Fliprock.wav",    nullptr },// IS_FROCK
	{ sfx_MISC,                  "Sfx\\Items\\Flipscrl.wav",    nullptr },// IS_FSCRL
	{ sfx_MISC,                  "Sfx\\Items\\Flipshld.wav",    nullptr },// IS_FSHLD
	{ sfx_MISC,                  "Sfx\\Items\\Flipsign.wav",    nullptr },// IS_FSIGN
	{ sfx_MISC,                  "Sfx\\Items\\Flipstaf.wav",    nullptr },// IS_FSTAF
	{ sfx_MISC,                  "Sfx\\Items\\Flipswor.wav",    nullptr },// IS_FSWOR
	{ sfx_MISC,                  "Sfx\\Items\\Gold.wav",        nullptr },// IS_GOLD
	{ sfx_MISC,                  "Sfx\\Items\\Hlmtfkd.wav",     nullptr },// IS_HLMTFKD
	{ sfx_MISC,                  "Sfx\\Items\\Invanvl.wav",     nullptr },// IS_IANVL
	{ sfx_MISC,                  "Sfx\\Items\\Invaxe.wav",      nullptr },// IS_IAXE
	{ sfx_MISC,                  "Sfx\\Items\\Invblst.wav",     nullptr },// IS_IBLST
	{ sfx_MISC,                  "Sfx\\Items\\Invbody.wav",     nullptr },// IS_IBODY
	{ sfx_MISC,                  "Sfx\\Items\\Invbook.wav",     nullptr },// IS_IBOOK
	{ sfx_MISC,                  "Sfx\\Items\\Invbow.wav",      nullptr },// IS_IBOW
	{ sfx_MISC,                  "Sfx\\Items\\Invcap.wav",      nullptr },// IS_ICAP
	{ sfx_MISC,                  "Sfx\\Items\\Invgrab.wav",     nullptr },// IS_IGRAB
	{ sfx_MISC,                  "Sfx\\Items\\Invharm.wav",     nullptr },// IS_IHARM
	{ sfx_MISC,                  "Sfx\\Items\\Invlarm.wav",     nullptr },// IS_ILARM
	{ sfx_MISC,                  "Sfx\\Items\\Invmush.wav",     nullptr },// IS_IMUSH
	{ sfx_MISC,                  "Sfx\\Items\\Invpot.wav",      nullptr },// IS_IPOT
	{ sfx_MISC,                  "Sfx\\Items\\Invring.wav",     nullptr },// IS_IRING
	{ sfx_MISC,                  "Sfx\\Items\\Invrock.wav",     nullptr },// IS_IROCK
	{ sfx_MISC,                  "Sfx\\Items\\Invscrol.wav",    nullptr },// IS_ISCROL
	{ sfx_MISC,                  "Sfx\\Items\\Invshiel.wav",    nullptr },// IS_ISHIEL
	{ sfx_MISC,                  "Sfx\\Items\\Invsign.wav",     nullptr },// IS_ISIGN
	{ sfx_MISC,                  "Sfx\\Items\\Invstaf.wav",     nullptr },// IS_ISTAF
	{ sfx_MISC,                  "Sfx\\Items\\Invsword.wav",    nullptr },// IS_ISWORD
	{ sfx_MISC,                  "Sfx\\Items\\Lever.wav",       nullptr },// IS_LEVER
	{ sfx_MISC,                  "Sfx\\Items\\Magic.wav",       nullptr },// IS_MAGIC
	{ sfx_MISC,                  "Sfx\\Items\\Magic1.wav",      nullptr },// IS_MAGIC1
	{ sfx_MISC,                  "Sfx\\Items\\Readbook.wav",    nullptr },// IS_RBOOK
	{ sfx_MISC,                  "Sfx\\Items\\Sarc.wav",        nullptr },// IS_SARC
	{ sfx_MISC,                  "Sfx\\Items\\Shielfkd.wav",    nullptr },// IS_SHLDFKD
	{ sfx_MISC,                  "Sfx\\Items\\Swrdfkd.wav",     nullptr },// IS_SWRDFKD
	{ sfx_UI,                    "Sfx\\Items\\Titlemov.wav",    nullptr },// IS_TITLEMOV
	{ sfx_UI,                    "Sfx\\Items\\Titlslct.wav",    nullptr },// IS_TITLSLCT
	{ sfx_UI,                    "Sfx\\Misc\\blank.wav",        nullptr },// SFX_SILENCE
	{ sfx_MISC,                  "Sfx\\Items\\Trap.wav",        nullptr },// IS_TRAP
	{ sfx_MISC,                  "Sfx\\Misc\\Cast1.wav",        nullptr },// IS_CAST1
	{ sfx_MISC,                  "Sfx\\Misc\\Cast10.wav",       nullptr },// IS_CAST10
	{ sfx_MISC,                  "Sfx\\Misc\\Cast12.wav",       nullptr },// IS_CAST12
	{ sfx_MISC,                  "Sfx\\Misc\\Cast2.wav",        nullptr },// IS_CAST2
	{ sfx_MISC,                  "Sfx\\Misc\\Cast3.wav",        nullptr },// IS_CAST3
	{ sfx_MISC,                  "Sfx\\Misc\\Cast4.wav",        nullptr },// IS_CAST4
	{ sfx_MISC,                  "Sfx\\Misc\\Cast5.wav",        nullptr },// IS_CAST5
	{ sfx_MISC,                  "Sfx\\Misc\\Cast6.wav",        nullptr },// IS_CAST6
	{ sfx_MISC,                  "Sfx\\Misc\\Cast7.wav",        nullptr },// IS_CAST7
	{ sfx_MISC,                  "Sfx\\Misc\\Cast8.wav",        nullptr },// IS_CAST8
	{ sfx_MISC,                  "Sfx\\Misc\\Cast9.wav",        nullptr },// IS_CAST9
	{ sfx_MISC,                  "Sfx\\Misc\\Healing.wav",      nullptr },// LS_HEALING
	{ sfx_MISC,                  "Sfx\\Misc\\Repair.wav",       nullptr },// IS_REPAIR
	{ sfx_MISC,                  "Sfx\\Misc\\Acids1.wav",       nullptr },// LS_ACID
	{ sfx_MISC,                  "Sfx\\Misc\\Acids2.wav",       nullptr },// LS_ACIDS
	{ sfx_MISC,                  "Sfx\\Misc\\Apoc.wav",         nullptr },// LS_APOC
	{ sfx_MISC,                  "Sfx\\Misc\\Arrowall.wav",     nullptr },// LS_ARROWALL
	{ sfx_MISC,                  "Sfx\\Misc\\Bldboil.wav",      nullptr },// LS_BLODBOIL
	{ sfx_MISC,                  "Sfx\\Misc\\Blodstar.wav",     nullptr },// LS_BLODSTAR
	{ sfx_MISC,                  "Sfx\\Misc\\Blsimpt.wav",      nullptr },// LS_BLSIMPT
	{ sfx_MISC,                  "Sfx\\Misc\\Bonesp.wav",       nullptr },// LS_BONESP
	{ sfx_MISC,                  "Sfx\\Misc\\Bsimpct.wav",      nullptr },// LS_BSIMPCT
	{ sfx_MISC,                  "Sfx\\Misc\\Caldron.wav",      nullptr },// LS_CALDRON
	{ sfx_MISC,                  "Sfx\\Misc\\Cbolt.wav",        nullptr },// LS_CBOLT
	{ sfx_MISC,                  "Sfx\\Misc\\Chltning.wav",     nullptr },// LS_CHLTNING
	{ sfx_MISC,                  "Sfx\\Misc\\DSerp.wav",        nullptr },// LS_DSERP
	{ sfx_MISC,                  "Sfx\\Misc\\Elecimp1.wav",     nullptr },// LS_ELECIMP1
	{ sfx_MISC,                  "Sfx\\Misc\\Elementl.wav",     nullptr },// LS_ELEMENTL
	{ sfx_MISC,                  "Sfx\\Misc\\Ethereal.wav",     nullptr },// LS_ETHEREAL
	{ sfx_MISC,                  "Sfx\\Misc\\Fball.wav",        nullptr },// LS_FBALL
	{ sfx_MISC,                  "Sfx\\Misc\\Fbolt1.wav",       nullptr },// LS_FBOLT1
	{ sfx_MISC,                  "Sfx\\Misc\\Fbolt2.wav",       nullptr },// LS_FBOLT2
	{ sfx_MISC,                  "Sfx\\Misc\\Firimp1.wav",      nullptr },// LS_FIRIMP1
	{ sfx_MISC,                  "Sfx\\Misc\\Firimp2.wav",      nullptr },// LS_FIRIMP2
	{ sfx_MISC,                  "Sfx\\Misc\\Flamwave.wav",     nullptr },// LS_FLAMWAVE
	{ sfx_MISC,                  "Sfx\\Misc\\Flash.wav",        nullptr },// LS_FLASH
	{ sfx_MISC,                  "Sfx\\Misc\\Fountain.wav",     nullptr },// LS_FOUNTAIN
	{ sfx_MISC,                  "Sfx\\Misc\\Golum.wav",        nullptr },// LS_GOLUM
	{ sfx_MISC,                  "Sfx\\Misc\\Golumded.wav",     nullptr },// LS_GOLUMDED
	{ sfx_MISC,                  "Sfx\\Misc\\Gshrine.wav",      nullptr },// LS_GSHRINE
	{ sfx_MISC,                  "Sfx\\Misc\\Guard.wav",        nullptr },// LS_GUARD
	{ sfx_MISC,                  "Sfx\\Misc\\Grdlanch.wav",     nullptr },// LS_GUARDLAN
	{ sfx_MISC,                  "Sfx\\Misc\\Holybolt.wav",     nullptr },// LS_HOLYBOLT
	{ sfx_MISC,                  "Sfx\\Misc\\Hyper.wav",        nullptr },// LS_HYPER
	{ sfx_MISC,                  "Sfx\\Misc\\Infravis.wav",     nullptr },// LS_INFRAVIS
	{ sfx_MISC,                  "Sfx\\Misc\\Invisibl.wav",     nullptr },// LS_INVISIBL
	{ sfx_MISC,                  "Sfx\\Misc\\Invpot.wav",       nullptr },// LS_INVPOT
	{ sfx_MISC,                  "Sfx\\Misc\\Lning1.wav",       nullptr },// LS_LNING1
	{ sfx_MISC,                  "Sfx\\Misc\\Ltning.wav",       nullptr },// LS_LTNING
	{ sfx_MISC,                  "Sfx\\Misc\\Mshield.wav",      nullptr },// LS_MSHIELD
	{ sfx_MISC | sfx_HELLFIRE,   "Sfx\\Misc\\NestXpld.wav",     nullptr },// LS_NESTXPLD
	{ sfx_MISC,                  "Sfx\\Misc\\Nova.wav",         nullptr },// LS_NOVA
	{ sfx_MISC,                  "Sfx\\Misc\\Portal.wav",       nullptr },// LS_PORTAL
	{ sfx_MISC,                  "Sfx\\Misc\\Puddle.wav",       nullptr },// LS_PUDDLE
	{ sfx_MISC,                  "Sfx\\Misc\\Resur.wav",        nullptr },// LS_RESUR
	{ sfx_MISC,                  "Sfx\\Misc\\Scurse.wav",       nullptr },// LS_SCURSE
	{ sfx_MISC,                  "Sfx\\Misc\\Scurimp.wav",      nullptr },// LS_SCURIMP
	{ sfx_MISC,                  "Sfx\\Misc\\Sentinel.wav",     nullptr },// LS_SENTINEL
	{ sfx_MISC,                  "Sfx\\Misc\\Shatter.wav",      nullptr },// LS_SHATTER
	{ sfx_MISC,                  "Sfx\\Misc\\Soulfire.wav",     nullptr },// LS_SOULFIRE
	{ sfx_MISC,                  "Sfx\\Misc\\Spoutlop.wav",     nullptr },// LS_SPOUTLOP
	{ sfx_MISC,                  "Sfx\\Misc\\Spoutstr.wav",     nullptr },// LS_SPOUTSTR
	{ sfx_MISC,                  "Sfx\\Misc\\Storm.wav",        nullptr },// LS_STORM
	{ sfx_MISC,                  "Sfx\\Misc\\Trapdis.wav",      nullptr },// LS_TRAPDIS
	{ sfx_MISC,                  "Sfx\\Misc\\Teleport.wav",     nullptr },// LS_TELEPORT
	{ sfx_MISC,                  "Sfx\\Misc\\Vtheft.wav",       nullptr },// LS_VTHEFT
	{ sfx_MISC,                  "Sfx\\Misc\\Wallloop.wav",     nullptr },// LS_WALLLOOP
	{ sfx_MISC,                  "Sfx\\Misc\\Wallstrt.wav",     nullptr },// LS_WALLSTRT
	{ sfx_MISC | sfx_HELLFIRE,   "Sfx\\Misc\\LMag.wav",         nullptr },// LS_LMAG
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid01.wav",   nullptr },// TSFX_BMAID1
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid02.wav",   nullptr },// TSFX_BMAID2
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid03.wav",   nullptr },// TSFX_BMAID3
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid04.wav",   nullptr },// TSFX_BMAID4
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid05.wav",   nullptr },// TSFX_BMAID5
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid06.wav",   nullptr },// TSFX_BMAID6
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid07.wav",   nullptr },// TSFX_BMAID7
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid08.wav",   nullptr },// TSFX_BMAID8
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid09.wav",   nullptr },// TSFX_BMAID9
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid10.wav",   nullptr },// TSFX_BMAID10
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid11.wav",   nullptr },// TSFX_BMAID11
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid12.wav",   nullptr },// TSFX_BMAID12
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid13.wav",   nullptr },// TSFX_BMAID13
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid14.wav",   nullptr },// TSFX_BMAID14
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid15.wav",   nullptr },// TSFX_BMAID15
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid16.wav",   nullptr },// TSFX_BMAID16
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid17.wav",   nullptr },// TSFX_BMAID17
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid18.wav",   nullptr },// TSFX_BMAID18
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid19.wav",   nullptr },// TSFX_BMAID19
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid20.wav",   nullptr },// TSFX_BMAID20
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid21.wav",   nullptr },// TSFX_BMAID21
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid22.wav",   nullptr },// TSFX_BMAID22
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid23.wav",   nullptr },// TSFX_BMAID23
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid24.wav",   nullptr },// TSFX_BMAID24
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid25.wav",   nullptr },// TSFX_BMAID25
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid26.wav",   nullptr },// TSFX_BMAID26
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid27.wav",   nullptr },// TSFX_BMAID27
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid28.wav",   nullptr },// TSFX_BMAID28
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid29.wav",   nullptr },// TSFX_BMAID29
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid30.wav",   nullptr },// TSFX_BMAID30
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid31.wav",   nullptr },// TSFX_BMAID31
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid32.wav",   nullptr },// TSFX_BMAID32
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid33.wav",   nullptr },// TSFX_BMAID33
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid34.wav",   nullptr },// TSFX_BMAID34
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid35.wav",   nullptr },// TSFX_BMAID35
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid36.wav",   nullptr },// TSFX_BMAID36
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid37.wav",   nullptr },// TSFX_BMAID37
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid38.wav",   nullptr },// TSFX_BMAID38
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid39.wav",   nullptr },// TSFX_BMAID39
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid40.wav",   nullptr },// TSFX_BMAID40
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith01.wav",  nullptr },// TSFX_SMITH1
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith02.wav",  nullptr },// TSFX_SMITH2
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith03.wav",  nullptr },// TSFX_SMITH3
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith04.wav",  nullptr },// TSFX_SMITH4
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith05.wav",  nullptr },// TSFX_SMITH5
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith06.wav",  nullptr },// TSFX_SMITH6
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith07.wav",  nullptr },// TSFX_SMITH7
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith08.wav",  nullptr },// TSFX_SMITH8
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith09.wav",  nullptr },// TSFX_SMITH9
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith10.wav",  nullptr },// TSFX_SMITH10
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith11.wav",  nullptr },// TSFX_SMITH11
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith12.wav",  nullptr },// TSFX_SMITH12
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith13.wav",  nullptr },// TSFX_SMITH13
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith14.wav",  nullptr },// TSFX_SMITH14
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith15.wav",  nullptr },// TSFX_SMITH15
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith16.wav",  nullptr },// TSFX_SMITH16
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith17.wav",  nullptr },// TSFX_SMITH17
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith18.wav",  nullptr },// TSFX_SMITH18
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith19.wav",  nullptr },// TSFX_SMITH19
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith20.wav",  nullptr },// TSFX_SMITH20
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith21.wav",  nullptr },// TSFX_SMITH21
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith22.wav",  nullptr },// TSFX_SMITH22
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith23.wav",  nullptr },// TSFX_SMITH23
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith24.wav",  nullptr },// TSFX_SMITH24
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith25.wav",  nullptr },// TSFX_SMITH25
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith26.wav",  nullptr },// TSFX_SMITH26
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith27.wav",  nullptr },// TSFX_SMITH27
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith28.wav",  nullptr },// TSFX_SMITH28
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith29.wav",  nullptr },// TSFX_SMITH29
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith30.wav",  nullptr },// TSFX_SMITH30
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith31.wav",  nullptr },// TSFX_SMITH31
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith32.wav",  nullptr },// TSFX_SMITH32
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith33.wav",  nullptr },// TSFX_SMITH33
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith34.wav",  nullptr },// TSFX_SMITH34
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith35.wav",  nullptr },// TSFX_SMITH35
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith36.wav",  nullptr },// TSFX_SMITH36
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith37.wav",  nullptr },// TSFX_SMITH37
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith38.wav",  nullptr },// TSFX_SMITH38
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith39.wav",  nullptr },// TSFX_SMITH39
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith40.wav",  nullptr },// TSFX_SMITH40
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith41.wav",  nullptr },// TSFX_SMITH41
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith42.wav",  nullptr },// TSFX_SMITH42
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith43.wav",  nullptr },// TSFX_SMITH43
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith44.wav",  nullptr },// TSFX_SMITH44
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith45.wav",  nullptr },// TSFX_SMITH45
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith46.wav",  nullptr },// TSFX_SMITH46
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith47.wav",  nullptr },// TSFX_SMITH47
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith48.wav",  nullptr },// TSFX_SMITH48
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith49.wav",  nullptr },// TSFX_SMITH49
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith50.wav",  nullptr },// TSFX_SMITH50
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith51.wav",  nullptr },// TSFX_SMITH51
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith52.wav",  nullptr },// TSFX_SMITH52
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith53.wav",  nullptr },// TSFX_SMITH53
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith54.wav",  nullptr },// TSFX_SMITH54
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith55.wav",  nullptr },// TSFX_SMITH55
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith56.wav",  nullptr },// TSFX_SMITH56
	{ sfx_MISC,                  "Sfx\\Towners\\Cow1.wav",      nullptr },// TSFX_COW1
	{ sfx_MISC,                  "Sfx\\Towners\\Cow2.wav",      nullptr },// TSFX_COW2
/*
	{ sfx_MISC,                  "Sfx\\Towners\\Cow3.wav",      nullptr },// TSFX_COW3
	{ sfx_MISC,                  "Sfx\\Towners\\Cow4.wav",      nullptr },// TSFX_COW4
	{ sfx_MISC,                  "Sfx\\Towners\\Cow5.wav",      nullptr },// TSFX_COW5
	{ sfx_MISC,                  "Sfx\\Towners\\Cow6.wav",      nullptr },// TSFX_COW6
*/
	{ sfx_MISC | sfx_HELLFIRE,   "Sfx\\Towners\\Cow7.wav",      nullptr },// TSFX_COW7
	{ sfx_MISC | sfx_HELLFIRE,   "Sfx\\Towners\\Cow8.wav",      nullptr },// TSFX_COW8
	{ sfx_STREAM,                "Sfx\\Towners\\Deadguy2.wav",  nullptr },// TSFX_DEADGUY
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk01.wav",   nullptr },// TSFX_DRUNK1
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk02.wav",   nullptr },// TSFX_DRUNK2
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk03.wav",   nullptr },// TSFX_DRUNK3
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk04.wav",   nullptr },// TSFX_DRUNK4
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk05.wav",   nullptr },// TSFX_DRUNK5
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk06.wav",   nullptr },// TSFX_DRUNK6
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk07.wav",   nullptr },// TSFX_DRUNK7
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk08.wav",   nullptr },// TSFX_DRUNK8
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk09.wav",   nullptr },// TSFX_DRUNK9
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk10.wav",   nullptr },// TSFX_DRUNK10
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk11.wav",   nullptr },// TSFX_DRUNK11
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk12.wav",   nullptr },// TSFX_DRUNK12
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk13.wav",   nullptr },// TSFX_DRUNK13
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk14.wav",   nullptr },// TSFX_DRUNK14
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk15.wav",   nullptr },// TSFX_DRUNK15
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk16.wav",   nullptr },// TSFX_DRUNK16
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk17.wav",   nullptr },// TSFX_DRUNK17
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk18.wav",   nullptr },// TSFX_DRUNK18
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk19.wav",   nullptr },// TSFX_DRUNK19
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk20.wav",   nullptr },// TSFX_DRUNK20
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk21.wav",   nullptr },// TSFX_DRUNK21
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk22.wav",   nullptr },// TSFX_DRUNK22
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk23.wav",   nullptr },// TSFX_DRUNK23
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk24.wav",   nullptr },// TSFX_DRUNK24
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk25.wav",   nullptr },// TSFX_DRUNK25
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk26.wav",   nullptr },// TSFX_DRUNK26
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk27.wav",   nullptr },// TSFX_DRUNK27
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk28.wav",   nullptr },// TSFX_DRUNK28
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk29.wav",   nullptr },// TSFX_DRUNK29
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk30.wav",   nullptr },// TSFX_DRUNK30
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk31.wav",   nullptr },// TSFX_DRUNK31
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk32.wav",   nullptr },// TSFX_DRUNK32
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk33.wav",   nullptr },// TSFX_DRUNK33
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk34.wav",   nullptr },// TSFX_DRUNK34
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk35.wav",   nullptr },// TSFX_DRUNK35
	{ sfx_STREAM,                "Sfx\\Towners\\Healer01.wav",  nullptr },// TSFX_HEALER1
	{ sfx_STREAM,                "Sfx\\Towners\\Healer02.wav",  nullptr },// TSFX_HEALER2
	{ sfx_STREAM,                "Sfx\\Towners\\Healer03.wav",  nullptr },// TSFX_HEALER3
	{ sfx_STREAM,                "Sfx\\Towners\\Healer04.wav",  nullptr },// TSFX_HEALER4
	{ sfx_STREAM,                "Sfx\\Towners\\Healer05.wav",  nullptr },// TSFX_HEALER5
	{ sfx_STREAM,                "Sfx\\Towners\\Healer06.wav",  nullptr },// TSFX_HEALER6
	{ sfx_STREAM,                "Sfx\\Towners\\Healer07.wav",  nullptr },// TSFX_HEALER7
	{ sfx_STREAM,                "Sfx\\Towners\\Healer08.wav",  nullptr },// TSFX_HEALER8
	{ sfx_STREAM,                "Sfx\\Towners\\Healer09.wav",  nullptr },// TSFX_HEALER9
	{ sfx_STREAM,                "Sfx\\Towners\\Healer10.wav",  nullptr },// TSFX_HEALER10
	{ sfx_STREAM,                "Sfx\\Towners\\Healer11.wav",  nullptr },// TSFX_HEALER11
	{ sfx_STREAM,                "Sfx\\Towners\\Healer12.wav",  nullptr },// TSFX_HEALER12
	{ sfx_STREAM,                "Sfx\\Towners\\Healer13.wav",  nullptr },// TSFX_HEALER13
	{ sfx_STREAM,                "Sfx\\Towners\\Healer14.wav",  nullptr },// TSFX_HEALER14
	{ sfx_STREAM,                "Sfx\\Towners\\Healer15.wav",  nullptr },// TSFX_HEALER15
	{ sfx_STREAM,                "Sfx\\Towners\\Healer16.wav",  nullptr },// TSFX_HEALER16
	{ sfx_STREAM,                "Sfx\\Towners\\Healer17.wav",  nullptr },// TSFX_HEALER17
	{ sfx_STREAM,                "Sfx\\Towners\\Healer18.wav",  nullptr },// TSFX_HEALER18
	{ sfx_STREAM,                "Sfx\\Towners\\Healer19.wav",  nullptr },// TSFX_HEALER19
	{ sfx_STREAM,                "Sfx\\Towners\\Healer20.wav",  nullptr },// TSFX_HEALER20
	{ sfx_STREAM,                "Sfx\\Towners\\Healer21.wav",  nullptr },// TSFX_HEALER21
	{ sfx_STREAM,                "Sfx\\Towners\\Healer22.wav",  nullptr },// TSFX_HEALER22
	{ sfx_STREAM,                "Sfx\\Towners\\Healer23.wav",  nullptr },// TSFX_HEALER23
	{ sfx_STREAM,                "Sfx\\Towners\\Healer24.wav",  nullptr },// TSFX_HEALER24
	{ sfx_STREAM,                "Sfx\\Towners\\Healer25.wav",  nullptr },// TSFX_HEALER25
	{ sfx_STREAM,                "Sfx\\Towners\\Healer26.wav",  nullptr },// TSFX_HEALER26
	{ sfx_STREAM,                "Sfx\\Towners\\Healer27.wav",  nullptr },// TSFX_HEALER27
	{ sfx_STREAM,                "Sfx\\Towners\\Healer28.wav",  nullptr },// TSFX_HEALER28
	{ sfx_STREAM,                "Sfx\\Towners\\Healer29.wav",  nullptr },// TSFX_HEALER29
	{ sfx_STREAM,                "Sfx\\Towners\\Healer30.wav",  nullptr },// TSFX_HEALER30
	{ sfx_STREAM,                "Sfx\\Towners\\Healer31.wav",  nullptr },// TSFX_HEALER31
	{ sfx_STREAM,                "Sfx\\Towners\\Healer32.wav",  nullptr },// TSFX_HEALER32
	{ sfx_STREAM,                "Sfx\\Towners\\Healer33.wav",  nullptr },// TSFX_HEALER33
	{ sfx_STREAM,                "Sfx\\Towners\\Healer34.wav",  nullptr },// TSFX_HEALER34
	{ sfx_STREAM,                "Sfx\\Towners\\Healer35.wav",  nullptr },// TSFX_HEALER35
	{ sfx_STREAM,                "Sfx\\Towners\\Healer36.wav",  nullptr },// TSFX_HEALER36
	{ sfx_STREAM,                "Sfx\\Towners\\Healer37.wav",  nullptr },// TSFX_HEALER37
	{ sfx_STREAM,                "Sfx\\Towners\\Healer38.wav",  nullptr },// TSFX_HEALER38
	{ sfx_STREAM,                "Sfx\\Towners\\Healer39.wav",  nullptr },// TSFX_HEALER39
	{ sfx_STREAM,                "Sfx\\Towners\\Healer40.wav",  nullptr },// TSFX_HEALER40
	{ sfx_STREAM,                "Sfx\\Towners\\Healer41.wav",  nullptr },// TSFX_HEALER41
	{ sfx_STREAM,                "Sfx\\Towners\\Healer42.wav",  nullptr },// TSFX_HEALER42
	{ sfx_STREAM,                "Sfx\\Towners\\Healer43.wav",  nullptr },// TSFX_HEALER43
	{ sfx_STREAM,                "Sfx\\Towners\\Healer44.wav",  nullptr },// TSFX_HEALER44
	{ sfx_STREAM,                "Sfx\\Towners\\Healer45.wav",  nullptr },// TSFX_HEALER45
	{ sfx_STREAM,                "Sfx\\Towners\\Healer46.wav",  nullptr },// TSFX_HEALER46
	{ sfx_STREAM,                "Sfx\\Towners\\Healer47.wav",  nullptr },// TSFX_HEALER47
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy01.wav",  nullptr },// TSFX_PEGBOY1
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy02.wav",  nullptr },// TSFX_PEGBOY2
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy03.wav",  nullptr },// TSFX_PEGBOY3
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy04.wav",  nullptr },// TSFX_PEGBOY4
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy05.wav",  nullptr },// TSFX_PEGBOY5
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy06.wav",  nullptr },// TSFX_PEGBOY6
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy07.wav",  nullptr },// TSFX_PEGBOY7
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy08.wav",  nullptr },// TSFX_PEGBOY8
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy09.wav",  nullptr },// TSFX_PEGBOY9
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy10.wav",  nullptr },// TSFX_PEGBOY10
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy11.wav",  nullptr },// TSFX_PEGBOY11
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy12.wav",  nullptr },// TSFX_PEGBOY12
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy13.wav",  nullptr },// TSFX_PEGBOY13
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy14.wav",  nullptr },// TSFX_PEGBOY14
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy15.wav",  nullptr },// TSFX_PEGBOY15
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy16.wav",  nullptr },// TSFX_PEGBOY16
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy17.wav",  nullptr },// TSFX_PEGBOY17
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy18.wav",  nullptr },// TSFX_PEGBOY18
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy19.wav",  nullptr },// TSFX_PEGBOY19
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy20.wav",  nullptr },// TSFX_PEGBOY20
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy21.wav",  nullptr },// TSFX_PEGBOY21
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy22.wav",  nullptr },// TSFX_PEGBOY22
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy23.wav",  nullptr },// TSFX_PEGBOY23
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy24.wav",  nullptr },// TSFX_PEGBOY24
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy25.wav",  nullptr },// TSFX_PEGBOY25
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy26.wav",  nullptr },// TSFX_PEGBOY26
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy27.wav",  nullptr },// TSFX_PEGBOY27
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy28.wav",  nullptr },// TSFX_PEGBOY28
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy29.wav",  nullptr },// TSFX_PEGBOY29
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy30.wav",  nullptr },// TSFX_PEGBOY30
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy31.wav",  nullptr },// TSFX_PEGBOY31
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy32.wav",  nullptr },// TSFX_PEGBOY32
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy33.wav",  nullptr },// TSFX_PEGBOY33
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy34.wav",  nullptr },// TSFX_PEGBOY34
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy35.wav",  nullptr },// TSFX_PEGBOY35
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy36.wav",  nullptr },// TSFX_PEGBOY36
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy37.wav",  nullptr },// TSFX_PEGBOY37
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy38.wav",  nullptr },// TSFX_PEGBOY38
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy39.wav",  nullptr },// TSFX_PEGBOY39
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy40.wav",  nullptr },// TSFX_PEGBOY40
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy41.wav",  nullptr },// TSFX_PEGBOY41
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy42.wav",  nullptr },// TSFX_PEGBOY42
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy43.wav",  nullptr },// TSFX_PEGBOY43
	{ sfx_STREAM,                "Sfx\\Towners\\Priest00.wav",  nullptr },// TSFX_PRIEST0
	{ sfx_STREAM,                "Sfx\\Towners\\Priest01.wav",  nullptr },// TSFX_PRIEST1
	{ sfx_STREAM,                "Sfx\\Towners\\Priest02.wav",  nullptr },// TSFX_PRIEST2
	{ sfx_STREAM,                "Sfx\\Towners\\Priest03.wav",  nullptr },// TSFX_PRIEST3
	{ sfx_STREAM,                "Sfx\\Towners\\Priest04.wav",  nullptr },// TSFX_PRIEST4
	{ sfx_STREAM,                "Sfx\\Towners\\Priest05.wav",  nullptr },// TSFX_PRIEST5
	{ sfx_STREAM,                "Sfx\\Towners\\Priest06.wav",  nullptr },// TSFX_PRIEST6
	{ sfx_STREAM,                "Sfx\\Towners\\Priest07.wav",  nullptr },// TSFX_PRIEST7
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt00.wav",  nullptr },// TSFX_STORY0
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt01.wav",  nullptr },// TSFX_STORY1
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt02.wav",  nullptr },// TSFX_STORY2
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt03.wav",  nullptr },// TSFX_STORY3
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt04.wav",  nullptr },// TSFX_STORY4
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt05.wav",  nullptr },// TSFX_STORY5
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt06.wav",  nullptr },// TSFX_STORY6
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt07.wav",  nullptr },// TSFX_STORY7
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt08.wav",  nullptr },// TSFX_STORY8
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt09.wav",  nullptr },// TSFX_STORY9
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt10.wav",  nullptr },// TSFX_STORY10
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt11.wav",  nullptr },// TSFX_STORY11
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt12.wav",  nullptr },// TSFX_STORY12
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt13.wav",  nullptr },// TSFX_STORY13
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt14.wav",  nullptr },// TSFX_STORY14
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt15.wav",  nullptr },// TSFX_STORY15
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt16.wav",  nullptr },// TSFX_STORY16
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt17.wav",  nullptr },// TSFX_STORY17
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt18.wav",  nullptr },// TSFX_STORY18
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt19.wav",  nullptr },// TSFX_STORY19
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt20.wav",  nullptr },// TSFX_STORY20
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt21.wav",  nullptr },// TSFX_STORY21
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt22.wav",  nullptr },// TSFX_STORY22
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt23.wav",  nullptr },// TSFX_STORY23
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt24.wav",  nullptr },// TSFX_STORY24
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt25.wav",  nullptr },// TSFX_STORY25
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt26.wav",  nullptr },// TSFX_STORY26
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt27.wav",  nullptr },// TSFX_STORY27
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt28.wav",  nullptr },// TSFX_STORY28
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt29.wav",  nullptr },// TSFX_STORY29
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt30.wav",  nullptr },// TSFX_STORY30
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt31.wav",  nullptr },// TSFX_STORY31
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt32.wav",  nullptr },// TSFX_STORY32
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt33.wav",  nullptr },// TSFX_STORY33
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt34.wav",  nullptr },// TSFX_STORY34
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt35.wav",  nullptr },// TSFX_STORY35
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt36.wav",  nullptr },// TSFX_STORY36
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt37.wav",  nullptr },// TSFX_STORY37
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt38.wav",  nullptr },// TSFX_STORY38
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown00.wav",  nullptr },// TSFX_TAVERN0
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown01.wav",  nullptr },// TSFX_TAVERN1
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown02.wav",  nullptr },// TSFX_TAVERN2
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown03.wav",  nullptr },// TSFX_TAVERN3
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown04.wav",  nullptr },// TSFX_TAVERN4
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown05.wav",  nullptr },// TSFX_TAVERN5
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown06.wav",  nullptr },// TSFX_TAVERN6
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown07.wav",  nullptr },// TSFX_TAVERN7
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown08.wav",  nullptr },// TSFX_TAVERN8
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown09.wav",  nullptr },// TSFX_TAVERN9
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown10.wav",  nullptr },// TSFX_TAVERN10
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown11.wav",  nullptr },// TSFX_TAVERN11
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown12.wav",  nullptr },// TSFX_TAVERN12
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown13.wav",  nullptr },// TSFX_TAVERN13
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown14.wav",  nullptr },// TSFX_TAVERN14
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown15.wav",  nullptr },// TSFX_TAVERN15
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown16.wav",  nullptr },// TSFX_TAVERN16
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown17.wav",  nullptr },// TSFX_TAVERN17
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown18.wav",  nullptr },// TSFX_TAVERN18
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown19.wav",  nullptr },// TSFX_TAVERN19
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown20.wav",  nullptr },// TSFX_TAVERN20
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown21.wav",  nullptr },// TSFX_TAVERN21
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown22.wav",  nullptr },// TSFX_TAVERN22
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown23.wav",  nullptr },// TSFX_TAVERN23
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown24.wav",  nullptr },// TSFX_TAVERN24
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown25.wav",  nullptr },// TSFX_TAVERN25
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown26.wav",  nullptr },// TSFX_TAVERN26
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown27.wav",  nullptr },// TSFX_TAVERN27
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown28.wav",  nullptr },// TSFX_TAVERN28
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown29.wav",  nullptr },// TSFX_TAVERN29
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown30.wav",  nullptr },// TSFX_TAVERN30
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown31.wav",  nullptr },// TSFX_TAVERN31
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown32.wav",  nullptr },// TSFX_TAVERN32
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown33.wav",  nullptr },// TSFX_TAVERN33
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown34.wav",  nullptr },// TSFX_TAVERN34
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown35.wav",  nullptr },// TSFX_TAVERN35
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown36.wav",  nullptr },// TSFX_TAVERN36
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown37.wav",  nullptr },// TSFX_TAVERN37
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown38.wav",  nullptr },// TSFX_TAVERN38
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown39.wav",  nullptr },// TSFX_TAVERN39
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown40.wav",  nullptr },// TSFX_TAVERN40
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown41.wav",  nullptr },// TSFX_TAVERN41
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown42.wav",  nullptr },// TSFX_TAVERN42
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown43.wav",  nullptr },// TSFX_TAVERN43
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown44.wav",  nullptr },// TSFX_TAVERN44
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown45.wav",  nullptr },// TSFX_TAVERN45
	{ sfx_STREAM,                "Sfx\\Towners\\Witch01.wav",   nullptr },// TSFX_WITCH1
	{ sfx_STREAM,                "Sfx\\Towners\\Witch02.wav",   nullptr },// TSFX_WITCH2
	{ sfx_STREAM,                "Sfx\\Towners\\Witch03.wav",   nullptr },// TSFX_WITCH3
	{ sfx_STREAM,                "Sfx\\Towners\\Witch04.wav",   nullptr },// TSFX_WITCH4
	{ sfx_STREAM,                "Sfx\\Towners\\Witch05.wav",   nullptr },// TSFX_WITCH5
	{ sfx_STREAM,                "Sfx\\Towners\\Witch06.wav",   nullptr },// TSFX_WITCH6
	{ sfx_STREAM,                "Sfx\\Towners\\Witch07.wav",   nullptr },// TSFX_WITCH7
	{ sfx_STREAM,                "Sfx\\Towners\\Witch08.wav",   nullptr },// TSFX_WITCH8
	{ sfx_STREAM,                "Sfx\\Towners\\Witch09.wav",   nullptr },// TSFX_WITCH9
	{ sfx_STREAM,                "Sfx\\Towners\\Witch10.wav",   nullptr },// TSFX_WITCH10
	{ sfx_STREAM,                "Sfx\\Towners\\Witch11.wav",   nullptr },// TSFX_WITCH11
	{ sfx_STREAM,                "Sfx\\Towners\\Witch12.wav",   nullptr },// TSFX_WITCH12
	{ sfx_STREAM,                "Sfx\\Towners\\Witch13.wav",   nullptr },// TSFX_WITCH13
	{ sfx_STREAM,                "Sfx\\Towners\\Witch14.wav",   nullptr },// TSFX_WITCH14
	{ sfx_STREAM,                "Sfx\\Towners\\Witch15.wav",   nullptr },// TSFX_WITCH15
	{ sfx_STREAM,                "Sfx\\Towners\\Witch16.wav",   nullptr },// TSFX_WITCH16
	{ sfx_STREAM,                "Sfx\\Towners\\Witch17.wav",   nullptr },// TSFX_WITCH17
	{ sfx_STREAM,                "Sfx\\Towners\\Witch18.wav",   nullptr },// TSFX_WITCH18
	{ sfx_STREAM,                "Sfx\\Towners\\Witch19.wav",   nullptr },// TSFX_WITCH19
	{ sfx_STREAM,                "Sfx\\Towners\\Witch20.wav",   nullptr },// TSFX_WITCH20
	{ sfx_STREAM,                "Sfx\\Towners\\Witch21.wav",   nullptr },// TSFX_WITCH21
	{ sfx_STREAM,                "Sfx\\Towners\\Witch22.wav",   nullptr },// TSFX_WITCH22
	{ sfx_STREAM,                "Sfx\\Towners\\Witch23.wav",   nullptr },// TSFX_WITCH23
	{ sfx_STREAM,                "Sfx\\Towners\\Witch24.wav",   nullptr },// TSFX_WITCH24
	{ sfx_STREAM,                "Sfx\\Towners\\Witch25.wav",   nullptr },// TSFX_WITCH25
	{ sfx_STREAM,                "Sfx\\Towners\\Witch26.wav",   nullptr },// TSFX_WITCH26
	{ sfx_STREAM,                "Sfx\\Towners\\Witch27.wav",   nullptr },// TSFX_WITCH27
	{ sfx_STREAM,                "Sfx\\Towners\\Witch28.wav",   nullptr },// TSFX_WITCH28
	{ sfx_STREAM,                "Sfx\\Towners\\Witch29.wav",   nullptr },// TSFX_WITCH29
	{ sfx_STREAM,                "Sfx\\Towners\\Witch30.wav",   nullptr },// TSFX_WITCH30
	{ sfx_STREAM,                "Sfx\\Towners\\Witch31.wav",   nullptr },// TSFX_WITCH31
	{ sfx_STREAM,                "Sfx\\Towners\\Witch32.wav",   nullptr },// TSFX_WITCH32
	{ sfx_STREAM,                "Sfx\\Towners\\Witch33.wav",   nullptr },// TSFX_WITCH33
	{ sfx_STREAM,                "Sfx\\Towners\\Witch34.wav",   nullptr },// TSFX_WITCH34
	{ sfx_STREAM,                "Sfx\\Towners\\Witch35.wav",   nullptr },// TSFX_WITCH35
	{ sfx_STREAM,                "Sfx\\Towners\\Witch36.wav",   nullptr },// TSFX_WITCH36
	{ sfx_STREAM,                "Sfx\\Towners\\Witch37.wav",   nullptr },// TSFX_WITCH37
	{ sfx_STREAM,                "Sfx\\Towners\\Witch38.wav",   nullptr },// TSFX_WITCH38
	{ sfx_STREAM,                "Sfx\\Towners\\Witch39.wav",   nullptr },// TSFX_WITCH39
	{ sfx_STREAM,                "Sfx\\Towners\\Witch40.wav",   nullptr },// TSFX_WITCH40
	{ sfx_STREAM,                "Sfx\\Towners\\Witch41.wav",   nullptr },// TSFX_WITCH41
	{ sfx_STREAM,                "Sfx\\Towners\\Witch42.wav",   nullptr },// TSFX_WITCH42
	{ sfx_STREAM,                "Sfx\\Towners\\Witch43.wav",   nullptr },// TSFX_WITCH43
	{ sfx_STREAM,                "Sfx\\Towners\\Witch44.wav",   nullptr },// TSFX_WITCH44
	{ sfx_STREAM,                "Sfx\\Towners\\Witch45.wav",   nullptr },// TSFX_WITCH45
	{ sfx_STREAM,                "Sfx\\Towners\\Witch46.wav",   nullptr },// TSFX_WITCH46
	{ sfx_STREAM,                "Sfx\\Towners\\Witch47.wav",   nullptr },// TSFX_WITCH47
	{ sfx_STREAM,                "Sfx\\Towners\\Witch48.wav",   nullptr },// TSFX_WITCH48
	{ sfx_STREAM,                "Sfx\\Towners\\Witch49.wav",   nullptr },// TSFX_WITCH49
	{ sfx_STREAM,                "Sfx\\Towners\\Witch50.wav",   nullptr },// TSFX_WITCH50
	{ sfx_STREAM,                "Sfx\\Towners\\Wound01.wav",   nullptr },// TSFX_WOUND
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage01.wav",   nullptr },// PS_MAGE1
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage02.wav",   nullptr },// PS_MAGE2
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage03.wav",   nullptr },// PS_MAGE3
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage04.wav",   nullptr },// PS_MAGE4
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage05.wav",   nullptr },// PS_MAGE5
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage06.wav",   nullptr },// PS_MAGE6
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage07.wav",   nullptr },// PS_MAGE7
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage08.wav",   nullptr },// PS_MAGE8
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage09.wav",   nullptr },// PS_MAGE9
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage10.wav",   nullptr },// PS_MAGE10
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage11.wav",   nullptr },// PS_MAGE11
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage12.wav",   nullptr },// PS_MAGE12
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage13.wav",   nullptr },// PS_MAGE13
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage14.wav",   nullptr },// PS_MAGE14
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage15.wav",   nullptr },// PS_MAGE15
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage16.wav",   nullptr },// PS_MAGE16
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage17.wav",   nullptr },// PS_MAGE17
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage18.wav",   nullptr },// PS_MAGE18
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage19.wav",   nullptr },// PS_MAGE19
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage20.wav",   nullptr },// PS_MAGE20
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage21.wav",   nullptr },// PS_MAGE21
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage22.wav",   nullptr },// PS_MAGE22
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage23.wav",   nullptr },// PS_MAGE23
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage24.wav",   nullptr },// PS_MAGE24
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage25.wav",   nullptr },// PS_MAGE25
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage26.wav",   nullptr },// PS_MAGE26
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage27.wav",   nullptr },// PS_MAGE27
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage28.wav",   nullptr },// PS_MAGE28
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage29.wav",   nullptr },// PS_MAGE29
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage30.wav",   nullptr },// PS_MAGE30
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage31.wav",   nullptr },// PS_MAGE31
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage32.wav",   nullptr },// PS_MAGE32
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage33.wav",   nullptr },// PS_MAGE33
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage34.wav",   nullptr },// PS_MAGE34
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage35.wav",   nullptr },// PS_MAGE35
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage36.wav",   nullptr },// PS_MAGE36
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage37.wav",   nullptr },// PS_MAGE37
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage38.wav",   nullptr },// PS_MAGE38
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage39.wav",   nullptr },// PS_MAGE39
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage40.wav",   nullptr },// PS_MAGE40
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage41.wav",   nullptr },// PS_MAGE41
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage42.wav",   nullptr },// PS_MAGE42
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage43.wav",   nullptr },// PS_MAGE43
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage44.wav",   nullptr },// PS_MAGE44
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage45.wav",   nullptr },// PS_MAGE45
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage46.wav",   nullptr },// PS_MAGE46
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage47.wav",   nullptr },// PS_MAGE47
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage48.wav",   nullptr },// PS_MAGE48
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage49.wav",   nullptr },// PS_MAGE49
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage50.wav",   nullptr },// PS_MAGE50
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage51.wav",   nullptr },// PS_MAGE51
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage52.wav",   nullptr },// PS_MAGE52
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage53.wav",   nullptr },// PS_MAGE53
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage54.wav",   nullptr },// PS_MAGE54
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage55.wav",   nullptr },// PS_MAGE55
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage56.wav",   nullptr },// PS_MAGE56
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage57.wav",   nullptr },// PS_MAGE57
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage58.wav",   nullptr },// PS_MAGE58
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage59.wav",   nullptr },// PS_MAGE59
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage60.wav",   nullptr },// PS_MAGE60
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage61.wav",   nullptr },// PS_MAGE61
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage62.wav",   nullptr },// PS_MAGE62
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage63.wav",   nullptr },// PS_MAGE63
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage64.wav",   nullptr },// PS_MAGE64
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage65.wav",   nullptr },// PS_MAGE65
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage66.wav",   nullptr },// PS_MAGE66
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage67.wav",   nullptr },// PS_MAGE67
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage68.wav",   nullptr },// PS_MAGE68
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage69.wav",   nullptr },// PS_MAGE69
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage69b.wav",  nullptr },// PS_MAGE69B
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage70.wav",   nullptr },// PS_MAGE70
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage71.wav",   nullptr },// PS_MAGE71
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage72.wav",   nullptr },// PS_MAGE72
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage73.wav",   nullptr },// PS_MAGE73
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage74.wav",   nullptr },// PS_MAGE74
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage75.wav",   nullptr },// PS_MAGE75
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage76.wav",   nullptr },// PS_MAGE76
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage77.wav",   nullptr },// PS_MAGE77
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage78.wav",   nullptr },// PS_MAGE78
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage79.wav",   nullptr },// PS_MAGE79
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage80.wav",   nullptr },// PS_MAGE80
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage81.wav",   nullptr },// PS_MAGE81
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage82.wav",   nullptr },// PS_MAGE82
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage83.wav",   nullptr },// PS_MAGE83
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage84.wav",   nullptr },// PS_MAGE84
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage85.wav",   nullptr },// PS_MAGE85
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage86.wav",   nullptr },// PS_MAGE86
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage87.wav",   nullptr },// PS_MAGE87
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage88.wav",   nullptr },// PS_MAGE88
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage89.wav",   nullptr },// PS_MAGE89
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage90.wav",   nullptr },// PS_MAGE90
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage91.wav",   nullptr },// PS_MAGE91
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage92.wav",   nullptr },// PS_MAGE92
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage93.wav",   nullptr },// PS_MAGE93
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage94.wav",   nullptr },// PS_MAGE94
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage95.wav",   nullptr },// PS_MAGE95
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage96.wav",   nullptr },// PS_MAGE96
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage97.wav",   nullptr },// PS_MAGE97
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage98.wav",   nullptr },// PS_MAGE98
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage99.wav",   nullptr },// PS_MAGE99
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage100.wav",  nullptr },// PS_MAGE100
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage101.wav",  nullptr },// PS_MAGE101
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage102.wav",  nullptr },// PS_MAGE102
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue01.wav",     nullptr },// PS_ROGUE1
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue02.wav",     nullptr },// PS_ROGUE2
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue03.wav",     nullptr },// PS_ROGUE3
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue04.wav",     nullptr },// PS_ROGUE4
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue05.wav",     nullptr },// PS_ROGUE5
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue06.wav",     nullptr },// PS_ROGUE6
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue07.wav",     nullptr },// PS_ROGUE7
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue08.wav",     nullptr },// PS_ROGUE8
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue09.wav",     nullptr },// PS_ROGUE9
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue10.wav",     nullptr },// PS_ROGUE10
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue11.wav",     nullptr },// PS_ROGUE11
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue12.wav",     nullptr },// PS_ROGUE12
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue13.wav",     nullptr },// PS_ROGUE13
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue14.wav",     nullptr },// PS_ROGUE14
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue15.wav",     nullptr },// PS_ROGUE15
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue16.wav",     nullptr },// PS_ROGUE16
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue17.wav",     nullptr },// PS_ROGUE17
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue18.wav",     nullptr },// PS_ROGUE18
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue19.wav",     nullptr },// PS_ROGUE19
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue20.wav",     nullptr },// PS_ROGUE20
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue21.wav",     nullptr },// PS_ROGUE21
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue22.wav",     nullptr },// PS_ROGUE22
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue23.wav",     nullptr },// PS_ROGUE23
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue24.wav",     nullptr },// PS_ROGUE24
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue25.wav",     nullptr },// PS_ROGUE25
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue26.wav",     nullptr },// PS_ROGUE26
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue27.wav",     nullptr },// PS_ROGUE27
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue28.wav",     nullptr },// PS_ROGUE28
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue29.wav",     nullptr },// PS_ROGUE29
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue30.wav",     nullptr },// PS_ROGUE30
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue31.wav",     nullptr },// PS_ROGUE31
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue32.wav",     nullptr },// PS_ROGUE32
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue33.wav",     nullptr },// PS_ROGUE33
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue34.wav",     nullptr },// PS_ROGUE34
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue35.wav",     nullptr },// PS_ROGUE35
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue36.wav",     nullptr },// PS_ROGUE36
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue37.wav",     nullptr },// PS_ROGUE37
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue38.wav",     nullptr },// PS_ROGUE38
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue39.wav",     nullptr },// PS_ROGUE39
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue40.wav",     nullptr },// PS_ROGUE40
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue41.wav",     nullptr },// PS_ROGUE41
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue42.wav",     nullptr },// PS_ROGUE42
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue43.wav",     nullptr },// PS_ROGUE43
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue44.wav",     nullptr },// PS_ROGUE44
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue45.wav",     nullptr },// PS_ROGUE45
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue46.wav",     nullptr },// PS_ROGUE46
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue47.wav",     nullptr },// PS_ROGUE47
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue48.wav",     nullptr },// PS_ROGUE48
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue49.wav",     nullptr },// PS_ROGUE49
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue50.wav",     nullptr },// PS_ROGUE50
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue51.wav",     nullptr },// PS_ROGUE51
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue52.wav",     nullptr },// PS_ROGUE52
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue53.wav",     nullptr },// PS_ROGUE53
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue54.wav",     nullptr },// PS_ROGUE54
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue55.wav",     nullptr },// PS_ROGUE55
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue56.wav",     nullptr },// PS_ROGUE56
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue57.wav",     nullptr },// PS_ROGUE57
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue58.wav",     nullptr },// PS_ROGUE58
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue59.wav",     nullptr },// PS_ROGUE59
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue60.wav",     nullptr },// PS_ROGUE60
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue61.wav",     nullptr },// PS_ROGUE61
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue62.wav",     nullptr },// PS_ROGUE62
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue63.wav",     nullptr },// PS_ROGUE63
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue64.wav",     nullptr },// PS_ROGUE64
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue65.wav",     nullptr },// PS_ROGUE65
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue66.wav",     nullptr },// PS_ROGUE66
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue67.wav",     nullptr },// PS_ROGUE67
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue68.wav",     nullptr },// PS_ROGUE68
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue69.wav",     nullptr },// PS_ROGUE69
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue69b.wav",    nullptr },// PS_ROGUE69B
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue70.wav",     nullptr },// PS_ROGUE70
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue71.wav",     nullptr },// PS_ROGUE71
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue72.wav",     nullptr },// PS_ROGUE72
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue73.wav",     nullptr },// PS_ROGUE73
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue74.wav",     nullptr },// PS_ROGUE74
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue75.wav",     nullptr },// PS_ROGUE75
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue76.wav",     nullptr },// PS_ROGUE76
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue77.wav",     nullptr },// PS_ROGUE77
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue78.wav",     nullptr },// PS_ROGUE78
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue79.wav",     nullptr },// PS_ROGUE79
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue80.wav",     nullptr },// PS_ROGUE80
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue81.wav",     nullptr },// PS_ROGUE81
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue82.wav",     nullptr },// PS_ROGUE82
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue83.wav",     nullptr },// PS_ROGUE83
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue84.wav",     nullptr },// PS_ROGUE84
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue85.wav",     nullptr },// PS_ROGUE85
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue86.wav",     nullptr },// PS_ROGUE86
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue87.wav",     nullptr },// PS_ROGUE87
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue88.wav",     nullptr },// PS_ROGUE88
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue89.wav",     nullptr },// PS_ROGUE89
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue90.wav",     nullptr },// PS_ROGUE90
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue91.wav",     nullptr },// PS_ROGUE91
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue92.wav",     nullptr },// PS_ROGUE92
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue93.wav",     nullptr },// PS_ROGUE93
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue94.wav",     nullptr },// PS_ROGUE94
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue95.wav",     nullptr },// PS_ROGUE95
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue96.wav",     nullptr },// PS_ROGUE96
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue97.wav",     nullptr },// PS_ROGUE97
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue98.wav",     nullptr },// PS_ROGUE98
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue99.wav",     nullptr },// PS_ROGUE99
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue100.wav",    nullptr },// PS_ROGUE100
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue101.wav",    nullptr },// PS_ROGUE101
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue102.wav",    nullptr },// PS_ROGUE102
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior01.wav",  nullptr },// PS_WARR1
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior02.wav",  nullptr },// PS_WARR2
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior03.wav",  nullptr },// PS_WARR3
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior04.wav",  nullptr },// PS_WARR4
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior05.wav",  nullptr },// PS_WARR5
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior06.wav",  nullptr },// PS_WARR6
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior07.wav",  nullptr },// PS_WARR7
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior08.wav",  nullptr },// PS_WARR8
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior09.wav",  nullptr },// PS_WARR9
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior10.wav",  nullptr },// PS_WARR10
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior11.wav",  nullptr },// PS_WARR11
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior12.wav",  nullptr },// PS_WARR12
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior13.wav",  nullptr },// PS_WARR13
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior14.wav",  nullptr },// PS_WARR14
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Wario14b.wav",  nullptr },// PS_WARR14B
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Wario14c.wav",  nullptr },// PS_WARR14C
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior15.wav",  nullptr },// PS_WARR15
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Wario15b.wav",  nullptr },// PS_WARR15B
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Wario15c.wav",  nullptr },// PS_WARR15C
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior16.wav",  nullptr },// PS_WARR16
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Wario16b.wav",  nullptr },// PS_WARR16B
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Wario16c.wav",  nullptr },// PS_WARR16C
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior17.wav",  nullptr },// PS_WARR17
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior18.wav",  nullptr },// PS_WARR18
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior19.wav",  nullptr },// PS_WARR19
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior20.wav",  nullptr },// PS_WARR20
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior21.wav",  nullptr },// PS_WARR21
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior22.wav",  nullptr },// PS_WARR22
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior23.wav",  nullptr },// PS_WARR23
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior24.wav",  nullptr },// PS_WARR24
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior25.wav",  nullptr },// PS_WARR25
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior26.wav",  nullptr },// PS_WARR26
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior27.wav",  nullptr },// PS_WARR27
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior28.wav",  nullptr },// PS_WARR28
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior29.wav",  nullptr },// PS_WARR29
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior30.wav",  nullptr },// PS_WARR30
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior31.wav",  nullptr },// PS_WARR31
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior32.wav",  nullptr },// PS_WARR32
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior33.wav",  nullptr },// PS_WARR33
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior34.wav",  nullptr },// PS_WARR34
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior35.wav",  nullptr },// PS_WARR35
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior36.wav",  nullptr },// PS_WARR36
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior37.wav",  nullptr },// PS_WARR37
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior38.wav",  nullptr },// PS_WARR38
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior39.wav",  nullptr },// PS_WARR39
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior40.wav",  nullptr },// PS_WARR40
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior41.wav",  nullptr },// PS_WARR41
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior42.wav",  nullptr },// PS_WARR42
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior43.wav",  nullptr },// PS_WARR43
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior44.wav",  nullptr },// PS_WARR44
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior45.wav",  nullptr },// PS_WARR45
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior46.wav",  nullptr },// PS_WARR46
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior47.wav",  nullptr },// PS_WARR47
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior48.wav",  nullptr },// PS_WARR48
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior49.wav",  nullptr },// PS_WARR49
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior50.wav",  nullptr },// PS_WARR50
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior51.wav",  nullptr },// PS_WARR51
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior52.wav",  nullptr },// PS_WARR52
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior53.wav",  nullptr },// PS_WARR53
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior54.wav",  nullptr },// PS_WARR54
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior55.wav",  nullptr },// PS_WARR55
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior56.wav",  nullptr },// PS_WARR56
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior57.wav",  nullptr },// PS_WARR57
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior58.wav",  nullptr },// PS_WARR58
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior59.wav",  nullptr },// PS_WARR59
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior60.wav",  nullptr },// PS_WARR60
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior61.wav",  nullptr },// PS_WARR61
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior62.wav",  nullptr },// PS_WARR62
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior63.wav",  nullptr },// PS_WARR63
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior64.wav",  nullptr },// PS_WARR64
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior65.wav",  nullptr },// PS_WARR65
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior66.wav",  nullptr },// PS_WARR66
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior67.wav",  nullptr },// PS_WARR67
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior68.wav",  nullptr },// PS_WARR68
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior69.wav",  nullptr },// PS_WARR69
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Wario69b.wav",  nullptr },// PS_WARR69B
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior70.wav",  nullptr },// PS_WARR70
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior71.wav",  nullptr },// PS_WARR71
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior72.wav",  nullptr },// PS_WARR72
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior73.wav",  nullptr },// PS_WARR73
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior74.wav",  nullptr },// PS_WARR74
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior75.wav",  nullptr },// PS_WARR75
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior76.wav",  nullptr },// PS_WARR76
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior77.wav",  nullptr },// PS_WARR77
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior78.wav",  nullptr },// PS_WARR78
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior79.wav",  nullptr },// PS_WARR79
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior80.wav",  nullptr },// PS_WARR80
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior81.wav",  nullptr },// PS_WARR81
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior82.wav",  nullptr },// PS_WARR82
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior83.wav",  nullptr },// PS_WARR83
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior84.wav",  nullptr },// PS_WARR84
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior85.wav",  nullptr },// PS_WARR85
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior86.wav",  nullptr },// PS_WARR86
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior87.wav",  nullptr },// PS_WARR87
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior88.wav",  nullptr },// PS_WARR88
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior89.wav",  nullptr },// PS_WARR89
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior90.wav",  nullptr },// PS_WARR90
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior91.wav",  nullptr },// PS_WARR91
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior92.wav",  nullptr },// PS_WARR92
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior93.wav",  nullptr },// PS_WARR93
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior94.wav",  nullptr },// PS_WARR94
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior95.wav",  nullptr },// PS_WARR95
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Wario95b.wav",  nullptr },// PS_WARR95B
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Wario95c.wav",  nullptr },// PS_WARR95C
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Wario95d.wav",  nullptr },// PS_WARR95D
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Wario95e.wav",  nullptr },// PS_WARR95E
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Wario95f.wav",  nullptr },// PS_WARR95F
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Wario96b.wav",  nullptr },// PS_WARR96B
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Wario97.wav",   nullptr },// PS_WARR97
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Wario98.wav",   nullptr },// PS_WARR98
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior99.wav",  nullptr },// PS_WARR99
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Wario100.wav",  nullptr },// PS_WARR100
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Wario101.wav",  nullptr },// PS_WARR101
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Wario102.wav",  nullptr },// PS_WARR102
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk01.wav",       nullptr },// PS_MONK1
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK2
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK3
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK4
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK5
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK6
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK7
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk08.wav",       nullptr },// PS_MONK8
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk09.wav",       nullptr },// PS_MONK9
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk10.wav",       nullptr },// PS_MONK10
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk11.wav",       nullptr },// PS_MONK11
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk12.wav",       nullptr },// PS_MONK12
	{ sfx_MONK,                  "Sfx\\Monk\\Monk13.wav",       nullptr },// PS_MONK13
	{ sfx_MONK,                  "Sfx\\Monk\\Monk14.wav",       nullptr },// PS_MONK14
	{ sfx_MONK,                  "Sfx\\Monk\\Monk15.wav",       nullptr },// PS_MONK15
	{ sfx_MONK,                  "Sfx\\Monk\\Monk16.wav",       nullptr },// PS_MONK16
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK17
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK18
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK19
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK20
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK21
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK22
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK23
	{ sfx_MONK,                  "Sfx\\Monk\\Monk24.wav",       nullptr },// PS_MONK24
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK25
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK26
	{ sfx_MONK,                  "Sfx\\Monk\\Monk27.wav",       nullptr },// PS_MONK27
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK28
	{ sfx_MONK,                  "Sfx\\Monk\\Monk29.wav",       nullptr },// PS_MONK29
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK30
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK31
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK32
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK33
	{ sfx_MONK,                  "Sfx\\Monk\\Monk34.wav",       nullptr },// PS_MONK34
	{ sfx_MONK,                  "Sfx\\Monk\\Monk35.wav",       nullptr },// PS_MONK35
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK36
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK37
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK38
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK39
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK40
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK41
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK42
	{ sfx_MONK,                  "Sfx\\Monk\\Monk43.wav",       nullptr },// PS_MONK43
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK44
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK45
	{ sfx_MONK,                  "Sfx\\Monk\\Monk46.wav",       nullptr },// PS_MONK46
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK47
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK48
	{ sfx_MONK,                  "Sfx\\Monk\\Monk49.wav",       nullptr },// PS_MONK49
	{ sfx_MONK,                  "Sfx\\Monk\\Monk50.wav",       nullptr },// PS_MONK50
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK51
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk52.wav",       nullptr },// PS_MONK52
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK53
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk54.wav",       nullptr },// PS_MONK54
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk55.wav",       nullptr },// PS_MONK55
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk56.wav",       nullptr },// PS_MONK56
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK57
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK58
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK59
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK60
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk61.wav",       nullptr },// PS_MONK61
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk62.wav",       nullptr },// PS_MONK62
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK63
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK64
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK65
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK66
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK67
	{ sfx_MONK,                  "Sfx\\Monk\\Monk68.wav",       nullptr },// PS_MONK68
	{ sfx_MONK,                  "Sfx\\Monk\\Monk69.wav",       nullptr },// PS_MONK69
	{ sfx_MONK,                  "Sfx\\Monk\\Monk69b.wav",      nullptr },// PS_MONK69B
	{ sfx_MONK,                  "Sfx\\Monk\\Monk70.wav",       nullptr },// PS_MONK70
	{ sfx_MONK,                  "Sfx\\Monk\\Monk71.wav",       nullptr },// PS_MONK71
	{ sfx_MONK,                  "Sfx\\Sorceror\\Mage72.wav",   nullptr },// PS_MONK72
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK73
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK74
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK75
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK76
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK77
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK78
	{ sfx_MONK,                  "Sfx\\Monk\\Monk79.wav",       nullptr },// PS_MONK79
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk80.wav",       nullptr },// PS_MONK80
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK81
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk82.wav",       nullptr },// PS_MONK82
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk83.wav",       nullptr },// PS_MONK83
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK84
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK85
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK86
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk87.wav",       nullptr },// PS_MONK87
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk88.wav",       nullptr },// PS_MONK88
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk89.wav",       nullptr },// PS_MONK89
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK90
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk91.wav",       nullptr },// PS_MONK91
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk92.wav",       nullptr },// PS_MONK92
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK93
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk94.wav",       nullptr },// PS_MONK94
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk95.wav",       nullptr },// PS_MONK95
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk96.wav",       nullptr },// PS_MONK96
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk97.wav",       nullptr },// PS_MONK97
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk98.wav",       nullptr },// PS_MONK98
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk99.wav",       nullptr },// PS_MONK99
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK100
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK101
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Misc\\blank.wav",        nullptr },// PS_MONK102
	{ sfx_STREAM,                "Sfx\\Narrator\\Nar01.wav",    nullptr },// PS_NAR1
	{ sfx_STREAM,                "Sfx\\Narrator\\Nar02.wav",    nullptr },// PS_NAR2
	{ sfx_STREAM,                "Sfx\\Narrator\\Nar03.wav",    nullptr },// PS_NAR3
	{ sfx_STREAM,                "Sfx\\Narrator\\Nar04.wav",    nullptr },// PS_NAR4
	{ sfx_STREAM,                "Sfx\\Narrator\\Nar05.wav",    nullptr },// PS_NAR5
	{ sfx_STREAM,                "Sfx\\Narrator\\Nar06.wav",    nullptr },// PS_NAR6
	{ sfx_STREAM,                "Sfx\\Narrator\\Nar07.wav",    nullptr },// PS_NAR7
	{ sfx_STREAM,                "Sfx\\Narrator\\Nar08.wav",    nullptr },// PS_NAR8
	{ sfx_STREAM,                "Sfx\\Narrator\\Nar09.wav",    nullptr },// PS_NAR9
	{ sfx_STREAM,                "Sfx\\Misc\\Lvl16int.wav",     nullptr },// PS_DIABLVLINT
	{ sfx_STREAM,                "Sfx\\Monsters\\Butcher.wav",  nullptr },// USFX_CLEAVER
	{ sfx_STREAM,                "Sfx\\Monsters\\Garbud01.wav", nullptr },// USFX_GARBUD1
	{ sfx_STREAM,                "Sfx\\Monsters\\Garbud02.wav", nullptr },// USFX_GARBUD2
	{ sfx_STREAM,                "Sfx\\Monsters\\Garbud03.wav", nullptr },// USFX_GARBUD3
	{ sfx_STREAM,                "Sfx\\Monsters\\Garbud04.wav", nullptr },// USFX_GARBUD4
	{ sfx_STREAM,                "Sfx\\Monsters\\Izual01.wav",  nullptr },// USFX_IZUAL1
	{ sfx_STREAM,                "Sfx\\Monsters\\Lach01.wav",   nullptr },// USFX_LACH1
	{ sfx_STREAM,                "Sfx\\Monsters\\Lach02.wav",   nullptr },// USFX_LACH2
	{ sfx_STREAM,                "Sfx\\Monsters\\Lach03.wav",   nullptr },// USFX_LACH3
	{ sfx_STREAM,                "Sfx\\Monsters\\Laz01.wav",    nullptr },// USFX_LAZ1
	{ sfx_STREAM,                "Sfx\\Monsters\\Laz02.wav",    nullptr },// USFX_LAZ2
	{ sfx_STREAM,                "Sfx\\Monsters\\Sking01.wav",  nullptr },// USFX_SKING1
	{ sfx_STREAM,                "Sfx\\Monsters\\Snot01.wav",   nullptr },// USFX_SNOT1
	{ sfx_STREAM,                "Sfx\\Monsters\\Snot02.wav",   nullptr },// USFX_SNOT2
	{ sfx_STREAM,                "Sfx\\Monsters\\Snot03.wav",   nullptr },// USFX_SNOT3
	{ sfx_STREAM,                "Sfx\\Monsters\\Warlrd01.wav", nullptr },// USFX_WARLRD1
	{ sfx_STREAM,                "Sfx\\Monsters\\Wlock01.wav",  nullptr },// USFX_WLOCK1
	{ sfx_STREAM,                "Sfx\\Monsters\\Zhar01.wav",   nullptr },// USFX_ZHAR1
	{ sfx_STREAM,                "Sfx\\Monsters\\Zhar02.wav",   nullptr },// USFX_ZHAR2
	{ sfx_STREAM,                "Sfx\\Monsters\\DiabloD.wav",  nullptr },// USFX_DIABLOD
	{ sfx_STREAM,                "Sfx\\Hellfire\\Farmer1.wav",  nullptr },// TSFX_FARMER1
	{ sfx_STREAM,                "Sfx\\Hellfire\\Farmer2.wav",  nullptr },// TSFX_FARMER2
	{ sfx_STREAM,                "Sfx\\Hellfire\\Farmer2A.wav", nullptr },// TSFX_FARMER2A
	{ sfx_STREAM,                "Sfx\\Hellfire\\Farmer3.wav",  nullptr },// TSFX_FARMER3
	{ sfx_STREAM,                "Sfx\\Hellfire\\Farmer4.wav",  nullptr },// TSFX_FARMER4
	{ sfx_STREAM,                "Sfx\\Hellfire\\Farmer5.wav",  nullptr },// TSFX_FARMER5
	{ sfx_STREAM,                "Sfx\\Hellfire\\Farmer6.wav",  nullptr },// TSFX_FARMER6
	{ sfx_STREAM,                "Sfx\\Hellfire\\Farmer7.wav",  nullptr },// TSFX_FARMER7
	{ sfx_STREAM,                "Sfx\\Hellfire\\Farmer8.wav",  nullptr },// TSFX_FARMER8
	{ sfx_STREAM,                "Sfx\\Hellfire\\Farmer9.wav",  nullptr },// TSFX_FARMER9
	{ sfx_STREAM,                "Sfx\\Hellfire\\TEDDYBR1.wav", nullptr },// TSFX_TEDDYBR1
	{ sfx_STREAM,                "Sfx\\Hellfire\\TEDDYBR2.wav", nullptr },// TSFX_TEDDYBR2
	{ sfx_STREAM,                "Sfx\\Hellfire\\TEDDYBR3.wav", nullptr },// TSFX_TEDDYBR3
	{ sfx_STREAM,                "Sfx\\Hellfire\\TEDDYBR4.wav", nullptr },// TSFX_TEDDYBR4
	{ sfx_STREAM,                "Sfx\\Hellfire\\DEFILER1.wav", nullptr },// USFX_DEFILER1
	{ sfx_STREAM,                "Sfx\\Hellfire\\DEFILER2.wav", nullptr },// USFX_DEFILER2
	{ sfx_STREAM,                "Sfx\\Hellfire\\DEFILER3.wav", nullptr },// USFX_DEFILER3
	{ sfx_STREAM,                "Sfx\\Hellfire\\DEFILER4.wav", nullptr },// USFX_DEFILER4
	{ sfx_STREAM,                "Sfx\\Hellfire\\DEFILER8.wav", nullptr },// USFX_DEFILER8
	{ sfx_STREAM,                "Sfx\\Hellfire\\DEFILER6.wav", nullptr },// USFX_DEFILER6
	{ sfx_STREAM,                "Sfx\\Hellfire\\DEFILER7.wav", nullptr },// USFX_DEFILER7
	{ sfx_STREAM,                "Sfx\\Hellfire\\NAKRUL1.wav",  nullptr },// USFX_NAKRUL1
	{ sfx_STREAM,                "Sfx\\Hellfire\\NAKRUL2.wav",  nullptr },// USFX_NAKRUL2
	{ sfx_STREAM,                "Sfx\\Hellfire\\NAKRUL3.wav",  nullptr },// USFX_NAKRUL3
	{ sfx_STREAM,                "Sfx\\Hellfire\\NAKRUL4.wav",  nullptr },// USFX_NAKRUL4
	{ sfx_STREAM,                "Sfx\\Hellfire\\NAKRUL5.wav",  nullptr },// USFX_NAKRUL5
	{ sfx_STREAM,                "Sfx\\Hellfire\\NAKRUL6.wav",  nullptr },// USFX_NAKRUL6
	{ sfx_STREAM,                "Sfx\\Hellfire\\NARATR3.wav",  nullptr },// PS_NARATR3
	{ sfx_STREAM,                "Sfx\\Hellfire\\COWSUT1.wav",  nullptr },// TSFX_COWSUT1
	{ sfx_STREAM,                "Sfx\\Hellfire\\COWSUT2.wav",  nullptr },// TSFX_COWSUT2
	{ sfx_STREAM,                "Sfx\\Hellfire\\COWSUT3.wav",  nullptr },// TSFX_COWSUT3
	{ sfx_STREAM,                "Sfx\\Hellfire\\COWSUT4.wav",  nullptr },// TSFX_COWSUT4
	{ sfx_STREAM,                "Sfx\\Hellfire\\COWSUT4A.wav", nullptr },// TSFX_COWSUT4A
	{ sfx_STREAM,                "Sfx\\Hellfire\\COWSUT5.wav",  nullptr },// TSFX_COWSUT5
	{ sfx_STREAM,                "Sfx\\Hellfire\\COWSUT6.wav",  nullptr },// TSFX_COWSUT6
	{ sfx_STREAM,                "Sfx\\Hellfire\\COWSUT7.wav",  nullptr },// TSFX_COWSUT7
	{ sfx_STREAM,                "Sfx\\Hellfire\\COWSUT8.wav",  nullptr },// TSFX_COWSUT8
	{ sfx_STREAM,                "Sfx\\Hellfire\\COWSUT9.wav",  nullptr },// TSFX_COWSUT9
	{ sfx_STREAM,                "Sfx\\Hellfire\\COWSUT10.wav", nullptr },// TSFX_COWSUT10
	{ sfx_STREAM,                "Sfx\\Hellfire\\COWSUT11.wav", nullptr },// TSFX_COWSUT11
	{ sfx_STREAM,                "Sfx\\Hellfire\\COWSUT12.wav", nullptr },// TSFX_COWSUT12
	{ sfx_STREAM,                "Sfx\\Hellfire\\Skljrn1.wav",  nullptr },// USFX_SKLJRN1
	{ sfx_STREAM,                "Sfx\\Hellfire\\Naratr6.wav",  nullptr },// PS_NARATR6
	{ sfx_STREAM,                "Sfx\\Hellfire\\Naratr7.wav",  nullptr },// PS_NARATR7
	{ sfx_STREAM,                "Sfx\\Hellfire\\Naratr8.wav",  nullptr },// PS_NARATR8
	{ sfx_STREAM,                "Sfx\\Hellfire\\Naratr5.wav",  nullptr },// PS_NARATR5
	{ sfx_STREAM,                "Sfx\\Hellfire\\Naratr9.wav",  nullptr },// PS_NARATR9
	{ sfx_STREAM,                "Sfx\\Hellfire\\Naratr4.wav",  nullptr },// PS_NARATR4
	{ sfx_STREAM,                "Sfx\\Hellfire\\TRADER1.wav",  nullptr },// TSFX_TRADER1
	{ sfx_MISC | sfx_HELLFIRE,   "Sfx\\Items\\Cropen.wav",      nullptr },// IS_CROPEN
	{ sfx_MISC | sfx_HELLFIRE,   "Sfx\\Items\\Crclos.wav",      nullptr },// IS_CRCLOS
	// clang-format on
};

void StreamPlay(TSFX *pSFX, int lVolume, int lPan)
{
	assert(pSFX);
	assert(pSFX->bFlags & sfx_STREAM);
	stream_stop();

	if (lVolume >= VOLUME_MIN) {
		if (lVolume > VOLUME_MAX)
			lVolume = VOLUME_MAX;
		if (pSFX->pSnd == nullptr)
			pSFX->pSnd = sound_file_load(pSFX->pszName, AllowStreaming);
		pSFX->pSnd->DSB.Play(lVolume, sound_get_or_set_sound_volume(1), lPan);
		sgpStreamSFX = pSFX;
	}
}

void StreamUpdate()
{
	if (sgpStreamSFX != nullptr && !sgpStreamSFX->pSnd->isPlaying()) {
		stream_stop();
	}
}

void PlaySfxPriv(TSFX *pSFX, bool loc, Point position)
{
	if (Players[MyPlayerId].pLvlLoad != 0 && gbIsMultiplayer) {
		return;
	}
	if (!gbSndInited || !gbSoundOn || gbBufferMsgs != 0) {
		return;
	}

	if ((pSFX->bFlags & (sfx_STREAM | sfx_MISC)) == 0 && pSFX->pSnd != nullptr && pSFX->pSnd->isPlaying()) {
		return;
	}

	int lVolume = 0;
	int lPan = 0;
	if (loc && !CalculateSoundPosition(position, &lVolume, &lPan)) {
		return;
	}

	if ((pSFX->bFlags & sfx_STREAM) != 0) {
		StreamPlay(pSFX, lVolume, lPan);
		return;
	}

	if (pSFX->pSnd == nullptr)
		pSFX->pSnd = sound_file_load(pSFX->pszName);

	if (pSFX->pSnd != nullptr)
		snd_play_snd(pSFX->pSnd.get(), lVolume, lPan);
}

_sfx_id RndSFX(_sfx_id psfx)
{
	int nRand;

	switch (psfx) {
	case PS_WARR69:
	case PS_MAGE69:
	case PS_ROGUE69:
	case PS_MONK69:
	case PS_SWING:
	case LS_ACID:
	case IS_FMAG:
	case IS_MAGIC:
	case IS_BHIT:
		nRand = 2;
		break;
	case PS_WARR14:
	case PS_WARR15:
	case PS_WARR16:
	case PS_WARR2:
	case PS_ROGUE14:
	case PS_MAGE14:
	case PS_MONK14:
		nRand = 3;
		break;
	default:
		return psfx;
	}

	return static_cast<_sfx_id>(psfx + GenerateRnd(nRand));
}

void PrivSoundInit(BYTE bLoadMask)
{
	if (!gbSndInited) {
		return;
	}

	for (auto &sfx : sgSFX) {
		if (sfx.pSnd != nullptr) {
			continue;
		}

		if ((sfx.bFlags & sfx_STREAM) != 0) {
			continue;
		}

		if ((sfx.bFlags & bLoadMask) == 0) {
			continue;
		}

		if (!gbIsHellfire && (sfx.bFlags & sfx_HELLFIRE) != 0) {
			continue;
		}

		sfx.pSnd = sound_file_load(sfx.pszName);
	}
}

} // namespace

bool effect_is_playing(int nSFX)
{
	TSFX *sfx = &sgSFX[nSFX];
	if (sfx->pSnd != nullptr)
		return sfx->pSnd->isPlaying();

	if ((sfx->bFlags & sfx_STREAM) != 0)
		return sfx == sgpStreamSFX;

	return false;
}

void stream_stop()
{
	if (sgpStreamSFX != nullptr) {
		sgpStreamSFX->pSnd = nullptr;
		sgpStreamSFX = nullptr;
	}
}

void InitMonsterSND(int monst)
{
	if (!gbSndInited) {
		return;
	}

	const int mtype = LevelMonsterTypes[monst].mtype;
	for (int i = 0; i < 4; i++) {
		if (MonstSndChar[i] != 's' || MonsterData[mtype].snd_special) {
			for (int j = 0; j < 2; j++) {
				char path[MAX_PATH];
				sprintf(path, MonsterData[mtype].sndfile, MonstSndChar[i], j + 1);
				LevelMonsterTypes[monst].Snds[i][j] = sound_file_load(path);
			}
		}
	}
}

void FreeMonsterSnd()
{
	for (int i = 0; i < LevelMonsterTypeCount; i++) {
		for (auto &variants : LevelMonsterTypes[i].Snds) {
			for (auto &snd : variants) {
				snd = nullptr;
			}
		}
	}
}

bool CalculateSoundPosition(Point soundPosition, int *plVolume, int *plPan)
{
	const auto &playerPosition = Players[MyPlayerId].position.tile;
	const auto delta = soundPosition - playerPosition;

	int pan = (delta.deltaX - delta.deltaY) * 256;
	*plPan = clamp(pan, PAN_MIN, PAN_MAX);

	int volume = playerPosition.ApproxDistance(soundPosition);
	volume *= -64;

	if (volume <= ATTENUATION_MIN)
		return false;

	*plVolume = volume;

	return true;
}

void PlaySFX(_sfx_id psfx)
{
	psfx = RndSFX(psfx);

	PlaySfxPriv(&sgSFX[psfx], false, { 0, 0 });
}

void PlaySfxLoc(_sfx_id psfx, Point position, bool randomizeByCategory)
{
	if (randomizeByCategory) {
		psfx = RndSFX(psfx);
	}

	if (psfx >= 0 && psfx <= 3) {
		TSnd *pSnd = sgSFX[psfx].pSnd.get();
		if (pSnd != nullptr)
			pSnd->start_tc = 0;
	}

	PlaySfxPriv(&sgSFX[psfx], true, position);
}

void sound_stop()
{
	ClearDuplicateSounds();
	for (auto &sfx : sgSFX) {
		if (sfx.pSnd != nullptr) {
			sfx.pSnd->DSB.Stop();
		}
	}
}

void sound_update()
{
	if (!gbSndInited) {
		return;
	}

	StreamUpdate();
}

void effects_cleanup_sfx()
{
	sound_stop();

	for (auto &sfx : sgSFX)
		sfx.pSnd = nullptr;
}

void sound_init()
{
	uint8_t mask = sfx_MISC;
	if (gbIsMultiplayer) {
		mask |= sfx_WARRIOR;
		if (!gbIsSpawn)
			mask |= (sfx_ROGUE | sfx_SORCERER);
		if (gbIsHellfire)
			mask |= sfx_MONK;
	} else {
		switch (Players[MyPlayerId]._pClass) {
		case HeroClass::Warrior:
		case HeroClass::Barbarian:
			mask |= sfx_WARRIOR;
			break;
		case HeroClass::Rogue:
		case HeroClass::Bard:
			mask |= sfx_ROGUE;
			break;
		case HeroClass::Sorcerer:
			mask |= sfx_SORCERER;
			break;
		case HeroClass::Monk:
			mask |= sfx_MONK;
			break;
		default:
			app_fatal("effects:1");
		}
	}

	PrivSoundInit(mask);
}

void ui_sound_init()
{
	PrivSoundInit(sfx_UI);
}

void effects_play_sound(const char *sndFile)
{
	if (!gbSndInited || !gbSoundOn) {
		return;
	}

	for (auto &sfx : sgSFX) {
		if (strcasecmp(sfx.pszName, sndFile) == 0 && sfx.pSnd != nullptr) {
			if (!sfx.pSnd->isPlaying())
				snd_play_snd(sfx.pSnd.get(), 0, 0);

			return;
		}
	}
}

int GetSFXLength(int nSFX)
{
	if (sgSFX[nSFX].pSnd == nullptr)
		sgSFX[nSFX].pSnd = sound_file_load(sgSFX[nSFX].pszName,
		    /*stream=*/AllowStreaming && (sgSFX[nSFX].bFlags & sfx_STREAM) != 0);
	return sgSFX[nSFX].pSnd->DSB.GetLength();
}

} // namespace devilution
