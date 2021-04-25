/**
 * @file effects.cpp
 *
 * Implementation of functions for loading and playing sounds.
 */
#include <SDL_mixer.h>

#include "init.h"
#include "player.h"
#include "sound.h"

namespace devilution {
namespace {
#ifndef DISABLE_STREAMING_SOUNDS
constexpr bool AllowStreaming = true;
#else
constexpr bool AllowStreaming = false;
#endif
} // namespace

int sfxdelay;
_sfx_id sfxdnum = SFX_NONE;
/** Specifies the sound file and the playback state of the current sound effect. */
static TSFX *sgpStreamSFX = nullptr;

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
	{ sfx_MISC,                  N_("Sfx\\Misc\\Walk1.wav"),        nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Walk2.wav"),        nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Walk3.wav"),        nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Walk4.wav"),        nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\BFire.wav"),        nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Fmag.wav"),         nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Tmag.wav"),         nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Lghit.wav"),        nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Lghit1.wav"),       nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Swing.wav"),        nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Swing2.wav"),       nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Dead.wav"),         nullptr },
	{ sfx_MISC | sfx_HELLFIRE,   N_("Sfx\\Misc\\Sting1.wav"),       nullptr },
	{ sfx_MISC | sfx_HELLFIRE,   N_("Sfx\\Misc\\FBallBow.wav"),     nullptr },
	{ sfx_STREAM,                N_("Sfx\\Misc\\Questdon.wav"),     nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Armrfkd.wav"),     nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Barlfire.wav"),    nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Barrel.wav"),      nullptr },
	{ sfx_MISC | sfx_HELLFIRE,   N_("Sfx\\Items\\PodPop8.wav"),     nullptr },
	{ sfx_MISC | sfx_HELLFIRE,   N_("Sfx\\Items\\PodPop5.wav"),     nullptr },
	{ sfx_MISC | sfx_HELLFIRE,   N_("Sfx\\Items\\UrnPop3.wav"),     nullptr },
	{ sfx_MISC | sfx_HELLFIRE,   N_("Sfx\\Items\\UrnPop2.wav"),     nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Bhit.wav"),        nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Bhit1.wav"),       nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Chest.wav"),       nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Doorclos.wav"),    nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Dooropen.wav"),    nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Flipanvl.wav"),    nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Flipaxe.wav"),     nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Flipblst.wav"),    nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Flipbody.wav"),    nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Flipbook.wav"),    nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Flipbow.wav"),     nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Flipcap.wav"),     nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Flipharm.wav"),    nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Fliplarm.wav"),    nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Flipmag.wav"),     nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Flipmag1.wav"),    nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Flipmush.wav"),    nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Flippot.wav"),     nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Flipring.wav"),    nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Fliprock.wav"),    nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Flipscrl.wav"),    nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Flipshld.wav"),    nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Flipsign.wav"),    nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Flipstaf.wav"),    nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Flipswor.wav"),    nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Gold.wav"),        nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Hlmtfkd.wav"),     nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Invanvl.wav"),     nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Invaxe.wav"),      nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Invblst.wav"),     nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Invbody.wav"),     nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Invbook.wav"),     nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Invbow.wav"),      nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Invcap.wav"),      nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Invgrab.wav"),     nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Invharm.wav"),     nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Invlarm.wav"),     nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Invmush.wav"),     nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Invpot.wav"),      nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Invring.wav"),     nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Invrock.wav"),     nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Invscrol.wav"),    nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Invshiel.wav"),    nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Invsign.wav"),     nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Invstaf.wav"),     nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Invsword.wav"),    nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Lever.wav"),       nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Magic.wav"),       nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Magic1.wav"),      nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Readbook.wav"),    nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Sarc.wav"),        nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Shielfkd.wav"),    nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Swrdfkd.wav"),     nullptr },
	{ sfx_UI,                    N_("Sfx\\Items\\Titlemov.wav"),    nullptr },
	{ sfx_UI,                    N_("Sfx\\Items\\Titlslct.wav"),    nullptr },
	{ sfx_UI,                    N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_MISC,                  N_("Sfx\\Items\\Trap.wav"),        nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Cast1.wav"),        nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Cast10.wav"),       nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Cast12.wav"),       nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Cast2.wav"),        nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Cast3.wav"),        nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Cast4.wav"),        nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Cast5.wav"),        nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Cast6.wav"),        nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Cast7.wav"),        nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Cast8.wav"),        nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Cast9.wav"),        nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Healing.wav"),      nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Repair.wav"),       nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Acids1.wav"),       nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Acids2.wav"),       nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Apoc.wav"),         nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Arrowall.wav"),     nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Bldboil.wav"),      nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Blodstar.wav"),     nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Blsimpt.wav"),      nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Bonesp.wav"),       nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Bsimpct.wav"),      nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Caldron.wav"),      nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Cbolt.wav"),        nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Chltning.wav"),     nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\DSerp.wav"),        nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Elecimp1.wav"),     nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Elementl.wav"),     nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Ethereal.wav"),     nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Fball.wav"),        nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Fbolt1.wav"),       nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Fbolt2.wav"),       nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Firimp1.wav"),      nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Firimp2.wav"),      nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Flamwave.wav"),     nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Flash.wav"),        nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Fountain.wav"),     nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Golum.wav"),        nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Golumded.wav"),     nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Gshrine.wav"),      nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Guard.wav"),        nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Grdlanch.wav"),     nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Holybolt.wav"),     nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Hyper.wav"),        nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Infravis.wav"),     nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Invisibl.wav"),     nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Invpot.wav"),       nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Lning1.wav"),       nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Ltning.wav"),       nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Mshield.wav"),      nullptr },
	{ sfx_MISC | sfx_HELLFIRE,   N_("Sfx\\Misc\\NestXpld.wav"),     nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Nova.wav"),         nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Portal.wav"),       nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Puddle.wav"),       nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Resur.wav"),        nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Scurse.wav"),       nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Scurimp.wav"),      nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Sentinel.wav"),     nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Shatter.wav"),      nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Soulfire.wav"),     nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Spoutlop.wav"),     nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Spoutstr.wav"),     nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Storm.wav"),        nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Trapdis.wav"),      nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Teleport.wav"),     nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Vtheft.wav"),       nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Wallloop.wav"),     nullptr },
	{ sfx_MISC,                  N_("Sfx\\Misc\\Wallstrt.wav"),     nullptr },
	{ sfx_MISC | sfx_HELLFIRE,   N_("Sfx\\Misc\\LMag.wav"),         nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bmaid01.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bmaid02.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bmaid03.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bmaid04.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bmaid05.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bmaid06.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bmaid07.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bmaid08.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bmaid09.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bmaid10.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bmaid11.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bmaid12.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bmaid13.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bmaid14.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bmaid15.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bmaid16.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bmaid17.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bmaid18.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bmaid19.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bmaid20.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bmaid21.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bmaid22.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bmaid23.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bmaid24.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bmaid25.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bmaid26.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bmaid27.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bmaid28.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bmaid29.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bmaid30.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bmaid31.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bmaid32.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bmaid33.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bmaid34.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bmaid35.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bmaid36.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bmaid37.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bmaid38.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bmaid39.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bmaid40.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith01.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith02.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith03.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith04.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith05.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith06.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith07.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith08.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith09.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith10.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith11.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith12.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith13.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith14.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith15.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith16.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith17.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith18.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith19.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith20.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith21.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith22.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith23.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith24.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith25.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith26.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith27.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith28.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith29.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith30.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith31.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith32.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith33.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith34.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith35.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith36.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith37.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith38.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith39.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith40.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith41.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith42.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith43.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith44.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith45.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith46.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith47.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith48.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith49.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith50.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith51.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith52.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith53.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith54.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith55.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Bsmith56.wav"),  nullptr },
	{ sfx_MISC,                  N_("Sfx\\Towners\\Cow1.wav"),      nullptr },
	{ sfx_MISC,                  N_("Sfx\\Towners\\Cow2.wav"),      nullptr },
/*
	{ sfx_MISC,                  "Sfx\\Towners\\Cow3.wav",      NULL },
	{ sfx_MISC,                  "Sfx\\Towners\\Cow4.wav",      NULL },
	{ sfx_MISC,                  "Sfx\\Towners\\Cow5.wav",      NULL },
	{ sfx_MISC,                  "Sfx\\Towners\\Cow6.wav",      NULL },
*/
	{ sfx_MISC | sfx_HELLFIRE,   N_("Sfx\\Towners\\Cow7.wav"),      nullptr },
	{ sfx_MISC | sfx_HELLFIRE,   N_("Sfx\\Towners\\Cow8.wav"),      nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Deadguy2.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Drunk01.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Drunk02.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Drunk03.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Drunk04.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Drunk05.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Drunk06.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Drunk07.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Drunk08.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Drunk09.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Drunk10.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Drunk11.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Drunk12.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Drunk13.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Drunk14.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Drunk15.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Drunk16.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Drunk17.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Drunk18.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Drunk19.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Drunk20.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Drunk21.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Drunk22.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Drunk23.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Drunk24.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Drunk25.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Drunk26.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Drunk27.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Drunk28.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Drunk29.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Drunk30.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Drunk31.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Drunk32.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Drunk33.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Drunk34.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Drunk35.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Healer01.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Healer02.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Healer03.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Healer04.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Healer05.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Healer06.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Healer07.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Healer08.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Healer09.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Healer10.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Healer11.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Healer12.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Healer13.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Healer14.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Healer15.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Healer16.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Healer17.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Healer18.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Healer19.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Healer20.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Healer21.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Healer22.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Healer23.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Healer24.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Healer25.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Healer26.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Healer27.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Healer28.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Healer29.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Healer30.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Healer31.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Healer32.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Healer33.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Healer34.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Healer35.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Healer36.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Healer37.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Healer38.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Healer39.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Healer40.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Healer41.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Healer42.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Healer43.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Healer44.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Healer45.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Healer46.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Healer47.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Pegboy01.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Pegboy02.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Pegboy03.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Pegboy04.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Pegboy05.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Pegboy06.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Pegboy07.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Pegboy08.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Pegboy09.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Pegboy10.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Pegboy11.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Pegboy12.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Pegboy13.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Pegboy14.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Pegboy15.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Pegboy16.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Pegboy17.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Pegboy18.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Pegboy19.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Pegboy20.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Pegboy21.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Pegboy22.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Pegboy23.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Pegboy24.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Pegboy25.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Pegboy26.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Pegboy27.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Pegboy28.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Pegboy29.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Pegboy30.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Pegboy31.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Pegboy32.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Pegboy33.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Pegboy34.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Pegboy35.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Pegboy36.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Pegboy37.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Pegboy38.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Pegboy39.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Pegboy40.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Pegboy41.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Pegboy42.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Pegboy43.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Priest00.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Priest01.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Priest02.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Priest03.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Priest04.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Priest05.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Priest06.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Priest07.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Storyt00.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Storyt01.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Storyt02.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Storyt03.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Storyt04.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Storyt05.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Storyt06.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Storyt07.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Storyt08.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Storyt09.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Storyt10.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Storyt11.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Storyt12.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Storyt13.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Storyt14.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Storyt15.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Storyt16.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Storyt17.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Storyt18.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Storyt19.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Storyt20.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Storyt21.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Storyt22.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Storyt23.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Storyt24.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Storyt25.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Storyt26.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Storyt27.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Storyt28.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Storyt29.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Storyt30.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Storyt31.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Storyt32.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Storyt33.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Storyt34.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Storyt35.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Storyt36.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Storyt37.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Storyt38.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Tavown00.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Tavown01.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Tavown02.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Tavown03.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Tavown04.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Tavown05.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Tavown06.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Tavown07.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Tavown08.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Tavown09.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Tavown10.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Tavown11.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Tavown12.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Tavown13.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Tavown14.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Tavown15.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Tavown16.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Tavown17.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Tavown18.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Tavown19.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Tavown20.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Tavown21.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Tavown22.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Tavown23.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Tavown24.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Tavown25.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Tavown26.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Tavown27.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Tavown28.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Tavown29.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Tavown30.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Tavown31.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Tavown32.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Tavown33.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Tavown34.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Tavown35.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Tavown36.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Tavown37.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Tavown38.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Tavown39.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Tavown40.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Tavown41.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Tavown42.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Tavown43.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Tavown44.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Tavown45.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Witch01.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Witch02.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Witch03.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Witch04.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Witch05.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Witch06.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Witch07.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Witch08.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Witch09.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Witch10.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Witch11.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Witch12.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Witch13.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Witch14.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Witch15.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Witch16.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Witch17.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Witch18.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Witch19.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Witch20.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Witch21.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Witch22.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Witch23.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Witch24.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Witch25.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Witch26.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Witch27.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Witch28.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Witch29.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Witch30.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Witch31.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Witch32.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Witch33.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Witch34.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Witch35.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Witch36.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Witch37.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Witch38.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Witch39.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Witch40.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Witch41.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Witch42.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Witch43.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Witch44.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Witch45.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Witch46.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Witch47.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Witch48.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Witch49.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Witch50.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Towners\\Wound01.wav"),   nullptr },
	{ sfx_STREAM | sfx_SORCERER, N_("Sfx\\Sorceror\\Mage01.wav"),   nullptr },
	{ sfx_STREAM | sfx_SORCERER, N_("Sfx\\Sorceror\\Mage02.wav"),   nullptr },
	{ sfx_STREAM | sfx_SORCERER, N_("Sfx\\Sorceror\\Mage03.wav"),   nullptr },
	{ sfx_STREAM | sfx_SORCERER, N_("Sfx\\Sorceror\\Mage04.wav"),   nullptr },
	{ sfx_STREAM | sfx_SORCERER, N_("Sfx\\Sorceror\\Mage05.wav"),   nullptr },
	{ sfx_STREAM | sfx_SORCERER, N_("Sfx\\Sorceror\\Mage06.wav"),   nullptr },
	{ sfx_STREAM | sfx_SORCERER, N_("Sfx\\Sorceror\\Mage07.wav"),   nullptr },
	{ sfx_STREAM | sfx_SORCERER, N_("Sfx\\Sorceror\\Mage08.wav"),   nullptr },
	{ sfx_STREAM | sfx_SORCERER, N_("Sfx\\Sorceror\\Mage09.wav"),   nullptr },
	{ sfx_STREAM | sfx_SORCERER, N_("Sfx\\Sorceror\\Mage10.wav"),   nullptr },
	{ sfx_STREAM | sfx_SORCERER, N_("Sfx\\Sorceror\\Mage11.wav"),   nullptr },
	{ sfx_STREAM | sfx_SORCERER, N_("Sfx\\Sorceror\\Mage12.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage13.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage14.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage15.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage16.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage17.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage18.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage19.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage20.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage21.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage22.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage23.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage24.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage25.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage26.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage27.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage28.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage29.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage30.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage31.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage32.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage33.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage34.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage35.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage36.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage37.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage38.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage39.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage40.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage41.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage42.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage43.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage44.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage45.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage46.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage47.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage48.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage49.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage50.wav"),   nullptr },
	{ sfx_STREAM | sfx_SORCERER, N_("Sfx\\Sorceror\\Mage51.wav"),   nullptr },
	{ sfx_STREAM | sfx_SORCERER, N_("Sfx\\Sorceror\\Mage52.wav"),   nullptr },
	{ sfx_STREAM | sfx_SORCERER, N_("Sfx\\Sorceror\\Mage53.wav"),   nullptr },
	{ sfx_STREAM | sfx_SORCERER, N_("Sfx\\Sorceror\\Mage54.wav"),   nullptr },
	{ sfx_STREAM | sfx_SORCERER, N_("Sfx\\Sorceror\\Mage55.wav"),   nullptr },
	{ sfx_STREAM | sfx_SORCERER, N_("Sfx\\Sorceror\\Mage56.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage57.wav"),   nullptr },
	{ sfx_STREAM | sfx_SORCERER, N_("Sfx\\Sorceror\\Mage58.wav"),   nullptr },
	{ sfx_STREAM | sfx_SORCERER, N_("Sfx\\Sorceror\\Mage59.wav"),   nullptr },
	{ sfx_STREAM | sfx_SORCERER, N_("Sfx\\Sorceror\\Mage60.wav"),   nullptr },
	{ sfx_STREAM | sfx_SORCERER, N_("Sfx\\Sorceror\\Mage61.wav"),   nullptr },
	{ sfx_STREAM | sfx_SORCERER, N_("Sfx\\Sorceror\\Mage62.wav"),   nullptr },
	{ sfx_STREAM | sfx_SORCERER, N_("Sfx\\Sorceror\\Mage63.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage64.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage65.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage66.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage67.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage68.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage69.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage69b.wav"),  nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage70.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage71.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage72.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage73.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage74.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage75.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage76.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage77.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage78.wav"),   nullptr },
	{ sfx_SORCERER,              N_("Sfx\\Sorceror\\Mage79.wav"),   nullptr },
	{ sfx_STREAM | sfx_SORCERER, N_("Sfx\\Sorceror\\Mage80.wav"),   nullptr },
	{ sfx_STREAM | sfx_SORCERER, N_("Sfx\\Sorceror\\Mage81.wav"),   nullptr },
	{ sfx_STREAM | sfx_SORCERER, N_("Sfx\\Sorceror\\Mage82.wav"),   nullptr },
	{ sfx_STREAM | sfx_SORCERER, N_("Sfx\\Sorceror\\Mage83.wav"),   nullptr },
	{ sfx_STREAM | sfx_SORCERER, N_("Sfx\\Sorceror\\Mage84.wav"),   nullptr },
	{ sfx_STREAM | sfx_SORCERER, N_("Sfx\\Sorceror\\Mage85.wav"),   nullptr },
	{ sfx_STREAM | sfx_SORCERER, N_("Sfx\\Sorceror\\Mage86.wav"),   nullptr },
	{ sfx_STREAM | sfx_SORCERER, N_("Sfx\\Sorceror\\Mage87.wav"),   nullptr },
	{ sfx_STREAM | sfx_SORCERER, N_("Sfx\\Sorceror\\Mage88.wav"),   nullptr },
	{ sfx_STREAM | sfx_SORCERER, N_("Sfx\\Sorceror\\Mage89.wav"),   nullptr },
	{ sfx_STREAM | sfx_SORCERER, N_("Sfx\\Sorceror\\Mage90.wav"),   nullptr },
	{ sfx_STREAM | sfx_SORCERER, N_("Sfx\\Sorceror\\Mage91.wav"),   nullptr },
	{ sfx_STREAM | sfx_SORCERER, N_("Sfx\\Sorceror\\Mage92.wav"),   nullptr },
	{ sfx_STREAM | sfx_SORCERER, N_("Sfx\\Sorceror\\Mage93.wav"),   nullptr },
	{ sfx_STREAM | sfx_SORCERER, N_("Sfx\\Sorceror\\Mage94.wav"),   nullptr },
	{ sfx_STREAM | sfx_SORCERER, N_("Sfx\\Sorceror\\Mage95.wav"),   nullptr },
	{ sfx_STREAM | sfx_SORCERER, N_("Sfx\\Sorceror\\Mage96.wav"),   nullptr },
	{ sfx_STREAM | sfx_SORCERER, N_("Sfx\\Sorceror\\Mage97.wav"),   nullptr },
	{ sfx_STREAM | sfx_SORCERER, N_("Sfx\\Sorceror\\Mage98.wav"),   nullptr },
	{ sfx_STREAM | sfx_SORCERER, N_("Sfx\\Sorceror\\Mage99.wav"),   nullptr },
	{ sfx_STREAM | sfx_SORCERER, N_("Sfx\\Sorceror\\Mage100.wav"),  nullptr },
	{ sfx_STREAM | sfx_SORCERER, N_("Sfx\\Sorceror\\Mage101.wav"),  nullptr },
	{ sfx_STREAM | sfx_SORCERER, N_("Sfx\\Sorceror\\Mage102.wav"),  nullptr },
	{ sfx_STREAM | sfx_ROGUE,    N_("Sfx\\Rogue\\Rogue01.wav"),     nullptr },
	{ sfx_STREAM | sfx_ROGUE,    N_("Sfx\\Rogue\\Rogue02.wav"),     nullptr },
	{ sfx_STREAM | sfx_ROGUE,    N_("Sfx\\Rogue\\Rogue03.wav"),     nullptr },
	{ sfx_STREAM | sfx_ROGUE,    N_("Sfx\\Rogue\\Rogue04.wav"),     nullptr },
	{ sfx_STREAM | sfx_ROGUE,    N_("Sfx\\Rogue\\Rogue05.wav"),     nullptr },
	{ sfx_STREAM | sfx_ROGUE,    N_("Sfx\\Rogue\\Rogue06.wav"),     nullptr },
	{ sfx_STREAM | sfx_ROGUE,    N_("Sfx\\Rogue\\Rogue07.wav"),     nullptr },
	{ sfx_STREAM | sfx_ROGUE,    N_("Sfx\\Rogue\\Rogue08.wav"),     nullptr },
	{ sfx_STREAM | sfx_ROGUE,    N_("Sfx\\Rogue\\Rogue09.wav"),     nullptr },
	{ sfx_STREAM | sfx_ROGUE,    N_("Sfx\\Rogue\\Rogue10.wav"),     nullptr },
	{ sfx_STREAM | sfx_ROGUE,    N_("Sfx\\Rogue\\Rogue11.wav"),     nullptr },
	{ sfx_STREAM | sfx_ROGUE,    N_("Sfx\\Rogue\\Rogue12.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue13.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue14.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue15.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue16.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue17.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue18.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue19.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue20.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue21.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue22.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue23.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue24.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue25.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue26.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue27.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue28.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue29.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue30.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue31.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue32.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue33.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue34.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue35.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue36.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue37.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue38.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue39.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue40.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue41.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue42.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue43.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue44.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue45.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue46.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue47.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue48.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue49.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue50.wav"),     nullptr },
	{ sfx_STREAM | sfx_ROGUE,    N_("Sfx\\Rogue\\Rogue51.wav"),     nullptr },
	{ sfx_STREAM | sfx_ROGUE,    N_("Sfx\\Rogue\\Rogue52.wav"),     nullptr },
	{ sfx_STREAM | sfx_ROGUE,    N_("Sfx\\Rogue\\Rogue53.wav"),     nullptr },
	{ sfx_STREAM | sfx_ROGUE,    N_("Sfx\\Rogue\\Rogue54.wav"),     nullptr },
	{ sfx_STREAM | sfx_ROGUE,    N_("Sfx\\Rogue\\Rogue55.wav"),     nullptr },
	{ sfx_STREAM | sfx_ROGUE,    N_("Sfx\\Rogue\\Rogue56.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue57.wav"),     nullptr },
	{ sfx_STREAM | sfx_ROGUE,    N_("Sfx\\Rogue\\Rogue58.wav"),     nullptr },
	{ sfx_STREAM | sfx_ROGUE,    N_("Sfx\\Rogue\\Rogue59.wav"),     nullptr },
	{ sfx_STREAM | sfx_ROGUE,    N_("Sfx\\Rogue\\Rogue60.wav"),     nullptr },
	{ sfx_STREAM | sfx_ROGUE,    N_("Sfx\\Rogue\\Rogue61.wav"),     nullptr },
	{ sfx_STREAM | sfx_ROGUE,    N_("Sfx\\Rogue\\Rogue62.wav"),     nullptr },
	{ sfx_STREAM | sfx_ROGUE,    N_("Sfx\\Rogue\\Rogue63.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue64.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue65.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue66.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue67.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue68.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue69.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue69b.wav"),    nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue70.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue71.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue72.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue73.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue74.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue75.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue76.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue77.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue78.wav"),     nullptr },
	{ sfx_ROGUE,                 N_("Sfx\\Rogue\\Rogue79.wav"),     nullptr },
	{ sfx_STREAM | sfx_ROGUE,    N_("Sfx\\Rogue\\Rogue80.wav"),     nullptr },
	{ sfx_STREAM | sfx_ROGUE,    N_("Sfx\\Rogue\\Rogue81.wav"),     nullptr },
	{ sfx_STREAM | sfx_ROGUE,    N_("Sfx\\Rogue\\Rogue82.wav"),     nullptr },
	{ sfx_STREAM | sfx_ROGUE,    N_("Sfx\\Rogue\\Rogue83.wav"),     nullptr },
	{ sfx_STREAM | sfx_ROGUE,    N_("Sfx\\Rogue\\Rogue84.wav"),     nullptr },
	{ sfx_STREAM | sfx_ROGUE,    N_("Sfx\\Rogue\\Rogue85.wav"),     nullptr },
	{ sfx_STREAM | sfx_ROGUE,    N_("Sfx\\Rogue\\Rogue86.wav"),     nullptr },
	{ sfx_STREAM | sfx_ROGUE,    N_("Sfx\\Rogue\\Rogue87.wav"),     nullptr },
	{ sfx_STREAM | sfx_ROGUE,    N_("Sfx\\Rogue\\Rogue88.wav"),     nullptr },
	{ sfx_STREAM | sfx_ROGUE,    N_("Sfx\\Rogue\\Rogue89.wav"),     nullptr },
	{ sfx_STREAM | sfx_ROGUE,    N_("Sfx\\Rogue\\Rogue90.wav"),     nullptr },
	{ sfx_STREAM | sfx_ROGUE,    N_("Sfx\\Rogue\\Rogue91.wav"),     nullptr },
	{ sfx_STREAM | sfx_ROGUE,    N_("Sfx\\Rogue\\Rogue92.wav"),     nullptr },
	{ sfx_STREAM | sfx_ROGUE,    N_("Sfx\\Rogue\\Rogue93.wav"),     nullptr },
	{ sfx_STREAM | sfx_ROGUE,    N_("Sfx\\Rogue\\Rogue94.wav"),     nullptr },
	{ sfx_STREAM | sfx_ROGUE,    N_("Sfx\\Rogue\\Rogue95.wav"),     nullptr },
	{ sfx_STREAM | sfx_ROGUE,    N_("Sfx\\Rogue\\Rogue96.wav"),     nullptr },
	{ sfx_STREAM | sfx_ROGUE,    N_("Sfx\\Rogue\\Rogue97.wav"),     nullptr },
	{ sfx_STREAM | sfx_ROGUE,    N_("Sfx\\Rogue\\Rogue98.wav"),     nullptr },
	{ sfx_STREAM | sfx_ROGUE,    N_("Sfx\\Rogue\\Rogue99.wav"),     nullptr },
	{ sfx_STREAM | sfx_ROGUE,    N_("Sfx\\Rogue\\Rogue100.wav"),    nullptr },
	{ sfx_STREAM | sfx_ROGUE,    N_("Sfx\\Rogue\\Rogue101.wav"),    nullptr },
	{ sfx_STREAM | sfx_ROGUE,    N_("Sfx\\Rogue\\Rogue102.wav"),    nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Warior01.wav"),  nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Warior02.wav"),  nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Warior03.wav"),  nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Warior04.wav"),  nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Warior05.wav"),  nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Warior06.wav"),  nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Warior07.wav"),  nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Warior08.wav"),  nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Warior09.wav"),  nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Warior10.wav"),  nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Warior11.wav"),  nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Warior12.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior13.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior14.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Wario14b.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Wario14c.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior15.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Wario15b.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Wario15c.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior16.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Wario16b.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Wario16c.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior17.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior18.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior19.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior20.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior21.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior22.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior23.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior24.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior25.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior26.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior27.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior28.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior29.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior30.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior31.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior32.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior33.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior34.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior35.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior36.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior37.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior38.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior39.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior40.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior41.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior42.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior43.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior44.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior45.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior46.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior47.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior48.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior49.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior50.wav"),  nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Warior51.wav"),  nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Warior52.wav"),  nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Warior53.wav"),  nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Warior54.wav"),  nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Warior55.wav"),  nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Warior56.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior57.wav"),  nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Warior58.wav"),  nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Warior59.wav"),  nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Warior60.wav"),  nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Warior61.wav"),  nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Warior62.wav"),  nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Warior63.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior64.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior65.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior66.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior67.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior68.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior69.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Wario69b.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior70.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior71.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior72.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior73.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior74.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior75.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior76.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior77.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior78.wav"),  nullptr },
	{ sfx_WARRIOR,               N_("Sfx\\Warrior\\Warior79.wav"),  nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Warior80.wav"),  nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Warior81.wav"),  nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Warior82.wav"),  nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Warior83.wav"),  nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Warior84.wav"),  nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Warior85.wav"),  nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Warior86.wav"),  nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Warior87.wav"),  nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Warior88.wav"),  nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Warior89.wav"),  nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Warior90.wav"),  nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Warior91.wav"),  nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Warior92.wav"),  nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Warior93.wav"),  nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Warior94.wav"),  nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Warior95.wav"),  nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Wario95b.wav"),  nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Wario95c.wav"),  nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Wario95d.wav"),  nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Wario95e.wav"),  nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Wario95f.wav"),  nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Wario96b.wav"),  nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Wario97.wav"),   nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Wario98.wav"),   nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Warior99.wav"),  nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Wario100.wav"),  nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Wario101.wav"),  nullptr },
	{ sfx_STREAM | sfx_WARRIOR,  N_("Sfx\\Warrior\\Wario102.wav"),  nullptr },
	{ sfx_STREAM | sfx_MONK,     N_("Sfx\\Monk\\Monk01.wav"),       nullptr },
	{ sfx_STREAM | sfx_MONK,     N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_STREAM | sfx_MONK,     N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_STREAM | sfx_MONK,     N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_STREAM | sfx_MONK,     N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_STREAM | sfx_MONK,     N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_STREAM | sfx_MONK,     N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_STREAM | sfx_MONK,     N_("Sfx\\Monk\\Monk08.wav"),       nullptr },
	{ sfx_STREAM | sfx_MONK,     N_("Sfx\\Monk\\Monk09.wav"),       nullptr },
	{ sfx_STREAM | sfx_MONK,     N_("Sfx\\Monk\\Monk10.wav"),       nullptr },
	{ sfx_STREAM | sfx_MONK,     N_("Sfx\\Monk\\Monk11.wav"),       nullptr },
	{ sfx_STREAM | sfx_MONK,     N_("Sfx\\Monk\\Monk12.wav"),       nullptr },
	{ sfx_MONK,                  N_("Sfx\\Monk\\Monk13.wav"),       nullptr },
	{ sfx_MONK,                  N_("Sfx\\Monk\\Monk14.wav"),       nullptr },
	{ sfx_MONK,                  N_("Sfx\\Monk\\Monk15.wav"),       nullptr },
	{ sfx_MONK,                  N_("Sfx\\Monk\\Monk16.wav"),       nullptr },
	{ sfx_MONK,                  N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_MONK,                  N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_MONK,                  N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_MONK,                  N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_MONK,                  N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_MONK,                  N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_MONK,                  N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_MONK,                  N_("Sfx\\Monk\\Monk24.wav"),       nullptr },
	{ sfx_MONK,                  N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_MONK,                  N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_MONK,                  N_("Sfx\\Monk\\Monk27.wav"),       nullptr },
	{ sfx_MONK,                  N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_MONK,                  N_("Sfx\\Monk\\Monk29.wav"),       nullptr },
	{ sfx_MONK,                  N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_MONK,                  N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_MONK,                  N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_MONK,                  N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_MONK,                  N_("Sfx\\Monk\\Monk34.wav"),       nullptr },
	{ sfx_MONK,                  N_("Sfx\\Monk\\Monk35.wav"),       nullptr },
	{ sfx_MONK,                  N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_MONK,                  N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_MONK,                  N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_MONK,                  N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_MONK,                  N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_MONK,                  N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_MONK,                  N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_MONK,                  N_("Sfx\\Monk\\Monk43.wav"),       nullptr },
	{ sfx_MONK,                  N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_MONK,                  N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_MONK,                  N_("Sfx\\Monk\\Monk46.wav"),       nullptr },
	{ sfx_MONK,                  N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_MONK,                  N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_MONK,                  N_("Sfx\\Monk\\Monk49.wav"),       nullptr },
	{ sfx_MONK,                  N_("Sfx\\Monk\\Monk50.wav"),       nullptr },
	{ sfx_STREAM | sfx_MONK,     N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_STREAM | sfx_MONK,     N_("Sfx\\Monk\\Monk52.wav"),       nullptr },
	{ sfx_STREAM | sfx_MONK,     N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_STREAM | sfx_MONK,     N_("Sfx\\Monk\\Monk54.wav"),       nullptr },
	{ sfx_STREAM | sfx_MONK,     N_("Sfx\\Monk\\Monk55.wav"),       nullptr },
	{ sfx_STREAM | sfx_MONK,     N_("Sfx\\Monk\\Monk56.wav"),       nullptr },
	{ sfx_MONK,                  N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_STREAM | sfx_MONK,     N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_STREAM | sfx_MONK,     N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_STREAM | sfx_MONK,     N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_STREAM | sfx_MONK,     N_("Sfx\\Monk\\Monk61.wav"),       nullptr },
	{ sfx_STREAM | sfx_MONK,     N_("Sfx\\Monk\\Monk62.wav"),       nullptr },
	{ sfx_STREAM | sfx_MONK,     N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_MONK,                  N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_MONK,                  N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_MONK,                  N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_MONK,                  N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_MONK,                  N_("Sfx\\Monk\\Monk68.wav"),       nullptr },
	{ sfx_MONK,                  N_("Sfx\\Monk\\Monk69.wav"),       nullptr },
	{ sfx_MONK,                  N_("Sfx\\Monk\\Monk69b.wav"),      nullptr },
	{ sfx_MONK,                  N_("Sfx\\Monk\\Monk70.wav"),       nullptr },
	{ sfx_MONK,                  N_("Sfx\\Monk\\Monk71.wav"),       nullptr },
	{ sfx_MONK,                  N_("Sfx\\Sorceror\\Mage72.wav"),   nullptr },
	{ sfx_MONK,                  N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_MONK,                  N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_MONK,                  N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_MONK,                  N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_MONK,                  N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_MONK,                  N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_MONK,                  N_("Sfx\\Monk\\Monk79.wav"),       nullptr },
	{ sfx_STREAM | sfx_MONK,     N_("Sfx\\Monk\\Monk80.wav"),       nullptr },
	{ sfx_STREAM | sfx_MONK,     N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_STREAM | sfx_MONK,     N_("Sfx\\Monk\\Monk82.wav"),       nullptr },
	{ sfx_STREAM | sfx_MONK,     N_("Sfx\\Monk\\Monk83.wav"),       nullptr },
	{ sfx_STREAM | sfx_MONK,     N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_STREAM | sfx_MONK,     N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_STREAM | sfx_MONK,     N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_STREAM | sfx_MONK,     N_("Sfx\\Monk\\Monk87.wav"),       nullptr },
	{ sfx_STREAM | sfx_MONK,     N_("Sfx\\Monk\\Monk88.wav"),       nullptr },
	{ sfx_STREAM | sfx_MONK,     N_("Sfx\\Monk\\Monk89.wav"),       nullptr },
	{ sfx_STREAM | sfx_MONK,     N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_STREAM | sfx_MONK,     N_("Sfx\\Monk\\Monk91.wav"),       nullptr },
	{ sfx_STREAM | sfx_MONK,     N_("Sfx\\Monk\\Monk92.wav"),       nullptr },
	{ sfx_STREAM | sfx_MONK,     N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_STREAM | sfx_MONK,     N_("Sfx\\Monk\\Monk94.wav"),       nullptr },
	{ sfx_STREAM | sfx_MONK,     N_("Sfx\\Monk\\Monk95.wav"),       nullptr },
	{ sfx_STREAM | sfx_MONK,     N_("Sfx\\Monk\\Monk96.wav"),       nullptr },
	{ sfx_STREAM | sfx_MONK,     N_("Sfx\\Monk\\Monk97.wav"),       nullptr },
	{ sfx_STREAM | sfx_MONK,     N_("Sfx\\Monk\\Monk98.wav"),       nullptr },
	{ sfx_STREAM | sfx_MONK,     N_("Sfx\\Monk\\Monk99.wav"),       nullptr },
	{ sfx_STREAM | sfx_MONK,     N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_STREAM | sfx_MONK,     N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_STREAM | sfx_MONK,     N_("Sfx\\Misc\\blank.wav"),        nullptr },
	{ sfx_STREAM,                N_("Sfx\\Narrator\\Nar01.wav"),    nullptr },
	{ sfx_STREAM,                N_("Sfx\\Narrator\\Nar02.wav"),    nullptr },
	{ sfx_STREAM,                N_("Sfx\\Narrator\\Nar03.wav"),    nullptr },
	{ sfx_STREAM,                N_("Sfx\\Narrator\\Nar04.wav"),    nullptr },
	{ sfx_STREAM,                N_("Sfx\\Narrator\\Nar05.wav"),    nullptr },
	{ sfx_STREAM,                N_("Sfx\\Narrator\\Nar06.wav"),    nullptr },
	{ sfx_STREAM,                N_("Sfx\\Narrator\\Nar07.wav"),    nullptr },
	{ sfx_STREAM,                N_("Sfx\\Narrator\\Nar08.wav"),    nullptr },
	{ sfx_STREAM,                N_("Sfx\\Narrator\\Nar09.wav"),    nullptr },
	{ sfx_STREAM,                N_("Sfx\\Misc\\Lvl16int.wav"),     nullptr },
	{ sfx_STREAM,                N_("Sfx\\Monsters\\Butcher.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Monsters\\Garbud01.wav"), nullptr },
	{ sfx_STREAM,                N_("Sfx\\Monsters\\Garbud02.wav"), nullptr },
	{ sfx_STREAM,                N_("Sfx\\Monsters\\Garbud03.wav"), nullptr },
	{ sfx_STREAM,                N_("Sfx\\Monsters\\Garbud04.wav"), nullptr },
	{ sfx_STREAM,                N_("Sfx\\Monsters\\Izual01.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Monsters\\Lach01.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Monsters\\Lach02.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Monsters\\Lach03.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Monsters\\Laz01.wav"),    nullptr },
	{ sfx_STREAM,                N_("Sfx\\Monsters\\Laz02.wav"),    nullptr },
	{ sfx_STREAM,                N_("Sfx\\Monsters\\Sking01.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Monsters\\Snot01.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Monsters\\Snot02.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Monsters\\Snot03.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Monsters\\Warlrd01.wav"), nullptr },
	{ sfx_STREAM,                N_("Sfx\\Monsters\\Wlock01.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Monsters\\Zhar01.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Monsters\\Zhar02.wav"),   nullptr },
	{ sfx_STREAM,                N_("Sfx\\Monsters\\DiabloD.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Hellfire\\Farmer1.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Hellfire\\Farmer2.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Hellfire\\Farmer2A.wav"), nullptr },
	{ sfx_STREAM,                N_("Sfx\\Hellfire\\Farmer3.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Hellfire\\Farmer4.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Hellfire\\Farmer5.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Hellfire\\Farmer6.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Hellfire\\Farmer7.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Hellfire\\Farmer8.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Hellfire\\Farmer9.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Hellfire\\TEDDYBR1.wav"), nullptr },
	{ sfx_STREAM,                N_("Sfx\\Hellfire\\TEDDYBR2.wav"), nullptr },
	{ sfx_STREAM,                N_("Sfx\\Hellfire\\TEDDYBR3.wav"), nullptr },
	{ sfx_STREAM,                N_("Sfx\\Hellfire\\TEDDYBR4.wav"), nullptr },
	{ sfx_STREAM,                N_("Sfx\\Hellfire\\DEFILER1.wav"), nullptr },
	{ sfx_STREAM,                N_("Sfx\\Hellfire\\DEFILER2.wav"), nullptr },
	{ sfx_STREAM,                N_("Sfx\\Hellfire\\DEFILER3.wav"), nullptr },
	{ sfx_STREAM,                N_("Sfx\\Hellfire\\DEFILER4.wav"), nullptr },
	{ sfx_STREAM,                N_("Sfx\\Hellfire\\DEFILER8.wav"), nullptr },
	{ sfx_STREAM,                N_("Sfx\\Hellfire\\DEFILER6.wav"), nullptr },
	{ sfx_STREAM,                N_("Sfx\\Hellfire\\DEFILER7.wav"), nullptr },
	{ sfx_STREAM,                N_("Sfx\\Hellfire\\NAKRUL1.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Hellfire\\NAKRUL2.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Hellfire\\NAKRUL3.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Hellfire\\NAKRUL4.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Hellfire\\NAKRUL5.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Hellfire\\NAKRUL6.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Hellfire\\NARATR3.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Hellfire\\COWSUT1.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Hellfire\\COWSUT2.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Hellfire\\COWSUT3.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Hellfire\\COWSUT4.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Hellfire\\COWSUT4A.wav"), nullptr },
	{ sfx_STREAM,                N_("Sfx\\Hellfire\\COWSUT5.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Hellfire\\COWSUT6.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Hellfire\\COWSUT7.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Hellfire\\COWSUT8.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Hellfire\\COWSUT9.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Hellfire\\COWSUT10.wav"), nullptr },
	{ sfx_STREAM,                N_("Sfx\\Hellfire\\COWSUT11.wav"), nullptr },
	{ sfx_STREAM,                N_("Sfx\\Hellfire\\COWSUT12.wav"), nullptr },
	{ sfx_STREAM,                N_("Sfx\\Hellfire\\Skljrn1.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Hellfire\\Naratr6.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Hellfire\\Naratr7.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Hellfire\\Naratr8.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Hellfire\\Naratr5.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Hellfire\\Naratr9.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Hellfire\\Naratr4.wav"),  nullptr },
	{ sfx_STREAM,                N_("Sfx\\Hellfire\\TRADER1.wav"),  nullptr },
	{ sfx_MISC | sfx_HELLFIRE,   N_("Sfx\\Items\\Cropen.wav"),      nullptr },
	{ sfx_MISC | sfx_HELLFIRE,   N_("Sfx\\Items\\Crclos.wav"),      nullptr },
	// clang-format on
};

