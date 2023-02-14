/**
 * @file misdat.cpp
 *
 * Implementation of data related to missiles.
 */
#include "misdat.h"

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
MissileData MissilesData[] = {
	// clang-format off
// id                      mAddProc,                mProc,                        mlSFX,       miSFX,       mFileNum,                               flags,                 MovementDistribution;
/*Arrow*/                { &AddArrow,               &ProcessArrow,                SFX_NONE,    SFX_NONE,    MissileGraphicID::Arrow,                Physical | Arrow,      MissileMovementDistribution::Blockable   },
/*Firebolt*/             { &AddFirebolt,            &ProcessGenericProjectile,    LS_FBOLT1,   LS_FIRIMP2,  MissileGraphicID::Fireball,             Fire,                  MissileMovementDistribution::Blockable   },
/*Guardian*/             { &AddGuardian,            &ProcessGuardian,             LS_GUARD,    LS_GUARDLAN, MissileGraphicID::Guardian,             Physical,              MissileMovementDistribution::Disabled    },
/*Phasing*/              { &AddPhasing,             &ProcessTeleport,             LS_TELEPORT, SFX_NONE,    MissileGraphicID::None,                 Physical | Invisible,  MissileMovementDistribution::Disabled    },
/*NovaBall*/             { &AddNovaBall,            &ProcessNovaBall,             SFX_NONE,    SFX_NONE,    MissileGraphicID::Lightning,            Lightning,             MissileMovementDistribution::Unblockable },
/*FireWall*/             { &AddFireWall,            &ProcessFireWall,             LS_WALLLOOP, LS_FIRIMP2,  MissileGraphicID::FireWall,             Fire,                  MissileMovementDistribution::Disabled    },
/*Fireball*/             { &AddFireball,            &ProcessFireball,             LS_FBOLT1,   LS_FIRIMP2,  MissileGraphicID::Fireball,             Fire,                  MissileMovementDistribution::Blockable   },
/*LightningControl*/     { &AddLightningControl,    &ProcessLightningControl,     SFX_NONE,    SFX_NONE,    MissileGraphicID::Lightning,            Lightning | Invisible, MissileMovementDistribution::Disabled    },
/*Lightning*/            { &AddLightning,           &ProcessLightning,            LS_LNING1,   LS_ELECIMP1, MissileGraphicID::Lightning,            Lightning,             MissileMovementDistribution::Disabled    },
/*MagmaBallExplosion*/   { &AddMissileExplosion,    &ProcessMissileExplosion,     SFX_NONE,    SFX_NONE,    MissileGraphicID::MagmaBallExplosion,   Physical,              MissileMovementDistribution::Disabled    },
/*TownPortal*/           { &AddTownPortal,          &ProcessTownPortal,           LS_SENTINEL, LS_ELEMENTL, MissileGraphicID::TownPortal,           Magic,                 MissileMovementDistribution::Disabled    },
/*FlashBottom*/          { &AddFlashBottom,         &ProcessFlashBottom,          LS_NOVA,     LS_ELECIMP1, MissileGraphicID::FlashBottom,          Magic,                 MissileMovementDistribution::Disabled    },
/*FlashTop*/             { &AddFlashTop,            &ProcessFlashTop,             SFX_NONE,    SFX_NONE,    MissileGraphicID::FlashTop,             Magic,                 MissileMovementDistribution::Disabled    },
/*ManaShield*/           { &AddManaShield,          nullptr,                      LS_MSHIELD,  SFX_NONE,    MissileGraphicID::ManaShield,           Magic | Invisible,     MissileMovementDistribution::Disabled    },
/*FlameWave*/            { &AddFlameWave,           &ProcessFlameWave,            SFX_NONE,    SFX_NONE,    MissileGraphicID::FireWall,             Fire,                  MissileMovementDistribution::Unblockable },
/*ChainLightning*/       { &AddChainLightning,      &ProcessChainLightning,       LS_LNING1,   LS_ELECIMP1, MissileGraphicID::Lightning,            Lightning,             MissileMovementDistribution::Disabled    },
/*ChainBall*/            { nullptr,                 nullptr,                      SFX_NONE,    SFX_NONE,    MissileGraphicID::Lightning,            Lightning,             MissileMovementDistribution::Disabled    },
/*BloodHit*/             { nullptr,                 nullptr,                      LS_BLODSTAR, LS_BLSIMPT,  MissileGraphicID::BloodHit,             Physical,              MissileMovementDistribution::Disabled    },
/*BoneHit*/              { nullptr,                 nullptr,                      SFX_NONE,    SFX_NONE,    MissileGraphicID::BoneHit,              Physical,              MissileMovementDistribution::Disabled    },
/*MetalHit*/             { nullptr,                 nullptr,                      SFX_NONE,    SFX_NONE,    MissileGraphicID::MetalHit,             Physical,              MissileMovementDistribution::Disabled    },
/*Rhino*/                { &AddRhino,               &ProcessRhino,                SFX_NONE,    SFX_NONE,    MissileGraphicID::None,                 Physical,              MissileMovementDistribution::Blockable   },
/*MagmaBall*/            { &AddMagmaBall,           &ProcessGenericProjectile,    SFX_NONE,    SFX_NONE,    MissileGraphicID::MagmaBall,            Fire,                  MissileMovementDistribution::Blockable   },
/*ThinLightningControl*/ { &AddLightningControl,    &ProcessLightningControl,     SFX_NONE,    SFX_NONE,    MissileGraphicID::ThinLightning,        Lightning | Invisible, MissileMovementDistribution::Disabled    },
/*ThinLightning*/        { &AddLightning,           &ProcessLightning,            SFX_NONE,    SFX_NONE,    MissileGraphicID::ThinLightning,        Lightning,             MissileMovementDistribution::Disabled    },
/*BloodStar*/            { &AddGenericMagicMissile, &ProcessGenericProjectile,    SFX_NONE,    SFX_NONE,    MissileGraphicID::BloodStar,            Magic,                 MissileMovementDistribution::Blockable   },
/*BloodStarExplosion*/   { &AddMissileExplosion,    &ProcessMissileExplosion,     SFX_NONE,    SFX_NONE,    MissileGraphicID::BloodStarExplosion,   Magic,                 MissileMovementDistribution::Disabled    },
/*Teleport*/             { &AddTeleport,            &ProcessTeleport,             LS_ELEMENTL, SFX_NONE,    MissileGraphicID::None,                 Physical | Invisible,  MissileMovementDistribution::Disabled    },
/*FireArrow*/            { &AddElementalArrow,      &ProcessElementalArrow,       SFX_NONE,    SFX_NONE,    MissileGraphicID::FireArrow,            Fire | Arrow,          MissileMovementDistribution::Blockable   },
/*DoomSerpents*/         { nullptr,                 nullptr,                      LS_DSERP,    SFX_NONE,    MissileGraphicID::DoomSerpents,         Magic | Invisible,     MissileMovementDistribution::Disabled    },
/*FireOnly*/             { nullptr,                 nullptr,                      SFX_NONE,    SFX_NONE,    MissileGraphicID::FireWall,             Fire,                  MissileMovementDistribution::Disabled    },
/*StoneCurse*/           { &AddStoneCurse,          &ProcessStoneCurse,           LS_SCURIMP,  SFX_NONE,    MissileGraphicID::None,                 Magic | Invisible,     MissileMovementDistribution::Disabled    },
/*BloodRitual*/          { nullptr,                 nullptr,                      SFX_NONE,    SFX_NONE,    MissileGraphicID::None,                 Physical,              MissileMovementDistribution::Disabled    },
/*Invisibility*/         { nullptr,                 nullptr,                      LS_INVISIBL, SFX_NONE,    MissileGraphicID::None,                 Physical | Invisible,  MissileMovementDistribution::Disabled    },
/*Golem*/                { &AddGolem,               nullptr,                      LS_GOLUM,    SFX_NONE,    MissileGraphicID::None,                 Physical | Invisible,  MissileMovementDistribution::Disabled    },
/*Etherealize*/          { nullptr,                 nullptr,                      LS_ETHEREAL, SFX_NONE,    MissileGraphicID::Etherealize,          Physical,              MissileMovementDistribution::Disabled    },
/*Spurt*/                { nullptr,                 nullptr,                      SFX_NONE,    SFX_NONE,    MissileGraphicID::Spurt,                Physical,              MissileMovementDistribution::Disabled    },
/*ApocalypseBoom*/       { &AddApocalypseBoom,      &ProcessApocalypseBoom,       SFX_NONE,    SFX_NONE,    MissileGraphicID::ApocalypseBoom,       Physical,              MissileMovementDistribution::Disabled    },
/*Healing*/              { &AddHealing,             nullptr,                      SFX_NONE,    SFX_NONE,    MissileGraphicID::None,                 Physical | Invisible,  MissileMovementDistribution::Disabled    },
/*FireWallControl*/      { &AddFireWallControl,     &ProcessFireWallControl,      SFX_NONE,    SFX_NONE,    MissileGraphicID::FireWall,             Fire | Invisible,      MissileMovementDistribution::Disabled    },
/*Infravision*/          { &AddInfravision,         &ProcessInfravision,          LS_INFRAVIS, SFX_NONE,    MissileGraphicID::None,                 Physical | Invisible,  MissileMovementDistribution::Disabled    },
/*Identify*/             { &AddIdentify,            nullptr,                      SFX_NONE,    SFX_NONE,    MissileGraphicID::None,                 Physical | Invisible,  MissileMovementDistribution::Disabled    },
/*FlameWaveControl*/     { &AddFlameWaveControl,    &ProcessFlameWaveControl,     LS_FLAMWAVE, SFX_NONE,    MissileGraphicID::FireWall,             Fire,                  MissileMovementDistribution::Disabled    },
/*Nova*/                 { &AddNova,                &ProcessNova,                 LS_NOVA,     SFX_NONE,    MissileGraphicID::Lightning,            Lightning,             MissileMovementDistribution::Disabled    },
/*Rage*/                 { &AddRage,                &ProcessRage,                 SFX_NONE,    SFX_NONE,    MissileGraphicID::None,                 Physical | Invisible,  MissileMovementDistribution::Disabled    },
/*Apocalypse*/           { &AddApocalypse,          &ProcessApocalypse,           LS_APOC,     SFX_NONE,    MissileGraphicID::ApocalypseBoom,       Magic,                 MissileMovementDistribution::Disabled    },
/*ItemRepair*/           { &AddItemRepair,          nullptr,                      SFX_NONE,    SFX_NONE,    MissileGraphicID::None,                 Physical | Invisible,  MissileMovementDistribution::Disabled    },
/*StaffRecharge*/        { &AddStaffRecharge,       nullptr,                      SFX_NONE,    SFX_NONE,    MissileGraphicID::None,                 Physical | Invisible,  MissileMovementDistribution::Disabled    },
/*TrapDisarm*/           { &AddTrapDisarm,          nullptr,                      LS_TRAPDIS,  SFX_NONE,    MissileGraphicID::None,                 Physical | Invisible,  MissileMovementDistribution::Disabled    },
/*Inferno*/              { &AddInferno,             &ProcessInferno,              LS_SPOUTSTR, SFX_NONE,    MissileGraphicID::Inferno,              Fire,                  MissileMovementDistribution::Disabled    },
/*InfernoControl*/       { &AddInfernoControl,      &ProcessInfernoControl,       SFX_NONE,    SFX_NONE,    MissileGraphicID::None,                 Fire | Invisible,      MissileMovementDistribution::Disabled    },
/*FireMan*/              { nullptr,                 nullptr,                      SFX_NONE,    SFX_NONE,    MissileGraphicID::None,                 Physical,              MissileMovementDistribution::Blockable   },
/*Krull*/                { nullptr,                 nullptr,                      SFX_NONE,    SFX_NONE,    MissileGraphicID::Krull,                Fire | Arrow,          MissileMovementDistribution::Blockable   },
/*ChargedBolt*/          { &AddChargedBolt,         &ProcessChargedBolt,          LS_CBOLT,    SFX_NONE,    MissileGraphicID::ChargedBolt,          Lightning,             MissileMovementDistribution::Blockable   },
/*HolyBolt*/             { &AddHolyBolt,            &ProcessHolyBolt,             LS_HOLYBOLT, LS_ELECIMP1, MissileGraphicID::HolyBolt,             Physical,              MissileMovementDistribution::Blockable   },
/*Resurrect*/            { &AddResurrect,           nullptr,                      SFX_NONE,    LS_RESUR,    MissileGraphicID::None,                 Magic | Invisible,     MissileMovementDistribution::Disabled    },
/*Telekinesis*/          { &AddTelekinesis,         nullptr,                      LS_ETHEREAL, SFX_NONE,    MissileGraphicID::None,                 Physical | Invisible,  MissileMovementDistribution::Disabled    },
/*LightningArrow*/       { &AddElementalArrow,      &ProcessElementalArrow,       SFX_NONE,    SFX_NONE,    MissileGraphicID::LightningArrow,       Lightning | Arrow,     MissileMovementDistribution::Blockable   },
/*Acid*/                 { &AddAcid,                &ProcessGenericProjectile,    LS_ACID,     SFX_NONE,    MissileGraphicID::Acid,                 Acid,                  MissileMovementDistribution::Blockable   },
/*AcidSplat*/            { &AddMissileExplosion,    &ProcessAcidSplate,           SFX_NONE,    SFX_NONE,    MissileGraphicID::AcidSplat,            Acid,                  MissileMovementDistribution::Disabled    },
/*AcidPuddle*/           { &AddAcidPuddle,          &ProcessAcidPuddle,           LS_PUDDLE,   SFX_NONE,    MissileGraphicID::AcidPuddle,           Acid,                  MissileMovementDistribution::Disabled    },
/*HealOther*/            { &AddHealOther,           nullptr,                      SFX_NONE,    SFX_NONE,    MissileGraphicID::None,                 Physical | Invisible,  MissileMovementDistribution::Disabled    },
/*Elemental*/            { &AddElemental,           &ProcessElemental,            LS_ELEMENTL, SFX_NONE,    MissileGraphicID::Elemental,            Fire,                  MissileMovementDistribution::Unblockable },
/*ResurrectBeam*/        { &AddResurrectBeam,       &ProcessResurrectBeam,        SFX_NONE,    SFX_NONE,    MissileGraphicID::Resurrect,            Physical,              MissileMovementDistribution::Disabled    },
/*BoneSpirit*/           { &AddBoneSpirit,          &ProcessBoneSpirit,           LS_BONESP,   LS_BSIMPCT,  MissileGraphicID::BoneSpirit,           Magic,                 MissileMovementDistribution::Blockable   },
/*WeaponExplosion*/      { &AddWeaponExplosion,     &ProcessWeaponExplosion,      SFX_NONE,    SFX_NONE,    MissileGraphicID::None,                 Physical,              MissileMovementDistribution::Disabled    },
/*RedPortal*/            { &AddRedPortal,           &ProcessRedPortal,            LS_SENTINEL, LS_ELEMENTL, MissileGraphicID::RedPortal,            Physical,              MissileMovementDistribution::Disabled    },
/*DiabloApocalypseBoom*/ { &AddApocalypseBoom,      &ProcessApocalypseBoom,       SFX_NONE,    SFX_NONE,    MissileGraphicID::DiabloApocalypseBoom, Physical,              MissileMovementDistribution::Disabled    },
/*DiabloApocalypse*/     { &AddDiabloApocalypse,    nullptr,                      SFX_NONE,    SFX_NONE,    MissileGraphicID::None,                 Physical | Invisible,  MissileMovementDistribution::Disabled    },
/*Mana*/                 { &AddMana,                nullptr,                      SFX_NONE,    SFX_NONE,    MissileGraphicID::None,                 Physical | Invisible,  MissileMovementDistribution::Disabled    },
/*Magi*/                 { &AddMagi,                nullptr,                      SFX_NONE,    SFX_NONE,    MissileGraphicID::None,                 Physical | Invisible,  MissileMovementDistribution::Disabled    },
/*LightningWall*/        { &AddLightningWall,       &ProcessLightningWall,        LS_LMAG,     LS_ELECIMP1, MissileGraphicID::Lightning,            Lightning,             MissileMovementDistribution::Disabled    },
/*LightningWallControl*/ { &AddFireWallControl,     &ProcessLightningWallControl, SFX_NONE,    SFX_NONE,    MissileGraphicID::Lightning,            Lightning | Invisible, MissileMovementDistribution::Disabled    },
/*Immolation*/           { &AddNova,                &ProcessImmolation,           LS_FBOLT1,   LS_FIRIMP2,  MissileGraphicID::Fireball,             Fire,                  MissileMovementDistribution::Disabled    },
/*SpectralArrow*/        { &AddSpectralArrow,       &ProcessSpectralArrow,        SFX_NONE,    SFX_NONE,    MissileGraphicID::Arrow,                Physical | Arrow,      MissileMovementDistribution::Disabled    },
/*FireballBow*/          { &AddImmolation,          &ProcessFireball,             IS_FBALLBOW, LS_FIRIMP2,  MissileGraphicID::Fireball,             Fire,                  MissileMovementDistribution::Blockable   },
/*LightningBow*/         { &AddLightningBow,        &ProcessLightningBow,         IS_FBALLBOW, SFX_NONE,    MissileGraphicID::Lightning,            Lightning | Invisible, MissileMovementDistribution::Disabled    },
/*ChargedBoltBow*/       { &AddChargedBoltBow,      &ProcessChargedBolt,          LS_CBOLT,    SFX_NONE,    MissileGraphicID::ChargedBolt,          Lightning,             MissileMovementDistribution::Blockable   },
/*HolyBoltBow*/          { &AddHolyBolt,            &ProcessHolyBolt,             LS_HOLYBOLT, LS_ELECIMP1, MissileGraphicID::HolyBolt,             Physical,              MissileMovementDistribution::Blockable   },
/*Warp*/                 { &AddWarp,                &ProcessTeleport,             LS_ETHEREAL, SFX_NONE,    MissileGraphicID::None,                 Physical | Invisible,  MissileMovementDistribution::Disabled    },
/*Reflect*/              { &AddReflect,             nullptr,                      LS_MSHIELD,  SFX_NONE,    MissileGraphicID::Reflect,              Physical | Invisible,  MissileMovementDistribution::Disabled    },
/*Berserk*/              { &AddBerserk,             nullptr,                      SFX_NONE,    SFX_NONE,    MissileGraphicID::None,                 Physical | Invisible,  MissileMovementDistribution::Disabled    },
/*RingOfFire*/           { &AddRingOfFire,          &ProcessRingOfFire,           SFX_NONE,    SFX_NONE,    MissileGraphicID::FireWall,             Fire | Invisible,      MissileMovementDistribution::Disabled    },
/*StealPotions*/         { &AddStealPotions,        nullptr,                      SFX_NONE,    SFX_NONE,    MissileGraphicID::None,                 Physical | Invisible,  MissileMovementDistribution::Disabled    },
/*StealMana*/            { &AddStealMana,           nullptr,                      IS_CAST7,    SFX_NONE,    MissileGraphicID::None,                 Physical | Invisible,  MissileMovementDistribution::Disabled    },
/*RingOfLightning*/      { nullptr,                 nullptr,                      SFX_NONE,    SFX_NONE,    MissileGraphicID::Lightning,            Lightning | Invisible, MissileMovementDistribution::Disabled    },
/*Search*/               { &AddSearch,              &ProcessSearch,               SFX_NONE,    SFX_NONE,    MissileGraphicID::None,                 Physical | Invisible,  MissileMovementDistribution::Disabled    },
/*Aura*/                 { nullptr,                 nullptr,                      SFX_NONE,    LS_ELECIMP1, MissileGraphicID::FlashBottom,          Magic | Invisible,     MissileMovementDistribution::Disabled    },
/*Aura2*/                { nullptr,                 nullptr,                      SFX_NONE,    SFX_NONE,    MissileGraphicID::FlashTop,             Magic | Invisible,     MissileMovementDistribution::Disabled    },
/*SpiralFireball*/       { nullptr,                 nullptr,                      LS_FBOLT1,   LS_FIRIMP2,  MissileGraphicID::Fireball,             Fire,                  MissileMovementDistribution::Disabled    },
/*RuneOfFire*/           { &AddRuneOfFire,          &ProcessRune,                 SFX_NONE,    SFX_NONE,    MissileGraphicID::Rune,                 Physical,              MissileMovementDistribution::Disabled    },
/*RuneOfLight*/          { &AddRuneOfLight,         &ProcessRune,                 SFX_NONE,    SFX_NONE,    MissileGraphicID::Rune,                 Physical,              MissileMovementDistribution::Disabled    },
/*RuneOfNova*/           { &AddRuneOfNova,          &ProcessRune,                 SFX_NONE,    SFX_NONE,    MissileGraphicID::Rune,                 Physical,              MissileMovementDistribution::Disabled    },
/*RuneOfImmolation*/     { &AddRuneOfImmolation,    &ProcessRune,                 SFX_NONE,    SFX_NONE,    MissileGraphicID::Rune,                 Physical,              MissileMovementDistribution::Disabled    },
/*RuneOfStone*/          { &AddRuneOfStone,         &ProcessRune,                 SFX_NONE,    SFX_NONE,    MissileGraphicID::Rune,                 Physical,              MissileMovementDistribution::Disabled    },
/*BigExplosion*/         { &AddBigExplosion,        &ProcessBigExplosion,         LS_NESTXPLD, LS_NESTXPLD, MissileGraphicID::BigExplosion,         Fire,                  MissileMovementDistribution::Disabled    },
/*HorkSpawn*/            { &AddHorkSpawn,           &ProcessHorkSpawn,            SFX_NONE,    SFX_NONE,    MissileGraphicID::None,                 Physical | Invisible,  MissileMovementDistribution::Disabled    },
/*Jester*/               { &AddJester,              nullptr,                      SFX_NONE,    SFX_NONE,    MissileGraphicID::None,                 Physical | Invisible,  MissileMovementDistribution::Disabled    },
/*OpenNest*/             { &AddOpenNest,            nullptr,                      SFX_NONE,    SFX_NONE,    MissileGraphicID::None,                 Physical | Invisible,  MissileMovementDistribution::Disabled    },
/*OrangeFlare*/          { &AddGenericMagicMissile, &ProcessGenericProjectile,    SFX_NONE,    SFX_NONE,    MissileGraphicID::OrangeFlare,          Magic,                 MissileMovementDistribution::Blockable   },
/*BlueFlare*/            { &AddGenericMagicMissile, &ProcessGenericProjectile,    SFX_NONE,    SFX_NONE,    MissileGraphicID::BlueFlare2,           Magic,                 MissileMovementDistribution::Blockable   },
/*RedFlare*/             { &AddGenericMagicMissile, &ProcessGenericProjectile,    SFX_NONE,    SFX_NONE,    MissileGraphicID::RedFlare,             Magic,                 MissileMovementDistribution::Blockable   },
/*YellowFlare*/          { &AddGenericMagicMissile, &ProcessGenericProjectile,    SFX_NONE,    SFX_NONE,    MissileGraphicID::YellowFlare,          Magic,                 MissileMovementDistribution::Blockable   },
/*BlueFlare2*/           { &AddGenericMagicMissile, &ProcessGenericProjectile,    SFX_NONE,    SFX_NONE,    MissileGraphicID::BlueFlare2,           Magic,                 MissileMovementDistribution::Blockable   },
/*YellowExplosion*/      { &AddMissileExplosion,    &ProcessMissileExplosion,     LS_FIRIMP2,  SFX_NONE,    MissileGraphicID::YellowFlareExplosion, Physical,              MissileMovementDistribution::Disabled    },
/*RedExplosion*/         { &AddMissileExplosion,    &ProcessMissileExplosion,     LS_FIRIMP2,  SFX_NONE,    MissileGraphicID::RedFlareExplosion,    Physical,              MissileMovementDistribution::Disabled    },
/*BlueExplosion*/        { &AddMissileExplosion,    &ProcessMissileExplosion,     LS_FIRIMP2,  SFX_NONE,    MissileGraphicID::BlueFlareExplosion,   Physical,              MissileMovementDistribution::Disabled    },
/*BlueExplosion2*/       { &AddMissileExplosion,    &ProcessMissileExplosion,     LS_FIRIMP2,  SFX_NONE,    MissileGraphicID::BlueFlareExplosion2,  Physical,              MissileMovementDistribution::Disabled    },
/*OrangeExplosion*/      { &AddMissileExplosion,    &ProcessMissileExplosion,     LS_FIRIMP2,  SFX_NONE,    MissileGraphicID::OrangeFlareExplosion, Physical,              MissileMovementDistribution::Disabled    },
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
	Repeat(5),
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
constexpr uint8_t AnimLen_5 = 3;        // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_6 = 4;        // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_7 = 5;        // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_8 = 6;        // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_9 = 7;        // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_10 = 8;       // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_12 = 9;       // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_13 = 10;      // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_14 = 11;      // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_15 = 12;      // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_16 = 13;      // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_17 = 14;      // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_19 = 15;      // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_20 = 16;      // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_9_4 = 17;     // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_15_14_3 = 18; // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_13_11 = 19;   // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_16x8_8 = 20;  // NOLINT(readability-identifier-naming)

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
	sprites.emplace(OwnedClxSpriteListOrSheet { LoadClxListOrSheet(path) });
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
		if (!loadHellfireGraphics && mi > static_cast<uint8_t>(MissileGraphicID::BloodStarRedExplosion))
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
