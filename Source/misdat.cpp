/**
 * @file misdat.cpp
 *
 * Implementation of data related to missiles.
 */
#include "misdat.h"

#include <cstdint>

#include "engine/load_cl2.hpp"
#include "engine/load_clx.hpp"
#include "missiles.h"
#include "mpq/mpq_common.hpp"
#include "utils/file_name_generator.hpp"
#include "utils/str_cat.hpp"

namespace devilution {

namespace {
constexpr auto Physical = MissileDataFlags::Physical;
constexpr auto Fire = MissileDataFlags::Fire;
constexpr auto Lightning = MissileDataFlags::Lightning;
constexpr auto Magic = MissileDataFlags::Magic;
constexpr auto Acid = MissileDataFlags::Acid;
constexpr auto Arrow = MissileDataFlags::Arrow;
constexpr auto Invisible = MissileDataFlags::Invisible;
} // namespace

/** Data related to each missile ID. */
const MissileData MissilesData[] = {
	// clang-format off
// id                      mAddProc,                mProc,                        mlSFX,                     miSFX,                     mFileNum,                               flags,                 MovementDistribution;
/*Arrow*/                { &AddArrow,               &ProcessArrow,                SfxID::None,               SfxID::None,               MissileGraphicID::Arrow,                Physical | Arrow,      MissileMovementDistribution::Blockable   },
/*Firebolt*/             { &AddFirebolt,            &ProcessGenericProjectile,    SfxID::SpellFirebolt,      SfxID::SpellFireHit,       MissileGraphicID::Fireball,             Fire,                  MissileMovementDistribution::Blockable   },
/*Guardian*/             { &AddGuardian,            &ProcessGuardian,             SfxID::SpellGuardian,      SfxID::None,               MissileGraphicID::Guardian,             Physical,              MissileMovementDistribution::Disabled    },
/*Phasing*/              { &AddPhasing,             &ProcessTeleport,             SfxID::SpellTeleport,      SfxID::None,               MissileGraphicID::None,                 Physical | Invisible,  MissileMovementDistribution::Disabled    },
/*NovaBall*/             { &AddNovaBall,            &ProcessNovaBall,             SfxID::None,               SfxID::None,               MissileGraphicID::Lightning,            Lightning,             MissileMovementDistribution::Unblockable },
/*FireWall*/             { &AddFireWall,            &ProcessFireWall,             SfxID::SpellFireWall,      SfxID::SpellFireHit,       MissileGraphicID::FireWall,             Fire,                  MissileMovementDistribution::Disabled    },
/*Fireball*/             { &AddFireball,            &ProcessFireball,             SfxID::SpellFirebolt,      SfxID::SpellFireHit,       MissileGraphicID::Fireball,             Fire,                  MissileMovementDistribution::Blockable   },
/*LightningControl*/     { &AddLightningControl,    &ProcessLightningControl,     SfxID::None,               SfxID::None,               MissileGraphicID::Lightning,            Lightning | Invisible, MissileMovementDistribution::Disabled    },
/*Lightning*/            { &AddLightning,           &ProcessLightning,            SfxID::SpellLightning,     SfxID::SpellLightningHit,  MissileGraphicID::Lightning,            Lightning,             MissileMovementDistribution::Disabled    },
/*MagmaBallExplosion*/   { &AddMissileExplosion,    &ProcessMissileExplosion,     SfxID::None,               SfxID::None,               MissileGraphicID::MagmaBallExplosion,   Physical,              MissileMovementDistribution::Disabled    },
/*TownPortal*/           { &AddTownPortal,          &ProcessTownPortal,           SfxID::SpellPortal,        SfxID::None,               MissileGraphicID::TownPortal,           Magic,                 MissileMovementDistribution::Disabled    },
/*FlashBottom*/          { &AddFlashBottom,         &ProcessFlashBottom,          SfxID::SpellNova,          SfxID::SpellLightningHit,  MissileGraphicID::FlashBottom,          Magic,                 MissileMovementDistribution::Disabled    },
/*FlashTop*/             { &AddFlashTop,            &ProcessFlashTop,             SfxID::None,               SfxID::None,               MissileGraphicID::FlashTop,             Magic,                 MissileMovementDistribution::Disabled    },
/*ManaShield*/           { &AddManaShield,          nullptr,                      SfxID::SpellManaShield,    SfxID::None,               MissileGraphicID::ManaShield,           Magic | Invisible,     MissileMovementDistribution::Disabled    },
/*FlameWave*/            { &AddFlameWave,           &ProcessFlameWave,            SfxID::None,               SfxID::None,               MissileGraphicID::FireWall,             Fire,                  MissileMovementDistribution::Unblockable },
/*ChainLightning*/       { &AddChainLightning,      &ProcessChainLightning,       SfxID::SpellLightning,     SfxID::SpellLightningHit,  MissileGraphicID::Lightning,            Lightning,             MissileMovementDistribution::Disabled    },
/*ChainBall*/            { nullptr,                 nullptr,                      SfxID::None,               SfxID::None,               MissileGraphicID::Lightning,            Lightning,             MissileMovementDistribution::Disabled    },
/*BloodHit*/             { nullptr,                 nullptr,                      SfxID::SpellBloodStar,     SfxID::SpellBloodStarHit,  MissileGraphicID::BloodHit,             Physical,              MissileMovementDistribution::Disabled    },
/*BoneHit*/              { nullptr,                 nullptr,                      SfxID::None,               SfxID::None,               MissileGraphicID::BoneHit,              Physical,              MissileMovementDistribution::Disabled    },
/*MetalHit*/             { nullptr,                 nullptr,                      SfxID::None,               SfxID::None,               MissileGraphicID::MetalHit,             Physical,              MissileMovementDistribution::Disabled    },
/*Rhino*/                { &AddRhino,               &ProcessRhino,                SfxID::None,               SfxID::None,               MissileGraphicID::None,                 Physical,              MissileMovementDistribution::Blockable   },
/*MagmaBall*/            { &AddMagmaBall,           &ProcessGenericProjectile,    SfxID::None,               SfxID::None,               MissileGraphicID::MagmaBall,            Fire,                  MissileMovementDistribution::Blockable   },
/*ThinLightningControl*/ { &AddLightningControl,    &ProcessLightningControl,     SfxID::None,               SfxID::None,               MissileGraphicID::ThinLightning,        Lightning | Invisible, MissileMovementDistribution::Disabled    },
/*ThinLightning*/        { &AddLightning,           &ProcessLightning,            SfxID::None,               SfxID::None,               MissileGraphicID::ThinLightning,        Lightning,             MissileMovementDistribution::Disabled    },
/*BloodStar*/            { &AddGenericMagicMissile, &ProcessGenericProjectile,    SfxID::None,               SfxID::None,               MissileGraphicID::BloodStar,            Magic,                 MissileMovementDistribution::Blockable   },
/*BloodStarExplosion*/   { &AddMissileExplosion,    &ProcessMissileExplosion,     SfxID::None,               SfxID::None,               MissileGraphicID::BloodStarExplosion,   Magic,                 MissileMovementDistribution::Disabled    },
/*Teleport*/             { &AddTeleport,            &ProcessTeleport,             SfxID::SpellElemental,     SfxID::None,               MissileGraphicID::None,                 Physical | Invisible,  MissileMovementDistribution::Disabled    },
/*FireArrow*/            { &AddElementalArrow,      &ProcessElementalArrow,       SfxID::None,               SfxID::None,               MissileGraphicID::FireArrow,            Fire | Arrow,          MissileMovementDistribution::Blockable   },
/*DoomSerpents*/         { nullptr,                 nullptr,                      SfxID::SpellDoomSerpents,  SfxID::None,               MissileGraphicID::DoomSerpents,         Magic | Invisible,     MissileMovementDistribution::Disabled    },
/*FireOnly*/             { nullptr,                 nullptr,                      SfxID::None,               SfxID::None,               MissileGraphicID::FireWall,             Fire,                  MissileMovementDistribution::Disabled    },
/*StoneCurse*/           { &AddStoneCurse,          &ProcessStoneCurse,           SfxID::SpellStoneCurse,    SfxID::None,               MissileGraphicID::None,                 Magic | Invisible,     MissileMovementDistribution::Disabled    },
/*BloodRitual*/          { nullptr,                 nullptr,                      SfxID::None,               SfxID::None,               MissileGraphicID::None,                 Physical,              MissileMovementDistribution::Disabled    },
/*Invisibility*/         { nullptr,                 nullptr,                      SfxID::SpellInvisibility,  SfxID::None,               MissileGraphicID::None,                 Physical | Invisible,  MissileMovementDistribution::Disabled    },
/*Golem*/                { &AddGolem,               nullptr,                      SfxID::SpellGolem,         SfxID::None,               MissileGraphicID::None,                 Physical | Invisible,  MissileMovementDistribution::Disabled    },
/*Etherealize*/          { nullptr,                 nullptr,                      SfxID::SpellEtherealize,   SfxID::None,               MissileGraphicID::Etherealize,          Physical,              MissileMovementDistribution::Disabled    },
/*Spurt*/                { nullptr,                 nullptr,                      SfxID::None,               SfxID::None,               MissileGraphicID::Spurt,                Physical,              MissileMovementDistribution::Disabled    },
/*ApocalypseBoom*/       { &AddApocalypseBoom,      &ProcessApocalypseBoom,       SfxID::None,               SfxID::None,               MissileGraphicID::ApocalypseBoom,       Physical,              MissileMovementDistribution::Disabled    },
/*Healing*/              { &AddHealing,             nullptr,                      SfxID::None,               SfxID::None,               MissileGraphicID::None,                 Physical | Invisible,  MissileMovementDistribution::Disabled    },
/*FireWallControl*/      { &AddFireWallControl,     &ProcessFireWallControl,      SfxID::None,               SfxID::None,               MissileGraphicID::FireWall,             Fire | Invisible,      MissileMovementDistribution::Disabled    },
/*Infravision*/          { &AddInfravision,         &ProcessInfravision,          SfxID::SpellInfravision,   SfxID::None,               MissileGraphicID::None,                 Physical | Invisible,  MissileMovementDistribution::Disabled    },
/*Identify*/             { &AddIdentify,            nullptr,                      SfxID::None,               SfxID::None,               MissileGraphicID::None,                 Physical | Invisible,  MissileMovementDistribution::Disabled    },
/*FlameWaveControl*/     { &AddFlameWaveControl,    &ProcessFlameWaveControl,     SfxID::SpellFlameWave,     SfxID::None,               MissileGraphicID::FireWall,             Fire,                  MissileMovementDistribution::Disabled    },
/*Nova*/                 { &AddNova,                &ProcessNova,                 SfxID::SpellNova,          SfxID::None,               MissileGraphicID::Lightning,            Lightning,             MissileMovementDistribution::Disabled    },
/*Rage*/                 { &AddRage,                &ProcessRage,                 SfxID::None,               SfxID::None,               MissileGraphicID::None,                 Physical | Invisible,  MissileMovementDistribution::Disabled    },
/*Apocalypse*/           { &AddApocalypse,          &ProcessApocalypse,           SfxID::SpellApocalypse,    SfxID::None,               MissileGraphicID::ApocalypseBoom,       Magic,                 MissileMovementDistribution::Disabled    },
/*ItemRepair*/           { &AddItemRepair,          nullptr,                      SfxID::None,               SfxID::None,               MissileGraphicID::None,                 Physical | Invisible,  MissileMovementDistribution::Disabled    },
/*StaffRecharge*/        { &AddStaffRecharge,       nullptr,                      SfxID::None,               SfxID::None,               MissileGraphicID::None,                 Physical | Invisible,  MissileMovementDistribution::Disabled    },
/*TrapDisarm*/           { &AddTrapDisarm,          nullptr,                      SfxID::SpellTrapDisarm,    SfxID::None,               MissileGraphicID::None,                 Physical | Invisible,  MissileMovementDistribution::Disabled    },
/*Inferno*/              { &AddInferno,             &ProcessInferno,              SfxID::SpellInferno,       SfxID::None,               MissileGraphicID::Inferno,              Fire,                  MissileMovementDistribution::Disabled    },
/*InfernoControl*/       { &AddInfernoControl,      &ProcessInfernoControl,       SfxID::None,               SfxID::None,               MissileGraphicID::None,                 Fire | Invisible,      MissileMovementDistribution::Disabled    },
/*FireMan*/              { nullptr,                 nullptr,                      SfxID::None,               SfxID::None,               MissileGraphicID::None,                 Physical,              MissileMovementDistribution::Blockable   },
/*Krull*/                { nullptr,                 nullptr,                      SfxID::None,               SfxID::None,               MissileGraphicID::Krull,                Fire | Arrow,          MissileMovementDistribution::Blockable   },
/*ChargedBolt*/          { &AddChargedBolt,         &ProcessChargedBolt,          SfxID::SpellChargedBolt,   SfxID::None,               MissileGraphicID::ChargedBolt,          Lightning,             MissileMovementDistribution::Blockable   },
/*HolyBolt*/             { &AddHolyBolt,            &ProcessHolyBolt,             SfxID::SpellHolyBolt,      SfxID::SpellLightningHit,  MissileGraphicID::HolyBolt,             Physical,              MissileMovementDistribution::Blockable   },
/*Resurrect*/            { &AddResurrect,           nullptr,                      SfxID::None,               SfxID::SpellResurrect,     MissileGraphicID::None,                 Magic | Invisible,     MissileMovementDistribution::Disabled    },
/*Telekinesis*/          { &AddTelekinesis,         nullptr,                      SfxID::SpellEtherealize,   SfxID::None,               MissileGraphicID::None,                 Physical | Invisible,  MissileMovementDistribution::Disabled    },
/*LightningArrow*/       { &AddElementalArrow,      &ProcessElementalArrow,       SfxID::None,               SfxID::None,               MissileGraphicID::LightningArrow,       Lightning | Arrow,     MissileMovementDistribution::Blockable   },
/*Acid*/                 { &AddAcid,                &ProcessGenericProjectile,    SfxID::SpellAcid,          SfxID::None,               MissileGraphicID::Acid,                 Acid,                  MissileMovementDistribution::Blockable   },
/*AcidSplat*/            { &AddMissileExplosion,    &ProcessAcidSplate,           SfxID::None,               SfxID::None,               MissileGraphicID::AcidSplat,            Acid,                  MissileMovementDistribution::Disabled    },
/*AcidPuddle*/           { &AddAcidPuddle,          &ProcessAcidPuddle,           SfxID::SpellPuddle,        SfxID::None,               MissileGraphicID::AcidPuddle,           Acid,                  MissileMovementDistribution::Disabled    },
/*HealOther*/            { &AddHealOther,           nullptr,                      SfxID::None,               SfxID::None,               MissileGraphicID::None,                 Physical | Invisible,  MissileMovementDistribution::Disabled    },
/*Elemental*/            { &AddElemental,           &ProcessElemental,            SfxID::SpellElemental,     SfxID::None,               MissileGraphicID::Elemental,            Fire,                  MissileMovementDistribution::Unblockable },
/*ResurrectBeam*/        { &AddResurrectBeam,       &ProcessResurrectBeam,        SfxID::None,               SfxID::None,               MissileGraphicID::Resurrect,            Physical,              MissileMovementDistribution::Disabled    },
/*BoneSpirit*/           { &AddBoneSpirit,          &ProcessBoneSpirit,           SfxID::SpellBoneSpirit,    SfxID::SpellBoneSpiritHit, MissileGraphicID::BoneSpirit,           Magic,                 MissileMovementDistribution::Blockable   },
/*WeaponExplosion*/      { &AddWeaponExplosion,     &ProcessWeaponExplosion,      SfxID::None,               SfxID::None,               MissileGraphicID::None,                 Physical,              MissileMovementDistribution::Disabled    },
/*RedPortal*/            { &AddRedPortal,           &ProcessRedPortal,            SfxID::SpellPortal,        SfxID::None,               MissileGraphicID::RedPortal,            Physical,              MissileMovementDistribution::Disabled    },
/*DiabloApocalypseBoom*/ { &AddApocalypseBoom,      &ProcessApocalypseBoom,       SfxID::None,               SfxID::None,               MissileGraphicID::DiabloApocalypseBoom, Physical,              MissileMovementDistribution::Disabled    },
/*DiabloApocalypse*/     { &AddDiabloApocalypse,    nullptr,                      SfxID::None,               SfxID::None,               MissileGraphicID::None,                 Physical | Invisible,  MissileMovementDistribution::Disabled    },
/*Mana*/                 { &AddMana,                nullptr,                      SfxID::None,               SfxID::None,               MissileGraphicID::None,                 Physical | Invisible,  MissileMovementDistribution::Disabled    },
/*Magi*/                 { &AddMagi,                nullptr,                      SfxID::None,               SfxID::None,               MissileGraphicID::None,                 Physical | Invisible,  MissileMovementDistribution::Disabled    },
/*LightningWall*/        { &AddLightningWall,       &ProcessLightningWall,        SfxID::SpellLightningWall, SfxID::SpellLightningHit,  MissileGraphicID::Lightning,            Lightning,             MissileMovementDistribution::Disabled    },
/*LightningWallControl*/ { &AddFireWallControl,     &ProcessLightningWallControl, SfxID::None,               SfxID::None,               MissileGraphicID::Lightning,            Lightning | Invisible, MissileMovementDistribution::Disabled    },
/*Immolation*/           { &AddNova,                &ProcessImmolation,           SfxID::SpellFirebolt,      SfxID::SpellFireHit,       MissileGraphicID::Fireball,             Fire,                  MissileMovementDistribution::Disabled    },
/*SpectralArrow*/        { &AddSpectralArrow,       &ProcessSpectralArrow,        SfxID::None,               SfxID::None,               MissileGraphicID::Arrow,                Physical | Arrow,      MissileMovementDistribution::Disabled    },
/*FireballBow*/          { &AddImmolation,          &ProcessFireball,             SfxID::ShootFireballBow,   SfxID::SpellFireHit,       MissileGraphicID::Fireball,             Fire,                  MissileMovementDistribution::Blockable   },
/*LightningBow*/         { &AddLightningBow,        &ProcessLightningBow,         SfxID::ShootFireballBow,   SfxID::None,               MissileGraphicID::Lightning,            Lightning | Invisible, MissileMovementDistribution::Disabled    },
/*ChargedBoltBow*/       { &AddChargedBoltBow,      &ProcessChargedBolt,          SfxID::SpellChargedBolt,   SfxID::None,               MissileGraphicID::ChargedBolt,          Lightning,             MissileMovementDistribution::Blockable   },
/*HolyBoltBow*/          { &AddHolyBolt,            &ProcessHolyBolt,             SfxID::SpellHolyBolt,      SfxID::SpellLightningHit,  MissileGraphicID::HolyBolt,             Physical,              MissileMovementDistribution::Blockable   },
/*Warp*/                 { &AddWarp,                &ProcessTeleport,             SfxID::SpellEtherealize,   SfxID::None,               MissileGraphicID::None,                 Physical | Invisible,  MissileMovementDistribution::Disabled    },
/*Reflect*/              { &AddReflect,             nullptr,                      SfxID::SpellManaShield,    SfxID::None,               MissileGraphicID::Reflect,              Physical | Invisible,  MissileMovementDistribution::Disabled    },
/*Berserk*/              { &AddBerserk,             nullptr,                      SfxID::None,               SfxID::None,               MissileGraphicID::None,                 Physical | Invisible,  MissileMovementDistribution::Disabled    },
/*RingOfFire*/           { &AddRingOfFire,          &ProcessRingOfFire,           SfxID::None,               SfxID::None,               MissileGraphicID::FireWall,             Fire | Invisible,      MissileMovementDistribution::Disabled    },
/*StealPotions*/         { &AddStealPotions,        nullptr,                      SfxID::None,               SfxID::None,               MissileGraphicID::None,                 Physical | Invisible,  MissileMovementDistribution::Disabled    },
/*StealMana*/            { &AddStealMana,           nullptr,                      SfxID::SpellEnd,           SfxID::None,               MissileGraphicID::None,                 Physical | Invisible,  MissileMovementDistribution::Disabled    },
/*RingOfLightning*/      { nullptr,                 nullptr,                      SfxID::None,               SfxID::None,               MissileGraphicID::Lightning,            Lightning | Invisible, MissileMovementDistribution::Disabled    },
/*Search*/               { &AddSearch,              &ProcessSearch,               SfxID::None,               SfxID::None,               MissileGraphicID::None,                 Physical | Invisible,  MissileMovementDistribution::Disabled    },
/*Aura*/                 { nullptr,                 nullptr,                      SfxID::None,               SfxID::SpellLightningHit,  MissileGraphicID::FlashBottom,          Magic | Invisible,     MissileMovementDistribution::Disabled    },
/*Aura2*/                { nullptr,                 nullptr,                      SfxID::None,               SfxID::None,               MissileGraphicID::FlashTop,             Magic | Invisible,     MissileMovementDistribution::Disabled    },
/*SpiralFireball*/       { nullptr,                 nullptr,                      SfxID::SpellFirebolt,      SfxID::SpellFireHit,       MissileGraphicID::Fireball,             Fire,                  MissileMovementDistribution::Disabled    },
/*RuneOfFire*/           { &AddRuneOfFire,          &ProcessRune,                 SfxID::None,               SfxID::None,               MissileGraphicID::Rune,                 Physical,              MissileMovementDistribution::Disabled    },
/*RuneOfLight*/          { &AddRuneOfLight,         &ProcessRune,                 SfxID::None,               SfxID::None,               MissileGraphicID::Rune,                 Physical,              MissileMovementDistribution::Disabled    },
/*RuneOfNova*/           { &AddRuneOfNova,          &ProcessRune,                 SfxID::None,               SfxID::None,               MissileGraphicID::Rune,                 Physical,              MissileMovementDistribution::Disabled    },
/*RuneOfImmolation*/     { &AddRuneOfImmolation,    &ProcessRune,                 SfxID::None,               SfxID::None,               MissileGraphicID::Rune,                 Physical,              MissileMovementDistribution::Disabled    },
/*RuneOfStone*/          { &AddRuneOfStone,         &ProcessRune,                 SfxID::None,               SfxID::None,               MissileGraphicID::Rune,                 Physical,              MissileMovementDistribution::Disabled    },
/*BigExplosion*/         { &AddBigExplosion,        &ProcessBigExplosion,         SfxID::BigExplosion,       SfxID::BigExplosion,       MissileGraphicID::BigExplosion,         Fire,                  MissileMovementDistribution::Disabled    },
/*HorkSpawn*/            { &AddHorkSpawn,           &ProcessHorkSpawn,            SfxID::None,               SfxID::None,               MissileGraphicID::None,                 Physical | Invisible,  MissileMovementDistribution::Disabled    },
/*Jester*/               { &AddJester,              nullptr,                      SfxID::None,               SfxID::None,               MissileGraphicID::None,                 Physical | Invisible,  MissileMovementDistribution::Disabled    },
/*OpenNest*/             { &AddOpenNest,            nullptr,                      SfxID::None,               SfxID::None,               MissileGraphicID::None,                 Physical | Invisible,  MissileMovementDistribution::Disabled    },
/*OrangeFlare*/          { &AddGenericMagicMissile, &ProcessGenericProjectile,    SfxID::None,               SfxID::None,               MissileGraphicID::OrangeFlare,          Magic,                 MissileMovementDistribution::Blockable   },
/*BlueFlare*/            { &AddGenericMagicMissile, &ProcessGenericProjectile,    SfxID::None,               SfxID::None,               MissileGraphicID::BlueFlare2,           Magic,                 MissileMovementDistribution::Blockable   },
/*RedFlare*/             { &AddGenericMagicMissile, &ProcessGenericProjectile,    SfxID::None,               SfxID::None,               MissileGraphicID::RedFlare,             Magic,                 MissileMovementDistribution::Blockable   },
/*YellowFlare*/          { &AddGenericMagicMissile, &ProcessGenericProjectile,    SfxID::None,               SfxID::None,               MissileGraphicID::YellowFlare,          Magic,                 MissileMovementDistribution::Blockable   },
/*BlueFlare2*/           { &AddGenericMagicMissile, &ProcessGenericProjectile,    SfxID::None,               SfxID::None,               MissileGraphicID::BlueFlare2,           Magic,                 MissileMovementDistribution::Blockable   },
/*YellowExplosion*/      { &AddMissileExplosion,    &ProcessMissileExplosion,     SfxID::SpellFireHit,       SfxID::None,               MissileGraphicID::YellowFlareExplosion, Physical,              MissileMovementDistribution::Disabled    },
/*RedExplosion*/         { &AddMissileExplosion,    &ProcessMissileExplosion,     SfxID::SpellFireHit,       SfxID::None,               MissileGraphicID::RedFlareExplosion,    Physical,              MissileMovementDistribution::Disabled    },
/*BlueExplosion*/        { &AddMissileExplosion,    &ProcessMissileExplosion,     SfxID::SpellFireHit,       SfxID::None,               MissileGraphicID::BlueFlareExplosion,   Physical,              MissileMovementDistribution::Disabled    },
/*BlueExplosion2*/       { &AddMissileExplosion,    &ProcessMissileExplosion,     SfxID::SpellFireHit,       SfxID::None,               MissileGraphicID::BlueFlareExplosion2,  Physical,              MissileMovementDistribution::Disabled    },
/*OrangeExplosion*/      { &AddMissileExplosion,    &ProcessMissileExplosion,     SfxID::SpellFireHit,       SfxID::None,               MissileGraphicID::OrangeFlareExplosion, Physical,              MissileMovementDistribution::Disabled    },
	// clang-format on
};

namespace {

constexpr std::array<uint8_t, 16> Repeat(uint8_t v) // NOLINT(readability-identifier-length)
{
	return { v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v };
}

const std::array<uint8_t, 16> MissileAnimDelays[] {
	{},
	Repeat(1),
	Repeat(2),
	{ 0, 1 },
	{ 1 },
};

const std::array<uint8_t, 16> MissileAnimLengths[] {
	{},
	Repeat(1),
	Repeat(4),
	Repeat(6),
	Repeat(7),
	Repeat(8),
	Repeat(9),
	Repeat(10),
	Repeat(12),
	Repeat(13),
	Repeat(14),
	Repeat(15),
	Repeat(16),
	Repeat(17),
	Repeat(19),
	Repeat(20),
	{ 9, 4 },
	{ 15, 14, 3 },
	{ 13, 11 },
	{ 16, 16, 16, 16, 16, 16, 16, 16, 8 }
};

constexpr uint8_t AnimLen_0 = 0;        // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_1 = 1;        // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_4 = 2;        // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_6 = 3;        // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_7 = 4;        // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_8 = 5;        // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_9 = 6;        // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_10 = 7;       // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_12 = 8;       // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_13 = 9;       // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_14 = 10;      // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_15 = 11;      // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_16 = 12;      // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_17 = 13;      // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_19 = 14;      // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_20 = 15;      // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_9_4 = 16;     // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_15_14_3 = 17; // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_13_11 = 18;   // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_16x8_8 = 19;  // NOLINT(readability-identifier-naming)

} // namespace

/** Data related to each missile graphic ID. */
MissileFileData MissileSpriteData[] = {
	// clang-format off
// id                          sprites,   animWidth,  animWidth2, name,        animFAmt, flags,                               animDelayIdx, animLenIdx
/*Arrow*/                    { {},               96,          16, "arrows",           1, MissileGraphicsFlags::NotAnimated,              0, AnimLen_16      },
/*Fireball*/                 { {},               96,          16, "fireba",          16, MissileGraphicsFlags::None,                     0, AnimLen_14      },
/*Guardian*/                 { {},               96,          16, "guard",            3, MissileGraphicsFlags::None,                     1, AnimLen_15_14_3 },
/*Lightning*/                { {},               96,          16, "lghning",          1, MissileGraphicsFlags::None,                     0, AnimLen_8       },
/*FireWall*/                 { {},              128,          32, "firewal",          2, MissileGraphicsFlags::None,                     0, AnimLen_13_11   },
/*MagmaBallExplosion*/       { {},              128,          32, "magblos",          1, MissileGraphicsFlags::None,                     1, AnimLen_10      },
/*TownPortal*/               { {},               96,          16, "portal",           2, MissileGraphicsFlags::None,                     3, AnimLen_16      },
/*FlashBottom*/              { {},              160,          48, "bluexfr",          1, MissileGraphicsFlags::None,                     0, AnimLen_19      },
/*FlashTop*/                 { {},              160,          48, "bluexbk",          1, MissileGraphicsFlags::None,                     0, AnimLen_19      },
/*ManaShield*/               { {},               96,          16, "manashld",         1, MissileGraphicsFlags::NotAnimated,              0, AnimLen_1       },
/*BloodHit*/                 { {},               96,          16, {},                 4, MissileGraphicsFlags::None,                     0, AnimLen_15      },
/*BoneHit*/                  { {},              128,          32, {},                 3, MissileGraphicsFlags::None,                     2, AnimLen_8       },
/*MetalHit*/                 { {},               96,          16, {},                 3, MissileGraphicsFlags::None,                     2, AnimLen_10      },
/*FireArrow*/                { {},               96,          16, "farrow",          16, MissileGraphicsFlags::None,                     0, AnimLen_4       },
/*DoomSerpents*/             { {},               96,          16, "doom",             9, MissileGraphicsFlags::MonsterOwned,             1, AnimLen_15      },
/*Golem*/                    { {},                0,           0, {},                 1, MissileGraphicsFlags::MonsterOwned,             0, AnimLen_0       },
/*Spurt*/                    { {},              128,          32, {},                 2, MissileGraphicsFlags::None,                     2, AnimLen_8       },
/*ApocalypseBoom*/           { {},               96,          16, "newexp",           1, MissileGraphicsFlags::None,                     1, AnimLen_15      },
/*StoneCurseShatter*/        { {},              128,          32, "shatter1",         1, MissileGraphicsFlags::None,                     1, AnimLen_12      },
/*BigExplosion*/             { {},              160,          48, "bigexp",           1, MissileGraphicsFlags::None,                     0, AnimLen_15      },
/*Inferno*/                  { {},               96,          16, "inferno",          1, MissileGraphicsFlags::None,                     0, AnimLen_20      },
/*ThinLightning*/            { {},               96,          16, "thinlght",         1, MissileGraphicsFlags::MonsterOwned,             0, AnimLen_8       },
/*BloodStar*/                { {},              128,          32, "flare",            1, MissileGraphicsFlags::None,                     0, AnimLen_16      },
/*BloodStarExplosion*/       { {},              128,          32, "flareexp",         1, MissileGraphicsFlags::None,                     0, AnimLen_7       },
/*MagmaBall*/                { {},              128,          32, "magball",          8, MissileGraphicsFlags::MonsterOwned,             1, AnimLen_16      },
/*Krull*/                    { {},               96,          16, "krull",            1, MissileGraphicsFlags::MonsterOwned,             0, AnimLen_14      },
/*ChargedBolt*/              { {},               64,           0, "miniltng",         1, MissileGraphicsFlags::None,                     1, AnimLen_8       },
/*HolyBolt*/                 { {},               96,          16, "holy",            16, MissileGraphicsFlags::None,                     4, AnimLen_14      },
/*HolyBoltExplosion*/        { {},              160,          48, "holyexpl",         1, MissileGraphicsFlags::None,                     0, AnimLen_8       },
/*LightningArrow*/           { {},               96,          16, "larrow",          16, MissileGraphicsFlags::None,                     0, AnimLen_4       },
/*FireArrowExplosion*/       { {},               64,           0, {},                 1, MissileGraphicsFlags::None,                     0, AnimLen_6       },
/*Acid*/                     { {},               96,          16, "acidbf",          16, MissileGraphicsFlags::MonsterOwned,             0, AnimLen_8       },
/*AcidSplat*/                { {},               96,          16, "acidspla",         1, MissileGraphicsFlags::MonsterOwned,             0, AnimLen_8       },
/*AcidPuddle*/               { {},               96,          16, "acidpud",          2, MissileGraphicsFlags::MonsterOwned,             0, AnimLen_9_4     },
/*Etherealize*/              { {},               96,          16, {},                 1, MissileGraphicsFlags::None,                     0, AnimLen_1       },
/*Elemental*/                { {},               96,          16, "firerun",          8, MissileGraphicsFlags::None,                     1, AnimLen_12      },
/*Resurrect*/                { {},               96,          16, "ressur1",          1, MissileGraphicsFlags::None,                     0, AnimLen_16      },
/*BoneSpirit*/               { {},               96,          16, "sklball",          9, MissileGraphicsFlags::None,                     1, AnimLen_16x8_8  },
/*RedPortal*/                { {},               96,          16, "rportal",          2, MissileGraphicsFlags::None,                     0, AnimLen_16      },
/*DiabloApocalypseBoom*/     { {},              160,          48, "fireplar",         1, MissileGraphicsFlags::MonsterOwned,             1, AnimLen_17      },
/*BloodStarBlue*/            { {},               96,          16, "scubmisb",         1, MissileGraphicsFlags::MonsterOwned,             0, AnimLen_16      },
/*BloodStarBlueExplosion*/   { {},              128,          32, "scbsexpb",         1, MissileGraphicsFlags::MonsterOwned,             0, AnimLen_6       },
/*BloodStarYellow*/          { {},               96,          16, "scubmisc",         1, MissileGraphicsFlags::MonsterOwned,             0, AnimLen_16      },
/*BloodStarYellowExplosion*/ { {},              128,          32, "scbsexpc",         1, MissileGraphicsFlags::MonsterOwned,             0, AnimLen_6       },
/*BloodStarRed*/             { {},               96,          16, "scubmisd",         1, MissileGraphicsFlags::MonsterOwned,             0, AnimLen_16      },
/*BloodStarRedExplosion*/    { {},              128,          32, "scbsexpd",         1, MissileGraphicsFlags::MonsterOwned,             0, AnimLen_6       },
/*HorkSpawn*/                { {},               96,          16, "spawns",           8, MissileGraphicsFlags::MonsterOwned,             0, AnimLen_9       },
/*Reflect*/                  { {},              160,          64, "reflect",          1, MissileGraphicsFlags::NotAnimated,              0, AnimLen_1       },
/*OrangeFlare*/              { {},               96,           8, "ms_ora",          16, MissileGraphicsFlags::MonsterOwned,             0, AnimLen_15      },
/*BlueFlare*/                { {},               96,           8, "ms_bla",          16, MissileGraphicsFlags::MonsterOwned,             0, AnimLen_15      },
/*RedFlare*/                 { {},               96,           8, "ms_reb",          16, MissileGraphicsFlags::MonsterOwned,             0, AnimLen_15      },
/*YellowFlare*/              { {},               96,           8, "ms_yeb",          16, MissileGraphicsFlags::MonsterOwned,             0, AnimLen_15      },
/*Rune*/                     { {},               96,           8, "rglows1",          1, MissileGraphicsFlags::None,                     0, AnimLen_10      },
/*YellowFlareExplosion*/     { {},              220,          78, "ex_yel2",          1, MissileGraphicsFlags::MonsterOwned,             0, AnimLen_10      },
/*BlueFlareExplosion*/       { {},              212,          86, "ex_blu2",          1, MissileGraphicsFlags::MonsterOwned,             0, AnimLen_10      },
/*RedFlareExplosion*/        { {},              292,         114, "ex_red3",          1, MissileGraphicsFlags::MonsterOwned,             0, AnimLen_7       },
/*BlueFlare2*/               { {},               96,           8, "ms_blb",          16, MissileGraphicsFlags::MonsterOwned,             0, AnimLen_15      },
/*OrangeFlareExplosion*/     { {},               96,         -12, "ex_ora1",          1, MissileGraphicsFlags::MonsterOwned,             0, AnimLen_13      },
/*BlueFlareExplosion2*/      { {},              292,         114, "ex_blu3",          1, MissileGraphicsFlags::MonsterOwned,             0, AnimLen_7       },
/*None*/                     { {},                0,           0, {},                 0, MissileGraphicsFlags::None,                     0, 0               },
	// clang-format on
};

uint8_t MissileFileData::animDelay(uint8_t dir) const
{
	return MissileAnimDelays[animDelayIdx][dir];
}

uint8_t MissileFileData::animLen(uint8_t dir) const
{
	return MissileAnimLengths[animLenIdx][dir];
}

void MissileFileData::LoadGFX()
{
	if (sprites)
		return;

	if (name[0] == '\0')
		return;

#ifdef UNPACKED_MPQS
	char path[MaxMpqPathSize];
	*BufCopy(path, "missiles\\", name, ".clx") = '\0';
	sprites.emplace(LoadClxListOrSheet(path));
#else
	if (animFAmt == 1) {
		char path[MaxMpqPathSize];
		*BufCopy(path, "missiles\\", name) = '\0';
		sprites.emplace(OwnedClxSpriteListOrSheet { LoadCl2(path, animWidth) });
	} else {
		FileNameGenerator pathGenerator({ "missiles\\", name }, DEVILUTIONX_CL2_EXT);
		sprites.emplace(OwnedClxSpriteListOrSheet { LoadMultipleCl2Sheet<16>(pathGenerator, animFAmt, animWidth) });
	}
#endif
}

void InitMissileGFX(bool loadHellfireGraphics)
{
	if (HeadlessMode)
		return;

	for (size_t mi = 0; MissileSpriteData[mi].animFAmt != 0; mi++) {
		if (!loadHellfireGraphics && mi >= static_cast<uint8_t>(MissileGraphicID::HorkSpawn))
			break;
		if (MissileSpriteData[mi].flags == MissileGraphicsFlags::MonsterOwned)
			continue;
		MissileSpriteData[mi].LoadGFX();
	}
}

void FreeMissileGFX()
{
	for (auto &missileData : MissileSpriteData) {
		missileData.FreeGFX();
	}
}

} // namespace devilution