bool effect_is_playing(int nSFX)
{
	TSFX *sfx = &sgSFX[nSFX];
	if (sfx->pSnd != nullptr)
		return snd_playing(sfx->pSnd);

	if ((sfx->bFlags & sfx_STREAM) != 0)
		return sfx == sgpStreamSFX;

	return false;
}

void stream_stop()
{
	if (sgpStreamSFX != nullptr) {
		sgpStreamSFX->pSnd->DSB->Stop();
		sound_file_cleanup(sgpStreamSFX->pSnd);
		sgpStreamSFX->pSnd = nullptr;
		sgpStreamSFX = nullptr;
	}
}

static void stream_play(TSFX *pSFX, int lVolume, int lPan)
{
	assert(pSFX);
	assert(pSFX->bFlags & sfx_STREAM);
	stream_stop();
	lVolume += sound_get_or_set_sound_volume(1);
	if (lVolume >= VOLUME_MIN) {
		if (lVolume > VOLUME_MAX)
			lVolume = VOLUME_MAX;
		if (pSFX->pSnd == nullptr)
			pSFX->pSnd = sound_file_load(pSFX->pszName, AllowStreaming);
		pSFX->pSnd->DSB->Play(lVolume, lPan, 0);
		sgpStreamSFX = pSFX;
	}
}

