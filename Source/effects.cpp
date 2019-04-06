//HEADER_GOES_HERE

#include "../types.h"

DEVILUTION_BEGIN_NAMESPACE

int sfxdelay; // weak
int sfxdnum;
HANDLE sfx_stream;
TSFX *sfx_data_cur;

const char monster_action_sounds[] = { 'a', 'h', 'd', 's' }; // idb

/* data */

TSFX sgSFX[NUM_SFX] = {
	// clang-format off
	// bFlags, pszName,                       pSnd
	{       2, "Sfx\\Misc\\Walk1.wav",        NULL },
	{       2, "Sfx\\Misc\\Walk2.wav",        NULL },
	{       2, "Sfx\\Misc\\Walk3.wav",        NULL },
	{       2, "Sfx\\Misc\\Walk4.wav",        NULL },
	{       2, "Sfx\\Misc\\BFire.wav",        NULL },
	{       2, "Sfx\\Misc\\Fmag.wav",         NULL },
	{       2, "Sfx\\Misc\\Tmag.wav",         NULL },
	{       2, "Sfx\\Misc\\Lghit.wav",        NULL },
	{       2, "Sfx\\Misc\\Lghit1.wav",       NULL },
	{       2, "Sfx\\Misc\\Swing.wav",        NULL },
	{       2, "Sfx\\Misc\\Swing2.wav",       NULL },
	{       2, "Sfx\\Misc\\Dead.wav",         NULL },
	{       1, "Sfx\\Misc\\Questdon.wav",     NULL },
	{       2, "Sfx\\Items\\Armrfkd.wav",     NULL },
	{       2, "Sfx\\Items\\Barlfire.wav",    NULL },
	{       2, "Sfx\\Items\\Barrel.wav",      NULL },
	{       2, "Sfx\\Items\\Bhit.wav",        NULL },
	{       2, "Sfx\\Items\\Bhit1.wav",       NULL },
	{       2, "Sfx\\Items\\Chest.wav",       NULL },
	{       2, "Sfx\\Items\\Doorclos.wav",    NULL },
	{       2, "Sfx\\Items\\Dooropen.wav",    NULL },
	{       2, "Sfx\\Items\\Flipanvl.wav",    NULL },
	{       2, "Sfx\\Items\\Flipaxe.wav",     NULL },
	{       2, "Sfx\\Items\\Flipblst.wav",    NULL },
	{       2, "Sfx\\Items\\Flipbody.wav",    NULL },
	{       2, "Sfx\\Items\\Flipbook.wav",    NULL },
	{       2, "Sfx\\Items\\Flipbow.wav",     NULL },
	{       2, "Sfx\\Items\\Flipcap.wav",     NULL },
	{       2, "Sfx\\Items\\Flipharm.wav",    NULL },
	{       2, "Sfx\\Items\\Fliplarm.wav",    NULL },
	{       2, "Sfx\\Items\\Flipmag.wav",     NULL },
	{       2, "Sfx\\Items\\Flipmag1.wav",    NULL },
	{       2, "Sfx\\Items\\Flipmush.wav",    NULL },
	{       2, "Sfx\\Items\\Flippot.wav",     NULL },
	{       2, "Sfx\\Items\\Flipring.wav",    NULL },
	{       2, "Sfx\\Items\\Fliprock.wav",    NULL },
	{       2, "Sfx\\Items\\Flipscrl.wav",    NULL },
	{       2, "Sfx\\Items\\Flipshld.wav",    NULL },
	{       2, "Sfx\\Items\\Flipsign.wav",    NULL },
	{       2, "Sfx\\Items\\Flipstaf.wav",    NULL },
	{       2, "Sfx\\Items\\Flipswor.wav",    NULL },
	{       2, "Sfx\\Items\\Gold.wav",        NULL },
	{       2, "Sfx\\Items\\Hlmtfkd.wav",     NULL },
	{       2, "Sfx\\Items\\Invanvl.wav",     NULL },
	{       2, "Sfx\\Items\\Invaxe.wav",      NULL },
	{       2, "Sfx\\Items\\Invblst.wav",     NULL },
	{       2, "Sfx\\Items\\Invbody.wav",     NULL },
	{       2, "Sfx\\Items\\Invbook.wav",     NULL },
	{       2, "Sfx\\Items\\Invbow.wav",      NULL },
	{       2, "Sfx\\Items\\Invcap.wav",      NULL },
	{       2, "Sfx\\Items\\Invgrab.wav",     NULL },
	{       2, "Sfx\\Items\\Invharm.wav",     NULL },
	{       2, "Sfx\\Items\\Invlarm.wav",     NULL },
	{       2, "Sfx\\Items\\Invmush.wav",     NULL },
	{       2, "Sfx\\Items\\Invpot.wav",      NULL },
	{       2, "Sfx\\Items\\Invring.wav",     NULL },
	{       2, "Sfx\\Items\\Invrock.wav",     NULL },
	{       2, "Sfx\\Items\\Invscrol.wav",    NULL },
	{       2, "Sfx\\Items\\Invshiel.wav",    NULL },
	{       2, "Sfx\\Items\\Invsign.wav",     NULL },
	{       2, "Sfx\\Items\\Invstaf.wav",     NULL },
	{       2, "Sfx\\Items\\Invsword.wav",    NULL },
	{       2, "Sfx\\Items\\Lever.wav",       NULL },
	{       2, "Sfx\\Items\\Magic.wav",       NULL },
	{       2, "Sfx\\Items\\Magic1.wav",      NULL },
	{       2, "Sfx\\Items\\Readbook.wav",    NULL },
	{       2, "Sfx\\Items\\Sarc.wav",        NULL },
	{       2, "Sfx\\Items\\Shielfkd.wav",    NULL },
	{       2, "Sfx\\Items\\Swrdfkd.wav",     NULL },
	{       4, "Sfx\\Items\\Titlemov.wav",    NULL },
	{       4, "Sfx\\Items\\Titlslct.wav",    NULL },
	{       4, "Sfx\\Misc\\blank.wav",        NULL },
	{       2, "Sfx\\Items\\Trap.wav",        NULL },
	{       2, "Sfx\\Misc\\Cast1.wav",        NULL },
	{       2, "Sfx\\Misc\\Cast10.wav",       NULL },
	{       2, "Sfx\\Misc\\Cast12.wav",       NULL },
	{       2, "Sfx\\Misc\\Cast2.wav",        NULL },
	{       2, "Sfx\\Misc\\Cast3.wav",        NULL },
	{       2, "Sfx\\Misc\\Cast4.wav",        NULL },
	{       2, "Sfx\\Misc\\Cast5.wav",        NULL },
	{       2, "Sfx\\Misc\\Cast6.wav",        NULL },
	{       2, "Sfx\\Misc\\Cast7.wav",        NULL },
	{       2, "Sfx\\Misc\\Cast8.wav",        NULL },
	{       2, "Sfx\\Misc\\Cast9.wav",        NULL },
	{       2, "Sfx\\Misc\\Healing.wav",      NULL },
	{       2, "Sfx\\Misc\\Repair.wav",       NULL },
	{       2, "Sfx\\Misc\\Acids1.wav",       NULL },
	{       2, "Sfx\\Misc\\Acids2.wav",       NULL },
	{       2, "Sfx\\Misc\\Apoc.wav",         NULL },
	{       2, "Sfx\\Misc\\Arrowall.wav",     NULL },
	{       2, "Sfx\\Misc\\Bldboil.wav",      NULL },
	{       2, "Sfx\\Misc\\Blodstar.wav",     NULL },
	{       2, "Sfx\\Misc\\Blsimpt.wav",      NULL },
	{       2, "Sfx\\Misc\\Bonesp.wav",       NULL },
	{       2, "Sfx\\Misc\\Bsimpct.wav",      NULL },
	{       2, "Sfx\\Misc\\Caldron.wav",      NULL },
	{       2, "Sfx\\Misc\\Cbolt.wav",        NULL },
	{       2, "Sfx\\Misc\\Chltning.wav",     NULL },
	{       2, "Sfx\\Misc\\DSerp.wav",        NULL },
	{       2, "Sfx\\Misc\\Elecimp1.wav",     NULL },
	{       2, "Sfx\\Misc\\Elementl.wav",     NULL },
	{       2, "Sfx\\Misc\\Ethereal.wav",     NULL },
	{       2, "Sfx\\Misc\\Fball.wav",        NULL },
	{       2, "Sfx\\Misc\\Fbolt1.wav",       NULL },
	{       2, "Sfx\\Misc\\Fbolt2.wav",       NULL },
	{       2, "Sfx\\Misc\\Firimp1.wav",      NULL },
	{       2, "Sfx\\Misc\\Firimp2.wav",      NULL },
	{       2, "Sfx\\Misc\\Flamwave.wav",     NULL },
	{       2, "Sfx\\Misc\\Flash.wav",        NULL },
	{       2, "Sfx\\Misc\\Fountain.wav",     NULL },
	{       2, "Sfx\\Misc\\Golum.wav",        NULL },
	{       2, "Sfx\\Misc\\Golumded.wav",     NULL },
	{       2, "Sfx\\Misc\\Gshrine.wav",      NULL },
	{       2, "Sfx\\Misc\\Guard.wav",        NULL },
	{       2, "Sfx\\Misc\\Grdlanch.wav",     NULL },
	{       2, "Sfx\\Misc\\Holybolt.wav",     NULL },
	{       2, "Sfx\\Misc\\Hyper.wav",        NULL },
	{       2, "Sfx\\Misc\\Infravis.wav",     NULL },
	{       2, "Sfx\\Misc\\Invisibl.wav",     NULL },
	{       2, "Sfx\\Misc\\Invpot.wav",       NULL },
	{       2, "Sfx\\Misc\\Lning1.wav",       NULL },
	{       2, "Sfx\\Misc\\Ltning.wav",       NULL },
	{       2, "Sfx\\Misc\\Mshield.wav",      NULL },
	{       2, "Sfx\\Misc\\Nova.wav",         NULL },
	{       2, "Sfx\\Misc\\Portal.wav",       NULL },
	{       2, "Sfx\\Misc\\Puddle.wav",       NULL },
	{       2, "Sfx\\Misc\\Resur.wav",        NULL },
	{       2, "Sfx\\Misc\\Scurse.wav",       NULL },
	{       2, "Sfx\\Misc\\Scurimp.wav",      NULL },
	{       2, "Sfx\\Misc\\Sentinel.wav",     NULL },
	{       2, "Sfx\\Misc\\Shatter.wav",      NULL },
	{       2, "Sfx\\Misc\\Soulfire.wav",     NULL },
	{       2, "Sfx\\Misc\\Spoutlop.wav",     NULL },
	{       2, "Sfx\\Misc\\Spoutstr.wav",     NULL },
	{       2, "Sfx\\Misc\\Storm.wav",        NULL },
	{       2, "Sfx\\Misc\\Trapdis.wav",      NULL },
	{       2, "Sfx\\Misc\\Teleport.wav",     NULL },
	{       2, "Sfx\\Misc\\Vtheft.wav",       NULL },
	{       2, "Sfx\\Misc\\Wallloop.wav",     NULL },
	{       2, "Sfx\\Misc\\Wallstrt.wav",     NULL },
	{       1, "Sfx\\Towners\\Bmaid01.wav",   NULL },
	{       1, "Sfx\\Towners\\Bmaid02.wav",   NULL },
	{       1, "Sfx\\Towners\\Bmaid03.wav",   NULL },
	{       1, "Sfx\\Towners\\Bmaid04.wav",   NULL },
	{       1, "Sfx\\Towners\\Bmaid05.wav",   NULL },
	{       1, "Sfx\\Towners\\Bmaid06.wav",   NULL },
	{       1, "Sfx\\Towners\\Bmaid07.wav",   NULL },
	{       1, "Sfx\\Towners\\Bmaid08.wav",   NULL },
	{       1, "Sfx\\Towners\\Bmaid09.wav",   NULL },
	{       1, "Sfx\\Towners\\Bmaid10.wav",   NULL },
	{       1, "Sfx\\Towners\\Bmaid11.wav",   NULL },
	{       1, "Sfx\\Towners\\Bmaid12.wav",   NULL },
	{       1, "Sfx\\Towners\\Bmaid13.wav",   NULL },
	{       1, "Sfx\\Towners\\Bmaid14.wav",   NULL },
	{       1, "Sfx\\Towners\\Bmaid15.wav",   NULL },
	{       1, "Sfx\\Towners\\Bmaid16.wav",   NULL },
	{       1, "Sfx\\Towners\\Bmaid17.wav",   NULL },
	{       1, "Sfx\\Towners\\Bmaid18.wav",   NULL },
	{       1, "Sfx\\Towners\\Bmaid19.wav",   NULL },
	{       1, "Sfx\\Towners\\Bmaid20.wav",   NULL },
	{       1, "Sfx\\Towners\\Bmaid21.wav",   NULL },
	{       1, "Sfx\\Towners\\Bmaid22.wav",   NULL },
	{       1, "Sfx\\Towners\\Bmaid23.wav",   NULL },
	{       1, "Sfx\\Towners\\Bmaid24.wav",   NULL },
	{       1, "Sfx\\Towners\\Bmaid25.wav",   NULL },
	{       1, "Sfx\\Towners\\Bmaid26.wav",   NULL },
	{       1, "Sfx\\Towners\\Bmaid27.wav",   NULL },
	{       1, "Sfx\\Towners\\Bmaid28.wav",   NULL },
	{       1, "Sfx\\Towners\\Bmaid29.wav",   NULL },
	{       1, "Sfx\\Towners\\Bmaid30.wav",   NULL },
	{       1, "Sfx\\Towners\\Bmaid31.wav",   NULL },
	{       1, "Sfx\\Towners\\Bmaid32.wav",   NULL },
	{       1, "Sfx\\Towners\\Bmaid33.wav",   NULL },
	{       1, "Sfx\\Towners\\Bmaid34.wav",   NULL },
	{       1, "Sfx\\Towners\\Bmaid35.wav",   NULL },
	{       1, "Sfx\\Towners\\Bmaid36.wav",   NULL },
	{       1, "Sfx\\Towners\\Bmaid37.wav",   NULL },
	{       1, "Sfx\\Towners\\Bmaid38.wav",   NULL },
	{       1, "Sfx\\Towners\\Bmaid39.wav",   NULL },
	{       1, "Sfx\\Towners\\Bmaid40.wav",   NULL },
	{       1, "Sfx\\Towners\\Bsmith01.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith02.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith03.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith04.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith05.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith06.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith07.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith08.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith09.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith10.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith11.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith12.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith13.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith14.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith15.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith16.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith17.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith18.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith19.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith20.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith21.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith22.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith23.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith24.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith25.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith26.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith27.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith28.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith29.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith30.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith31.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith32.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith33.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith34.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith35.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith36.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith37.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith38.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith39.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith40.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith41.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith42.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith43.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith44.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith45.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith46.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith47.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith48.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith49.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith50.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith51.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith52.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith53.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith54.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith55.wav",  NULL },
	{       1, "Sfx\\Towners\\Bsmith56.wav",  NULL },
	{       0, "Sfx\\Towners\\Cow1.wav",      NULL },
	{       0, "Sfx\\Towners\\Cow2.wav",      NULL },
	{       1, "Sfx\\Towners\\Deadguy2.wav",  NULL },
	{       1, "Sfx\\Towners\\Drunk01.wav",   NULL },
	{       1, "Sfx\\Towners\\Drunk02.wav",   NULL },
	{       1, "Sfx\\Towners\\Drunk03.wav",   NULL },
	{       1, "Sfx\\Towners\\Drunk04.wav",   NULL },
	{       1, "Sfx\\Towners\\Drunk05.wav",   NULL },
	{       1, "Sfx\\Towners\\Drunk06.wav",   NULL },
	{       1, "Sfx\\Towners\\Drunk07.wav",   NULL },
	{       1, "Sfx\\Towners\\Drunk08.wav",   NULL },
	{       1, "Sfx\\Towners\\Drunk09.wav",   NULL },
	{       1, "Sfx\\Towners\\Drunk10.wav",   NULL },
	{       1, "Sfx\\Towners\\Drunk11.wav",   NULL },
	{       1, "Sfx\\Towners\\Drunk12.wav",   NULL },
	{       1, "Sfx\\Towners\\Drunk13.wav",   NULL },
	{       1, "Sfx\\Towners\\Drunk14.wav",   NULL },
	{       1, "Sfx\\Towners\\Drunk15.wav",   NULL },
	{       1, "Sfx\\Towners\\Drunk16.wav",   NULL },
	{       1, "Sfx\\Towners\\Drunk17.wav",   NULL },
	{       1, "Sfx\\Towners\\Drunk18.wav",   NULL },
	{       1, "Sfx\\Towners\\Drunk19.wav",   NULL },
	{       1, "Sfx\\Towners\\Drunk20.wav",   NULL },
	{       1, "Sfx\\Towners\\Drunk21.wav",   NULL },
	{       1, "Sfx\\Towners\\Drunk22.wav",   NULL },
	{       1, "Sfx\\Towners\\Drunk23.wav",   NULL },
	{       1, "Sfx\\Towners\\Drunk24.wav",   NULL },
	{       1, "Sfx\\Towners\\Drunk25.wav",   NULL },
	{       1, "Sfx\\Towners\\Drunk26.wav",   NULL },
	{       1, "Sfx\\Towners\\Drunk27.wav",   NULL },
	{       1, "Sfx\\Towners\\Drunk28.wav",   NULL },
	{       1, "Sfx\\Towners\\Drunk29.wav",   NULL },
	{       1, "Sfx\\Towners\\Drunk30.wav",   NULL },
	{       1, "Sfx\\Towners\\Drunk31.wav",   NULL },
	{       1, "Sfx\\Towners\\Drunk32.wav",   NULL },
	{       1, "Sfx\\Towners\\Drunk33.wav",   NULL },
	{       1, "Sfx\\Towners\\Drunk34.wav",   NULL },
	{       1, "Sfx\\Towners\\Drunk35.wav",   NULL },
	{       1, "Sfx\\Towners\\Healer01.wav",  NULL },
	{       1, "Sfx\\Towners\\Healer02.wav",  NULL },
	{       1, "Sfx\\Towners\\Healer03.wav",  NULL },
	{       1, "Sfx\\Towners\\Healer04.wav",  NULL },
	{       1, "Sfx\\Towners\\Healer05.wav",  NULL },
	{       1, "Sfx\\Towners\\Healer06.wav",  NULL },
	{       1, "Sfx\\Towners\\Healer07.wav",  NULL },
	{       1, "Sfx\\Towners\\Healer08.wav",  NULL },
	{       1, "Sfx\\Towners\\Healer09.wav",  NULL },
	{       1, "Sfx\\Towners\\Healer10.wav",  NULL },
	{       1, "Sfx\\Towners\\Healer11.wav",  NULL },
	{       1, "Sfx\\Towners\\Healer12.wav",  NULL },
	{       1, "Sfx\\Towners\\Healer13.wav",  NULL },
	{       1, "Sfx\\Towners\\Healer14.wav",  NULL },
	{       1, "Sfx\\Towners\\Healer15.wav",  NULL },
	{       1, "Sfx\\Towners\\Healer16.wav",  NULL },
	{       1, "Sfx\\Towners\\Healer17.wav",  NULL },
	{       1, "Sfx\\Towners\\Healer18.wav",  NULL },
	{       1, "Sfx\\Towners\\Healer19.wav",  NULL },
	{       1, "Sfx\\Towners\\Healer20.wav",  NULL },
	{       1, "Sfx\\Towners\\Healer21.wav",  NULL },
	{       1, "Sfx\\Towners\\Healer22.wav",  NULL },
	{       1, "Sfx\\Towners\\Healer23.wav",  NULL },
	{       1, "Sfx\\Towners\\Healer24.wav",  NULL },
	{       1, "Sfx\\Towners\\Healer25.wav",  NULL },
	{       1, "Sfx\\Towners\\Healer26.wav",  NULL },
	{       1, "Sfx\\Towners\\Healer27.wav",  NULL },
	{       1, "Sfx\\Towners\\Healer28.wav",  NULL },
	{       1, "Sfx\\Towners\\Healer29.wav",  NULL },
	{       1, "Sfx\\Towners\\Healer30.wav",  NULL },
	{       1, "Sfx\\Towners\\Healer31.wav",  NULL },
	{       1, "Sfx\\Towners\\Healer32.wav",  NULL },
	{       1, "Sfx\\Towners\\Healer33.wav",  NULL },
	{       1, "Sfx\\Towners\\Healer34.wav",  NULL },
	{       1, "Sfx\\Towners\\Healer35.wav",  NULL },
	{       1, "Sfx\\Towners\\Healer36.wav",  NULL },
	{       1, "Sfx\\Towners\\Healer37.wav",  NULL },
	{       1, "Sfx\\Towners\\Healer38.wav",  NULL },
	{       1, "Sfx\\Towners\\Healer39.wav",  NULL },
	{       1, "Sfx\\Towners\\Healer40.wav",  NULL },
	{       1, "Sfx\\Towners\\Healer41.wav",  NULL },
	{       1, "Sfx\\Towners\\Healer42.wav",  NULL },
	{       1, "Sfx\\Towners\\Healer43.wav",  NULL },
	{       1, "Sfx\\Towners\\Healer44.wav",  NULL },
	{       1, "Sfx\\Towners\\Healer45.wav",  NULL },
	{       1, "Sfx\\Towners\\Healer46.wav",  NULL },
	{       1, "Sfx\\Towners\\Healer47.wav",  NULL },
	{       1, "Sfx\\Towners\\Pegboy01.wav",  NULL },
	{       1, "Sfx\\Towners\\Pegboy02.wav",  NULL },
	{       1, "Sfx\\Towners\\Pegboy03.wav",  NULL },
	{       1, "Sfx\\Towners\\Pegboy04.wav",  NULL },
	{       1, "Sfx\\Towners\\Pegboy05.wav",  NULL },
	{       1, "Sfx\\Towners\\Pegboy06.wav",  NULL },
	{       1, "Sfx\\Towners\\Pegboy07.wav",  NULL },
	{       1, "Sfx\\Towners\\Pegboy08.wav",  NULL },
	{       1, "Sfx\\Towners\\Pegboy09.wav",  NULL },
	{       1, "Sfx\\Towners\\Pegboy10.wav",  NULL },
	{       1, "Sfx\\Towners\\Pegboy11.wav",  NULL },
	{       1, "Sfx\\Towners\\Pegboy12.wav",  NULL },
	{       1, "Sfx\\Towners\\Pegboy13.wav",  NULL },
	{       1, "Sfx\\Towners\\Pegboy14.wav",  NULL },
	{       1, "Sfx\\Towners\\Pegboy15.wav",  NULL },
	{       1, "Sfx\\Towners\\Pegboy16.wav",  NULL },
	{       1, "Sfx\\Towners\\Pegboy17.wav",  NULL },
	{       1, "Sfx\\Towners\\Pegboy18.wav",  NULL },
	{       1, "Sfx\\Towners\\Pegboy19.wav",  NULL },
	{       1, "Sfx\\Towners\\Pegboy20.wav",  NULL },
	{       1, "Sfx\\Towners\\Pegboy21.wav",  NULL },
	{       1, "Sfx\\Towners\\Pegboy22.wav",  NULL },
	{       1, "Sfx\\Towners\\Pegboy23.wav",  NULL },
	{       1, "Sfx\\Towners\\Pegboy24.wav",  NULL },
	{       1, "Sfx\\Towners\\Pegboy25.wav",  NULL },
	{       1, "Sfx\\Towners\\Pegboy26.wav",  NULL },
	{       1, "Sfx\\Towners\\Pegboy27.wav",  NULL },
	{       1, "Sfx\\Towners\\Pegboy28.wav",  NULL },
	{       1, "Sfx\\Towners\\Pegboy29.wav",  NULL },
	{       1, "Sfx\\Towners\\Pegboy30.wav",  NULL },
	{       1, "Sfx\\Towners\\Pegboy31.wav",  NULL },
	{       1, "Sfx\\Towners\\Pegboy32.wav",  NULL },
	{       1, "Sfx\\Towners\\Pegboy33.wav",  NULL },
	{       1, "Sfx\\Towners\\Pegboy34.wav",  NULL },
	{       1, "Sfx\\Towners\\Pegboy35.wav",  NULL },
	{       1, "Sfx\\Towners\\Pegboy36.wav",  NULL },
	{       1, "Sfx\\Towners\\Pegboy37.wav",  NULL },
	{       1, "Sfx\\Towners\\Pegboy38.wav",  NULL },
	{       1, "Sfx\\Towners\\Pegboy39.wav",  NULL },
	{       1, "Sfx\\Towners\\Pegboy40.wav",  NULL },
	{       1, "Sfx\\Towners\\Pegboy41.wav",  NULL },
	{       1, "Sfx\\Towners\\Pegboy42.wav",  NULL },
	{       1, "Sfx\\Towners\\Pegboy43.wav",  NULL },
	{       1, "Sfx\\Towners\\Priest00.wav",  NULL },
	{       1, "Sfx\\Towners\\Priest01.wav",  NULL },
	{       1, "Sfx\\Towners\\Priest02.wav",  NULL },
	{       1, "Sfx\\Towners\\Priest03.wav",  NULL },
	{       1, "Sfx\\Towners\\Priest04.wav",  NULL },
	{       1, "Sfx\\Towners\\Priest05.wav",  NULL },
	{       1, "Sfx\\Towners\\Priest06.wav",  NULL },
	{       1, "Sfx\\Towners\\Priest07.wav",  NULL },
	{       1, "Sfx\\Towners\\Storyt00.wav",  NULL },
	{       1, "Sfx\\Towners\\Storyt01.wav",  NULL },
	{       1, "Sfx\\Towners\\Storyt02.wav",  NULL },
	{       1, "Sfx\\Towners\\Storyt03.wav",  NULL },
	{       1, "Sfx\\Towners\\Storyt04.wav",  NULL },
	{       1, "Sfx\\Towners\\Storyt05.wav",  NULL },
	{       1, "Sfx\\Towners\\Storyt06.wav",  NULL },
	{       1, "Sfx\\Towners\\Storyt07.wav",  NULL },
	{       1, "Sfx\\Towners\\Storyt08.wav",  NULL },
	{       1, "Sfx\\Towners\\Storyt09.wav",  NULL },
	{       1, "Sfx\\Towners\\Storyt10.wav",  NULL },
	{       1, "Sfx\\Towners\\Storyt11.wav",  NULL },
	{       1, "Sfx\\Towners\\Storyt12.wav",  NULL },
	{       1, "Sfx\\Towners\\Storyt13.wav",  NULL },
	{       1, "Sfx\\Towners\\Storyt14.wav",  NULL },
	{       1, "Sfx\\Towners\\Storyt15.wav",  NULL },
	{       1, "Sfx\\Towners\\Storyt16.wav",  NULL },
	{       1, "Sfx\\Towners\\Storyt17.wav",  NULL },
	{       1, "Sfx\\Towners\\Storyt18.wav",  NULL },
	{       1, "Sfx\\Towners\\Storyt19.wav",  NULL },
	{       1, "Sfx\\Towners\\Storyt20.wav",  NULL },
	{       1, "Sfx\\Towners\\Storyt21.wav",  NULL },
	{       1, "Sfx\\Towners\\Storyt22.wav",  NULL },
	{       1, "Sfx\\Towners\\Storyt23.wav",  NULL },
	{       1, "Sfx\\Towners\\Storyt24.wav",  NULL },
	{       1, "Sfx\\Towners\\Storyt25.wav",  NULL },
	{       1, "Sfx\\Towners\\Storyt26.wav",  NULL },
	{       1, "Sfx\\Towners\\Storyt27.wav",  NULL },
	{       1, "Sfx\\Towners\\Storyt28.wav",  NULL },
	{       1, "Sfx\\Towners\\Storyt29.wav",  NULL },
	{       1, "Sfx\\Towners\\Storyt30.wav",  NULL },
	{       1, "Sfx\\Towners\\Storyt31.wav",  NULL },
	{       1, "Sfx\\Towners\\Storyt32.wav",  NULL },
	{       1, "Sfx\\Towners\\Storyt33.wav",  NULL },
	{       1, "Sfx\\Towners\\Storyt34.wav",  NULL },
	{       1, "Sfx\\Towners\\Storyt35.wav",  NULL },
	{       1, "Sfx\\Towners\\Storyt36.wav",  NULL },
	{       1, "Sfx\\Towners\\Storyt37.wav",  NULL },
	{       1, "Sfx\\Towners\\Storyt38.wav",  NULL },
	{       1, "Sfx\\Towners\\Tavown00.wav",  NULL },
	{       1, "Sfx\\Towners\\Tavown01.wav",  NULL },
	{       1, "Sfx\\Towners\\Tavown02.wav",  NULL },
	{       1, "Sfx\\Towners\\Tavown03.wav",  NULL },
	{       1, "Sfx\\Towners\\Tavown04.wav",  NULL },
	{       1, "Sfx\\Towners\\Tavown05.wav",  NULL },
	{       1, "Sfx\\Towners\\Tavown06.wav",  NULL },
	{       1, "Sfx\\Towners\\Tavown07.wav",  NULL },
	{       1, "Sfx\\Towners\\Tavown08.wav",  NULL },
	{       1, "Sfx\\Towners\\Tavown09.wav",  NULL },
	{       1, "Sfx\\Towners\\Tavown10.wav",  NULL },
	{       1, "Sfx\\Towners\\Tavown11.wav",  NULL },
	{       1, "Sfx\\Towners\\Tavown12.wav",  NULL },
	{       1, "Sfx\\Towners\\Tavown13.wav",  NULL },
	{       1, "Sfx\\Towners\\Tavown14.wav",  NULL },
	{       1, "Sfx\\Towners\\Tavown15.wav",  NULL },
	{       1, "Sfx\\Towners\\Tavown16.wav",  NULL },
	{       1, "Sfx\\Towners\\Tavown17.wav",  NULL },
	{       1, "Sfx\\Towners\\Tavown18.wav",  NULL },
	{       1, "Sfx\\Towners\\Tavown19.wav",  NULL },
	{       1, "Sfx\\Towners\\Tavown20.wav",  NULL },
	{       1, "Sfx\\Towners\\Tavown21.wav",  NULL },
	{       1, "Sfx\\Towners\\Tavown22.wav",  NULL },
	{       1, "Sfx\\Towners\\Tavown23.wav",  NULL },
	{       1, "Sfx\\Towners\\Tavown24.wav",  NULL },
	{       1, "Sfx\\Towners\\Tavown25.wav",  NULL },
	{       1, "Sfx\\Towners\\Tavown26.wav",  NULL },
	{       1, "Sfx\\Towners\\Tavown27.wav",  NULL },
	{       1, "Sfx\\Towners\\Tavown28.wav",  NULL },
	{       1, "Sfx\\Towners\\Tavown29.wav",  NULL },
	{       1, "Sfx\\Towners\\Tavown30.wav",  NULL },
	{       1, "Sfx\\Towners\\Tavown31.wav",  NULL },
	{       1, "Sfx\\Towners\\Tavown32.wav",  NULL },
	{       1, "Sfx\\Towners\\Tavown33.wav",  NULL },
	{       1, "Sfx\\Towners\\Tavown34.wav",  NULL },
	{       1, "Sfx\\Towners\\Tavown35.wav",  NULL },
	{       1, "Sfx\\Towners\\Tavown36.wav",  NULL },
	{       1, "Sfx\\Towners\\Tavown37.wav",  NULL },
	{       1, "Sfx\\Towners\\Tavown38.wav",  NULL },
	{       1, "Sfx\\Towners\\Tavown39.wav",  NULL },
	{       1, "Sfx\\Towners\\Tavown40.wav",  NULL },
	{       1, "Sfx\\Towners\\Tavown41.wav",  NULL },
	{       1, "Sfx\\Towners\\Tavown42.wav",  NULL },
	{       1, "Sfx\\Towners\\Tavown43.wav",  NULL },
	{       1, "Sfx\\Towners\\Tavown44.wav",  NULL },
	{       1, "Sfx\\Towners\\Tavown45.wav",  NULL },
	{       1, "Sfx\\Towners\\Witch01.wav",   NULL },
	{       1, "Sfx\\Towners\\Witch02.wav",   NULL },
	{       1, "Sfx\\Towners\\Witch03.wav",   NULL },
	{       1, "Sfx\\Towners\\Witch04.wav",   NULL },
	{       1, "Sfx\\Towners\\Witch05.wav",   NULL },
	{       1, "Sfx\\Towners\\Witch06.wav",   NULL },
	{       1, "Sfx\\Towners\\Witch07.wav",   NULL },
	{       1, "Sfx\\Towners\\Witch08.wav",   NULL },
	{       1, "Sfx\\Towners\\Witch09.wav",   NULL },
	{       1, "Sfx\\Towners\\Witch10.wav",   NULL },
	{       1, "Sfx\\Towners\\Witch11.wav",   NULL },
	{       1, "Sfx\\Towners\\Witch12.wav",   NULL },
	{       1, "Sfx\\Towners\\Witch13.wav",   NULL },
	{       1, "Sfx\\Towners\\Witch14.wav",   NULL },
	{       1, "Sfx\\Towners\\Witch15.wav",   NULL },
	{       1, "Sfx\\Towners\\Witch16.wav",   NULL },
	{       1, "Sfx\\Towners\\Witch17.wav",   NULL },
	{       1, "Sfx\\Towners\\Witch18.wav",   NULL },
	{       1, "Sfx\\Towners\\Witch19.wav",   NULL },
	{       1, "Sfx\\Towners\\Witch20.wav",   NULL },
	{       1, "Sfx\\Towners\\Witch21.wav",   NULL },
	{       1, "Sfx\\Towners\\Witch22.wav",   NULL },
	{       1, "Sfx\\Towners\\Witch23.wav",   NULL },
	{       1, "Sfx\\Towners\\Witch24.wav",   NULL },
	{       1, "Sfx\\Towners\\Witch25.wav",   NULL },
	{       1, "Sfx\\Towners\\Witch26.wav",   NULL },
	{       1, "Sfx\\Towners\\Witch27.wav",   NULL },
	{       1, "Sfx\\Towners\\Witch28.wav",   NULL },
	{       1, "Sfx\\Towners\\Witch29.wav",   NULL },
	{       1, "Sfx\\Towners\\Witch30.wav",   NULL },
	{       1, "Sfx\\Towners\\Witch31.wav",   NULL },
	{       1, "Sfx\\Towners\\Witch32.wav",   NULL },
	{       1, "Sfx\\Towners\\Witch33.wav",   NULL },
	{       1, "Sfx\\Towners\\Witch34.wav",   NULL },
	{       1, "Sfx\\Towners\\Witch35.wav",   NULL },
	{       1, "Sfx\\Towners\\Witch36.wav",   NULL },
	{       1, "Sfx\\Towners\\Witch37.wav",   NULL },
	{       1, "Sfx\\Towners\\Witch38.wav",   NULL },
	{       1, "Sfx\\Towners\\Witch39.wav",   NULL },
	{       1, "Sfx\\Towners\\Witch40.wav",   NULL },
	{       1, "Sfx\\Towners\\Witch41.wav",   NULL },
	{       1, "Sfx\\Towners\\Witch42.wav",   NULL },
	{       1, "Sfx\\Towners\\Witch43.wav",   NULL },
	{       1, "Sfx\\Towners\\Witch44.wav",   NULL },
	{       1, "Sfx\\Towners\\Witch45.wav",   NULL },
	{       1, "Sfx\\Towners\\Witch46.wav",   NULL },
	{       1, "Sfx\\Towners\\Witch47.wav",   NULL },
	{       1, "Sfx\\Towners\\Witch48.wav",   NULL },
	{       1, "Sfx\\Towners\\Witch49.wav",   NULL },
	{       1, "Sfx\\Towners\\Witch50.wav",   NULL },
	{       1, "Sfx\\Towners\\Wound01.wav",   NULL },
	{      65, "Sfx\\Sorceror\\Mage01.wav",   NULL },
	{      65, "Sfx\\Sorceror\\Mage02.wav",   NULL },
	{      65, "Sfx\\Sorceror\\Mage03.wav",   NULL },
	{      65, "Sfx\\Sorceror\\Mage04.wav",   NULL },
	{      65, "Sfx\\Sorceror\\Mage05.wav",   NULL },
	{      65, "Sfx\\Sorceror\\Mage06.wav",   NULL },
	{      65, "Sfx\\Sorceror\\Mage07.wav",   NULL },
	{      65, "Sfx\\Sorceror\\Mage08.wav",   NULL },
	{      65, "Sfx\\Sorceror\\Mage09.wav",   NULL },
	{      65, "Sfx\\Sorceror\\Mage10.wav",   NULL },
	{      65, "Sfx\\Sorceror\\Mage11.wav",   NULL },
	{      65, "Sfx\\Sorceror\\Mage12.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage13.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage14.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage15.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage16.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage17.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage18.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage19.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage20.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage21.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage22.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage23.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage24.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage25.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage26.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage27.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage28.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage29.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage30.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage31.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage32.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage33.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage34.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage35.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage36.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage37.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage38.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage39.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage40.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage41.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage42.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage43.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage44.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage45.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage46.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage47.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage48.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage49.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage50.wav",   NULL },
	{      65, "Sfx\\Sorceror\\Mage51.wav",   NULL },
	{      65, "Sfx\\Sorceror\\Mage52.wav",   NULL },
	{      65, "Sfx\\Sorceror\\Mage53.wav",   NULL },
	{      65, "Sfx\\Sorceror\\Mage54.wav",   NULL },
	{      65, "Sfx\\Sorceror\\Mage55.wav",   NULL },
	{      65, "Sfx\\Sorceror\\Mage56.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage57.wav",   NULL },
	{      65, "Sfx\\Sorceror\\Mage58.wav",   NULL },
	{      65, "Sfx\\Sorceror\\Mage59.wav",   NULL },
	{      65, "Sfx\\Sorceror\\Mage60.wav",   NULL },
	{      65, "Sfx\\Sorceror\\Mage61.wav",   NULL },
	{      65, "Sfx\\Sorceror\\Mage62.wav",   NULL },
	{      65, "Sfx\\Sorceror\\Mage63.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage64.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage65.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage66.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage67.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage68.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage69.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage69b.wav",  NULL },
	{      64, "Sfx\\Sorceror\\Mage70.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage71.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage72.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage73.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage74.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage75.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage76.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage77.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage78.wav",   NULL },
	{      64, "Sfx\\Sorceror\\Mage79.wav",   NULL },
	{      65, "Sfx\\Sorceror\\Mage80.wav",   NULL },
	{      65, "Sfx\\Sorceror\\Mage81.wav",   NULL },
	{      65, "Sfx\\Sorceror\\Mage82.wav",   NULL },
	{      65, "Sfx\\Sorceror\\Mage83.wav",   NULL },
	{      65, "Sfx\\Sorceror\\Mage84.wav",   NULL },
	{      65, "Sfx\\Sorceror\\Mage85.wav",   NULL },
	{      65, "Sfx\\Sorceror\\Mage86.wav",   NULL },
	{      65, "Sfx\\Sorceror\\Mage87.wav",   NULL },
	{      65, "Sfx\\Sorceror\\Mage88.wav",   NULL },
	{      65, "Sfx\\Sorceror\\Mage89.wav",   NULL },
	{      65, "Sfx\\Sorceror\\Mage90.wav",   NULL },
	{      65, "Sfx\\Sorceror\\Mage91.wav",   NULL },
	{      65, "Sfx\\Sorceror\\Mage92.wav",   NULL },
	{      65, "Sfx\\Sorceror\\Mage93.wav",   NULL },
	{      65, "Sfx\\Sorceror\\Mage94.wav",   NULL },
	{      65, "Sfx\\Sorceror\\Mage95.wav",   NULL },
	{      65, "Sfx\\Sorceror\\Mage96.wav",   NULL },
	{      65, "Sfx\\Sorceror\\Mage97.wav",   NULL },
	{      65, "Sfx\\Sorceror\\Mage98.wav",   NULL },
	{      65, "Sfx\\Sorceror\\Mage99.wav",   NULL },
	{      65, "Sfx\\Sorceror\\Mage100.wav",  NULL },
	{      65, "Sfx\\Sorceror\\Mage101.wav",  NULL },
	{      65, "Sfx\\Sorceror\\Mage102.wav",  NULL },
	{      17, "Sfx\\Rogue\\Rogue01.wav",     NULL },
	{      17, "Sfx\\Rogue\\Rogue02.wav",     NULL },
	{      17, "Sfx\\Rogue\\Rogue03.wav",     NULL },
	{      17, "Sfx\\Rogue\\Rogue04.wav",     NULL },
	{      17, "Sfx\\Rogue\\Rogue05.wav",     NULL },
	{      17, "Sfx\\Rogue\\Rogue06.wav",     NULL },
	{      17, "Sfx\\Rogue\\Rogue07.wav",     NULL },
	{      17, "Sfx\\Rogue\\Rogue08.wav",     NULL },
	{      17, "Sfx\\Rogue\\Rogue09.wav",     NULL },
	{      17, "Sfx\\Rogue\\Rogue10.wav",     NULL },
	{      17, "Sfx\\Rogue\\Rogue11.wav",     NULL },
	{      17, "Sfx\\Rogue\\Rogue12.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue13.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue14.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue15.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue16.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue17.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue18.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue19.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue20.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue21.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue22.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue23.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue24.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue25.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue26.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue27.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue28.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue29.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue30.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue31.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue32.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue33.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue34.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue35.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue36.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue37.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue38.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue39.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue40.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue41.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue42.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue43.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue44.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue45.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue46.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue47.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue48.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue49.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue50.wav",     NULL },
	{      17, "Sfx\\Rogue\\Rogue51.wav",     NULL },
	{      17, "Sfx\\Rogue\\Rogue52.wav",     NULL },
	{      17, "Sfx\\Rogue\\Rogue53.wav",     NULL },
	{      17, "Sfx\\Rogue\\Rogue54.wav",     NULL },
	{      17, "Sfx\\Rogue\\Rogue55.wav",     NULL },
	{      17, "Sfx\\Rogue\\Rogue56.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue57.wav",     NULL },
	{      17, "Sfx\\Rogue\\Rogue58.wav",     NULL },
	{      17, "Sfx\\Rogue\\Rogue59.wav",     NULL },
	{      17, "Sfx\\Rogue\\Rogue60.wav",     NULL },
	{      17, "Sfx\\Rogue\\Rogue61.wav",     NULL },
	{      17, "Sfx\\Rogue\\Rogue62.wav",     NULL },
	{      17, "Sfx\\Rogue\\Rogue63.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue64.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue65.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue66.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue67.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue68.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue69.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue69b.wav",    NULL },
	{      16, "Sfx\\Rogue\\Rogue70.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue71.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue72.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue73.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue74.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue75.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue76.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue77.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue78.wav",     NULL },
	{      16, "Sfx\\Rogue\\Rogue79.wav",     NULL },
	{      17, "Sfx\\Rogue\\Rogue80.wav",     NULL },
	{      17, "Sfx\\Rogue\\Rogue81.wav",     NULL },
	{      17, "Sfx\\Rogue\\Rogue82.wav",     NULL },
	{      17, "Sfx\\Rogue\\Rogue83.wav",     NULL },
	{      17, "Sfx\\Rogue\\Rogue84.wav",     NULL },
	{      17, "Sfx\\Rogue\\Rogue85.wav",     NULL },
	{      17, "Sfx\\Rogue\\Rogue86.wav",     NULL },
	{      17, "Sfx\\Rogue\\Rogue87.wav",     NULL },
	{      17, "Sfx\\Rogue\\Rogue88.wav",     NULL },
	{      17, "Sfx\\Rogue\\Rogue89.wav",     NULL },
	{      17, "Sfx\\Rogue\\Rogue90.wav",     NULL },
	{      17, "Sfx\\Rogue\\Rogue91.wav",     NULL },
	{      17, "Sfx\\Rogue\\Rogue92.wav",     NULL },
	{      17, "Sfx\\Rogue\\Rogue93.wav",     NULL },
	{      17, "Sfx\\Rogue\\Rogue94.wav",     NULL },
	{      17, "Sfx\\Rogue\\Rogue95.wav",     NULL },
	{      17, "Sfx\\Rogue\\Rogue96.wav",     NULL },
	{      17, "Sfx\\Rogue\\Rogue97.wav",     NULL },
	{      17, "Sfx\\Rogue\\Rogue98.wav",     NULL },
	{      17, "Sfx\\Rogue\\Rogue99.wav",     NULL },
	{      17, "Sfx\\Rogue\\Rogue100.wav",    NULL },
	{      17, "Sfx\\Rogue\\Rogue101.wav",    NULL },
	{      17, "Sfx\\Rogue\\Rogue102.wav",    NULL },
	{      33, "Sfx\\Warrior\\Warior01.wav",  NULL },
	{      33, "Sfx\\Warrior\\Warior02.wav",  NULL },
	{      33, "Sfx\\Warrior\\Warior03.wav",  NULL },
	{      33, "Sfx\\Warrior\\Warior04.wav",  NULL },
	{      33, "Sfx\\Warrior\\Warior05.wav",  NULL },
	{      33, "Sfx\\Warrior\\Warior06.wav",  NULL },
	{      33, "Sfx\\Warrior\\Warior07.wav",  NULL },
	{      33, "Sfx\\Warrior\\Warior08.wav",  NULL },
	{      33, "Sfx\\Warrior\\Warior09.wav",  NULL },
	{      33, "Sfx\\Warrior\\Warior10.wav",  NULL },
	{      33, "Sfx\\Warrior\\Warior11.wav",  NULL },
	{      33, "Sfx\\Warrior\\Warior12.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior13.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior14.wav",  NULL },
	{      32, "Sfx\\Warrior\\Wario14b.wav",  NULL },
	{      32, "Sfx\\Warrior\\Wario14c.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior15.wav",  NULL },
	{      32, "Sfx\\Warrior\\Wario15b.wav",  NULL },
	{      32, "Sfx\\Warrior\\Wario15c.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior16.wav",  NULL },
	{      32, "Sfx\\Warrior\\Wario16b.wav",  NULL },
	{      32, "Sfx\\Warrior\\Wario16c.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior17.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior18.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior19.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior20.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior21.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior22.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior23.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior24.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior25.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior26.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior27.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior28.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior29.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior30.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior31.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior32.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior33.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior34.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior35.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior36.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior37.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior38.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior39.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior40.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior41.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior42.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior43.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior44.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior45.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior46.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior47.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior48.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior49.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior50.wav",  NULL },
	{      33, "Sfx\\Warrior\\Warior51.wav",  NULL },
	{      33, "Sfx\\Warrior\\Warior52.wav",  NULL },
	{      33, "Sfx\\Warrior\\Warior53.wav",  NULL },
	{      33, "Sfx\\Warrior\\Warior54.wav",  NULL },
	{      33, "Sfx\\Warrior\\Warior55.wav",  NULL },
	{      33, "Sfx\\Warrior\\Warior56.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior57.wav",  NULL },
	{      33, "Sfx\\Warrior\\Warior58.wav",  NULL },
	{      33, "Sfx\\Warrior\\Warior59.wav",  NULL },
	{      33, "Sfx\\Warrior\\Warior60.wav",  NULL },
	{      33, "Sfx\\Warrior\\Warior61.wav",  NULL },
	{      33, "Sfx\\Warrior\\Warior62.wav",  NULL },
	{      33, "Sfx\\Warrior\\Warior63.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior64.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior65.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior66.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior67.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior68.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior69.wav",  NULL },
	{      32, "Sfx\\Warrior\\Wario69b.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior70.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior71.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior72.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior73.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior74.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior75.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior76.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior77.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior78.wav",  NULL },
	{      32, "Sfx\\Warrior\\Warior79.wav",  NULL },
	{      33, "Sfx\\Warrior\\Warior80.wav",  NULL },
	{      33, "Sfx\\Warrior\\Warior81.wav",  NULL },
	{      33, "Sfx\\Warrior\\Warior82.wav",  NULL },
	{      33, "Sfx\\Warrior\\Warior83.wav",  NULL },
	{      33, "Sfx\\Warrior\\Warior84.wav",  NULL },
	{      33, "Sfx\\Warrior\\Warior85.wav",  NULL },
	{      33, "Sfx\\Warrior\\Warior86.wav",  NULL },
	{      33, "Sfx\\Warrior\\Warior87.wav",  NULL },
	{      33, "Sfx\\Warrior\\Warior88.wav",  NULL },
	{      33, "Sfx\\Warrior\\Warior89.wav",  NULL },
	{      33, "Sfx\\Warrior\\Warior90.wav",  NULL },
	{      33, "Sfx\\Warrior\\Warior91.wav",  NULL },
	{      33, "Sfx\\Warrior\\Warior92.wav",  NULL },
	{      33, "Sfx\\Warrior\\Warior93.wav",  NULL },
	{      33, "Sfx\\Warrior\\Warior94.wav",  NULL },
	{      33, "Sfx\\Warrior\\Warior95.wav",  NULL },
	{      33, "Sfx\\Warrior\\Wario95b.wav",  NULL },
	{      33, "Sfx\\Warrior\\Wario95c.wav",  NULL },
	{      33, "Sfx\\Warrior\\Wario95d.wav",  NULL },
	{      33, "Sfx\\Warrior\\Wario95e.wav",  NULL },
	{      33, "Sfx\\Warrior\\Wario95f.wav",  NULL },
	{      33, "Sfx\\Warrior\\Wario96b.wav",  NULL },
	{      33, "Sfx\\Warrior\\Wario97.wav",   NULL },
	{      33, "Sfx\\Warrior\\Wario98.wav",   NULL },
	{      33, "Sfx\\Warrior\\Warior99.wav",  NULL },
	{      33, "Sfx\\Warrior\\Wario100.wav",  NULL },
	{      33, "Sfx\\Warrior\\Wario101.wav",  NULL },
	{      33, "Sfx\\Warrior\\Wario102.wav",  NULL },
	{       1, "Sfx\\Narrator\\Nar01.wav",    NULL },
	{       1, "Sfx\\Narrator\\Nar02.wav",    NULL },
	{       1, "Sfx\\Narrator\\Nar03.wav",    NULL },
	{       1, "Sfx\\Narrator\\Nar04.wav",    NULL },
	{       1, "Sfx\\Narrator\\Nar05.wav",    NULL },
	{       1, "Sfx\\Narrator\\Nar06.wav",    NULL },
	{       1, "Sfx\\Narrator\\Nar07.wav",    NULL },
	{       1, "Sfx\\Narrator\\Nar08.wav",    NULL },
	{       1, "Sfx\\Narrator\\Nar09.wav",    NULL },
	{       1, "Sfx\\Misc\\Lvl16int.wav",     NULL },
	{       1, "Sfx\\Monsters\\Butcher.wav",  NULL },
	{       1, "Sfx\\Monsters\\Garbud01.wav", NULL },
	{       1, "Sfx\\Monsters\\Garbud02.wav", NULL },
	{       1, "Sfx\\Monsters\\Garbud03.wav", NULL },
	{       1, "Sfx\\Monsters\\Garbud04.wav", NULL },
	{       1, "Sfx\\Monsters\\Izual01.wav",  NULL },
	{       1, "Sfx\\Monsters\\Lach01.wav",   NULL },
	{       1, "Sfx\\Monsters\\Lach02.wav",   NULL },
	{       1, "Sfx\\Monsters\\Lach03.wav",   NULL },
	{       1, "Sfx\\Monsters\\Laz01.wav",    NULL },
	{       1, "Sfx\\Monsters\\Laz02.wav",    NULL },
	{       1, "Sfx\\Monsters\\Sking01.wav",  NULL },
	{       1, "Sfx\\Monsters\\Snot01.wav",   NULL },
	{       1, "Sfx\\Monsters\\Snot02.wav",   NULL },
	{       1, "Sfx\\Monsters\\Snot03.wav",   NULL },
	{       1, "Sfx\\Monsters\\Warlrd01.wav", NULL },
	{       1, "Sfx\\Monsters\\Wlock01.wav",  NULL },
	{       1, "Sfx\\Monsters\\Zhar01.wav",   NULL },
	{       1, "Sfx\\Monsters\\Zhar02.wav",   NULL },
	{       1, "Sfx\\Monsters\\DiabloD.wav",  NULL }
	// clang-format on
};

