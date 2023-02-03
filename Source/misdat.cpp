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
	{ 15, 4, 3 },
	{ 13, 11 },
	{ 16, 16, 16, 16, 16, 16, 16, 16, 8 }
};

constexpr uint8_t AnimLen_0 = 0;       // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_1 = 1;       // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_4 = 2;       // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_5 = 3;       // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_6 = 4;       // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_7 = 5;       // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_8 = 6;       // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_9 = 7;       // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_10 = 8;      // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_12 = 9;      // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_13 = 10;     // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_14 = 11;     // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_15 = 12;     // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_16 = 13;     // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_17 = 14;     // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_19 = 15;     // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_20 = 16;     // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_9_4 = 17;    // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_15_4_3 = 18; // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_13_11 = 19;  // NOLINT(readability-identifier-naming)
constexpr uint8_t AnimLen_16x8_8 = 20; // NOLINT(readability-identifier-naming)

} // namespace

/** Data related to each missile graphic ID. */
MissileFileData MissileSpriteData[] = {
	// clang-format off
// id                          sprites,   animWidth,  animWidth2, name,        animFAmt, flags,                           animDelayIdx, animLenIdx
/*Arrow*/                    { {},               96,          16, "arrows",           1, MissileDataFlags::NotAnimated,              0, AnimLen_16     },
/*Fireball*/                 { {},               96,          16, "fireba",          16, MissileDataFlags::None,                     0, AnimLen_14     },
/*Guardian*/                 { {},               96,          16, "guard",            3, MissileDataFlags::None,                     1, AnimLen_15_4_3 },
/*Lightning*/                { {},               96,          16, "lghning",          1, MissileDataFlags::None,                     0, AnimLen_8      },
/*FireWall*/                 { {},              128,          32, "firewal",          2, MissileDataFlags::None,                     0, AnimLen_13_11  },
/*MagmaBallExplosion*/       { {},              128,          32, "magblos",          1, MissileDataFlags::None,                     1, AnimLen_10     },
/*TownPortal*/               { {},               96,          16, "portal",           2, MissileDataFlags::None,                     3, AnimLen_16     },
/*FlashBottom*/              { {},              160,          48, "bluexfr",          1, MissileDataFlags::None,                     0, AnimLen_19     },
/*FlashTop*/                 { {},              160,          48, "bluexbk",          1, MissileDataFlags::None,                     0, AnimLen_19     },
/*ManaShield*/               { {},               96,          16, "manashld",         1, MissileDataFlags::NotAnimated,              0, AnimLen_1      },
/*BloodHit*/                 { {},               96,          16, {},                 4, MissileDataFlags::None,                     0, AnimLen_15     },
/*BoneHit*/                  { {},              128,          32, {},                 3, MissileDataFlags::None,                     2, AnimLen_8      },
/*MetalHit*/                 { {},               96,          16, {},                 3, MissileDataFlags::None,                     2, AnimLen_10     },
/*FireArrow*/                { {},               96,          16, "farrow",          16, MissileDataFlags::None,                     0, AnimLen_4      },
/*DoomSerpents*/             { {},               96,          16, "doom",             9, MissileDataFlags::MonsterOwned,             1, AnimLen_15     },
/*Golem*/                    { {},                0,           0, {},                 1, MissileDataFlags::MonsterOwned,             0, AnimLen_0      },
/*Spurt*/                    { {},              128,          32, {},                 2, MissileDataFlags::None,                     2, AnimLen_8      },
/*ApocalypseBoom*/           { {},               96,          16, "newexp",           1, MissileDataFlags::None,                     1, AnimLen_15     },
/*StoneCurseShatter*/        { {},              128,          32, "shatter1",         1, MissileDataFlags::None,                     1, AnimLen_12     },
/*BigExplosion*/             { {},              160,          48, "bigexp",           1, MissileDataFlags::None,                     0, AnimLen_15     },
/*Inferno*/                  { {},               96,          16, "inferno",          1, MissileDataFlags::None,                     0, AnimLen_20     },
/*ThinLightning*/            { {},               96,          16, "thinlght",         1, MissileDataFlags::MonsterOwned,             0, AnimLen_8      },
/*BloodStar*/                { {},              128,          32, "flare",            1, MissileDataFlags::None,                     0, AnimLen_16     },
/*BloodStarExplosion*/       { {},              128,          32, "flareexp",         1, MissileDataFlags::None,                     0, AnimLen_7      },
/*MagmaBall*/                { {},              128,          32, "magball",          8, MissileDataFlags::MonsterOwned,             1, AnimLen_16     },
/*Krull*/                    { {},               96,          16, "krull",            1, MissileDataFlags::MonsterOwned,             0, AnimLen_14     },
/*ChargedBolt*/              { {},               64,           0, "miniltng",         1, MissileDataFlags::None,                     1, AnimLen_8      },
/*HolyBolt*/                 { {},               96,          16, "holy",            16, MissileDataFlags::None,                     4, AnimLen_14     },
/*HolyBoltExplosion*/        { {},              160,          48, "holyexpl",         1, MissileDataFlags::None,                     0, AnimLen_8      },
/*LightningArrow*/           { {},               96,          16, "larrow",          16, MissileDataFlags::None,                     0, AnimLen_4      },
/*FireArrowExplosion*/       { {},               64,           0, {},                 1, MissileDataFlags::None,                     0, AnimLen_6      },
/*Acid*/                     { {},               96,          16, "acidbf",          16, MissileDataFlags::MonsterOwned,             0, AnimLen_8      },
/*AcidSplat*/                { {},               96,          16, "acidspla",         1, MissileDataFlags::MonsterOwned,             0, AnimLen_8      },
/*AcidPuddle*/               { {},               96,          16, "acidpud",          2, MissileDataFlags::MonsterOwned,             0, AnimLen_9_4    },
/*Etherealize*/              { {},               96,          16, {},                 1, MissileDataFlags::None,                     0, AnimLen_1      },
/*Elemental*/                { {},               96,          16, "firerun",          8, MissileDataFlags::None,                     1, AnimLen_12     },
/*Resurrect*/                { {},               96,          16, "ressur1",          1, MissileDataFlags::None,                     0, AnimLen_16     },
/*BoneSpirit*/               { {},               96,          16, "sklball",          9, MissileDataFlags::None,                     1, AnimLen_16x8_8 },
/*RedPortal*/                { {},               96,          16, "rportal",          2, MissileDataFlags::None,                     0, AnimLen_16     },
/*DiabloApocalypseBoom*/     { {},              160,          48, "fireplar",         1, MissileDataFlags::MonsterOwned,             1, AnimLen_17     },
/*BloodStarBlue*/            { {},               96,          16, "scubmisb",         1, MissileDataFlags::MonsterOwned,             0, AnimLen_16     },
/*BloodStarBlueExplosion*/   { {},              128,          32, "scbsexpb",         1, MissileDataFlags::MonsterOwned,             0, AnimLen_6      },
/*BloodStarYellow*/          { {},               96,          16, "scubmisc",         1, MissileDataFlags::MonsterOwned,             0, AnimLen_16     },
/*BloodStarYellowExplosion*/ { {},              128,          32, "scbsexpc",         1, MissileDataFlags::MonsterOwned,             0, AnimLen_6      },
/*BloodStarRed*/             { {},               96,          16, "scubmisd",         1, MissileDataFlags::MonsterOwned,             0, AnimLen_16     },
/*BloodStarRedExplosion*/    { {},              128,          32, "scbsexpd",         1, MissileDataFlags::MonsterOwned,             0, AnimLen_6      },
/*HorkSpawn*/                { {},               96,          16, "spawns",           8, MissileDataFlags::MonsterOwned,             0, AnimLen_9      },
/*Reflect*/                  { {},              160,          64, "reflect",          1, MissileDataFlags::NotAnimated,              0, AnimLen_1      },
/*OrangeFlare*/              { {},               96,           8, "ms_ora",          16, MissileDataFlags::MonsterOwned,             0, AnimLen_15     },
/*BlueFlare*/                { {},               96,           8, "ms_bla",          16, MissileDataFlags::MonsterOwned,             0, AnimLen_15     },
/*RedFlare*/                 { {},               96,           8, "ms_reb",          16, MissileDataFlags::MonsterOwned,             0, AnimLen_15     },
/*YellowFlare*/              { {},               96,           8, "ms_yeb",          16, MissileDataFlags::MonsterOwned,             0, AnimLen_15     },
/*Rune*/                     { {},               96,           8, "rglows1",          1, MissileDataFlags::None,                     0, AnimLen_10     },
/*YellowFlareExplosion*/     { {},              220,          78, "ex_yel2",          1, MissileDataFlags::MonsterOwned,             0, AnimLen_10     },
/*BlueFlareExplosion*/       { {},              212,          86, "ex_blu2",          1, MissileDataFlags::MonsterOwned,             0, AnimLen_10     },
/*RedFlareExplosion*/        { {},              292,         114, "ex_red3",          1, MissileDataFlags::MonsterOwned,             0, AnimLen_7      },
/*BlueFlare2*/               { {},               96,           8, "ms_blb",          16, MissileDataFlags::MonsterOwned,             0, AnimLen_15     },
/*OrangeFlareExplosion*/     { {},               96,         -12, "ex_ora1",          1, MissileDataFlags::MonsterOwned,             0, AnimLen_13     },
/*BlueFlareExplosion2*/      { {},              292,         114, "ex_blu3",          1, MissileDataFlags::MonsterOwned,             0, AnimLen_7      },
/*None*/                     { {},                0,           0, {},                 0, MissileDataFlags::None,                     0, 0              },
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