static void stream_update()
{
	if (sgpStreamSFX != nullptr && !sgpStreamSFX->pSnd->DSB->IsPlaying()) {
		stream_stop();
	}
}

void InitMonsterSND(int monst)
{
	TSnd *pSnd;
	char name[MAX_PATH];
	char *path;
	int mtype, i, j;

	if (!gbSndInited) {
		return;
	}

	mtype = Monsters[monst].mtype;
	for (i = 0; i < 4; i++) {
		if (MonstSndChar[i] != 's' || monsterdata[mtype].snd_special) {
			for (j = 0; j < 2; j++) {
				sprintf(name, monsterdata[mtype].sndfile, MonstSndChar[i], j + 1);
				path = (char *)DiabloAllocPtr(strlen(name) + 1);
				strcpy(path, name);
				pSnd = sound_file_load(path);
				Monsters[monst].Snds[i][j] = pSnd;
				if (pSnd == nullptr)
					mem_free_dbg(path);
			}
		}
	}
}

void FreeMonsterSnd()
{
	int i, j, k;
	const char *file;
	TSnd *pSnd;

	for (i = 0; i < nummtypes; i++) {
		for (j = 0; j < 4; ++j) {
			for (k = 0; k < 2; ++k) {
				pSnd = Monsters[i].Snds[j][k];
				if (pSnd != nullptr) {
					Monsters[i].Snds[j][k] = nullptr;
					file = pSnd->sound_path;
					pSnd->sound_path = nullptr;
					sound_file_cleanup(pSnd);

					// pSnd->sound_path is malloc'd (but only for monsters).
					mem_free_dbg(const_cast<char *>(file));
				}
			}
		}
	}
}

