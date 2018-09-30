// HEADER_GOES_HERE

#include "../types.h"

int effects_cpp_init_value; // weak

#ifndef NO_GLOBALS
int sfxdelay; // weak
int sfxdnum;
void *sfx_stream;
TSFX *sfx_data_cur;
#endif

const int effects_inf = 0x7F800000; // weak
const char monster_action_sounds[] = {'a', 'h', 'd', 's'}; // idb

/* data */

TSFX sgSFX[NUM_SFX] = {{2u, "Sfx\\Misc\\Walk1.wav", NULL},        {2u, "Sfx\\Misc\\Walk2.wav", NULL},
                       {2u, "Sfx\\Misc\\Walk3.wav", NULL},        {2u, "Sfx\\Misc\\Walk4.wav", NULL},
                       {2u, "Sfx\\Misc\\BFire.wav", NULL},        {2u, "Sfx\\Misc\\Fmag.wav", NULL},
                       {2u, "Sfx\\Misc\\Tmag.wav", NULL},         {2u, "Sfx\\Misc\\Lghit.wav", NULL},
                       {2u, "Sfx\\Misc\\Lghit1.wav", NULL},       {2u, "Sfx\\Misc\\Swing.wav", NULL},
                       {2u, "Sfx\\Misc\\Swing2.wav", NULL},       {2u, "Sfx\\Misc\\Dead.wav", NULL},
                       {1u, "Sfx\\Misc\\Questdon.wav", NULL},     {2u, "Sfx\\Items\\Armrfkd.wav", NULL},
                       {2u, "Sfx\\Items\\Barlfire.wav", NULL},    {2u, "Sfx\\Items\\Barrel.wav", NULL},
                       {2u, "Sfx\\Items\\Bhit.wav", NULL},        {2u, "Sfx\\Items\\Bhit1.wav", NULL},
                       {2u, "Sfx\\Items\\Chest.wav", NULL},       {2u, "Sfx\\Items\\Doorclos.wav", NULL},
                       {2u, "Sfx\\Items\\Dooropen.wav", NULL},    {2u, "Sfx\\Items\\Flipanvl.wav", NULL},
                       {2u, "Sfx\\Items\\Flipaxe.wav", NULL},     {2u, "Sfx\\Items\\Flipblst.wav", NULL},
                       {2u, "Sfx\\Items\\Flipbody.wav", NULL},    {2u, "Sfx\\Items\\Flipbook.wav", NULL},
                       {2u, "Sfx\\Items\\Flipbow.wav", NULL},     {2u, "Sfx\\Items\\Flipcap.wav", NULL},
                       {2u, "Sfx\\Items\\Flipharm.wav", NULL},    {2u, "Sfx\\Items\\Fliplarm.wav", NULL},
                       {2u, "Sfx\\Items\\Flipmag.wav", NULL},     {2u, "Sfx\\Items\\Flipmag1.wav", NULL},
                       {2u, "Sfx\\Items\\Flipmush.wav", NULL},    {2u, "Sfx\\Items\\Flippot.wav", NULL},
                       {2u, "Sfx\\Items\\Flipring.wav", NULL},    {2u, "Sfx\\Items\\Fliprock.wav", NULL},
                       {2u, "Sfx\\Items\\Flipscrl.wav", NULL},    {2u, "Sfx\\Items\\Flipshld.wav", NULL},
                       {2u, "Sfx\\Items\\Flipsign.wav", NULL},    {2u, "Sfx\\Items\\Flipstaf.wav", NULL},
                       {2u, "Sfx\\Items\\Flipswor.wav", NULL},    {2u, "Sfx\\Items\\Gold.wav", NULL},
                       {2u, "Sfx\\Items\\Hlmtfkd.wav", NULL},     {2u, "Sfx\\Items\\Invanvl.wav", NULL},
                       {2u, "Sfx\\Items\\Invaxe.wav", NULL},      {2u, "Sfx\\Items\\Invblst.wav", NULL},
                       {2u, "Sfx\\Items\\Invbody.wav", NULL},     {2u, "Sfx\\Items\\Invbook.wav", NULL},
                       {2u, "Sfx\\Items\\Invbow.wav", NULL},      {2u, "Sfx\\Items\\Invcap.wav", NULL},
                       {2u, "Sfx\\Items\\Invgrab.wav", NULL},     {2u, "Sfx\\Items\\Invharm.wav", NULL},
                       {2u, "Sfx\\Items\\Invlarm.wav", NULL},     {2u, "Sfx\\Items\\Invmush.wav", NULL},
                       {2u, "Sfx\\Items\\Invpot.wav", NULL},      {2u, "Sfx\\Items\\Invring.wav", NULL},
                       {2u, "Sfx\\Items\\Invrock.wav", NULL},     {2u, "Sfx\\Items\\Invscrol.wav", NULL},
                       {2u, "Sfx\\Items\\Invshiel.wav", NULL},    {2u, "Sfx\\Items\\Invsign.wav", NULL},
                       {2u, "Sfx\\Items\\Invstaf.wav", NULL},     {2u, "Sfx\\Items\\Invsword.wav", NULL},
                       {2u, "Sfx\\Items\\Lever.wav", NULL},       {2u, "Sfx\\Items\\Magic.wav", NULL},
                       {2u, "Sfx\\Items\\Magic1.wav", NULL},      {2u, "Sfx\\Items\\Readbook.wav", NULL},
                       {2u, "Sfx\\Items\\Sarc.wav", NULL},        {2u, "Sfx\\Items\\Shielfkd.wav", NULL},
                       {2u, "Sfx\\Items\\Swrdfkd.wav", NULL},     {4u, "Sfx\\Items\\Titlemov.wav", NULL},
                       {4u, "Sfx\\Items\\Titlslct.wav", NULL},    {4u, "Sfx\\Misc\\blank.wav", NULL},
                       {2u, "Sfx\\Items\\Trap.wav", NULL},        {2u, "Sfx\\Misc\\Cast1.wav", NULL},
                       {2u, "Sfx\\Misc\\Cast10.wav", NULL},       {2u, "Sfx\\Misc\\Cast12.wav", NULL},
                       {2u, "Sfx\\Misc\\Cast2.wav", NULL},        {2u, "Sfx\\Misc\\Cast3.wav", NULL},
                       {2u, "Sfx\\Misc\\Cast4.wav", NULL},        {2u, "Sfx\\Misc\\Cast5.wav", NULL},
                       {2u, "Sfx\\Misc\\Cast6.wav", NULL},        {2u, "Sfx\\Misc\\Cast7.wav", NULL},
                       {2u, "Sfx\\Misc\\Cast8.wav", NULL},        {2u, "Sfx\\Misc\\Cast9.wav", NULL},
                       {2u, "Sfx\\Misc\\Healing.wav", NULL},      {2u, "Sfx\\Misc\\Repair.wav", NULL},
                       {2u, "Sfx\\Misc\\Acids1.wav", NULL},       {2u, "Sfx\\Misc\\Acids2.wav", NULL},
                       {2u, "Sfx\\Misc\\Apoc.wav", NULL},         {2u, "Sfx\\Misc\\Arrowall.wav", NULL},
                       {2u, "Sfx\\Misc\\Bldboil.wav", NULL},      {2u, "Sfx\\Misc\\Blodstar.wav", NULL},
                       {2u, "Sfx\\Misc\\Blsimpt.wav", NULL},      {2u, "Sfx\\Misc\\Bonesp.wav", NULL},
                       {2u, "Sfx\\Misc\\Bsimpct.wav", NULL},      {2u, "Sfx\\Misc\\Caldron.wav", NULL},
                       {2u, "Sfx\\Misc\\Cbolt.wav", NULL},        {2u, "Sfx\\Misc\\Chltning.wav", NULL},
                       {2u, "Sfx\\Misc\\DSerp.wav", NULL},        {2u, "Sfx\\Misc\\Elecimp1.wav", NULL},
                       {2u, "Sfx\\Misc\\Elementl.wav", NULL},     {2u, "Sfx\\Misc\\Ethereal.wav", NULL},
                       {2u, "Sfx\\Misc\\Fball.wav", NULL},        {2u, "Sfx\\Misc\\Fbolt1.wav", NULL},
                       {2u, "Sfx\\Misc\\Fbolt2.wav", NULL},       {2u, "Sfx\\Misc\\Firimp1.wav", NULL},
                       {2u, "Sfx\\Misc\\Firimp2.wav", NULL},      {2u, "Sfx\\Misc\\Flamwave.wav", NULL},
                       {2u, "Sfx\\Misc\\Flash.wav", NULL},        {2u, "Sfx\\Misc\\Fountain.wav", NULL},
                       {2u, "Sfx\\Misc\\Golum.wav", NULL},        {2u, "Sfx\\Misc\\Golumded.wav", NULL},
                       {2u, "Sfx\\Misc\\Gshrine.wav", NULL},      {2u, "Sfx\\Misc\\Guard.wav", NULL},
                       {2u, "Sfx\\Misc\\Grdlanch.wav", NULL},     {2u, "Sfx\\Misc\\Holybolt.wav", NULL},
                       {2u, "Sfx\\Misc\\Hyper.wav", NULL},        {2u, "Sfx\\Misc\\Infravis.wav", NULL},
                       {2u, "Sfx\\Misc\\Invisibl.wav", NULL},     {2u, "Sfx\\Misc\\Invpot.wav", NULL},
                       {2u, "Sfx\\Misc\\Lning1.wav", NULL},       {2u, "Sfx\\Misc\\Ltning.wav", NULL},
                       {2u, "Sfx\\Misc\\Mshield.wav", NULL},      {2u, "Sfx\\Misc\\Nova.wav", NULL},
                       {2u, "Sfx\\Misc\\Portal.wav", NULL},       {2u, "Sfx\\Misc\\Puddle.wav", NULL},
                       {2u, "Sfx\\Misc\\Resur.wav", NULL},        {2u, "Sfx\\Misc\\Scurse.wav", NULL},
                       {2u, "Sfx\\Misc\\Scurimp.wav", NULL},      {2u, "Sfx\\Misc\\Sentinel.wav", NULL},
                       {2u, "Sfx\\Misc\\Shatter.wav", NULL},      {2u, "Sfx\\Misc\\Soulfire.wav", NULL},
                       {2u, "Sfx\\Misc\\Spoutlop.wav", NULL},     {2u, "Sfx\\Misc\\Spoutstr.wav", NULL},
                       {2u, "Sfx\\Misc\\Storm.wav", NULL},        {2u, "Sfx\\Misc\\Trapdis.wav", NULL},
                       {2u, "Sfx\\Misc\\Teleport.wav", NULL},     {2u, "Sfx\\Misc\\Vtheft.wav", NULL},
                       {2u, "Sfx\\Misc\\Wallloop.wav", NULL},     {2u, "Sfx\\Misc\\Wallstrt.wav", NULL},
                       {1u, "Sfx\\Towners\\Bmaid01.wav", NULL},   {1u, "Sfx\\Towners\\Bmaid02.wav", NULL},
                       {1u, "Sfx\\Towners\\Bmaid03.wav", NULL},   {1u, "Sfx\\Towners\\Bmaid04.wav", NULL},
                       {1u, "Sfx\\Towners\\Bmaid05.wav", NULL},   {1u, "Sfx\\Towners\\Bmaid06.wav", NULL},
                       {1u, "Sfx\\Towners\\Bmaid07.wav", NULL},   {1u, "Sfx\\Towners\\Bmaid08.wav", NULL},
                       {1u, "Sfx\\Towners\\Bmaid09.wav", NULL},   {1u, "Sfx\\Towners\\Bmaid10.wav", NULL},
                       {1u, "Sfx\\Towners\\Bmaid11.wav", NULL},   {1u, "Sfx\\Towners\\Bmaid12.wav", NULL},
                       {1u, "Sfx\\Towners\\Bmaid13.wav", NULL},   {1u, "Sfx\\Towners\\Bmaid14.wav", NULL},
                       {1u, "Sfx\\Towners\\Bmaid15.wav", NULL},   {1u, "Sfx\\Towners\\Bmaid16.wav", NULL},
                       {1u, "Sfx\\Towners\\Bmaid17.wav", NULL},   {1u, "Sfx\\Towners\\Bmaid18.wav", NULL},
                       {1u, "Sfx\\Towners\\Bmaid19.wav", NULL},   {1u, "Sfx\\Towners\\Bmaid20.wav", NULL},
                       {1u, "Sfx\\Towners\\Bmaid21.wav", NULL},   {1u, "Sfx\\Towners\\Bmaid22.wav", NULL},
                       {1u, "Sfx\\Towners\\Bmaid23.wav", NULL},   {1u, "Sfx\\Towners\\Bmaid24.wav", NULL},
                       {1u, "Sfx\\Towners\\Bmaid25.wav", NULL},   {1u, "Sfx\\Towners\\Bmaid26.wav", NULL},
                       {1u, "Sfx\\Towners\\Bmaid27.wav", NULL},   {1u, "Sfx\\Towners\\Bmaid28.wav", NULL},
                       {1u, "Sfx\\Towners\\Bmaid29.wav", NULL},   {1u, "Sfx\\Towners\\Bmaid30.wav", NULL},
                       {1u, "Sfx\\Towners\\Bmaid31.wav", NULL},   {1u, "Sfx\\Towners\\Bmaid32.wav", NULL},
                       {1u, "Sfx\\Towners\\Bmaid33.wav", NULL},   {1u, "Sfx\\Towners\\Bmaid34.wav", NULL},
                       {1u, "Sfx\\Towners\\Bmaid35.wav", NULL},   {1u, "Sfx\\Towners\\Bmaid36.wav", NULL},
                       {1u, "Sfx\\Towners\\Bmaid37.wav", NULL},   {1u, "Sfx\\Towners\\Bmaid38.wav", NULL},
                       {1u, "Sfx\\Towners\\Bmaid39.wav", NULL},   {1u, "Sfx\\Towners\\Bmaid40.wav", NULL},
                       {1u, "Sfx\\Towners\\Bsmith01.wav", NULL},  {1u, "Sfx\\Towners\\Bsmith02.wav", NULL},
                       {1u, "Sfx\\Towners\\Bsmith03.wav", NULL},  {1u, "Sfx\\Towners\\Bsmith04.wav", NULL},
                       {1u, "Sfx\\Towners\\Bsmith05.wav", NULL},  {1u, "Sfx\\Towners\\Bsmith06.wav", NULL},
                       {1u, "Sfx\\Towners\\Bsmith07.wav", NULL},  {1u, "Sfx\\Towners\\Bsmith08.wav", NULL},
                       {1u, "Sfx\\Towners\\Bsmith09.wav", NULL},  {1u, "Sfx\\Towners\\Bsmith10.wav", NULL},
                       {1u, "Sfx\\Towners\\Bsmith11.wav", NULL},  {1u, "Sfx\\Towners\\Bsmith12.wav", NULL},
                       {1u, "Sfx\\Towners\\Bsmith13.wav", NULL},  {1u, "Sfx\\Towners\\Bsmith14.wav", NULL},
                       {1u, "Sfx\\Towners\\Bsmith15.wav", NULL},  {1u, "Sfx\\Towners\\Bsmith16.wav", NULL},
                       {1u, "Sfx\\Towners\\Bsmith17.wav", NULL},  {1u, "Sfx\\Towners\\Bsmith18.wav", NULL},
                       {1u, "Sfx\\Towners\\Bsmith19.wav", NULL},  {1u, "Sfx\\Towners\\Bsmith20.wav", NULL},
                       {1u, "Sfx\\Towners\\Bsmith21.wav", NULL},  {1u, "Sfx\\Towners\\Bsmith22.wav", NULL},
                       {1u, "Sfx\\Towners\\Bsmith23.wav", NULL},  {1u, "Sfx\\Towners\\Bsmith24.wav", NULL},
                       {1u, "Sfx\\Towners\\Bsmith25.wav", NULL},  {1u, "Sfx\\Towners\\Bsmith26.wav", NULL},
                       {1u, "Sfx\\Towners\\Bsmith27.wav", NULL},  {1u, "Sfx\\Towners\\Bsmith28.wav", NULL},
                       {1u, "Sfx\\Towners\\Bsmith29.wav", NULL},  {1u, "Sfx\\Towners\\Bsmith30.wav", NULL},
                       {1u, "Sfx\\Towners\\Bsmith31.wav", NULL},  {1u, "Sfx\\Towners\\Bsmith32.wav", NULL},
                       {1u, "Sfx\\Towners\\Bsmith33.wav", NULL},  {1u, "Sfx\\Towners\\Bsmith34.wav", NULL},
                       {1u, "Sfx\\Towners\\Bsmith35.wav", NULL},  {1u, "Sfx\\Towners\\Bsmith36.wav", NULL},
                       {1u, "Sfx\\Towners\\Bsmith37.wav", NULL},  {1u, "Sfx\\Towners\\Bsmith38.wav", NULL},
                       {1u, "Sfx\\Towners\\Bsmith39.wav", NULL},  {1u, "Sfx\\Towners\\Bsmith40.wav", NULL},
                       {1u, "Sfx\\Towners\\Bsmith41.wav", NULL},  {1u, "Sfx\\Towners\\Bsmith42.wav", NULL},
                       {1u, "Sfx\\Towners\\Bsmith43.wav", NULL},  {1u, "Sfx\\Towners\\Bsmith44.wav", NULL},
                       {1u, "Sfx\\Towners\\Bsmith45.wav", NULL},  {1u, "Sfx\\Towners\\Bsmith46.wav", NULL},
                       {1u, "Sfx\\Towners\\Bsmith47.wav", NULL},  {1u, "Sfx\\Towners\\Bsmith48.wav", NULL},
                       {1u, "Sfx\\Towners\\Bsmith49.wav", NULL},  {1u, "Sfx\\Towners\\Bsmith50.wav", NULL},
                       {1u, "Sfx\\Towners\\Bsmith51.wav", NULL},  {1u, "Sfx\\Towners\\Bsmith52.wav", NULL},
                       {1u, "Sfx\\Towners\\Bsmith53.wav", NULL},  {1u, "Sfx\\Towners\\Bsmith54.wav", NULL},
                       {1u, "Sfx\\Towners\\Bsmith55.wav", NULL},  {1u, "Sfx\\Towners\\Bsmith56.wav", NULL},
                       {0u, "Sfx\\Towners\\Cow1.wav", NULL},      {0u, "Sfx\\Towners\\Cow2.wav", NULL},
                       {1u, "Sfx\\Towners\\Deadguy2.wav", NULL},  {1u, "Sfx\\Towners\\Drunk01.wav", NULL},
                       {1u, "Sfx\\Towners\\Drunk02.wav", NULL},   {1u, "Sfx\\Towners\\Drunk03.wav", NULL},
                       {1u, "Sfx\\Towners\\Drunk04.wav", NULL},   {1u, "Sfx\\Towners\\Drunk05.wav", NULL},
                       {1u, "Sfx\\Towners\\Drunk06.wav", NULL},   {1u, "Sfx\\Towners\\Drunk07.wav", NULL},
                       {1u, "Sfx\\Towners\\Drunk08.wav", NULL},   {1u, "Sfx\\Towners\\Drunk09.wav", NULL},
                       {1u, "Sfx\\Towners\\Drunk10.wav", NULL},   {1u, "Sfx\\Towners\\Drunk11.wav", NULL},
                       {1u, "Sfx\\Towners\\Drunk12.wav", NULL},   {1u, "Sfx\\Towners\\Drunk13.wav", NULL},
                       {1u, "Sfx\\Towners\\Drunk14.wav", NULL},   {1u, "Sfx\\Towners\\Drunk15.wav", NULL},
                       {1u, "Sfx\\Towners\\Drunk16.wav", NULL},   {1u, "Sfx\\Towners\\Drunk17.wav", NULL},
                       {1u, "Sfx\\Towners\\Drunk18.wav", NULL},   {1u, "Sfx\\Towners\\Drunk19.wav", NULL},
                       {1u, "Sfx\\Towners\\Drunk20.wav", NULL},   {1u, "Sfx\\Towners\\Drunk21.wav", NULL},
                       {1u, "Sfx\\Towners\\Drunk22.wav", NULL},   {1u, "Sfx\\Towners\\Drunk23.wav", NULL},
                       {1u, "Sfx\\Towners\\Drunk24.wav", NULL},   {1u, "Sfx\\Towners\\Drunk25.wav", NULL},
                       {1u, "Sfx\\Towners\\Drunk26.wav", NULL},   {1u, "Sfx\\Towners\\Drunk27.wav", NULL},
                       {1u, "Sfx\\Towners\\Drunk28.wav", NULL},   {1u, "Sfx\\Towners\\Drunk29.wav", NULL},
                       {1u, "Sfx\\Towners\\Drunk30.wav", NULL},   {1u, "Sfx\\Towners\\Drunk31.wav", NULL},
                       {1u, "Sfx\\Towners\\Drunk32.wav", NULL},   {1u, "Sfx\\Towners\\Drunk33.wav", NULL},
                       {1u, "Sfx\\Towners\\Drunk34.wav", NULL},   {1u, "Sfx\\Towners\\Drunk35.wav", NULL},
                       {1u, "Sfx\\Towners\\Healer01.wav", NULL},  {1u, "Sfx\\Towners\\Healer02.wav", NULL},
                       {1u, "Sfx\\Towners\\Healer03.wav", NULL},  {1u, "Sfx\\Towners\\Healer04.wav", NULL},
                       {1u, "Sfx\\Towners\\Healer05.wav", NULL},  {1u, "Sfx\\Towners\\Healer06.wav", NULL},
                       {1u, "Sfx\\Towners\\Healer07.wav", NULL},  {1u, "Sfx\\Towners\\Healer08.wav", NULL},
                       {1u, "Sfx\\Towners\\Healer09.wav", NULL},  {1u, "Sfx\\Towners\\Healer10.wav", NULL},
                       {1u, "Sfx\\Towners\\Healer11.wav", NULL},  {1u, "Sfx\\Towners\\Healer12.wav", NULL},
                       {1u, "Sfx\\Towners\\Healer13.wav", NULL},  {1u, "Sfx\\Towners\\Healer14.wav", NULL},
                       {1u, "Sfx\\Towners\\Healer15.wav", NULL},  {1u, "Sfx\\Towners\\Healer16.wav", NULL},
                       {1u, "Sfx\\Towners\\Healer17.wav", NULL},  {1u, "Sfx\\Towners\\Healer18.wav", NULL},
                       {1u, "Sfx\\Towners\\Healer19.wav", NULL},  {1u, "Sfx\\Towners\\Healer20.wav", NULL},
                       {1u, "Sfx\\Towners\\Healer21.wav", NULL},  {1u, "Sfx\\Towners\\Healer22.wav", NULL},
                       {1u, "Sfx\\Towners\\Healer23.wav", NULL},  {1u, "Sfx\\Towners\\Healer24.wav", NULL},
                       {1u, "Sfx\\Towners\\Healer25.wav", NULL},  {1u, "Sfx\\Towners\\Healer26.wav", NULL},
                       {1u, "Sfx\\Towners\\Healer27.wav", NULL},  {1u, "Sfx\\Towners\\Healer28.wav", NULL},
                       {1u, "Sfx\\Towners\\Healer29.wav", NULL},  {1u, "Sfx\\Towners\\Healer30.wav", NULL},
                       {1u, "Sfx\\Towners\\Healer31.wav", NULL},  {1u, "Sfx\\Towners\\Healer32.wav", NULL},
                       {1u, "Sfx\\Towners\\Healer33.wav", NULL},  {1u, "Sfx\\Towners\\Healer34.wav", NULL},
                       {1u, "Sfx\\Towners\\Healer35.wav", NULL},  {1u, "Sfx\\Towners\\Healer36.wav", NULL},
                       {1u, "Sfx\\Towners\\Healer37.wav", NULL},  {1u, "Sfx\\Towners\\Healer38.wav", NULL},
                       {1u, "Sfx\\Towners\\Healer39.wav", NULL},  {1u, "Sfx\\Towners\\Healer40.wav", NULL},
                       {1u, "Sfx\\Towners\\Healer41.wav", NULL},  {1u, "Sfx\\Towners\\Healer42.wav", NULL},
                       {1u, "Sfx\\Towners\\Healer43.wav", NULL},  {1u, "Sfx\\Towners\\Healer44.wav", NULL},
                       {1u, "Sfx\\Towners\\Healer45.wav", NULL},  {1u, "Sfx\\Towners\\Healer46.wav", NULL},
                       {1u, "Sfx\\Towners\\Healer47.wav", NULL},  {1u, "Sfx\\Towners\\Pegboy01.wav", NULL},
                       {1u, "Sfx\\Towners\\Pegboy02.wav", NULL},  {1u, "Sfx\\Towners\\Pegboy03.wav", NULL},
                       {1u, "Sfx\\Towners\\Pegboy04.wav", NULL},  {1u, "Sfx\\Towners\\Pegboy05.wav", NULL},
                       {1u, "Sfx\\Towners\\Pegboy06.wav", NULL},  {1u, "Sfx\\Towners\\Pegboy07.wav", NULL},
                       {1u, "Sfx\\Towners\\Pegboy08.wav", NULL},  {1u, "Sfx\\Towners\\Pegboy09.wav", NULL},
                       {1u, "Sfx\\Towners\\Pegboy10.wav", NULL},  {1u, "Sfx\\Towners\\Pegboy11.wav", NULL},
                       {1u, "Sfx\\Towners\\Pegboy12.wav", NULL},  {1u, "Sfx\\Towners\\Pegboy13.wav", NULL},
                       {1u, "Sfx\\Towners\\Pegboy14.wav", NULL},  {1u, "Sfx\\Towners\\Pegboy15.wav", NULL},
                       {1u, "Sfx\\Towners\\Pegboy16.wav", NULL},  {1u, "Sfx\\Towners\\Pegboy17.wav", NULL},
                       {1u, "Sfx\\Towners\\Pegboy18.wav", NULL},  {1u, "Sfx\\Towners\\Pegboy19.wav", NULL},
                       {1u, "Sfx\\Towners\\Pegboy20.wav", NULL},  {1u, "Sfx\\Towners\\Pegboy21.wav", NULL},
                       {1u, "Sfx\\Towners\\Pegboy22.wav", NULL},  {1u, "Sfx\\Towners\\Pegboy23.wav", NULL},
                       {1u, "Sfx\\Towners\\Pegboy24.wav", NULL},  {1u, "Sfx\\Towners\\Pegboy25.wav", NULL},
                       {1u, "Sfx\\Towners\\Pegboy26.wav", NULL},  {1u, "Sfx\\Towners\\Pegboy27.wav", NULL},
                       {1u, "Sfx\\Towners\\Pegboy28.wav", NULL},  {1u, "Sfx\\Towners\\Pegboy29.wav", NULL},
                       {1u, "Sfx\\Towners\\Pegboy30.wav", NULL},  {1u, "Sfx\\Towners\\Pegboy31.wav", NULL},
                       {1u, "Sfx\\Towners\\Pegboy32.wav", NULL},  {1u, "Sfx\\Towners\\Pegboy33.wav", NULL},
                       {1u, "Sfx\\Towners\\Pegboy34.wav", NULL},  {1u, "Sfx\\Towners\\Pegboy35.wav", NULL},
                       {1u, "Sfx\\Towners\\Pegboy36.wav", NULL},  {1u, "Sfx\\Towners\\Pegboy37.wav", NULL},
                       {1u, "Sfx\\Towners\\Pegboy38.wav", NULL},  {1u, "Sfx\\Towners\\Pegboy39.wav", NULL},
                       {1u, "Sfx\\Towners\\Pegboy40.wav", NULL},  {1u, "Sfx\\Towners\\Pegboy41.wav", NULL},
                       {1u, "Sfx\\Towners\\Pegboy42.wav", NULL},  {1u, "Sfx\\Towners\\Pegboy43.wav", NULL},
                       {1u, "Sfx\\Towners\\Priest00.wav", NULL},  {1u, "Sfx\\Towners\\Priest01.wav", NULL},
                       {1u, "Sfx\\Towners\\Priest02.wav", NULL},  {1u, "Sfx\\Towners\\Priest03.wav", NULL},
                       {1u, "Sfx\\Towners\\Priest04.wav", NULL},  {1u, "Sfx\\Towners\\Priest05.wav", NULL},
                       {1u, "Sfx\\Towners\\Priest06.wav", NULL},  {1u, "Sfx\\Towners\\Priest07.wav", NULL},
                       {1u, "Sfx\\Towners\\Storyt00.wav", NULL},  {1u, "Sfx\\Towners\\Storyt01.wav", NULL},
                       {1u, "Sfx\\Towners\\Storyt02.wav", NULL},  {1u, "Sfx\\Towners\\Storyt03.wav", NULL},
                       {1u, "Sfx\\Towners\\Storyt04.wav", NULL},  {1u, "Sfx\\Towners\\Storyt05.wav", NULL},
                       {1u, "Sfx\\Towners\\Storyt06.wav", NULL},  {1u, "Sfx\\Towners\\Storyt07.wav", NULL},
                       {1u, "Sfx\\Towners\\Storyt08.wav", NULL},  {1u, "Sfx\\Towners\\Storyt09.wav", NULL},
                       {1u, "Sfx\\Towners\\Storyt10.wav", NULL},  {1u, "Sfx\\Towners\\Storyt11.wav", NULL},
                       {1u, "Sfx\\Towners\\Storyt12.wav", NULL},  {1u, "Sfx\\Towners\\Storyt13.wav", NULL},
                       {1u, "Sfx\\Towners\\Storyt14.wav", NULL},  {1u, "Sfx\\Towners\\Storyt15.wav", NULL},
                       {1u, "Sfx\\Towners\\Storyt16.wav", NULL},  {1u, "Sfx\\Towners\\Storyt17.wav", NULL},
                       {1u, "Sfx\\Towners\\Storyt18.wav", NULL},  {1u, "Sfx\\Towners\\Storyt19.wav", NULL},
                       {1u, "Sfx\\Towners\\Storyt20.wav", NULL},  {1u, "Sfx\\Towners\\Storyt21.wav", NULL},
                       {1u, "Sfx\\Towners\\Storyt22.wav", NULL},  {1u, "Sfx\\Towners\\Storyt23.wav", NULL},
                       {1u, "Sfx\\Towners\\Storyt24.wav", NULL},  {1u, "Sfx\\Towners\\Storyt25.wav", NULL},
                       {1u, "Sfx\\Towners\\Storyt26.wav", NULL},  {1u, "Sfx\\Towners\\Storyt27.wav", NULL},
                       {1u, "Sfx\\Towners\\Storyt28.wav", NULL},  {1u, "Sfx\\Towners\\Storyt29.wav", NULL},
                       {1u, "Sfx\\Towners\\Storyt30.wav", NULL},  {1u, "Sfx\\Towners\\Storyt31.wav", NULL},
                       {1u, "Sfx\\Towners\\Storyt32.wav", NULL},  {1u, "Sfx\\Towners\\Storyt33.wav", NULL},
                       {1u, "Sfx\\Towners\\Storyt34.wav", NULL},  {1u, "Sfx\\Towners\\Storyt35.wav", NULL},
                       {1u, "Sfx\\Towners\\Storyt36.wav", NULL},  {1u, "Sfx\\Towners\\Storyt37.wav", NULL},
                       {1u, "Sfx\\Towners\\Storyt38.wav", NULL},  {1u, "Sfx\\Towners\\Tavown00.wav", NULL},
                       {1u, "Sfx\\Towners\\Tavown01.wav", NULL},  {1u, "Sfx\\Towners\\Tavown02.wav", NULL},
                       {1u, "Sfx\\Towners\\Tavown03.wav", NULL},  {1u, "Sfx\\Towners\\Tavown04.wav", NULL},
                       {1u, "Sfx\\Towners\\Tavown05.wav", NULL},  {1u, "Sfx\\Towners\\Tavown06.wav", NULL},
                       {1u, "Sfx\\Towners\\Tavown07.wav", NULL},  {1u, "Sfx\\Towners\\Tavown08.wav", NULL},
                       {1u, "Sfx\\Towners\\Tavown09.wav", NULL},  {1u, "Sfx\\Towners\\Tavown10.wav", NULL},
                       {1u, "Sfx\\Towners\\Tavown11.wav", NULL},  {1u, "Sfx\\Towners\\Tavown12.wav", NULL},
                       {1u, "Sfx\\Towners\\Tavown13.wav", NULL},  {1u, "Sfx\\Towners\\Tavown14.wav", NULL},
                       {1u, "Sfx\\Towners\\Tavown15.wav", NULL},  {1u, "Sfx\\Towners\\Tavown16.wav", NULL},
                       {1u, "Sfx\\Towners\\Tavown17.wav", NULL},  {1u, "Sfx\\Towners\\Tavown18.wav", NULL},
                       {1u, "Sfx\\Towners\\Tavown19.wav", NULL},  {1u, "Sfx\\Towners\\Tavown20.wav", NULL},
                       {1u, "Sfx\\Towners\\Tavown21.wav", NULL},  {1u, "Sfx\\Towners\\Tavown22.wav", NULL},
                       {1u, "Sfx\\Towners\\Tavown23.wav", NULL},  {1u, "Sfx\\Towners\\Tavown24.wav", NULL},
                       {1u, "Sfx\\Towners\\Tavown25.wav", NULL},  {1u, "Sfx\\Towners\\Tavown26.wav", NULL},
                       {1u, "Sfx\\Towners\\Tavown27.wav", NULL},  {1u, "Sfx\\Towners\\Tavown28.wav", NULL},
                       {1u, "Sfx\\Towners\\Tavown29.wav", NULL},  {1u, "Sfx\\Towners\\Tavown30.wav", NULL},
                       {1u, "Sfx\\Towners\\Tavown31.wav", NULL},  {1u, "Sfx\\Towners\\Tavown32.wav", NULL},
                       {1u, "Sfx\\Towners\\Tavown33.wav", NULL},  {1u, "Sfx\\Towners\\Tavown34.wav", NULL},
                       {1u, "Sfx\\Towners\\Tavown35.wav", NULL},  {1u, "Sfx\\Towners\\Tavown36.wav", NULL},
                       {1u, "Sfx\\Towners\\Tavown37.wav", NULL},  {1u, "Sfx\\Towners\\Tavown38.wav", NULL},
                       {1u, "Sfx\\Towners\\Tavown39.wav", NULL},  {1u, "Sfx\\Towners\\Tavown40.wav", NULL},
                       {1u, "Sfx\\Towners\\Tavown41.wav", NULL},  {1u, "Sfx\\Towners\\Tavown42.wav", NULL},
                       {1u, "Sfx\\Towners\\Tavown43.wav", NULL},  {1u, "Sfx\\Towners\\Tavown44.wav", NULL},
                       {1u, "Sfx\\Towners\\Tavown45.wav", NULL},  {1u, "Sfx\\Towners\\Witch01.wav", NULL},
                       {1u, "Sfx\\Towners\\Witch02.wav", NULL},   {1u, "Sfx\\Towners\\Witch03.wav", NULL},
                       {1u, "Sfx\\Towners\\Witch04.wav", NULL},   {1u, "Sfx\\Towners\\Witch05.wav", NULL},
                       {1u, "Sfx\\Towners\\Witch06.wav", NULL},   {1u, "Sfx\\Towners\\Witch07.wav", NULL},
                       {1u, "Sfx\\Towners\\Witch08.wav", NULL},   {1u, "Sfx\\Towners\\Witch09.wav", NULL},
                       {1u, "Sfx\\Towners\\Witch10.wav", NULL},   {1u, "Sfx\\Towners\\Witch11.wav", NULL},
                       {1u, "Sfx\\Towners\\Witch12.wav", NULL},   {1u, "Sfx\\Towners\\Witch13.wav", NULL},
                       {1u, "Sfx\\Towners\\Witch14.wav", NULL},   {1u, "Sfx\\Towners\\Witch15.wav", NULL},
                       {1u, "Sfx\\Towners\\Witch16.wav", NULL},   {1u, "Sfx\\Towners\\Witch17.wav", NULL},
                       {1u, "Sfx\\Towners\\Witch18.wav", NULL},   {1u, "Sfx\\Towners\\Witch19.wav", NULL},
                       {1u, "Sfx\\Towners\\Witch20.wav", NULL},   {1u, "Sfx\\Towners\\Witch21.wav", NULL},
                       {1u, "Sfx\\Towners\\Witch22.wav", NULL},   {1u, "Sfx\\Towners\\Witch23.wav", NULL},
                       {1u, "Sfx\\Towners\\Witch24.wav", NULL},   {1u, "Sfx\\Towners\\Witch25.wav", NULL},
                       {1u, "Sfx\\Towners\\Witch26.wav", NULL},   {1u, "Sfx\\Towners\\Witch27.wav", NULL},
                       {1u, "Sfx\\Towners\\Witch28.wav", NULL},   {1u, "Sfx\\Towners\\Witch29.wav", NULL},
                       {1u, "Sfx\\Towners\\Witch30.wav", NULL},   {1u, "Sfx\\Towners\\Witch31.wav", NULL},
                       {1u, "Sfx\\Towners\\Witch32.wav", NULL},   {1u, "Sfx\\Towners\\Witch33.wav", NULL},
                       {1u, "Sfx\\Towners\\Witch34.wav", NULL},   {1u, "Sfx\\Towners\\Witch35.wav", NULL},
                       {1u, "Sfx\\Towners\\Witch36.wav", NULL},   {1u, "Sfx\\Towners\\Witch37.wav", NULL},
                       {1u, "Sfx\\Towners\\Witch38.wav", NULL},   {1u, "Sfx\\Towners\\Witch39.wav", NULL},
                       {1u, "Sfx\\Towners\\Witch40.wav", NULL},   {1u, "Sfx\\Towners\\Witch41.wav", NULL},
                       {1u, "Sfx\\Towners\\Witch42.wav", NULL},   {1u, "Sfx\\Towners\\Witch43.wav", NULL},
                       {1u, "Sfx\\Towners\\Witch44.wav", NULL},   {1u, "Sfx\\Towners\\Witch45.wav", NULL},
                       {1u, "Sfx\\Towners\\Witch46.wav", NULL},   {1u, "Sfx\\Towners\\Witch47.wav", NULL},
                       {1u, "Sfx\\Towners\\Witch48.wav", NULL},   {1u, "Sfx\\Towners\\Witch49.wav", NULL},
                       {1u, "Sfx\\Towners\\Witch50.wav", NULL},   {1u, "Sfx\\Towners\\Wound01.wav", NULL},
                       {65u, "Sfx\\Sorceror\\Mage01.wav", NULL},  {65u, "Sfx\\Sorceror\\Mage02.wav", NULL},
                       {65u, "Sfx\\Sorceror\\Mage03.wav", NULL},  {65u, "Sfx\\Sorceror\\Mage04.wav", NULL},
                       {65u, "Sfx\\Sorceror\\Mage05.wav", NULL},  {65u, "Sfx\\Sorceror\\Mage06.wav", NULL},
                       {65u, "Sfx\\Sorceror\\Mage07.wav", NULL},  {65u, "Sfx\\Sorceror\\Mage08.wav", NULL},
                       {65u, "Sfx\\Sorceror\\Mage09.wav", NULL},  {65u, "Sfx\\Sorceror\\Mage10.wav", NULL},
                       {65u, "Sfx\\Sorceror\\Mage11.wav", NULL},  {65u, "Sfx\\Sorceror\\Mage12.wav", NULL},
                       {64u, "Sfx\\Sorceror\\Mage13.wav", NULL},  {64u, "Sfx\\Sorceror\\Mage14.wav", NULL},
                       {64u, "Sfx\\Sorceror\\Mage15.wav", NULL},  {64u, "Sfx\\Sorceror\\Mage16.wav", NULL},
                       {64u, "Sfx\\Sorceror\\Mage17.wav", NULL},  {64u, "Sfx\\Sorceror\\Mage18.wav", NULL},
                       {64u, "Sfx\\Sorceror\\Mage19.wav", NULL},  {64u, "Sfx\\Sorceror\\Mage20.wav", NULL},
                       {64u, "Sfx\\Sorceror\\Mage21.wav", NULL},  {64u, "Sfx\\Sorceror\\Mage22.wav", NULL},
                       {64u, "Sfx\\Sorceror\\Mage23.wav", NULL},  {64u, "Sfx\\Sorceror\\Mage24.wav", NULL},
                       {64u, "Sfx\\Sorceror\\Mage25.wav", NULL},  {64u, "Sfx\\Sorceror\\Mage26.wav", NULL},
                       {64u, "Sfx\\Sorceror\\Mage27.wav", NULL},  {64u, "Sfx\\Sorceror\\Mage28.wav", NULL},
                       {64u, "Sfx\\Sorceror\\Mage29.wav", NULL},  {64u, "Sfx\\Sorceror\\Mage30.wav", NULL},
                       {64u, "Sfx\\Sorceror\\Mage31.wav", NULL},  {64u, "Sfx\\Sorceror\\Mage32.wav", NULL},
                       {64u, "Sfx\\Sorceror\\Mage33.wav", NULL},  {64u, "Sfx\\Sorceror\\Mage34.wav", NULL},
                       {64u, "Sfx\\Sorceror\\Mage35.wav", NULL},  {64u, "Sfx\\Sorceror\\Mage36.wav", NULL},
                       {64u, "Sfx\\Sorceror\\Mage37.wav", NULL},  {64u, "Sfx\\Sorceror\\Mage38.wav", NULL},
                       {64u, "Sfx\\Sorceror\\Mage39.wav", NULL},  {64u, "Sfx\\Sorceror\\Mage40.wav", NULL},
                       {64u, "Sfx\\Sorceror\\Mage41.wav", NULL},  {64u, "Sfx\\Sorceror\\Mage42.wav", NULL},
                       {64u, "Sfx\\Sorceror\\Mage43.wav", NULL},  {64u, "Sfx\\Sorceror\\Mage44.wav", NULL},
                       {64u, "Sfx\\Sorceror\\Mage45.wav", NULL},  {64u, "Sfx\\Sorceror\\Mage46.wav", NULL},
                       {64u, "Sfx\\Sorceror\\Mage47.wav", NULL},  {64u, "Sfx\\Sorceror\\Mage48.wav", NULL},
                       {64u, "Sfx\\Sorceror\\Mage49.wav", NULL},  {64u, "Sfx\\Sorceror\\Mage50.wav", NULL},
                       {65u, "Sfx\\Sorceror\\Mage51.wav", NULL},  {65u, "Sfx\\Sorceror\\Mage52.wav", NULL},
                       {65u, "Sfx\\Sorceror\\Mage53.wav", NULL},  {65u, "Sfx\\Sorceror\\Mage54.wav", NULL},
                       {65u, "Sfx\\Sorceror\\Mage55.wav", NULL},  {65u, "Sfx\\Sorceror\\Mage56.wav", NULL},
                       {64u, "Sfx\\Sorceror\\Mage57.wav", NULL},  {65u, "Sfx\\Sorceror\\Mage58.wav", NULL},
                       {65u, "Sfx\\Sorceror\\Mage59.wav", NULL},  {65u, "Sfx\\Sorceror\\Mage60.wav", NULL},
                       {65u, "Sfx\\Sorceror\\Mage61.wav", NULL},  {65u, "Sfx\\Sorceror\\Mage62.wav", NULL},
                       {65u, "Sfx\\Sorceror\\Mage63.wav", NULL},  {64u, "Sfx\\Sorceror\\Mage64.wav", NULL},
                       {64u, "Sfx\\Sorceror\\Mage65.wav", NULL},  {64u, "Sfx\\Sorceror\\Mage66.wav", NULL},
                       {64u, "Sfx\\Sorceror\\Mage67.wav", NULL},  {64u, "Sfx\\Sorceror\\Mage68.wav", NULL},
                       {64u, "Sfx\\Sorceror\\Mage69.wav", NULL},  {64u, "Sfx\\Sorceror\\Mage69b.wav", NULL},
                       {64u, "Sfx\\Sorceror\\Mage70.wav", NULL},  {64u, "Sfx\\Sorceror\\Mage71.wav", NULL},
                       {64u, "Sfx\\Sorceror\\Mage72.wav", NULL},  {64u, "Sfx\\Sorceror\\Mage73.wav", NULL},
                       {64u, "Sfx\\Sorceror\\Mage74.wav", NULL},  {64u, "Sfx\\Sorceror\\Mage75.wav", NULL},
                       {64u, "Sfx\\Sorceror\\Mage76.wav", NULL},  {64u, "Sfx\\Sorceror\\Mage77.wav", NULL},
                       {64u, "Sfx\\Sorceror\\Mage78.wav", NULL},  {64u, "Sfx\\Sorceror\\Mage79.wav", NULL},
                       {65u, "Sfx\\Sorceror\\Mage80.wav", NULL},  {65u, "Sfx\\Sorceror\\Mage81.wav", NULL},
                       {65u, "Sfx\\Sorceror\\Mage82.wav", NULL},  {65u, "Sfx\\Sorceror\\Mage83.wav", NULL},
                       {65u, "Sfx\\Sorceror\\Mage84.wav", NULL},  {65u, "Sfx\\Sorceror\\Mage85.wav", NULL},
                       {65u, "Sfx\\Sorceror\\Mage86.wav", NULL},  {65u, "Sfx\\Sorceror\\Mage87.wav", NULL},
                       {65u, "Sfx\\Sorceror\\Mage88.wav", NULL},  {65u, "Sfx\\Sorceror\\Mage89.wav", NULL},
                       {65u, "Sfx\\Sorceror\\Mage90.wav", NULL},  {65u, "Sfx\\Sorceror\\Mage91.wav", NULL},
                       {65u, "Sfx\\Sorceror\\Mage92.wav", NULL},  {65u, "Sfx\\Sorceror\\Mage93.wav", NULL},
                       {65u, "Sfx\\Sorceror\\Mage94.wav", NULL},  {65u, "Sfx\\Sorceror\\Mage95.wav", NULL},
                       {65u, "Sfx\\Sorceror\\Mage96.wav", NULL},  {65u, "Sfx\\Sorceror\\Mage97.wav", NULL},
                       {65u, "Sfx\\Sorceror\\Mage98.wav", NULL},  {65u, "Sfx\\Sorceror\\Mage99.wav", NULL},
                       {65u, "Sfx\\Sorceror\\Mage100.wav", NULL}, {65u, "Sfx\\Sorceror\\Mage101.wav", NULL},
                       {65u, "Sfx\\Sorceror\\Mage102.wav", NULL}, {17u, "Sfx\\Rogue\\Rogue01.wav", NULL},
                       {17u, "Sfx\\Rogue\\Rogue02.wav", NULL},    {17u, "Sfx\\Rogue\\Rogue03.wav", NULL},
                       {17u, "Sfx\\Rogue\\Rogue04.wav", NULL},    {17u, "Sfx\\Rogue\\Rogue05.wav", NULL},
                       {17u, "Sfx\\Rogue\\Rogue06.wav", NULL},    {17u, "Sfx\\Rogue\\Rogue07.wav", NULL},
                       {17u, "Sfx\\Rogue\\Rogue08.wav", NULL},    {17u, "Sfx\\Rogue\\Rogue09.wav", NULL},
                       {17u, "Sfx\\Rogue\\Rogue10.wav", NULL},    {17u, "Sfx\\Rogue\\Rogue11.wav", NULL},
                       {17u, "Sfx\\Rogue\\Rogue12.wav", NULL},    {16u, "Sfx\\Rogue\\Rogue13.wav", NULL},
                       {16u, "Sfx\\Rogue\\Rogue14.wav", NULL},    {16u, "Sfx\\Rogue\\Rogue15.wav", NULL},
                       {16u, "Sfx\\Rogue\\Rogue16.wav", NULL},    {16u, "Sfx\\Rogue\\Rogue17.wav", NULL},
                       {16u, "Sfx\\Rogue\\Rogue18.wav", NULL},    {16u, "Sfx\\Rogue\\Rogue19.wav", NULL},
                       {16u, "Sfx\\Rogue\\Rogue20.wav", NULL},    {16u, "Sfx\\Rogue\\Rogue21.wav", NULL},
                       {16u, "Sfx\\Rogue\\Rogue22.wav", NULL},    {16u, "Sfx\\Rogue\\Rogue23.wav", NULL},
                       {16u, "Sfx\\Rogue\\Rogue24.wav", NULL},    {16u, "Sfx\\Rogue\\Rogue25.wav", NULL},
                       {16u, "Sfx\\Rogue\\Rogue26.wav", NULL},    {16u, "Sfx\\Rogue\\Rogue27.wav", NULL},
                       {16u, "Sfx\\Rogue\\Rogue28.wav", NULL},    {16u, "Sfx\\Rogue\\Rogue29.wav", NULL},
                       {16u, "Sfx\\Rogue\\Rogue30.wav", NULL},    {16u, "Sfx\\Rogue\\Rogue31.wav", NULL},
                       {16u, "Sfx\\Rogue\\Rogue32.wav", NULL},    {16u, "Sfx\\Rogue\\Rogue33.wav", NULL},
                       {16u, "Sfx\\Rogue\\Rogue34.wav", NULL},    {16u, "Sfx\\Rogue\\Rogue35.wav", NULL},
                       {16u, "Sfx\\Rogue\\Rogue36.wav", NULL},    {16u, "Sfx\\Rogue\\Rogue37.wav", NULL},
                       {16u, "Sfx\\Rogue\\Rogue38.wav", NULL},    {16u, "Sfx\\Rogue\\Rogue39.wav", NULL},
                       {16u, "Sfx\\Rogue\\Rogue40.wav", NULL},    {16u, "Sfx\\Rogue\\Rogue41.wav", NULL},
                       {16u, "Sfx\\Rogue\\Rogue42.wav", NULL},    {16u, "Sfx\\Rogue\\Rogue43.wav", NULL},
                       {16u, "Sfx\\Rogue\\Rogue44.wav", NULL},    {16u, "Sfx\\Rogue\\Rogue45.wav", NULL},
                       {16u, "Sfx\\Rogue\\Rogue46.wav", NULL},    {16u, "Sfx\\Rogue\\Rogue47.wav", NULL},
                       {16u, "Sfx\\Rogue\\Rogue48.wav", NULL},    {16u, "Sfx\\Rogue\\Rogue49.wav", NULL},
                       {16u, "Sfx\\Rogue\\Rogue50.wav", NULL},    {17u, "Sfx\\Rogue\\Rogue51.wav", NULL},
                       {17u, "Sfx\\Rogue\\Rogue52.wav", NULL},    {17u, "Sfx\\Rogue\\Rogue53.wav", NULL},
                       {17u, "Sfx\\Rogue\\Rogue54.wav", NULL},    {17u, "Sfx\\Rogue\\Rogue55.wav", NULL},
                       {17u, "Sfx\\Rogue\\Rogue56.wav", NULL},    {16u, "Sfx\\Rogue\\Rogue57.wav", NULL},
                       {17u, "Sfx\\Rogue\\Rogue58.wav", NULL},    {17u, "Sfx\\Rogue\\Rogue59.wav", NULL},
                       {17u, "Sfx\\Rogue\\Rogue60.wav", NULL},    {17u, "Sfx\\Rogue\\Rogue61.wav", NULL},
                       {17u, "Sfx\\Rogue\\Rogue62.wav", NULL},    {17u, "Sfx\\Rogue\\Rogue63.wav", NULL},
                       {16u, "Sfx\\Rogue\\Rogue64.wav", NULL},    {16u, "Sfx\\Rogue\\Rogue65.wav", NULL},
                       {16u, "Sfx\\Rogue\\Rogue66.wav", NULL},    {16u, "Sfx\\Rogue\\Rogue67.wav", NULL},
                       {16u, "Sfx\\Rogue\\Rogue68.wav", NULL},    {16u, "Sfx\\Rogue\\Rogue69.wav", NULL},
                       {16u, "Sfx\\Rogue\\Rogue69b.wav", NULL},   {16u, "Sfx\\Rogue\\Rogue70.wav", NULL},
                       {16u, "Sfx\\Rogue\\Rogue71.wav", NULL},    {16u, "Sfx\\Rogue\\Rogue72.wav", NULL},
                       {16u, "Sfx\\Rogue\\Rogue73.wav", NULL},    {16u, "Sfx\\Rogue\\Rogue74.wav", NULL},
                       {16u, "Sfx\\Rogue\\Rogue75.wav", NULL},    {16u, "Sfx\\Rogue\\Rogue76.wav", NULL},
                       {16u, "Sfx\\Rogue\\Rogue77.wav", NULL},    {16u, "Sfx\\Rogue\\Rogue78.wav", NULL},
                       {16u, "Sfx\\Rogue\\Rogue79.wav", NULL},    {17u, "Sfx\\Rogue\\Rogue80.wav", NULL},
                       {17u, "Sfx\\Rogue\\Rogue81.wav", NULL},    {17u, "Sfx\\Rogue\\Rogue82.wav", NULL},
                       {17u, "Sfx\\Rogue\\Rogue83.wav", NULL},    {17u, "Sfx\\Rogue\\Rogue84.wav", NULL},
                       {17u, "Sfx\\Rogue\\Rogue85.wav", NULL},    {17u, "Sfx\\Rogue\\Rogue86.wav", NULL},
                       {17u, "Sfx\\Rogue\\Rogue87.wav", NULL},    {17u, "Sfx\\Rogue\\Rogue88.wav", NULL},
                       {17u, "Sfx\\Rogue\\Rogue89.wav", NULL},    {17u, "Sfx\\Rogue\\Rogue90.wav", NULL},
                       {17u, "Sfx\\Rogue\\Rogue91.wav", NULL},    {17u, "Sfx\\Rogue\\Rogue92.wav", NULL},
                       {17u, "Sfx\\Rogue\\Rogue93.wav", NULL},    {17u, "Sfx\\Rogue\\Rogue94.wav", NULL},
                       {17u, "Sfx\\Rogue\\Rogue95.wav", NULL},    {17u, "Sfx\\Rogue\\Rogue96.wav", NULL},
                       {17u, "Sfx\\Rogue\\Rogue97.wav", NULL},    {17u, "Sfx\\Rogue\\Rogue98.wav", NULL},
                       {17u, "Sfx\\Rogue\\Rogue99.wav", NULL},    {17u, "Sfx\\Rogue\\Rogue100.wav", NULL},
                       {17u, "Sfx\\Rogue\\Rogue101.wav", NULL},   {17u, "Sfx\\Rogue\\Rogue102.wav", NULL},
                       {33u, "Sfx\\Warrior\\Warior01.wav", NULL}, {33u, "Sfx\\Warrior\\Warior02.wav", NULL},
                       {33u, "Sfx\\Warrior\\Warior03.wav", NULL}, {33u, "Sfx\\Warrior\\Warior04.wav", NULL},
                       {33u, "Sfx\\Warrior\\Warior05.wav", NULL}, {33u, "Sfx\\Warrior\\Warior06.wav", NULL},
                       {33u, "Sfx\\Warrior\\Warior07.wav", NULL}, {33u, "Sfx\\Warrior\\Warior08.wav", NULL},
                       {33u, "Sfx\\Warrior\\Warior09.wav", NULL}, {33u, "Sfx\\Warrior\\Warior10.wav", NULL},
                       {33u, "Sfx\\Warrior\\Warior11.wav", NULL}, {33u, "Sfx\\Warrior\\Warior12.wav", NULL},
                       {32u, "Sfx\\Warrior\\Warior13.wav", NULL}, {32u, "Sfx\\Warrior\\Warior14.wav", NULL},
                       {32u, "Sfx\\Warrior\\Wario14b.wav", NULL}, {32u, "Sfx\\Warrior\\Wario14c.wav", NULL},
                       {32u, "Sfx\\Warrior\\Warior15.wav", NULL}, {32u, "Sfx\\Warrior\\Wario15b.wav", NULL},
                       {32u, "Sfx\\Warrior\\Wario15c.wav", NULL}, {32u, "Sfx\\Warrior\\Warior16.wav", NULL},
                       {32u, "Sfx\\Warrior\\Wario16b.wav", NULL}, {32u, "Sfx\\Warrior\\Wario16c.wav", NULL},
                       {32u, "Sfx\\Warrior\\Warior17.wav", NULL}, {32u, "Sfx\\Warrior\\Warior18.wav", NULL},
                       {32u, "Sfx\\Warrior\\Warior19.wav", NULL}, {32u, "Sfx\\Warrior\\Warior20.wav", NULL},
                       {32u, "Sfx\\Warrior\\Warior21.wav", NULL}, {32u, "Sfx\\Warrior\\Warior22.wav", NULL},
                       {32u, "Sfx\\Warrior\\Warior23.wav", NULL}, {32u, "Sfx\\Warrior\\Warior24.wav", NULL},
                       {32u, "Sfx\\Warrior\\Warior25.wav", NULL}, {32u, "Sfx\\Warrior\\Warior26.wav", NULL},
                       {32u, "Sfx\\Warrior\\Warior27.wav", NULL}, {32u, "Sfx\\Warrior\\Warior28.wav", NULL},
                       {32u, "Sfx\\Warrior\\Warior29.wav", NULL}, {32u, "Sfx\\Warrior\\Warior30.wav", NULL},
                       {32u, "Sfx\\Warrior\\Warior31.wav", NULL}, {32u, "Sfx\\Warrior\\Warior32.wav", NULL},
                       {32u, "Sfx\\Warrior\\Warior33.wav", NULL}, {32u, "Sfx\\Warrior\\Warior34.wav", NULL},
                       {32u, "Sfx\\Warrior\\Warior35.wav", NULL}, {32u, "Sfx\\Warrior\\Warior36.wav", NULL},
                       {32u, "Sfx\\Warrior\\Warior37.wav", NULL}, {32u, "Sfx\\Warrior\\Warior38.wav", NULL},
                       {32u, "Sfx\\Warrior\\Warior39.wav", NULL}, {32u, "Sfx\\Warrior\\Warior40.wav", NULL},
                       {32u, "Sfx\\Warrior\\Warior41.wav", NULL}, {32u, "Sfx\\Warrior\\Warior42.wav", NULL},
                       {32u, "Sfx\\Warrior\\Warior43.wav", NULL}, {32u, "Sfx\\Warrior\\Warior44.wav", NULL},
                       {32u, "Sfx\\Warrior\\Warior45.wav", NULL}, {32u, "Sfx\\Warrior\\Warior46.wav", NULL},
                       {32u, "Sfx\\Warrior\\Warior47.wav", NULL}, {32u, "Sfx\\Warrior\\Warior48.wav", NULL},
                       {32u, "Sfx\\Warrior\\Warior49.wav", NULL}, {32u, "Sfx\\Warrior\\Warior50.wav", NULL},
                       {33u, "Sfx\\Warrior\\Warior51.wav", NULL}, {33u, "Sfx\\Warrior\\Warior52.wav", NULL},
                       {33u, "Sfx\\Warrior\\Warior53.wav", NULL}, {33u, "Sfx\\Warrior\\Warior54.wav", NULL},
                       {33u, "Sfx\\Warrior\\Warior55.wav", NULL}, {33u, "Sfx\\Warrior\\Warior56.wav", NULL},
                       {32u, "Sfx\\Warrior\\Warior57.wav", NULL}, {33u, "Sfx\\Warrior\\Warior58.wav", NULL},
                       {33u, "Sfx\\Warrior\\Warior59.wav", NULL}, {33u, "Sfx\\Warrior\\Warior60.wav", NULL},
                       {33u, "Sfx\\Warrior\\Warior61.wav", NULL}, {33u, "Sfx\\Warrior\\Warior62.wav", NULL},
                       {33u, "Sfx\\Warrior\\Warior63.wav", NULL}, {32u, "Sfx\\Warrior\\Warior64.wav", NULL},
                       {32u, "Sfx\\Warrior\\Warior65.wav", NULL}, {32u, "Sfx\\Warrior\\Warior66.wav", NULL},
                       {32u, "Sfx\\Warrior\\Warior67.wav", NULL}, {32u, "Sfx\\Warrior\\Warior68.wav", NULL},
                       {32u, "Sfx\\Warrior\\Warior69.wav", NULL}, {32u, "Sfx\\Warrior\\Wario69b.wav", NULL},
                       {32u, "Sfx\\Warrior\\Warior70.wav", NULL}, {32u, "Sfx\\Warrior\\Warior71.wav", NULL},
                       {32u, "Sfx\\Warrior\\Warior72.wav", NULL}, {32u, "Sfx\\Warrior\\Warior73.wav", NULL},
                       {32u, "Sfx\\Warrior\\Warior74.wav", NULL}, {32u, "Sfx\\Warrior\\Warior75.wav", NULL},
                       {32u, "Sfx\\Warrior\\Warior76.wav", NULL}, {32u, "Sfx\\Warrior\\Warior77.wav", NULL},
                       {32u, "Sfx\\Warrior\\Warior78.wav", NULL}, {32u, "Sfx\\Warrior\\Warior79.wav", NULL},
                       {33u, "Sfx\\Warrior\\Warior80.wav", NULL}, {33u, "Sfx\\Warrior\\Warior81.wav", NULL},
                       {33u, "Sfx\\Warrior\\Warior82.wav", NULL}, {33u, "Sfx\\Warrior\\Warior83.wav", NULL},
                       {33u, "Sfx\\Warrior\\Warior84.wav", NULL}, {33u, "Sfx\\Warrior\\Warior85.wav", NULL},
                       {33u, "Sfx\\Warrior\\Warior86.wav", NULL}, {33u, "Sfx\\Warrior\\Warior87.wav", NULL},
                       {33u, "Sfx\\Warrior\\Warior88.wav", NULL}, {33u, "Sfx\\Warrior\\Warior89.wav", NULL},
                       {33u, "Sfx\\Warrior\\Warior90.wav", NULL}, {33u, "Sfx\\Warrior\\Warior91.wav", NULL},
                       {33u, "Sfx\\Warrior\\Warior92.wav", NULL}, {33u, "Sfx\\Warrior\\Warior93.wav", NULL},
                       {33u, "Sfx\\Warrior\\Warior94.wav", NULL}, {33u, "Sfx\\Warrior\\Warior95.wav", NULL},
                       {33u, "Sfx\\Warrior\\Wario95b.wav", NULL}, {33u, "Sfx\\Warrior\\Wario95c.wav", NULL},
                       {33u, "Sfx\\Warrior\\Wario95d.wav", NULL}, {33u, "Sfx\\Warrior\\Wario95e.wav", NULL},
                       {33u, "Sfx\\Warrior\\Wario95f.wav", NULL}, {33u, "Sfx\\Warrior\\Wario96b.wav", NULL},
                       {33u, "Sfx\\Warrior\\Wario97.wav", NULL},  {33u, "Sfx\\Warrior\\Wario98.wav", NULL},
                       {33u, "Sfx\\Warrior\\Warior99.wav", NULL}, {33u, "Sfx\\Warrior\\Wario100.wav", NULL},
                       {33u, "Sfx\\Warrior\\Wario101.wav", NULL}, {33u, "Sfx\\Warrior\\Wario102.wav", NULL},
                       {1u, "Sfx\\Narrator\\Nar01.wav", NULL},    {1u, "Sfx\\Narrator\\Nar02.wav", NULL},
                       {1u, "Sfx\\Narrator\\Nar03.wav", NULL},    {1u, "Sfx\\Narrator\\Nar04.wav", NULL},
                       {1u, "Sfx\\Narrator\\Nar05.wav", NULL},    {1u, "Sfx\\Narrator\\Nar06.wav", NULL},
                       {1u, "Sfx\\Narrator\\Nar07.wav", NULL},    {1u, "Sfx\\Narrator\\Nar08.wav", NULL},
                       {1u, "Sfx\\Narrator\\Nar09.wav", NULL},    {1u, "Sfx\\Misc\\Lvl16int.wav", NULL},
                       {1u, "Sfx\\Monsters\\Butcher.wav", NULL},  {1u, "Sfx\\Monsters\\Garbud01.wav", NULL},
                       {1u, "Sfx\\Monsters\\Garbud02.wav", NULL}, {1u, "Sfx\\Monsters\\Garbud03.wav", NULL},
                       {1u, "Sfx\\Monsters\\Garbud04.wav", NULL}, {1u, "Sfx\\Monsters\\Izual01.wav", NULL},
                       {1u, "Sfx\\Monsters\\Lach01.wav", NULL},   {1u, "Sfx\\Monsters\\Lach02.wav", NULL},
                       {1u, "Sfx\\Monsters\\Lach03.wav", NULL},   {1u, "Sfx\\Monsters\\Laz01.wav", NULL},
                       {1u, "Sfx\\Monsters\\Laz02.wav", NULL},    {1u, "Sfx\\Monsters\\Sking01.wav", NULL},
                       {1u, "Sfx\\Monsters\\Snot01.wav", NULL},   {1u, "Sfx\\Monsters\\Snot02.wav", NULL},
                       {1u, "Sfx\\Monsters\\Snot03.wav", NULL},   {1u, "Sfx\\Monsters\\Warlrd01.wav", NULL},
                       {1u, "Sfx\\Monsters\\Wlock01.wav", NULL},  {1u, "Sfx\\Monsters\\Zhar01.wav", NULL},
                       {1u, "Sfx\\Monsters\\Zhar02.wav", NULL},   {1u, "Sfx\\Monsters\\DiabloD.wav", NULL}};

