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
// mAddProc,               mProc,                        name,                            mDraw,   mType, damageType,            mFileNum,                               mlSFX,       miSFX,       MovementDistribution;
{ &AddArrow,               &ProcessArrow,                MissileID::Arrow,                true,        0, DamageType::Physical,  MissileGraphicID::Arrow,                SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
{ &AddFirebolt,            &ProcessGenericProjectile,    MissileID::Firebolt,             true,        1, DamageType::Fire,      MissileGraphicID::Fireball,             LS_FBOLT1,   LS_FIRIMP2,  MissileMovementDistribution::Blockable   },
{ &AddGuardian,            &ProcessGuardian,             MissileID::Guardian,             true,        1, DamageType::Physical,  MissileGraphicID::Guardian,             LS_GUARD,    LS_GUARDLAN, MissileMovementDistribution::Disabled    },
{ &AddPhasing,             &ProcessTeleport,             MissileID::Phasing,              false,       1, DamageType::Physical,  MissileGraphicID::None,                 LS_TELEPORT, SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddNovaBall,            &ProcessNovaBall,             MissileID::NovaBall,             true,        1, DamageType::Lightning, MissileGraphicID::Lightning,            SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Unblockable },
{ &AddFireWall,            &ProcessFireWall,             MissileID::FireWall,             true,        1, DamageType::Fire,      MissileGraphicID::FireWall,             LS_WALLLOOP, LS_FIRIMP2,  MissileMovementDistribution::Disabled    },
{ &AddFireball,            &ProcessFireball,             MissileID::Fireball,             true,        1, DamageType::Fire,      MissileGraphicID::Fireball,             LS_FBOLT1,   LS_FIRIMP2,  MissileMovementDistribution::Blockable   },
{ &AddLightningControl,    &ProcessLightningControl,     MissileID::LightningControl,     false,       1, DamageType::Lightning, MissileGraphicID::Lightning,            SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddLightning,           &ProcessLightning,            MissileID::Lightning,            true,        1, DamageType::Lightning, MissileGraphicID::Lightning,            LS_LNING1,   LS_ELECIMP1, MissileMovementDistribution::Disabled    },
{ &AddMissileExplosion,    &ProcessMissileExplosion,     MissileID::MagmaBallExplosion,   true,        2, DamageType::Physical,  MissileGraphicID::MagmaBallExplosion,   SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddTownPortal,          &ProcessTownPortal,           MissileID::TownPortal,           true,        1, DamageType::Magic,     MissileGraphicID::TownPortal,           LS_SENTINEL, LS_ELEMENTL, MissileMovementDistribution::Disabled    },
{ &AddFlashBottom,         &ProcessFlashBottom,          MissileID::FlashBottom,          true,        1, DamageType::Magic,     MissileGraphicID::FlashBottom,          LS_NOVA,     LS_ELECIMP1, MissileMovementDistribution::Disabled    },
{ &AddFlashTop,            &ProcessFlashTop,             MissileID::FlashTop,             true,        1, DamageType::Magic,     MissileGraphicID::FlashTop,             SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddManaShield,          nullptr,                      MissileID::ManaShield,           false,       1, DamageType::Magic,     MissileGraphicID::ManaShield,           LS_MSHIELD,  SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddFlameWave,           &ProcessFlameWave,            MissileID::FlameWave,            true,        1, DamageType::Fire,      MissileGraphicID::FireWall,             SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Unblockable },
{ &AddChainLightning,      &ProcessChainLightning,       MissileID::ChainLightning,       true,        1, DamageType::Lightning, MissileGraphicID::Lightning,            LS_LNING1,   LS_ELECIMP1, MissileMovementDistribution::Disabled    },
{ nullptr,                 nullptr,                      MissileID::ChainBall,            true,        1, DamageType::Lightning, MissileGraphicID::Lightning,            SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ nullptr,                 nullptr,                      MissileID::BloodHit,             true,        2, DamageType::Physical,  MissileGraphicID::BloodHit,             LS_BLODSTAR, LS_BLSIMPT,  MissileMovementDistribution::Disabled    },
{ nullptr,                 nullptr,                      MissileID::BoneHit,              true,        2, DamageType::Physical,  MissileGraphicID::BoneHit,              SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ nullptr,                 nullptr,                      MissileID::MetalHit,             true,        2, DamageType::Physical,  MissileGraphicID::MetalHit,             SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddRhino,               &ProcessRhino,                MissileID::Rhino,                true,        2, DamageType::Physical,  MissileGraphicID::None,                 SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
{ &AddMagmaBall,           &ProcessGenericProjectile,    MissileID::MagmaBall,            true,        1, DamageType::Fire,      MissileGraphicID::MagmaBall,            SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
{ &AddLightningControl,    &ProcessLightningControl,     MissileID::ThinLightningControl, false,       1, DamageType::Lightning, MissileGraphicID::ThinLightning,        SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddLightning,           &ProcessLightning,            MissileID::ThinLightning,        true,        1, DamageType::Lightning, MissileGraphicID::ThinLightning,        SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddGenericMagicMissile, &ProcessGenericProjectile,    MissileID::BloodStar,            true,        1, DamageType::Magic,     MissileGraphicID::BloodStar,            SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
{ &AddMissileExplosion,    &ProcessMissileExplosion,     MissileID::BloodStarExplosion,   true,        2, DamageType::Magic,     MissileGraphicID::BloodStarExplosion,   SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddTeleport,            &ProcessTeleport,             MissileID::Teleport,             false,       1, DamageType::Physical,  MissileGraphicID::None,                 LS_ELEMENTL, SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddElementalArrow,      &ProcessElementalArrow,       MissileID::FireArrow,            true,        0, DamageType::Fire,      MissileGraphicID::FireArrow,            SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
{ nullptr,                 nullptr,                      MissileID::DoomSerpents,         false,       1, DamageType::Magic,     MissileGraphicID::DoomSerpents,         LS_DSERP,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ nullptr,                 nullptr,                      MissileID::FireOnly,             true,        2, DamageType::Fire,      MissileGraphicID::FireWall,             SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddStoneCurse,          &ProcessStoneCurse,           MissileID::StoneCurse,           false,       1, DamageType::Magic,     MissileGraphicID::None,                 LS_SCURIMP,  SFX_NONE,    MissileMovementDistribution::Disabled    },
{ nullptr,                 nullptr,                      MissileID::BloodRitual,          true,        1, DamageType::Physical,  MissileGraphicID::None,                 SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ nullptr,                 nullptr,                      MissileID::Invisibility,         false,       1, DamageType::Physical,  MissileGraphicID::None,                 LS_INVISIBL, SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddGolem,               nullptr,                      MissileID::Golem,                false,       1, DamageType::Physical,  MissileGraphicID::None,                 LS_GOLUM,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ nullptr,                 nullptr,                      MissileID::Etherealize,          true,        1, DamageType::Physical,  MissileGraphicID::Etherealize,          LS_ETHEREAL, SFX_NONE,    MissileMovementDistribution::Disabled    },
{ nullptr,                 nullptr,                      MissileID::Spurt,                true,        2, DamageType::Physical,  MissileGraphicID::Spurt,                SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddApocalypseBoom,      &ProcessApocalypseBoom,       MissileID::ApocalypseBoom,       true,        2, DamageType::Physical,  MissileGraphicID::ApocalypseBoom,       SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddHealing,             nullptr,                      MissileID::Healing,              false,       1, DamageType::Physical,  MissileGraphicID::None,                 SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddFireWallControl,     &ProcessFireWallControl,      MissileID::FireWallControl,      false,       1, DamageType::Fire,      MissileGraphicID::FireWall,             SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddInfravision,         &ProcessInfravision,          MissileID::Infravision,          false,       1, DamageType::Physical,  MissileGraphicID::None,                 LS_INFRAVIS, SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddIdentify,            nullptr,                      MissileID::Identify,             false,       1, DamageType::Physical,  MissileGraphicID::None,                 SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddFlameWaveControl,    &ProcessFlameWaveControl,     MissileID::FlameWaveControl,     true,        1, DamageType::Fire,      MissileGraphicID::FireWall,             LS_FLAMWAVE, SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddNova,                &ProcessNova,                 MissileID::Nova,                 true,        1, DamageType::Lightning, MissileGraphicID::Lightning,            LS_NOVA,     SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddRage,                &ProcessRage,                 MissileID::Rage,                 false,       1, DamageType::Physical,  MissileGraphicID::None,                 SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddApocalypse,          &ProcessApocalypse,           MissileID::Apocalypse,           true,        1, DamageType::Magic,     MissileGraphicID::ApocalypseBoom,       LS_APOC,     SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddItemRepair,          nullptr,                      MissileID::ItemRepair,           false,       2, DamageType::Physical,  MissileGraphicID::None,                 SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddStaffRecharge,       nullptr,                      MissileID::StaffRecharge,        false,       2, DamageType::Physical,  MissileGraphicID::None,                 SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddTrapDisarm,          nullptr,                      MissileID::TrapDisarm,           false,       2, DamageType::Physical,  MissileGraphicID::None,                 LS_TRAPDIS,  SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddInferno,             &ProcessInferno,              MissileID::Inferno,              true,        1, DamageType::Fire,      MissileGraphicID::Inferno,              LS_SPOUTSTR, SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddInfernoControl,      &ProcessInfernoControl,       MissileID::InfernoControl,       false,       1, DamageType::Fire,      MissileGraphicID::None,                 SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ nullptr,                 nullptr,                      MissileID::FireMan,              true,        2, DamageType::Physical,  MissileGraphicID::None,                 SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
{ nullptr,                 nullptr,                      MissileID::Krull,                true,        0, DamageType::Fire,      MissileGraphicID::Krull,                SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
{ &AddChargedBolt,         &ProcessChargedBolt,          MissileID::ChargedBolt,          true,        1, DamageType::Lightning, MissileGraphicID::ChargedBolt,          LS_CBOLT,    SFX_NONE,    MissileMovementDistribution::Blockable   },
{ &AddHolyBolt,            &ProcessHolyBolt,             MissileID::HolyBolt,             true,        1, DamageType::Physical,  MissileGraphicID::HolyBolt,             LS_HOLYBOLT, LS_ELECIMP1, MissileMovementDistribution::Blockable   },
{ &AddResurrect,           nullptr,                      MissileID::Resurrect,            false,       1, DamageType::Magic,     MissileGraphicID::None,                 SFX_NONE,    LS_RESUR,    MissileMovementDistribution::Disabled    },
{ &AddTelekinesis,         nullptr,                      MissileID::Telekinesis,          false,       1, DamageType::Physical,  MissileGraphicID::None,                 LS_ETHEREAL, SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddElementalArrow,      &ProcessElementalArrow,       MissileID::LightningArrow,       true,        0, DamageType::Lightning, MissileGraphicID::LightningArrow,       SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
{ &AddAcid,                &ProcessGenericProjectile,    MissileID::Acid,                 true,        1, DamageType::Acid,      MissileGraphicID::Acid,                 LS_ACID,     SFX_NONE,    MissileMovementDistribution::Blockable   },
{ &AddMissileExplosion,    &ProcessAcidSplate,           MissileID::AcidSplat,            true,        2, DamageType::Acid,      MissileGraphicID::AcidSplat,            SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddAcidPuddle,          &ProcessAcidPuddle,           MissileID::AcidPuddle,           true,        2, DamageType::Acid,      MissileGraphicID::AcidPuddle,           LS_PUDDLE,   SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddHealOther,           nullptr,                      MissileID::HealOther,            false,       1, DamageType::Physical,  MissileGraphicID::None,                 SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddElemental,           &ProcessElemental,            MissileID::Elemental,            true,        1, DamageType::Fire,      MissileGraphicID::Elemental,            LS_ELEMENTL, SFX_NONE,    MissileMovementDistribution::Unblockable },
{ &AddResurrectBeam,       &ProcessResurrectBeam,        MissileID::ResurrectBeam,        true,        1, DamageType::Physical,  MissileGraphicID::Resurrect,            SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddBoneSpirit,          &ProcessBoneSpirit,           MissileID::BoneSpirit,           true,        1, DamageType::Magic,     MissileGraphicID::BoneSpirit,           LS_BONESP,   LS_BSIMPCT,  MissileMovementDistribution::Blockable   },
{ &AddWeaponExplosion,     &ProcessWeaponExplosion,      MissileID::WeaponExplosion,      true,        2, DamageType::Physical,  MissileGraphicID::None,                 SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddRedPortal,           &ProcessRedPortal,            MissileID::RedPortal,            true,        2, DamageType::Physical,  MissileGraphicID::RedPortal,            LS_SENTINEL, LS_ELEMENTL, MissileMovementDistribution::Disabled    },
{ &AddApocalypseBoom,      &ProcessApocalypseBoom,       MissileID::DiabloApocalypseBoom, true,        2, DamageType::Physical,  MissileGraphicID::DiabloApocalypseBoom, SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddDiabloApocalypse,    nullptr,                      MissileID::DiabloApocalypse,     false,       2, DamageType::Physical,  MissileGraphicID::None,                 SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddMana,                nullptr,                      MissileID::Mana,                 false,       1, DamageType::Physical,  MissileGraphicID::None,                 SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddMagi,                nullptr,                      MissileID::Magi,                 false,       1, DamageType::Physical,  MissileGraphicID::None,                 SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddLightningWall,       &ProcessLightningWall,        MissileID::LightningWall,        true,        1, DamageType::Lightning, MissileGraphicID::Lightning,            LS_LMAG,     LS_ELECIMP1, MissileMovementDistribution::Disabled    },
{ &AddFireWallControl,     &ProcessLightningWallControl, MissileID::LightningWallControl, false,       1, DamageType::Lightning, MissileGraphicID::Lightning,            SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddNova,                &ProcessImmolation,           MissileID::Immolation,           true,        1, DamageType::Fire,      MissileGraphicID::Fireball,             LS_FBOLT1,   LS_FIRIMP2,  MissileMovementDistribution::Disabled    },
{ &AddSpectralArrow,       &ProcessSpectralArrow,        MissileID::SpectralArrow,        true,        0, DamageType::Physical,  MissileGraphicID::Arrow,                SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddImmolation,          &ProcessFireball,             MissileID::FireballBow,          true,        1, DamageType::Fire,      MissileGraphicID::Fireball,             IS_FBALLBOW, LS_FIRIMP2,  MissileMovementDistribution::Blockable   },
{ &AddLightningBow,        &ProcessLightningBow,         MissileID::LightningBow,         false,       1, DamageType::Lightning, MissileGraphicID::Lightning,            IS_FBALLBOW, SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddChargedBoltBow,      &ProcessChargedBolt,          MissileID::ChargedBoltBow,       true,        1, DamageType::Lightning, MissileGraphicID::ChargedBolt,          LS_CBOLT,    SFX_NONE,    MissileMovementDistribution::Blockable   },
{ &AddHolyBolt,            &ProcessHolyBolt,             MissileID::HolyBoltBow,          true,        1, DamageType::Physical,  MissileGraphicID::HolyBolt,             LS_HOLYBOLT, LS_ELECIMP1, MissileMovementDistribution::Blockable   },
{ &AddWarp,                &ProcessTeleport,             MissileID::Warp,                 false,       1, DamageType::Physical,  MissileGraphicID::None,                 LS_ETHEREAL, SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddReflect,             nullptr,                      MissileID::Reflect,              false,       1, DamageType::Physical,  MissileGraphicID::Reflect,              LS_MSHIELD,  SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddBerserk,             nullptr,                      MissileID::Berserk,              false,       1, DamageType::Physical,  MissileGraphicID::None,                 SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddRingOfFire,          &ProcessRingOfFire,           MissileID::RingOfFire,           false,       1, DamageType::Fire,      MissileGraphicID::FireWall,             SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddStealPotions,        nullptr,                      MissileID::StealPotions,         false,       1, DamageType::Physical,  MissileGraphicID::None,                 SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddStealMana,           nullptr,                      MissileID::StealMana,            false,       1, DamageType::Physical,  MissileGraphicID::None,                 IS_CAST7,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ nullptr,                 nullptr,                      MissileID::RingOfLightning,      false,       1, DamageType::Lightning, MissileGraphicID::Lightning,            SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddSearch,              &ProcessSearch,               MissileID::Search,               false,       1, DamageType::Physical,  MissileGraphicID::None,                 SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ nullptr,                 nullptr,                      MissileID::Aura,                 false,       1, DamageType::Magic,     MissileGraphicID::FlashBottom,          SFX_NONE,    LS_ELECIMP1, MissileMovementDistribution::Disabled    },
{ nullptr,                 nullptr,                      MissileID::Aura2,                false,       1, DamageType::Magic,     MissileGraphicID::FlashTop,             SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ nullptr,                 nullptr,                      MissileID::SpiralFireball,       true,        1, DamageType::Fire,      MissileGraphicID::Fireball,             LS_FBOLT1,   LS_FIRIMP2,  MissileMovementDistribution::Disabled    },
{ &AddRuneOfFire,          &ProcessRune,                 MissileID::RuneOfFire,           true,        1, DamageType::Physical,  MissileGraphicID::Rune,                 SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddRuneOfLight,         &ProcessRune,                 MissileID::RuneOfLight,          true,        1, DamageType::Physical,  MissileGraphicID::Rune,                 SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddRuneOfNova,          &ProcessRune,                 MissileID::RuneOfNova,           true,        1, DamageType::Physical,  MissileGraphicID::Rune,                 SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddRuneOfImmolation,    &ProcessRune,                 MissileID::RuneOfImmolation,     true,        1, DamageType::Physical,  MissileGraphicID::Rune,                 SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddRuneOfStone,         &ProcessRune,                 MissileID::RuneOfStone,          true,        1, DamageType::Physical,  MissileGraphicID::Rune,                 SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddBigExplosion,        &ProcessBigExplosion,         MissileID::BigExplosion,         true,        1, DamageType::Fire,      MissileGraphicID::BigExplosion,         LS_NESTXPLD, LS_NESTXPLD, MissileMovementDistribution::Disabled    },
{ &AddHorkSpawn,           &ProcessHorkSpawn,            MissileID::HorkSpawn,            false,       2, DamageType::Physical,  MissileGraphicID::None,                 SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddJester,              nullptr,                      MissileID::Jester,               false,       2, DamageType::Physical,  MissileGraphicID::None,                 SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddOpenNest,            nullptr,                      MissileID::OpenNest,             false,       2, DamageType::Physical,  MissileGraphicID::None,                 SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddGenericMagicMissile, &ProcessGenericProjectile,    MissileID::OrangeFlare,          true,        1, DamageType::Magic,     MissileGraphicID::OrangeFlare,          SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
{ &AddGenericMagicMissile, &ProcessGenericProjectile,    MissileID::BlueFlare,            true,        1, DamageType::Magic,     MissileGraphicID::BlueFlare2,           SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
{ &AddGenericMagicMissile, &ProcessGenericProjectile,    MissileID::RedFlare,             true,        1, DamageType::Magic,     MissileGraphicID::RedFlare,             SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
{ &AddGenericMagicMissile, &ProcessGenericProjectile,    MissileID::YellowFlare,          true,        1, DamageType::Magic,     MissileGraphicID::YellowFlare,          SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
{ &AddGenericMagicMissile, &ProcessGenericProjectile,    MissileID::BlueFlare2,           true,        1, DamageType::Magic,     MissileGraphicID::BlueFlare2,           SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
{ &AddMissileExplosion,    &ProcessMissileExplosion,     MissileID::YellowExplosion,      true,        2, DamageType::Physical,  MissileGraphicID::YellowFlareExplosion, LS_FIRIMP2,  SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddMissileExplosion,    &ProcessMissileExplosion,     MissileID::RedExplosion,         true,        2, DamageType::Physical,  MissileGraphicID::RedFlareExplosion,    LS_FIRIMP2,  SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddMissileExplosion,    &ProcessMissileExplosion,     MissileID::BlueExplosion,        true,        2, DamageType::Physical,  MissileGraphicID::BlueFlareExplosion,   LS_FIRIMP2,  SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddMissileExplosion,    &ProcessMissileExplosion,     MissileID::BlueExplosion2,       true,        2, DamageType::Physical,  MissileGraphicID::BlueFlareExplosion2,  LS_FIRIMP2,  SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddMissileExplosion,    &ProcessMissileExplosion,     MissileID::OrangeExplosion,      true,        2, DamageType::Physical,  MissileGraphicID::OrangeFlareExplosion, LS_FIRIMP2,  SFX_NONE,    MissileMovementDistribution::Disabled    },
	// clang-format on
};

/** Data related to each missile graphic ID. */
MissileFileData MissileSpriteData[] = {
	// clang-format off
// name,      animName,                                    animFAmt, flags,                          animDelay[16],  animLen[16],                            animWidth,  animWidth2
{ "arrows",   MissileGraphicID::Arrow,                            1, MissileDataFlags::NotAnimated,  { 0 },          { 16 },                                        96,          16 },
{ "fireba",   MissileGraphicID::Fireball,                        16, MissileDataFlags::None,         { 0 },          { 14 },                                        96,          16 },
{ "guard",    MissileGraphicID::Guardian,                         3, MissileDataFlags::None,         { 1 },          { 15, 14,  3 },                                96,          16 },
{ "lghning",  MissileGraphicID::Lightning,                        1, MissileDataFlags::None,         { 0 },          {  8 },                                        96,          16 },
{ "firewal",  MissileGraphicID::FireWall,                         2, MissileDataFlags::None,         { 0 },          { 13, 11 },                                   128,          32 },
{ "magblos",  MissileGraphicID::MagmaBallExplosion,               1, MissileDataFlags::None,         { 1 },          { 10 },                                       128,          32 },
{ "portal",   MissileGraphicID::TownPortal,                       2, MissileDataFlags::None,         { 0, 1 },       { 16 },                                        96,          16 },
{ "bluexfr",  MissileGraphicID::FlashBottom,                      1, MissileDataFlags::None,         { 0 },          { 19 },                                       160,          48 },
{ "bluexbk",  MissileGraphicID::FlashTop,                         1, MissileDataFlags::None,         { 0 },          { 19 },                                       160,          48 },
{ "manashld", MissileGraphicID::ManaShield,                       1, MissileDataFlags::NotAnimated,  { 0 },          {  1 },                                        96,          16 },
{ {},         MissileGraphicID::BloodHit,                         4, MissileDataFlags::None,         { 0 },          { 15 },                                        96,          16 },
{ {},         MissileGraphicID::BoneHit,                          3, MissileDataFlags::None,         { 2 },          {  8 },                                       128,          32 },
{ {},         MissileGraphicID::MetalHit,                         3, MissileDataFlags::None,         { 2 },          { 10 },                                        96,          16 },
{ "farrow",   MissileGraphicID::FireArrow,                       16, MissileDataFlags::None,         { 0 },          {  4 },                                        96,          16 },
{ "doom",     MissileGraphicID::DoomSerpents,                     9, MissileDataFlags::MonsterOwned, { 1 },          { 15 },                                        96,          16 },
{ {},         MissileGraphicID::Golem,                            1, MissileDataFlags::MonsterOwned, { 0 },          {  0 },                                         0,           0 },
{ {},         MissileGraphicID::Spurt,                            2, MissileDataFlags::None,         { 2 },          {  8 },                                       128,          32 },
{ "newexp",   MissileGraphicID::ApocalypseBoom,                   1, MissileDataFlags::None,         { 1 },          { 15 },                                        96,          16 },
{ "shatter1", MissileGraphicID::StoneCurseShatter,                1, MissileDataFlags::None,         { 1 },          { 12 },                                       128,          32 },
{ "bigexp",   MissileGraphicID::BigExplosion,                     1, MissileDataFlags::None,         { 0 },          { 15 },                                       160,          48 },
{ "inferno",  MissileGraphicID::Inferno,                          1, MissileDataFlags::None,         { 0 },          { 20 },                                        96,          16 },
{ "thinlght", MissileGraphicID::ThinLightning,                    1, MissileDataFlags::MonsterOwned, { 0 },          {  8 },                                        96,          16 },
{ "flare",    MissileGraphicID::BloodStar,                        1, MissileDataFlags::None,         { 0 },          { 16 },                                       128,          32 },
{ "flareexp", MissileGraphicID::BloodStarExplosion,               1, MissileDataFlags::None,         { 0 },          {  7 },                                       128,          32 },
{ "magball",  MissileGraphicID::MagmaBall,                        8, MissileDataFlags::MonsterOwned, { 1 },          { 16 },                                       128,          32 },
{ "krull",    MissileGraphicID::Krull,                            1, MissileDataFlags::MonsterOwned, { 0 },          { 14 },                                        96,          16 },
{ "miniltng", MissileGraphicID::ChargedBolt,                      1, MissileDataFlags::None,         { 1 },          {  8 },                                        64,           0 },
{ "holy",     MissileGraphicID::HolyBolt,                        16, MissileDataFlags::None,         { 1, 0 },       { 14 },                                        96,          16 },
{ "holyexpl", MissileGraphicID::HolyBoltExplosion,                1, MissileDataFlags::None,         { 0 },          {  8 },                                       160,          48 },
{ "larrow",   MissileGraphicID::LightningArrow,                  16, MissileDataFlags::None,         { 0 },          {  4 },                                        96,          16 },
{ {},         MissileGraphicID::FireArrowExplosion,               1, MissileDataFlags::None,         { 0 },          {  6 },                                        64,           0 },
{ "acidbf",   MissileGraphicID::Acid,                            16, MissileDataFlags::MonsterOwned, { 0 },          {  8 },                                        96,          16 },
{ "acidspla", MissileGraphicID::AcidSplat,                        1, MissileDataFlags::MonsterOwned, { 0 },          {  8 },                                        96,          16 },
{ "acidpud",  MissileGraphicID::AcidPuddle,                       2, MissileDataFlags::MonsterOwned, { 0 },          {  9,  4 },                                    96,          16 },
{ {},         MissileGraphicID::Etherealize,                      1, MissileDataFlags::None,         { 0 },          {  1 },                                        96,          16 },
{ "firerun",  MissileGraphicID::Elemental,                        8, MissileDataFlags::None,         { 1 },          { 12 },                                        96,          16 },
{ "ressur1",  MissileGraphicID::Resurrect,                        1, MissileDataFlags::None,         { 0 },          { 16 },                                        96,          16 },
{ "sklball",  MissileGraphicID::BoneSpirit,                       9, MissileDataFlags::None,         { 1 },          { 16, 16, 16, 16, 16, 16, 16, 16, 8 },         96,          16 },
{ "rportal",  MissileGraphicID::RedPortal,                        2, MissileDataFlags::None,         { 0 },          { 16 },                                        96,          16 },
{ "fireplar", MissileGraphicID::DiabloApocalypseBoom,             1, MissileDataFlags::MonsterOwned, { 1 },          { 17 },                                       160,          48 },
{ "scubmisb", MissileGraphicID::BloodStarBlue,                    1, MissileDataFlags::MonsterOwned, { 0 },          { 16 },                                        96,          16 },
{ "scbsexpb", MissileGraphicID::BloodStarBlueExplosion,           1, MissileDataFlags::MonsterOwned, { 0 },          {  6 },                                       128,          32 },
{ "scubmisc", MissileGraphicID::BloodStarYellow,                  1, MissileDataFlags::MonsterOwned, { 0 },          { 16 },                                        96,          16 },
{ "scbsexpc", MissileGraphicID::BloodStarYellowExplosion,         1, MissileDataFlags::MonsterOwned, { 0 },          {  6 },                                       128,          32 },
{ "scubmisd", MissileGraphicID::BloodStarRed,                     1, MissileDataFlags::MonsterOwned, { 0 },          { 16 },                                        96,          16 },
{ "scbsexpd", MissileGraphicID::BloodStarRedExplosion,            1, MissileDataFlags::MonsterOwned, { 0 },          {  6 },                                       128,          32 },
{ "spawns",   MissileGraphicID::HorkSpawn,                        8, MissileDataFlags::MonsterOwned, { 0 },          {  9 },                                        96,          16 },
{ "reflect",  MissileGraphicID::Reflect,                          1, MissileDataFlags::NotAnimated,  { 0 },          {  1 },                                       160,          64 },
{ "ms_ora",   MissileGraphicID::OrangeFlare,                     16, MissileDataFlags::MonsterOwned, { 0 },          { 15 },                                        96,           8 },
{ "ms_bla",   MissileGraphicID::BlueFlare,                       16, MissileDataFlags::MonsterOwned, { 0 },          { 15 },                                        96,           8 },
{ "ms_reb",   MissileGraphicID::RedFlare,                        16, MissileDataFlags::MonsterOwned, { 0 },          { 15 },                                        96,           8 },
{ "ms_yeb",   MissileGraphicID::YellowFlare,                     16, MissileDataFlags::MonsterOwned, { 0 },          { 15 },                                        96,           8 },
{ "rglows1",  MissileGraphicID::Rune,                             1, MissileDataFlags::None,         { 0 },          { 10 },                                        96,           8 },
{ "ex_yel2",  MissileGraphicID::YellowFlareExplosion,             1, MissileDataFlags::MonsterOwned, { 0 },          { 10 },                                       220,          78 },
{ "ex_blu2",  MissileGraphicID::BlueFlareExplosion,               1, MissileDataFlags::MonsterOwned, { 0 },          { 10 },                                       212,          86 },
{ "ex_red3",  MissileGraphicID::RedFlareExplosion,                1, MissileDataFlags::MonsterOwned, { 0 },          {  7 },                                       292,         114 },
{ "ms_blb",   MissileGraphicID::BlueFlare2,                      16, MissileDataFlags::MonsterOwned, { 0 },          { 15 },                                        96,           8 },
{ "ex_ora1",  MissileGraphicID::OrangeFlareExplosion,             1, MissileDataFlags::MonsterOwned, { 0 },          { 13 },                                        96,         -12 },
{ "ex_blu3",  MissileGraphicID::BlueFlareExplosion2,              1, MissileDataFlags::MonsterOwned, { 0 },          {  7 },                                       292,         114 },
{ {},         MissileGraphicID::None,                             0, MissileDataFlags::None,         {  },           { },                                            0,           0 },
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

MissileFileData::MissileFileData(string_view name, MissileGraphicID animName, uint8_t animFAmt, MissileDataFlags flags,
    std::initializer_list<uint8_t> animDelay, std::initializer_list<uint8_t> animLen,
    uint16_t animWidth, int16_t animWidth2)
    : name(name)
    , animName(animName)
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