bool calc_snd_position(int x, int y, int *plVolume, int *plPan)
{
	int pan, volume;

	x -= plr[myplr].position.tile.x;
	y -= plr[myplr].position.tile.y;

	pan = (x - y) * 256;
	*plPan = pan;

	if (abs(pan) > 6400)
		return false;

	volume = abs(x) > abs(y) ? abs(x) : abs(y);
	volume *= 64;
	*plVolume = volume;

	if (volume >= 6400)
		return false;

	*plVolume = -volume;

	return true;
}

static void PlaySFX_priv(TSFX *pSFX, bool loc, int x, int y)
{
	int lPan, lVolume;

	if (plr[myplr].pLvlLoad && gbIsMultiplayer) {
		return;
	}
	if (!gbSndInited || !gbSoundOn || gbBufferMsgs) {
		return;
	}

	if ((pSFX->bFlags & (sfx_STREAM | sfx_MISC)) == 0 && pSFX->pSnd != nullptr && snd_playing(pSFX->pSnd)) {
		return;
	}

	lPan = 0;
	lVolume = 0;
	if (loc && !calc_snd_position(x, y, &lVolume, &lPan)) {
		return;
	}

	if ((pSFX->bFlags & sfx_STREAM) != 0) {
		stream_play(pSFX, lVolume, lPan);
		return;
	}

	if (pSFX->pSnd == nullptr)
		pSFX->pSnd = sound_file_load(pSFX->pszName);

	if (pSFX->pSnd != nullptr)
		snd_play_snd(pSFX->pSnd, lVolume, lPan);
}