struct effects_cpp_init {
	effects_cpp_init() { effects_cpp_init_value = effects_inf; }
} _effects_cpp_init;
// 47A468: using guessed type int effects_inf;
// 52A550: using guessed type int effects_cpp_init_value;

bool __fastcall effect_is_playing(int nSFX)
{
	TSFX *v1; // eax
	TSnd *v2; // ecx

	v1 = &sgSFX[nSFX];
	v2 = v1->pSnd;
//	printf("DEBUG effect_is_playing %s\n", v2);
	if (v2)
		return snd_playing(v2);
	if (v1->bFlags & 1)
		return v1 == sfx_data_cur;
	return 0;
}

void __cdecl sfx_stop()
{
	if (sfx_stream) {
		SFileDdaEnd(sfx_stream);
		SFileCloseFile(sfx_stream);
		sfx_stream = 0;
		sfx_data_cur = 0;
	}
}

void __fastcall InitMonsterSND(int monst)
{
	signed int v1; // ebx
	int v2; // eax
	TSnd **v3; // esi
	int v4; // edi
	size_t v5; // eax
	TSnd *v6; // eax
	char v7[260]; // [esp+0h] [ebp-110h]
	int v8; // [esp+104h] [ebp-Ch]
	int v9; // [esp+108h] [ebp-8h]
	char *ptr; // [esp+10Ch] [ebp-4h]

	v8 = monst;
	if (gbSndInited) {
		v1 = 0;
		v9 = Monsters[monst].mtype << 7;
		do {
			if (monster_action_sounds[v1] != 's' || *(int *)((char *)&monsterdata[0].snd_special + v9)) {
				v2 = 0;
				v3 = &Monsters[0].Snds[2 * (v1 + 41 * v8)];
				do {
					v4 = v2 + 1;
					sprintf(v7, *(const char **)((char *)&monsterdata[0].sndfile + v9), monster_action_sounds[v1],
					        v2 + 1);
					v5 = strlen(v7);
					ptr = (char *)DiabloAllocPtr(v5 + 1);
					strcpy(ptr, v7);

					printf("DEBUG InitMonsterSND\n");
					v6 = sound_file_load(ptr);
					*v3 = v6;
					if (!v6)
						mem_free_dbg(ptr);
					v2 = v4;
					++v3;
				} while (v4 < 2);
			}
			++v1;
		} while (v1 < 4);
	}
}

