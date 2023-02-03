/**
 * @file misdat.cpp
 *
 * Implementation of data related to missiles.
 */
#include "misdat.h"

#include "engine/load_cl2.hpp"
#include "missiles.h"
#include "mpq/mpq_common.hpp"
#include "utils/file_name_generator.hpp"
#include "utils/str_cat.hpp"

namespace devilution {

/** Data related to each missile ID. */
MissileData MissilesData[] = {
	// clang-format off
// id                      mAddProc,                mProc,                        mDraw,   mType, damageType,            mFileNum,                               mlSFX,       miSFX,       MovementDistribution;
/*Arrow*/                { &AddArrow,               &ProcessArrow,                true,        0, DamageType::Physical,  MissileGraphicID::Arrow,                SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
/*Firebolt*/             { &AddFirebolt,            &ProcessGenericProjectile,    true,        1, DamageType::Fire,      MissileGraphicID::Fireball,             LS_FBOLT1,   LS_FIRIMP2,  MissileMovementDistribution::Blockable   },
/*Guardian*/             { &AddGuardian,            &ProcessGuardian,             true,        1, DamageType::Physical,  MissileGraphicID::Guardian,             LS_GUARD,    LS_GUARDLAN, MissileMovementDistribution::Disabled    },
/*Phasing*/              { &AddPhasing,             &ProcessTeleport,             false,       1, DamageType::Physical,  MissileGraphicID::None,                 LS_TELEPORT, SFX_NONE,    MissileMovementDistribution::Disabled    },
/*NovaBall*/             { &AddNovaBall,            &ProcessNovaBall,             true,        1, DamageType::Lightning, MissileGraphicID::Lightning,            SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Unblockable },
/*FireWall*/             { &AddFireWall,            &ProcessFireWall,             true,        1, DamageType::Fire,      MissileGraphicID::FireWall,             LS_WALLLOOP, LS_FIRIMP2,  MissileMovementDistribution::Disabled    },
/*Fireball*/             { &AddFireball,            &ProcessFireball,             true,        1, DamageType::Fire,      MissileGraphicID::Fireball,             LS_FBOLT1,   LS_FIRIMP2,  MissileMovementDistribution::Blockable   },
/*LightningControl*/     { &AddLightningControl,    &ProcessLightningControl,     false,       1, DamageType::Lightning, MissileGraphicID::Lightning,            SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
/*Lightning*/            { &AddLightning,           &ProcessLightning,            true,        1, DamageType::Lightning, MissileGraphicID::Lightning,            LS_LNING1,   LS_ELECIMP1, MissileMovementDistribution::Disabled    },
/*MagmaBallExplosion*/   { &AddMissileExplosion,    &ProcessMissileExplosion,     true,        2, DamageType::Physical,  MissileGraphicID::MagmaBallExplosion,   SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
/*TownPortal*/           { &AddTownPortal,          &ProcessTownPortal,           true,        1, DamageType::Magic,     MissileGraphicID::TownPortal,           LS_SENTINEL, LS_ELEMENTL, MissileMovementDistribution::Disabled    },
/*FlashBottom*/          { &AddFlashBottom,         &ProcessFlashBottom,          true,        1, DamageType::Magic,     MissileGraphicID::FlashBottom,          LS_NOVA,     LS_ELECIMP1, MissileMovementDistribution::Disabled    },
/*FlashTop*/             { &AddFlashTop,            &ProcessFlashTop,             true,        1, DamageType::Magic,     MissileGraphicID::FlashTop,             SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
/*ManaShield*/           { &AddManaShield,          nullptr,                      false,       1, DamageType::Magic,     MissileGraphicID::ManaShield,           LS_MSHIELD,  SFX_NONE,    MissileMovementDistribution::Disabled    },
/*FlameWave*/            { &AddFlameWave,           &ProcessFlameWave,            true,        1, DamageType::Fire,      MissileGraphicID::FireWall,             SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Unblockable },
/*ChainLightning*/       { &AddChainLightning,      &ProcessChainLightning,       true,        1, DamageType::Lightning, MissileGraphicID::Lightning,            LS_LNING1,   LS_ELECIMP1, MissileMovementDistribution::Disabled    },
/*ChainBall*/            { nullptr,                 nullptr,                      true,        1, DamageType::Lightning, MissileGraphicID::Lightning,            SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
/*BloodHit*/             { nullptr,                 nullptr,                      true,        2, DamageType::Physical,  MissileGraphicID::BloodHit,             LS_BLODSTAR, LS_BLSIMPT,  MissileMovementDistribution::Disabled    },
/*BoneHit*/              { nullptr,                 nullptr,                      true,        2, DamageType::Physical,  MissileGraphicID::BoneHit,              SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
/*MetalHit*/             { nullptr,                 nullptr,                      true,        2, DamageType::Physical,  MissileGraphicID::MetalHit,             SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
/*Rhino*/                { &AddRhino,               &ProcessRhino,                true,        2, DamageType::Physical,  MissileGraphicID::None,                 SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
/*MagmaBall*/            { &AddMagmaBall,           &ProcessGenericProjectile,    true,        1, DamageType::Fire,      MissileGraphicID::MagmaBall,            SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
/*ThinLightningControl*/ { &AddLightningControl,    &ProcessLightningControl,     false,       1, DamageType::Lightning, MissileGraphicID::ThinLightning,        SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
/*ThinLightning*/        { &AddLightning,           &ProcessLightning,            true,        1, DamageType::Lightning, MissileGraphicID::ThinLightning,        SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
/*BloodStar*/            { &AddGenericMagicMissile, &ProcessGenericProjectile,    true,        1, DamageType::Magic,     MissileGraphicID::BloodStar,            SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
/*BloodStarExplosion*/   { &AddMissileExplosion,    &ProcessMissileExplosion,     true,        2, DamageType::Magic,     MissileGraphicID::BloodStarExplosion,   SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
/*Teleport*/             { &AddTeleport,            &ProcessTeleport,             false,       1, DamageType::Physical,  MissileGraphicID::None,                 LS_ELEMENTL, SFX_NONE,    MissileMovementDistribution::Disabled    },
/*FireArrow*/            { &AddElementalArrow,      &ProcessElementalArrow,       true,        0, DamageType::Fire,      MissileGraphicID::FireArrow,            SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
/*DoomSerpents*/         { nullptr,                 nullptr,                      false,       1, DamageType::Magic,     MissileGraphicID::DoomSerpents,         LS_DSERP,    SFX_NONE,    MissileMovementDistribution::Disabled    },
/*FireOnly*/             { nullptr,                 nullptr,                      true,        2, DamageType::Fire,      MissileGraphicID::FireWall,             SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
/*StoneCurse*/           { &AddStoneCurse,          &ProcessStoneCurse,           false,       1, DamageType::Magic,     MissileGraphicID::None,                 LS_SCURIMP,  SFX_NONE,    MissileMovementDistribution::Disabled    },
/*BloodRitual*/          { nullptr,                 nullptr,                      true,        1, DamageType::Physical,  MissileGraphicID::None,                 SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
/*Invisibility*/         { nullptr,                 nullptr,                      false,       1, DamageType::Physical,  MissileGraphicID::None,                 LS_INVISIBL, SFX_NONE,    MissileMovementDistribution::Disabled    },
/*Golem*/                { &AddGolem,               nullptr,                      false,       1, DamageType::Physical,  MissileGraphicID::None,                 LS_GOLUM,    SFX_NONE,    MissileMovementDistribution::Disabled    },
/*Etherealize*/          { nullptr,                 nullptr,                      true,        1, DamageType::Physical,  MissileGraphicID::Etherealize,          LS_ETHEREAL, SFX_NONE,    MissileMovementDistribution::Disabled    },
/*Spurt*/                { nullptr,                 nullptr,                      true,        2, DamageType::Physical,  MissileGraphicID::Spurt,                SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
/*ApocalypseBoom*/       { &AddApocalypseBoom,      &ProcessApocalypseBoom,       true,        2, DamageType::Physical,  MissileGraphicID::ApocalypseBoom,       SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
/*Healing*/              { &AddHealing,             nullptr,                      false,       1, DamageType::Physical,  MissileGraphicID::None,                 SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
/*FireWallControl*/      { &AddFireWallControl,     &ProcessFireWallControl,      false,       1, DamageType::Fire,      MissileGraphicID::FireWall,             SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
/*Infravision*/          { &AddInfravision,         &ProcessInfravision,          false,       1, DamageType::Physical,  MissileGraphicID::None,                 LS_INFRAVIS, SFX_NONE,    MissileMovementDistribution::Disabled    },
/*Identify*/             { &AddIdentify,            nullptr,                      false,       1, DamageType::Physical,  MissileGraphicID::None,                 SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
/*FlameWaveControl*/     { &AddFlameWaveControl,    &ProcessFlameWaveControl,     true,        1, DamageType::Fire,      MissileGraphicID::FireWall,             LS_FLAMWAVE, SFX_NONE,    MissileMovementDistribution::Disabled    },
/*Nova*/                 { &AddNova,                &ProcessNova,                 true,        1, DamageType::Lightning, MissileGraphicID::Lightning,            LS_NOVA,     SFX_NONE,    MissileMovementDistribution::Disabled    },
/*Rage*/                 { &AddRage,                &ProcessRage,                 false,       1, DamageType::Physical,  MissileGraphicID::None,                 SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
/*Apocalypse*/           { &AddApocalypse,          &ProcessApocalypse,           true,        1, DamageType::Magic,     MissileGraphicID::ApocalypseBoom,       LS_APOC,     SFX_NONE,    MissileMovementDistribution::Disabled    },
/*ItemRepair*/           { &AddItemRepair,          nullptr,                      false,       2, DamageType::Physical,  MissileGraphicID::None,                 SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
/*StaffRecharge*/        { &AddStaffRecharge,       nullptr,                      false,       2, DamageType::Physical,  MissileGraphicID::None,                 SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
/*TrapDisarm*/           { &AddTrapDisarm,          nullptr,                      false,       2, DamageType::Physical,  MissileGraphicID::None,                 LS_TRAPDIS,  SFX_NONE,    MissileMovementDistribution::Disabled    },
/*Inferno*/              { &AddInferno,             &ProcessInferno,              true,        1, DamageType::Fire,      MissileGraphicID::Inferno,              LS_SPOUTSTR, SFX_NONE,    MissileMovementDistribution::Disabled    },
/*InfernoControl*/       { &AddInfernoControl,      &ProcessInfernoControl,       false,       1, DamageType::Fire,      MissileGraphicID::None,                 SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
/*FireMan*/              { nullptr,                 nullptr,                      true,        2, DamageType::Physical,  MissileGraphicID::None,                 SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
/*Krull*/                { nullptr,                 nullptr,                      true,        0, DamageType::Fire,      MissileGraphicID::Krull,                SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
/*ChargedBolt*/          { &AddChargedBolt,         &ProcessChargedBolt,          true,        1, DamageType::Lightning, MissileGraphicID::ChargedBolt,          LS_CBOLT,    SFX_NONE,    MissileMovementDistribution::Blockable   },
/*HolyBolt*/             { &AddHolyBolt,            &ProcessHolyBolt,             true,        1, DamageType::Physical,  MissileGraphicID::HolyBolt,             LS_HOLYBOLT, LS_ELECIMP1, MissileMovementDistribution::Blockable   },
/*Resurrect*/            { &AddResurrect,           nullptr,                      false,       1, DamageType::Magic,     MissileGraphicID::None,                 SFX_NONE,    LS_RESUR,    MissileMovementDistribution::Disabled    },
/*Telekinesis*/          { &AddTelekinesis,         nullptr,                      false,       1, DamageType::Physical,  MissileGraphicID::None,                 LS_ETHEREAL, SFX_NONE,    MissileMovementDistribution::Disabled    },
/*LightningArrow*/       { &AddElementalArrow,      &ProcessElementalArrow,       true,        0, DamageType::Lightning, MissileGraphicID::LightningArrow,       SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
/*Acid*/                 { &AddAcid,                &ProcessGenericProjectile,    true,        1, DamageType::Acid,      MissileGraphicID::Acid,                 LS_ACID,     SFX_NONE,    MissileMovementDistribution::Blockable   },
/*AcidSplat*/            { &AddMissileExplosion,    &ProcessAcidSplate,           true,        2, DamageType::Acid,      MissileGraphicID::AcidSplat,            SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
/*AcidPuddle*/           { &AddAcidPuddle,          &ProcessAcidPuddle,           true,        2, DamageType::Acid,      MissileGraphicID::AcidPuddle,           LS_PUDDLE,   SFX_NONE,    MissileMovementDistribution::Disabled    },
/*HealOther*/            { &AddHealOther,           nullptr,                      false,       1, DamageType::Physical,  MissileGraphicID::None,                 SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
/*Elemental*/            { &AddElemental,           &ProcessElemental,            true,        1, DamageType::Fire,      MissileGraphicID::Elemental,            LS_ELEMENTL, SFX_NONE,    MissileMovementDistribution::Unblockable },
/*ResurrectBeam*/        { &AddResurrectBeam,       &ProcessResurrectBeam,        true,        1, DamageType::Physical,  MissileGraphicID::Resurrect,            SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
/*BoneSpirit*/           { &AddBoneSpirit,          &ProcessBoneSpirit,           true,        1, DamageType::Magic,     MissileGraphicID::BoneSpirit,           LS_BONESP,   LS_BSIMPCT,  MissileMovementDistribution::Blockable   },
/*WeaponExplosion*/      { &AddWeaponExplosion,     &ProcessWeaponExplosion,      true,        2, DamageType::Physical,  MissileGraphicID::None,                 SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
/*RedPortal*/            { &AddRedPortal,           &ProcessRedPortal,            true,        2, DamageType::Physical,  MissileGraphicID::RedPortal,            LS_SENTINEL, LS_ELEMENTL, MissileMovementDistribution::Disabled    },
/*DiabloApocalypseBoom*/ { &AddApocalypseBoom,      &ProcessApocalypseBoom,       true,        2, DamageType::Physical,  MissileGraphicID::DiabloApocalypseBoom, SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
/*DiabloApocalypse*/     { &AddDiabloApocalypse,    nullptr,                      false,       2, DamageType::Physical,  MissileGraphicID::None,                 SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
/*Mana*/                 { &AddMana,                nullptr,                      false,       1, DamageType::Physical,  MissileGraphicID::None,                 SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
/*Magi*/                 { &AddMagi,                nullptr,                      false,       1, DamageType::Physical,  MissileGraphicID::None,                 SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
/*LightningWall*/        { &AddLightningWall,       &ProcessLightningWall,        true,        1, DamageType::Lightning, MissileGraphicID::Lightning,            LS_LMAG,     LS_ELECIMP1, MissileMovementDistribution::Disabled    },
/*LightningWallControl*/ { &AddFireWallControl,     &ProcessLightningWallControl, false,       1, DamageType::Lightning, MissileGraphicID::Lightning,            SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
/*Immolation*/           { &AddNova,                &ProcessImmolation,           true,        1, DamageType::Fire,      MissileGraphicID::Fireball,             LS_FBOLT1,   LS_FIRIMP2,  MissileMovementDistribution::Disabled    },
/*SpectralArrow*/        { &AddSpectralArrow,       &ProcessSpectralArrow,        true,        0, DamageType::Physical,  MissileGraphicID::Arrow,                SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
/*FireballBow*/          { &AddImmolation,          &ProcessFireball,             true,        1, DamageType::Fire,      MissileGraphicID::Fireball,             IS_FBALLBOW, LS_FIRIMP2,  MissileMovementDistribution::Blockable   },
/*LightningBow*/         { &AddLightningBow,        &ProcessLightningBow,         false,       1, DamageType::Lightning, MissileGraphicID::Lightning,            IS_FBALLBOW, SFX_NONE,    MissileMovementDistribution::Disabled    },
/*ChargedBoltBow*/       { &AddChargedBoltBow,      &ProcessChargedBolt,          true,        1, DamageType::Lightning, MissileGraphicID::ChargedBolt,          LS_CBOLT,    SFX_NONE,    MissileMovementDistribution::Blockable   },
/*HolyBoltBow*/          { &AddHolyBolt,            &ProcessHolyBolt,             true,        1, DamageType::Physical,  MissileGraphicID::HolyBolt,             LS_HOLYBOLT, LS_ELECIMP1, MissileMovementDistribution::Blockable   },
/*Warp*/                 { &AddWarp,                &ProcessTeleport,             false,       1, DamageType::Physical,  MissileGraphicID::None,                 LS_ETHEREAL, SFX_NONE,    MissileMovementDistribution::Disabled    },
/*Reflect*/              { &AddReflect,             nullptr,                      false,       1, DamageType::Physical,  MissileGraphicID::Reflect,              LS_MSHIELD,  SFX_NONE,    MissileMovementDistribution::Disabled    },
/*Berserk*/              { &AddBerserk,             nullptr,                      false,       1, DamageType::Physical,  MissileGraphicID::None,                 SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
/*RingOfFire*/           { &AddRingOfFire,          &ProcessRingOfFire,           false,       1, DamageType::Fire,      MissileGraphicID::FireWall,             SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
/*StealPotions*/         { &AddStealPotions,        nullptr,                      false,       1, DamageType::Physical,  MissileGraphicID::None,                 SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
/*StealMana*/            { &AddStealMana,           nullptr,                      false,       1, DamageType::Physical,  MissileGraphicID::None,                 IS_CAST7,    SFX_NONE,    MissileMovementDistribution::Disabled    },
/*RingOfLightning*/      { nullptr,                 nullptr,                      false,       1, DamageType::Lightning, MissileGraphicID::Lightning,            SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
/*Search*/               { &AddSearch,              &ProcessSearch,               false,       1, DamageType::Physical,  MissileGraphicID::None,                 SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
/*Aura*/                 { nullptr,                 nullptr,                      false,       1, DamageType::Magic,     MissileGraphicID::FlashBottom,          SFX_NONE,    LS_ELECIMP1, MissileMovementDistribution::Disabled    },
/*Aura2*/                { nullptr,                 nullptr,                      false,       1, DamageType::Magic,     MissileGraphicID::FlashTop,             SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
/*SpiralFireball*/       { nullptr,                 nullptr,                      true,        1, DamageType::Fire,      MissileGraphicID::Fireball,             LS_FBOLT1,   LS_FIRIMP2,  MissileMovementDistribution::Disabled    },
/*RuneOfFire*/           { &AddRuneOfFire,          &ProcessRune,                 true,        1, DamageType::Physical,  MissileGraphicID::Rune,                 SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
/*RuneOfLight*/          { &AddRuneOfLight,         &ProcessRune,                 true,        1, DamageType::Physical,  MissileGraphicID::Rune,                 SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
/*RuneOfNova*/           { &AddRuneOfNova,          &ProcessRune,                 true,        1, DamageType::Physical,  MissileGraphicID::Rune,                 SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
/*RuneOfImmolation*/     { &AddRuneOfImmolation,    &ProcessRune,                 true,        1, DamageType::Physical,  MissileGraphicID::Rune,                 SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
/*RuneOfStone*/          { &AddRuneOfStone,         &ProcessRune,                 true,        1, DamageType::Physical,  MissileGraphicID::Rune,                 SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
/*BigExplosion*/         { &AddBigExplosion,        &ProcessBigExplosion,         true,        1, DamageType::Fire,      MissileGraphicID::BigExplosion,         LS_NESTXPLD, LS_NESTXPLD, MissileMovementDistribution::Disabled    },
/*HorkSpawn*/            { &AddHorkSpawn,           &ProcessHorkSpawn,            false,       2, DamageType::Physical,  MissileGraphicID::None,                 SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
/*Jester*/               { &AddJester,              nullptr,                      false,       2, DamageType::Physical,  MissileGraphicID::None,                 SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
/*OpenNest*/             { &AddOpenNest,            nullptr,                      false,       2, DamageType::Physical,  MissileGraphicID::None,                 SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
/*OrangeFlare*/          { &AddGenericMagicMissile, &ProcessGenericProjectile,    true,        1, DamageType::Magic,     MissileGraphicID::OrangeFlare,          SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
/*BlueFlare*/            { &AddGenericMagicMissile, &ProcessGenericProjectile,    true,        1, DamageType::Magic,     MissileGraphicID::BlueFlare2,           SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
/*RedFlare*/             { &AddGenericMagicMissile, &ProcessGenericProjectile,    true,        1, DamageType::Magic,     MissileGraphicID::RedFlare,             SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
/*YellowFlare*/          { &AddGenericMagicMissile, &ProcessGenericProjectile,    true,        1, DamageType::Magic,     MissileGraphicID::YellowFlare,          SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
/*BlueFlare2*/           { &AddGenericMagicMissile, &ProcessGenericProjectile,    true,        1, DamageType::Magic,     MissileGraphicID::BlueFlare2,           SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
/*YellowExplosion*/      { &AddMissileExplosion,    &ProcessMissileExplosion,     true,        2, DamageType::Physical,  MissileGraphicID::YellowFlareExplosion, LS_FIRIMP2,  SFX_NONE,    MissileMovementDistribution::Disabled    },
/*RedExplosion*/         { &AddMissileExplosion,    &ProcessMissileExplosion,     true,        2, DamageType::Physical,  MissileGraphicID::RedFlareExplosion,    LS_FIRIMP2,  SFX_NONE,    MissileMovementDistribution::Disabled    },
/*BlueExplosion*/        { &AddMissileExplosion,    &ProcessMissileExplosion,     true,        2, DamageType::Physical,  MissileGraphicID::BlueFlareExplosion,   LS_FIRIMP2,  SFX_NONE,    MissileMovementDistribution::Disabled    },
/*BlueExplosion2*/       { &AddMissileExplosion,    &ProcessMissileExplosion,     true,        2, DamageType::Physical,  MissileGraphicID::BlueFlareExplosion2,  LS_FIRIMP2,  SFX_NONE,    MissileMovementDistribution::Disabled    },
/*OrangeExplosion*/      { &AddMissileExplosion,    &ProcessMissileExplosion,     true,        2, DamageType::Physical,  MissileGraphicID::OrangeFlareExplosion, LS_FIRIMP2,  SFX_NONE,    MissileMovementDistribution::Disabled    },
	// clang-format on
};

/** Data related to each missile graphic ID. */
MissileFileData MissileSpriteData[] = {
	// clang-format off
// id                          name,        animFAmt, flags,                          animDelay[16],  animLen[16],                            animWidth,  animWidth2
/*Arrow*/                    { "arrows",           1, MissileDataFlags::NotAnimated,  { 0 },          { 16 },                                        96,          16 },
/*Fireball*/                 { "fireba",          16, MissileDataFlags::None,         { 0 },          { 14 },                                        96,          16 },
/*Guardian*/                 { "guard",            3, MissileDataFlags::None,         { 1 },          { 15, 14,  3 },                                96,          16 },
/*Lightning*/                { "lghning",          1, MissileDataFlags::None,         { 0 },          {  8 },                                        96,          16 },
/*FireWall*/                 { "firewal",          2, MissileDataFlags::None,         { 0 },          { 13, 11 },                                   128,          32 },
/*MagmaBallExplosion*/       { "magblos",          1, MissileDataFlags::None,         { 1 },          { 10 },                                       128,          32 },
/*TownPortal*/               { "portal",           2, MissileDataFlags::None,         { 0, 1 },       { 16 },                                        96,          16 },
/*FlashBottom*/              { "bluexfr",          1, MissileDataFlags::None,         { 0 },          { 19 },                                       160,          48 },
/*FlashTop*/                 { "bluexbk",          1, MissileDataFlags::None,         { 0 },          { 19 },                                       160,          48 },
/*ManaShield*/               { "manashld",         1, MissileDataFlags::NotAnimated,  { 0 },          {  1 },                                        96,          16 },
/*BloodHit*/                 { {},                 4, MissileDataFlags::None,         { 0 },          { 15 },                                        96,          16 },
/*BoneHit*/                  { {},                 3, MissileDataFlags::None,         { 2 },          {  8 },                                       128,          32 },
/*MetalHit*/                 { {},                 3, MissileDataFlags::None,         { 2 },          { 10 },                                        96,          16 },
/*FireArrow*/                { "farrow",          16, MissileDataFlags::None,         { 0 },          {  4 },                                        96,          16 },
/*DoomSerpents*/             { "doom",             9, MissileDataFlags::MonsterOwned, { 1 },          { 15 },                                        96,          16 },
/*Golem*/                    { {},                 1, MissileDataFlags::MonsterOwned, { 0 },          {  0 },                                         0,           0 },
/*Spurt*/                    { {},                 2, MissileDataFlags::None,         { 2 },          {  8 },                                       128,          32 },
/*ApocalypseBoom*/           { "newexp",           1, MissileDataFlags::None,         { 1 },          { 15 },                                        96,          16 },
/*StoneCurseShatter*/        { "shatter1",         1, MissileDataFlags::None,         { 1 },          { 12 },                                       128,          32 },
/*BigExplosion*/             { "bigexp",           1, MissileDataFlags::None,         { 0 },          { 15 },                                       160,          48 },
/*Inferno*/                  { "inferno",          1, MissileDataFlags::None,         { 0 },          { 20 },                                        96,          16 },
/*ThinLightning*/            { "thinlght",         1, MissileDataFlags::MonsterOwned, { 0 },          {  8 },                                        96,          16 },
/*BloodStar*/                { "flare",            1, MissileDataFlags::None,         { 0 },          { 16 },                                       128,          32 },
/*BloodStarExplosion*/       { "flareexp",         1, MissileDataFlags::None,         { 0 },          {  7 },                                       128,          32 },
/*MagmaBall*/                { "magball",          8, MissileDataFlags::MonsterOwned, { 1 },          { 16 },                                       128,          32 },
/*Krull*/                    { "krull",            1, MissileDataFlags::MonsterOwned, { 0 },          { 14 },                                        96,          16 },
/*ChargedBolt*/              { "miniltng",         1, MissileDataFlags::None,         { 1 },          {  8 },                                        64,           0 },
/*HolyBolt*/                 { "holy",            16, MissileDataFlags::None,         { 1, 0 },       { 14 },                                        96,          16 },
/*HolyBoltExplosion*/        { "holyexpl",         1, MissileDataFlags::None,         { 0 },          {  8 },                                       160,          48 },
/*LightningArrow*/           { "larrow",          16, MissileDataFlags::None,         { 0 },          {  4 },                                        96,          16 },
/*FireArrowExplosion*/       { {},                 1, MissileDataFlags::None,         { 0 },          {  6 },                                        64,           0 },
/*Acid*/                     { "acidbf",          16, MissileDataFlags::MonsterOwned, { 0 },          {  8 },                                        96,          16 },
/*AcidSplat*/                { "acidspla",         1, MissileDataFlags::MonsterOwned, { 0 },          {  8 },                                        96,          16 },
/*AcidPuddle*/               { "acidpud",          2, MissileDataFlags::MonsterOwned, { 0 },          {  9,  4 },                                    96,          16 },
/*Etherealize*/              { {},                 1, MissileDataFlags::None,         { 0 },          {  1 },                                        96,          16 },
/*Elemental*/                { "firerun",          8, MissileDataFlags::None,         { 1 },          { 12 },                                        96,          16 },
/*Resurrect*/                { "ressur1",          1, MissileDataFlags::None,         { 0 },          { 16 },                                        96,          16 },
/*BoneSpirit*/               { "sklball",          9, MissileDataFlags::None,         { 1 },          { 16, 16, 16, 16, 16, 16, 16, 16, 8 },         96,          16 },
/*RedPortal*/                { "rportal",          2, MissileDataFlags::None,         { 0 },          { 16 },                                        96,          16 },
/*DiabloApocalypseBoom*/     { "fireplar",         1, MissileDataFlags::MonsterOwned, { 1 },          { 17 },                                       160,          48 },
/*BloodStarBlue*/            { "scubmisb",         1, MissileDataFlags::MonsterOwned, { 0 },          { 16 },                                        96,          16 },
/*BloodStarBlueExplosion*/   { "scbsexpb",         1, MissileDataFlags::MonsterOwned, { 0 },          {  6 },                                       128,          32 },
/*BloodStarYellow*/          { "scubmisc",         1, MissileDataFlags::MonsterOwned, { 0 },          { 16 },                                        96,          16 },
/*BloodStarYellowExplosion*/ { "scbsexpc",         1, MissileDataFlags::MonsterOwned, { 0 },          {  6 },                                       128,          32 },
/*BloodStarRed*/             { "scubmisd",         1, MissileDataFlags::MonsterOwned, { 0 },          { 16 },                                        96,          16 },
/*BloodStarRedExplosion*/    { "scbsexpd",         1, MissileDataFlags::MonsterOwned, { 0 },          {  6 },                                       128,          32 },
/*HorkSpawn*/                { "spawns",           8, MissileDataFlags::MonsterOwned, { 0 },          {  9 },                                        96,          16 },
/*Reflect*/                  { "reflect",          1, MissileDataFlags::NotAnimated,  { 0 },          {  1 },                                       160,          64 },
/*OrangeFlare*/              { "ms_ora",          16, MissileDataFlags::MonsterOwned, { 0 },          { 15 },                                        96,           8 },
/*BlueFlare*/                { "ms_bla",          16, MissileDataFlags::MonsterOwned, { 0 },          { 15 },                                        96,           8 },
/*RedFlare*/                 { "ms_reb",          16, MissileDataFlags::MonsterOwned, { 0 },          { 15 },                                        96,           8 },
/*YellowFlare*/              { "ms_yeb",          16, MissileDataFlags::MonsterOwned, { 0 },          { 15 },                                        96,           8 },
/*Rune*/                     { "rglows1",          1, MissileDataFlags::None,         { 0 },          { 10 },                                        96,           8 },
/*YellowFlareExplosion*/     { "ex_yel2",          1, MissileDataFlags::MonsterOwned, { 0 },          { 10 },                                       220,          78 },
/*BlueFlareExplosion*/       { "ex_blu2",          1, MissileDataFlags::MonsterOwned, { 0 },          { 10 },                                       212,          86 },
/*RedFlareExplosion*/        { "ex_red3",          1, MissileDataFlags::MonsterOwned, { 0 },          {  7 },                                       292,         114 },
/*BlueFlare2*/               { "ms_blb",          16, MissileDataFlags::MonsterOwned, { 0 },          { 15 },                                        96,           8 },
/*OrangeFlareExplosion*/     { "ex_ora1",          1, MissileDataFlags::MonsterOwned, { 0 },          { 13 },                                        96,         -12 },
/*BlueFlareExplosion2*/      { "ex_blu3",          1, MissileDataFlags::MonsterOwned, { 0 },          {  7 },                                       292,         114 },
/*None*/                     { {},                 0, MissileDataFlags::None,         {  },           { },                                            0,           0 },
	// clang-format on
};

namespace {

template <typename T>
std::array<T, 16> maybeAutofill(std::initializer_list<T> list)
{
	assert(list.size() <= 16);

	std::array<T, 16> ret = {};

	if (list.size() == 1) {
		ret.fill(*list.begin());
	} else {
		int i = 0;
		for (T x : list)
			ret[i++] = x;
	}
	return ret;
}

} // namespace

MissileFileData::MissileFileData(string_view name, uint8_t animFAmt, MissileDataFlags flags,
    std::initializer_list<uint8_t> animDelay, std::initializer_list<uint8_t> animLen,
    uint16_t animWidth, int16_t animWidth2)
    : name(name)
    , animFAmt(animFAmt)
    , flags(flags)
    , animDelay(maybeAutofill(animDelay))
    , animLen(maybeAutofill(animLen))
    , animWidth(animWidth)
    , animWidth2(animWidth2)
{
}

void MissileFileData::LoadGFX()
{
	if (sprites)
		return;

	if (name.empty())
		return;

	if (animFAmt == 1) {
		char path[MaxMpqPathSize];
		*BufCopy(path, "missiles\\", name) = '\0';
		sprites.emplace(OwnedClxSpriteListOrSheet { LoadCl2(path, animWidth) });
	} else {
		FileNameGenerator pathGenerator({ "missiles\\", name }, DEVILUTIONX_CL2_EXT);
		sprites.emplace(OwnedClxSpriteListOrSheet { LoadMultipleCl2Sheet<16>(pathGenerator, animFAmt, animWidth) });
	}
}

void InitMissileGFX(bool loadHellfireGraphics)
{
	if (HeadlessMode)
		return;

	for (size_t mi = 0; MissileSpriteData[mi].animFAmt != 0; mi++) {
		if (!loadHellfireGraphics && mi > static_cast<uint8_t>(MissileGraphicID::BloodStarRedExplosion))
			break;
		if (MissileSpriteData[mi].flags == MissileDataFlags::MonsterOwned)
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