BOOL __fastcall effect_is_playing(int nSFX)
{
	TSFX *sfx = &sgSFX[nSFX];
	if (sfx->pSnd)
		return snd_playing(sfx->pSnd);

	if (sfx->bFlags & SFX_STREAM)
		return sfx == sfx_data_cur;

	return FALSE;
}

void __cdecl sfx_stop()
{
	if (sfx_stream) {
		SFileDdaEnd(sfx_stream);
		SFileCloseFile(sfx_stream);
		sfx_stream = NULL;
		sfx_data_cur = NULL;
	}
}

void __fastcall InitMonsterSND(int monst)
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

void __cdecl FreeEffects()
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

void __fastcall PlayEffect(int i, int mode)
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
// 676194: using guessed type char gbBufferMsgs;

BOOL __fastcall calc_snd_position(int x, int y, int *plVolume, int *plPan)
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

void __fastcall PlaySFX(int psfx)
{
	psfx = RndSFX(psfx);
	PlaySFX_priv(&sgSFX[psfx], 0, 0, 0);
}

void __fastcall PlaySFX_priv(TSFX *pSFX, BOOL loc, int x, int y)
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
// 676194: using guessed type char gbBufferMsgs;
// 679660: using guessed type char gbMaxPlayers;

void __fastcall stream_play(TSFX *pSFX, int lVolume, int lPan)
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