void __cdecl FreeEffects()
{
	printf("FreeEffects");
	TSnd **v0; // esi
	signed int v1; // ebp
	signed int v2; // ebx
	TSnd *v3; // ecx
	void *v4; // edi
	TSnd **v5; // [esp+0h] [ebp-8h]
	int v6; // [esp+4h] [ebp-4h]

	v6 = 0;
	if (nummtypes > 0) {
		v5 = Monsters[0].Snds;
		do {
			v0 = v5;
			v1 = 4;
			do {
				v2 = 2;
				do {
					v3 = *v0;
					if (*v0) {
						*v0 = 0;
						v4 = (void *)v3->sound_path;
						v3->sound_path = 0;
						sound_file_cleanup(v3);
						mem_free_dbg(v4);
					}
					++v0;
					--v2;
				} while (v2);
				--v1;
			} while (v1);
			++v6;
			v5 += 82;
		} while (v6 < nummtypes);
	}
}

void __fastcall PlayEffect(int i, int mode)
{
	int v2; // edi
	int v3; // esi
	int v4; // eax
	int v5; // esi
	int v6; // eax
	TSnd *v7; // edi
    TSnd var;
    

	// int v8; // eax
	int volume_delta; // [esp+8h] [ebp-8h]
	int pan; // [esp+Ch] [ebp-4h]

    foo meh;

	

	v2 = mode;
	v3 = i;
	if (!plr[myplr].pLvlLoad) {
		v4 = random(164, 2);
		if (gbSndInited) {
			if (gbSoundOn) {
				if (!gbBufferMsgs) {
					v5 = v3;
					v6 = v4 + 2 * (v2 + 41 * monster[v5]._mMTidx);
					v7 = Monsters[0].Snds[v6];
					if (v7) {
						//_LOBYTE(v8) = snd_playing(Monsters[0].Snds[v6]);
                        printf("DEBUG PlayEffect *sound_path %p\n", Monsters[0].Snds[v6]);
						if (!snd_playing(Monsters[0].Snds[v6])) {
							if (calc_snd_position(monster[v5]._mx, monster[v5]._my, &volume_delta, &pan))
								snd_play_snd(v7, volume_delta, pan);
						}
					}
				}
			}
		}
	}
}
// 4A22D5: using guessed type char gbSoundOn;
// 676194: using guessed type char gbBufferMsgs;