void PlayEffect(int i, int mode)
{
	int sndIdx, mi, lVolume, lPan;
	TSnd *snd;

	if (plr[myplr].pLvlLoad) {
		return;
	}

	sndIdx = GenerateRnd(2);
	if (!gbSndInited || !gbSoundOn || gbBufferMsgs) {
		return;
	}

	mi = monster[i]._mMTidx;
	snd = Monsters[mi].Snds[mode][sndIdx];
	if (snd == nullptr || snd_playing(snd)) {
		return;
	}

	if (!calc_snd_position(monster[i].position.tile.x, monster[i].position.tile.y, &lVolume, &lPan))
		return;

	snd_play_snd(snd, lVolume, lPan);
}

static _sfx_id RndSFX(_sfx_id psfx)
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

void PlaySFX(_sfx_id psfx)
{
	psfx = RndSFX(psfx);

	PlaySFX_priv(&sgSFX[psfx], false, 0, 0);
}

void PlaySfxLoc(_sfx_id psfx, int x, int y, bool randomizeByCategory)
{
	TSnd *pSnd;

	if (randomizeByCategory) {
		psfx = RndSFX(psfx);
	}

	if (psfx >= 0 && psfx <= 3) {
		pSnd = sgSFX[psfx].pSnd;
		if (pSnd != nullptr)
			pSnd->start_tc = 0;
	}

	PlaySFX_priv(&sgSFX[psfx], true, x, y);
}

