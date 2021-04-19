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

int sfxdelay;
_sfx_id sfxdnum = SFX_NONE;
/** Specifies the sound file and the playback state of the current sound effect. */
static TSFX *sgpStreamSFX = NULL;

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
	{ sfx_MISC,                  "Sfx\\Misc\\Walk1.wav",        NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Walk2.wav",        NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Walk3.wav",        NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Walk4.wav",        NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\BFire.wav",        NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Fmag.wav",         NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Tmag.wav",         NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Lghit.wav",        NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Lghit1.wav",       NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Swing.wav",        NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Swing2.wav",       NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Dead.wav",         NULL },
	{ sfx_MISC | sfx_HELLFIRE,   "Sfx\\Misc\\Sting1.wav",       NULL },
	{ sfx_MISC | sfx_HELLFIRE,   "Sfx\\Misc\\FBallBow.wav",     NULL },
	{ sfx_STREAM,                "Sfx\\Misc\\Questdon.wav",     NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Armrfkd.wav",     NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Barlfire.wav",    NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Barrel.wav",      NULL },
	{ sfx_MISC | sfx_HELLFIRE,   "Sfx\\Items\\PodPop8.wav",     NULL },
	{ sfx_MISC | sfx_HELLFIRE,   "Sfx\\Items\\PodPop5.wav",     NULL },
	{ sfx_MISC | sfx_HELLFIRE,   "Sfx\\Items\\UrnPop3.wav",     NULL },
	{ sfx_MISC | sfx_HELLFIRE,   "Sfx\\Items\\UrnPop2.wav",     NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Bhit.wav",        NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Bhit1.wav",       NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Chest.wav",       NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Doorclos.wav",    NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Dooropen.wav",    NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Flipanvl.wav",    NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Flipaxe.wav",     NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Flipblst.wav",    NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Flipbody.wav",    NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Flipbook.wav",    NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Flipbow.wav",     NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Flipcap.wav",     NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Flipharm.wav",    NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Fliplarm.wav",    NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Flipmag.wav",     NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Flipmag1.wav",    NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Flipmush.wav",    NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Flippot.wav",     NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Flipring.wav",    NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Fliprock.wav",    NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Flipscrl.wav",    NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Flipshld.wav",    NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Flipsign.wav",    NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Flipstaf.wav",    NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Flipswor.wav",    NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Gold.wav",        NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Hlmtfkd.wav",     NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Invanvl.wav",     NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Invaxe.wav",      NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Invblst.wav",     NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Invbody.wav",     NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Invbook.wav",     NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Invbow.wav",      NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Invcap.wav",      NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Invgrab.wav",     NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Invharm.wav",     NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Invlarm.wav",     NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Invmush.wav",     NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Invpot.wav",      NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Invring.wav",     NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Invrock.wav",     NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Invscrol.wav",    NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Invshiel.wav",    NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Invsign.wav",     NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Invstaf.wav",     NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Invsword.wav",    NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Lever.wav",       NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Magic.wav",       NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Magic1.wav",      NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Readbook.wav",    NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Sarc.wav",        NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Shielfkd.wav",    NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Swrdfkd.wav",     NULL },
	{ sfx_UI,                    "Sfx\\Items\\Titlemov.wav",    NULL },
	{ sfx_UI,                    "Sfx\\Items\\Titlslct.wav",    NULL },
	{ sfx_UI,                    "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_MISC,                  "Sfx\\Items\\Trap.wav",        NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Cast1.wav",        NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Cast10.wav",       NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Cast12.wav",       NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Cast2.wav",        NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Cast3.wav",        NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Cast4.wav",        NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Cast5.wav",        NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Cast6.wav",        NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Cast7.wav",        NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Cast8.wav",        NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Cast9.wav",        NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Healing.wav",      NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Repair.wav",       NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Acids1.wav",       NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Acids2.wav",       NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Apoc.wav",         NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Arrowall.wav",     NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Bldboil.wav",      NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Blodstar.wav",     NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Blsimpt.wav",      NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Bonesp.wav",       NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Bsimpct.wav",      NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Caldron.wav",      NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Cbolt.wav",        NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Chltning.wav",     NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\DSerp.wav",        NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Elecimp1.wav",     NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Elementl.wav",     NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Ethereal.wav",     NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Fball.wav",        NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Fbolt1.wav",       NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Fbolt2.wav",       NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Firimp1.wav",      NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Firimp2.wav",      NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Flamwave.wav",     NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Flash.wav",        NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Fountain.wav",     NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Golum.wav",        NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Golumded.wav",     NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Gshrine.wav",      NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Guard.wav",        NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Grdlanch.wav",     NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Holybolt.wav",     NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Hyper.wav",        NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Infravis.wav",     NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Invisibl.wav",     NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Invpot.wav",       NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Lning1.wav",       NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Ltning.wav",       NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Mshield.wav",      NULL },
	{ sfx_MISC | sfx_HELLFIRE,   "Sfx\\Misc\\NestXpld.wav",     NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Nova.wav",         NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Portal.wav",       NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Puddle.wav",       NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Resur.wav",        NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Scurse.wav",       NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Scurimp.wav",      NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Sentinel.wav",     NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Shatter.wav",      NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Soulfire.wav",     NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Spoutlop.wav",     NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Spoutstr.wav",     NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Storm.wav",        NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Trapdis.wav",      NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Teleport.wav",     NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Vtheft.wav",       NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Wallloop.wav",     NULL },
	{ sfx_MISC,                  "Sfx\\Misc\\Wallstrt.wav",     NULL },
	{ sfx_MISC | sfx_HELLFIRE,   "Sfx\\Misc\\LMag.wav",         NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid01.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid02.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid03.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid04.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid05.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid06.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid07.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid08.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid09.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid10.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid11.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid12.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid13.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid14.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid15.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid16.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid17.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid18.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid19.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid20.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid21.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid22.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid23.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid24.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid25.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid26.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid27.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid28.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid29.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid30.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid31.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid32.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid33.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid34.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid35.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid36.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid37.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid38.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid39.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bmaid40.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith01.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith02.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith03.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith04.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith05.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith06.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith07.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith08.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith09.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith10.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith11.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith12.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith13.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith14.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith15.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith16.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith17.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith18.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith19.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith20.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith21.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith22.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith23.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith24.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith25.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith26.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith27.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith28.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith29.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith30.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith31.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith32.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith33.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith34.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith35.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith36.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith37.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith38.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith39.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith40.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith41.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith42.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith43.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith44.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith45.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith46.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith47.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith48.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith49.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith50.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith51.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith52.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith53.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith54.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith55.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Bsmith56.wav",  NULL },
	{ sfx_MISC,                  "Sfx\\Towners\\Cow1.wav",      NULL },
	{ sfx_MISC,                  "Sfx\\Towners\\Cow2.wav",      NULL },
/*
	{ sfx_MISC,                  "Sfx\\Towners\\Cow3.wav",      NULL },
	{ sfx_MISC,                  "Sfx\\Towners\\Cow4.wav",      NULL },
	{ sfx_MISC,                  "Sfx\\Towners\\Cow5.wav",      NULL },
	{ sfx_MISC,                  "Sfx\\Towners\\Cow6.wav",      NULL },
*/
	{ sfx_MISC | sfx_HELLFIRE,   "Sfx\\Towners\\Cow7.wav",      NULL },
	{ sfx_MISC | sfx_HELLFIRE,   "Sfx\\Towners\\Cow8.wav",      NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Deadguy2.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk01.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk02.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk03.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk04.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk05.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk06.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk07.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk08.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk09.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk10.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk11.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk12.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk13.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk14.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk15.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk16.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk17.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk18.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk19.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk20.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk21.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk22.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk23.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk24.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk25.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk26.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk27.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk28.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk29.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk30.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk31.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk32.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk33.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk34.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Drunk35.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Healer01.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Healer02.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Healer03.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Healer04.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Healer05.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Healer06.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Healer07.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Healer08.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Healer09.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Healer10.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Healer11.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Healer12.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Healer13.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Healer14.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Healer15.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Healer16.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Healer17.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Healer18.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Healer19.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Healer20.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Healer21.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Healer22.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Healer23.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Healer24.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Healer25.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Healer26.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Healer27.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Healer28.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Healer29.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Healer30.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Healer31.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Healer32.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Healer33.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Healer34.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Healer35.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Healer36.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Healer37.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Healer38.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Healer39.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Healer40.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Healer41.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Healer42.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Healer43.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Healer44.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Healer45.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Healer46.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Healer47.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy01.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy02.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy03.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy04.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy05.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy06.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy07.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy08.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy09.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy10.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy11.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy12.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy13.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy14.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy15.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy16.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy17.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy18.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy19.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy20.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy21.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy22.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy23.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy24.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy25.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy26.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy27.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy28.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy29.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy30.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy31.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy32.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy33.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy34.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy35.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy36.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy37.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy38.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy39.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy40.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy41.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy42.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Pegboy43.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Priest00.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Priest01.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Priest02.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Priest03.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Priest04.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Priest05.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Priest06.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Priest07.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt00.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt01.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt02.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt03.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt04.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt05.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt06.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt07.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt08.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt09.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt10.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt11.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt12.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt13.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt14.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt15.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt16.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt17.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt18.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt19.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt20.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt21.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt22.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt23.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt24.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt25.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt26.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt27.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt28.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt29.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt30.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt31.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt32.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt33.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt34.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt35.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt36.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt37.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Storyt38.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown00.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown01.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown02.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown03.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown04.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown05.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown06.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown07.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown08.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown09.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown10.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown11.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown12.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown13.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown14.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown15.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown16.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown17.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown18.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown19.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown20.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown21.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown22.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown23.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown24.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown25.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown26.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown27.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown28.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown29.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown30.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown31.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown32.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown33.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown34.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown35.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown36.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown37.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown38.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown39.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown40.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown41.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown42.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown43.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown44.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Tavown45.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Witch01.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Witch02.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Witch03.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Witch04.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Witch05.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Witch06.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Witch07.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Witch08.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Witch09.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Witch10.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Witch11.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Witch12.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Witch13.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Witch14.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Witch15.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Witch16.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Witch17.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Witch18.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Witch19.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Witch20.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Witch21.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Witch22.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Witch23.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Witch24.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Witch25.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Witch26.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Witch27.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Witch28.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Witch29.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Witch30.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Witch31.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Witch32.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Witch33.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Witch34.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Witch35.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Witch36.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Witch37.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Witch38.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Witch39.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Witch40.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Witch41.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Witch42.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Witch43.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Witch44.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Witch45.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Witch46.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Witch47.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Witch48.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Witch49.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Witch50.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Towners\\Wound01.wav",   NULL },
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage01.wav",   NULL },
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage02.wav",   NULL },
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage03.wav",   NULL },
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage04.wav",   NULL },
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage05.wav",   NULL },
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage06.wav",   NULL },
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage07.wav",   NULL },
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage08.wav",   NULL },
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage09.wav",   NULL },
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage10.wav",   NULL },
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage11.wav",   NULL },
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage12.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage13.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage14.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage15.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage16.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage17.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage18.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage19.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage20.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage21.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage22.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage23.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage24.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage25.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage26.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage27.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage28.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage29.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage30.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage31.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage32.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage33.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage34.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage35.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage36.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage37.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage38.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage39.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage40.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage41.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage42.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage43.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage44.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage45.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage46.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage47.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage48.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage49.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage50.wav",   NULL },
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage51.wav",   NULL },
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage52.wav",   NULL },
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage53.wav",   NULL },
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage54.wav",   NULL },
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage55.wav",   NULL },
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage56.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage57.wav",   NULL },
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage58.wav",   NULL },
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage59.wav",   NULL },
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage60.wav",   NULL },
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage61.wav",   NULL },
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage62.wav",   NULL },
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage63.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage64.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage65.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage66.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage67.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage68.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage69.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage69b.wav",  NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage70.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage71.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage72.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage73.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage74.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage75.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage76.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage77.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage78.wav",   NULL },
	{ sfx_SORCERER,              "Sfx\\Sorceror\\Mage79.wav",   NULL },
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage80.wav",   NULL },
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage81.wav",   NULL },
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage82.wav",   NULL },
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage83.wav",   NULL },
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage84.wav",   NULL },
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage85.wav",   NULL },
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage86.wav",   NULL },
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage87.wav",   NULL },
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage88.wav",   NULL },
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage89.wav",   NULL },
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage90.wav",   NULL },
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage91.wav",   NULL },
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage92.wav",   NULL },
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage93.wav",   NULL },
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage94.wav",   NULL },
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage95.wav",   NULL },
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage96.wav",   NULL },
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage97.wav",   NULL },
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage98.wav",   NULL },
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage99.wav",   NULL },
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage100.wav",  NULL },
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage101.wav",  NULL },
	{ sfx_STREAM | sfx_SORCERER, "Sfx\\Sorceror\\Mage102.wav",  NULL },
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue01.wav",     NULL },
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue02.wav",     NULL },
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue03.wav",     NULL },
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue04.wav",     NULL },
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue05.wav",     NULL },
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue06.wav",     NULL },
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue07.wav",     NULL },
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue08.wav",     NULL },
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue09.wav",     NULL },
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue10.wav",     NULL },
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue11.wav",     NULL },
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue12.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue13.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue14.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue15.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue16.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue17.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue18.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue19.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue20.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue21.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue22.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue23.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue24.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue25.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue26.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue27.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue28.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue29.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue30.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue31.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue32.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue33.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue34.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue35.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue36.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue37.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue38.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue39.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue40.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue41.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue42.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue43.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue44.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue45.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue46.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue47.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue48.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue49.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue50.wav",     NULL },
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue51.wav",     NULL },
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue52.wav",     NULL },
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue53.wav",     NULL },
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue54.wav",     NULL },
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue55.wav",     NULL },
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue56.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue57.wav",     NULL },
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue58.wav",     NULL },
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue59.wav",     NULL },
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue60.wav",     NULL },
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue61.wav",     NULL },
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue62.wav",     NULL },
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue63.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue64.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue65.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue66.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue67.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue68.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue69.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue69b.wav",    NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue70.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue71.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue72.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue73.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue74.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue75.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue76.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue77.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue78.wav",     NULL },
	{ sfx_ROGUE,                 "Sfx\\Rogue\\Rogue79.wav",     NULL },
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue80.wav",     NULL },
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue81.wav",     NULL },
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue82.wav",     NULL },
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue83.wav",     NULL },
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue84.wav",     NULL },
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue85.wav",     NULL },
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue86.wav",     NULL },
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue87.wav",     NULL },
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue88.wav",     NULL },
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue89.wav",     NULL },
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue90.wav",     NULL },
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue91.wav",     NULL },
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue92.wav",     NULL },
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue93.wav",     NULL },
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue94.wav",     NULL },
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue95.wav",     NULL },
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue96.wav",     NULL },
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue97.wav",     NULL },
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue98.wav",     NULL },
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue99.wav",     NULL },
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue100.wav",    NULL },
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue101.wav",    NULL },
	{ sfx_STREAM | sfx_ROGUE,    "Sfx\\Rogue\\Rogue102.wav",    NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior01.wav",  NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior02.wav",  NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior03.wav",  NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior04.wav",  NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior05.wav",  NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior06.wav",  NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior07.wav",  NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior08.wav",  NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior09.wav",  NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior10.wav",  NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior11.wav",  NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior12.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior13.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior14.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Wario14b.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Wario14c.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior15.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Wario15b.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Wario15c.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior16.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Wario16b.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Wario16c.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior17.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior18.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior19.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior20.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior21.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior22.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior23.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior24.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior25.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior26.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior27.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior28.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior29.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior30.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior31.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior32.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior33.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior34.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior35.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior36.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior37.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior38.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior39.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior40.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior41.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior42.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior43.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior44.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior45.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior46.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior47.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior48.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior49.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior50.wav",  NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior51.wav",  NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior52.wav",  NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior53.wav",  NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior54.wav",  NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior55.wav",  NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior56.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior57.wav",  NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior58.wav",  NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior59.wav",  NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior60.wav",  NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior61.wav",  NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior62.wav",  NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior63.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior64.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior65.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior66.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior67.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior68.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior69.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Wario69b.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior70.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior71.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior72.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior73.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior74.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior75.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior76.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior77.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior78.wav",  NULL },
	{ sfx_WARRIOR,               "Sfx\\Warrior\\Warior79.wav",  NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior80.wav",  NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior81.wav",  NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior82.wav",  NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior83.wav",  NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior84.wav",  NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior85.wav",  NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior86.wav",  NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior87.wav",  NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior88.wav",  NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior89.wav",  NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior90.wav",  NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior91.wav",  NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior92.wav",  NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior93.wav",  NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior94.wav",  NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior95.wav",  NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Wario95b.wav",  NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Wario95c.wav",  NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Wario95d.wav",  NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Wario95e.wav",  NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Wario95f.wav",  NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Wario96b.wav",  NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Wario97.wav",   NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Wario98.wav",   NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Warior99.wav",  NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Wario100.wav",  NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Wario101.wav",  NULL },
	{ sfx_STREAM | sfx_WARRIOR,  "Sfx\\Warrior\\Wario102.wav",  NULL },
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk01.wav",       NULL },
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk08.wav",       NULL },
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk09.wav",       NULL },
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk10.wav",       NULL },
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk11.wav",       NULL },
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk12.wav",       NULL },
	{ sfx_MONK,                  "Sfx\\Monk\\Monk13.wav",       NULL },
	{ sfx_MONK,                  "Sfx\\Monk\\Monk14.wav",       NULL },
	{ sfx_MONK,                  "Sfx\\Monk\\Monk15.wav",       NULL },
	{ sfx_MONK,                  "Sfx\\Monk\\Monk16.wav",       NULL },
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_MONK,                  "Sfx\\Monk\\Monk24.wav",       NULL },
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_MONK,                  "Sfx\\Monk\\Monk27.wav",       NULL },
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_MONK,                  "Sfx\\Monk\\Monk29.wav",       NULL },
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_MONK,                  "Sfx\\Monk\\Monk34.wav",       NULL },
	{ sfx_MONK,                  "Sfx\\Monk\\Monk35.wav",       NULL },
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_MONK,                  "Sfx\\Monk\\Monk43.wav",       NULL },
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_MONK,                  "Sfx\\Monk\\Monk46.wav",       NULL },
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_MONK,                  "Sfx\\Monk\\Monk49.wav",       NULL },
	{ sfx_MONK,                  "Sfx\\Monk\\Monk50.wav",       NULL },
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk52.wav",       NULL },
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk54.wav",       NULL },
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk55.wav",       NULL },
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk56.wav",       NULL },
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk61.wav",       NULL },
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk62.wav",       NULL },
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_MONK,                  "Sfx\\Monk\\Monk68.wav",       NULL },
	{ sfx_MONK,                  "Sfx\\Monk\\Monk69.wav",       NULL },
	{ sfx_MONK,                  "Sfx\\Monk\\Monk69b.wav",      NULL },
	{ sfx_MONK,                  "Sfx\\Monk\\Monk70.wav",       NULL },
	{ sfx_MONK,                  "Sfx\\Monk\\Monk71.wav",       NULL },
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_MONK,                  "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_MONK,                  "Sfx\\Monk\\Monk79.wav",       NULL },
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk80.wav",       NULL },
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk82.wav",       NULL },
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk83.wav",       NULL },
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk87.wav",       NULL },
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk88.wav",       NULL },
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk89.wav",       NULL },
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk91.wav",       NULL },
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk92.wav",       NULL },
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk94.wav",       NULL },
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk95.wav",       NULL },
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk96.wav",       NULL },
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk97.wav",       NULL },
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk98.wav",       NULL },
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Monk\\Monk99.wav",       NULL },
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_STREAM | sfx_MONK,     "Sfx\\Misc\\blank.wav",        NULL },
	{ sfx_STREAM,                "Sfx\\Narrator\\Nar01.wav",    NULL },
	{ sfx_STREAM,                "Sfx\\Narrator\\Nar02.wav",    NULL },
	{ sfx_STREAM,                "Sfx\\Narrator\\Nar03.wav",    NULL },
	{ sfx_STREAM,                "Sfx\\Narrator\\Nar04.wav",    NULL },
	{ sfx_STREAM,                "Sfx\\Narrator\\Nar05.wav",    NULL },
	{ sfx_STREAM,                "Sfx\\Narrator\\Nar06.wav",    NULL },
	{ sfx_STREAM,                "Sfx\\Narrator\\Nar07.wav",    NULL },
	{ sfx_STREAM,                "Sfx\\Narrator\\Nar08.wav",    NULL },
	{ sfx_STREAM,                "Sfx\\Narrator\\Nar09.wav",    NULL },
	{ sfx_STREAM,                "Sfx\\Misc\\Lvl16int.wav",     NULL },
	{ sfx_STREAM,                "Sfx\\Monsters\\Butcher.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Monsters\\Garbud01.wav", NULL },
	{ sfx_STREAM,                "Sfx\\Monsters\\Garbud02.wav", NULL },
	{ sfx_STREAM,                "Sfx\\Monsters\\Garbud03.wav", NULL },
	{ sfx_STREAM,                "Sfx\\Monsters\\Garbud04.wav", NULL },
	{ sfx_STREAM,                "Sfx\\Monsters\\Izual01.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Monsters\\Lach01.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Monsters\\Lach02.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Monsters\\Lach03.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Monsters\\Laz01.wav",    NULL },
	{ sfx_STREAM,                "Sfx\\Monsters\\Laz02.wav",    NULL },
	{ sfx_STREAM,                "Sfx\\Monsters\\Sking01.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Monsters\\Snot01.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Monsters\\Snot02.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Monsters\\Snot03.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Monsters\\Warlrd01.wav", NULL },
	{ sfx_STREAM,                "Sfx\\Monsters\\Wlock01.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Monsters\\Zhar01.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Monsters\\Zhar02.wav",   NULL },
	{ sfx_STREAM,                "Sfx\\Monsters\\DiabloD.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Hellfire\\Farmer1.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Hellfire\\Farmer2.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Hellfire\\Farmer2A.wav", NULL },
	{ sfx_STREAM,                "Sfx\\Hellfire\\Farmer3.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Hellfire\\Farmer4.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Hellfire\\Farmer5.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Hellfire\\Farmer6.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Hellfire\\Farmer7.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Hellfire\\Farmer8.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Hellfire\\Farmer9.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Hellfire\\TEDDYBR1.wav", NULL },
	{ sfx_STREAM,                "Sfx\\Hellfire\\TEDDYBR2.wav", NULL },
	{ sfx_STREAM,                "Sfx\\Hellfire\\TEDDYBR3.wav", NULL },
	{ sfx_STREAM,                "Sfx\\Hellfire\\TEDDYBR4.wav", NULL },
	{ sfx_STREAM,                "Sfx\\Hellfire\\DEFILER1.wav", NULL },
	{ sfx_STREAM,                "Sfx\\Hellfire\\DEFILER2.wav", NULL },
	{ sfx_STREAM,                "Sfx\\Hellfire\\DEFILER3.wav", NULL },
	{ sfx_STREAM,                "Sfx\\Hellfire\\DEFILER4.wav", NULL },
	{ sfx_STREAM,                "Sfx\\Hellfire\\DEFILER8.wav", NULL },
	{ sfx_STREAM,                "Sfx\\Hellfire\\DEFILER6.wav", NULL },
	{ sfx_STREAM,                "Sfx\\Hellfire\\DEFILER7.wav", NULL },
	{ sfx_STREAM,                "Sfx\\Hellfire\\NAKRUL1.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Hellfire\\NAKRUL2.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Hellfire\\NAKRUL3.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Hellfire\\NAKRUL4.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Hellfire\\NAKRUL5.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Hellfire\\NAKRUL6.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Hellfire\\NARATR3.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Hellfire\\COWSUT1.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Hellfire\\COWSUT2.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Hellfire\\COWSUT3.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Hellfire\\COWSUT4.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Hellfire\\COWSUT4A.wav", NULL },
	{ sfx_STREAM,                "Sfx\\Hellfire\\COWSUT5.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Hellfire\\COWSUT6.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Hellfire\\COWSUT7.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Hellfire\\COWSUT8.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Hellfire\\COWSUT9.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Hellfire\\COWSUT10.wav", NULL },
	{ sfx_STREAM,                "Sfx\\Hellfire\\COWSUT11.wav", NULL },
	{ sfx_STREAM,                "Sfx\\Hellfire\\COWSUT12.wav", NULL },
	{ sfx_STREAM,                "Sfx\\Hellfire\\Skljrn1.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Hellfire\\Naratr6.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Hellfire\\Naratr7.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Hellfire\\Naratr8.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Hellfire\\Naratr5.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Hellfire\\Naratr9.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Hellfire\\Naratr4.wav",  NULL },
	{ sfx_STREAM,                "Sfx\\Hellfire\\TRADER1.wav",  NULL },
	{ sfx_MISC | sfx_HELLFIRE,   "Sfx\\Items\\Cropen.wav",      NULL },
	{ sfx_MISC | sfx_HELLFIRE,   "Sfx\\Items\\Crclos.wav",      NULL },
	// clang-format on
};