int __fastcall calc_snd_position(int x, int y, int *plVolume, int *plPan)
{
	printf(" calc_snd_position activated\n");
	int v4; // edi
	int v5; // esi
	int v6; // eax
	int v7; // ebx
	int v8; // eax
	int v9; // eax

	v4 = x - plr[myplr].WorldX;
	v5 = y - plr[myplr].WorldY;
	v6 = (v4 - v5) << 8;
	*plPan = v6;
	if (abs(v6) > 6400)
		return 0;
	v7 = abs(v4);
	v8 = v7 <= abs(v5) ? abs(v5) : abs(v4);
	v9 = v8 << 6;
	*plVolume = v9;
	if (v9 >= 6400)
		return 0;
	*plVolume = -v9;
	return 1;
}

void __fastcall PlaySFX(int psfx)
{
	//printf("HERE PlaySFX");
	int v1; // eax
	v1 = RndSFX(psfx);
	PlaySFX_priv(&sgSFX[v1], 0, 0, 0);
}



void __fastcall PlaySFX_priv(TSFX *pSFX, char loc, int x, int y)
{
	//printf("PlaySFX_priv activated\n");
	int v4; // edi
	TSFX *v5; // esi
	TSnd *v6; // ecx
	// int v7; // eax
	TSnd *v8; // ecx
	int volume_delta; // [esp+Ch] [ebp-8h]
	int pan; // [esp+10h] [ebp-4h]
    TSFX *sound;

	v4 = loc;
	v5 = pSFX;
    sound = pSFX;
    char * wav = sound->pszName;
    if(gbSndInited && gbSoundOn){
    printf("Sound I want to Play %s\n\n",wav );
    calc_snd_position(x, y, &volume_delta, &pan);
    
    LoadAndPlaySound(wav,volume_delta,pan);


    }
    









	// if (!plr[myplr].pLvlLoad || gbMaxPlayers == 1) {
	// 	if (gbSndInited) {
	// 		if (gbSoundOn) {
	// 			if (!gbBufferMsgs) {
	// 				if (pSFX->bFlags & 3 || (v6 = pSFX->pSnd) == 0 || !snd_playing(v6)) {
	// 					pan = 0;
	// 					volume_delta = 0;
	// 					if (!v4 || calc_snd_position(x, y, &volume_delta, &pan)) {
	// 						if (v5->bFlags & 1) {

	// 							//	stream_play(v5, volume_delta, pan); // Debuf
	// 						} else {
	// 							if (!v5->pSnd)
	// 								v5->pSnd = sound_file_load(v5->pszName);
	// 							v8 = v5->pSnd;
	// 							if (v8)
    //                               //  printf("Playing Sound If Not Loaded %d\n\n", v8);
	// 								snd_play_snd(v8, volume_delta, pan);
	// 						}
	// 					}
	// 				}
	// 			}
	// 		}
	// 	}
	// }
}
// 4A22D5: using guessed type char gbSoundOn;
// 676194: using guessed type char gbBufferMsgs;
// 679660: using guessed type char gbMaxPlayers;