int __fastcall RndSFX(int psfx)
{
	int v1; // esi
	int v3; // [esp-4h] [ebp-8h]

	v1 = psfx;
	switch (psfx) {
	case PS_WARR69:
		goto LABEL_12;
	case PS_WARR14:
	case PS_WARR15:
	case PS_WARR16:
		goto LABEL_19;
	case PS_MAGE69:
	case PS_ROGUE69:
	case PS_SWING:
	case LS_ACID:
	case IS_FMAG:
	case IS_MAGIC:
	case IS_BHIT:
	LABEL_12:
		v3 = 2;
	LABEL_15:
		return v1 + random(165, v3);
	case PS_WARR2:
	LABEL_19:
		v3 = 3;
		goto LABEL_15;
	}
	return psfx;
}

void __fastcall PlaySfxLoc(int psfx, int x, int y)
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

void __cdecl FreeMonsterSnd()
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

void __cdecl sound_stop()
{
	int i; // edi

	for (i = 0; i < NUM_SFX; i++) {
		if (sgSFX[i].pSnd)
			snd_stop_snd(sgSFX[i].pSnd);
	}
}

void __cdecl sound_update()
{
	int current, end;

	if (!gbSndInited) {
		return;
	}

	snd_update(FALSE);
	if (sfx_stream != NULL && SFileDdaGetPos(sfx_stream, &current, &end) && current >= end) {
		sfx_stop();
	}
}

// 415DBA: could not find valid save-restore pair for ebp

void __cdecl effects_cleanup_sfx()
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

void __cdecl stream_update()
{
	UCHAR mask = 0;
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
// 679660: using guessed type char gbMaxPlayers;

void __fastcall priv_sound_init(UCHAR bLoadMask)
{
	UCHAR pc, bFlags;
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

void __cdecl sound_init()
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