void sound_stop()
{
	Mix_HaltChannel(-1);
}

void sound_update()
{
	if (!gbSndInited) {
		return;
	}

	stream_update();
}

void effects_cleanup_sfx()
{
	DWORD i;

	sound_stop();

	for (i = 0; i < sizeof(sgSFX) / sizeof(TSFX); i++) {
		if (sgSFX[i].pSnd != nullptr) {
			sound_file_cleanup(sgSFX[i].pSnd);
			sgSFX[i].pSnd = nullptr;
		}
	}
}

static void priv_sound_init(BYTE bLoadMask)
{
	DWORD i;

	if (!gbSndInited) {
		return;
	}

	for (i = 0; i < sizeof(sgSFX) / sizeof(TSFX); i++) {
		if (sgSFX[i].pSnd != nullptr) {
			continue;
		}

		if ((sgSFX[i].bFlags & sfx_STREAM) != 0) {
			continue;
		}

		if ((sgSFX[i].bFlags & bLoadMask) == 0) {
			continue;
		}

		if (!gbIsHellfire && (sgSFX[i].bFlags & sfx_HELLFIRE) != 0) {
			continue;
		}

		sgSFX[i].pSnd = sound_file_load(sgSFX[i].pszName);
	}
}

void sound_init()
{
	BYTE mask = sfx_MISC;
	if (gbIsMultiplayer) {
		mask |= sfx_WARRIOR;
		if (!gbIsSpawn)
			mask |= (sfx_ROGUE | sfx_SORCERER);
		if (gbIsHellfire)
			mask |= sfx_MONK;
	} else if (plr[myplr]._pClass == HeroClass::Warrior) {
		mask |= sfx_WARRIOR;
	} else if (plr[myplr]._pClass == HeroClass::Rogue) {
		mask |= sfx_ROGUE;
	} else if (plr[myplr]._pClass == HeroClass::Sorcerer) {
		mask |= sfx_SORCERER;
	} else if (plr[myplr]._pClass == HeroClass::Monk) {
		mask |= sfx_MONK;
	} else if (plr[myplr]._pClass == HeroClass::Bard) {
		mask |= sfx_ROGUE;
	} else if (plr[myplr]._pClass == HeroClass::Barbarian) {
		mask |= sfx_WARRIOR;
	} else {
		app_fatal(_("effects:1"));
	}

	priv_sound_init(mask);
}

void ui_sound_init()
{
	priv_sound_init(sfx_UI);
}

void effects_play_sound(const char *snd_file)
{
	DWORD i;

	if (!gbSndInited || !gbSoundOn) {
		return;
	}

	for (i = 0; i < sizeof(sgSFX) / sizeof(TSFX); i++) {
		if (strcasecmp(sgSFX[i].pszName, snd_file) == 0 && sgSFX[i].pSnd != nullptr) {
			if (!snd_playing(sgSFX[i].pSnd))
				snd_play_snd(sgSFX[i].pSnd, 0, 0);

			return;
		}
	}
}

int GetSFXLength(int nSFX)
{
	if (sgSFX[nSFX].pSnd == nullptr)
		sgSFX[nSFX].pSnd = sound_file_load(sgSFX[nSFX].pszName,
		    /*stream=*/AllowStreaming && (sgSFX[nSFX].bFlags & sfx_STREAM) != 0);
	return sgSFX[nSFX].pSnd->DSB->GetLength();
}

} // namespace devilution