void __fastcall stream_play(TSFX *pSFX, int lVolume, int lPan)
{
    printf("stream_play\n");
	// printf("stream_play\n");
	// int v3; // esi
	// TSFX *v4; // edi
	// int v5; // esi
	// // int v6; // eax
	// // int v7; // eax

	// v3 = lVolume;
	// v4 = pSFX;
	// sfx_stop();
	// v5 = sound_get_or_set_sound_volume(1) + v3;
	// if (v5 >= -1600) {
	// 	if (v5 > 0)
	// 		v5 = 0;
	// 	//_LOBYTE(v6) = SFileOpenFile(v4->pszName, &sfx_stream);
	// 	if (SFileOpenFile(v4->pszName, &sfx_stream)) {
	// 		//_LOBYTE(v7) = SFileDdaBeginEx(sfx_stream, 0x40000, 0, 0, v5, lPan, 0);
	// 		if (SFileDdaBeginEx(sfx_stream, 0x40000, 0, 0, v5, lPan,
	// 		                    0)) // This shouldn't be called but it does get called
	// 			sfx_data_cur = v4;
	// 		else
	// 			sfx_stop();
	// 	} else {
	// 		sfx_stream = 0;
	// 	}
	// }
}

int __fastcall RndSFX(int psfx)
{
	printf("RndSFX\n");
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
	printf("PlaySfxLoc\n");
	// int v3; // esi
	// int v4; // eax
	// TSnd *v5; // ecx

	// v3 = x;
	// v4 = RndSFX(psfx); // Crash here....
	// if ( v4 >= 0 && v4 <= 3 )
	// {
	// 	v5 = sgSFX[v4].pSnd;
	// 	if ( v5 )
	// 		v5->start_tc = 0;
	// }
	// PlaySFX_priv(&sgSFX[v4], 1, v3, y);
}

