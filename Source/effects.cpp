/**
 * @file effects.cpp
 *
 * Implementation of functions for loading and playing sounds.
 */
#include "effects.h"

#include <algorithm>
#include <cstdint>

#include "engine/random.hpp"
#include "engine/sound.h"
#include "engine/sound_defs.hpp"
#include "engine/sound_position.hpp"
#include "init.h"
#include "player.h"
#include "utils/str_cat.hpp"

namespace devilution {

int sfxdelay;
SfxID sfxdnum = SfxID::None;

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
// SfxID                   bFlags                     pszName                        pSnd
/*Walk*/                 { sfx_MISC,                  "sfx\\misc\\walk1.wav",        nullptr },
/*ShootBow*/             { sfx_MISC,                  "sfx\\misc\\bfire.wav",        nullptr },
/*CastSpell*/            { sfx_MISC,                  "sfx\\misc\\tmag.wav",         nullptr },
/*Swing*/                { sfx_MISC,                  "sfx\\misc\\swing.wav",        nullptr },
/*Swing2*/               { sfx_MISC,                  "sfx\\misc\\swing2.wav",       nullptr },
/*WarriorDeath*/         { sfx_MISC,                  "sfx\\misc\\dead.wav",         nullptr },
/*ShootBow2*/            { sfx_MISC | sfx_HELLFIRE,   "sfx\\misc\\sting1.wav",       nullptr },
/*ShootFireballBow*/     { sfx_MISC | sfx_HELLFIRE,   "sfx\\misc\\fballbow.wav",     nullptr },
/*QuestDone*/            { sfx_STREAM,                "sfx\\misc\\questdon.wav",     nullptr },
/*BarrelExpload*/        { sfx_MISC,                  "sfx\\items\\barlfire.wav",    nullptr },
/*BarrelBreak*/          { sfx_MISC,                  "sfx\\items\\barrel.wav",      nullptr },
/*PodExpload*/           { sfx_MISC | sfx_HELLFIRE,   "sfx\\items\\podpop8.wav",     nullptr },
/*PodPop*/               { sfx_MISC | sfx_HELLFIRE,   "sfx\\items\\podpop5.wav",     nullptr },
/*UrnExpload*/           { sfx_MISC | sfx_HELLFIRE,   "sfx\\items\\urnpop3.wav",     nullptr },
/*UrnBreak*/             { sfx_MISC | sfx_HELLFIRE,   "sfx\\items\\urnpop2.wav",     nullptr },
/*BrutalHit*/            { sfx_MISC,                  "sfx\\items\\bhit.wav",        nullptr },
/*BrutalHit1*/           { sfx_MISC,                  "sfx\\items\\bhit1.wav",       nullptr },
/*ChestOpen*/            { sfx_MISC,                  "sfx\\items\\chest.wav",       nullptr },
/*DoorClose*/            { sfx_MISC,                  "sfx\\items\\doorclos.wav",    nullptr },
/*DoorOpen*/             { sfx_MISC,                  "sfx\\items\\dooropen.wav",    nullptr },
/*ItemAnvilFlip*/        { sfx_MISC,                  "sfx\\items\\flipanvl.wav",    nullptr },
/*ItemAxeFlip*/          { sfx_MISC,                  "sfx\\items\\flipaxe.wav",     nullptr },
/*ItemBloodStoneFlip*/   { sfx_MISC,                  "sfx\\items\\flipblst.wav",    nullptr },
/*ItemBodyPartFlip*/     { sfx_MISC,                  "sfx\\items\\flipbody.wav",    nullptr },
/*ItemBookFlip*/         { sfx_MISC,                  "sfx\\items\\flipbook.wav",    nullptr },
/*ItemBowFlip*/          { sfx_MISC,                  "sfx\\items\\flipbow.wav",     nullptr },
/*ItemCapFlip*/          { sfx_MISC,                  "sfx\\items\\flipcap.wav",     nullptr },
/*ItemArmorFlip*/        { sfx_MISC,                  "sfx\\items\\flipharm.wav",    nullptr },
/*ItemLeatherFlip*/      { sfx_MISC,                  "sfx\\items\\fliplarm.wav",    nullptr },
/*ItemMushroomFlip*/     { sfx_MISC,                  "sfx\\items\\flipmush.wav",    nullptr },
/*ItemPotionFlip*/       { sfx_MISC,                  "sfx\\items\\flippot.wav",     nullptr },
/*ItemRingFlip*/         { sfx_MISC,                  "sfx\\items\\flipring.wav",    nullptr },
/*ItemRockFlip*/         { sfx_MISC,                  "sfx\\items\\fliprock.wav",    nullptr },
/*ItemScrollFlip*/       { sfx_MISC,                  "sfx\\items\\flipscrl.wav",    nullptr },
/*ItemShieldFlip*/       { sfx_MISC,                  "sfx\\items\\flipshld.wav",    nullptr },
/*ItemSignFlip*/         { sfx_MISC,                  "sfx\\items\\flipsign.wav",    nullptr },
/*ItemStaffFlip*/        { sfx_MISC,                  "sfx\\items\\flipstaf.wav",    nullptr },
/*ItemSwordFlip*/        { sfx_MISC,                  "sfx\\items\\flipswor.wav",    nullptr },
/*ItemGold*/             { sfx_MISC,                  "sfx\\items\\gold.wav",        nullptr },
/*ItemAnvil*/            { sfx_MISC,                  "sfx\\items\\invanvl.wav",     nullptr },
/*ItemAxe*/              { sfx_MISC,                  "sfx\\items\\invaxe.wav",      nullptr },
/*ItemBloodStone*/       { sfx_MISC,                  "sfx\\items\\invblst.wav",     nullptr },
/*ItemBodyPart*/         { sfx_MISC,                  "sfx\\items\\invbody.wav",     nullptr },
/*ItemBook*/             { sfx_MISC,                  "sfx\\items\\invbook.wav",     nullptr },
/*ItemBow*/              { sfx_MISC,                  "sfx\\items\\invbow.wav",      nullptr },
/*ItemCap*/              { sfx_MISC,                  "sfx\\items\\invcap.wav",      nullptr },
/*GrabItem*/             { sfx_MISC,                  "sfx\\items\\invgrab.wav",     nullptr },
/*ItemArmor*/            { sfx_MISC,                  "sfx\\items\\invharm.wav",     nullptr },
/*ItemLeather*/          { sfx_MISC,                  "sfx\\items\\invlarm.wav",     nullptr },
/*ItemMushroom*/         { sfx_MISC,                  "sfx\\items\\invmush.wav",     nullptr },
/*ItemPotion*/           { sfx_MISC,                  "sfx\\items\\invpot.wav",      nullptr },
/*ItemRing*/             { sfx_MISC,                  "sfx\\items\\invring.wav",     nullptr },
/*ItemRock*/             { sfx_MISC,                  "sfx\\items\\invrock.wav",     nullptr },
/*ItemScroll*/           { sfx_MISC,                  "sfx\\items\\invscrol.wav",    nullptr },
/*ItemShield*/           { sfx_MISC,                  "sfx\\items\\invshiel.wav",    nullptr },
/*ItemSign*/             { sfx_MISC,                  "sfx\\items\\invsign.wav",     nullptr },
/*ItemStaff*/            { sfx_MISC,                  "sfx\\items\\invstaf.wav",     nullptr },
/*ItemSword*/            { sfx_MISC,                  "sfx\\items\\invsword.wav",    nullptr },
/*OperateLever*/         { sfx_MISC,                  "sfx\\items\\lever.wav",       nullptr },
/*OperateShrine*/        { sfx_MISC,                  "sfx\\items\\magic.wav",       nullptr },
/*OperateShrine1*/       { sfx_MISC,                  "sfx\\items\\magic1.wav",      nullptr },
/*ReadBook*/             { sfx_MISC,                  "sfx\\items\\readbook.wav",    nullptr },
/*Sarcophagus*/          { sfx_MISC,                  "sfx\\items\\sarc.wav",        nullptr },
/*MenuMove*/             { sfx_UI,                    "sfx\\items\\titlemov.wav",    nullptr },
/*MenuSelect*/           { sfx_UI,                    "sfx\\items\\titlslct.wav",    nullptr },
/*TriggerTrap*/          { sfx_MISC,                  "sfx\\items\\trap.wav",        nullptr },
/*CastFire*/             { sfx_MISC,                  "sfx\\misc\\cast2.wav",        nullptr },
/*CastLightning*/        { sfx_MISC,                  "sfx\\misc\\cast4.wav",        nullptr },
/*CastSkill*/            { sfx_MISC,                  "sfx\\misc\\cast6.wav",        nullptr },
/*SpellEnd*/             { sfx_MISC,                  "sfx\\misc\\cast7.wav",        nullptr },
/*CastHealing*/          { sfx_MISC,                  "sfx\\misc\\cast8.wav",        nullptr },
/*SpellRepair*/          { sfx_MISC,                  "sfx\\misc\\repair.wav",       nullptr },
/*SpellAcid*/            { sfx_MISC,                  "sfx\\misc\\acids1.wav",       nullptr },
/*SpellAcid1*/           { sfx_MISC,                  "sfx\\misc\\acids2.wav",       nullptr },
/*SpellApocalypse*/      { sfx_MISC,                  "sfx\\misc\\apoc.wav",         nullptr },
/*SpellBloodStar*/       { sfx_MISC,                  "sfx\\misc\\blodstar.wav",     nullptr },
/*SpellBloodStarHit*/    { sfx_MISC,                  "sfx\\misc\\blsimpt.wav",      nullptr },
/*SpellBoneSpirit*/      { sfx_MISC,                  "sfx\\misc\\bonesp.wav",       nullptr },
/*SpellBoneSpiritHit*/   { sfx_MISC,                  "sfx\\misc\\bsimpct.wav",      nullptr },
/*OperateCaldron*/       { sfx_MISC,                  "sfx\\misc\\caldron.wav",      nullptr },
/*SpellChargedBolt*/     { sfx_MISC,                  "sfx\\misc\\cbolt.wav",        nullptr },
/*SpellDoomSerpents*/    { sfx_MISC,                  "sfx\\misc\\dserp.wav",        nullptr },
/*SpellLightningHit*/    { sfx_MISC,                  "sfx\\misc\\elecimp1.wav",     nullptr },
/*SpellElemental*/       { sfx_MISC,                  "sfx\\misc\\elementl.wav",     nullptr },
/*SpellEtherealize*/     { sfx_MISC,                  "sfx\\misc\\ethereal.wav",     nullptr },
/*SpellFirebolt*/        { sfx_MISC,                  "sfx\\misc\\fbolt1.wav",       nullptr },
/*SpellFireHit*/         { sfx_MISC,                  "sfx\\misc\\firimp2.wav",      nullptr },
/*SpellFlameWave*/       { sfx_MISC,                  "sfx\\misc\\flamwave.wav",     nullptr },
/*OperateFountain*/      { sfx_MISC,                  "sfx\\misc\\fountain.wav",     nullptr },
/*SpellGolem*/           { sfx_MISC,                  "sfx\\misc\\golum.wav",        nullptr },
/*OperateGoatShrine*/    { sfx_MISC,                  "sfx\\misc\\gshrine.wav",      nullptr },
/*SpellGuardian*/        { sfx_MISC,                  "sfx\\misc\\guard.wav",        nullptr },
/*SpellGuardianHit*/     { sfx_MISC,                  "sfx\\misc\\grdlanch.wav",     nullptr },
/*SpellHolyBolt*/        { sfx_MISC,                  "sfx\\misc\\holybolt.wav",     nullptr },
/*SpellInfravision*/     { sfx_MISC,                  "sfx\\misc\\infravis.wav",     nullptr },
/*SpellInvisibility*/    { sfx_MISC,                  "sfx\\misc\\invisibl.wav",     nullptr },
/*SpellLightning*/       { sfx_MISC,                  "sfx\\misc\\lning1.wav",       nullptr },
/*SpellManaShield*/      { sfx_MISC,                  "sfx\\misc\\mshield.wav",      nullptr },
/*BigExplosion*/         { sfx_MISC | sfx_HELLFIRE,   "sfx\\misc\\nestxpld.wav",     nullptr },
/*SpellNova*/            { sfx_MISC,                  "sfx\\misc\\nova.wav",         nullptr },
/*SpellPuddle*/          { sfx_MISC,                  "sfx\\misc\\puddle.wav",       nullptr },
/*SpellResurrect*/       { sfx_MISC,                  "sfx\\misc\\resur.wav",        nullptr },
/*SpellStoneCurse*/      { sfx_MISC,                  "sfx\\misc\\scurimp.wav",      nullptr },
/*SpellPortal*/          { sfx_MISC,                  "sfx\\misc\\sentinel.wav",     nullptr },
/*SpellInferno*/         { sfx_MISC,                  "sfx\\misc\\spoutstr.wav",     nullptr },
/*SpellTrapDisarm*/      { sfx_MISC,                  "sfx\\misc\\trapdis.wav",      nullptr },
/*SpellTeleport*/        { sfx_MISC,                  "sfx\\misc\\teleport.wav",     nullptr },
/*SpellFireWall*/        { sfx_MISC,                  "sfx\\misc\\wallloop.wav",     nullptr },
/*SpellLightningWall*/   { sfx_MISC | sfx_HELLFIRE,   "sfx\\misc\\lmag.wav",         nullptr },
/*Gillian1*/             { sfx_STREAM,                "sfx\\towners\\bmaid01.wav",   nullptr },
/*Gillian2*/             { sfx_STREAM,                "sfx\\towners\\bmaid02.wav",   nullptr },
/*Gillian3*/             { sfx_STREAM,                "sfx\\towners\\bmaid03.wav",   nullptr },
/*Gillian4*/             { sfx_STREAM,                "sfx\\towners\\bmaid04.wav",   nullptr },
/*Gillian5*/             { sfx_STREAM,                "sfx\\towners\\bmaid05.wav",   nullptr },
/*Gillian6*/             { sfx_STREAM,                "sfx\\towners\\bmaid06.wav",   nullptr },
/*Gillian7*/             { sfx_STREAM,                "sfx\\towners\\bmaid07.wav",   nullptr },
/*Gillian8*/             { sfx_STREAM,                "sfx\\towners\\bmaid08.wav",   nullptr },
/*Gillian9*/             { sfx_STREAM,                "sfx\\towners\\bmaid09.wav",   nullptr },
/*Gillian10*/            { sfx_STREAM,                "sfx\\towners\\bmaid10.wav",   nullptr },
/*Gillian11*/            { sfx_STREAM,                "sfx\\towners\\bmaid11.wav",   nullptr },
/*Gillian12*/            { sfx_STREAM,                "sfx\\towners\\bmaid12.wav",   nullptr },
/*Gillian13*/            { sfx_STREAM,                "sfx\\towners\\bmaid13.wav",   nullptr },
/*Gillian14*/            { sfx_STREAM,                "sfx\\towners\\bmaid14.wav",   nullptr },
/*Gillian15*/            { sfx_STREAM,                "sfx\\towners\\bmaid15.wav",   nullptr },
/*Gillian16*/            { sfx_STREAM,                "sfx\\towners\\bmaid16.wav",   nullptr },
/*Gillian17*/            { sfx_STREAM,                "sfx\\towners\\bmaid17.wav",   nullptr },
/*Gillian18*/            { sfx_STREAM,                "sfx\\towners\\bmaid18.wav",   nullptr },
/*Gillian19*/            { sfx_STREAM,                "sfx\\towners\\bmaid19.wav",   nullptr },
/*Gillian20*/            { sfx_STREAM,                "sfx\\towners\\bmaid20.wav",   nullptr },
/*Gillian21*/            { sfx_STREAM,                "sfx\\towners\\bmaid21.wav",   nullptr },
/*Gillian22*/            { sfx_STREAM,                "sfx\\towners\\bmaid22.wav",   nullptr },
/*Gillian23*/            { sfx_STREAM,                "sfx\\towners\\bmaid23.wav",   nullptr },
/*Gillian24*/            { sfx_STREAM,                "sfx\\towners\\bmaid24.wav",   nullptr },
/*Gillian25*/            { sfx_STREAM,                "sfx\\towners\\bmaid25.wav",   nullptr },
/*Gillian26*/            { sfx_STREAM,                "sfx\\towners\\bmaid26.wav",   nullptr },
/*Gillian27*/            { sfx_STREAM,                "sfx\\towners\\bmaid27.wav",   nullptr },
/*Gillian28*/            { sfx_STREAM,                "sfx\\towners\\bmaid28.wav",   nullptr },
/*Gillian29*/            { sfx_STREAM,                "sfx\\towners\\bmaid29.wav",   nullptr },
/*Gillian30*/            { sfx_STREAM,                "sfx\\towners\\bmaid30.wav",   nullptr },
/*Gillian31*/            { sfx_STREAM,                "sfx\\towners\\bmaid31.wav",   nullptr },
/*Gillian32*/            { sfx_STREAM,                "sfx\\towners\\bmaid32.wav",   nullptr },
/*Gillian33*/            { sfx_STREAM,                "sfx\\towners\\bmaid33.wav",   nullptr },
/*Gillian34*/            { sfx_STREAM,                "sfx\\towners\\bmaid34.wav",   nullptr },
/*Gillian35*/            { sfx_STREAM,                "sfx\\towners\\bmaid35.wav",   nullptr },
/*Gillian36*/            { sfx_STREAM,                "sfx\\towners\\bmaid36.wav",   nullptr },
/*Gillian37*/            { sfx_STREAM,                "sfx\\towners\\bmaid37.wav",   nullptr },
/*Gillian38*/            { sfx_STREAM,                "sfx\\towners\\bmaid38.wav",   nullptr },
/*Gillian39*/            { sfx_STREAM,                "sfx\\towners\\bmaid39.wav",   nullptr },
/*Gillian40*/            { sfx_STREAM,                "sfx\\towners\\bmaid40.wav",   nullptr },
/*Griswold1*/            { sfx_STREAM,                "sfx\\towners\\bsmith01.wav",  nullptr },
/*Griswold2*/            { sfx_STREAM,                "sfx\\towners\\bsmith02.wav",  nullptr },
/*Griswold3*/            { sfx_STREAM,                "sfx\\towners\\bsmith03.wav",  nullptr },
/*Griswold4*/            { sfx_STREAM,                "sfx\\towners\\bsmith04.wav",  nullptr },
/*Griswold5*/            { sfx_STREAM,                "sfx\\towners\\bsmith05.wav",  nullptr },
/*Griswold6*/            { sfx_STREAM,                "sfx\\towners\\bsmith06.wav",  nullptr },
/*Griswold7*/            { sfx_STREAM,                "sfx\\towners\\bsmith07.wav",  nullptr },
/*Griswold8*/            { sfx_STREAM,                "sfx\\towners\\bsmith08.wav",  nullptr },
/*Griswold9*/            { sfx_STREAM,                "sfx\\towners\\bsmith09.wav",  nullptr },
/*Griswold10*/           { sfx_STREAM,                "sfx\\towners\\bsmith10.wav",  nullptr },
/*Griswold11*/           { sfx_STREAM,                "sfx\\towners\\bsmith11.wav",  nullptr },
/*Griswold12*/           { sfx_STREAM,                "sfx\\towners\\bsmith12.wav",  nullptr },
/*Griswold13*/           { sfx_STREAM,                "sfx\\towners\\bsmith13.wav",  nullptr },
/*Griswold14*/           { sfx_STREAM,                "sfx\\towners\\bsmith14.wav",  nullptr },
/*Griswold15*/           { sfx_STREAM,                "sfx\\towners\\bsmith15.wav",  nullptr },
/*Griswold16*/           { sfx_STREAM,                "sfx\\towners\\bsmith16.wav",  nullptr },
/*Griswold17*/           { sfx_STREAM,                "sfx\\towners\\bsmith17.wav",  nullptr },
/*Griswold18*/           { sfx_STREAM,                "sfx\\towners\\bsmith18.wav",  nullptr },
/*Griswold19*/           { sfx_STREAM,                "sfx\\towners\\bsmith19.wav",  nullptr },
/*Griswold20*/           { sfx_STREAM,                "sfx\\towners\\bsmith20.wav",  nullptr },
/*Griswold21*/           { sfx_STREAM,                "sfx\\towners\\bsmith21.wav",  nullptr },
/*Griswold22*/           { sfx_STREAM,                "sfx\\towners\\bsmith22.wav",  nullptr },
/*Griswold23*/           { sfx_STREAM,                "sfx\\towners\\bsmith23.wav",  nullptr },
/*Griswold24*/           { sfx_STREAM,                "sfx\\towners\\bsmith24.wav",  nullptr },
/*Griswold25*/           { sfx_STREAM,                "sfx\\towners\\bsmith25.wav",  nullptr },
/*Griswold26*/           { sfx_STREAM,                "sfx\\towners\\bsmith26.wav",  nullptr },
/*Griswold27*/           { sfx_STREAM,                "sfx\\towners\\bsmith27.wav",  nullptr },
/*Griswold28*/           { sfx_STREAM,                "sfx\\towners\\bsmith28.wav",  nullptr },
/*Griswold29*/           { sfx_STREAM,                "sfx\\towners\\bsmith29.wav",  nullptr },
/*Griswold30*/           { sfx_STREAM,                "sfx\\towners\\bsmith30.wav",  nullptr },
/*Griswold31*/           { sfx_STREAM,                "sfx\\towners\\bsmith31.wav",  nullptr },
/*Griswold32*/           { sfx_STREAM,                "sfx\\towners\\bsmith32.wav",  nullptr },
/*Griswold33*/           { sfx_STREAM,                "sfx\\towners\\bsmith33.wav",  nullptr },
/*Griswold34*/           { sfx_STREAM,                "sfx\\towners\\bsmith34.wav",  nullptr },
/*Griswold35*/           { sfx_STREAM,                "sfx\\towners\\bsmith35.wav",  nullptr },
/*Griswold36*/           { sfx_STREAM,                "sfx\\towners\\bsmith36.wav",  nullptr },
/*Griswold37*/           { sfx_STREAM,                "sfx\\towners\\bsmith37.wav",  nullptr },
/*Griswold38*/           { sfx_STREAM,                "sfx\\towners\\bsmith38.wav",  nullptr },
/*Griswold39*/           { sfx_STREAM,                "sfx\\towners\\bsmith39.wav",  nullptr },
/*Griswold40*/           { sfx_STREAM,                "sfx\\towners\\bsmith40.wav",  nullptr },
/*Griswold41*/           { sfx_STREAM,                "sfx\\towners\\bsmith41.wav",  nullptr },
/*Griswold42*/           { sfx_STREAM,                "sfx\\towners\\bsmith42.wav",  nullptr },
/*Griswold43*/           { sfx_STREAM,                "sfx\\towners\\bsmith43.wav",  nullptr },
/*Griswold44*/           { sfx_STREAM,                "sfx\\towners\\bsmith44.wav",  nullptr },
/*Griswold45*/           { sfx_STREAM,                "sfx\\towners\\bsmith45.wav",  nullptr },
/*Griswold46*/           { sfx_STREAM,                "sfx\\towners\\bsmith46.wav",  nullptr },
/*Griswold47*/           { sfx_STREAM,                "sfx\\towners\\bsmith47.wav",  nullptr },
/*Griswold48*/           { sfx_STREAM,                "sfx\\towners\\bsmith48.wav",  nullptr },
/*Griswold49*/           { sfx_STREAM,                "sfx\\towners\\bsmith49.wav",  nullptr },
/*Griswold50*/           { sfx_STREAM,                "sfx\\towners\\bsmith50.wav",  nullptr },
/*Griswold51*/           { sfx_STREAM,                "sfx\\towners\\bsmith51.wav",  nullptr },
/*Griswold52*/           { sfx_STREAM,                "sfx\\towners\\bsmith52.wav",  nullptr },
/*Griswold53*/           { sfx_STREAM,                "sfx\\towners\\bsmith53.wav",  nullptr },
/*Griswold54*/           { sfx_STREAM,                "sfx\\towners\\bsmith54.wav",  nullptr },
/*Griswold55*/           { sfx_STREAM,                "sfx\\towners\\bsmith55.wav",  nullptr },
/*Griswold56*/           { sfx_STREAM,                "sfx\\towners\\bsmith56.wav",  nullptr },
/*Cow1*/                 { sfx_MISC,                  "sfx\\towners\\cow1.wav",      nullptr },
/*Cow2*/                 { sfx_MISC,                  "sfx\\towners\\cow2.wav",      nullptr },
/*Pig*/                  { sfx_MISC | sfx_HELLFIRE,   "sfx\\towners\\cow7.wav",      nullptr },
/*Duck*/                 { sfx_MISC | sfx_HELLFIRE,   "sfx\\towners\\cow8.wav",      nullptr },
/*WoundedTownsmanOld*/   { sfx_STREAM,                "sfx\\towners\\deadguy2.wav",  nullptr },
/*Farnham1*/             { sfx_STREAM,                "sfx\\towners\\drunk01.wav",   nullptr },
/*Farnham2*/             { sfx_STREAM,                "sfx\\towners\\drunk02.wav",   nullptr },
/*Farnham3*/             { sfx_STREAM,                "sfx\\towners\\drunk03.wav",   nullptr },
/*Farnham4*/             { sfx_STREAM,                "sfx\\towners\\drunk04.wav",   nullptr },
/*Farnham5*/             { sfx_STREAM,                "sfx\\towners\\drunk05.wav",   nullptr },
/*Farnham6*/             { sfx_STREAM,                "sfx\\towners\\drunk06.wav",   nullptr },
/*Farnham7*/             { sfx_STREAM,                "sfx\\towners\\drunk07.wav",   nullptr },
/*Farnham8*/             { sfx_STREAM,                "sfx\\towners\\drunk08.wav",   nullptr },
/*Farnham9*/             { sfx_STREAM,                "sfx\\towners\\drunk09.wav",   nullptr },
/*Farnham10*/            { sfx_STREAM,                "sfx\\towners\\drunk10.wav",   nullptr },
/*Farnham11*/            { sfx_STREAM,                "sfx\\towners\\drunk11.wav",   nullptr },
/*Farnham12*/            { sfx_STREAM,                "sfx\\towners\\drunk12.wav",   nullptr },
/*Farnham13*/            { sfx_STREAM,                "sfx\\towners\\drunk13.wav",   nullptr },
/*Farnham14*/            { sfx_STREAM,                "sfx\\towners\\drunk14.wav",   nullptr },
/*Farnham15*/            { sfx_STREAM,                "sfx\\towners\\drunk15.wav",   nullptr },
/*Farnham16*/            { sfx_STREAM,                "sfx\\towners\\drunk16.wav",   nullptr },
/*Farnham17*/            { sfx_STREAM,                "sfx\\towners\\drunk17.wav",   nullptr },
/*Farnham18*/            { sfx_STREAM,                "sfx\\towners\\drunk18.wav",   nullptr },
/*Farnham19*/            { sfx_STREAM,                "sfx\\towners\\drunk19.wav",   nullptr },
/*Farnham20*/            { sfx_STREAM,                "sfx\\towners\\drunk20.wav",   nullptr },
/*Farnham21*/            { sfx_STREAM,                "sfx\\towners\\drunk21.wav",   nullptr },
/*Farnham22*/            { sfx_STREAM,                "sfx\\towners\\drunk22.wav",   nullptr },
/*Farnham23*/            { sfx_STREAM,                "sfx\\towners\\drunk23.wav",   nullptr },
/*Farnham24*/            { sfx_STREAM,                "sfx\\towners\\drunk24.wav",   nullptr },
/*Farnham25*/            { sfx_STREAM,                "sfx\\towners\\drunk25.wav",   nullptr },
/*Farnham26*/            { sfx_STREAM,                "sfx\\towners\\drunk26.wav",   nullptr },
/*Farnham27*/            { sfx_STREAM,                "sfx\\towners\\drunk27.wav",   nullptr },
/*Farnham28*/            { sfx_STREAM,                "sfx\\towners\\drunk28.wav",   nullptr },
/*Farnham29*/            { sfx_STREAM,                "sfx\\towners\\drunk29.wav",   nullptr },
/*Farnham30*/            { sfx_STREAM,                "sfx\\towners\\drunk30.wav",   nullptr },
/*Farnham31*/            { sfx_STREAM,                "sfx\\towners\\drunk31.wav",   nullptr },
/*Farnham32*/            { sfx_STREAM,                "sfx\\towners\\drunk32.wav",   nullptr },
/*Farnham33*/            { sfx_STREAM,                "sfx\\towners\\drunk33.wav",   nullptr },
/*Farnham34*/            { sfx_STREAM,                "sfx\\towners\\drunk34.wav",   nullptr },
/*Farnham35*/            { sfx_STREAM,                "sfx\\towners\\drunk35.wav",   nullptr },
/*Pepin1*/               { sfx_STREAM,                "sfx\\towners\\healer01.wav",  nullptr },
/*Pepin2*/               { sfx_STREAM,                "sfx\\towners\\healer02.wav",  nullptr },
/*Pepin3*/               { sfx_STREAM,                "sfx\\towners\\healer03.wav",  nullptr },
/*Pepin4*/               { sfx_STREAM,                "sfx\\towners\\healer04.wav",  nullptr },
/*Pepin5*/               { sfx_STREAM,                "sfx\\towners\\healer05.wav",  nullptr },
/*Pepin6*/               { sfx_STREAM,                "sfx\\towners\\healer06.wav",  nullptr },
/*Pepin7*/               { sfx_STREAM,                "sfx\\towners\\healer07.wav",  nullptr },
/*Pepin8*/               { sfx_STREAM,                "sfx\\towners\\healer08.wav",  nullptr },
/*Pepin9*/               { sfx_STREAM,                "sfx\\towners\\healer09.wav",  nullptr },
/*Pepin10*/              { sfx_STREAM,                "sfx\\towners\\healer10.wav",  nullptr },
/*Pepin11*/              { sfx_STREAM,                "sfx\\towners\\healer11.wav",  nullptr },
/*Pepin12*/              { sfx_STREAM,                "sfx\\towners\\healer12.wav",  nullptr },
/*Pepin13*/              { sfx_STREAM,                "sfx\\towners\\healer13.wav",  nullptr },
/*Pepin14*/              { sfx_STREAM,                "sfx\\towners\\healer14.wav",  nullptr },
/*Pepin15*/              { sfx_STREAM,                "sfx\\towners\\healer15.wav",  nullptr },
/*Pepin16*/              { sfx_STREAM,                "sfx\\towners\\healer16.wav",  nullptr },
/*Pepin17*/              { sfx_STREAM,                "sfx\\towners\\healer17.wav",  nullptr },
/*Pepin18*/              { sfx_STREAM,                "sfx\\towners\\healer18.wav",  nullptr },
/*Pepin19*/              { sfx_STREAM,                "sfx\\towners\\healer19.wav",  nullptr },
/*Pepin20*/              { sfx_STREAM,                "sfx\\towners\\healer20.wav",  nullptr },
/*Pepin21*/              { sfx_STREAM,                "sfx\\towners\\healer21.wav",  nullptr },
/*Pepin22*/              { sfx_STREAM,                "sfx\\towners\\healer22.wav",  nullptr },
/*Pepin23*/              { sfx_STREAM,                "sfx\\towners\\healer23.wav",  nullptr },
/*Pepin24*/              { sfx_STREAM,                "sfx\\towners\\healer24.wav",  nullptr },
/*Pepin25*/              { sfx_STREAM,                "sfx\\towners\\healer25.wav",  nullptr },
/*Pepin26*/              { sfx_STREAM,                "sfx\\towners\\healer26.wav",  nullptr },
/*Pepin27*/              { sfx_STREAM,                "sfx\\towners\\healer27.wav",  nullptr },
/*Pepin28*/              { sfx_STREAM,                "sfx\\towners\\healer28.wav",  nullptr },
/*Pepin29*/              { sfx_STREAM,                "sfx\\towners\\healer29.wav",  nullptr },
/*Pepin30*/              { sfx_STREAM,                "sfx\\towners\\healer30.wav",  nullptr },
/*Pepin31*/              { sfx_STREAM,                "sfx\\towners\\healer31.wav",  nullptr },
/*Pepin32*/              { sfx_STREAM,                "sfx\\towners\\healer32.wav",  nullptr },
/*Pepin33*/              { sfx_STREAM,                "sfx\\towners\\healer33.wav",  nullptr },
/*Pepin34*/              { sfx_STREAM,                "sfx\\towners\\healer34.wav",  nullptr },
/*Pepin35*/              { sfx_STREAM,                "sfx\\towners\\healer35.wav",  nullptr },
/*Pepin36*/              { sfx_STREAM,                "sfx\\towners\\healer36.wav",  nullptr },
/*Pepin37*/              { sfx_STREAM,                "sfx\\towners\\healer37.wav",  nullptr },
/*Pepin38*/              { sfx_STREAM,                "sfx\\towners\\healer38.wav",  nullptr },
/*Pepin39*/              { sfx_STREAM,                "sfx\\towners\\healer39.wav",  nullptr },
/*Pepin40*/              { sfx_STREAM,                "sfx\\towners\\healer40.wav",  nullptr },
/*Pepin41*/              { sfx_STREAM,                "sfx\\towners\\healer41.wav",  nullptr },
/*Pepin42*/              { sfx_STREAM,                "sfx\\towners\\healer42.wav",  nullptr },
/*Pepin43*/              { sfx_STREAM,                "sfx\\towners\\healer43.wav",  nullptr },
/*Pepin44*/              { sfx_STREAM,                "sfx\\towners\\healer44.wav",  nullptr },
/*Pepin45*/              { sfx_STREAM,                "sfx\\towners\\healer45.wav",  nullptr },
/*Pepin46*/              { sfx_STREAM,                "sfx\\towners\\healer46.wav",  nullptr },
/*Pepin47*/              { sfx_STREAM,                "sfx\\towners\\healer47.wav",  nullptr },
/*Wirt1*/                { sfx_STREAM,                "sfx\\towners\\pegboy01.wav",  nullptr },
/*Wirt2*/                { sfx_STREAM,                "sfx\\towners\\pegboy02.wav",  nullptr },
/*Wirt3*/                { sfx_STREAM,                "sfx\\towners\\pegboy03.wav",  nullptr },
/*Wirt4*/                { sfx_STREAM,                "sfx\\towners\\pegboy04.wav",  nullptr },
/*Wirt5*/                { sfx_STREAM,                "sfx\\towners\\pegboy05.wav",  nullptr },
/*Wirt6*/                { sfx_STREAM,                "sfx\\towners\\pegboy06.wav",  nullptr },
/*Wirt7*/                { sfx_STREAM,                "sfx\\towners\\pegboy07.wav",  nullptr },
/*Wirt8*/                { sfx_STREAM,                "sfx\\towners\\pegboy08.wav",  nullptr },
/*Wirt9*/                { sfx_STREAM,                "sfx\\towners\\pegboy09.wav",  nullptr },
/*Wirt10*/               { sfx_STREAM,                "sfx\\towners\\pegboy10.wav",  nullptr },
/*Wirt11*/               { sfx_STREAM,                "sfx\\towners\\pegboy11.wav",  nullptr },
/*Wirt12*/               { sfx_STREAM,                "sfx\\towners\\pegboy12.wav",  nullptr },
/*Wirt13*/               { sfx_STREAM,                "sfx\\towners\\pegboy13.wav",  nullptr },
/*Wirt14*/               { sfx_STREAM,                "sfx\\towners\\pegboy14.wav",  nullptr },
/*Wirt15*/               { sfx_STREAM,                "sfx\\towners\\pegboy15.wav",  nullptr },
/*Wirt16*/               { sfx_STREAM,                "sfx\\towners\\pegboy16.wav",  nullptr },
/*Wirt17*/               { sfx_STREAM,                "sfx\\towners\\pegboy17.wav",  nullptr },
/*Wirt18*/               { sfx_STREAM,                "sfx\\towners\\pegboy18.wav",  nullptr },
/*Wirt19*/               { sfx_STREAM,                "sfx\\towners\\pegboy19.wav",  nullptr },
/*Wirt20*/               { sfx_STREAM,                "sfx\\towners\\pegboy20.wav",  nullptr },
/*Wirt21*/               { sfx_STREAM,                "sfx\\towners\\pegboy21.wav",  nullptr },
/*Wirt22*/               { sfx_STREAM,                "sfx\\towners\\pegboy22.wav",  nullptr },
/*Wirt23*/               { sfx_STREAM,                "sfx\\towners\\pegboy23.wav",  nullptr },
/*Wirt24*/               { sfx_STREAM,                "sfx\\towners\\pegboy24.wav",  nullptr },
/*Wirt25*/               { sfx_STREAM,                "sfx\\towners\\pegboy25.wav",  nullptr },
/*Wirt26*/               { sfx_STREAM,                "sfx\\towners\\pegboy26.wav",  nullptr },
/*Wirt27*/               { sfx_STREAM,                "sfx\\towners\\pegboy27.wav",  nullptr },
/*Wirt28*/               { sfx_STREAM,                "sfx\\towners\\pegboy28.wav",  nullptr },
/*Wirt29*/               { sfx_STREAM,                "sfx\\towners\\pegboy29.wav",  nullptr },
/*Wirt30*/               { sfx_STREAM,                "sfx\\towners\\pegboy30.wav",  nullptr },
/*Wirt31*/               { sfx_STREAM,                "sfx\\towners\\pegboy31.wav",  nullptr },
/*Wirt32*/               { sfx_STREAM,                "sfx\\towners\\pegboy32.wav",  nullptr },
/*Wirt33*/               { sfx_STREAM,                "sfx\\towners\\pegboy33.wav",  nullptr },
/*Wirt34*/               { sfx_STREAM,                "sfx\\towners\\pegboy34.wav",  nullptr },
/*Wirt35*/               { sfx_STREAM,                "sfx\\towners\\pegboy35.wav",  nullptr },
/*Wirt36*/               { sfx_STREAM,                "sfx\\towners\\pegboy36.wav",  nullptr },
/*Wirt37*/               { sfx_STREAM,                "sfx\\towners\\pegboy37.wav",  nullptr },
/*Wirt38*/               { sfx_STREAM,                "sfx\\towners\\pegboy38.wav",  nullptr },
/*Wirt39*/               { sfx_STREAM,                "sfx\\towners\\pegboy39.wav",  nullptr },
/*Wirt40*/               { sfx_STREAM,                "sfx\\towners\\pegboy40.wav",  nullptr },
/*Wirt41*/               { sfx_STREAM,                "sfx\\towners\\pegboy41.wav",  nullptr },
/*Wirt42*/               { sfx_STREAM,                "sfx\\towners\\pegboy42.wav",  nullptr },
/*Wirt43*/               { sfx_STREAM,                "sfx\\towners\\pegboy43.wav",  nullptr },
/*Tremain0*/             { sfx_STREAM,                "sfx\\towners\\priest00.wav",  nullptr },
/*Tremain1*/             { sfx_STREAM,                "sfx\\towners\\priest01.wav",  nullptr },
/*Tremain2*/             { sfx_STREAM,                "sfx\\towners\\priest02.wav",  nullptr },
/*Tremain3*/             { sfx_STREAM,                "sfx\\towners\\priest03.wav",  nullptr },
/*Tremain4*/             { sfx_STREAM,                "sfx\\towners\\priest04.wav",  nullptr },
/*Tremain5*/             { sfx_STREAM,                "sfx\\towners\\priest05.wav",  nullptr },
/*Tremain6*/             { sfx_STREAM,                "sfx\\towners\\priest06.wav",  nullptr },
/*Tremain7*/             { sfx_STREAM,                "sfx\\towners\\priest07.wav",  nullptr },
/*Cain0*/                { sfx_STREAM,                "sfx\\towners\\storyt00.wav",  nullptr },
/*Cain1*/                { sfx_STREAM,                "sfx\\towners\\storyt01.wav",  nullptr },
/*Cain2*/                { sfx_STREAM,                "sfx\\towners\\storyt02.wav",  nullptr },
/*Cain3*/                { sfx_STREAM,                "sfx\\towners\\storyt03.wav",  nullptr },
/*Cain4*/                { sfx_STREAM,                "sfx\\towners\\storyt04.wav",  nullptr },
/*Cain5*/                { sfx_STREAM,                "sfx\\towners\\storyt05.wav",  nullptr },
/*Cain6*/                { sfx_STREAM,                "sfx\\towners\\storyt06.wav",  nullptr },
/*Cain7*/                { sfx_STREAM,                "sfx\\towners\\storyt07.wav",  nullptr },
/*Cain8*/                { sfx_STREAM,                "sfx\\towners\\storyt08.wav",  nullptr },
/*Cain9*/                { sfx_STREAM,                "sfx\\towners\\storyt09.wav",  nullptr },
/*Cain10*/               { sfx_STREAM,                "sfx\\towners\\storyt10.wav",  nullptr },
/*Cain11*/               { sfx_STREAM,                "sfx\\towners\\storyt11.wav",  nullptr },
/*Cain12*/               { sfx_STREAM,                "sfx\\towners\\storyt12.wav",  nullptr },
/*Cain13*/               { sfx_STREAM,                "sfx\\towners\\storyt13.wav",  nullptr },
/*Cain14*/               { sfx_STREAM,                "sfx\\towners\\storyt14.wav",  nullptr },
/*Cain15*/               { sfx_STREAM,                "sfx\\towners\\storyt15.wav",  nullptr },
/*Cain16*/               { sfx_STREAM,                "sfx\\towners\\storyt16.wav",  nullptr },
/*Cain17*/               { sfx_STREAM,                "sfx\\towners\\storyt17.wav",  nullptr },
/*Cain18*/               { sfx_STREAM,                "sfx\\towners\\storyt18.wav",  nullptr },
/*Cain19*/               { sfx_STREAM,                "sfx\\towners\\storyt19.wav",  nullptr },
/*Cain20*/               { sfx_STREAM,                "sfx\\towners\\storyt20.wav",  nullptr },
/*Cain21*/               { sfx_STREAM,                "sfx\\towners\\storyt21.wav",  nullptr },
/*Cain22*/               { sfx_STREAM,                "sfx\\towners\\storyt22.wav",  nullptr },
/*Cain23*/               { sfx_STREAM,                "sfx\\towners\\storyt23.wav",  nullptr },
/*Cain24*/               { sfx_STREAM,                "sfx\\towners\\storyt24.wav",  nullptr },
/*Cain25*/               { sfx_STREAM,                "sfx\\towners\\storyt25.wav",  nullptr },
/*Cain26*/               { sfx_STREAM,                "sfx\\towners\\storyt26.wav",  nullptr },
/*Cain27*/               { sfx_STREAM,                "sfx\\towners\\storyt27.wav",  nullptr },
/*Cain28*/               { sfx_STREAM,                "sfx\\towners\\storyt28.wav",  nullptr },
/*Cain29*/               { sfx_STREAM,                "sfx\\towners\\storyt29.wav",  nullptr },
/*Cain30*/               { sfx_STREAM,                "sfx\\towners\\storyt30.wav",  nullptr },
/*Cain31*/               { sfx_STREAM,                "sfx\\towners\\storyt31.wav",  nullptr },
/*Cain32*/               { sfx_STREAM,                "sfx\\towners\\storyt32.wav",  nullptr },
/*Cain33*/               { sfx_STREAM,                "sfx\\towners\\storyt33.wav",  nullptr },
/*Cain34*/               { sfx_STREAM,                "sfx\\towners\\storyt34.wav",  nullptr },
/*Cain35*/               { sfx_STREAM,                "sfx\\towners\\storyt35.wav",  nullptr },
/*Cain36*/               { sfx_STREAM,                "sfx\\towners\\storyt36.wav",  nullptr },
/*Cain37*/               { sfx_STREAM,                "sfx\\towners\\storyt37.wav",  nullptr },
/*Cain38*/               { sfx_STREAM,                "sfx\\towners\\storyt38.wav",  nullptr },
/*Ogden0*/               { sfx_STREAM,                "sfx\\towners\\tavown00.wav",  nullptr },
/*Ogden1*/               { sfx_STREAM,                "sfx\\towners\\tavown01.wav",  nullptr },
/*Ogden2*/               { sfx_STREAM,                "sfx\\towners\\tavown02.wav",  nullptr },
/*Ogden3*/               { sfx_STREAM,                "sfx\\towners\\tavown03.wav",  nullptr },
/*Ogden4*/               { sfx_STREAM,                "sfx\\towners\\tavown04.wav",  nullptr },
/*Ogden5*/               { sfx_STREAM,                "sfx\\towners\\tavown05.wav",  nullptr },
/*Ogden6*/               { sfx_STREAM,                "sfx\\towners\\tavown06.wav",  nullptr },
/*Ogden7*/               { sfx_STREAM,                "sfx\\towners\\tavown07.wav",  nullptr },
/*Ogden8*/               { sfx_STREAM,                "sfx\\towners\\tavown08.wav",  nullptr },
/*Ogden9*/               { sfx_STREAM,                "sfx\\towners\\tavown09.wav",  nullptr },
/*Ogden10*/              { sfx_STREAM,                "sfx\\towners\\tavown10.wav",  nullptr },
/*Ogden11*/              { sfx_STREAM,                "sfx\\towners\\tavown11.wav",  nullptr },
/*Ogden12*/              { sfx_STREAM,                "sfx\\towners\\tavown12.wav",  nullptr },
/*Ogden13*/              { sfx_STREAM,                "sfx\\towners\\tavown13.wav",  nullptr },
/*Ogden14*/              { sfx_STREAM,                "sfx\\towners\\tavown14.wav",  nullptr },
/*Ogden15*/              { sfx_STREAM,                "sfx\\towners\\tavown15.wav",  nullptr },
/*Ogden16*/              { sfx_STREAM,                "sfx\\towners\\tavown16.wav",  nullptr },
/*Ogden17*/              { sfx_STREAM,                "sfx\\towners\\tavown17.wav",  nullptr },
/*Ogden18*/              { sfx_STREAM,                "sfx\\towners\\tavown18.wav",  nullptr },
/*Ogden19*/              { sfx_STREAM,                "sfx\\towners\\tavown19.wav",  nullptr },
/*Ogden20*/              { sfx_STREAM,                "sfx\\towners\\tavown20.wav",  nullptr },
/*Ogden21*/              { sfx_STREAM,                "sfx\\towners\\tavown21.wav",  nullptr },
/*Ogden22*/              { sfx_STREAM,                "sfx\\towners\\tavown22.wav",  nullptr },
/*Ogden23*/              { sfx_STREAM,                "sfx\\towners\\tavown23.wav",  nullptr },
/*Ogden24*/              { sfx_STREAM,                "sfx\\towners\\tavown24.wav",  nullptr },
/*Ogden25*/              { sfx_STREAM,                "sfx\\towners\\tavown25.wav",  nullptr },
/*Ogden26*/              { sfx_STREAM,                "sfx\\towners\\tavown26.wav",  nullptr },
/*Ogden27*/              { sfx_STREAM,                "sfx\\towners\\tavown27.wav",  nullptr },
/*Ogden28*/              { sfx_STREAM,                "sfx\\towners\\tavown28.wav",  nullptr },
/*Ogden29*/              { sfx_STREAM,                "sfx\\towners\\tavown29.wav",  nullptr },
/*Ogden30*/              { sfx_STREAM,                "sfx\\towners\\tavown30.wav",  nullptr },
/*Ogden31*/              { sfx_STREAM,                "sfx\\towners\\tavown31.wav",  nullptr },
/*Ogden32*/              { sfx_STREAM,                "sfx\\towners\\tavown32.wav",  nullptr },
/*Ogden33*/              { sfx_STREAM,                "sfx\\towners\\tavown33.wav",  nullptr },
/*Ogden34*/              { sfx_STREAM,                "sfx\\towners\\tavown34.wav",  nullptr },
/*Ogden35*/              { sfx_STREAM,                "sfx\\towners\\tavown35.wav",  nullptr },
/*Ogden36*/              { sfx_STREAM,                "sfx\\towners\\tavown36.wav",  nullptr },
/*Ogden37*/              { sfx_STREAM,                "sfx\\towners\\tavown37.wav",  nullptr },
/*Ogden38*/              { sfx_STREAM,                "sfx\\towners\\tavown38.wav",  nullptr },
/*Ogden39*/              { sfx_STREAM,                "sfx\\towners\\tavown39.wav",  nullptr },
/*Ogden40*/              { sfx_STREAM,                "sfx\\towners\\tavown40.wav",  nullptr },
/*Ogden41*/              { sfx_STREAM,                "sfx\\towners\\tavown41.wav",  nullptr },
/*Ogden42*/              { sfx_STREAM,                "sfx\\towners\\tavown42.wav",  nullptr },
/*Ogden43*/              { sfx_STREAM,                "sfx\\towners\\tavown43.wav",  nullptr },
/*Ogden44*/              { sfx_STREAM,                "sfx\\towners\\tavown44.wav",  nullptr },
/*Ogden45*/              { sfx_STREAM,                "sfx\\towners\\tavown45.wav",  nullptr },
/*Adria1*/               { sfx_STREAM,                "sfx\\towners\\witch01.wav",   nullptr },
/*Adria2*/               { sfx_STREAM,                "sfx\\towners\\witch02.wav",   nullptr },
/*Adria3*/               { sfx_STREAM,                "sfx\\towners\\witch03.wav",   nullptr },
/*Adria4*/               { sfx_STREAM,                "sfx\\towners\\witch04.wav",   nullptr },
/*Adria5*/               { sfx_STREAM,                "sfx\\towners\\witch05.wav",   nullptr },
/*Adria6*/               { sfx_STREAM,                "sfx\\towners\\witch06.wav",   nullptr },
/*Adria7*/               { sfx_STREAM,                "sfx\\towners\\witch07.wav",   nullptr },
/*Adria8*/               { sfx_STREAM,                "sfx\\towners\\witch08.wav",   nullptr },
/*Adria9*/               { sfx_STREAM,                "sfx\\towners\\witch09.wav",   nullptr },
/*Adria10*/              { sfx_STREAM,                "sfx\\towners\\witch10.wav",   nullptr },
/*Adria11*/              { sfx_STREAM,                "sfx\\towners\\witch11.wav",   nullptr },
/*Adria12*/              { sfx_STREAM,                "sfx\\towners\\witch12.wav",   nullptr },
/*Adria13*/              { sfx_STREAM,                "sfx\\towners\\witch13.wav",   nullptr },
/*Adria14*/              { sfx_STREAM,                "sfx\\towners\\witch14.wav",   nullptr },
/*Adria15*/              { sfx_STREAM,                "sfx\\towners\\witch15.wav",   nullptr },
/*Adria16*/              { sfx_STREAM,                "sfx\\towners\\witch16.wav",   nullptr },
/*Adria17*/              { sfx_STREAM,                "sfx\\towners\\witch17.wav",   nullptr },
/*Adria18*/              { sfx_STREAM,                "sfx\\towners\\witch18.wav",   nullptr },
/*Adria19*/              { sfx_STREAM,                "sfx\\towners\\witch19.wav",   nullptr },
/*Adria20*/              { sfx_STREAM,                "sfx\\towners\\witch20.wav",   nullptr },
/*Adria21*/              { sfx_STREAM,                "sfx\\towners\\witch21.wav",   nullptr },
/*Adria22*/              { sfx_STREAM,                "sfx\\towners\\witch22.wav",   nullptr },
/*Adria23*/              { sfx_STREAM,                "sfx\\towners\\witch23.wav",   nullptr },
/*Adria24*/              { sfx_STREAM,                "sfx\\towners\\witch24.wav",   nullptr },
/*Adria25*/              { sfx_STREAM,                "sfx\\towners\\witch25.wav",   nullptr },
/*Adria26*/              { sfx_STREAM,                "sfx\\towners\\witch26.wav",   nullptr },
/*Adria27*/              { sfx_STREAM,                "sfx\\towners\\witch27.wav",   nullptr },
/*Adria28*/              { sfx_STREAM,                "sfx\\towners\\witch28.wav",   nullptr },
/*Adria29*/              { sfx_STREAM,                "sfx\\towners\\witch29.wav",   nullptr },
/*Adria30*/              { sfx_STREAM,                "sfx\\towners\\witch30.wav",   nullptr },
/*Adria31*/              { sfx_STREAM,                "sfx\\towners\\witch31.wav",   nullptr },
/*Adria32*/              { sfx_STREAM,                "sfx\\towners\\witch32.wav",   nullptr },
/*Adria33*/              { sfx_STREAM,                "sfx\\towners\\witch33.wav",   nullptr },
/*Adria34*/              { sfx_STREAM,                "sfx\\towners\\witch34.wav",   nullptr },
/*Adria35*/              { sfx_STREAM,                "sfx\\towners\\witch35.wav",   nullptr },
/*Adria36*/              { sfx_STREAM,                "sfx\\towners\\witch36.wav",   nullptr },
/*Adria37*/              { sfx_STREAM,                "sfx\\towners\\witch37.wav",   nullptr },
/*Adria38*/              { sfx_STREAM,                "sfx\\towners\\witch38.wav",   nullptr },
/*Adria39*/              { sfx_STREAM,                "sfx\\towners\\witch39.wav",   nullptr },
/*Adria40*/              { sfx_STREAM,                "sfx\\towners\\witch40.wav",   nullptr },
/*Adria41*/              { sfx_STREAM,                "sfx\\towners\\witch41.wav",   nullptr },
/*Adria42*/              { sfx_STREAM,                "sfx\\towners\\witch42.wav",   nullptr },
/*Adria43*/              { sfx_STREAM,                "sfx\\towners\\witch43.wav",   nullptr },
/*Adria44*/              { sfx_STREAM,                "sfx\\towners\\witch44.wav",   nullptr },
/*Adria45*/              { sfx_STREAM,                "sfx\\towners\\witch45.wav",   nullptr },
/*Adria46*/              { sfx_STREAM,                "sfx\\towners\\witch46.wav",   nullptr },
/*Adria47*/              { sfx_STREAM,                "sfx\\towners\\witch47.wav",   nullptr },
/*Adria48*/              { sfx_STREAM,                "sfx\\towners\\witch48.wav",   nullptr },
/*Adria49*/              { sfx_STREAM,                "sfx\\towners\\witch49.wav",   nullptr },
/*Adria50*/              { sfx_STREAM,                "sfx\\towners\\witch50.wav",   nullptr },
/*WoundedTownsman*/      { sfx_STREAM,                "sfx\\towners\\wound01.wav",   nullptr },
/*Sorceror1*/            { sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage01.wav",   nullptr },
/*Sorceror2*/            { sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage02.wav",   nullptr },
/*Sorceror3*/            { sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage03.wav",   nullptr },
/*Sorceror4*/            { sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage04.wav",   nullptr },
/*Sorceror5*/            { sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage05.wav",   nullptr },
/*Sorceror6*/            { sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage06.wav",   nullptr },
/*Sorceror7*/            { sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage07.wav",   nullptr },
/*Sorceror8*/            { sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage08.wav",   nullptr },
/*Sorceror9*/            { sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage09.wav",   nullptr },
/*Sorceror10*/           { sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage10.wav",   nullptr },
/*Sorceror11*/           { sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage11.wav",   nullptr },
/*Sorceror12*/           { sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage12.wav",   nullptr },
/*Sorceror13*/           { sfx_SORCERER,              "sfx\\sorceror\\mage13.wav",   nullptr },
/*Sorceror14*/           { sfx_SORCERER,              "sfx\\sorceror\\mage14.wav",   nullptr },
/*Sorceror15*/           { sfx_SORCERER,              "sfx\\sorceror\\mage15.wav",   nullptr },
/*Sorceror16*/           { sfx_SORCERER,              "sfx\\sorceror\\mage16.wav",   nullptr },
/*Sorceror17*/           { sfx_SORCERER,              "sfx\\sorceror\\mage17.wav",   nullptr },
/*Sorceror18*/           { sfx_SORCERER,              "sfx\\sorceror\\mage18.wav",   nullptr },
/*Sorceror19*/           { sfx_SORCERER,              "sfx\\sorceror\\mage19.wav",   nullptr },
/*Sorceror20*/           { sfx_SORCERER,              "sfx\\sorceror\\mage20.wav",   nullptr },
/*Sorceror21*/           { sfx_SORCERER,              "sfx\\sorceror\\mage21.wav",   nullptr },
/*Sorceror22*/           { sfx_SORCERER,              "sfx\\sorceror\\mage22.wav",   nullptr },
/*Sorceror23*/           { sfx_SORCERER,              "sfx\\sorceror\\mage23.wav",   nullptr },
/*Sorceror24*/           { sfx_SORCERER,              "sfx\\sorceror\\mage24.wav",   nullptr },
/*Sorceror25*/           { sfx_SORCERER,              "sfx\\sorceror\\mage25.wav",   nullptr },
/*Sorceror26*/           { sfx_SORCERER,              "sfx\\sorceror\\mage26.wav",   nullptr },
/*Sorceror27*/           { sfx_SORCERER,              "sfx\\sorceror\\mage27.wav",   nullptr },
/*Sorceror28*/           { sfx_SORCERER,              "sfx\\sorceror\\mage28.wav",   nullptr },
/*Sorceror29*/           { sfx_SORCERER,              "sfx\\sorceror\\mage29.wav",   nullptr },
/*Sorceror30*/           { sfx_SORCERER,              "sfx\\sorceror\\mage30.wav",   nullptr },
/*Sorceror31*/           { sfx_SORCERER,              "sfx\\sorceror\\mage31.wav",   nullptr },
/*Sorceror32*/           { sfx_SORCERER,              "sfx\\sorceror\\mage32.wav",   nullptr },
/*Sorceror33*/           { sfx_SORCERER,              "sfx\\sorceror\\mage33.wav",   nullptr },
/*Sorceror34*/           { sfx_SORCERER,              "sfx\\sorceror\\mage34.wav",   nullptr },
/*Sorceror35*/           { sfx_SORCERER,              "sfx\\sorceror\\mage35.wav",   nullptr },
/*Sorceror36*/           { sfx_SORCERER,              "sfx\\sorceror\\mage36.wav",   nullptr },
/*Sorceror37*/           { sfx_SORCERER,              "sfx\\sorceror\\mage37.wav",   nullptr },
/*Sorceror38*/           { sfx_SORCERER,              "sfx\\sorceror\\mage38.wav",   nullptr },
/*Sorceror39*/           { sfx_SORCERER,              "sfx\\sorceror\\mage39.wav",   nullptr },
/*Sorceror40*/           { sfx_SORCERER,              "sfx\\sorceror\\mage40.wav",   nullptr },
/*Sorceror41*/           { sfx_SORCERER,              "sfx\\sorceror\\mage41.wav",   nullptr },
/*Sorceror42*/           { sfx_SORCERER,              "sfx\\sorceror\\mage42.wav",   nullptr },
/*Sorceror43*/           { sfx_SORCERER,              "sfx\\sorceror\\mage43.wav",   nullptr },
/*Sorceror44*/           { sfx_SORCERER,              "sfx\\sorceror\\mage44.wav",   nullptr },
/*Sorceror45*/           { sfx_SORCERER,              "sfx\\sorceror\\mage45.wav",   nullptr },
/*Sorceror46*/           { sfx_SORCERER,              "sfx\\sorceror\\mage46.wav",   nullptr },
/*Sorceror47*/           { sfx_SORCERER,              "sfx\\sorceror\\mage47.wav",   nullptr },
/*Sorceror48*/           { sfx_SORCERER,              "sfx\\sorceror\\mage48.wav",   nullptr },
/*Sorceror49*/           { sfx_SORCERER,              "sfx\\sorceror\\mage49.wav",   nullptr },
/*Sorceror50*/           { sfx_SORCERER,              "sfx\\sorceror\\mage50.wav",   nullptr },
/*Sorceror51*/           { sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage51.wav",   nullptr },
/*Sorceror52*/           { sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage52.wav",   nullptr },
/*Sorceror53*/           { sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage53.wav",   nullptr },
/*Sorceror54*/           { sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage54.wav",   nullptr },
/*Sorceror55*/           { sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage55.wav",   nullptr },
/*Sorceror56*/           { sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage56.wav",   nullptr },
/*Sorceror57*/           { sfx_SORCERER,              "sfx\\sorceror\\mage57.wav",   nullptr },
/*Sorceror58*/           { sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage58.wav",   nullptr },
/*Sorceror59*/           { sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage59.wav",   nullptr },
/*Sorceror60*/           { sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage60.wav",   nullptr },
/*Sorceror61*/           { sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage61.wav",   nullptr },
/*Sorceror62*/           { sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage62.wav",   nullptr },
/*Sorceror63*/           { sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage63.wav",   nullptr },
/*Sorceror64*/           { sfx_SORCERER,              "sfx\\sorceror\\mage64.wav",   nullptr },
/*Sorceror65*/           { sfx_SORCERER,              "sfx\\sorceror\\mage65.wav",   nullptr },
/*Sorceror66*/           { sfx_SORCERER,              "sfx\\sorceror\\mage66.wav",   nullptr },
/*Sorceror67*/           { sfx_SORCERER,              "sfx\\sorceror\\mage67.wav",   nullptr },
/*Sorceror68*/           { sfx_SORCERER,              "sfx\\sorceror\\mage68.wav",   nullptr },
/*Sorceror69*/           { sfx_SORCERER,              "sfx\\sorceror\\mage69.wav",   nullptr },
/*Sorceror69b*/          { sfx_SORCERER,              "sfx\\sorceror\\mage69b.wav",  nullptr },
/*Sorceror70*/           { sfx_SORCERER,              "sfx\\sorceror\\mage70.wav",   nullptr },
/*Sorceror71*/           { sfx_SORCERER,              "sfx\\sorceror\\mage71.wav",   nullptr },
/*Sorceror72*/           { sfx_SORCERER,              "sfx\\sorceror\\mage72.wav",   nullptr },
/*Sorceror73*/           { sfx_SORCERER,              "sfx\\sorceror\\mage73.wav",   nullptr },
/*Sorceror74*/           { sfx_SORCERER,              "sfx\\sorceror\\mage74.wav",   nullptr },
/*Sorceror75*/           { sfx_SORCERER,              "sfx\\sorceror\\mage75.wav",   nullptr },
/*Sorceror76*/           { sfx_SORCERER,              "sfx\\sorceror\\mage76.wav",   nullptr },
/*Sorceror77*/           { sfx_SORCERER,              "sfx\\sorceror\\mage77.wav",   nullptr },
/*Sorceror78*/           { sfx_SORCERER,              "sfx\\sorceror\\mage78.wav",   nullptr },
/*Sorceror79*/           { sfx_SORCERER,              "sfx\\sorceror\\mage79.wav",   nullptr },
/*Sorceror80*/           { sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage80.wav",   nullptr },
/*Sorceror81*/           { sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage81.wav",   nullptr },
/*Sorceror82*/           { sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage82.wav",   nullptr },
/*Sorceror83*/           { sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage83.wav",   nullptr },
/*Sorceror84*/           { sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage84.wav",   nullptr },
/*Sorceror85*/           { sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage85.wav",   nullptr },
/*Sorceror86*/           { sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage86.wav",   nullptr },
/*Sorceror87*/           { sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage87.wav",   nullptr },
/*Sorceror88*/           { sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage88.wav",   nullptr },
/*Sorceror89*/           { sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage89.wav",   nullptr },
/*Sorceror90*/           { sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage90.wav",   nullptr },
/*Sorceror91*/           { sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage91.wav",   nullptr },
/*Sorceror92*/           { sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage92.wav",   nullptr },
/*Sorceror93*/           { sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage93.wav",   nullptr },
/*Sorceror94*/           { sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage94.wav",   nullptr },
/*Sorceror95*/           { sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage95.wav",   nullptr },
/*Sorceror96*/           { sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage96.wav",   nullptr },
/*Sorceror97*/           { sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage97.wav",   nullptr },
/*Sorceror98*/           { sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage98.wav",   nullptr },
/*Sorceror99*/           { sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage99.wav",   nullptr },
/*Sorceror100*/          { sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage100.wav",  nullptr },
/*Sorceror101*/          { sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage101.wav",  nullptr },
/*Sorceror102*/          { sfx_STREAM | sfx_SORCERER, "sfx\\sorceror\\mage102.wav",  nullptr },
/*Rogue1*/               { sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue01.wav",     nullptr },
/*Rogue2*/               { sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue02.wav",     nullptr },
/*Rogue3*/               { sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue03.wav",     nullptr },
/*Rogue4*/               { sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue04.wav",     nullptr },
/*Rogue5*/               { sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue05.wav",     nullptr },
/*Rogue6*/               { sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue06.wav",     nullptr },
/*Rogue7*/               { sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue07.wav",     nullptr },
/*Rogue8*/               { sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue08.wav",     nullptr },
/*Rogue9*/               { sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue09.wav",     nullptr },
/*Rogue10*/              { sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue10.wav",     nullptr },
/*Rogue11*/              { sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue11.wav",     nullptr },
/*Rogue12*/              { sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue12.wav",     nullptr },
/*Rogue13*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue13.wav",     nullptr },
/*Rogue14*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue14.wav",     nullptr },
/*Rogue15*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue15.wav",     nullptr },
/*Rogue16*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue16.wav",     nullptr },
/*Rogue17*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue17.wav",     nullptr },
/*Rogue18*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue18.wav",     nullptr },
/*Rogue19*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue19.wav",     nullptr },
/*Rogue20*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue20.wav",     nullptr },
/*Rogue21*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue21.wav",     nullptr },
/*Rogue22*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue22.wav",     nullptr },
/*Rogue23*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue23.wav",     nullptr },
/*Rogue24*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue24.wav",     nullptr },
/*Rogue25*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue25.wav",     nullptr },
/*Rogue26*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue26.wav",     nullptr },
/*Rogue27*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue27.wav",     nullptr },
/*Rogue28*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue28.wav",     nullptr },
/*Rogue29*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue29.wav",     nullptr },
/*Rogue30*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue30.wav",     nullptr },
/*Rogue31*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue31.wav",     nullptr },
/*Rogue32*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue32.wav",     nullptr },
/*Rogue33*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue33.wav",     nullptr },
/*Rogue34*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue34.wav",     nullptr },
/*Rogue35*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue35.wav",     nullptr },
/*Rogue36*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue36.wav",     nullptr },
/*Rogue37*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue37.wav",     nullptr },
/*Rogue38*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue38.wav",     nullptr },
/*Rogue39*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue39.wav",     nullptr },
/*Rogue40*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue40.wav",     nullptr },
/*Rogue41*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue41.wav",     nullptr },
/*Rogue42*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue42.wav",     nullptr },
/*Rogue43*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue43.wav",     nullptr },
/*Rogue44*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue44.wav",     nullptr },
/*Rogue45*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue45.wav",     nullptr },
/*Rogue46*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue46.wav",     nullptr },
/*Rogue47*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue47.wav",     nullptr },
/*Rogue48*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue48.wav",     nullptr },
/*Rogue49*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue49.wav",     nullptr },
/*Rogue50*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue50.wav",     nullptr },
/*Rogue51*/              { sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue51.wav",     nullptr },
/*Rogue52*/              { sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue52.wav",     nullptr },
/*Rogue53*/              { sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue53.wav",     nullptr },
/*Rogue54*/              { sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue54.wav",     nullptr },
/*Rogue55*/              { sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue55.wav",     nullptr },
/*Rogue56*/              { sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue56.wav",     nullptr },
/*Rogue57*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue57.wav",     nullptr },
/*Rogue58*/              { sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue58.wav",     nullptr },
/*Rogue59*/              { sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue59.wav",     nullptr },
/*Rogue60*/              { sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue60.wav",     nullptr },
/*Rogue61*/              { sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue61.wav",     nullptr },
/*Rogue62*/              { sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue62.wav",     nullptr },
/*Rogue63*/              { sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue63.wav",     nullptr },
/*Rogue64*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue64.wav",     nullptr },
/*Rogue65*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue65.wav",     nullptr },
/*Rogue66*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue66.wav",     nullptr },
/*Rogue67*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue67.wav",     nullptr },
/*Rogue68*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue68.wav",     nullptr },
/*Rogue69*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue69.wav",     nullptr },
/*Rogue69b*/             { sfx_ROGUE,                 "sfx\\rogue\\rogue69b.wav",    nullptr },
/*Rogue70*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue70.wav",     nullptr },
/*Rogue71*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue71.wav",     nullptr },
/*Rogue72*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue72.wav",     nullptr },
/*Rogue73*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue73.wav",     nullptr },
/*Rogue74*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue74.wav",     nullptr },
/*Rogue75*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue75.wav",     nullptr },
/*Rogue76*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue76.wav",     nullptr },
/*Rogue77*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue77.wav",     nullptr },
/*Rogue78*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue78.wav",     nullptr },
/*Rogue79*/              { sfx_ROGUE,                 "sfx\\rogue\\rogue79.wav",     nullptr },
/*Rogue80*/              { sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue80.wav",     nullptr },
/*Rogue81*/              { sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue81.wav",     nullptr },
/*Rogue82*/              { sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue82.wav",     nullptr },
/*Rogue83*/              { sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue83.wav",     nullptr },
/*Rogue84*/              { sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue84.wav",     nullptr },
/*Rogue85*/              { sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue85.wav",     nullptr },
/*Rogue86*/              { sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue86.wav",     nullptr },
/*Rogue87*/              { sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue87.wav",     nullptr },
/*Rogue88*/              { sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue88.wav",     nullptr },
/*Rogue89*/              { sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue89.wav",     nullptr },
/*Rogue90*/              { sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue90.wav",     nullptr },
/*Rogue91*/              { sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue91.wav",     nullptr },
/*Rogue92*/              { sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue92.wav",     nullptr },
/*Rogue93*/              { sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue93.wav",     nullptr },
/*Rogue94*/              { sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue94.wav",     nullptr },
/*Rogue95*/              { sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue95.wav",     nullptr },
/*Rogue96*/              { sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue96.wav",     nullptr },
/*Rogue97*/              { sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue97.wav",     nullptr },
/*Rogue98*/              { sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue98.wav",     nullptr },
/*Rogue99*/              { sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue99.wav",     nullptr },
/*Rogue100*/             { sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue100.wav",    nullptr },
/*Rogue101*/             { sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue101.wav",    nullptr },
/*Rogue102*/             { sfx_STREAM | sfx_ROGUE,    "sfx\\rogue\\rogue102.wav",    nullptr },
/*Warrior1*/             { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior01.wav",  nullptr },
/*Warrior2*/             { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior02.wav",  nullptr },
/*Warrior3*/             { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior03.wav",  nullptr },
/*Warrior4*/             { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior04.wav",  nullptr },
/*Warrior5*/             { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior05.wav",  nullptr },
/*Warrior6*/             { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior06.wav",  nullptr },
/*Warrior7*/             { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior07.wav",  nullptr },
/*Warrior8*/             { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior08.wav",  nullptr },
/*Warrior9*/             { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior09.wav",  nullptr },
/*Warrior10*/            { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior10.wav",  nullptr },
/*Warrior11*/            { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior11.wav",  nullptr },
/*Warrior12*/            { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior12.wav",  nullptr },
/*Warrior13*/            { sfx_WARRIOR,               "sfx\\warrior\\warior13.wav",  nullptr },
/*Warrior14*/            { sfx_WARRIOR,               "sfx\\warrior\\warior14.wav",  nullptr },
/*Warrior14b*/           { sfx_WARRIOR,               "sfx\\warrior\\wario14b.wav",  nullptr },
/*Warrior14c*/           { sfx_WARRIOR,               "sfx\\warrior\\wario14c.wav",  nullptr },
/*Warrior15*/            { sfx_WARRIOR,               "sfx\\warrior\\warior15.wav",  nullptr },
/*Warrior15b*/           { sfx_WARRIOR,               "sfx\\warrior\\wario15b.wav",  nullptr },
/*Warrior15c*/           { sfx_WARRIOR,               "sfx\\warrior\\wario15c.wav",  nullptr },
/*Warrior16*/            { sfx_WARRIOR,               "sfx\\warrior\\warior16.wav",  nullptr },
/*Warrior16b*/           { sfx_WARRIOR,               "sfx\\warrior\\wario16b.wav",  nullptr },
/*Warrior16c*/           { sfx_WARRIOR,               "sfx\\warrior\\wario16c.wav",  nullptr },
/*Warrior17*/            { sfx_WARRIOR,               "sfx\\warrior\\warior17.wav",  nullptr },
/*Warrior18*/            { sfx_WARRIOR,               "sfx\\warrior\\warior18.wav",  nullptr },
/*Warrior19*/            { sfx_WARRIOR,               "sfx\\warrior\\warior19.wav",  nullptr },
/*Warrior20*/            { sfx_WARRIOR,               "sfx\\warrior\\warior20.wav",  nullptr },
/*Warrior21*/            { sfx_WARRIOR,               "sfx\\warrior\\warior21.wav",  nullptr },
/*Warrior22*/            { sfx_WARRIOR,               "sfx\\warrior\\warior22.wav",  nullptr },
/*Warrior23*/            { sfx_WARRIOR,               "sfx\\warrior\\warior23.wav",  nullptr },
/*Warrior24*/            { sfx_WARRIOR,               "sfx\\warrior\\warior24.wav",  nullptr },
/*Warrior25*/            { sfx_WARRIOR,               "sfx\\warrior\\warior25.wav",  nullptr },
/*Warrior26*/            { sfx_WARRIOR,               "sfx\\warrior\\warior26.wav",  nullptr },
/*Warrior27*/            { sfx_WARRIOR,               "sfx\\warrior\\warior27.wav",  nullptr },
/*Warrior28*/            { sfx_WARRIOR,               "sfx\\warrior\\warior28.wav",  nullptr },
/*Warrior29*/            { sfx_WARRIOR,               "sfx\\warrior\\warior29.wav",  nullptr },
/*Warrior30*/            { sfx_WARRIOR,               "sfx\\warrior\\warior30.wav",  nullptr },
/*Warrior31*/            { sfx_WARRIOR,               "sfx\\warrior\\warior31.wav",  nullptr },
/*Warrior32*/            { sfx_WARRIOR,               "sfx\\warrior\\warior32.wav",  nullptr },
/*Warrior33*/            { sfx_WARRIOR,               "sfx\\warrior\\warior33.wav",  nullptr },
/*Warrior34*/            { sfx_WARRIOR,               "sfx\\warrior\\warior34.wav",  nullptr },
/*Warrior35*/            { sfx_WARRIOR,               "sfx\\warrior\\warior35.wav",  nullptr },
/*Warrior36*/            { sfx_WARRIOR,               "sfx\\warrior\\warior36.wav",  nullptr },
/*Warrior37*/            { sfx_WARRIOR,               "sfx\\warrior\\warior37.wav",  nullptr },
/*Warrior38*/            { sfx_WARRIOR,               "sfx\\warrior\\warior38.wav",  nullptr },
/*Warrior39*/            { sfx_WARRIOR,               "sfx\\warrior\\warior39.wav",  nullptr },
/*Warrior40*/            { sfx_WARRIOR,               "sfx\\warrior\\warior40.wav",  nullptr },
/*Warrior41*/            { sfx_WARRIOR,               "sfx\\warrior\\warior41.wav",  nullptr },
/*Warrior42*/            { sfx_WARRIOR,               "sfx\\warrior\\warior42.wav",  nullptr },
/*Warrior43*/            { sfx_WARRIOR,               "sfx\\warrior\\warior43.wav",  nullptr },
/*Warrior44*/            { sfx_WARRIOR,               "sfx\\warrior\\warior44.wav",  nullptr },
/*Warrior45*/            { sfx_WARRIOR,               "sfx\\warrior\\warior45.wav",  nullptr },
/*Warrior46*/            { sfx_WARRIOR,               "sfx\\warrior\\warior46.wav",  nullptr },
/*Warrior47*/            { sfx_WARRIOR,               "sfx\\warrior\\warior47.wav",  nullptr },
/*Warrior48*/            { sfx_WARRIOR,               "sfx\\warrior\\warior48.wav",  nullptr },
/*Warrior49*/            { sfx_WARRIOR,               "sfx\\warrior\\warior49.wav",  nullptr },
/*Warrior50*/            { sfx_WARRIOR,               "sfx\\warrior\\warior50.wav",  nullptr },
/*Warrior51*/            { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior51.wav",  nullptr },
/*Warrior52*/            { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior52.wav",  nullptr },
/*Warrior53*/            { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior53.wav",  nullptr },
/*Warrior54*/            { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior54.wav",  nullptr },
/*Warrior55*/            { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior55.wav",  nullptr },
/*Warrior56*/            { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior56.wav",  nullptr },
/*Warrior57*/            { sfx_WARRIOR,               "sfx\\warrior\\warior57.wav",  nullptr },
/*Warrior58*/            { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior58.wav",  nullptr },
/*Warrior59*/            { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior59.wav",  nullptr },
/*Warrior60*/            { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior60.wav",  nullptr },
/*Warrior61*/            { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior61.wav",  nullptr },
/*Warrior62*/            { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior62.wav",  nullptr },
/*Warrior63*/            { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior63.wav",  nullptr },
/*Warrior64*/            { sfx_WARRIOR,               "sfx\\warrior\\warior64.wav",  nullptr },
/*Warrior65*/            { sfx_WARRIOR,               "sfx\\warrior\\warior65.wav",  nullptr },
/*Warrior66*/            { sfx_WARRIOR,               "sfx\\warrior\\warior66.wav",  nullptr },
/*Warrior67*/            { sfx_WARRIOR,               "sfx\\warrior\\warior67.wav",  nullptr },
/*Warrior68*/            { sfx_WARRIOR,               "sfx\\warrior\\warior68.wav",  nullptr },
/*Warrior69*/            { sfx_WARRIOR,               "sfx\\warrior\\warior69.wav",  nullptr },
/*Warrior69b*/           { sfx_WARRIOR,               "sfx\\warrior\\wario69b.wav",  nullptr },
/*Warrior70*/            { sfx_WARRIOR,               "sfx\\warrior\\warior70.wav",  nullptr },
/*Warrior71*/            { sfx_WARRIOR,               "sfx\\warrior\\warior71.wav",  nullptr },
/*Warrior72*/            { sfx_WARRIOR,               "sfx\\warrior\\warior72.wav",  nullptr },
/*Warrior73*/            { sfx_WARRIOR,               "sfx\\warrior\\warior73.wav",  nullptr },
/*Warrior74*/            { sfx_WARRIOR,               "sfx\\warrior\\warior74.wav",  nullptr },
/*Warrior75*/            { sfx_WARRIOR,               "sfx\\warrior\\warior75.wav",  nullptr },
/*Warrior76*/            { sfx_WARRIOR,               "sfx\\warrior\\warior76.wav",  nullptr },
/*Warrior77*/            { sfx_WARRIOR,               "sfx\\warrior\\warior77.wav",  nullptr },
/*Warrior78*/            { sfx_WARRIOR,               "sfx\\warrior\\warior78.wav",  nullptr },
/*Warrior79*/            { sfx_WARRIOR,               "sfx\\warrior\\warior79.wav",  nullptr },
/*Warrior80*/            { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior80.wav",  nullptr },
/*Warrior81*/            { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior81.wav",  nullptr },
/*Warrior82*/            { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior82.wav",  nullptr },
/*Warrior83*/            { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior83.wav",  nullptr },
/*Warrior84*/            { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior84.wav",  nullptr },
/*Warrior85*/            { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior85.wav",  nullptr },
/*Warrior86*/            { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior86.wav",  nullptr },
/*Warrior87*/            { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior87.wav",  nullptr },
/*Warrior88*/            { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior88.wav",  nullptr },
/*Warrior89*/            { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior89.wav",  nullptr },
/*Warrior90*/            { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior90.wav",  nullptr },
/*Warrior91*/            { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior91.wav",  nullptr },
/*Warrior92*/            { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior92.wav",  nullptr },
/*Warrior93*/            { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior93.wav",  nullptr },
/*Warrior94*/            { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior94.wav",  nullptr },
/*Warrior95*/            { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior95.wav",  nullptr },
/*Warrior95b*/           { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\wario95b.wav",  nullptr },
/*Warrior95c*/           { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\wario95c.wav",  nullptr },
/*Warrior95d*/           { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\wario95d.wav",  nullptr },
/*Warrior95e*/           { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\wario95e.wav",  nullptr },
/*Warrior95f*/           { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\wario95f.wav",  nullptr },
/*Warrior96b*/           { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\wario96b.wav",  nullptr },
/*Warrior97*/            { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\wario97.wav",   nullptr },
/*Warrior98*/            { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\wario98.wav",   nullptr },
/*Warrior99*/            { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\warior99.wav",  nullptr },
/*Warrior100*/           { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\wario100.wav",  nullptr },
/*Warrior101*/           { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\wario101.wav",  nullptr },
/*Warrior102*/           { sfx_STREAM | sfx_WARRIOR,  "sfx\\warrior\\wario102.wav",  nullptr },
/*Monk1*/                { sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk01.wav",       nullptr },
/*Monk8*/                { sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk08.wav",       nullptr },
/*Monk9*/                { sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk09.wav",       nullptr },
/*Monk10*/               { sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk10.wav",       nullptr },
/*Monk11*/               { sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk11.wav",       nullptr },
/*Monk12*/               { sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk12.wav",       nullptr },
/*Monk13*/               { sfx_MONK,                  "sfx\\monk\\monk13.wav",       nullptr },
/*Monk14*/               { sfx_MONK,                  "sfx\\monk\\monk14.wav",       nullptr },
/*Monk15*/               { sfx_MONK,                  "sfx\\monk\\monk15.wav",       nullptr },
/*Monk16*/               { sfx_MONK,                  "sfx\\monk\\monk16.wav",       nullptr },
/*Monk24*/               { sfx_MONK,                  "sfx\\monk\\monk24.wav",       nullptr },
/*Monk27*/               { sfx_MONK,                  "sfx\\monk\\monk27.wav",       nullptr },
/*Monk29*/               { sfx_MONK,                  "sfx\\monk\\monk29.wav",       nullptr },
/*Monk34*/               { sfx_MONK,                  "sfx\\monk\\monk34.wav",       nullptr },
/*Monk35*/               { sfx_MONK,                  "sfx\\monk\\monk35.wav",       nullptr },
/*Monk43*/               { sfx_MONK,                  "sfx\\monk\\monk43.wav",       nullptr },
/*Monk46*/               { sfx_MONK,                  "sfx\\monk\\monk46.wav",       nullptr },
/*Monk49*/               { sfx_MONK,                  "sfx\\monk\\monk49.wav",       nullptr },
/*Monk50*/               { sfx_MONK,                  "sfx\\monk\\monk50.wav",       nullptr },
/*Monk52*/               { sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk52.wav",       nullptr },
/*Monk54*/               { sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk54.wav",       nullptr },
/*Monk55*/               { sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk55.wav",       nullptr },
/*Monk56*/               { sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk56.wav",       nullptr },
/*Monk61*/               { sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk61.wav",       nullptr },
/*Monk62*/               { sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk62.wav",       nullptr },
/*Monk68*/               { sfx_MONK,                  "sfx\\monk\\monk68.wav",       nullptr },
/*Monk69*/               { sfx_MONK,                  "sfx\\monk\\monk69.wav",       nullptr },
/*Monk69b*/              { sfx_MONK,                  "sfx\\monk\\monk69b.wav",      nullptr },
/*Monk70*/               { sfx_MONK,                  "sfx\\monk\\monk70.wav",       nullptr },
/*Monk71*/               { sfx_MONK,                  "sfx\\monk\\monk71.wav",       nullptr },
/*Monk79*/               { sfx_MONK,                  "sfx\\monk\\monk79.wav",       nullptr },
/*Monk80*/               { sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk80.wav",       nullptr },
/*Monk82*/               { sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk82.wav",       nullptr },
/*Monk83*/               { sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk83.wav",       nullptr },
/*Monk87*/               { sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk87.wav",       nullptr },
/*Monk88*/               { sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk88.wav",       nullptr },
/*Monk89*/               { sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk89.wav",       nullptr },
/*Monk91*/               { sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk91.wav",       nullptr },
/*Monk92*/               { sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk92.wav",       nullptr },
/*Monk94*/               { sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk94.wav",       nullptr },
/*Monk95*/               { sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk95.wav",       nullptr },
/*Monk96*/               { sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk96.wav",       nullptr },
/*Monk97*/               { sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk97.wav",       nullptr },
/*Monk98*/               { sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk98.wav",       nullptr },
/*Monk99*/               { sfx_STREAM | sfx_MONK,     "sfx\\monk\\monk99.wav",       nullptr },
/*Narrator1*/            { sfx_STREAM,                "sfx\\narrator\\nar01.wav",    nullptr },
/*Narrator2*/            { sfx_STREAM,                "sfx\\narrator\\nar02.wav",    nullptr },
/*Narrator3*/            { sfx_STREAM,                "sfx\\narrator\\nar03.wav",    nullptr },
/*Narrator4*/            { sfx_STREAM,                "sfx\\narrator\\nar04.wav",    nullptr },
/*Narrator5*/            { sfx_STREAM,                "sfx\\narrator\\nar05.wav",    nullptr },
/*Narrator6*/            { sfx_STREAM,                "sfx\\narrator\\nar06.wav",    nullptr },
/*Narrator7*/            { sfx_STREAM,                "sfx\\narrator\\nar07.wav",    nullptr },
/*Narrator8*/            { sfx_STREAM,                "sfx\\narrator\\nar08.wav",    nullptr },
/*Narrator9*/            { sfx_STREAM,                "sfx\\narrator\\nar09.wav",    nullptr },
/*DiabloGreeting*/       { sfx_STREAM,                "sfx\\misc\\lvl16int.wav",     nullptr },
/*ButcherGreeting*/      { sfx_STREAM,                "sfx\\monsters\\butcher.wav",  nullptr },
/*Gharbad1*/             { sfx_STREAM,                "sfx\\monsters\\garbud01.wav", nullptr },
/*Gharbad2*/             { sfx_STREAM,                "sfx\\monsters\\garbud02.wav", nullptr },
/*Gharbad3*/             { sfx_STREAM,                "sfx\\monsters\\garbud03.wav", nullptr },
/*Gharbad4*/             { sfx_STREAM,                "sfx\\monsters\\garbud04.wav", nullptr },
/*Izual*/                { sfx_STREAM,                "sfx\\monsters\\izual01.wav",  nullptr },
/*Lachdanan1*/           { sfx_STREAM,                "sfx\\monsters\\lach01.wav",   nullptr },
/*Lachdanan2*/           { sfx_STREAM,                "sfx\\monsters\\lach02.wav",   nullptr },
/*Lachdanan3*/           { sfx_STREAM,                "sfx\\monsters\\lach03.wav",   nullptr },
/*LazarusGreeting*/      { sfx_STREAM,                "sfx\\monsters\\laz01.wav",    nullptr },
/*LazarusGreetingShort*/ { sfx_STREAM,                "sfx\\monsters\\laz02.wav",    nullptr },
/*LeoricGreeting*/       { sfx_STREAM,                "sfx\\monsters\\sking01.wav",  nullptr },
/*Snotspill1*/           { sfx_STREAM,                "sfx\\monsters\\snot01.wav",   nullptr },
/*Snotspill2*/           { sfx_STREAM,                "sfx\\monsters\\snot02.wav",   nullptr },
/*Snotspill3*/           { sfx_STREAM,                "sfx\\monsters\\snot03.wav",   nullptr },
/*Warlord*/              { sfx_STREAM,                "sfx\\monsters\\warlrd01.wav", nullptr },
/*Warlock*/              { sfx_STREAM,                "sfx\\monsters\\wlock01.wav",  nullptr },
/*Zhar1*/                { sfx_STREAM,                "sfx\\monsters\\zhar01.wav",   nullptr },
/*Zhar2*/                { sfx_STREAM,                "sfx\\monsters\\zhar02.wav",   nullptr },
/*DiabloDeath*/          { sfx_STREAM,                "sfx\\monsters\\diablod.wav",  nullptr },
/*Farmer1*/              { sfx_STREAM,                "sfx\\hellfire\\farmer1.wav",  nullptr },
/*Farmer2*/              { sfx_STREAM,                "sfx\\hellfire\\farmer2.wav",  nullptr },
/*Farmer2a*/             { sfx_STREAM,                "sfx\\hellfire\\Farmer2a.wav", nullptr },
/*Farmer3*/              { sfx_STREAM,                "sfx\\hellfire\\farmer3.wav",  nullptr },
/*Farmer4*/              { sfx_STREAM,                "sfx\\hellfire\\farmer4.wav",  nullptr },
/*Farmer5*/              { sfx_STREAM,                "sfx\\hellfire\\farmer5.wav",  nullptr },
/*Farmer6*/              { sfx_STREAM,                "sfx\\hellfire\\farmer6.wav",  nullptr },
/*Farmer7*/              { sfx_STREAM,                "sfx\\hellfire\\farmer7.wav",  nullptr },
/*Farmer8*/              { sfx_STREAM,                "sfx\\hellfire\\farmer8.wav",  nullptr },
/*Farmer9*/              { sfx_STREAM,                "sfx\\hellfire\\farmer9.wav",  nullptr },
/*Celia1*/               { sfx_STREAM,                "sfx\\hellfire\\teddybr1.wav", nullptr },
/*Celia2*/               { sfx_STREAM,                "sfx\\hellfire\\teddybr2.wav", nullptr },
/*Celia3*/               { sfx_STREAM,                "sfx\\hellfire\\teddybr3.wav", nullptr },
/*Celia4*/               { sfx_STREAM,                "sfx\\hellfire\\teddybr4.wav", nullptr },
/*Defiler1*/             { sfx_STREAM,                "sfx\\hellfire\\defiler1.wav", nullptr },
/*Defiler2*/             { sfx_STREAM,                "sfx\\hellfire\\defiler2.wav", nullptr },
/*Defiler3*/             { sfx_STREAM,                "sfx\\hellfire\\defiler3.wav", nullptr },
/*Defiler4*/             { sfx_STREAM,                "sfx\\hellfire\\defiler4.wav", nullptr },
/*Defiler8*/             { sfx_STREAM,                "sfx\\hellfire\\defiler8.wav", nullptr },
/*Defiler6*/             { sfx_STREAM,                "sfx\\hellfire\\defiler6.wav", nullptr },
/*Defiler7*/             { sfx_STREAM,                "sfx\\hellfire\\defiler7.wav", nullptr },
/*NaKrul1*/              { sfx_STREAM,                "sfx\\hellfire\\nakrul1.wav",  nullptr },
/*NaKrul2*/              { sfx_STREAM,                "sfx\\hellfire\\nakrul2.wav",  nullptr },
/*NaKrul3*/              { sfx_STREAM,                "sfx\\hellfire\\nakrul3.wav",  nullptr },
/*NaKrul4*/              { sfx_STREAM,                "sfx\\hellfire\\nakrul4.wav",  nullptr },
/*NaKrul5*/              { sfx_STREAM,                "sfx\\hellfire\\nakrul5.wav",  nullptr },
/*NaKrul6*/              { sfx_STREAM,                "sfx\\hellfire\\nakrul6.wav",  nullptr },
/*NarratorHF3*/          { sfx_STREAM,                "sfx\\hellfire\\naratr3.wav",  nullptr },
/*CompleteNut1*/         { sfx_STREAM,                "sfx\\hellfire\\cowsut1.wav",  nullptr },
/*CompleteNut2*/         { sfx_STREAM,                "sfx\\hellfire\\cowsut2.wav",  nullptr },
/*CompleteNut3*/         { sfx_STREAM,                "sfx\\hellfire\\cowsut3.wav",  nullptr },
/*CompleteNut4*/         { sfx_STREAM,                "sfx\\hellfire\\cowsut4.wav",  nullptr },
/*CompleteNut4a*/        { sfx_STREAM,                "sfx\\hellfire\\cowsut4a.wav", nullptr },
/*CompleteNut5*/         { sfx_STREAM,                "sfx\\hellfire\\cowsut5.wav",  nullptr },
/*CompleteNut6*/         { sfx_STREAM,                "sfx\\hellfire\\cowsut6.wav",  nullptr },
/*CompleteNut7*/         { sfx_STREAM,                "sfx\\hellfire\\cowsut7.wav",  nullptr },
/*CompleteNut8*/         { sfx_STREAM,                "sfx\\hellfire\\cowsut8.wav",  nullptr },
/*CompleteNut9*/         { sfx_STREAM,                "sfx\\hellfire\\cowsut9.wav",  nullptr },
/*CompleteNut10*/        { sfx_STREAM,                "sfx\\hellfire\\cowsut10.wav", nullptr },
/*CompleteNut11*/        { sfx_STREAM,                "sfx\\hellfire\\cowsut11.wav", nullptr },
/*CompleteNut12*/        { sfx_STREAM,                "sfx\\hellfire\\cowsut12.wav", nullptr },
/*NarratorHF6*/          { sfx_STREAM,                "sfx\\hellfire\\naratr6.wav",  nullptr },
/*NarratorHF7*/          { sfx_STREAM,                "sfx\\hellfire\\naratr7.wav",  nullptr },
/*NarratorHF8*/          { sfx_STREAM,                "sfx\\hellfire\\naratr8.wav",  nullptr },
/*NarratorHF5*/          { sfx_STREAM,                "sfx\\hellfire\\naratr5.wav",  nullptr },
/*NarratorHF9*/          { sfx_STREAM,                "sfx\\hellfire\\naratr9.wav",  nullptr },
/*NarratorHF4*/          { sfx_STREAM,                "sfx\\hellfire\\naratr4.wav",  nullptr },
/*CryptDoorOpen*/        { sfx_MISC | sfx_HELLFIRE,   "sfx\\items\\cropen.wav",      nullptr },
/*CryptDoorClose*/       { sfx_MISC | sfx_HELLFIRE,   "sfx\\items\\crclos.wav",      nullptr },
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

SfxID RndSFX(SfxID psfx)
{
	switch (psfx) {
	case SfxID::Warrior69:
	case SfxID::Sorceror69:
	case SfxID::Rogue69:
	case SfxID::Monk69:
	case SfxID::Swing:
	case SfxID::SpellAcid:
	case SfxID::OperateShrine:
		return PickRandomlyAmong({ psfx, static_cast<SfxID>(static_cast<int16_t>(psfx) + 1) });
	case SfxID::Warrior14:
	case SfxID::Warrior15:
	case SfxID::Warrior16:
	case SfxID::Warrior2:
	case SfxID::Rogue14:
	case SfxID::Sorceror14:
	case SfxID::Monk14:
		return PickRandomlyAmong({ psfx, static_cast<SfxID>(static_cast<int16_t>(psfx) + 1), static_cast<SfxID>(static_cast<int16_t>(psfx) + 2) });
	default:
		return psfx;
	}
}

void PrivSoundInit(uint8_t bLoadMask)
{
	if (!gbSndInited) {
		return;
	}

	for (auto &sfx : sgSFX) {
		if (sfx.bFlags == 0 || sfx.pSnd != nullptr) {
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

bool effect_is_playing(SfxID nSFX)
{
	TSFX *sfx = &sgSFX[static_cast<int16_t>(nSFX)];
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

void PlaySFX(SfxID psfx)
{
	psfx = RndSFX(psfx);

	PlaySfxPriv(&sgSFX[static_cast<int16_t>(psfx)], false, { 0, 0 });
}

void PlaySfxLoc(SfxID psfx, Point position, bool randomizeByCategory)
{
	if (randomizeByCategory) {
		psfx = RndSFX(psfx);
	}

	if (IsAnyOf(psfx, SfxID::Walk, SfxID::ShootBow, SfxID::CastSpell, SfxID::Swing)) {
		TSnd *pSnd = sgSFX[static_cast<int16_t>(psfx)].pSnd.get();
		if (pSnd != nullptr)
			pSnd->start_tc = 0;
	}

	PlaySfxPriv(&sgSFX[static_cast<int16_t>(psfx)], true, position);
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

void effects_play_sound(SfxID id)
{
	if (!gbSndInited || !gbSoundOn) {
		return;
	}

	TSFX &sfx = sgSFX[static_cast<int16_t>(id)];
	if (sfx.pSnd != nullptr && !sfx.pSnd->isPlaying()) {
		snd_play_snd(sfx.pSnd.get(), 0, 0);
	}
}

int GetSFXLength(SfxID nSFX)
{
	TSFX &sfx = sgSFX[static_cast<int16_t>(nSFX)];
	if (sfx.pSnd == nullptr)
		sfx.pSnd = sound_file_load(sfx.pszName,
		    /*stream=*/AllowStreaming && (sfx.bFlags & sfx_STREAM) != 0);
	return sfx.pSnd->DSB.GetLength();
}

} // namespace devilution
