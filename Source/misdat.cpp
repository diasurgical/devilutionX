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
// mAddProc,              mProc,              name,                            mDraw,   mType, damageType,            mFileNum,        mlSFX,       miSFX,       MovementDistribution;
{ &AddArrow,              &MI_Arrow,          MissileID::Arrow,                true,        0, DamageType::Physical,  MFILE_ARROWS,    SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
{ &AddFirebolt,           &MI_Projectile,     MissileID::Firebolt,             true,        1, DamageType::Fire,      MFILE_FIREBA,    LS_FBOLT1,   LS_FIRIMP2,  MissileMovementDistribution::Blockable   },
{ &AddGuardian,           &MI_Guardian,       MissileID::Guardian,             true,        1, DamageType::Physical,  MFILE_GUARD,     LS_GUARD,    LS_GUARDLAN, MissileMovementDistribution::Disabled    },
{ &AddRndTeleport,        &MI_Teleport,       MissileID::Phasing,              false,       1, DamageType::Physical,  MFILE_NONE,      LS_TELEPORT, SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddLightball,          &MI_Lightball,      MissileID::NovaBall,             true,        1, DamageType::Lightning, MFILE_LGHNING,   SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Unblockable },
{ &AddFirewall,           &MI_Firewall,       MissileID::FireWall,             true,        1, DamageType::Fire,      MFILE_FIREWAL,   LS_WALLLOOP, LS_FIRIMP2,  MissileMovementDistribution::Disabled    },
{ &AddFireball,           &MI_Fireball,       MissileID::Fireball,             true,        1, DamageType::Fire,      MFILE_FIREBA,    LS_FBOLT1,   LS_FIRIMP2,  MissileMovementDistribution::Blockable   },
{ &AddLightctrl,          &MI_Lightctrl,      MissileID::LightningControl,     false,       1, DamageType::Lightning, MFILE_LGHNING,   SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddLightning,          &MI_Lightning,      MissileID::Lightning,            true,        1, DamageType::Lightning, MFILE_LGHNING,   LS_LNING1,   LS_ELECIMP1, MissileMovementDistribution::Disabled    },
{ &AddMisexp,             &MI_Misexp,         MissileID::MagmaBallExplosion,   true,        2, DamageType::Physical,  MFILE_MAGBLOS,   SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddTown,               &MI_Town,           MissileID::TownPortal,           true,        1, DamageType::Magic,     MFILE_PORTAL,    LS_SENTINEL, LS_ELEMENTL, MissileMovementDistribution::Disabled    },
{ &AddFlash,              &MI_Flash,          MissileID::FlashBottom,          true,        1, DamageType::Magic,     MFILE_BLUEXFR,   LS_NOVA,     LS_ELECIMP1, MissileMovementDistribution::Disabled    },
{ &AddFlash2,             &MI_Flash2,         MissileID::FlashTop,             true,        1, DamageType::Magic,     MFILE_BLUEXBK,   SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddManashield,         nullptr,            MissileID::ManaShield,           false,       1, DamageType::Magic,     MFILE_MANASHLD,  LS_MSHIELD,  SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddFiremove,           &MI_Firemove,       MissileID::FlameWave,            true,        1, DamageType::Fire,      MFILE_FIREWAL,   SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Unblockable },
{ &AddChain,              &MI_Chain,          MissileID::ChainLightning,       true,        1, DamageType::Lightning, MFILE_LGHNING,   LS_LNING1,   LS_ELECIMP1, MissileMovementDistribution::Disabled    },
{ nullptr,                nullptr,            MissileID::ChainBall,            true,        1, DamageType::Lightning, MFILE_LGHNING,   SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ nullptr,                nullptr,            MissileID::BloodHit,             true,        2, DamageType::Physical,  MFILE_BLOOD,     LS_BLODSTAR, LS_BLSIMPT,  MissileMovementDistribution::Disabled    },
{ nullptr,                nullptr,            MissileID::BoneHit,              true,        2, DamageType::Physical,  MFILE_BONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ nullptr,                nullptr,            MissileID::MetalHit,             true,        2, DamageType::Physical,  MFILE_METLHIT,   SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddRhino,              &MI_Rhino,          MissileID::Rhino,                true,        2, DamageType::Physical,  MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
{ &AddMagmaball,          &MI_Projectile,     MissileID::MagmaBall,            true,        1, DamageType::Fire,      MFILE_MAGBALL,   SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
{ &AddLightctrl,          &MI_Lightctrl,      MissileID::ThinLightningControl, false,       1, DamageType::Lightning, MFILE_THINLGHT,  SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddLightning,          &MI_Lightning,      MissileID::ThinLightning,        true,        1, DamageType::Lightning, MFILE_THINLGHT,  SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddFlare,              &MI_Projectile,     MissileID::BloodStar,            true,        1, DamageType::Magic,     MFILE_FLARE,     SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
{ &AddMisexp,             &MI_Misexp,         MissileID::BloodStarExplosion,   true,        2, DamageType::Magic,     MFILE_FLAREEXP,  SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddTeleport,           &MI_Teleport,       MissileID::Teleport,             false,       1, DamageType::Physical,  MFILE_NONE,      LS_ELEMENTL, SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddLArrow,             &MI_LArrow,         MissileID::FireArrow,            true,        0, DamageType::Fire,      MFILE_FARROW,    SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
{ nullptr,                nullptr,            MissileID::DoomSerpents,         false,       1, DamageType::Magic,     MFILE_DOOM,      LS_DSERP,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ nullptr,                nullptr,            MissileID::FireOnly,             true,        2, DamageType::Fire,      MFILE_FIREWAL,   SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddStone,              &MI_Stone,          MissileID::StoneCurse,           false,       1, DamageType::Magic,     MFILE_NONE,      LS_SCURIMP,  SFX_NONE,    MissileMovementDistribution::Disabled    },
{ nullptr,                nullptr,            MissileID::BloodRitual,          true,        1, DamageType::Physical,  MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ nullptr,                nullptr,            MissileID::Invisibility,         false,       1, DamageType::Physical,  MFILE_NONE,      LS_INVISIBL, SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddGolem,              nullptr,            MissileID::Golem,                false,       1, DamageType::Physical,  MFILE_NONE,      LS_GOLUM,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ nullptr,                nullptr,            MissileID::Etherealize,          true,        1, DamageType::Physical,  MFILE_ETHRSHLD,  LS_ETHEREAL, SFX_NONE,    MissileMovementDistribution::Disabled    },
{ nullptr,                nullptr,            MissileID::Spurt,                true,        2, DamageType::Physical,  MFILE_BLODBUR,   SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddBoom,               &MI_Boom,           MissileID::ApocalypseBoom,       true,        2, DamageType::Physical,  MFILE_NEWEXP,    SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddHeal,               nullptr,            MissileID::Healing,              false,       1, DamageType::Physical,  MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddFirewallC,          &MI_FirewallC,      MissileID::FireWallControl,      false,       1, DamageType::Fire,      MFILE_FIREWAL,   SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddInfra,              &MI_Infra,          MissileID::Infravision,          false,       1, DamageType::Physical,  MFILE_NONE,      LS_INFRAVIS, SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddIdentify,           nullptr,            MissileID::Identify,             false,       1, DamageType::Physical,  MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddWave,               &MI_Wave,           MissileID::FlameWaveControl,     true,        1, DamageType::Fire,      MFILE_FIREWAL,   LS_FLAMWAVE, SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddNova,               &MI_Nova,           MissileID::Nova,                 true,        1, DamageType::Lightning, MFILE_LGHNING,   LS_NOVA,     SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddBlodboil,           &MI_Blodboil,       MissileID::Rage,                 false,       1, DamageType::Physical,  MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddApoca,              &MI_Apoca,          MissileID::Apocalypse,           true,        1, DamageType::Magic,     MFILE_NEWEXP,    LS_APOC,     SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddRepair,             nullptr,            MissileID::ItemRepair,           false,       2, DamageType::Physical,  MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddRecharge,           nullptr,            MissileID::StaffRecharge,        false,       2, DamageType::Physical,  MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddDisarm,             nullptr,            MissileID::TrapDisarm,           false,       2, DamageType::Physical,  MFILE_NONE,      LS_TRAPDIS,  SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddFlame,              &MI_Flame,          MissileID::Inferno,              true,        1, DamageType::Fire,      MFILE_INFERNO,   LS_SPOUTSTR, SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddFlamec,             &MI_Flamec,         MissileID::InfernoControl,       false,       1, DamageType::Fire,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ nullptr,                nullptr,            MissileID::FireMan,              true,        2, DamageType::Physical,  MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
{ nullptr,                nullptr,            MissileID::Krull,                true,        0, DamageType::Fire,      MFILE_KRULL,     SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
{ &AddCbolt,              &MI_Cbolt,          MissileID::ChargedBolt,          true,        1, DamageType::Lightning, MFILE_MINILTNG,  LS_CBOLT,    SFX_NONE,    MissileMovementDistribution::Blockable   },
{ &AddHbolt,              &MI_Hbolt,          MissileID::HolyBolt,             true,        1, DamageType::Physical,  MFILE_HOLY,      LS_HOLYBOLT, LS_ELECIMP1, MissileMovementDistribution::Blockable   },
{ &AddResurrect,          nullptr,            MissileID::Resurrect,            false,       1, DamageType::Magic,     MFILE_NONE,      SFX_NONE,    LS_RESUR,    MissileMovementDistribution::Disabled    },
{ &AddTelekinesis,        nullptr,            MissileID::Telekinesis,          false,       1, DamageType::Physical,  MFILE_NONE,      LS_ETHEREAL, SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddLArrow,             &MI_LArrow,         MissileID::LightningArrow,       true,        0, DamageType::Lightning, MFILE_LARROW,    SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
{ &AddAcid,               &MI_Projectile,     MissileID::Acid,                 true,        1, DamageType::Acid,      MFILE_ACIDBF,    LS_ACID,     SFX_NONE,    MissileMovementDistribution::Blockable   },
{ &AddMisexp,             &MI_Acidsplat,      MissileID::AcidSplat,            true,        2, DamageType::Acid,      MFILE_ACIDSPLA,  SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddAcidpud,            &MI_Acidpud,        MissileID::AcidPuddle,           true,        2, DamageType::Acid,      MFILE_ACIDPUD,   LS_PUDDLE,   SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddHealOther,          nullptr,            MissileID::HealOther,            false,       1, DamageType::Physical,  MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddElement,            &MI_Element,        MissileID::Elemental,            true,        1, DamageType::Fire,      MFILE_FIRERUN,   LS_ELEMENTL, SFX_NONE,    MissileMovementDistribution::Unblockable },
{ &AddResurrectBeam,      &MI_ResurrectBeam,  MissileID::ResurrectBeam,        true,        1, DamageType::Physical,  MFILE_RESSUR1,   SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddBoneSpirit,         &MI_Bonespirit,     MissileID::BoneSpirit,           true,        1, DamageType::Magic,     MFILE_SKLBALL,   LS_BONESP,   LS_BSIMPCT,  MissileMovementDistribution::Blockable   },
{ &AddWeapexp,            &MI_Weapexp,        MissileID::WeaponExplosion,      true,        2, DamageType::Physical,  MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddRportal,            &MI_Rportal,        MissileID::RedPortal,            true,        2, DamageType::Physical,  MFILE_RPORTAL,   LS_SENTINEL, LS_ELEMENTL, MissileMovementDistribution::Disabled    },
{ &AddBoom,               &MI_Boom,           MissileID::DiabloApocalypseBoom, true,        2, DamageType::Physical,  MFILE_FIREPLAR,  SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddDiabApoca,          nullptr,            MissileID::DiabloApocalypse,     false,       2, DamageType::Physical,  MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddMana,               nullptr,            MissileID::Mana,                 false,       1, DamageType::Physical,  MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddMagi,               nullptr,            MissileID::Magi,                 false,       1, DamageType::Physical,  MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddLightningWall,      &MI_LightningWall,  MissileID::LightningWall,        true,        1, DamageType::Lightning, MFILE_LGHNING,   LS_LMAG,     LS_ELECIMP1, MissileMovementDistribution::Disabled    },
{ &AddFirewallC,          &MI_LightningWallC, MissileID::LightningWallControl, false,       1, DamageType::Lightning, MFILE_LGHNING,   SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddNova,               &MI_FireNova,       MissileID::Immolation,           true,        1, DamageType::Fire,      MFILE_FIREBA,    LS_FBOLT1,   LS_FIRIMP2,  MissileMovementDistribution::Disabled    },
{ &AddSpecArrow,          &MI_SpecArrow,      MissileID::SpectralArrow,        true,        0, DamageType::Physical,  MFILE_ARROWS,    SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddFireNova,           &MI_Fireball,       MissileID::FireballBow,          true,        1, DamageType::Fire,      MFILE_FIREBA,    IS_FBALLBOW, LS_FIRIMP2,  MissileMovementDistribution::Blockable   },
{ &AddLightningArrow,     &MI_LightningArrow, MissileID::LightningBow,         false,       1, DamageType::Lightning, MFILE_LGHNING,   IS_FBALLBOW, SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddCboltArrow,         &MI_Cbolt,          MissileID::ChargedBoltBow,       true,        1, DamageType::Lightning, MFILE_MINILTNG,  LS_CBOLT,    SFX_NONE,    MissileMovementDistribution::Blockable   },
{ &AddHbolt,              &MI_Hbolt,          MissileID::HolyBoltBow,          true,        1, DamageType::Physical,  MFILE_HOLY,      LS_HOLYBOLT, LS_ELECIMP1, MissileMovementDistribution::Blockable   },
{ &AddWarp,               &MI_Teleport,       MissileID::Warp,                 false,       1, DamageType::Physical,  MFILE_NONE,      LS_ETHEREAL, SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddReflection,         nullptr,            MissileID::Reflect,              false,       1, DamageType::Physical,  MFILE_REFLECT,   LS_MSHIELD,  SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddBerserk,            nullptr,            MissileID::Berserk,              false,       1, DamageType::Physical,  MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddRing,               &MI_FireRing,       MissileID::RingOfFire,           false,       1, DamageType::Fire,      MFILE_FIREWAL,   SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddStealPotions,       nullptr,            MissileID::StealPotions,         false,       1, DamageType::Physical,  MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddManaTrap,           nullptr,            MissileID::StealMana,            false,       1, DamageType::Physical,  MFILE_NONE,      IS_CAST7,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ nullptr,                nullptr,            MissileID::RingOfLightning,      false,       1, DamageType::Lightning, MFILE_LGHNING,   SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddSearch,             &MI_Search,         MissileID::Search,               false,       1, DamageType::Physical,  MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ nullptr,                nullptr,            MissileID::Aura,                 false,       1, DamageType::Magic,     MFILE_BLUEXFR,   SFX_NONE,    LS_ELECIMP1, MissileMovementDistribution::Disabled    },
{ nullptr,                nullptr,            MissileID::Aura2,                false,       1, DamageType::Magic,     MFILE_BLUEXBK,   SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ nullptr,                nullptr,            MissileID::SpiralFireball,       true,        1, DamageType::Fire,      MFILE_FIREBA,    LS_FBOLT1,   LS_FIRIMP2,  MissileMovementDistribution::Disabled    },
{ &AddFireRune,           &MI_Rune,           MissileID::RuneOfFire,           true,        1, DamageType::Physical,  MFILE_RUNE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddLightningRune,      &MI_Rune,           MissileID::RuneOfLight,          true,        1, DamageType::Physical,  MFILE_RUNE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddGreatLightningRune, &MI_Rune,           MissileID::RuneOfNova,           true,        1, DamageType::Physical,  MFILE_RUNE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddImmolationRune,     &MI_Rune,           MissileID::RuneOfImmolation,     true,        1, DamageType::Physical,  MFILE_RUNE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddStoneRune,          &MI_Rune,           MissileID::RuneOfStone,          true,        1, DamageType::Physical,  MFILE_RUNE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddRuneExplosion,      &MI_HiveExplode,    MissileID::BigExplosion,         true,        1, DamageType::Fire,      MFILE_BIGEXP,    LS_NESTXPLD, LS_NESTXPLD, MissileMovementDistribution::Disabled    },
{ &AddHorkSpawn,          &MI_HorkSpawn,      MissileID::HorkSpawn,            false,       2, DamageType::Physical,  MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddJester,             nullptr,            MissileID::Jester,               false,       2, DamageType::Physical,  MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddHiveExplosion,      nullptr,            MissileID::OpenNest,             false,       2, DamageType::Physical,  MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddFlare,              &MI_Projectile,     MissileID::OrangeFlare,          true,        1, DamageType::Magic,     MFILE_LICH,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
{ &AddFlare,              &MI_Projectile,     MissileID::BlueFlare,            true,        1, DamageType::Magic,     MFILE_BONEDEMON, SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
{ &AddFlare,              &MI_Projectile,     MissileID::RedFlare,             true,        1, DamageType::Magic,     MFILE_NECROMORB, SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
{ &AddFlare,              &MI_Projectile,     MissileID::YellowFlare,          true,        1, DamageType::Magic,     MFILE_ARCHLICH,  SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
{ &AddFlare,              &MI_Projectile,     MissileID::BlueFlare2,           true,        1, DamageType::Magic,     MFILE_BONEDEMON, SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
{ &AddMisexp,             &MI_Misexp,         MissileID::YellowExplosion,      true,        2, DamageType::Physical,  MFILE_EXYEL2,    LS_FIRIMP2,  SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddMisexp,             &MI_Misexp,         MissileID::RedExplosion,         true,        2, DamageType::Physical,  MFILE_EXRED3,    LS_FIRIMP2,  SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddMisexp,             &MI_Misexp,         MissileID::BlueExplosion,        true,        2, DamageType::Physical,  MFILE_EXBL2,     LS_FIRIMP2,  SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddMisexp,             &MI_Misexp,         MissileID::BlueExplosion2,       true,        2, DamageType::Physical,  MFILE_EXBL3,     LS_FIRIMP2,  SFX_NONE,    MissileMovementDistribution::Disabled    },
{ &AddMisexp,             &MI_Misexp,         MissileID::OrangeExplosion,      true,        2, DamageType::Physical,  MFILE_EXORA1,    LS_FIRIMP2,  SFX_NONE,    MissileMovementDistribution::Disabled    },
	// clang-format on
};

/** Data related to each missile graphic ID. */
MissileFileData MissileSpriteData[] = {
	// clang-format off
// name,      animName,         animFAmt, flags,                          animDelay[16],  animLen[16],                            animWidth,  animWidth2
{ "arrows",   MFILE_ARROWS,            1, MissileDataFlags::NotAnimated,  { 0 },          { 16 },                                        96,          16 },
{ "fireba",   MFILE_FIREBA,           16, MissileDataFlags::None,         { 0 },          { 14 },                                        96,          16 },
{ "guard",    MFILE_GUARD,             3, MissileDataFlags::None,         { 1 },          { 15, 14,  3 },                                96,          16 },
{ "lghning",  MFILE_LGHNING,           1, MissileDataFlags::None,         { 0 },          {  8 },                                        96,          16 },
{ "firewal",  MFILE_FIREWAL,           2, MissileDataFlags::None,         { 0 },          { 13, 11 },                                   128,          32 },
{ "magblos",  MFILE_MAGBLOS,           1, MissileDataFlags::None,         { 1 },          { 10 },                                       128,          32 },
{ "portal",   MFILE_PORTAL,            2, MissileDataFlags::None,         { 0, 1 },       { 16 },                                        96,          16 },
{ "bluexfr",  MFILE_BLUEXFR,           1, MissileDataFlags::None,         { 0 },          { 19 },                                       160,          48 },
{ "bluexbk",  MFILE_BLUEXBK,           1, MissileDataFlags::None,         { 0 },          { 19 },                                       160,          48 },
{ "manashld", MFILE_MANASHLD,          1, MissileDataFlags::NotAnimated,  { 0 },          {  1 },                                        96,          16 },
{ {},         MFILE_BLOOD,             4, MissileDataFlags::None,         { 0 },          { 15 },                                        96,          16 },
{ {},         MFILE_BONE,              3, MissileDataFlags::None,         { 2 },          {  8 },                                       128,          32 },
{ {},         MFILE_METLHIT,           3, MissileDataFlags::None,         { 2 },          { 10 },                                        96,          16 },
{ "farrow",   MFILE_FARROW,           16, MissileDataFlags::None,         { 0 },          {  4 },                                        96,          16 },
{ "doom",     MFILE_DOOM,              9, MissileDataFlags::MonsterOwned, { 1 },          { 15 },                                        96,          16 },
{ {},         MFILE_0F,                1, MissileDataFlags::MonsterOwned, { 0 },          {  0 },                                         0,           0 },
{ {},         MFILE_BLODBUR,           2, MissileDataFlags::None,         { 2 },          {  8 },                                       128,          32 },
{ "newexp",   MFILE_NEWEXP,            1, MissileDataFlags::None,         { 1 },          { 15 },                                        96,          16 },
{ "shatter1", MFILE_SHATTER1,          1, MissileDataFlags::None,         { 1 },          { 12 },                                       128,          32 },
{ "bigexp",   MFILE_BIGEXP,            1, MissileDataFlags::None,         { 0 },          { 15 },                                       160,          48 },
{ "inferno",  MFILE_INFERNO,           1, MissileDataFlags::None,         { 0 },          { 20 },                                        96,          16 },
{ "thinlght", MFILE_THINLGHT,          1, MissileDataFlags::MonsterOwned, { 0 },          {  8 },                                        96,          16 },
{ "flare",    MFILE_FLARE,             1, MissileDataFlags::None,         { 0 },          { 16 },                                       128,          32 },
{ "flareexp", MFILE_FLAREEXP,          1, MissileDataFlags::None,         { 0 },          {  7 },                                       128,          32 },
{ "magball",  MFILE_MAGBALL,           8, MissileDataFlags::MonsterOwned, { 1 },          { 16 },                                       128,          32 },
{ "krull",    MFILE_KRULL,             1, MissileDataFlags::MonsterOwned, { 0 },          { 14 },                                        96,          16 },
{ "miniltng", MFILE_MINILTNG,          1, MissileDataFlags::None,         { 1 },          {  8 },                                        64,           0 },
{ "holy",     MFILE_HOLY,             16, MissileDataFlags::None,         { 1, 0 },       { 14 },                                        96,          16 },
{ "holyexpl", MFILE_HOLYEXPL,          1, MissileDataFlags::None,         { 0 },          {  8 },                                       160,          48 },
{ "larrow",   MFILE_LARROW,           16, MissileDataFlags::None,         { 0 },          {  4 },                                        96,          16 },
{ {},         MFILE_FIRARWEX,          1, MissileDataFlags::None,         { 0 },          {  6 },                                        64,           0 },
{ "acidbf",   MFILE_ACIDBF,           16, MissileDataFlags::MonsterOwned, { 0 },          {  8 },                                        96,          16 },
{ "acidspla", MFILE_ACIDSPLA,          1, MissileDataFlags::MonsterOwned, { 0 },          {  8 },                                        96,          16 },
{ "acidpud",  MFILE_ACIDPUD,           2, MissileDataFlags::MonsterOwned, { 0 },          {  9,  4 },                                    96,          16 },
{ {},         MFILE_ETHRSHLD,          1, MissileDataFlags::None,         { 0 },          {  1 },                                        96,          16 },
{ "firerun",  MFILE_FIRERUN,           8, MissileDataFlags::None,         { 1 },          { 12 },                                        96,          16 },
{ "ressur1",  MFILE_RESSUR1,           1, MissileDataFlags::None,         { 0 },          { 16 },                                        96,          16 },
{ "sklball",  MFILE_SKLBALL,           9, MissileDataFlags::None,         { 1 },          { 16, 16, 16, 16, 16, 16, 16, 16, 8 },         96,          16 },
{ "rportal",  MFILE_RPORTAL,           2, MissileDataFlags::None,         { 0 },          { 16 },                                        96,          16 },
{ "fireplar", MFILE_FIREPLAR,          1, MissileDataFlags::MonsterOwned, { 1 },          { 17 },                                       160,          48 },
{ "scubmisb", MFILE_SCUBMISB,          1, MissileDataFlags::MonsterOwned, { 0 },          { 16 },                                        96,          16 },
{ "scbsexpb", MFILE_SCBSEXPB,          1, MissileDataFlags::MonsterOwned, { 0 },          {  6 },                                       128,          32 },
{ "scubmisc", MFILE_SCUBMISC,          1, MissileDataFlags::MonsterOwned, { 0 },          { 16 },                                        96,          16 },
{ "scbsexpc", MFILE_SCBSEXPC,          1, MissileDataFlags::MonsterOwned, { 0 },          {  6 },                                       128,          32 },
{ "scubmisd", MFILE_SCUBMISD,          1, MissileDataFlags::MonsterOwned, { 0 },          { 16 },                                        96,          16 },
{ "scbsexpd", MFILE_SCBSEXPD,          1, MissileDataFlags::MonsterOwned, { 0 },          {  6 },                                       128,          32 },
{ "spawns",   MFILE_SPAWNS,            8, MissileDataFlags::MonsterOwned, { 0 },          {  9 },                                        96,          16 },
{ "reflect",  MFILE_REFLECT,           1, MissileDataFlags::NotAnimated,  { 0 },          {  1 },                                       160,          64 },
{ "ms_ora",   MFILE_LICH,             16, MissileDataFlags::MonsterOwned, { 0 },          { 15 },                                        96,           8 },
{ "ms_bla",   MFILE_MSBLA,            16, MissileDataFlags::MonsterOwned, { 0 },          { 15 },                                        96,           8 },
{ "ms_reb",   MFILE_NECROMORB,        16, MissileDataFlags::MonsterOwned, { 0 },          { 15 },                                        96,           8 },
{ "ms_yeb",   MFILE_ARCHLICH,         16, MissileDataFlags::MonsterOwned, { 0 },          { 15 },                                        96,           8 },
{ "rglows1",  MFILE_RUNE,              1, MissileDataFlags::None,         { 0 },          { 10 },                                        96,           8 },
{ "ex_yel2",  MFILE_EXYEL2,            1, MissileDataFlags::MonsterOwned, { 0 },          { 10 },                                       220,          78 },
{ "ex_blu2",  MFILE_EXBL2,             1, MissileDataFlags::MonsterOwned, { 0 },          { 10 },                                       212,          86 },
{ "ex_red3",  MFILE_EXRED3,            1, MissileDataFlags::MonsterOwned, { 0 },          {  7 },                                       292,         114 },
{ "ms_blb",   MFILE_BONEDEMON,        16, MissileDataFlags::MonsterOwned, { 0 },          { 15 },                                        96,           8 },
{ "ex_ora1",  MFILE_EXORA1,            1, MissileDataFlags::MonsterOwned, { 0 },          { 13 },                                        96,         -12 },
{ "ex_blu3",  MFILE_EXBL3,             1, MissileDataFlags::MonsterOwned, { 0 },          {  7 },                                       292,         114 },
{ {},         MFILE_NONE,              0, MissileDataFlags::None,         {  },           { },                                            0,           0 },
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

MissileFileData::MissileFileData(string_view name, uint8_t animName, uint8_t animFAmt, MissileDataFlags flags,
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
		if (!loadHellfireGraphics && mi > MFILE_SCBSEXPD)
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
