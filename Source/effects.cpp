/**
 * @file effects.cpp
 *
 * Implementation of functions for loading and playing sounds.
 */
#include "effects.h"

#include "engine/random.hpp"
#include "engine/sound.h"
#include "engine/sound_defs.hpp"
#include "init.h"
#include "player.h"
#include "utils/stdcompat/algorithm.hpp"
#include "utils/str_cat.hpp"

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

/* data */
/** List of all sounds, except monsters and music */
TSFX sgSFX[] = {
	// clang-format off
	// bFlags,                   pszName,                       pSnd
	{ sfx_MISC,                  "sfx\\misc\\walk1.wav",        nullptr },// PS_WALK1
	{ sfx_MISC,                  "sfx\\misc\\walk2.wav",        nullptr },// PS_WALK2
	{ sfx_MISC,                  "sfx\\misc\\walk3.wav",        nullptr },// PS_WALK3
	{ sfx_MISC,                  "sfx\\misc\\walk4.wav",        nullptr },// PS_WALK4
	{ sfx_MISC,                  "sfx\\misc\\bfire.wav",        nullptr },// PS_BFIRE
	{ sfx_MISC,                  "sfx\\misc\\fmag.wav",         nullptr },// PS_FMAG
	{ sfx_MISC,                  "sfx\\misc\\tmag.wav",         nullptr },// PS_TMAG
	{ sfx_MISC,                  "sfx\\misc\\lghit.wav",        nullptr },// PS_LGHIT
	{ sfx_MISC,                  "sfx\\misc\\lghit1.wav",       nullptr },// PS_LGHIT1
	{ sfx_MISC,                  "sfx\\misc\\swing.wav",        nullptr },// PS_SWING
	{ sfx_MISC,                  "sfx\\misc\\swing2.wav",       nullptr },// PS_SWING2
	{ sfx_MISC,                  "sfx\\misc\\dead.wav",         nullptr },// PS_DEAD
	{ sfx_MISC | sfx_HELLFIRE,   "sfx\\misc\\sting1.wav",       nullptr },// IS_STING1
	{ sfx_MISC | sfx_HELLFIRE,   "sfx\\misc\\fballbow.wav",     nullptr },// IS_FBALLBOW
	{ sfx_STREAM,                "sfx\\misc\\questdon.wav",     nullptr },// IS_QUESTDN
	{ sfx_MISC,                  "sfx\\items\\armrfkd.wav",     nullptr },// IS_ARMRFKD
	{ sfx_MISC,                  "sfx\\items\\barlfire.wav",    nullptr },// IS_BARLFIRE
	{ sfx_MISC,                  "sfx\\items\\barrel.wav",      nullptr },// IS_BARREL
	{ sfx_MISC | sfx_HELLFIRE,   "sfx\\items\\podpop8.wav",     nullptr },// IS_POPPOP8
	{ sfx_MISC | sfx_HELLFIRE,   "sfx\\items\\podpop5.wav",     nullptr },// IS_POPPOP5
	{ sfx_MISC | sfx_HELLFIRE,   "sfx\\items\\urnpop3.wav",     nullptr },// IS_POPPOP3
	{ sfx_MISC | sfx_HELLFIRE,   "sfx\\items\\urnpop2.wav",     nullptr },// IS_POPPOP2
	{ sfx_MISC,                  "sfx\\items\\bhit.wav",        nullptr },// IS_BHIT
	{ sfx_MISC,                  "sfx\\items\\bhit1.wav",       nullptr },// IS_BHIT1
	{ sfx_MISC,                  "sfx\\items\\chest.wav",       nullptr },// IS_CHEST
	{ sfx_MISC,                  "sfx\\items\\doorclos.wav",    nullptr },// IS_DOORCLOS
	{ sfx_MISC,                  "sfx\\items\\dooropen.wav",    nullptr },// IS_DOOROPEN
	{ sfx_MISC,                  "sfx\\items\\flipanvl.wav",    nullptr },// IS_FANVL
	{ sfx_MISC,                  "sfx\\items\\flipaxe.wav",     nullptr },// IS_FAXE
	{ sfx_MISC,                  "sfx\\items\\flipblst.wav",    nullptr },// IS_FBLST
	{ sfx_MISC,                  "sfx\\items\\flipbody.wav",    nullptr },// IS_FBODY
	{ sfx_MISC,                  "sfx\\items\\flipbook.wav",    nullptr },// IS_FBOOK
	{ sfx_MISC,                  "sfx\\items\\flipbow.wav",     nullptr },// IS_FBOW
	{ sfx_MISC,                  "sfx\\items\\flipcap.wav",     nullptr },// IS_FCAP
	{ sfx_MISC,                  "sfx\\items\\flipharm.wav",    nullptr },// IS_FHARM
	{ sfx_MISC,                  "sfx\\items\\fliplarm.wav",    nullptr },// IS_FLARM
	{ sfx_MISC,                  "sfx\\items\\flipmag.wav",     nullptr },// IS_FMAG
	{ sfx_MISC,                  "sfx\\items\\flipmag1.wav",    nullptr },// IS_FMAG1
	{ sfx_MISC,                  "sfx\\items\\flipmush.wav",    nullptr },// IS_FMUSH
	{ sfx_MISC,                  "sfx\\items\\flippot.wav",     nullptr },// IS_FPOT
	{ sfx_MISC,                  "sfx\\items\\flipring.wav",    nullptr },// IS_FRING
	{ sfx_MISC,                  "sfx\\items\\fliprock.wav",    nullptr },// IS_FROCK
	{ sfx_MISC,                  "sfx\\items\\flipscrl.wav",    nullptr },// IS_FSCRL
	{ sfx_MISC,                  "sfx\\items\\flipshld.wav",    nullptr },// IS_FSHLD
	{ sfx_MISC,                  "sfx\\items\\flipsign.wav",    nullptr },// IS_FSIGN
	{ sfx_MISC,                  "sfx\\items\\flipstaf.wav",    nullptr },// IS_FSTAF
	{ sfx_MISC,                  "sfx\\items\\flipswor.wav",    nullptr },// IS_FSWOR
	{ sfx_MISC,                  "sfx\\items\\gold.wav",        nullptr },// IS_GOLD
	{ sfx_MISC,                  "sfx\\items\\hlmtfkd.wav",     nullptr },// IS_HLMTFKD
	{ sfx_MISC,                  "sfx\\items\\invanvl.wav",     nullptr },// IS_IANVL
	{ sfx_MISC,                  "sfx\\items\\invaxe.wav",      nullptr },// IS_IAXE
	{ sfx_MISC,                  "sfx\\items\\invblst.wav",     nullptr },// IS_IBLST
	{ sfx_MISC,                  "sfx\\items\\invbody.wav",     nullptr },// IS_IBODY
	{ sfx_MISC,                  "sfx\\items\\invbook.wav",     nullptr },// IS_IBOOK
	{ sfx_MISC,                  "sfx\\items\\invbow.wav",      nullptr },// IS_IBOW
	{ sfx_MISC,                  "sfx\\items\\invcap.wav",      nullptr },// IS_ICAP
	{ sfx_MISC,                  "sfx\\items\\invgrab.wav",     nullptr },// IS_IGRAB
	{ sfx_MISC,                  "sfx\\items\\invharm.wav",     nullptr },// IS_IHARM
	{ sfx_MISC,                  "sfx\\items\\invlarm.wav",     nullptr },// IS_ILARM
	{ sfx_MISC,                  "sfx\\items\\invmush.wav",     nullptr },// IS_IMUSH
	{ sfx_MISC,                  "sfx\\items\\invpot.wav",      nullptr },// IS_IPOT
	{ sfx_MISC,                  "sfx\\items\\invring.wav",     nullptr },// IS_IRING
	{ sfx_MISC,                  "sfx\\items\\invrock.wav",     nullptr },// IS_IROCK
	{ sfx_MISC,                  "sfx\\items\\invscrol.wav",    nullptr },// IS_ISCROL
	{ sfx_MISC,                  "sfx\\items\\invshiel.wav",    nullptr },// IS_ISHIEL
	{ sfx_MISC,                  "sfx\\items\\invsign.wav",     nullptr },// IS_ISIGN
	{ sfx_MISC,                  "sfx\\items\\invstaf.wav",     nullptr },// IS_ISTAF
	{ sfx_MISC,                  "sfx\\items\\invsword.wav",    nullptr },// IS_ISWORD
	{ sfx_MISC,                  "sfx\\items\\lever.wav",       nullptr },// IS_LEVER
	{ sfx_MISC,                  "sfx\\items\\magic.wav",       nullptr },// IS_MAGIC
	{ sfx_MISC,                  "sfx\\items\\magic1.wav",      nullptr },// IS_MAGIC1
	{ sfx_MISC,                  "sfx\\items\\readbook.wav",    nullptr },// IS_RBOOK
	{ sfx_MISC,                  "sfx\\items\\sarc.wav",        nullptr },// IS_SARC
	{ sfx_MISC,                  "sfx\\items\\shielfkd.wav",    nullptr },// IS_SHLDFKD
	{ sfx_MISC,                  "sfx\\items\\swrdfkd.wav",     nullptr },// IS_SWRDFKD
	{ sfx_UI,                    "sfx\\items\\titlemov.wav",    nullptr },// IS_TITLEMOV
	{ sfx_UI,                    "sfx\\items\\titlslct.wav",    nullptr },// IS_TITLSLCT
	{ sfx_UI,                    "sfx\\misc\\blank.wav",        nullptr },// SFX_SILENCE
	{ sfx_MISC,                  "sfx\\items\\trap.wav",        nullptr },// IS_TRAP
	{ sfx_MISC,                  "sfx\\misc\\cast1.wav",        nullptr },// IS_CAST1
	{ sfx_MISC,                  "sfx\\misc\\cast10.wav",       nullptr },// IS_CAST10
	{ sfx_MISC,                  "sfx\\misc\\cast12.wav",       nullptr },// IS_CAST12
	{ sfx_MISC,                  "sfx\\misc\\cast2.wav",        nullptr },// IS_CAST2
	{ sfx_MISC,                  "sfx\\misc\\cast3.wav",        nullptr },// IS_CAST3
	{ sfx_MISC,                  "sfx\\misc\\cast4.wav",        nullptr },// IS_CAST4
	{ sfx_MISC,                  "sfx\\misc\\cast5.wav",        nullptr },// IS_CAST5
	{ sfx_MISC,                  "sfx\\misc\\cast6.wav",        nullptr },// IS_CAST6
	{ sfx_MISC,                  "sfx\\misc\\cast7.wav",        nullptr },// IS_CAST7
	{ sfx_MISC,                  "sfx\\misc\\cast8.wav",        nullptr },// IS_CAST8
	{ sfx_MISC,                  "sfx\\misc\\cast9.wav",        nullptr },// IS_CAST9
	{ sfx_MISC,                  "sfx\\misc\\healing.wav",      nullptr },// LS_HEALING
	{ sfx_MISC,                  "sfx\\misc\\repair.wav",       nullptr },// IS_REPAIR
	{ sfx_MISC,                  "sfx\\misc\\acids1.wav",       nullptr },// LS_ACID
	{ sfx_MISC,                  "sfx\\misc\\acids2.wav",       nullptr },// LS_ACIDS
	{ sfx_MISC,                  "sfx\\misc\\apoc.wav",         nullptr },// LS_APOC
	{ sfx_MISC,                  "sfx\\misc\\arrowall.wav",     nullptr },// LS_ARROWALL
	{ sfx_MISC,                  "sfx\\misc\\bldboil.wav",      nullptr },// LS_BLODBOIL
	{ sfx_MISC,                  "sfx\\misc\\blodstar.wav",     nullptr },// LS_BLODSTAR
	{ sfx_MISC,                  "sfx\\misc\\blsimpt.wav",      nullptr },// LS_BLSIMPT
	{ sfx_MISC,                  "sfx\\misc\\bonesp.wav",       nullptr },// LS_BONESP
	{ sfx_MISC,                  "sfx\\misc\\bsimpct.wav",      nullptr },// LS_BSIMPCT
	{ sfx_MISC,                  "sfx\\misc\\caldron.wav",      nullptr },// LS_CALDRON
	{ sfx_MISC,                  "sfx\\misc\\cbolt.wav",        nullptr },// LS_CBOLT
	{ sfx_MISC,                  "sfx\\misc\\chltning.wav",     nullptr },// LS_CHLTNING
	{ sfx_MISC,                  "sfx\\misc\\dserp.wav",        nullptr },// LS_DSERP
	{ sfx_MISC,                  "sfx\\misc\\elecimp1.wav",     nullptr },// LS_ELECIMP1
	{ sfx_MISC,                  "sfx\\misc\\elementl.wav",     nullptr },// LS_ELEMENTL
	{ sfx_MISC,                  "sfx\\misc\\ethereal.wav",     nullptr },// LS_ETHEREAL
	{ sfx_MISC,                  "sfx\\misc\\fball.wav",        nullptr },// LS_FBALL
	{ sfx_MISC,                  "sfx\\misc\\fbolt1.wav",       nullptr },// LS_FBOLT1
	{ sfx_MISC,                  "sfx\\misc\\fbolt2.wav",       nullptr },// LS_FBOLT2
	{ sfx_MISC,                  "sfx\\misc\\firimp1.wav",      nullptr },// LS_FIRIMP1
	{ sfx_MISC,                  "sfx\\misc\\firimp2.wav",      nullptr },// LS_FIRIMP2
	{ sfx_MISC,                  "sfx\\misc\\flamwave.wav",     nullptr },// LS_FLAMWAVE
	{ sfx_MISC,                  "sfx\\misc\\flash.wav",        nullptr },// LS_FLASH
	{ sfx_MISC,                  "sfx\\misc\\fountain.wav",     nullptr },// LS_FOUNTAIN
	{ sfx_MISC,                  "sfx\\misc\\golum.wav",        nullptr },// LS_GOLUM
	{ sfx_MISC,                  "sfx\\misc\\golumded.wav",     nullptr },// LS_GOLUMDED
	{ sfx_MISC,                  "sfx\\misc\\gshrine.wav",      nullptr },// LS_GSHRINE
	{ sfx_MISC,                  "sfx\\misc\\guard.wav",        nullptr },// LS_GUARD
	{ sfx_MISC,                  "sfx\\misc\\grdlanch.wav",     nullptr },// LS_GUARDLAN
	{ sfx_MISC,                  "sfx\\misc\\holybolt.wav",     nullptr },// LS_HOLYBOLT
	{ sfx_MISC,                  "sfx\\misc\\hyper.wav",        nullptr },// LS_HYPER
	{ sfx_MISC,                  "sfx\\misc\\infravis.wav",     nullptr },// LS_INFRAVIS
	{ sfx_MISC,                  "sfx\\misc\\invisibl.wav",     nullptr },// LS_INVISIBL
	{ sfx_MISC,                  "sfx\\misc\\invpot.wav",       nullptr },// LS_INVPOT
	{ sfx_MISC,                  "sfx\\misc\\lning1.wav",       nullptr },// LS_LNING1
	{ sfx_MISC,                  "sfx\\misc\\ltning.wav",       nullptr },// LS_LTNING
	{ sfx_MISC,                  "sfx\\misc\\mshield.wav",      nullptr },// LS_MSHIELD
	{ sfx_MISC | sfx_HELLFIRE,   "sfx\\misc\\nestxpld.wav",     nullptr },// LS_NESTXPLD
	{ sfx_MISC,                  "sfx\\misc\\nova.wav",         nullptr },// LS_NOVA
	{ sfx_MISC,                  "sfx\\misc\\portal.wav",       nullptr },// LS_PORTAL
	{ sfx_MISC,                  "sfx\\misc\\puddle.wav",       nullptr },// LS_PUDDLE
	{ sfx_MISC,                  "sfx\\misc\\resur.wav",        nullptr },// LS_RESUR
	{ sfx_MISC,                  "sfx\\misc\\scurse.wav",       nullptr },// LS_SCURSE
	{ sfx_MISC,                  "sfx\\misc\\scurimp.wav",      nullptr },// LS_SCURIMP
	{ sfx_MISC,                  "sfx\\misc\\sentinel.wav",     nullptr },// LS_SENTINEL
	{ sfx_MISC,                  "sfx\\misc\\shatter.wav",      nullptr },// LS_SHATTER
	{ sfx_MISC,                  "sfx\\misc\\soulfire.wav",     nullptr },// LS_SOULFIRE
	{ sfx_MISC,                  "sfx\\misc\\spoutlop.wav",     nullptr },// LS_SPOUTLOP
	{ sfx_MISC,                  "sfx\\misc\\spoutstr.wav",     nullptr },// LS_SPOUTSTR
	{ sfx_MISC,                  "sfx\\misc\\storm.wav",        nullptr },// LS_STORM
	{ sfx_MISC,                  "sfx\\misc\\trapdis.wav",      nullptr },// LS_TRAPDIS
	{ sfx_MISC,                  "sfx\\misc\\teleport.wav",     nullptr },// LS_TELEPORT
	{ sfx_MISC,                  "sfx\\misc\\vtheft.wav",       nullptr },// LS_VTHEFT
	{ sfx_MISC,                  "sfx\\misc\\wallloop.wav",     nullptr },// LS_WALLLOOP
	{ sfx_MISC,                  "sfx\\misc\\wallstrt.wav",     nullptr },// LS_WALLSTRT
	{ sfx_MISC | sfx_HELLFIRE,   "sfx\\misc\\lmag.wav",         nullptr },// LS_LMAG
	{ sfx_STREAM,                "sfx\\towners\\bmaid01.wav",   nullptr },// TSFX_BMAID1
	{ sfx_STREAM,                "sfx\\towners\\bmaid02.wav",   nullptr },// TSFX_BMAID2
	{ sfx_STREAM,                "sfx\\towners\\bmaid03.wav",   nullptr },// TSFX_BMAID3
	{ sfx_STREAM,                "sfx\\towners\\bmaid04.wav",   nullptr },// TSFX_BMAID4
	{ sfx_STREAM,                "sfx\\towners\\bmaid05.wav",   nullptr },// TSFX_BMAID5
	{ sfx_STREAM,                "sfx\\towners\\bmaid06.wav",   nullptr },// TSFX_BMAID6
	{ sfx_STREAM,                "sfx\\towners\\bmaid07.wav",   nullptr },// TSFX_BMAID7
	{ sfx_STREAM,                "sfx\\towners\\bmaid08.wav",   nullptr },// TSFX_BMAID8
	{ sfx_STREAM,                "sfx\\towners\\bmaid09.wav",   nullptr },// TSFX_BMAID9
	{ sfx_STREAM,                "sfx\\towners\\bmaid10.wav",   nullptr },// TSFX_BMAID10
	{ sfx_STREAM,                "sfx\\towners\\bmaid11.wav",   nullptr },// TSFX_BMAID11
	{ sfx_STREAM,                "sfx\\towners\\bmaid12.wav",   nullptr },// TSFX_BMAID12
	{ sfx_STREAM,                "sfx\\towners\\bmaid13.wav",   nullptr },// TSFX_BMAID13
	{ sfx_STREAM,                "sfx\\towners\\bmaid14.wav",   nullptr },// TSFX_BMAID14
	{ sfx_STREAM,                "sfx\\towners\\bmaid15.wav",   nullptr },// TSFX_BMAID15
	{ sfx_STREAM,                "sfx\\towners\\bmaid16.wav",   nullptr },// TSFX_BMAID16
	{ sfx_STREAM,                "sfx\\towners\\bmaid17.wav",   nullptr },// TSFX_BMAID17
	{ sfx_STREAM,                "sfx\\towners\\bmaid18.wav",   nullptr },// TSFX_BMAID18
	{ sfx_STREAM,                "sfx\\towners\\bmaid19.wav",   nullptr },// TSFX_BMAID19
	{ sfx_STREAM,                "sfx\\towners\\bmaid20.wav",   nullptr },// TSFX_BMAID20
	{ sfx_STREAM,                "sfx\\towners\\bmaid21.wav",   nullptr },// TSFX_BMAID21
	{ sfx_STREAM,                "sfx\\towners\\bmaid22.wav",   nullptr },// TSFX_BMAID22
	{ sfx_STREAM,                "sfx\\towners\\bmaid23.wav",   nullptr },// TSFX_BMAID23
	{ sfx_STREAM,                "sfx\\towners\\bmaid24.wav",   nullptr },// TSFX_BMAID24
	{ sfx_STREAM,                "sfx\\towners\\bmaid25.wav",   nullptr },// TSFX_BMAID25
	{ sfx_STREAM,                "sfx\\towners\\bmaid26.wav",   nullptr },// TSFX_BMAID26
	{ sfx_STREAM,                "sfx\\towners\\bmaid27.wav",   nullptr },// TSFX_BMAID27
	{ sfx_STREAM,                "sfx\\towners\\bmaid28.wav",   nullptr },// TSFX_BMAID28
	{ sfx_STREAM,                "sfx\\towners\\bmaid29.wav",   nullptr },// TSFX_BMAID29
	{ sfx_STREAM,                "sfx\\towners\\bmaid30.wav",   nullptr },// TSFX_BMAID30
	{ sfx_STREAM,                "sfx\\towners\\bmaid31.wav",   nullptr },// TSFX_BMAID31
	{ sfx_STREAM,                "sfx\\towners\\bmaid32.wav",   nullptr },// TSFX_BMAID32
	{ sfx_STREAM,                "sfx\\towners\\bmaid33.wav",   nullptr },// TSFX_BMAID33
	{ sfx_STREAM,                "sfx\\towners\\bmaid34.wav",   nullptr },// TSFX_BMAID34
	{ sfx_STREAM,                "sfx\\towners\\bmaid35.wav",   nullptr },// TSFX_BMAID35
	{ sfx_STREAM,                "sfx\\towners\\bmaid36.wav",   nullptr },// TSFX_BMAID36
	{ sfx_STREAM,                "sfx\\towners\\bmaid37.wav",   nullptr },// TSFX_BMAID37
	{ sfx_STREAM,                "sfx\\towners\\bmaid38.wav",   nullptr },// TSFX_BMAID38
	{ sfx_STREAM,                "sfx\\towners\\bmaid39.wav",   nullptr },// TSFX_BMAID39
	{ sfx_STREAM,                "sfx\\towners\\bmaid40.wav",   nullptr },// TSFX_BMAID40
	{ sfx_STREAM,                "sfx\\towners\\bsmith01.wav",  nullptr },// TSFX_SMITH1
	{ sfx_STREAM,                "sfx\\towners\\bsmith02.wav",  nullptr },// TSFX_SMITH2
	{ sfx_STREAM,                "sfx\\towners\\bsmith03.wav",  nullptr },// TSFX_SMITH3
	{ sfx_STREAM,                "sfx\\towners\\bsmith04.wav",  nullptr },// TSFX_SMITH4
	{ sfx_STREAM,                "sfx\\towners\\bsmith05.wav",  nullptr },// TSFX_SMITH5
	{ sfx_STREAM,                "sfx\\towners\\bsmith06.wav",  nullptr },// TSFX_SMITH6
	{ sfx_STREAM,                "sfx\\towners\\bsmith07.wav",  nullptr },// TSFX_SMITH7
	{ sfx_STREAM,                "sfx\\towners\\bsmith08.wav",  nullptr },// TSFX_SMITH8
	{ sfx_STREAM,                "sfx\\towners\\bsmith09.wav",  nullptr },// TSFX_SMITH9
	{ sfx_STREAM,                "sfx\\towners\\bsmith10.wav",  nullptr },// TSFX_SMITH10
	{ sfx_STREAM,                "sfx\\towners\\bsmith11.wav",  nullptr },// TSFX_SMITH11
	{ sfx_STREAM,                "sfx\\towners\\bsmith12.wav",  nullptr },// TSFX_SMITH12
	{ sfx_STREAM,                "sfx\\towners\\bsmith13.wav",  nullptr },// TSFX_SMITH13
	{ sfx_STREAM,                "sfx\\towners\\bsmith14.wav",  nullptr },// TSFX_SMITH14
	{ sfx_STREAM,                "sfx\\towners\\bsmith15.wav",  nullptr },// TSFX_SMITH15
	{ sfx_STREAM,                "sfx\\towners\\bsmith16.wav",  nullptr },// TSFX_SMITH16
	{ sfx_STREAM,                "sfx\\towners\\bsmith17.wav",  nullptr },// TSFX_SMITH17
	{ sfx_STREAM,                "sfx\\towners\\bsmith18.wav",  nullptr },// TSFX_SMITH18
	{ sfx_STREAM,                "sfx\\towners\\bsmith19.wav",  nullptr },// TSFX_SMITH19
	{ sfx_STREAM,                "sfx\\towners\\bsmith20.wav",  nullptr },// TSFX_SMITH20
	{ sfx_STREAM,                "sfx\\towners\\bsmith21.wav",  nullptr },// TSFX_SMITH21
	{ sfx_STREAM,                "sfx\\towners\\bsmith22.wav",  nullptr },// TSFX_SMITH22
	{ sfx_STREAM,                "sfx\\towners\\bsmith23.wav",  nullptr },// TSFX_SMITH23
	{ sfx_STREAM,                "sfx\\towners\\bsmith24.wav",  nullptr },// TSFX_SMITH24
	{ sfx_STREAM,                "sfx\\towners\\bsmith25.wav",  nullptr },// TSFX_SMITH25
	{ sfx_STREAM,                "sfx\\towners\\bsmith26.wav",  nullptr },// TSFX_SMITH26
	{ sfx_STREAM,                "sfx\\towners\\bsmith27.wav",  nullptr },// TSFX_SMITH27
	{ sfx_STREAM,                "sfx\\towners\\bsmith28.wav",  nullptr },// TSFX_SMITH28
	{ sfx_STREAM,                "sfx\\towners\\bsmith29.wav",  nullptr },// TSFX_SMITH29
	{ sfx_STREAM,                "sfx\\towners\\bsmith30.wav",  nullptr },// TSFX_SMITH30
	{ sfx_STREAM,                "sfx\\towners\\bsmith31.wav",  nullptr },// TSFX_SMITH31
	{ sfx_STREAM,                "sfx\\towners\\bsmith32.wav",  nullptr },// TSFX_SMITH32
	{ sfx_STREAM,                "sfx\\towners\\bsmith33.wav",  nullptr },// TSFX_SMITH33
	{ sfx_STREAM,                "sfx\\towners\\bsmith34.wav",  nullptr },// TSFX_SMITH34
	{ sfx_STREAM,                "sfx\\towners\\bsmith35.wav",  nullptr },// TSFX_SMITH35
	{ sfx_STREAM,                "sfx\\towners\\bsmith36.wav",  nullptr },// TSFX_SMITH36
	{ sfx_STREAM,                "sfx\\towners\\bsmith37.wav",  nullptr },// TSFX_SMITH37
	{ sfx_STREAM,                "sfx\\towners\\bsmith38.wav",  nullptr },// TSFX_SMITH38
	{ sfx_STREAM,                "sfx\\towners\\bsmith39.wav",  nullptr },// TSFX_SMITH39
	{ sfx_STREAM,                "sfx\\towners\\bsmith40.wav",  nullptr },// TSFX_SMITH40
	{ sfx_STREAM,                "sfx\\towners\\bsmith41.wav",  nullptr },// TSFX_SMITH41
	{ sfx_STREAM,                "sfx\\towners\\bsmith42.wav",  nullptr },// TSFX_SMITH42
	{ sfx_STREAM,                "sfx\\towners\\bsmith43.wav",  nullptr },// TSFX_SMITH43
	{ sfx_STREAM,                "sfx\\towners\\bsmith44.wav",  nullptr },// TSFX_SMITH44
	{ sfx_STREAM,                "sfx\\towners\\bsmith45.wav",  nullptr },// TSFX_SMITH45
	{ sfx_STREAM,                "sfx\\towners\\bsmith46.wav",  nullptr },// TSFX_SMITH46
	{ sfx_STREAM,                "sfx\\towners\\bsmith47.wav",  nullptr },// TSFX_SMITH47
	{ sfx_STREAM,                "sfx\\towners\\bsmith48.wav",  nullptr },// TSFX_SMITH48
	{ sfx_STREAM,                "sfx\\towners\\bsmith49.wav",  nullptr },// TSFX_SMITH49
	{ sfx_STREAM,                "sfx\\towners\\bsmith50.wav",  nullptr },// TSFX_SMITH50
	{ sfx_STREAM,                "sfx\\towners\\bsmith51.wav",  nullptr },// TSFX_SMITH51
	{ sfx_STREAM,                "sfx\\towners\\bsmith52.wav",  nullptr },// TSFX_SMITH52
	{ sfx_STREAM,                "sfx\\towners\\bsmith53.wav",  nullptr },// TSFX_SMITH53
	{ sfx_STREAM,                "sfx\\towners\\bsmith54.wav",  nullptr },// TSFX_SMITH54
	{ sfx_STREAM,                "sfx\\towners\\bsmith55.wav",  nullptr },// TSFX_SMITH55
	{ sfx_STREAM,                "sfx\\towners\\bsmith56.wav",  nullptr },// TSFX_SMITH56
	{ sfx_MISC,                  "sfx\\towners\\cow1.wav",      nullptr },// TSFX_COW1
	{ sfx_MISC,                  "sfx\\towners\\cow2.wav",      nullptr },// TSFX_COW2
/*
	{ sfx_MISC,                  "sfx\\towners\\cow3.wav",      nullptr },// TSFX_COW3
	{ sfx_MISC,                  "sfx\\towners\\cow4.wav",      nullptr },// TSFX_COW4
	{ sfx_MISC,                  "sfx\\towners\\cow5.wav",      nullptr },// TSFX_COW5
	{ sfx_MISC,                  "sfx\\towners\\cow6.wav",      nullptr },// TSFX_COW6
*/
	{ sfx_MISC | sfx_HELLFIRE,   "sfx\\towners\\cow7.wav",      nullptr },// TSFX_COW7
	{ sfx_MISC | sfx_HELLFIRE,   "sfx\\towners\\cow8.wav",      nullptr },// TSFX_COW8
	{ sfx_STREAM,                "sfx\\towners\\deadguy2.wav",  nullptr },// TSFX_DEADGUY
	{ sfx_STREAM,                "sfx\\towners\\drunk01.wav",   nullptr },// TSFX_DRUNK1
	{ sfx_STREAM,                "sfx\\towners\\drunk02.wav",   nullptr },// TSFX_DRUNK2
	{ sfx_STREAM,                "sfx\\towners\\drunk03.wav",   nullptr },// TSFX_DRUNK3
	{ sfx_STREAM,                "sfx\\towners\\drunk04.wav",   nullptr },// TSFX_DRUNK4
	{ sfx_STREAM,                "sfx\\towners\\drunk05.wav",   nullptr },// TSFX_DRUNK5
	{ sfx_STREAM,                "sfx\\towners\\drunk06.wav",   nullptr },// TSFX_DRUNK6
	{ sfx_STREAM,                "sfx\\towners\\drunk07.wav",   nullptr },// TSFX_DRUNK7
	{ sfx_STREAM,                "sfx\\towners\\drunk08.wav",   nullptr },// TSFX_DRUNK8
	{ sfx_STREAM,                "sfx\\towners\\drunk09.wav",   nullptr },// TSFX_DRUNK9
	{ sfx_STREAM,                "sfx\\towners\\drunk10.wav",   nullptr },// TSFX_DRUNK10
	{ sfx_STREAM,                "sfx\\towners\\drunk11.wav",   nullptr },// TSFX_DRUNK11
	{ sfx_STREAM,                "sfx\\towners\\drunk12.wav",   nullptr },// TSFX_DRUNK12
	{ sfx_STREAM,                "sfx\\towners\\drunk13.wav",   nullptr },// TSFX_DRUNK13
	{ sfx_STREAM,                "sfx\\towners\\drunk14.wav",   nullptr },// TSFX_DRUNK14
	{ sfx_STREAM,                "sfx\\towners\\drunk15.wav",   nullptr },// TSFX_DRUNK15
	{ sfx_STREAM,                "sfx\\towners\\drunk16.wav",   nullptr },// TSFX_DRUNK16
	{ sfx_STREAM,                "sfx\\towners\\drunk17.wav",   nullptr },// TSFX_DRUNK17
	{ sfx_STREAM,                "sfx\\towners\\drunk18.wav",   nullptr },// TSFX_DRUNK18
	{ sfx_STREAM,                "sfx\\towners\\drunk19.wav",   nullptr },// TSFX_DRUNK19
	{ sfx_STREAM,                "sfx\\towners\\drunk20.wav",   nullptr },// TSFX_DRUNK20
	{ sfx_STREAM,                "sfx\\towners\\drunk21.wav",   nullptr },// TSFX_DRUNK21
	{ sfx_STREAM,                "sfx\\towners\\drunk22.wav",   nullptr },// TSFX_DRUNK22
	{ sfx_STREAM,                "sfx\\towners\\drunk23.wav",   nullptr },// TSFX_DRUNK23
	{ sfx_STREAM,                "sfx\\towners\\drunk24.wav",   nullptr },// TSFX_DRUNK24
	{ sfx_STREAM,                "sfx\\towners\\drunk25.wav",   nullptr },// TSFX_DRUNK25
	{ sfx_STREAM,                "sfx\\towners\\drunk26.wav",   nullptr },// TSFX_DRUNK26
	{ sfx_STREAM,                "sfx\\towners\\drunk27.wav",   nullptr },// TSFX_DRUNK27
	{ sfx_STREAM,                "sfx\\towners\\drunk28.wav",   nullptr },// TSFX_DRUNK28
	{ sfx_STREAM,                "sfx\\towners\\drunk29.wav",   nullptr },// TSFX_DRUNK29
	{ sfx_STREAM,                "sfx\\towners\\drunk30.wav",   nullptr },// TSFX_DRUNK30
	{ sfx_STREAM,                "sfx\\towners\\drunk31.wav",   nullptr },// TSFX_DRUNK31
	{ sfx_STREAM,                "sfx\\towners\\drunk32.wav",   nullptr },// TSFX_DRUNK32
	{ sfx_STREAM,                "sfx\\towners\\drunk33.wav",   nullptr },// TSFX_DRUNK33
	{ sfx_STREAM,                "sfx\\towners\\drunk34.wav",   nullptr },// TSFX_DRUNK34
	{ sfx_STREAM,                "sfx\\towners\\drunk35.wav",   nullptr },// TSFX_DRUNK35
	{ sfx_STREAM,                "sfx\\towners\\healer01.wav",  nullptr },// TSFX_HEALER1
	{ sfx_STREAM,                "sfx\\towners\\healer02.wav",  nullptr },// TSFX_HEALER2
	{ sfx_STREAM,                "sfx\\towners\\healer03.wav",  nullptr },// TSFX_HEALER3
	{ sfx_STREAM,                "sfx\\towners\\healer04.wav",  nullptr },// TSFX_HEALER4
	{ sfx_STREAM,                "sfx\\towners\\healer05.wav",  nullptr },// TSFX_HEALER5
	{ sfx_STREAM,                "sfx\\towners\\healer06.wav",  nullptr },// TSFX_HEALER6
	{ sfx_STREAM,                "sfx\\towners\\healer07.wav",  nullptr },// TSFX_HEALER7
	{ sfx_STREAM,                "sfx\\towners\\healer08.wav",  nullptr },// TSFX_HEALER8
	{ sfx_STREAM,                "sfx\\towners\\healer09.wav",  nullptr },// TSFX_HEALER9
	{ sfx_STREAM,                "sfx\\towners\\healer10.wav",  nullptr },// TSFX_HEALER10
	{ sfx_STREAM,                "sfx\\towners\\healer11.wav",  nullptr },// TSFX_HEALER11
	{ sfx_STREAM,                "sfx\\towners\\healer12.wav",  nullptr },// TSFX_HEALER12
	{ sfx_STREAM,                "sfx\\towners\\healer13.wav",  nullptr },// TSFX_HEALER13
	{ sfx_STREAM,                "sfx\\towners\\healer14.wav",  nullptr },// TSFX_HEALER14
	{ sfx_STREAM,                "sfx\\towners\\healer15.wav",  nullptr },// TSFX_HEALER15
	{ sfx_STREAM,                "sfx\\towners\\healer16.wav",  nullptr },// TSFX_HEALER16
	{ sfx_STREAM,                "sfx\\towners\\healer17.wav",  nullptr },// TSFX_HEALER17
	{ sfx_STREAM,                "sfx\\towners\\healer18.wav",  nullptr },// TSFX_HEALER18
	{ sfx_STREAM,                "sfx\\towners\\healer19.wav",  nullptr },// TSFX_HEALER19
	{ sfx_STREAM,                "sfx\\towners\\healer20.wav",  nullptr },// TSFX_HEALER20
	{ sfx_STREAM,                "sfx\\towners\\healer21.wav",  nullptr },// TSFX_HEALER21
	{ sfx_STREAM,                "sfx\\towners\\healer22.wav",  nullptr },// TSFX_HEALER22
	{ sfx_STREAM,                "sfx\\towners\\healer23.wav",  nullptr },// TSFX_HEALER23
	{ sfx_STREAM,                "sfx\\towners\\healer24.wav",  nullptr },// TSFX_HEALER24
	{ sfx_STREAM,                "sfx\\towners\\healer25.wav",  nullptr },// TSFX_HEALER25
	{ sfx_STREAM,                "sfx\\towners\\healer26.wav",  nullptr },// TSFX_HEALER26
	{ sfx_STREAM,                "sfx\\towners\\healer27.wav",  nullptr },// TSFX_HEALER27
	{ sfx_STREAM,                "sfx\\towners\\healer28.wav",  nullptr },// TSFX_HEALER28
	{ sfx_STREAM,                "sfx\\towners\\healer29.wav",  nullptr },// TSFX_HEALER29
	{ sfx_STREAM,                "sfx\\towners\\healer30.wav",  nullptr },// TSFX_HEALER30
	{ sfx_STREAM,                "sfx\\towners\\healer31.wav",  nullptr },// TSFX_HEALER31
	{ sfx_STREAM,                "sfx\\towners\\healer32.wav",  nullptr },// TSFX_HEALER32
	{ sfx_STREAM,                "sfx\\towners\\healer33.wav",  nullptr },// TSFX_HEALER33
	{ sfx_STREAM,                "sfx\\towners\\healer34.wav",  nullptr },// TSFX_HEALER34
	{ sfx_STREAM,                "sfx\\towners\\healer35.wav",  nullptr },// TSFX_HEALER35
	{ sfx_STREAM,                "sfx\\towners\\healer36.wav",  nullptr },// TSFX_HEALER36
	{ sfx_STREAM,                "sfx\\towners\\healer37.wav",  nullptr },// TSFX_HEALER37
	{ sfx_STREAM,                "sfx\\towners\\healer38.wav",  nullptr },// TSFX_HEALER38
	{ sfx_STREAM,                "sfx\\towners\\healer39.wav",  nullptr },// TSFX_HEALER39
	{ sfx_STREAM,                "sfx\\towners\\healer40.wav",  nullptr },// TSFX_HEALER40
	{ sfx_STREAM,                "sfx\\towners\\healer41.wav",  nullptr },// TSFX_HEALER41
	{ sfx_STREAM,                "sfx\\towners\\healer42.wav",  nullptr },// TSFX_HEALER42
	{ sfx_STREAM,                "sfx\\towners\\healer43.wav",  nullptr },// TSFX_HEALER43
	{ sfx_STREAM,                "sfx\\towners\\healer44.wav",  nullptr },// TSFX_HEALER44
	{ sfx_STREAM,                "sfx\\towners\\healer45.wav",  nullptr },// TSFX_HEALER45
	{ sfx_STREAM,                "sfx\\towners\\healer46.wav",  nullptr },// TSFX_HEALER46
	{ sfx_STREAM,                "sfx\\towners\\healer47.wav",  nullptr },// TSFX_HEALER47
	{ sfx_STREAM,                "sfx\\towners\\pegboy01.wav",  nullptr },// TSFX_PEGBOY1
	{ sfx_STREAM,                "sfx\\towners\\pegboy02.wav",  nullptr },// TSFX_PEGBOY2
	{ sfx_STREAM,                "sfx\\towners\\pegboy03.wav",  nullptr },// TSFX_PEGBOY3
	{ sfx_STREAM,                "sfx\\towners\\pegboy04.wav",  nullptr },// TSFX_PEGBOY4
	{ sfx_STREAM,                "sfx\\towners\\pegboy05.wav",  nullptr },// TSFX_PEGBOY5
	{ sfx_STREAM,                "sfx\\towners\\pegboy06.wav",  nullptr },// TSFX_PEGBOY6
	{ sfx_STREAM,                "sfx\\towners\\pegboy07.wav",  nullptr },// TSFX_PEGBOY7
	{ sfx_STREAM,                "sfx\\towners\\pegboy08.wav",  nullptr },// TSFX_PEGBOY8
	{ sfx_STREAM,                "sfx\\towners\\pegboy09.wav",  nullptr },// TSFX_PEGBOY9
	{ sfx_STREAM,                "sfx\\towners\\pegboy10.wav",  nullptr },// TSFX_PEGBOY10
	{ sfx_STREAM,                "sfx\\towners\\pegboy11.wav",  nullptr },// TSFX_PEGBOY11
	{ sfx_STREAM,                "sfx\\towners\\pegboy12.wav",  nullptr },// TSFX_PEGBOY12
	{ sfx_STREAM,                "sfx\\towners\\pegboy13.wav",  nullptr },// TSFX_PEGBOY13
	{ sfx_STREAM,                "sfx\\towners\\pegboy14.wav",  nullptr },// TSFX_PEGBOY14
	{ sfx_STREAM,                "sfx\\towners\\pegboy15.wav",  nullptr },// TSFX_PEGBOY15
	{ sfx_STREAM,                "sfx\\towners\\pegboy16.wav",  nullptr },// TSFX_PEGBOY16
	{ sfx_STREAM,                "sfx\\towners\\pegboy17.wav",  nullptr },// TSFX_PEGBOY17
	{ sfx_STREAM,                "sfx\\towners\\pegboy18.wav",  nullptr },// TSFX_PEGBOY18
	{ sfx_STREAM,                "sfx\\towners\\pegboy19.wav",  nullptr },// TSFX_PEGBOY19
	{ sfx_STREAM,                "sfx\\towners\\pegboy20.wav",  nullptr },// TSFX_PEGBOY20
	{ sfx_STREAM,                "sfx\\towners\\pegboy21.wav",  nullptr },// TSFX_PEGBOY21
	{ sfx_STREAM,                "sfx\\towners\\pegboy22.wav",  nullptr },// TSFX_PEGBOY22
	{ sfx_STREAM,                "sfx\\towners\\pegboy23.wav",  nullptr },// TSFX_PEGBOY23
	{ sfx_STREAM,                "sfx\\towners\\pegboy24.wav",  nullptr },// TSFX_PEGBOY24
	{ sfx_STREAM,                "sfx\\towners\\pegboy25.wav",  nullptr },// TSFX_PEGBOY25
	{ sfx_STREAM,                "sfx\\towners\\pegboy26.wav",  nullptr },// TSFX_PEGBOY26
	{ sfx_STREAM,                "sfx\\towners\\pegboy27.wav",  nullptr },// TSFX_PEGBOY27
	{ sfx_STREAM,                "sfx\\towners\\pegboy28.wav",  nullptr },// TSFX_PEGBOY28
	{ sfx_STREAM,                "sfx\\towners\\pegboy29.wav",  nullptr },// TSFX_PEGBOY29
	{ sfx_STREAM,                "sfx\\towners\\pegboy30.wav",  nullptr },// TSFX_PEGBOY30
	{ sfx_STREAM,                "sfx\\towners\\pegboy31.wav",  nullptr },// TSFX_PEGBOY31
	{ sfx_STREAM,                "sfx\\towners\\pegboy32.wav",  nullptr },// TSFX_PEGBOY32
	{ sfx_STREAM,                "sfx\\towners\\pegboy33.wav",  nullptr },// TSFX_PEGBOY33
	{ sfx_STREAM,                "sfx\\towners\\pegboy34.wav",  nullptr },// TSFX_PEGBOY34
	{ sfx_STREAM,                "sfx\\towners\\pegboy35.wav",  nullptr },// TSFX_PEGBOY35
	{ sfx_STREAM,                "sfx\\towners\\pegboy36.wav",  nullptr },// TSFX_PEGBOY36
	{ sfx_STREAM,                "sfx\\towners\\pegboy37.wav",  nullptr },// TSFX_PEGBOY37
	{ sfx_STREAM,                "sfx\\towners\\pegboy38.wav",  nullptr },// TSFX_PEGBOY38
	{ sfx_STREAM,                "sfx\\towners\\pegboy39.wav",  nullptr },// TSFX_PEGBOY39
	{ sfx_STREAM,                "sfx\\towners\\pegboy40.wav",  nullptr },// TSFX_PEGBOY40
	{ sfx_STREAM,                "sfx\\towners\\pegboy41.wav",  nullptr },// TSFX_PEGBOY41
	{ sfx_STREAM,                "sfx\\towners\\pegboy42.wav",  nullptr },// TSFX_PEGBOY42
	{ sfx_STREAM,                "sfx\\towners\\pegboy43.wav",  nullptr },// TSFX_PEGBOY43
	{ sfx_STREAM,                "sfx\\towners\\priest00.wav",  nullptr },// TSFX_PRIEST0
	{ sfx_STREAM,                "sfx\\towners\\priest01.wav",  nullptr },// TSFX_PRIEST1
	{ sfx_STREAM,                "sfx\\towners\\priest02.wav",  nullptr },// TSFX_PRIEST2
	{ sfx_STREAM,                "sfx\\towners\\priest03.wav",  nullptr },// TSFX_PRIEST3
	{ sfx_STREAM,                "sfx\\towners\\priest04.wav",  nullptr },// TSFX_PRIEST4
	{ sfx_STREAM,                "sfx\\towners\\priest05.wav",  nullptr },// TSFX_PRIEST5
	{ sfx_STREAM,                "sfx\\towners\\priest06.wav",  nullptr },// TSFX_PRIEST6
	{ sfx_STREAM,                "sfx\\towners\\priest07.wav",  nullptr },// TSFX_PRIEST7
	{ sfx_STREAM,                "sfx\\towners\\storyt00.wav",  nullptr },// TSFX_STORY0
	{ sfx_STREAM,                "sfx\\towners\\storyt01.wav",  nullptr },// TSFX_STORY1
	{ sfx_STREAM,                "sfx\\towners\\storyt02.wav",  nullptr },// TSFX_STORY2
	{ sfx_STREAM,                "sfx\\towners\\storyt03.wav",  nullptr },// TSFX_STORY3
	{ sfx_STREAM,                "sfx\\towners\\storyt04.wav",  nullptr },// TSFX_STORY4
	{ sfx_STREAM,                "sfx\\towners\\storyt05.wav",  nullptr },// TSFX_STORY5
	{ sfx_STREAM,                "sfx\\towners\\storyt06.wav",  nullptr },// TSFX_STORY6
	{ sfx_STREAM,                "sfx\\towners\\storyt07.wav",  nullptr },// TSFX_STORY7
	{ sfx_STREAM,                "sfx\\towners\\storyt08.wav",  nullptr },// TSFX_STORY8
	{ sfx_STREAM,                "sfx\\towners\\storyt09.wav",  nullptr },// TSFX_STORY9
	{ sfx_STREAM,                "sfx\\towners\\storyt10.wav",  nullptr },// TSFX_STORY10
	{ sfx_STREAM,                "sfx\\towners\\storyt11.wav",  nullptr },// TSFX_STORY11
	{ sfx_STREAM,                "sfx\\towners\\storyt12.wav",  nullptr },// TSFX_STORY12
	{ sfx_STREAM,                "sfx\\towners\\storyt13.wav",  nullptr },// TSFX_STORY13
	{ sfx_STREAM,                "sfx\\towners\\storyt14.wav",  nullptr },// TSFX_STORY14
	{ sfx_STREAM,                "sfx\\towners\\storyt15.wav",  nullptr },// TSFX_STORY15
	{ sfx_STREAM,                "sfx\\towners\\storyt16.wav",  nullptr },// TSFX_STORY16
	{ sfx_STREAM,                "sfx\\towners\\storyt17.wav",  nullptr },// TSFX_STORY17
	{ sfx_STREAM,                "sfx\\towners\\storyt18.wav",  nullptr },// TSFX_STORY18
	{ sfx_STREAM,                "sfx\\towners\\storyt19.wav",  nullptr },// TSFX_STORY19
	{ sfx_STREAM,                "sfx\\towners\\storyt20.wav",  nullptr },// TSFX_STORY20
	{ sfx_STREAM,                "sfx\\towners\\storyt21.wav",  nullptr },// TSFX_STORY21
	{ sfx_STREAM,                "sfx\\towners\\storyt22.wav",  nullptr },// TSFX_STORY22
	{ sfx_STREAM,                "sfx\\towners\\storyt23.wav",  nullptr },// TSFX_STORY23
	{ sfx_STREAM,                "sfx\\towners\\storyt24.wav",  nullptr },// TSFX_STORY24
	{ sfx_STREAM,                "sfx\\towners\\storyt25.wav",  nullptr },// TSFX_STORY25
	{ sfx_STREAM,                "sfx\\towners\\storyt26.wav",  nullptr },// TSFX_STORY26
	{ sfx_STREAM,                "sfx\\towners\\storyt27.wav",  nullptr },// TSFX_STORY27
	{ sfx_STREAM,                "sfx\\towners\\storyt28.wav",  nullptr },// TSFX_STORY28
	{ sfx_STREAM,                "sfx\\towners\\storyt29.wav",  nullptr },// TSFX_STORY29
	{ sfx_STREAM,                "sfx\\towners\\storyt30.wav",  nullptr },// TSFX_STORY30
	{ sfx_STREAM,                "sfx\\towners\\storyt31.wav",  nullptr },// TSFX_STORY31
	{ sfx_STREAM,                "sfx\\towners\\storyt32.wav",  nullptr },// TSFX_STORY32
	{ sfx_STREAM,                "sfx\\towners\\storyt33.wav",  nullptr },// TSFX_STORY33
	{ sfx_STREAM,                "sfx\\towners\\storyt34.wav",  nullptr },// TSFX_STORY34
	{ sfx_STREAM,                "sfx\\towners\\storyt35.wav",  nullptr },// TSFX_STORY35
	{ sfx_STREAM,                "sfx\\towners\\storyt36.wav",  nullptr },// TSFX_STORY36
	{ sfx_STREAM,                "sfx\\towners\\storyt37.wav",  nullptr },// TSFX_STORY37
	{ sfx_STREAM,                "sfx\\towners\\storyt38.wav",  nullptr },// TSFX_STORY38
	{ sfx_STREAM,                "sfx\\towners\\tavown00.wav",  nullptr },// TSFX_TAVERN0
	{ sfx_STREAM,                "sfx\\towners\\tavown01.wav",  nullptr },// TSFX_TAVERN1
	{ sfx_STREAM,                "sfx\\towners\\tavown02.wav",  nullptr },// TSFX_TAVERN2
	{ sfx_STREAM,                "sfx\\towners\\tavown03.wav",  nullptr },// TSFX_TAVERN3
	{ sfx_STREAM,                "sfx\\towners\\tavown04.wav",  nullptr },// TSFX_TAVERN4
	{ sfx_STREAM,                "sfx\\towners\\tavown05.wav",  nullptr },// TSFX_TAVERN5
	{ sfx_STREAM,                "sfx\\towners\\tavown06.wav",  nullptr },// TSFX_TAVERN6
	{ sfx_STREAM,                "sfx\\towners\\tavown07.wav",  nullptr },// TSFX_TAVERN7
	{ sfx_STREAM,                "sfx\\towners\\tavown08.wav",  nullptr },// TSFX_TAVERN8
	{ sfx_STREAM,                "sfx\\towners\\tavown09.wav",  nullptr },// TSFX_TAVERN9
	{ sfx_STREAM,                "sfx\\towners\\tavown10.wav",  nullptr },// TSFX_TAVERN10
	{ sfx_STREAM,                "sfx\\towners\\tavown11.wav",  nullptr },// TSFX_TAVERN11
	{ sfx_STREAM,                "sfx\\towners\\tavown12.wav",  nullptr },// TSFX_TAVERN12
	{ sfx_STREAM,                "sfx\\towners\\tavown13.wav",  nullptr },// TSFX_TAVERN13
	{ sfx_STREAM,                "sfx\\towners\\tavown14.wav",  nullptr },// TSFX_TAVERN14
	{ sfx_STREAM,                "sfx\\towners\\tavown15.wav",  nullptr },// TSFX_TAVERN15
	{ sfx_STREAM,                "sfx\\towners\\tavown16.wav",  nullptr },// TSFX_TAVERN16
	{ sfx_STREAM,                "sfx\\towners\\tavown17.wav",  nullptr },// TSFX_TAVERN17
	{ sfx_STREAM,                "sfx\\towners\\tavown18.wav",  nullptr },// TSFX_TAVERN18
	{ sfx_STREAM,                "sfx\\towners\\tavown19.wav",  nullptr },// TSFX_TAVERN19
	{ sfx_STREAM,                "sfx\\towners\\tavown20.wav",  nullptr },// TSFX_TAVERN20
	{ sfx_STREAM,                "sfx\\towners\\tavown21.wav",  nullptr },// TSFX_TAVERN21
	{ sfx_STREAM,                "sfx\\towners\\tavown22.wav",  nullptr },// TSFX_TAVERN22
	{ sfx_STREAM,                "sfx\\towners\\tavown23.wav",  nullptr },// TSFX_TAVERN23
	{ sfx_STREAM,                "sfx\\towners\\tavown24.wav",  nullptr },// TSFX_TAVERN24
	{ sfx_STREAM,                "sfx\\towners\\tavown25.wav",  nullptr },// TSFX_TAVERN25
	{ sfx_STREAM,                "sfx\\towners\\tavown26.wav",  nullptr },// TSFX_TAVERN26
	{ sfx_STREAM,                "sfx\\towners\\tavown27.wav",  nullptr },// TSFX_TAVERN27
	{ sfx_STREAM,                "sfx\\towners\\tavown28.wav",  nullptr },// TSFX_TAVERN28
	{ sfx_STREAM,                "sfx\\towners\\tavown29.wav",  nullptr },// TSFX_TAVERN29
	{ sfx_STREAM,                "sfx\\towners\\tavown30.wav",  nullptr },// TSFX_TAVERN30
	{ sfx_STREAM,                "sfx\\towners\\tavown31.wav",  nullptr },// TSFX_TAVERN31
	{ sfx_STREAM,                "sfx\\towners\\tavown32.wav",  nullptr },// TSFX_TAVERN32
	{ sfx_STREAM,                "sfx\\towners\\tavown33.wav",  nullptr },// TSFX_TAVERN33
	{ sfx_STREAM,                "sfx\\towners\\tavown34.wav",  nullptr },// TSFX_TAVERN34
	{ sfx_STREAM,                "sfx\\towners\\tavown35.wav",  nullptr },// TSFX_TAVERN35
	{ sfx_STREAM,                "sfx\\towners\\tavown36.wav",  nullptr },// TSFX_TAVERN36
	{ sfx_STREAM,                "sfx\\towners\\tavown37.wav",  nullptr },// TSFX_TAVERN37
	{ sfx_STREAM,                "sfx\\towners\\tavown38.wav",  nullptr },// TSFX_TAVERN38
	{ sfx_STREAM,                "sfx\\towners\\tavown39.wav",  nullptr },// TSFX_TAVERN39
	{ sfx_STREAM,                "sfx\\towners\\tavown40.wav",  nullptr },// TSFX_TAVERN40
	{ sfx_STREAM,                "sfx\\towners\\tavown41.wav",  nullptr },// TSFX_TAVERN41
	{ sfx_STREAM,                "sfx\\towners\\tavown42.wav",  nullptr },// TSFX_TAVERN42
	{ sfx_STREAM,                "sfx\\towners\\tavown43.wav",  nullptr },// TSFX_TAVERN43
	{ sfx_STREAM,                "sfx\\towners\\tavown44.wav",  nullptr },// TSFX_TAVERN44
	{ sfx_STREAM,                "sfx\\towners\\tavown45.wav",  nullptr },// TSFX_TAVERN45
	{ sfx_STREAM,                "sfx\\towners\\witch01.wav",   nullptr },// TSFX_WITCH1
	{ sfx_STREAM,                "sfx\\towners\\witch02.wav",   nullptr },// TSFX_WITCH2
	{ sfx_STREAM,                "sfx\\towners\\witch03.wav",   nullptr },// TSFX_WITCH3
	{ sfx_STREAM,                "sfx\\towners\\witch04.wav",   nullptr },// TSFX_WITCH4
	{ sfx_STREAM,                "sfx\\towners\\witch05.wav",   nullptr },// TSFX_WITCH5
	{ sfx_STREAM,                "sfx\\towners\\witch06.wav",   nullptr },// TSFX_WITCH6
	{ sfx_STREAM,                "sfx\\towners\\witch07.wav",   nullptr },// TSFX_WITCH7
	{ sfx_STREAM,                "sfx\\towners\\witch08.wav",   nullptr },// TSFX_WITCH8
	{ sfx_STREAM,                "sfx\\towners\\witch09.wav",   nullptr },// TSFX_WITCH9
	{ sfx_STREAM,                "sfx\\towners\\witch10.wav",   nullptr },// TSFX_WITCH10
	{ sfx_STREAM,                "sfx\\towners\\witch11.wav",   nullptr },// TSFX_WITCH11
	{ sfx_STREAM,                "sfx\\towners\\witch12.wav",   nullptr },// TSFX_WITCH12
	{ sfx_STREAM,                "sfx\\towners\\witch13.wav",   nullptr },// TSFX_WITCH13
	{ sfx_STREAM,                "sfx\\towners\\witch14.wav",   nullptr },// TSFX_WITCH14
	{ sfx_STREAM,                "sfx\\towners\\witch15.wav",   nullptr },// TSFX_WITCH15
	{ sfx_STREAM,                "sfx\\towners\\witch16.wav",   nullptr },// TSFX_WITCH16
	{ sfx_STREAM,                "sfx\\towners\\witch17.wav",   nullptr },// TSFX_WITCH17
	{ sfx_STREAM,                "sfx\\towners\\witch18.wav",   nullptr },// TSFX_WITCH18
	{ sfx_STREAM,                "sfx\\towners\\witch19.wav",   nullptr },// TSFX_WITCH19
	{ sfx_STREAM,                "sfx\\towners\\witch20.wav",   nullptr },// TSFX_WITCH20
	{ sfx_STREAM,                "sfx\\towners\\witch21.wav",   nullptr },// TSFX_WITCH21
	{ sfx_STREAM,                "sfx\\towners\\witch22.wav",   nullptr },// TSFX_WITCH22
	{ sfx_STREAM,                "sfx\\towners\\witch23.wav",   nullptr },// TSFX_WITCH23
	{ sfx_STREAM,                "sfx\\towners\\witch24.wav",   nullptr },// TSFX_WITCH24
	{ sfx_STREAM,                "sfx\\towners\\witch25.wav",   nullptr },// TSFX_WITCH25
	{ sfx_STREAM,                "sfx\\towners\\witch26.wav",   nullptr },// TSFX_WITCH26
	{ sfx_STREAM,                "sfx\\towners\\witch27.wav",   nullptr },// TSFX_WITCH27
	{ sfx_STREAM,                "sfx\\towners\\witch28.wav",   nullptr },// TSFX_WITCH28
	{ sfx_STREAM,                "sfx\\towners\\witch29.wav",   nullptr },// TSFX_WITCH29
	{ sfx_STREAM,                "sfx\\towners\\witch30.wav",   nullptr },// TSFX_WITCH30
	{ sfx_STREAM,                "sfx\\towners\\witch31.wav",   nullptr },// TSFX_WITCH31
	{ sfx_STREAM,                "sfx\\towners\\witch32.wav",   nullptr },// TSFX_WITCH32
	{ sfx_STREAM,                "sfx\\towners\\witch33.wav",   nullptr },// TSFX_WITCH33
	{ sfx_STREAM,                "sfx\\towners\\witch34.wav",   nullptr },// TSFX_WITCH34
	{ sfx_STREAM,                "sfx\\towners\\witch35.wav",   nullptr },// TSFX_WITCH35
	{ sfx_STREAM,                "sfx\\towners\\witch36.wav",   nullptr },// TSFX_WITCH36
	{ sfx_STREAM,                "sfx\\towners\\witch37.wav",   nullptr },// TSFX_WITCH37
	{ sfx_STREAM,                "sfx\\towners\\witch38.wav",   nullptr },// TSFX_WITCH38
	{ sfx_STREAM,                "sfx\\towners\\witch39.wav",   nullptr },// TSFX_WITCH39
	{ sfx_STREAM,                "sfx\\towners\\witch40.wav",   nullptr },// TSFX_WITCH40
	{ sfx_STREAM,                "sfx\\towners\\witch41.wav",   nullptr },// TSFX_WITCH41
	{ sfx_STREAM,                "sfx\\towners\\witch42.wav",   nullptr },// TSFX_WITCH42
	{ sfx_STREAM,                "sfx\\towners\\witch43.wav",   nullptr },// TSFX_WITCH43
	{ sfx_STREAM,                "sfx\\towners\\witch44.wav",   nullptr },// TSFX_WITCH44
	{ sfx_STREAM,                "sfx\\towners\\witch45.wav",   nullptr },// TSFX_WITCH45
	{ sfx_STREAM,                "sfx\\towners\\witch46.wav",   nullptr },// TSFX_WITCH46
	{ sfx_STREAM,                "sfx\\towners\\witch47.wav",   nullptr },// TSFX_WITCH47
	{ sfx_STREAM,                "sfx\\towners\\witch48.wav",   nullptr },// TSFX_WITCH48
	{ sfx_STREAM,                "sfx\\towners\\witch49.wav",   nullptr },// TSFX_WITCH49
	{ sfx_STREAM,                "sfx\\towners\\witch50.wav",   nullptr },// TSFX_WITCH50
	{ sfx_STREAM,                "sfx\\towners\\wound01.wav",   nullptr },// TSFX_WOUND
	{ sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage01.wav",   nullptr },// PS_MAGE1
	{ sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage02.wav",   nullptr },// PS_MAGE2
	{ sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage03.wav",   nullptr },// PS_MAGE3
	{ sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage04.wav",   nullptr },// PS_MAGE4
	{ sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage05.wav",   nullptr },// PS_MAGE5
	{ sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage06.wav",   nullptr },// PS_MAGE6
	{ sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage07.wav",   nullptr },// PS_MAGE7
	{ sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage08.wav",   nullptr },// PS_MAGE8
	{ sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage09.wav",   nullptr },// PS_MAGE9
	{ sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage10.wav",   nullptr },// PS_MAGE10
	{ sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage11.wav",   nullptr },// PS_MAGE11
	{ sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage12.wav",   nullptr },// PS_MAGE12
	{ sfx_SORCERER,              "sfx\\sorceror\\mage13.wav",   nullptr },// PS_MAGE13
	{ sfx_SORCERER,              "sfx\\sorceror\\mage14.wav",   nullptr },// PS_MAGE14
	{ sfx_SORCERER,              "sfx\\sorceror\\mage15.wav",   nullptr },// PS_MAGE15
	{ sfx_SORCERER,              "sfx\\sorceror\\mage16.wav",   nullptr },// PS_MAGE16
	{ sfx_SORCERER,              "sfx\\sorceror\\mage17.wav",   nullptr },// PS_MAGE17
	{ sfx_SORCERER,              "sfx\\sorceror\\mage18.wav",   nullptr },// PS_MAGE18
	{ sfx_SORCERER,              "sfx\\sorceror\\mage19.wav",   nullptr },// PS_MAGE19
	{ sfx_SORCERER,              "sfx\\sorceror\\mage20.wav",   nullptr },// PS_MAGE20
	{ sfx_SORCERER,              "sfx\\sorceror\\mage21.wav",   nullptr },// PS_MAGE21
	{ sfx_SORCERER,              "sfx\\sorceror\\mage22.wav",   nullptr },// PS_MAGE22
	{ sfx_SORCERER,              "sfx\\sorceror\\mage23.wav",   nullptr },// PS_MAGE23
	{ sfx_SORCERER,              "sfx\\sorceror\\mage24.wav",   nullptr },// PS_MAGE24
	{ sfx_SORCERER,              "sfx\\sorceror\\mage25.wav",   nullptr },// PS_MAGE25
	{ sfx_SORCERER,              "sfx\\sorceror\\mage26.wav",   nullptr },// PS_MAGE26
	{ sfx_SORCERER,              "sfx\\sorceror\\mage27.wav",   nullptr },// PS_MAGE27
	{ sfx_SORCERER,              "sfx\\sorceror\\mage28.wav",   nullptr },// PS_MAGE28
	{ sfx_SORCERER,              "sfx\\sorceror\\mage29.wav",   nullptr },// PS_MAGE29
	{ sfx_SORCERER,              "sfx\\sorceror\\mage30.wav",   nullptr },// PS_MAGE30
	{ sfx_SORCERER,              "sfx\\sorceror\\mage31.wav",   nullptr },// PS_MAGE31
	{ sfx_SORCERER,              "sfx\\sorceror\\mage32.wav",   nullptr },// PS_MAGE32
	{ sfx_SORCERER,              "sfx\\sorceror\\mage33.wav",   nullptr },// PS_MAGE33
	{ sfx_SORCERER,              "sfx\\sorceror\\mage34.wav",   nullptr },// PS_MAGE34
	{ sfx_SORCERER,              "sfx\\sorceror\\mage35.wav",   nullptr },// PS_MAGE35
	{ sfx_SORCERER,              "sfx\\sorceror\\mage36.wav",   nullptr },// PS_MAGE36
	{ sfx_SORCERER,              "sfx\\sorceror\\mage37.wav",   nullptr },// PS_MAGE37
	{ sfx_SORCERER,              "sfx\\sorceror\\mage38.wav",   nullptr },// PS_MAGE38
	{ sfx_SORCERER,              "sfx\\sorceror\\mage39.wav",   nullptr },// PS_MAGE39
	{ sfx_SORCERER,              "sfx\\sorceror\\mage40.wav",   nullptr },// PS_MAGE40
	{ sfx_SORCERER,              "sfx\\sorceror\\mage41.wav",   nullptr },// PS_MAGE41
	{ sfx_SORCERER,              "sfx\\sorceror\\mage42.wav",   nullptr },// PS_MAGE42
	{ sfx_SORCERER,              "sfx\\sorceror\\mage43.wav",   nullptr },// PS_MAGE43
	{ sfx_SORCERER,              "sfx\\sorceror\\mage44.wav",   nullptr },// PS_MAGE44
	{ sfx_SORCERER,              "sfx\\sorceror\\mage45.wav",   nullptr },// PS_MAGE45
	{ sfx_SORCERER,              "sfx\\sorceror\\mage46.wav",   nullptr },// PS_MAGE46
	{ sfx_SORCERER,              "sfx\\sorceror\\mage47.wav",   nullptr },// PS_MAGE47
	{ sfx_SORCERER,              "sfx\\sorceror\\mage48.wav",   nullptr },// PS_MAGE48
	{ sfx_SORCERER,              "sfx\\sorceror\\mage49.wav",   nullptr },// PS_MAGE49
	{ sfx_SORCERER,              "sfx\\sorceror\\mage50.wav",   nullptr },// PS_MAGE50
	{ sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage51.wav",   nullptr },// PS_MAGE51
	{ sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage52.wav",   nullptr },// PS_MAGE52
	{ sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage53.wav",   nullptr },// PS_MAGE53
	{ sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage54.wav",   nullptr },// PS_MAGE54
	{ sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage55.wav",   nullptr },// PS_MAGE55
	{ sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage56.wav",   nullptr },// PS_MAGE56
	{ sfx_SORCERER,              "sfx\\sorceror\\mage57.wav",   nullptr },// PS_MAGE57
	{ sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage58.wav",   nullptr },// PS_MAGE58
	{ sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage59.wav",   nullptr },// PS_MAGE59
	{ sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage60.wav",   nullptr },// PS_MAGE60
	{ sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage61.wav",   nullptr },// PS_MAGE61
	{ sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage62.wav",   nullptr },// PS_MAGE62
	{ sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage63.wav",   nullptr },// PS_MAGE63
	{ sfx_SORCERER,              "sfx\\sorceror\\mage64.wav",   nullptr },// PS_MAGE64
	{ sfx_SORCERER,              "sfx\\sorceror\\mage65.wav",   nullptr },// PS_MAGE65
	{ sfx_SORCERER,              "sfx\\sorceror\\mage66.wav",   nullptr },// PS_MAGE66
	{ sfx_SORCERER,              "sfx\\sorceror\\mage67.wav",   nullptr },// PS_MAGE67
	{ sfx_SORCERER,              "sfx\\sorceror\\mage68.wav",   nullptr },// PS_MAGE68
	{ sfx_SORCERER,              "sfx\\sorceror\\mage69.wav",   nullptr },// PS_MAGE69
	{ sfx_SORCERER,              "sfx\\sorceror\\mage69b.wav",  nullptr },// PS_MAGE69B
	{ sfx_SORCERER,              "sfx\\sorceror\\mage70.wav",   nullptr },// PS_MAGE70
	{ sfx_SORCERER,              "sfx\\sorceror\\mage71.wav",   nullptr },// PS_MAGE71
	{ sfx_SORCERER,              "sfx\\sorceror\\mage72.wav",   nullptr },// PS_MAGE72
	{ sfx_SORCERER,              "sfx\\sorceror\\mage73.wav",   nullptr },// PS_MAGE73
	{ sfx_SORCERER,              "sfx\\sorceror\\mage74.wav",   nullptr },// PS_MAGE74
	{ sfx_SORCERER,              "sfx\\sorceror\\mage75.wav",   nullptr },// PS_MAGE75
	{ sfx_SORCERER,              "sfx\\sorceror\\mage76.wav",   nullptr },// PS_MAGE76
	{ sfx_SORCERER,              "sfx\\sorceror\\mage77.wav",   nullptr },// PS_MAGE77
	{ sfx_SORCERER,              "sfx\\sorceror\\mage78.wav",   nullptr },// PS_MAGE78
	{ sfx_SORCERER,              "sfx\\sorceror\\mage79.wav",   nullptr },// PS_MAGE79
	{ sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage80.wav",   nullptr },// PS_MAGE80
	{ sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage81.wav",   nullptr },// PS_MAGE81
	{ sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage82.wav",   nullptr },// PS_MAGE82
	{ sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage83.wav",   nullptr },// PS_MAGE83
	{ sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage84.wav",   nullptr },// PS_MAGE84
	{ sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage85.wav",   nullptr },// PS_MAGE85
	{ sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage86.wav",   nullptr },// PS_MAGE86
	{ sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage87.wav",   nullptr },// PS_MAGE87
	{ sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage88.wav",   nullptr },// PS_MAGE88
	{ sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage89.wav",   nullptr },// PS_MAGE89
	{ sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage90.wav",   nullptr },// PS_MAGE90
	{ sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage91.wav",   nullptr },// PS_MAGE91
	{ sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage92.wav",   nullptr },// PS_MAGE92
	{ sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage93.wav",   nullptr },// PS_MAGE93
	{ sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage94.wav",   nullptr },// PS_MAGE94
	{ sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage95.wav",   nullptr },// PS_MAGE95
	{ sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage96.wav",   nullptr },// PS_MAGE96
	{ sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage97.wav",   nullptr },// PS_MAGE97
	{ sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage98.wav",   nullptr },// PS_MAGE98
	{ sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage99.wav",   nullptr },// PS_MAGE99
	{ sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage100.wav",  nullptr },// PS_MAGE100
	{ sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage101.wav",  nullptr },// PS_MAGE101
	{ sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage102.wav",  nullptr },// PS_MAGE102
	{ sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue01.wav",     nullptr },// PS_ROGUE1
	{ sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue02.wav",     nullptr },// PS_ROGUE2
	{ sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue03.wav",     nullptr },// PS_ROGUE3
	{ sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue04.wav",     nullptr },// PS_ROGUE4
	{ sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue05.wav",     nullptr },// PS_ROGUE5
	{ sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue06.wav",     nullptr },// PS_ROGUE6
	{ sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue07.wav",     nullptr },// PS_ROGUE7
	{ sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue08.wav",     nullptr },// PS_ROGUE8
	{ sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue09.wav",     nullptr },// PS_ROGUE9
	{ sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue10.wav",     nullptr },// PS_ROGUE10
	{ sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue11.wav",     nullptr },// PS_ROGUE11
	{ sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue12.wav",     nullptr },// PS_ROGUE12
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue13.wav",     nullptr },// PS_ROGUE13
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue14.wav",     nullptr },// PS_ROGUE14
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue15.wav",     nullptr },// PS_ROGUE15
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue16.wav",     nullptr },// PS_ROGUE16
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue17.wav",     nullptr },// PS_ROGUE17
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue18.wav",     nullptr },// PS_ROGUE18
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue19.wav",     nullptr },// PS_ROGUE19
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue20.wav",     nullptr },// PS_ROGUE20
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue21.wav",     nullptr },// PS_ROGUE21
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue22.wav",     nullptr },// PS_ROGUE22
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue23.wav",     nullptr },// PS_ROGUE23
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue24.wav",     nullptr },// PS_ROGUE24
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue25.wav",     nullptr },// PS_ROGUE25
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue26.wav",     nullptr },// PS_ROGUE26
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue27.wav",     nullptr },// PS_ROGUE27
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue28.wav",     nullptr },// PS_ROGUE28
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue29.wav",     nullptr },// PS_ROGUE29
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue30.wav",     nullptr },// PS_ROGUE30
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue31.wav",     nullptr },// PS_ROGUE31
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue32.wav",     nullptr },// PS_ROGUE32
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue33.wav",     nullptr },// PS_ROGUE33
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue34.wav",     nullptr },// PS_ROGUE34
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue35.wav",     nullptr },// PS_ROGUE35
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue36.wav",     nullptr },// PS_ROGUE36
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue37.wav",     nullptr },// PS_ROGUE37
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue38.wav",     nullptr },// PS_ROGUE38
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue39.wav",     nullptr },// PS_ROGUE39
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue40.wav",     nullptr },// PS_ROGUE40
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue41.wav",     nullptr },// PS_ROGUE41
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue42.wav",     nullptr },// PS_ROGUE42
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue43.wav",     nullptr },// PS_ROGUE43
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue44.wav",     nullptr },// PS_ROGUE44
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue45.wav",     nullptr },// PS_ROGUE45
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue46.wav",     nullptr },// PS_ROGUE46
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue47.wav",     nullptr },// PS_ROGUE47
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue48.wav",     nullptr },// PS_ROGUE48
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue49.wav",     nullptr },// PS_ROGUE49
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue50.wav",     nullptr },// PS_ROGUE50
	{ sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue51.wav",     nullptr },// PS_ROGUE51
	{ sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue52.wav",     nullptr },// PS_ROGUE52
	{ sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue53.wav",     nullptr },// PS_ROGUE53
	{ sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue54.wav",     nullptr },// PS_ROGUE54
	{ sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue55.wav",     nullptr },// PS_ROGUE55
	{ sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue56.wav",     nullptr },// PS_ROGUE56
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue57.wav",     nullptr },// PS_ROGUE57
	{ sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue58.wav",     nullptr },// PS_ROGUE58
	{ sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue59.wav",     nullptr },// PS_ROGUE59
	{ sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue60.wav",     nullptr },// PS_ROGUE60
	{ sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue61.wav",     nullptr },// PS_ROGUE61
	{ sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue62.wav",     nullptr },// PS_ROGUE62
	{ sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue63.wav",     nullptr },// PS_ROGUE63
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue64.wav",     nullptr },// PS_ROGUE64
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue65.wav",     nullptr },// PS_ROGUE65
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue66.wav",     nullptr },// PS_ROGUE66
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue67.wav",     nullptr },// PS_ROGUE67
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue68.wav",     nullptr },// PS_ROGUE68
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue69.wav",     nullptr },// PS_ROGUE69
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue69b.wav",    nullptr },// PS_ROGUE69B
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue70.wav",     nullptr },// PS_ROGUE70
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue71.wav",     nullptr },// PS_ROGUE71
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue72.wav",     nullptr },// PS_ROGUE72
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue73.wav",     nullptr },// PS_ROGUE73
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue74.wav",     nullptr },// PS_ROGUE74
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue75.wav",     nullptr },// PS_ROGUE75
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue76.wav",     nullptr },// PS_ROGUE76
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue77.wav",     nullptr },// PS_ROGUE77
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue78.wav",     nullptr },// PS_ROGUE78
	{ sfx_ROGUE,                 "sfx\\rogue\\rogue79.wav",     nullptr },// PS_ROGUE79
	{ sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue80.wav",     nullptr },// PS_ROGUE80
	{ sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue81.wav",     nullptr },// PS_ROGUE81
	{ sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue82.wav",     nullptr },// PS_ROGUE82
	{ sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue83.wav",     nullptr },// PS_ROGUE83
	{ sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue84.wav",     nullptr },// PS_ROGUE84
	{ sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue85.wav",     nullptr },// PS_ROGUE85
	{ sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue86.wav",     nullptr },// PS_ROGUE86
	{ sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue87.wav",     nullptr },// PS_ROGUE87
	{ sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue88.wav",     nullptr },// PS_ROGUE88
	{ sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue89.wav",     nullptr },// PS_ROGUE89
	{ sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue90.wav",     nullptr },// PS_ROGUE90
	{ sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue91.wav",     nullptr },// PS_ROGUE91
	{ sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue92.wav",     nullptr },// PS_ROGUE92
	{ sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue93.wav",     nullptr },// PS_ROGUE93
	{ sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue94.wav",     nullptr },// PS_ROGUE94
	{ sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue95.wav",     nullptr },// PS_ROGUE95
	{ sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue96.wav",     nullptr },// PS_ROGUE96
	{ sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue97.wav",     nullptr },// PS_ROGUE97
	{ sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue98.wav",     nullptr },// PS_ROGUE98
	{ sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue99.wav",     nullptr },// PS_ROGUE99
	{ sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue100.wav",    nullptr },// PS_ROGUE100
	{ sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue101.wav",    nullptr },// PS_ROGUE101
	{ sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue102.wav",    nullptr },// PS_ROGUE102
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior01.wav",  nullptr },// PS_WARR1
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior02.wav",  nullptr },// PS_WARR2
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior03.wav",  nullptr },// PS_WARR3
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior04.wav",  nullptr },// PS_WARR4
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior05.wav",  nullptr },// PS_WARR5
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior06.wav",  nullptr },// PS_WARR6
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior07.wav",  nullptr },// PS_WARR7
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior08.wav",  nullptr },// PS_WARR8
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior09.wav",  nullptr },// PS_WARR9
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior10.wav",  nullptr },// PS_WARR10
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior11.wav",  nullptr },// PS_WARR11
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior12.wav",  nullptr },// PS_WARR12
	{ sfx_WARRIOR,               "sfx\\warrior\\warior13.wav",  nullptr },// PS_WARR13
	{ sfx_WARRIOR,               "sfx\\warrior\\warior14.wav",  nullptr },// PS_WARR14
	{ sfx_WARRIOR,               "sfx\\warrior\\wario14b.wav",  nullptr },// PS_WARR14B
	{ sfx_WARRIOR,               "sfx\\warrior\\wario14c.wav",  nullptr },// PS_WARR14C
	{ sfx_WARRIOR,               "sfx\\warrior\\warior15.wav",  nullptr },// PS_WARR15
	{ sfx_WARRIOR,               "sfx\\warrior\\wario15b.wav",  nullptr },// PS_WARR15B
	{ sfx_WARRIOR,               "sfx\\warrior\\wario15c.wav",  nullptr },// PS_WARR15C
	{ sfx_WARRIOR,               "sfx\\warrior\\warior16.wav",  nullptr },// PS_WARR16
	{ sfx_WARRIOR,               "sfx\\warrior\\wario16b.wav",  nullptr },// PS_WARR16B
	{ sfx_WARRIOR,               "sfx\\warrior\\wario16c.wav",  nullptr },// PS_WARR16C
	{ sfx_WARRIOR,               "sfx\\warrior\\warior17.wav",  nullptr },// PS_WARR17
	{ sfx_WARRIOR,               "sfx\\warrior\\warior18.wav",  nullptr },// PS_WARR18
	{ sfx_WARRIOR,               "sfx\\warrior\\warior19.wav",  nullptr },// PS_WARR19
	{ sfx_WARRIOR,               "sfx\\warrior\\warior20.wav",  nullptr },// PS_WARR20
	{ sfx_WARRIOR,               "sfx\\warrior\\warior21.wav",  nullptr },// PS_WARR21
	{ sfx_WARRIOR,               "sfx\\warrior\\warior22.wav",  nullptr },// PS_WARR22
	{ sfx_WARRIOR,               "sfx\\warrior\\warior23.wav",  nullptr },// PS_WARR23
	{ sfx_WARRIOR,               "sfx\\warrior\\warior24.wav",  nullptr },// PS_WARR24
	{ sfx_WARRIOR,               "sfx\\warrior\\warior25.wav",  nullptr },// PS_WARR25
	{ sfx_WARRIOR,               "sfx\\warrior\\warior26.wav",  nullptr },// PS_WARR26
	{ sfx_WARRIOR,               "sfx\\warrior\\warior27.wav",  nullptr },// PS_WARR27
	{ sfx_WARRIOR,               "sfx\\warrior\\warior28.wav",  nullptr },// PS_WARR28
	{ sfx_WARRIOR,               "sfx\\warrior\\warior29.wav",  nullptr },// PS_WARR29
	{ sfx_WARRIOR,               "sfx\\warrior\\warior30.wav",  nullptr },// PS_WARR30
	{ sfx_WARRIOR,               "sfx\\warrior\\warior31.wav",  nullptr },// PS_WARR31
	{ sfx_WARRIOR,               "sfx\\warrior\\warior32.wav",  nullptr },// PS_WARR32
	{ sfx_WARRIOR,               "sfx\\warrior\\warior33.wav",  nullptr },// PS_WARR33
	{ sfx_WARRIOR,               "sfx\\warrior\\warior34.wav",  nullptr },// PS_WARR34
	{ sfx_WARRIOR,               "sfx\\warrior\\warior35.wav",  nullptr },// PS_WARR35
	{ sfx_WARRIOR,               "sfx\\warrior\\warior36.wav",  nullptr },// PS_WARR36
	{ sfx_WARRIOR,               "sfx\\warrior\\warior37.wav",  nullptr },// PS_WARR37
	{ sfx_WARRIOR,               "sfx\\warrior\\warior38.wav",  nullptr },// PS_WARR38
	{ sfx_WARRIOR,               "sfx\\warrior\\warior39.wav",  nullptr },// PS_WARR39
	{ sfx_WARRIOR,               "sfx\\warrior\\warior40.wav",  nullptr },// PS_WARR40
	{ sfx_WARRIOR,               "sfx\\warrior\\warior41.wav",  nullptr },// PS_WARR41
	{ sfx_WARRIOR,               "sfx\\warrior\\warior42.wav",  nullptr },// PS_WARR42
	{ sfx_WARRIOR,               "sfx\\warrior\\warior43.wav",  nullptr },// PS_WARR43
	{ sfx_WARRIOR,               "sfx\\warrior\\warior44.wav",  nullptr },// PS_WARR44
	{ sfx_WARRIOR,               "sfx\\warrior\\warior45.wav",  nullptr },// PS_WARR45
	{ sfx_WARRIOR,               "sfx\\warrior\\warior46.wav",  nullptr },// PS_WARR46
	{ sfx_WARRIOR,               "sfx\\warrior\\warior47.wav",  nullptr },// PS_WARR47
	{ sfx_WARRIOR,               "sfx\\warrior\\warior48.wav",  nullptr },// PS_WARR48
	{ sfx_WARRIOR,               "sfx\\warrior\\warior49.wav",  nullptr },// PS_WARR49
	{ sfx_WARRIOR,               "sfx\\warrior\\warior50.wav",  nullptr },// PS_WARR50
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior51.wav",  nullptr },// PS_WARR51
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior52.wav",  nullptr },// PS_WARR52
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior53.wav",  nullptr },// PS_WARR53
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior54.wav",  nullptr },// PS_WARR54
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior55.wav",  nullptr },// PS_WARR55
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior56.wav",  nullptr },// PS_WARR56
	{ sfx_WARRIOR,               "sfx\\warrior\\warior57.wav",  nullptr },// PS_WARR57
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior58.wav",  nullptr },// PS_WARR58
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior59.wav",  nullptr },// PS_WARR59
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior60.wav",  nullptr },// PS_WARR60
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior61.wav",  nullptr },// PS_WARR61
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior62.wav",  nullptr },// PS_WARR62
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior63.wav",  nullptr },// PS_WARR63
	{ sfx_WARRIOR,               "sfx\\warrior\\warior64.wav",  nullptr },// PS_WARR64
	{ sfx_WARRIOR,               "sfx\\warrior\\warior65.wav",  nullptr },// PS_WARR65
	{ sfx_WARRIOR,               "sfx\\warrior\\warior66.wav",  nullptr },// PS_WARR66
	{ sfx_WARRIOR,               "sfx\\warrior\\warior67.wav",  nullptr },// PS_WARR67
	{ sfx_WARRIOR,               "sfx\\warrior\\warior68.wav",  nullptr },// PS_WARR68
	{ sfx_WARRIOR,               "sfx\\warrior\\warior69.wav",  nullptr },// PS_WARR69
	{ sfx_WARRIOR,               "sfx\\warrior\\wario69b.wav",  nullptr },// PS_WARR69B
	{ sfx_WARRIOR,               "sfx\\warrior\\warior70.wav",  nullptr },// PS_WARR70
	{ sfx_WARRIOR,               "sfx\\warrior\\warior71.wav",  nullptr },// PS_WARR71
	{ sfx_WARRIOR,               "sfx\\warrior\\warior72.wav",  nullptr },// PS_WARR72
	{ sfx_WARRIOR,               "sfx\\warrior\\warior73.wav",  nullptr },// PS_WARR73
	{ sfx_WARRIOR,               "sfx\\warrior\\warior74.wav",  nullptr },// PS_WARR74
	{ sfx_WARRIOR,               "sfx\\warrior\\warior75.wav",  nullptr },// PS_WARR75
	{ sfx_WARRIOR,               "sfx\\warrior\\warior76.wav",  nullptr },// PS_WARR76
	{ sfx_WARRIOR,               "sfx\\warrior\\warior77.wav",  nullptr },// PS_WARR77
	{ sfx_WARRIOR,               "sfx\\warrior\\warior78.wav",  nullptr },// PS_WARR78
	{ sfx_WARRIOR,               "sfx\\warrior\\warior79.wav",  nullptr },// PS_WARR79
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior80.wav",  nullptr },// PS_WARR80
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior81.wav",  nullptr },// PS_WARR81
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior82.wav",  nullptr },// PS_WARR82
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior83.wav",  nullptr },// PS_WARR83
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior84.wav",  nullptr },// PS_WARR84
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior85.wav",  nullptr },// PS_WARR85
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior86.wav",  nullptr },// PS_WARR86
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior87.wav",  nullptr },// PS_WARR87
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior88.wav",  nullptr },// PS_WARR88
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior89.wav",  nullptr },// PS_WARR89
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior90.wav",  nullptr },// PS_WARR90
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior91.wav",  nullptr },// PS_WARR91
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior92.wav",  nullptr },// PS_WARR92
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior93.wav",  nullptr },// PS_WARR93
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior94.wav",  nullptr },// PS_WARR94
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior95.wav",  nullptr },// PS_WARR95
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\wario95b.wav",  nullptr },// PS_WARR95B
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\wario95c.wav",  nullptr },// PS_WARR95C
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\wario95d.wav",  nullptr },// PS_WARR95D
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\wario95e.wav",  nullptr },// PS_WARR95E
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\wario95f.wav",  nullptr },// PS_WARR95F
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\wario96b.wav",  nullptr },// PS_WARR96B
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\wario97.wav",   nullptr },// PS_WARR97
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\wario98.wav",   nullptr },// PS_WARR98
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior99.wav",  nullptr },// PS_WARR99
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\wario100.wav",  nullptr },// PS_WARR100
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\wario101.wav",  nullptr },// PS_WARR101
	{ sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\wario102.wav",  nullptr },// PS_WARR102
	{ sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk01.wav",       nullptr },// PS_MONK1
	{ sfx_STREAM | sfx_MONK,     "sfx\\misc\\blank.wav",        nullptr },// PS_MONK2
	{ sfx_STREAM | sfx_MONK,     "sfx\\misc\\blank.wav",        nullptr },// PS_MONK3
	{ sfx_STREAM | sfx_MONK,     "sfx\\misc\\blank.wav",        nullptr },// PS_MONK4
	{ sfx_STREAM | sfx_MONK,     "sfx\\misc\\blank.wav",        nullptr },// PS_MONK5
	{ sfx_STREAM | sfx_MONK,     "sfx\\misc\\blank.wav",        nullptr },// PS_MONK6
	{ sfx_STREAM | sfx_MONK,     "sfx\\misc\\blank.wav",        nullptr },// PS_MONK7
	{ sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk08.wav",       nullptr },// PS_MONK8
	{ sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk09.wav",       nullptr },// PS_MONK9
	{ sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk10.wav",       nullptr },// PS_MONK10
	{ sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk11.wav",       nullptr },// PS_MONK11
	{ sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk12.wav",       nullptr },// PS_MONK12
	{ sfx_MONK,                  "sfx\\monk\\monk13.wav",       nullptr },// PS_MONK13
	{ sfx_MONK,                  "sfx\\monk\\monk14.wav",       nullptr },// PS_MONK14
	{ sfx_MONK,                  "sfx\\monk\\monk15.wav",       nullptr },// PS_MONK15
	{ sfx_MONK,                  "sfx\\monk\\monk16.wav",       nullptr },// PS_MONK16
	{ sfx_MONK,                  "sfx\\misc\\blank.wav",        nullptr },// PS_MONK17
	{ sfx_MONK,                  "sfx\\misc\\blank.wav",        nullptr },// PS_MONK18
	{ sfx_MONK,                  "sfx\\misc\\blank.wav",        nullptr },// PS_MONK19
	{ sfx_MONK,                  "sfx\\misc\\blank.wav",        nullptr },// PS_MONK20
	{ sfx_MONK,                  "sfx\\misc\\blank.wav",        nullptr },// PS_MONK21
	{ sfx_MONK,                  "sfx\\misc\\blank.wav",        nullptr },// PS_MONK22
	{ sfx_MONK,                  "sfx\\misc\\blank.wav",        nullptr },// PS_MONK23
	{ sfx_MONK,                  "sfx\\monk\\monk24.wav",       nullptr },// PS_MONK24
	{ sfx_MONK,                  "sfx\\misc\\blank.wav",        nullptr },// PS_MONK25
	{ sfx_MONK,                  "sfx\\misc\\blank.wav",        nullptr },// PS_MONK26
	{ sfx_MONK,                  "sfx\\monk\\monk27.wav",       nullptr },// PS_MONK27
	{ sfx_MONK,                  "sfx\\misc\\blank.wav",        nullptr },// PS_MONK28
	{ sfx_MONK,                  "sfx\\monk\\monk29.wav",       nullptr },// PS_MONK29
	{ sfx_MONK,                  "sfx\\misc\\blank.wav",        nullptr },// PS_MONK30
	{ sfx_MONK,                  "sfx\\misc\\blank.wav",        nullptr },// PS_MONK31
	{ sfx_MONK,                  "sfx\\misc\\blank.wav",        nullptr },// PS_MONK32
	{ sfx_MONK,                  "sfx\\misc\\blank.wav",        nullptr },// PS_MONK33
	{ sfx_MONK,                  "sfx\\monk\\monk34.wav",       nullptr },// PS_MONK34
	{ sfx_MONK,                  "sfx\\monk\\monk35.wav",       nullptr },// PS_MONK35
	{ sfx_MONK,                  "sfx\\misc\\blank.wav",        nullptr },// PS_MONK36
	{ sfx_MONK,                  "sfx\\misc\\blank.wav",        nullptr },// PS_MONK37
	{ sfx_MONK,                  "sfx\\misc\\blank.wav",        nullptr },// PS_MONK38
	{ sfx_MONK,                  "sfx\\misc\\blank.wav",        nullptr },// PS_MONK39
	{ sfx_MONK,                  "sfx\\misc\\blank.wav",        nullptr },// PS_MONK40
	{ sfx_MONK,                  "sfx\\misc\\blank.wav",        nullptr },// PS_MONK41
	{ sfx_MONK,                  "sfx\\misc\\blank.wav",        nullptr },// PS_MONK42
	{ sfx_MONK,                  "sfx\\monk\\monk43.wav",       nullptr },// PS_MONK43
	{ sfx_MONK,                  "sfx\\misc\\blank.wav",        nullptr },// PS_MONK44
	{ sfx_MONK,                  "sfx\\misc\\blank.wav",        nullptr },// PS_MONK45
	{ sfx_MONK,                  "sfx\\monk\\monk46.wav",       nullptr },// PS_MONK46
	{ sfx_MONK,                  "sfx\\misc\\blank.wav",        nullptr },// PS_MONK47
	{ sfx_MONK,                  "sfx\\misc\\blank.wav",        nullptr },// PS_MONK48
	{ sfx_MONK,                  "sfx\\monk\\monk49.wav",       nullptr },// PS_MONK49
	{ sfx_MONK,                  "sfx\\monk\\monk50.wav",       nullptr },// PS_MONK50
	{ sfx_STREAM | sfx_MONK,     "sfx\\misc\\blank.wav",        nullptr },// PS_MONK51
	{ sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk52.wav",       nullptr },// PS_MONK52
	{ sfx_STREAM | sfx_MONK,     "sfx\\misc\\blank.wav",        nullptr },// PS_MONK53
	{ sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk54.wav",       nullptr },// PS_MONK54
	{ sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk55.wav",       nullptr },// PS_MONK55
	{ sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk56.wav",       nullptr },// PS_MONK56
	{ sfx_MONK,                  "sfx\\misc\\blank.wav",        nullptr },// PS_MONK57
	{ sfx_STREAM | sfx_MONK,     "sfx\\misc\\blank.wav",        nullptr },// PS_MONK58
	{ sfx_STREAM | sfx_MONK,     "sfx\\misc\\blank.wav",        nullptr },// PS_MONK59
	{ sfx_STREAM | sfx_MONK,     "sfx\\misc\\blank.wav",        nullptr },// PS_MONK60
	{ sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk61.wav",       nullptr },// PS_MONK61
	{ sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk62.wav",       nullptr },// PS_MONK62
	{ sfx_STREAM | sfx_MONK,     "sfx\\misc\\blank.wav",        nullptr },// PS_MONK63
	{ sfx_MONK,                  "sfx\\misc\\blank.wav",        nullptr },// PS_MONK64
	{ sfx_MONK,                  "sfx\\misc\\blank.wav",        nullptr },// PS_MONK65
	{ sfx_MONK,                  "sfx\\misc\\blank.wav",        nullptr },// PS_MONK66
	{ sfx_MONK,                  "sfx\\misc\\blank.wav",        nullptr },// PS_MONK67
	{ sfx_MONK,                  "sfx\\monk\\monk68.wav",       nullptr },// PS_MONK68
	{ sfx_MONK,                  "sfx\\monk\\monk69.wav",       nullptr },// PS_MONK69
	{ sfx_MONK,                  "sfx\\monk\\monk69b.wav",      nullptr },// PS_MONK69B
	{ sfx_MONK,                  "sfx\\monk\\monk70.wav",       nullptr },// PS_MONK70
	{ sfx_MONK,                  "sfx\\monk\\monk71.wav",       nullptr },// PS_MONK71
	{ sfx_MONK,                  "sfx\\misc\\blank.wav",        nullptr },// PS_MONK72
	{ sfx_MONK,                  "sfx\\misc\\blank.wav",        nullptr },// PS_MONK73
	{ sfx_MONK,                  "sfx\\misc\\blank.wav",        nullptr },// PS_MONK74
	{ sfx_MONK,                  "sfx\\misc\\blank.wav",        nullptr },// PS_MONK75
	{ sfx_MONK,                  "sfx\\misc\\blank.wav",        nullptr },// PS_MONK76
	{ sfx_MONK,                  "sfx\\misc\\blank.wav",        nullptr },// PS_MONK77
	{ sfx_MONK,                  "sfx\\misc\\blank.wav",        nullptr },// PS_MONK78
	{ sfx_MONK,                  "sfx\\monk\\monk79.wav",       nullptr },// PS_MONK79
	{ sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk80.wav",       nullptr },// PS_MONK80
	{ sfx_STREAM | sfx_MONK,     "sfx\\misc\\blank.wav",        nullptr },// PS_MONK81
	{ sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk82.wav",       nullptr },// PS_MONK82
	{ sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk83.wav",       nullptr },// PS_MONK83
	{ sfx_STREAM | sfx_MONK,     "sfx\\misc\\blank.wav",        nullptr },// PS_MONK84
	{ sfx_STREAM | sfx_MONK,     "sfx\\misc\\blank.wav",        nullptr },// PS_MONK85
	{ sfx_STREAM | sfx_MONK,     "sfx\\misc\\blank.wav",        nullptr },// PS_MONK86
	{ sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk87.wav",       nullptr },// PS_MONK87
	{ sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk88.wav",       nullptr },// PS_MONK88
	{ sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk89.wav",       nullptr },// PS_MONK89
	{ sfx_STREAM | sfx_MONK,     "sfx\\misc\\blank.wav",        nullptr },// PS_MONK90
	{ sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk91.wav",       nullptr },// PS_MONK91
	{ sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk92.wav",       nullptr },// PS_MONK92
	{ sfx_STREAM | sfx_MONK,     "sfx\\misc\\blank.wav",        nullptr },// PS_MONK93
	{ sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk94.wav",       nullptr },// PS_MONK94
	{ sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk95.wav",       nullptr },// PS_MONK95
	{ sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk96.wav",       nullptr },// PS_MONK96
	{ sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk97.wav",       nullptr },// PS_MONK97
	{ sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk98.wav",       nullptr },// PS_MONK98
	{ sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk99.wav",       nullptr },// PS_MONK99
	{ sfx_STREAM | sfx_MONK,     "sfx\\misc\\blank.wav",        nullptr },// PS_MONK100
	{ sfx_STREAM | sfx_MONK,     "sfx\\misc\\blank.wav",        nullptr },// PS_MONK101
	{ sfx_STREAM | sfx_MONK,     "sfx\\misc\\blank.wav",        nullptr },// PS_MONK102
	{ sfx_STREAM,                "sfx\\narrator\\nar01.wav",    nullptr },// PS_NAR1
	{ sfx_STREAM,                "sfx\\narrator\\nar02.wav",    nullptr },// PS_NAR2
	{ sfx_STREAM,                "sfx\\narrator\\nar03.wav",    nullptr },// PS_NAR3
	{ sfx_STREAM,                "sfx\\narrator\\nar04.wav",    nullptr },// PS_NAR4
	{ sfx_STREAM,                "sfx\\narrator\\nar05.wav",    nullptr },// PS_NAR5
	{ sfx_STREAM,                "sfx\\narrator\\nar06.wav",    nullptr },// PS_NAR6
	{ sfx_STREAM,                "sfx\\narrator\\nar07.wav",    nullptr },// PS_NAR7
	{ sfx_STREAM,                "sfx\\narrator\\nar08.wav",    nullptr },// PS_NAR8
	{ sfx_STREAM,                "sfx\\narrator\\nar09.wav",    nullptr },// PS_NAR9
	{ sfx_STREAM,                "sfx\\misc\\lvl16int.wav",     nullptr },// PS_DIABLVLINT
	{ sfx_STREAM,                "sfx\\monsters\\butcher.wav",  nullptr },// USFX_CLEAVER
	{ sfx_STREAM,                "sfx\\monsters\\garbud01.wav", nullptr },// USFX_GARBUD1
	{ sfx_STREAM,                "sfx\\monsters\\garbud02.wav", nullptr },// USFX_GARBUD2
	{ sfx_STREAM,                "sfx\\monsters\\garbud03.wav", nullptr },// USFX_GARBUD3
	{ sfx_STREAM,                "sfx\\monsters\\garbud04.wav", nullptr },// USFX_GARBUD4
	{ sfx_STREAM,                "sfx\\monsters\\izual01.wav",  nullptr },// USFX_IZUAL1
	{ sfx_STREAM,                "sfx\\monsters\\lach01.wav",   nullptr },// USFX_LACH1
	{ sfx_STREAM,                "sfx\\monsters\\lach02.wav",   nullptr },// USFX_LACH2
	{ sfx_STREAM,                "sfx\\monsters\\lach03.wav",   nullptr },// USFX_LACH3
	{ sfx_STREAM,                "sfx\\monsters\\laz01.wav",    nullptr },// USFX_LAZ1
	{ sfx_STREAM,                "sfx\\monsters\\laz02.wav",    nullptr },// USFX_LAZ2
	{ sfx_STREAM,                "sfx\\monsters\\sking01.wav",  nullptr },// USFX_SKING1
	{ sfx_STREAM,                "sfx\\monsters\\snot01.wav",   nullptr },// USFX_SNOT1
	{ sfx_STREAM,                "sfx\\monsters\\snot02.wav",   nullptr },// USFX_SNOT2
	{ sfx_STREAM,                "sfx\\monsters\\snot03.wav",   nullptr },// USFX_SNOT3
	{ sfx_STREAM,                "sfx\\monsters\\warlrd01.wav", nullptr },// USFX_WARLRD1
	{ sfx_STREAM,                "sfx\\monsters\\wlock01.wav",  nullptr },// USFX_WLOCK1
	{ sfx_STREAM,                "sfx\\monsters\\zhar01.wav",   nullptr },// USFX_ZHAR1
	{ sfx_STREAM,                "sfx\\monsters\\zhar02.wav",   nullptr },// USFX_ZHAR2
	{ sfx_STREAM,                "sfx\\monsters\\diablod.wav",  nullptr },// USFX_DIABLOD
	{ sfx_STREAM,                "sfx\\hellfire\\farmer1.wav",  nullptr },// TSFX_FARMER1
	{ sfx_STREAM,                "sfx\\hellfire\\farmer2.wav",  nullptr },// TSFX_FARMER2
	{ sfx_STREAM,                "sfx\\hellfire\\farmer2a.wav", nullptr },// TSFX_FARMER2A
	{ sfx_STREAM,                "sfx\\hellfire\\farmer3.wav",  nullptr },// TSFX_FARMER3
	{ sfx_STREAM,                "sfx\\hellfire\\farmer4.wav",  nullptr },// TSFX_FARMER4
	{ sfx_STREAM,                "sfx\\hellfire\\farmer5.wav",  nullptr },// TSFX_FARMER5
	{ sfx_STREAM,                "sfx\\hellfire\\farmer6.wav",  nullptr },// TSFX_FARMER6
	{ sfx_STREAM,                "sfx\\hellfire\\farmer7.wav",  nullptr },// TSFX_FARMER7
	{ sfx_STREAM,                "sfx\\hellfire\\farmer8.wav",  nullptr },// TSFX_FARMER8
	{ sfx_STREAM,                "sfx\\hellfire\\farmer9.wav",  nullptr },// TSFX_FARMER9
	{ sfx_STREAM,                "sfx\\hellfire\\teddybr1.wav", nullptr },// TSFX_TEDDYBR1
	{ sfx_STREAM,                "sfx\\hellfire\\teddybr2.wav", nullptr },// TSFX_TEDDYBR2
	{ sfx_STREAM,                "sfx\\hellfire\\teddybr3.wav", nullptr },// TSFX_TEDDYBR3
	{ sfx_STREAM,                "sfx\\hellfire\\teddybr4.wav", nullptr },// TSFX_TEDDYBR4
	{ sfx_STREAM,                "sfx\\hellfire\\defiler1.wav", nullptr },// USFX_DEFILER1
	{ sfx_STREAM,                "sfx\\hellfire\\defiler2.wav", nullptr },// USFX_DEFILER2
	{ sfx_STREAM,                "sfx\\hellfire\\defiler3.wav", nullptr },// USFX_DEFILER3
	{ sfx_STREAM,                "sfx\\hellfire\\defiler4.wav", nullptr },// USFX_DEFILER4
	{ sfx_STREAM,                "sfx\\hellfire\\defiler8.wav", nullptr },// USFX_DEFILER8
	{ sfx_STREAM,                "sfx\\hellfire\\defiler6.wav", nullptr },// USFX_DEFILER6
	{ sfx_STREAM,                "sfx\\hellfire\\defiler7.wav", nullptr },// USFX_DEFILER7
	{ sfx_STREAM,                "sfx\\hellfire\\nakrul1.wav",  nullptr },// USFX_NAKRUL1
	{ sfx_STREAM,                "sfx\\hellfire\\nakrul2.wav",  nullptr },// USFX_NAKRUL2
	{ sfx_STREAM,                "sfx\\hellfire\\nakrul3.wav",  nullptr },// USFX_NAKRUL3
	{ sfx_STREAM,                "sfx\\hellfire\\nakrul4.wav",  nullptr },// USFX_NAKRUL4
	{ sfx_STREAM,                "sfx\\hellfire\\nakrul5.wav",  nullptr },// USFX_NAKRUL5
	{ sfx_STREAM,                "sfx\\hellfire\\nakrul6.wav",  nullptr },// USFX_NAKRUL6
	{ sfx_STREAM,                "sfx\\hellfire\\naratr3.wav",  nullptr },// PS_NARATR3
	{ sfx_STREAM,                "sfx\\hellfire\\cowsut1.wav",  nullptr },// TSFX_COWSUT1
	{ sfx_STREAM,                "sfx\\hellfire\\cowsut2.wav",  nullptr },// TSFX_COWSUT2
	{ sfx_STREAM,                "sfx\\hellfire\\cowsut3.wav",  nullptr },// TSFX_COWSUT3
	{ sfx_STREAM,                "sfx\\hellfire\\cowsut4.wav",  nullptr },// TSFX_COWSUT4
	{ sfx_STREAM,                "sfx\\hellfire\\cowsut4a.wav", nullptr },// TSFX_COWSUT4A
	{ sfx_STREAM,                "sfx\\hellfire\\cowsut5.wav",  nullptr },// TSFX_COWSUT5
	{ sfx_STREAM,                "sfx\\hellfire\\cowsut6.wav",  nullptr },// TSFX_COWSUT6
	{ sfx_STREAM,                "sfx\\hellfire\\cowsut7.wav",  nullptr },// TSFX_COWSUT7
	{ sfx_STREAM,                "sfx\\hellfire\\cowsut8.wav",  nullptr },// TSFX_COWSUT8
	{ sfx_STREAM,                "sfx\\hellfire\\cowsut9.wav",  nullptr },// TSFX_COWSUT9
	{ sfx_STREAM,                "sfx\\hellfire\\cowsut10.wav", nullptr },// TSFX_COWSUT10
	{ sfx_STREAM,                "sfx\\hellfire\\cowsut11.wav", nullptr },// TSFX_COWSUT11
	{ sfx_STREAM,                "sfx\\hellfire\\cowsut12.wav", nullptr },// TSFX_COWSUT12
	{ sfx_STREAM,                "sfx\\hellfire\\skljrn1.wav",  nullptr },// USFX_SKLJRN1
	{ sfx_STREAM,                "sfx\\hellfire\\naratr6.wav",  nullptr },// PS_NARATR6
	{ sfx_STREAM,                "sfx\\hellfire\\naratr7.wav",  nullptr },// PS_NARATR7
	{ sfx_STREAM,                "sfx\\hellfire\\naratr8.wav",  nullptr },// PS_NARATR8
	{ sfx_STREAM,                "sfx\\hellfire\\naratr5.wav",  nullptr },// PS_NARATR5
	{ sfx_STREAM,                "sfx\\hellfire\\naratr9.wav",  nullptr },// PS_NARATR9
	{ sfx_STREAM,                "sfx\\hellfire\\naratr4.wav",  nullptr },// PS_NARATR4
	{ sfx_STREAM,                "sfx\\hellfire\\trader1.wav",  nullptr },// TSFX_TRADER1
	{ sfx_MISC | sfx_HELLFIRE,   "sfx\\items\\cropen.wav",      nullptr },// IS_CROPEN
	{ sfx_MISC | sfx_HELLFIRE,   "sfx\\items\\crclos.wav",      nullptr },// IS_CRCLOS
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
		if (pSFX->pSnd->DSB.IsLoaded())
			pSFX->pSnd->DSB.PlayWithVolumeAndPan(lVolume, sound_get_or_set_sound_volume(1), lPan);
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
	if (MyPlayer->pLvlLoad != 0 && gbIsMultiplayer) {
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

	if (pSFX->pSnd != nullptr && pSFX->pSnd->DSB.IsLoaded())
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

void PrivSoundInit(uint8_t bLoadMask)
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

bool CalculateSoundPosition(Point soundPosition, int *plVolume, int *plPan)
{
	const auto &playerPosition = MyPlayer->position.tile;
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
	if (!gbSndInited)
		return;
	ClearDuplicateSounds();
	for (auto &sfx : sgSFX) {
		if (sfx.pSnd != nullptr && sfx.pSnd->DSB.IsLoaded()) {
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
		switch (MyPlayer->_pClass) {
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

void effects_play_sound(_sfx_id id)
{
	if (!gbSndInited || !gbSoundOn) {
		return;
	}

	TSFX &sfx = sgSFX[id];
	if (sfx.pSnd != nullptr && !sfx.pSnd->isPlaying()) {
		snd_play_snd(sfx.pSnd.get(), 0, 0);
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