void __cdecl FreeMonsterSnd()
{
    printf("FreeMonsterSnd");
	TSnd **v0; // esi
	signed int v1; // ebx
	signed int v2; // edi
	int v3; // [esp+0h] [ebp-8h]
	TSnd **v4; // [esp+4h] [ebp-4h]

	snd_update(1);
	sfx_stop();
	sound_stop();
	v3 = 0;
	if (nummtypes > 0) {
		v4 = Monsters[0].Snds;
		do {
			v0 = v4;
			v1 = 4;
			do {
				v2 = 2;
				do {
					snd_stop_snd(*v0);
					++v0;
					--v2;
				} while (v2);
				--v1;
			} while (v1);
			++v3;
			v4 += 82;
		} while (v3 < nummtypes);
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
{// This is called 

   // printf("sound_update\n");
	// unsigned int v3; // [esp-Ch] [ebp-Ch]
	// unsigned int v4; // [esp-8h] [ebp-8h]

	// if (gbSndInited) {
	// 	snd_update(0);

	// 	if (sfx_stream) {
	// 		if (SFileDdaGetPos(sfx_stream, (int)&v4, (int)&v3)) {
	// 			if (v4 >= v3)
	// 				sfx_stop();
	// 		}
	// 	}
	// }
}
// 415DBA: could not find valid save-restore pair for ebp

void __cdecl effects_cleanup_sfx()
{
	unsigned int v0; // edi
	TSnd *v1; // ecx

	FreeMonsterSnd();
	v0 = 0;
	do {
		v1 = sgSFX[v0].pSnd;
		if (v1) {
			sound_file_cleanup(v1);
			sgSFX[v0].pSnd = 0;
		}
		++v0;
	} while (v0 < NUM_SFX);
}

void __cdecl stream_update()
{
	char v0; // bl
	char v1; // al

	v0 = 0;
	if ((unsigned char)gbMaxPlayers <= 1u) {
		v1 = plr[myplr]._pClass;
		if (v1) {
			if (v1 == 1) {
				v0 = 16;
			} else if (v1 == 2) {
				v0 = 64;
			} else {
				TermMsg("effects:1");
			}
		} else {
			v0 = 32;
		}
	} else {
		v0 = 112;
	}
	priv_sound_init(v0);
}
// 679660: using guessed type char gbMaxPlayers;

void __fastcall priv_sound_init(int bLoadMask)
{
	unsigned char v1; // bl
	unsigned char v2; // cl
	unsigned int v3; // esi
	unsigned char v4; // al
	TSnd *v5; // eax
	unsigned char v6; // [esp+0h] [ebp-4h]

	if (gbSndInited) {
		v1 = bLoadMask & 0x70;
		v2 = bLoadMask & 0x70 ^ bLoadMask;
		v3 = 0;
		v6 = v2;
		do {
			if (!sgSFX[v3].pSnd) {
				v4 = sgSFX[v3].bFlags;
				if (!(v4 & 1) && (!v2 || v4 & v2) && (!(v4 & 0x70) || v4 & v1)) {
					v5 = sound_file_load(sgSFX[v3].pszName);
					v2 = v6;
					sgSFX[v3].pSnd = v5;
				}
			}
			++v3;
		} while (v3 < NUM_SFX);
	}
}

void __cdecl sound_init()
{
	priv_sound_init(4);
}

void __stdcall effects_play_sound(char *snd_file)
{
	int v1; // edi
	unsigned int v2; // esi
	TSnd **v3; // esi
	// int v4; // eax

	if (gbSndInited && gbSoundOn) {
		v1 = 0;
		v2 = 0;
		while (_strcmpi(sgSFX[v2].pszName, snd_file) || !sgSFX[v2].pSnd) {
			++v2;
			++v1;
			if (v2 >= NUM_SFX)
				return;
		}
		v3 = &sgSFX[v1].pSnd;
		//_LOBYTE(v4) = snd_playing(*v3);
		if (!snd_playing(*v3))
			snd_play_snd(*v3, 0, 0);
	}
}
// 4A22D5: using guessed type char gbSoundOn;