_sfx_id herosounds[enum_size<HeroClass>::value][102] = {
	{ PS_WARR1, PS_WARR2, PS_WARR3, PS_WARR4, PS_WARR5, PS_WARR6, PS_WARR7, PS_WARR8, PS_WARR9, PS_WARR10, PS_WARR11, PS_WARR12, PS_WARR13, PS_WARR14, PS_WARR15, PS_WARR16, PS_WARR17, PS_WARR18, PS_WARR19, PS_WARR20, PS_WARR21, PS_WARR22, PS_WARR23, PS_WARR24, PS_WARR25, PS_WARR26, PS_WARR27, PS_WARR28, PS_WARR29, PS_WARR30, PS_WARR31, PS_WARR32, PS_WARR33, PS_WARR34, PS_WARR35, PS_WARR36, PS_WARR37, PS_WARR38, PS_WARR39, PS_WARR40, PS_WARR41, PS_WARR42, PS_WARR43, PS_WARR44, PS_WARR45, PS_WARR46, PS_WARR47, PS_WARR48, PS_WARR49, PS_WARR50, PS_WARR51, PS_WARR52, PS_WARR53, PS_WARR54, PS_WARR55, PS_WARR56, PS_WARR57, PS_WARR58, PS_WARR59, PS_WARR60, PS_WARR61, PS_WARR62, PS_WARR63, PS_WARR64, PS_WARR65, PS_WARR66, PS_WARR67, PS_WARR68, PS_WARR69, PS_WARR70, PS_WARR71, PS_WARR72, PS_WARR73, PS_WARR74, PS_WARR75, PS_WARR76, PS_WARR77, PS_WARR78, PS_WARR79, PS_WARR80, PS_WARR81, PS_WARR82, PS_WARR83, PS_WARR84, PS_WARR85, PS_WARR86, PS_WARR87, PS_WARR88, PS_WARR89, PS_WARR90, PS_WARR91, PS_WARR92, PS_WARR93, PS_WARR94, PS_WARR95, PS_WARR96B, PS_WARR97, PS_WARR98, PS_WARR99, PS_WARR100, PS_WARR101, PS_WARR102 },
	{ PS_ROGUE1, PS_ROGUE2, PS_ROGUE3, PS_ROGUE4, PS_ROGUE5, PS_ROGUE6, PS_ROGUE7, PS_ROGUE8, PS_ROGUE9, PS_ROGUE10, PS_ROGUE11, PS_ROGUE12, PS_ROGUE13, PS_ROGUE14, PS_ROGUE15, PS_ROGUE16, PS_ROGUE17, PS_ROGUE18, PS_ROGUE19, PS_ROGUE20, PS_ROGUE21, PS_ROGUE22, PS_ROGUE23, PS_ROGUE24, PS_ROGUE25, PS_ROGUE26, PS_ROGUE27, PS_ROGUE28, PS_ROGUE29, PS_ROGUE30, PS_ROGUE31, PS_ROGUE32, PS_ROGUE33, PS_ROGUE34, PS_ROGUE35, PS_ROGUE36, PS_ROGUE37, PS_ROGUE38, PS_ROGUE39, PS_ROGUE40, PS_ROGUE41, PS_ROGUE42, PS_ROGUE43, PS_ROGUE44, PS_ROGUE45, PS_ROGUE46, PS_ROGUE47, PS_ROGUE48, PS_ROGUE49, PS_ROGUE50, PS_ROGUE51, PS_ROGUE52, PS_ROGUE53, PS_ROGUE54, PS_ROGUE55, PS_ROGUE56, PS_ROGUE57, PS_ROGUE58, PS_ROGUE59, PS_ROGUE60, PS_ROGUE61, PS_ROGUE62, PS_ROGUE63, PS_ROGUE64, PS_ROGUE65, PS_ROGUE66, PS_ROGUE67, PS_ROGUE68, PS_ROGUE69, PS_ROGUE70, PS_ROGUE71, PS_ROGUE72, PS_ROGUE73, PS_ROGUE74, PS_ROGUE75, PS_ROGUE76, PS_ROGUE77, PS_ROGUE78, PS_ROGUE79, PS_ROGUE80, PS_ROGUE81, PS_ROGUE82, PS_ROGUE83, PS_ROGUE84, PS_ROGUE85, PS_ROGUE86, PS_ROGUE87, PS_ROGUE88, PS_ROGUE89, PS_ROGUE90, PS_ROGUE91, PS_ROGUE92, PS_ROGUE93, PS_ROGUE94, PS_ROGUE95, PS_ROGUE96, PS_ROGUE97, PS_ROGUE98, PS_ROGUE99, PS_ROGUE100, PS_ROGUE101, PS_ROGUE102 },
	{ PS_MAGE1, PS_MAGE2, PS_MAGE3, PS_MAGE4, PS_MAGE5, PS_MAGE6, PS_MAGE7, PS_MAGE8, PS_MAGE9, PS_MAGE10, PS_MAGE11, PS_MAGE12, PS_MAGE13, PS_MAGE14, PS_MAGE15, PS_MAGE16, PS_MAGE17, PS_MAGE18, PS_MAGE19, PS_MAGE20, PS_MAGE21, PS_MAGE22, PS_MAGE23, PS_MAGE24, PS_MAGE25, PS_MAGE26, PS_MAGE27, PS_MAGE28, PS_MAGE29, PS_MAGE30, PS_MAGE31, PS_MAGE32, PS_MAGE33, PS_MAGE34, PS_MAGE35, PS_MAGE36, PS_MAGE37, PS_MAGE38, PS_MAGE39, PS_MAGE40, PS_MAGE41, PS_MAGE42, PS_MAGE43, PS_MAGE44, PS_MAGE45, PS_MAGE46, PS_MAGE47, PS_MAGE48, PS_MAGE49, PS_MAGE50, PS_MAGE51, PS_MAGE52, PS_MAGE53, PS_MAGE54, PS_MAGE55, PS_MAGE56, PS_MAGE57, PS_MAGE58, PS_MAGE59, PS_MAGE60, PS_MAGE61, PS_MAGE62, PS_MAGE63, PS_MAGE64, PS_MAGE65, PS_MAGE66, PS_MAGE67, PS_MAGE68, PS_MAGE69, PS_MAGE70, PS_MAGE71, PS_MAGE72, PS_MAGE73, PS_MAGE74, PS_MAGE75, PS_MAGE76, PS_MAGE77, PS_MAGE78, PS_MAGE79, PS_MAGE80, PS_MAGE81, PS_MAGE82, PS_MAGE83, PS_MAGE84, PS_MAGE85, PS_MAGE86, PS_MAGE87, PS_MAGE88, PS_MAGE89, PS_MAGE90, PS_MAGE91, PS_MAGE92, PS_MAGE93, PS_MAGE94, PS_MAGE95, PS_MAGE96, PS_MAGE97, PS_MAGE98, PS_MAGE99, PS_MAGE100, PS_MAGE101, PS_MAGE102 },
	{ PS_MONK1, PS_MONK2, PS_MONK3, PS_MONK4, PS_MONK5, PS_MONK6, PS_MONK7, PS_MONK8, PS_MONK9, PS_MONK10, PS_MONK11, PS_MONK12, PS_MONK13, PS_MONK14, PS_MONK15, PS_MONK16, PS_MONK17, PS_MONK18, PS_MONK19, PS_MONK20, PS_MONK21, PS_MONK22, PS_MONK23, PS_MONK24, PS_MONK25, PS_MONK26, PS_MONK27, PS_MONK28, PS_MONK29, PS_MONK30, PS_MONK31, PS_MONK32, PS_MONK33, PS_MONK34, PS_MONK35, PS_MONK36, PS_MONK37, PS_MONK38, PS_MONK39, PS_MONK40, PS_MONK41, PS_MONK42, PS_MONK43, PS_MONK44, PS_MONK45, PS_MONK46, PS_MONK47, PS_MONK48, PS_MONK49, PS_MONK50, PS_MONK51, PS_MONK52, PS_MONK53, PS_MONK54, PS_MONK55, PS_MONK56, PS_MONK57, PS_MONK58, PS_MONK59, PS_MONK60, PS_MONK61, PS_MONK62, PS_MONK63, PS_MONK64, PS_MONK65, PS_MONK66, PS_MONK67, PS_MONK68, PS_MONK69, PS_MONK70, PS_MONK71, PS_MONK72, PS_MONK73, PS_MONK74, PS_MONK75, PS_MONK76, PS_MONK77, PS_MONK78, PS_MONK79, PS_MONK80, PS_MONK81, PS_MONK82, PS_MONK83, PS_MONK84, PS_MONK85, PS_MONK86, PS_MONK87, PS_MONK88, PS_MONK89, PS_MONK90, PS_MONK91, PS_MONK92, PS_MONK93, PS_MONK94, PS_MONK95, PS_MONK96, PS_MONK97, PS_MONK98, PS_MONK99, PS_MONK100, PS_MONK101, PS_MONK102 },
	{ PS_ROGUE1, PS_ROGUE2, PS_ROGUE3, PS_ROGUE4, PS_ROGUE5, PS_ROGUE6, PS_ROGUE7, PS_ROGUE8, PS_ROGUE9, PS_ROGUE10, PS_ROGUE11, PS_ROGUE12, PS_ROGUE13, PS_ROGUE14, PS_ROGUE15, PS_ROGUE16, PS_ROGUE17, PS_ROGUE18, PS_ROGUE19, PS_ROGUE20, PS_ROGUE21, PS_ROGUE22, PS_ROGUE23, PS_ROGUE24, PS_ROGUE25, PS_ROGUE26, PS_ROGUE27, PS_ROGUE28, PS_ROGUE29, PS_ROGUE30, PS_ROGUE31, PS_ROGUE32, PS_ROGUE33, PS_ROGUE34, PS_ROGUE35, PS_ROGUE36, PS_ROGUE37, PS_ROGUE38, PS_ROGUE39, PS_ROGUE40, PS_ROGUE41, PS_ROGUE42, PS_ROGUE43, PS_ROGUE44, PS_ROGUE45, PS_ROGUE46, PS_ROGUE47, PS_ROGUE48, PS_ROGUE49, PS_ROGUE50, PS_ROGUE51, PS_ROGUE52, PS_ROGUE53, PS_ROGUE54, PS_ROGUE55, PS_ROGUE56, PS_ROGUE57, PS_ROGUE58, PS_ROGUE59, PS_ROGUE60, PS_ROGUE61, PS_ROGUE62, PS_ROGUE63, PS_ROGUE64, PS_ROGUE65, PS_ROGUE66, PS_ROGUE67, PS_ROGUE68, PS_ROGUE69, PS_ROGUE70, PS_ROGUE71, PS_ROGUE72, PS_ROGUE73, PS_ROGUE74, PS_ROGUE75, PS_ROGUE76, PS_ROGUE77, PS_ROGUE78, PS_ROGUE79, PS_ROGUE80, PS_ROGUE81, PS_ROGUE82, PS_ROGUE83, PS_ROGUE84, PS_ROGUE85, PS_ROGUE86, PS_ROGUE87, PS_ROGUE88, PS_ROGUE89, PS_ROGUE90, PS_ROGUE91, PS_ROGUE92, PS_ROGUE93, PS_ROGUE94, PS_ROGUE95, PS_ROGUE96, PS_ROGUE97, PS_ROGUE98, PS_ROGUE99, PS_ROGUE100, PS_ROGUE101, PS_ROGUE102 },
	{ PS_WARR1, PS_WARR2, PS_WARR3, PS_WARR4, PS_WARR5, PS_WARR6, PS_WARR7, PS_WARR8, PS_WARR9, PS_WARR10, PS_WARR11, PS_WARR12, PS_WARR13, PS_WARR14, PS_WARR15, PS_WARR16, PS_WARR17, PS_WARR18, PS_WARR19, PS_WARR20, PS_WARR21, PS_WARR22, PS_WARR23, PS_WARR24, PS_WARR25, PS_WARR26, PS_WARR27, PS_WARR28, PS_WARR29, PS_WARR30, PS_WARR31, PS_WARR32, PS_WARR33, PS_WARR34, PS_WARR35, PS_WARR36, PS_WARR37, PS_WARR38, PS_WARR39, PS_WARR40, PS_WARR41, PS_WARR42, PS_WARR43, PS_WARR44, PS_WARR45, PS_WARR46, PS_WARR47, PS_WARR48, PS_WARR49, PS_WARR50, PS_WARR51, PS_WARR52, PS_WARR53, PS_WARR54, PS_WARR55, PS_WARR56, PS_WARR57, PS_WARR58, PS_WARR59, PS_WARR60, PS_WARR61, PS_WARR62, PS_WARR63, PS_WARR64, PS_WARR65, PS_WARR66, PS_WARR67, PS_WARR68, PS_WARR69, PS_WARR70, PS_WARR71, PS_WARR72, PS_WARR73, PS_WARR74, PS_WARR75, PS_WARR76, PS_WARR77, PS_WARR78, PS_WARR79, PS_WARR80, PS_WARR81, PS_WARR82, PS_WARR83, PS_WARR84, PS_WARR85, PS_WARR86, PS_WARR87, PS_WARR88, PS_WARR89, PS_WARR90, PS_WARR91, PS_WARR92, PS_WARR93, PS_WARR94, PS_WARR95, PS_WARR96B, PS_WARR97, PS_WARR98, PS_WARR99, PS_WARR100, PS_WARR101, PS_WARR102 },
};


