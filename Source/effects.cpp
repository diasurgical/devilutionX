#include "diablo.h"
#include "../3rdParty/Storm/Source/storm.h"

DEVILUTION_BEGIN_NAMESPACE

int sfxdelay;
int sfxdnum;
HANDLE sfx_stream;
TSFX *sfx_data_cur;

const char monster_action_sounds[] = { 'a', 'h', 'd', 's' };

/* data */

TSFX sgSFX[NUM_SFX] = {
	// clang-format off
	// bFlags,                   pszName,                       pSnd
	{ SFX_MISC,                  "Sfx\\Misc\\Walk1.wav",        NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Walk2.wav",        NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Walk3.wav",        NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Walk4.wav",        NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\BFire.wav",        NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Fmag.wav",         NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Tmag.wav",         NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Lghit.wav",        NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Lghit1.wav",       NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Swing.wav",        NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Swing2.wav",       NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Dead.wav",         NULL },
	{ SFX_STREAM,                "Sfx\\Misc\\Questdon.wav",     NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Armrfkd.wav",     NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Barlfire.wav",    NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Barrel.wav",      NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Bhit.wav",        NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Bhit1.wav",       NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Chest.wav",       NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Doorclos.wav",    NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Dooropen.wav",    NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Flipanvl.wav",    NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Flipaxe.wav",     NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Flipblst.wav",    NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Flipbody.wav",    NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Flipbook.wav",    NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Flipbow.wav",     NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Flipcap.wav",     NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Flipharm.wav",    NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Fliplarm.wav",    NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Flipmag.wav",     NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Flipmag1.wav",    NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Flipmush.wav",    NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Flippot.wav",     NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Flipring.wav",    NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Fliprock.wav",    NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Flipscrl.wav",    NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Flipshld.wav",    NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Flipsign.wav",    NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Flipstaf.wav",    NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Flipswor.wav",    NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Gold.wav",        NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Hlmtfkd.wav",     NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Invanvl.wav",     NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Invaxe.wav",      NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Invblst.wav",     NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Invbody.wav",     NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Invbook.wav",     NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Invbow.wav",      NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Invcap.wav",      NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Invgrab.wav",     NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Invharm.wav",     NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Invlarm.wav",     NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Invmush.wav",     NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Invpot.wav",      NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Invring.wav",     NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Invrock.wav",     NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Invscrol.wav",    NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Invshiel.wav",    NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Invsign.wav",     NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Invstaf.wav",     NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Invsword.wav",    NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Lever.wav",       NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Magic.wav",       NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Magic1.wav",      NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Readbook.wav",    NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Sarc.wav",        NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Shielfkd.wav",    NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Swrdfkd.wav",     NULL },
	{ SFX_UI,                    "Sfx\\Items\\Titlemov.wav",    NULL },
	{ SFX_UI,                    "Sfx\\Items\\Titlslct.wav",    NULL },
	{ SFX_UI,                    "Sfx\\Misc\\blank.wav",        NULL },
	{ SFX_MISC,                  "Sfx\\Items\\Trap.wav",        NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Cast1.wav",        NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Cast10.wav",       NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Cast12.wav",       NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Cast2.wav",        NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Cast3.wav",        NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Cast4.wav",        NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Cast5.wav",        NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Cast6.wav",        NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Cast7.wav",        NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Cast8.wav",        NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Cast9.wav",        NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Healing.wav",      NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Repair.wav",       NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Acids1.wav",       NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Acids2.wav",       NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Apoc.wav",         NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Arrowall.wav",     NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Bldboil.wav",      NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Blodstar.wav",     NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Blsimpt.wav",      NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Bonesp.wav",       NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Bsimpct.wav",      NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Caldron.wav",      NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Cbolt.wav",        NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Chltning.wav",     NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\DSerp.wav",        NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Elecimp1.wav",     NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Elementl.wav",     NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Ethereal.wav",     NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Fball.wav",        NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Fbolt1.wav",       NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Fbolt2.wav",       NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Firimp1.wav",      NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Firimp2.wav",      NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Flamwave.wav",     NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Flash.wav",        NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Fountain.wav",     NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Golum.wav",        NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Golumded.wav",     NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Gshrine.wav",      NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Guard.wav",        NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Grdlanch.wav",     NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Holybolt.wav",     NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Hyper.wav",        NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Infravis.wav",     NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Invisibl.wav",     NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Invpot.wav",       NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Lning1.wav",       NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Ltning.wav",       NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Mshield.wav",      NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Nova.wav",         NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Portal.wav",       NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Puddle.wav",       NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Resur.wav",        NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Scurse.wav",       NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Scurimp.wav",      NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Sentinel.wav",     NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Shatter.wav",      NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Soulfire.wav",     NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Spoutlop.wav",     NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Spoutstr.wav",     NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Storm.wav",        NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Trapdis.wav",      NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Teleport.wav",     NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Vtheft.wav",       NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Wallloop.wav",     NULL },
	{ SFX_MISC,                  "Sfx\\Misc\\Wallstrt.wav",     NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bmaid01.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bmaid02.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bmaid03.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bmaid04.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bmaid05.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bmaid06.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bmaid07.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bmaid08.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bmaid09.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bmaid10.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bmaid11.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bmaid12.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bmaid13.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bmaid14.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bmaid15.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bmaid16.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bmaid17.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bmaid18.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bmaid19.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bmaid20.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bmaid21.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bmaid22.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bmaid23.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bmaid24.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bmaid25.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bmaid26.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bmaid27.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bmaid28.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bmaid29.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bmaid30.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bmaid31.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bmaid32.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bmaid33.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bmaid34.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bmaid35.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bmaid36.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bmaid37.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bmaid38.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bmaid39.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bmaid40.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith01.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith02.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith03.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith04.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith05.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith06.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith07.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith08.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith09.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith10.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith11.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith12.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith13.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith14.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith15.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith16.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith17.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith18.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith19.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith20.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith21.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith22.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith23.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith24.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith25.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith26.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith27.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith28.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith29.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith30.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith31.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith32.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith33.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith34.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith35.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith36.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith37.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith38.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith39.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith40.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith41.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith42.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith43.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith44.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith45.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith46.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith47.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith48.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith49.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith50.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith51.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith52.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith53.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith54.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith55.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Bsmith56.wav",  NULL },
	{ 0,                         "Sfx\\Towners\\Cow1.wav",      NULL },
	{ 0,                         "Sfx\\Towners\\Cow2.wav",      NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Deadguy2.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Drunk01.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Drunk02.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Drunk03.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Drunk04.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Drunk05.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Drunk06.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Drunk07.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Drunk08.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Drunk09.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Drunk10.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Drunk11.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Drunk12.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Drunk13.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Drunk14.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Drunk15.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Drunk16.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Drunk17.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Drunk18.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Drunk19.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Drunk20.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Drunk21.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Drunk22.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Drunk23.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Drunk24.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Drunk25.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Drunk26.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Drunk27.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Drunk28.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Drunk29.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Drunk30.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Drunk31.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Drunk32.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Drunk33.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Drunk34.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Drunk35.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Healer01.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Healer02.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Healer03.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Healer04.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Healer05.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Healer06.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Healer07.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Healer08.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Healer09.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Healer10.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Healer11.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Healer12.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Healer13.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Healer14.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Healer15.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Healer16.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Healer17.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Healer18.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Healer19.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Healer20.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Healer21.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Healer22.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Healer23.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Healer24.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Healer25.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Healer26.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Healer27.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Healer28.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Healer29.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Healer30.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Healer31.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Healer32.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Healer33.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Healer34.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Healer35.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Healer36.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Healer37.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Healer38.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Healer39.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Healer40.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Healer41.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Healer42.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Healer43.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Healer44.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Healer45.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Healer46.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Healer47.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Pegboy01.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Pegboy02.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Pegboy03.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Pegboy04.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Pegboy05.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Pegboy06.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Pegboy07.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Pegboy08.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Pegboy09.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Pegboy10.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Pegboy11.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Pegboy12.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Pegboy13.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Pegboy14.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Pegboy15.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Pegboy16.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Pegboy17.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Pegboy18.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Pegboy19.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Pegboy20.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Pegboy21.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Pegboy22.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Pegboy23.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Pegboy24.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Pegboy25.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Pegboy26.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Pegboy27.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Pegboy28.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Pegboy29.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Pegboy30.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Pegboy31.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Pegboy32.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Pegboy33.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Pegboy34.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Pegboy35.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Pegboy36.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Pegboy37.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Pegboy38.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Pegboy39.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Pegboy40.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Pegboy41.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Pegboy42.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Pegboy43.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Priest00.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Priest01.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Priest02.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Priest03.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Priest04.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Priest05.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Priest06.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Priest07.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Storyt00.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Storyt01.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Storyt02.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Storyt03.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Storyt04.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Storyt05.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Storyt06.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Storyt07.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Storyt08.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Storyt09.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Storyt10.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Storyt11.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Storyt12.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Storyt13.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Storyt14.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Storyt15.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Storyt16.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Storyt17.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Storyt18.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Storyt19.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Storyt20.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Storyt21.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Storyt22.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Storyt23.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Storyt24.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Storyt25.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Storyt26.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Storyt27.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Storyt28.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Storyt29.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Storyt30.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Storyt31.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Storyt32.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Storyt33.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Storyt34.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Storyt35.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Storyt36.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Storyt37.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Storyt38.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Tavown00.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Tavown01.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Tavown02.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Tavown03.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Tavown04.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Tavown05.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Tavown06.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Tavown07.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Tavown08.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Tavown09.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Tavown10.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Tavown11.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Tavown12.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Tavown13.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Tavown14.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Tavown15.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Tavown16.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Tavown17.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Tavown18.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Tavown19.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Tavown20.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Tavown21.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Tavown22.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Tavown23.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Tavown24.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Tavown25.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Tavown26.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Tavown27.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Tavown28.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Tavown29.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Tavown30.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Tavown31.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Tavown32.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Tavown33.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Tavown34.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Tavown35.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Tavown36.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Tavown37.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Tavown38.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Tavown39.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Tavown40.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Tavown41.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Tavown42.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Tavown43.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Tavown44.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Tavown45.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Witch01.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Witch02.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Witch03.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Witch04.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Witch05.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Witch06.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Witch07.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Witch08.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Witch09.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Witch10.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Witch11.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Witch12.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Witch13.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Witch14.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Witch15.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Witch16.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Witch17.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Witch18.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Witch19.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Witch20.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Witch21.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Witch22.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Witch23.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Witch24.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Witch25.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Witch26.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Witch27.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Witch28.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Witch29.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Witch30.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Witch31.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Witch32.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Witch33.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Witch34.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Witch35.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Witch36.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Witch37.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Witch38.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Witch39.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Witch40.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Witch41.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Witch42.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Witch43.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Witch44.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Witch45.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Witch46.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Witch47.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Witch48.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Witch49.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Witch50.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Towners\\Wound01.wav",   NULL },
	{ SFX_STREAM | SFX_SORCEROR, "Sfx\\Sorceror\\Mage01.wav",   NULL },
	{ SFX_STREAM | SFX_SORCEROR, "Sfx\\Sorceror\\Mage02.wav",   NULL },
	{ SFX_STREAM | SFX_SORCEROR, "Sfx\\Sorceror\\Mage03.wav",   NULL },
	{ SFX_STREAM | SFX_SORCEROR, "Sfx\\Sorceror\\Mage04.wav",   NULL },
	{ SFX_STREAM | SFX_SORCEROR, "Sfx\\Sorceror\\Mage05.wav",   NULL },
	{ SFX_STREAM | SFX_SORCEROR, "Sfx\\Sorceror\\Mage06.wav",   NULL },
	{ SFX_STREAM | SFX_SORCEROR, "Sfx\\Sorceror\\Mage07.wav",   NULL },
	{ SFX_STREAM | SFX_SORCEROR, "Sfx\\Sorceror\\Mage08.wav",   NULL },
	{ SFX_STREAM | SFX_SORCEROR, "Sfx\\Sorceror\\Mage09.wav",   NULL },
	{ SFX_STREAM | SFX_SORCEROR, "Sfx\\Sorceror\\Mage10.wav",   NULL },
	{ SFX_STREAM | SFX_SORCEROR, "Sfx\\Sorceror\\Mage11.wav",   NULL },
	{ SFX_STREAM | SFX_SORCEROR, "Sfx\\Sorceror\\Mage12.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage13.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage14.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage15.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage16.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage17.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage18.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage19.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage20.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage21.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage22.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage23.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage24.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage25.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage26.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage27.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage28.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage29.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage30.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage31.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage32.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage33.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage34.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage35.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage36.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage37.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage38.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage39.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage40.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage41.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage42.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage43.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage44.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage45.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage46.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage47.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage48.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage49.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage50.wav",   NULL },
	{ SFX_STREAM | SFX_SORCEROR, "Sfx\\Sorceror\\Mage51.wav",   NULL },
	{ SFX_STREAM | SFX_SORCEROR, "Sfx\\Sorceror\\Mage52.wav",   NULL },
	{ SFX_STREAM | SFX_SORCEROR, "Sfx\\Sorceror\\Mage53.wav",   NULL },
	{ SFX_STREAM | SFX_SORCEROR, "Sfx\\Sorceror\\Mage54.wav",   NULL },
	{ SFX_STREAM | SFX_SORCEROR, "Sfx\\Sorceror\\Mage55.wav",   NULL },
	{ SFX_STREAM | SFX_SORCEROR, "Sfx\\Sorceror\\Mage56.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage57.wav",   NULL },
	{ SFX_STREAM | SFX_SORCEROR, "Sfx\\Sorceror\\Mage58.wav",   NULL },
	{ SFX_STREAM | SFX_SORCEROR, "Sfx\\Sorceror\\Mage59.wav",   NULL },
	{ SFX_STREAM | SFX_SORCEROR, "Sfx\\Sorceror\\Mage60.wav",   NULL },
	{ SFX_STREAM | SFX_SORCEROR, "Sfx\\Sorceror\\Mage61.wav",   NULL },
	{ SFX_STREAM | SFX_SORCEROR, "Sfx\\Sorceror\\Mage62.wav",   NULL },
	{ SFX_STREAM | SFX_SORCEROR, "Sfx\\Sorceror\\Mage63.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage64.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage65.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage66.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage67.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage68.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage69.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage69b.wav",  NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage70.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage71.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage72.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage73.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage74.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage75.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage76.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage77.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage78.wav",   NULL },
	{ SFX_SORCEROR,              "Sfx\\Sorceror\\Mage79.wav",   NULL },
	{ SFX_STREAM | SFX_SORCEROR, "Sfx\\Sorceror\\Mage80.wav",   NULL },
	{ SFX_STREAM | SFX_SORCEROR, "Sfx\\Sorceror\\Mage81.wav",   NULL },
	{ SFX_STREAM | SFX_SORCEROR, "Sfx\\Sorceror\\Mage82.wav",   NULL },
	{ SFX_STREAM | SFX_SORCEROR, "Sfx\\Sorceror\\Mage83.wav",   NULL },
	{ SFX_STREAM | SFX_SORCEROR, "Sfx\\Sorceror\\Mage84.wav",   NULL },
	{ SFX_STREAM | SFX_SORCEROR, "Sfx\\Sorceror\\Mage85.wav",   NULL },
	{ SFX_STREAM | SFX_SORCEROR, "Sfx\\Sorceror\\Mage86.wav",   NULL },
	{ SFX_STREAM | SFX_SORCEROR, "Sfx\\Sorceror\\Mage87.wav",   NULL },
	{ SFX_STREAM | SFX_SORCEROR, "Sfx\\Sorceror\\Mage88.wav",   NULL },
	{ SFX_STREAM | SFX_SORCEROR, "Sfx\\Sorceror\\Mage89.wav",   NULL },
	{ SFX_STREAM | SFX_SORCEROR, "Sfx\\Sorceror\\Mage90.wav",   NULL },
	{ SFX_STREAM | SFX_SORCEROR, "Sfx\\Sorceror\\Mage91.wav",   NULL },
	{ SFX_STREAM | SFX_SORCEROR, "Sfx\\Sorceror\\Mage92.wav",   NULL },
	{ SFX_STREAM | SFX_SORCEROR, "Sfx\\Sorceror\\Mage93.wav",   NULL },
	{ SFX_STREAM | SFX_SORCEROR, "Sfx\\Sorceror\\Mage94.wav",   NULL },
	{ SFX_STREAM | SFX_SORCEROR, "Sfx\\Sorceror\\Mage95.wav",   NULL },
	{ SFX_STREAM | SFX_SORCEROR, "Sfx\\Sorceror\\Mage96.wav",   NULL },
	{ SFX_STREAM | SFX_SORCEROR, "Sfx\\Sorceror\\Mage97.wav",   NULL },
	{ SFX_STREAM | SFX_SORCEROR, "Sfx\\Sorceror\\Mage98.wav",   NULL },
	{ SFX_STREAM | SFX_SORCEROR, "Sfx\\Sorceror\\Mage99.wav",   NULL },
	{ SFX_STREAM | SFX_SORCEROR, "Sfx\\Sorceror\\Mage100.wav",  NULL },
	{ SFX_STREAM | SFX_SORCEROR, "Sfx\\Sorceror\\Mage101.wav",  NULL },
	{ SFX_STREAM | SFX_SORCEROR, "Sfx\\Sorceror\\Mage102.wav",  NULL },
	{ SFX_STREAM | SFX_ROGUE,    "Sfx\\Rogue\\Rogue01.wav",     NULL },
	{ SFX_STREAM | SFX_ROGUE,    "Sfx\\Rogue\\Rogue02.wav",     NULL },
	{ SFX_STREAM | SFX_ROGUE,    "Sfx\\Rogue\\Rogue03.wav",     NULL },
	{ SFX_STREAM | SFX_ROGUE,    "Sfx\\Rogue\\Rogue04.wav",     NULL },
	{ SFX_STREAM | SFX_ROGUE,    "Sfx\\Rogue\\Rogue05.wav",     NULL },
	{ SFX_STREAM | SFX_ROGUE,    "Sfx\\Rogue\\Rogue06.wav",     NULL },
	{ SFX_STREAM | SFX_ROGUE,    "Sfx\\Rogue\\Rogue07.wav",     NULL },
	{ SFX_STREAM | SFX_ROGUE,    "Sfx\\Rogue\\Rogue08.wav",     NULL },
	{ SFX_STREAM | SFX_ROGUE,    "Sfx\\Rogue\\Rogue09.wav",     NULL },
	{ SFX_STREAM | SFX_ROGUE,    "Sfx\\Rogue\\Rogue10.wav",     NULL },
	{ SFX_STREAM | SFX_ROGUE,    "Sfx\\Rogue\\Rogue11.wav",     NULL },
	{ SFX_STREAM | SFX_ROGUE,    "Sfx\\Rogue\\Rogue12.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue13.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue14.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue15.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue16.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue17.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue18.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue19.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue20.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue21.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue22.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue23.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue24.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue25.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue26.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue27.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue28.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue29.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue30.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue31.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue32.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue33.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue34.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue35.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue36.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue37.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue38.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue39.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue40.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue41.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue42.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue43.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue44.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue45.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue46.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue47.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue48.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue49.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue50.wav",     NULL },
	{ SFX_STREAM | SFX_ROGUE,    "Sfx\\Rogue\\Rogue51.wav",     NULL },
	{ SFX_STREAM | SFX_ROGUE,    "Sfx\\Rogue\\Rogue52.wav",     NULL },
	{ SFX_STREAM | SFX_ROGUE,    "Sfx\\Rogue\\Rogue53.wav",     NULL },
	{ SFX_STREAM | SFX_ROGUE,    "Sfx\\Rogue\\Rogue54.wav",     NULL },
	{ SFX_STREAM | SFX_ROGUE,    "Sfx\\Rogue\\Rogue55.wav",     NULL },
	{ SFX_STREAM | SFX_ROGUE,    "Sfx\\Rogue\\Rogue56.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue57.wav",     NULL },
	{ SFX_STREAM | SFX_ROGUE,    "Sfx\\Rogue\\Rogue58.wav",     NULL },
	{ SFX_STREAM | SFX_ROGUE,    "Sfx\\Rogue\\Rogue59.wav",     NULL },
	{ SFX_STREAM | SFX_ROGUE,    "Sfx\\Rogue\\Rogue60.wav",     NULL },
	{ SFX_STREAM | SFX_ROGUE,    "Sfx\\Rogue\\Rogue61.wav",     NULL },
	{ SFX_STREAM | SFX_ROGUE,    "Sfx\\Rogue\\Rogue62.wav",     NULL },
	{ SFX_STREAM | SFX_ROGUE,    "Sfx\\Rogue\\Rogue63.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue64.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue65.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue66.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue67.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue68.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue69.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue69b.wav",    NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue70.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue71.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue72.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue73.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue74.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue75.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue76.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue77.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue78.wav",     NULL },
	{ SFX_ROGUE,                 "Sfx\\Rogue\\Rogue79.wav",     NULL },
	{ SFX_STREAM | SFX_ROGUE,    "Sfx\\Rogue\\Rogue80.wav",     NULL },
	{ SFX_STREAM | SFX_ROGUE,    "Sfx\\Rogue\\Rogue81.wav",     NULL },
	{ SFX_STREAM | SFX_ROGUE,    "Sfx\\Rogue\\Rogue82.wav",     NULL },
	{ SFX_STREAM | SFX_ROGUE,    "Sfx\\Rogue\\Rogue83.wav",     NULL },
	{ SFX_STREAM | SFX_ROGUE,    "Sfx\\Rogue\\Rogue84.wav",     NULL },
	{ SFX_STREAM | SFX_ROGUE,    "Sfx\\Rogue\\Rogue85.wav",     NULL },
	{ SFX_STREAM | SFX_ROGUE,    "Sfx\\Rogue\\Rogue86.wav",     NULL },
	{ SFX_STREAM | SFX_ROGUE,    "Sfx\\Rogue\\Rogue87.wav",     NULL },
	{ SFX_STREAM | SFX_ROGUE,    "Sfx\\Rogue\\Rogue88.wav",     NULL },
	{ SFX_STREAM | SFX_ROGUE,    "Sfx\\Rogue\\Rogue89.wav",     NULL },
	{ SFX_STREAM | SFX_ROGUE,    "Sfx\\Rogue\\Rogue90.wav",     NULL },
	{ SFX_STREAM | SFX_ROGUE,    "Sfx\\Rogue\\Rogue91.wav",     NULL },
	{ SFX_STREAM | SFX_ROGUE,    "Sfx\\Rogue\\Rogue92.wav",     NULL },
	{ SFX_STREAM | SFX_ROGUE,    "Sfx\\Rogue\\Rogue93.wav",     NULL },
	{ SFX_STREAM | SFX_ROGUE,    "Sfx\\Rogue\\Rogue94.wav",     NULL },
	{ SFX_STREAM | SFX_ROGUE,    "Sfx\\Rogue\\Rogue95.wav",     NULL },
	{ SFX_STREAM | SFX_ROGUE,    "Sfx\\Rogue\\Rogue96.wav",     NULL },
	{ SFX_STREAM | SFX_ROGUE,    "Sfx\\Rogue\\Rogue97.wav",     NULL },
	{ SFX_STREAM | SFX_ROGUE,    "Sfx\\Rogue\\Rogue98.wav",     NULL },
	{ SFX_STREAM | SFX_ROGUE,    "Sfx\\Rogue\\Rogue99.wav",     NULL },
	{ SFX_STREAM | SFX_ROGUE,    "Sfx\\Rogue\\Rogue100.wav",    NULL },
	{ SFX_STREAM | SFX_ROGUE,    "Sfx\\Rogue\\Rogue101.wav",    NULL },
	{ SFX_STREAM | SFX_ROGUE,    "Sfx\\Rogue\\Rogue102.wav",    NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Warior01.wav",  NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Warior02.wav",  NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Warior03.wav",  NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Warior04.wav",  NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Warior05.wav",  NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Warior06.wav",  NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Warior07.wav",  NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Warior08.wav",  NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Warior09.wav",  NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Warior10.wav",  NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Warior11.wav",  NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Warior12.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior13.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior14.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Wario14b.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Wario14c.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior15.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Wario15b.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Wario15c.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior16.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Wario16b.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Wario16c.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior17.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior18.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior19.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior20.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior21.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior22.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior23.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior24.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior25.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior26.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior27.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior28.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior29.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior30.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior31.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior32.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior33.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior34.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior35.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior36.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior37.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior38.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior39.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior40.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior41.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior42.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior43.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior44.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior45.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior46.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior47.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior48.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior49.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior50.wav",  NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Warior51.wav",  NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Warior52.wav",  NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Warior53.wav",  NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Warior54.wav",  NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Warior55.wav",  NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Warior56.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior57.wav",  NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Warior58.wav",  NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Warior59.wav",  NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Warior60.wav",  NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Warior61.wav",  NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Warior62.wav",  NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Warior63.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior64.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior65.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior66.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior67.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior68.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior69.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Wario69b.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior70.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior71.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior72.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior73.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior74.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior75.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior76.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior77.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior78.wav",  NULL },
	{ SFX_WARRIOR,               "Sfx\\Warrior\\Warior79.wav",  NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Warior80.wav",  NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Warior81.wav",  NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Warior82.wav",  NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Warior83.wav",  NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Warior84.wav",  NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Warior85.wav",  NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Warior86.wav",  NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Warior87.wav",  NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Warior88.wav",  NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Warior89.wav",  NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Warior90.wav",  NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Warior91.wav",  NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Warior92.wav",  NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Warior93.wav",  NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Warior94.wav",  NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Warior95.wav",  NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Wario95b.wav",  NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Wario95c.wav",  NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Wario95d.wav",  NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Wario95e.wav",  NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Wario95f.wav",  NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Wario96b.wav",  NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Wario97.wav",   NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Wario98.wav",   NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Warior99.wav",  NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Wario100.wav",  NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Wario101.wav",  NULL },
	{ SFX_STREAM | SFX_WARRIOR,  "Sfx\\Warrior\\Wario102.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Narrator\\Nar01.wav",    NULL },
	{ SFX_STREAM,                "Sfx\\Narrator\\Nar02.wav",    NULL },
	{ SFX_STREAM,                "Sfx\\Narrator\\Nar03.wav",    NULL },
	{ SFX_STREAM,                "Sfx\\Narrator\\Nar04.wav",    NULL },
	{ SFX_STREAM,                "Sfx\\Narrator\\Nar05.wav",    NULL },
	{ SFX_STREAM,                "Sfx\\Narrator\\Nar06.wav",    NULL },
	{ SFX_STREAM,                "Sfx\\Narrator\\Nar07.wav",    NULL },
	{ SFX_STREAM,                "Sfx\\Narrator\\Nar08.wav",    NULL },
	{ SFX_STREAM,                "Sfx\\Narrator\\Nar09.wav",    NULL },
	{ SFX_STREAM,                "Sfx\\Misc\\Lvl16int.wav",     NULL },
	{ SFX_STREAM,                "Sfx\\Monsters\\Butcher.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Monsters\\Garbud01.wav", NULL },
	{ SFX_STREAM,                "Sfx\\Monsters\\Garbud02.wav", NULL },
	{ SFX_STREAM,                "Sfx\\Monsters\\Garbud03.wav", NULL },
	{ SFX_STREAM,                "Sfx\\Monsters\\Garbud04.wav", NULL },
	{ SFX_STREAM,                "Sfx\\Monsters\\Izual01.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Monsters\\Lach01.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Monsters\\Lach02.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Monsters\\Lach03.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Monsters\\Laz01.wav",    NULL },
	{ SFX_STREAM,                "Sfx\\Monsters\\Laz02.wav",    NULL },
	{ SFX_STREAM,                "Sfx\\Monsters\\Sking01.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Monsters\\Snot01.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Monsters\\Snot02.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Monsters\\Snot03.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Monsters\\Warlrd01.wav", NULL },
	{ SFX_STREAM,                "Sfx\\Monsters\\Wlock01.wav",  NULL },
	{ SFX_STREAM,                "Sfx\\Monsters\\Zhar01.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Monsters\\Zhar02.wav",   NULL },
	{ SFX_STREAM,                "Sfx\\Monsters\\DiabloD.wav",  NULL }
	// clang-format on
};

BOOL effect_is_playing(int nSFX)
{
	TSFX *sfx = &sgSFX[nSFX];
	if (sfx->pSnd)
		return snd_playing(sfx->pSnd);

	if (sfx->bFlags & SFX_STREAM)
		return sfx == sfx_data_cur;

	return FALSE;
}

void sfx_stop()
{
	if (sfx_stream) {
		SFileDdaEnd(sfx_stream);
		SFileCloseFile(sfx_stream);
		sfx_stream = NULL;
		sfx_data_cur = NULL;
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
		if (monster_action_sounds[i] != 's' || monsterdata[mtype].snd_special) {
			for (j = 0; j < 2; j++) {
				sprintf(name, monsterdata[mtype].sndfile, monster_action_sounds[i], j + 1);
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

void FreeEffects()
{
	int mtype, i, j, k;
	char *file;
	TSnd *pSnd;

	for (i = 0; i < nummtypes; i++) {
		mtype = Monsters[i].mtype;
		for (j = 0; j < 4; ++j) {
			for (k = 0; k < 2; ++k) {
				pSnd = Monsters[i].Snds[j][k];
				if (pSnd) {
					Monsters[i].Snds[j][k] = NULL;
					file = pSnd->sound_path;
					pSnd->sound_path = NULL;
					sound_file_cleanup(pSnd);
					mem_free_dbg(file);
				}
			}
		}
	}
}

void PlayEffect(int i, int mode)
{
	int sndIdx, mi, lVolume, lPan;
	TSnd *snd;

	if (plr[myplr].pLvlLoad) {
		return;
	}

	sndIdx = random(164, 2);
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

BOOL calc_snd_position(int x, int y, int *plVolume, int *plPan)
{
	int pan, volume;

	x -= plr[myplr].WorldX;
	y -= plr[myplr].WorldY;

	pan = (x - y) << 8;
	*plPan = pan;

	if (abs(pan) > 6400)
		return FALSE;

	volume = abs(x) > abs(y) ? abs(x) : abs(y);
	volume <<= 6;
	*plVolume = volume;

	if (volume >= 6400)
		return FALSE;

	*plVolume = -volume;

	return TRUE;
}

void PlaySFX(int psfx)
{
	psfx = RndSFX(psfx);
	PlaySFX_priv(&sgSFX[psfx], 0, 0, 0);
}

void PlaySFX_priv(TSFX *pSFX, BOOL loc, int x, int y)
{
	int lPan, lVolume;

	if (plr[myplr].pLvlLoad && gbMaxPlayers != 1) {
		return;
	}
	if (!gbSndInited || !gbSoundOn || gbBufferMsgs) {
		return;
	}

	if (!(pSFX->bFlags & (SFX_STREAM | SFX_MISC)) && pSFX->pSnd != 0 && snd_playing(pSFX->pSnd)) {
		return;
	}

	lPan = 0;
	lVolume = 0;
	if (loc && !calc_snd_position(x, y, &lVolume, &lPan)) {
		return;
	}

	if (pSFX->bFlags & SFX_STREAM) {
		stream_play(pSFX, lVolume, lPan);
		return;
	}

	if (!pSFX->pSnd)
		pSFX->pSnd = sound_file_load(pSFX->pszName);

	if (pSFX->pSnd)
		snd_play_snd(pSFX->pSnd, lVolume, lPan);
}

void stream_play(TSFX *pSFX, int lVolume, int lPan)
{
	BOOL success;

	/// ASSERT: assert(pSFX);
	/// ASSERT: assert(pSFX->bFlags & sfx_STREAM);
	sfx_stop();
	lVolume += sound_get_or_set_sound_volume(1);
	if (lVolume >= VOLUME_MIN) {
		if (lVolume > VOLUME_MAX)
			lVolume = VOLUME_MAX;
#ifdef _DEBUG
		SFileEnableDirectAccess(FALSE);
#endif
		success = SFileOpenFile(pSFX->pszName, &sfx_stream);
#ifdef _DEBUG
		SFileEnableDirectAccess(TRUE);
#endif
		if (!success) {
			sfx_stream = 0;
		} else {
			if (!SFileDdaBeginEx(sfx_stream, 0x40000, 0, 0, lVolume, lPan, 0))
				sfx_stop();
			else
				sfx_data_cur = pSFX;
		}
	}
}

int RndSFX(int psfx)
{
	int nRand;

	if (psfx == PS_WARR69)
		nRand = 2;
	else if (psfx == PS_WARR14)
		nRand = 3;
	else if (psfx == PS_WARR15)
		nRand = 3;
	else if (psfx == PS_WARR16)
		nRand = 3;
	else if (psfx == PS_MAGE69)
		nRand = 2;
	else if (psfx == PS_ROGUE69)
		nRand = 2;
	else if (psfx == PS_SWING)
		nRand = 2;
	else if (psfx == LS_ACID)
		nRand = 2;
	else if (psfx == IS_FMAG)
		nRand = 2;
	else if (psfx == IS_MAGIC)
		nRand = 2;
	else if (psfx == IS_BHIT)
		nRand = 2;
	else if (psfx == PS_WARR2)
		nRand = 3;
	else
		return psfx;
	return psfx + random(165, nRand);
}

void PlaySfxLoc(int psfx, int x, int y)
{
	TSnd *pSnd;

	psfx = RndSFX(psfx);

	if (psfx >= 0 && psfx <= 3) {
		pSnd = sgSFX[psfx].pSnd;
		if (pSnd)
			pSnd->start_tc = 0;
	}

	PlaySFX_priv(&sgSFX[psfx], 1, x, y);
}

void FreeMonsterSnd()
{
	int i, j, k;

	snd_update(TRUE);
	sfx_stop();
	sound_stop();

	for (i = 0; i < nummtypes; i++) {
		for (j = 0; j < 4; j++) {
			for (k = 0; k < 2; k++) {
				snd_stop_snd(Monsters[i].Snds[j][k]);
			}
		}
	}
}

void sound_stop()
{
	int i;
	TSFX *snd;

	snd = &sgSFX[0];
	for (i = 0; i < NUM_SFX; i++) {
		if (snd->pSnd)
			snd_stop_snd(snd->pSnd);
		snd++;
	}
}

void sound_update()
{
	if (!gbSndInited) {
		return;
	}

	snd_update(FALSE);
	effects_update();
}

void effects_update()
{
	DWORD current, end;

	if (sfx_stream != NULL && SFileDdaGetPos(sfx_stream, &current, &end) && current >= end) {
		sfx_stop();
	}
}

void effects_cleanup_sfx()
{
	DWORD i;

	FreeMonsterSnd();

	for (i = 0; i < NUM_SFX; i++) {
		if (sgSFX[i].pSnd) {
			sound_file_cleanup(sgSFX[i].pSnd);
			sgSFX[i].pSnd = NULL;
		}
	}
}

void stream_update()
{
	BYTE mask = 0;
	if (gbMaxPlayers > 1) {
		mask = SFX_WARRIOR | SFX_ROGUE | SFX_SORCEROR;
	} else if (plr[myplr]._pClass == PC_WARRIOR) {
		mask = SFX_WARRIOR;
	} else if (plr[myplr]._pClass == PC_ROGUE) {
		mask = SFX_ROGUE;
	} else if (plr[myplr]._pClass == PC_SORCERER) {
		mask = SFX_SORCEROR;
	} else {
		app_fatal("effects:1");
	}

	priv_sound_init(mask);
}

void priv_sound_init(BYTE bLoadMask)
{
	BYTE pc, bFlags;
	DWORD i;

	if (!gbSndInited) {
		return;
	}

	pc = bLoadMask & (SFX_ROGUE | SFX_WARRIOR | SFX_SORCEROR);
	bLoadMask ^= pc;

	for (i = 0; i < NUM_SFX; i++) {
		if (sgSFX[i].pSnd) {
			continue;
		}

		bFlags = sgSFX[i].bFlags;
		if (bFlags & SFX_STREAM) {
			continue;
		}

		if (bLoadMask && !(bFlags & bLoadMask)) {
			continue;
		}

		if (bFlags & (SFX_ROGUE | SFX_WARRIOR | SFX_SORCEROR) && !(bFlags & pc)) {
			continue;
		}

		sgSFX[i].pSnd = sound_file_load(sgSFX[i].pszName);
	}
}

void sound_init()
{
	priv_sound_init(SFX_UI);
}

void __stdcall effects_play_sound(char *snd_file)
{
	DWORD i;

	if (!gbSndInited || !gbSoundOn) {
		return;
	}

	for (i = 0; i < NUM_SFX; i++) {
		if (!_strcmpi(sgSFX[i].pszName, snd_file) && sgSFX[i].pSnd) {
			if (!snd_playing(sgSFX[i].pSnd))
				snd_play_snd(sgSFX[i].pSnd, 0, 0);

			return;
		}
	}
}

DEVILUTION_END_NAMESPACE
