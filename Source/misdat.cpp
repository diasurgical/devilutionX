/**
 * @file misdat.cpp
 *
 * Implementation of data related to missiles.
 */
#include "misdat.h"

#include <array>
#include <cstdint>
#include <vector>

#include <expected.hpp>

#include "data/file.hpp"
#include "data/iterators.hpp"
#include "data/record_reader.hpp"
#include "missiles.h"
#include "mpq/mpq_common.hpp"
#include "utils/file_name_generator.hpp"
#include "utils/str_cat.hpp"

#ifdef UNPACKED_MPQS
#include "engine/load_clx.hpp"
#else
#include "engine/load_cl2.hpp"
#endif

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

/** Data related to each missile graphic ID. */
std::vector<MissileFileData> MissileSpriteData;
std::vector<std::array<uint8_t, 16>> MissileAnimDelays;
std::vector<std::array<uint8_t, 16>> MissileAnimLengths;

size_t ToIndex(std::vector<std::array<uint8_t, 16>> &all, const std::array<uint8_t, 16> &value)
{
	for (size_t i = 0; i < all.size(); ++i) {
		if (all[i] == value) return i;
	}
	all.push_back(value);
	return all.size() - 1;
}

tl::expected<MissileGraphicsFlags, std::string> ParseMissileGraphicsFlag(std::string_view value)
{
	if (value.empty()) return MissileGraphicsFlags::None;
	if (value == "MonsterOwned") return MissileGraphicsFlags::MonsterOwned;
	if (value == "NotAnimated") return MissileGraphicsFlags::NotAnimated;
	return tl::make_unexpected("Unknown enum value");
}

void LoadMissileSpriteData()
{
	const std::string_view filename = "txtdata\\missiles\\missile_sprites.tsv";
	DataFile dataFile = DataFile::loadOrDie(filename);
	dataFile.skipHeaderOrDie(filename);

	MissileAnimDelays.clear();
	MissileAnimLengths.clear();
	MissileSpriteData.clear();
	MissileSpriteData.reserve(dataFile.numRecords());

	for (DataFileRecord record : dataFile) {
		RecordReader reader { record, filename };
		MissileFileData &item = MissileSpriteData.emplace_back();
		reader.advance(); // skip id
		reader.readInt("width", item.animWidth);
		reader.readInt("width2", item.animWidth2);
		reader.readString("name", item.name);
		reader.readInt("numFrames", item.animFAmt);
		reader.read("flags", item.flags, ParseMissileGraphicsFlag);

		std::array<uint8_t, 16> arr;
		reader.readIntArray("frameDelay", arr);
		item.animDelayIdx = static_cast<uint8_t>(ToIndex(MissileAnimDelays, arr));

		reader.readIntArray("frameLength", arr);
		item.animLenIdx = static_cast<uint8_t>(ToIndex(MissileAnimLengths, arr));
	}

	MissileSpriteData.shrink_to_fit();
	MissileAnimDelays.shrink_to_fit();
	MissileAnimLengths.shrink_to_fit();
}

} // namespace

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

MissileFileData &GetMissileSpriteData(MissileGraphicID graphicId)
{
	return MissileSpriteData[static_cast<std::underlying_type_t<MissileGraphicID>>(graphicId)];
}

void LoadMissileData()
{
	LoadMissileSpriteData();
}

void InitMissileGFX(bool loadHellfireGraphics)
{
	if (HeadlessMode)
		return;

	for (size_t mi = 0; mi < MissileSpriteData.size(); ++mi) {
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