_sfx_id GetSoundByClass(int playerID, int16_t soundID)
{
	return herosounds[static_cast<int>(plr[playerID]._pClass)][soundID - 1];
}

void PlaySoundByClass(int playerID, int16_t soundID, bool noRepeat /* = false */)
{
	_sfx_id soundEffect = GetSoundByClass(playerID, soundID);
	if (noRepeat && effect_is_playing(soundEffect))
		return;
	PlaySFX(soundEffect);
}

bool effect_is_playing(int nSFX)
{
	TSFX *sfx = &sgSFX[nSFX];
	if (sfx->pSnd)
		return snd_playing(sfx->pSnd);

	if (sfx->bFlags & sfx_STREAM)
		return sfx == sgpStreamSFX;

	return false;
}

void stream_stop()
{
	if (sgpStreamSFX != NULL) {
		sgpStreamSFX->pSnd->DSB->Stop();
		sound_file_cleanup(sgpStreamSFX->pSnd);
		sgpStreamSFX->pSnd = NULL;
		sgpStreamSFX = NULL;
	}
}

static void stream_play(TSFX *pSFX, int lVolume, int lPan)
{
#ifndef DISABLE_STREAMING_SOUNDS
	constexpr bool kAllowStreaming = true;
#else
	constexpr bool kAllowStreaming = false;
#endif

	assert(pSFX);
	assert(pSFX->bFlags & sfx_STREAM);
	stream_stop();
	lVolume += sound_get_or_set_sound_volume(1);
	if (lVolume >= VOLUME_MIN) {
		if (lVolume > VOLUME_MAX)
			lVolume = VOLUME_MAX;
		if (pSFX->pSnd == NULL)
			pSFX->pSnd = sound_file_load(pSFX->pszName, kAllowStreaming);
		pSFX->pSnd->DSB->Play(lVolume, lPan, 0);
		sgpStreamSFX = pSFX;
	}
}

static void stream_update()
{
	if (sgpStreamSFX != NULL && !sgpStreamSFX->pSnd->DSB->IsPlaying()) {
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
				if (!pSnd)
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
				if (pSnd) {
					Monsters[i].Snds[j][k] = NULL;
					file = pSnd->sound_path;
					pSnd->sound_path = NULL;
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

	x -= plr[myplr]._px;
	y -= plr[myplr]._py;

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

	if (!(pSFX->bFlags & (sfx_STREAM | sfx_MISC)) && pSFX->pSnd != 0 && snd_playing(pSFX->pSnd)) {
		return;
	}

	lPan = 0;
	lVolume = 0;
	if (loc && !calc_snd_position(x, y, &lVolume, &lPan)) {
		return;
	}

	if (pSFX->bFlags & sfx_STREAM) {
		stream_play(pSFX, lVolume, lPan);
		return;
	}

	if (!pSFX->pSnd)
		pSFX->pSnd = sound_file_load(pSFX->pszName);

	if (pSFX->pSnd)
		snd_play_snd(pSFX->pSnd, lVolume, lPan);
}

void PlayEffect(int i, int mode)
{
	int sndIdx, mi, lVolume, lPan;
	TSnd *snd;

	if (plr[myplr].pLvlLoad) {
		return;
	}

	sndIdx = random_(164, 2);
	if (!gbSndInited || !gbSoundOn || gbBufferMsgs) {
		return;
	}

	mi = monster[i]._mMTidx;
	snd = Monsters[mi].Snds[mode][sndIdx];
	if (!snd || snd_playing(snd)) {
		return;
	}

	if (!calc_snd_position(monster[i]._mx, monster[i]._my, &lVolume, &lPan))
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

	return static_cast<_sfx_id>(psfx + random_(165, nRand));
}

void PlaySFX(_sfx_id psfx, bool randomizeByCategory)
{
	if (randomizeByCategory) {
		psfx = RndSFX(psfx);
	}

	PlaySFX_priv(&sgSFX[psfx], false, 0, 0);
}

void PlaySfxLoc(_sfx_id psfx, int x, int y)
{
	TSnd *pSnd;

	psfx = RndSFX(psfx);

	if (psfx >= 0 && psfx <= 3) {
		pSnd = sgSFX[psfx].pSnd;
		if (pSnd)
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
		if (sgSFX[i].pSnd) {
			sound_file_cleanup(sgSFX[i].pSnd);
			sgSFX[i].pSnd = NULL;
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
		if (sgSFX[i].pSnd) {
			continue;
		}

		if (sgSFX[i].bFlags & sfx_STREAM) {
			continue;
		}

		if (!(sgSFX[i].bFlags & bLoadMask)) {
			continue;
		}

		if (!gbIsHellfire && sgSFX[i].bFlags & sfx_HELLFIRE) {
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
		app_fatal("effects:1");
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
		if (!strcasecmp(sgSFX[i].pszName, snd_file) && sgSFX[i].pSnd) {
			if (!snd_playing(sgSFX[i].pSnd))
				snd_play_snd(sgSFX[i].pSnd, 0, 0);

			return;
		}
	}
}

int GetSFXLength(int nSFX)
{
	if (sgSFX[nSFX].pSnd == NULL)
		sgSFX[nSFX].pSnd = sound_file_load(sgSFX[nSFX].pszName);
	return sgSFX[nSFX].pSnd->DSB->GetLength();
}

} // namespace devilution
