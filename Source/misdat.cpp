/**
 * @file misdat.cpp
 *
 * Implementation of data related to missiles.
 */
#include "misdat.h"

#include "engine/cel_header.hpp"
#include "engine/load_file.hpp"
#include "missiles.h"

namespace devilution {

/** Data related to each missile ID. */
MissileData MissilesData[] = {
	// clang-format off
	// mAddProc,                   mProc,               mName,              mDraw, mType, mResist,        mFileNum,        miSFX,       mlSFX,       MovementDistribution;
	{  &AddArrow,                  &MI_Arrow,           MIS_ARROW,          true,      0, MISR_NONE,      MFILE_ARROWS,    SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Blockable   },
	{  &AddFirebolt,               &MI_Projectile,      MIS_FIREBOLT,       true,      1, MISR_FIRE,      MFILE_FIREBA,    LS_FBOLT1,   LS_FIRIMP2,  MissileMovementDistrubution::Blockable   },
	{  &AddGuardian,               &MI_Guardian,        MIS_GUARDIAN,       true,      1, MISR_NONE,      MFILE_GUARD,     LS_GUARD,    LS_GUARDLAN, MissileMovementDistrubution::Disabled    },
	{  &AddPhasing,                &MI_Teleport,        MIS_PHASING,        false,     1, MISR_NONE,      MFILE_NONE,      LS_TELEPORT, SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddNova,                   &MI_Nova,            MIS_NOVA,           true,      1, MISR_LIGHTNING, MFILE_LGHNING,   SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Unblockable },
	{  &AddFireWall,               &MI_FireWall,        MIS_FIREWALL,       true,      1, MISR_FIRE,      MFILE_FIREWAL,   LS_WALLLOOP, LS_FIRIMP2,  MissileMovementDistrubution::Disabled    },
	{  &AddFireball,               &MI_Fireball,        MIS_FIREBALL,       true,      1, MISR_FIRE,      MFILE_FIREBA,    LS_FBOLT1,   LS_FIRIMP2,  MissileMovementDistrubution::Blockable   },
	{  &AddLightningC,             &MI_LightningC,      MIS_LIGHTNINGC,     false,     1, MISR_LIGHTNING, MFILE_LGHNING,   SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddLightning,              &MI_Lightning,       MIS_LIGHTNING,      true,      1, MISR_LIGHTNING, MFILE_LGHNING,   LS_LNING1,   LS_ELECIMP1, MissileMovementDistrubution::Disabled    },
	{  &AddMissileHit,             &MI_MissileHit,      MIS_MISSILEHIT,     true,      2, MISR_NONE,      MFILE_MAGBLOS,   SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddTownPortal,             &MI_TownPortal,      MIS_TOWNPORTAL,     true,      1, MISR_MAGIC,     MFILE_PORTAL,    LS_SENTINEL, LS_ELEMENTL, MissileMovementDistrubution::Disabled    },
	{  &AddFlash,                  &MI_Flash,           MIS_FLASH,          true,      1, MISR_MAGIC,     MFILE_BLUEXFR,   LS_NOVA,     LS_ELECIMP1, MissileMovementDistrubution::Disabled    },
	{  &AddFlash2,                 &MI_Flash2,          MIS_FLASH2,         true,      1, MISR_MAGIC,     MFILE_BLUEXBK,   SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddManaShield,             nullptr,             MIS_MANASHIELD,     false,     1, MISR_MAGIC,     MFILE_MANASHLD,  LS_MSHIELD,  SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddFlameWave,              &MI_FlameWave,       MIS_FLAMEWAVE,      true,      1, MISR_FIRE,      MFILE_FIREWAL,   SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Unblockable },
	{  &AddChainLightning,         &MI_ChainLightning,  MIS_CHAINLIGHTNING, true,      1, MISR_LIGHTNING, MFILE_LGHNING,   LS_LNING1,   LS_ELECIMP1, MissileMovementDistrubution::Disabled    },
	{  nullptr,                    nullptr,             MIS_NULL_10,        false,     0, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  nullptr,                    nullptr,             MIS_NULL_11,        false,     0, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  nullptr,                    nullptr,             MIS_NULL_12,        false,     0, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  nullptr,                    nullptr,             MIS_NULL_13,        false,     0, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddRhino,                  &MI_Rhino,           MIS_RHINO,          true,      2, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Blockable   },
	{  &AddMagmaBall,              &MI_Projectile,      MIS_MAGMABALL,      true,      1, MISR_FIRE,      MFILE_MAGBALL,   SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Blockable   },
	{  &AddLightningC,             &MI_LightningC,      MIS_MLIGHTNINGC,    false,     1, MISR_LIGHTNING, MFILE_THINLGHT,  SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddLightning,              &MI_Lightning,       MIS_MLIGHTNING,     true,      1, MISR_LIGHTNING, MFILE_THINLGHT,  SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddBloodStar,              &MI_Projectile,      MIS_BLOODSTAR,      true,      1, MISR_MAGIC,     MFILE_FLARE,     SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Blockable   },
	{  &AddMissileHit,             &MI_MissileHit,      MIS_MISSILEHIT2,    true,      2, MISR_MAGIC,     MFILE_FLAREEXP,  SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddTeleport,               &MI_Teleport,        MIS_TELEPORT,       false,     1, MISR_NONE,      MFILE_NONE,      LS_ELEMENTL, SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddElementArrow,           &MI_ElementArrow,    MIS_FIREARROW,      true,      0, MISR_FIRE,      MFILE_FARROW,    SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Blockable   },
	{  nullptr,                    nullptr,             MIS_NULL_1C,        false,     0, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  nullptr,                    nullptr,             MIS_NULL_1D,        false,     0, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddStoneCurse,             &MI_StoneCurse,      MIS_STONECURSE,     false,     1, MISR_MAGIC,     MFILE_NONE,      LS_SCURIMP,  SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  nullptr,                    nullptr,             MIS_NULL_1F,        false,     0, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  nullptr,                    nullptr,             MIS_NULL_20,        false,     0, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddGolem,                  nullptr,             MIS_GOLEM,          false,     1, MISR_NONE,      MFILE_NONE,      LS_GOLUM,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  nullptr,                    nullptr,             MIS_NULL_22,        false,     0, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  nullptr,                    nullptr,             MIS_NULL_23,        false,     0, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddApocalypse,             &MI_Apocalypse,      MIS_APOCALYPSE,     true,      2, MISR_NONE,      MFILE_NEWEXP,    SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddHealing,                nullptr,             MIS_HEALING,        false,     1, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddFireWallC,              &MI_FireWallC,       MIS_FIREWALLC,      false,     1, MISR_FIRE,      MFILE_FIREWAL,   SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddInfravision,            &MI_Infravision,     MIS_INFRAVISION,    false,     1, MISR_NONE,      MFILE_NONE,      LS_INFRAVIS, SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddIdentify,               nullptr,             MIS_IDENTIFY,       false,     1, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddFlameWaveC,             &MI_FlameWaveC,      MIS_FLAMEWAVEC,     true,      1, MISR_FIRE,      MFILE_FIREWAL,   LS_FLAMWAVE, SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddNova,                   &MI_Nova,            MIS_NOVA,           true,      1, MISR_LIGHTNING, MFILE_LGHNING,   LS_NOVA,     SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddRage,                   &MI_Rage,            MIS_RAGE,           false,     1, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddApocalypseC,            &MI_ApocalypseC,     MIS_APOCALYPSEC,    true,      1, MISR_MAGIC,     MFILE_NEWEXP,    LS_APOC,     SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddItemRepair,             nullptr,             MIS_ITEMREPAIR,     false,     2, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddStaffRecharge,          nullptr,             MIS_STAFFRECHARGE,  false,     2, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddDisarmTrap,             nullptr,             MIS_DISARMTRAP,     false,     2, MISR_NONE,      MFILE_NONE,      LS_TRAPDIS,  SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddInferno,                &MI_Inferno,         MIS_INFERNO,        true,      1, MISR_FIRE,      MFILE_INFERNO,   LS_SPOUTSTR, SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddInfernoC,               &MI_InfernoC,        MIS_INFERNOC,       false,     1, MISR_FIRE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  nullptr,                    nullptr,             MIS_NULL_32,        false,     0, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  nullptr,                    nullptr,             MIS_NULL_33,        false,     0, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddChargedBolt,            &MI_ChargedBolt,     MIS_CHARGEDBOLT,    true,      1, MISR_LIGHTNING, MFILE_MINILTNG,  LS_CBOLT,    SFX_NONE,    MissileMovementDistrubution::Blockable   },
	{  &AddHolyBolt,               &MI_HolyBolt,        MIS_HOLYBOLT,       true,      1, MISR_NONE,      MFILE_HOLY,      LS_HOLYBOLT, LS_ELECIMP1, MissileMovementDistrubution::Blockable   },
	{  &AddResurrect,              nullptr,             MIS_RESURRECT,      false,     1, MISR_MAGIC,     MFILE_NONE,      SFX_NONE,    LS_RESUR,    MissileMovementDistrubution::Disabled    },
	{  &AddTelekinesis,            nullptr,             MIS_TELEKINESIS,    false,     1, MISR_NONE,      MFILE_NONE,      LS_ETHEREAL, SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddElementArrow,           &MI_ElementArrow,    MIS_LIGHTNINGARROW, true,      0, MISR_LIGHTNING, MFILE_LARROW,    SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Blockable   },
	{  &AddAcid,                   &MI_Projectile,      MIS_ACID,           true,      1, MISR_ACID,      MFILE_ACIDBF,    LS_ACID,     SFX_NONE,    MissileMovementDistrubution::Blockable   },
	{  &AddMissileHit,             &MI_AcidHit,         MIS_MISSILEHIT3,    true,      2, MISR_ACID,      MFILE_ACIDSPLA,  SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddAcidPuddle,             &MI_AcidPuddle,      MIS_ACIDPUDDLE,     true,      2, MISR_ACID,      MFILE_ACIDPUD,   LS_PUDDLE,   SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddHealOther,              nullptr,             MIS_HEALOTHER,      false,     1, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddElemental,              &MI_Elemental,       MIS_ELEMENTAL,      true,      1, MISR_FIRE,      MFILE_FIRERUN,   LS_ELEMENTL, SFX_NONE,    MissileMovementDistrubution::Unblockable },
	{  &AddResurrectBeam,          &MI_ResurrectBeam,   MIS_RESURRECTBEAM,  true,      1, MISR_NONE,      MFILE_RESSUR1,   SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddBoneSpirit,             &MI_BoneSpirit,      MIS_BONESPIRIT,     true,      1, MISR_MAGIC,     MFILE_SKLBALL,   LS_BONESP,   LS_BSIMPCT,  MissileMovementDistrubution::Blockable   },
	{  &AddElementMeleeHit,        &MI_ElementMeleeHit, MIS_ELEMENTMELEEHIT, true,      2, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddRedPortal,              &MI_RedPortal,       MIS_REDPORTAL,       true,      2, MISR_NONE,      MFILE_RPORTAL,   LS_SENTINEL, LS_ELEMENTL, MissileMovementDistrubution::Disabled    },
	{  &AddApocalypse,             &MI_Apocalypse,      MIS_DAPOCALYPSE,     true,      2, MISR_NONE,      MFILE_FIREPLAR,  SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddDApocalypseC,           nullptr,             MIS_DAPOCALYPSEC,    false,     2, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddMana,                   nullptr,             MIS_MANA,            false,     1, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddMagi,                   nullptr,             MIS_MAGI,            false,     1, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddLightningWall,          &MI_LightningWall,   MIS_LIGHTNINGWALL,   true,      1, MISR_LIGHTNING, MFILE_LGHNING,   LS_LMAG,     LS_ELECIMP1, MissileMovementDistrubution::Disabled    },
	{  &AddFireWallC,              &MI_LightningWallC,  MIS_LIGHTNINGWALLC,  false,     1, MISR_LIGHTNING, MFILE_LGHNING,   SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddImmolationC,            &MI_ImmolationC,     MIS_IMMOLATIONC,     true,      1, MISR_FIRE,      MFILE_FIREBA,    LS_FBOLT1,   LS_FIRIMP2,  MissileMovementDistrubution::Disabled    },
	{  &AddSpecArrow,              &MI_SpecArrow,       MIS_SPECARROW,       true,      0, MISR_NONE,      MFILE_ARROWS,    SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddImmolation,             &MI_Fireball,        MIS_IMMOLATION,      true,      1, MISR_FIRE,      MFILE_FIREBA,    IS_FBALLBOW, LS_FIRIMP2,  MissileMovementDistrubution::Blockable   },
	{  &AddBowLightning,           &MI_BowLightning,    MIS_BOWLIGHTNING,    false,     1, MISR_LIGHTNING, MFILE_LGHNING,   IS_FBALLBOW, SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddBowChargedBolt,         &MI_ChargedBolt,     MIS_BOWCHARGEDBOLT,  true,      1, MISR_LIGHTNING, MFILE_MINILTNG,  LS_CBOLT,    SFX_NONE,    MissileMovementDistrubution::Blockable   },
	{  &AddHolyBolt,               &MI_HolyBolt,        MIS_BOWHOLYBOLT,     true,      1, MISR_NONE,      MFILE_HOLY,      LS_HOLYBOLT, LS_ELECIMP1, MissileMovementDistrubution::Blockable   },
	{  &AddWarp,                   &MI_Teleport,        MIS_WARP,            false,     1, MISR_NONE,      MFILE_NONE,      LS_ETHEREAL, SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddReflect,                nullptr,             MIS_REFLECT,         false,     1, MISR_NONE,      MFILE_REFLECT,   LS_MSHIELD,  SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddBerserk,                nullptr,             MIS_BERSERK,         false,     1, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddRingOfFire,             &MI_RingOfFire,      MIS_RINGOFFIRE,      false,     1, MISR_FIRE,      MFILE_FIREWAL,   SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddStealPotions,           nullptr,             MIS_STEALPOTS,       false,     1, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddManaTrap,               nullptr,             MIS_MANATRAP,        false,     1, MISR_NONE,      MFILE_NONE,      IS_CAST7,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  nullptr,                    nullptr,             MIS_NULL_54,         false,     0, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddSearch,                 &MI_Search,          MIS_SEARCH,          false,     1, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  nullptr,                    nullptr,             MIS_NULL_56,         false,     0, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  nullptr,                    nullptr,             MIS_NULL_57,         false,     0, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  nullptr,                    nullptr,             MIS_NULL_58,         false,     0, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddFireRune,               &MI_Rune,            MIS_RUNEFIRE,        true,      1, MISR_NONE,      MFILE_RUNE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddLightningRune,          &MI_Rune,            MIS_RUNELIGHT,       true,      1, MISR_NONE,      MFILE_RUNE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddGreatLightningRune,     &MI_Rune,            MIS_RUNENOVA,        true,      1, MISR_NONE,      MFILE_RUNE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddImmolationRune,         &MI_Rune,            MIS_RUNEIMMOLAT,     true,      1, MISR_NONE,      MFILE_RUNE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddStoneRune,              &MI_Rune,            MIS_RUNESTONE,       true,      1, MISR_NONE,      MFILE_RUNE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddRuneExplosion,          &MI_HiveExplode,     MIS_HIVEEXP,         true,      1, MISR_FIRE,      MFILE_BIGEXP,    LS_NESTXPLD, LS_NESTXPLD, MissileMovementDistrubution::Disabled    },
	{  &AddHorkSpawn,              &MI_HorkSpawn,       MIS_HORKDMN,         true,      2, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddJester,                 nullptr,             MIS_JESTER,          false,     2, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddHiveExplosion,          nullptr,             MIS_HIVEEXP2,        false,     2, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddBloodStar,              &MI_Projectile,      MIS_LICH,            true,      1, MISR_MAGIC,     MFILE_LICH,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Blockable   },
	{  &AddBloodStar,              &MI_Projectile,      MIS_PSYCHORB,        true,      1, MISR_MAGIC,     MFILE_BONEDEMON, SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Blockable   },
	{  &AddBloodStar,              &MI_Projectile,      MIS_NECROMORB,       true,      1, MISR_MAGIC,     MFILE_NECROMORB, SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Blockable   },
	{  &AddBloodStar,              &MI_Projectile,      MIS_ARCHLICH,        true,      1, MISR_MAGIC,     MFILE_ARCHLICH,  SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Blockable   },
	{  &AddBloodStar,              &MI_Projectile,      MIS_BONEDEMON,       true,      1, MISR_MAGIC,     MFILE_BONEDEMON, SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Blockable   },
	{  &AddMissileHit,             &MI_MissileHit,      MIS_EXYEL2,          true,      2, MISR_NONE,      MFILE_EXYEL2,    LS_FIRIMP2,  SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddMissileHit,             &MI_MissileHit,      MIS_EXRED3,          true,      2, MISR_NONE,      MFILE_EXRED3,    LS_FIRIMP2,  SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddMissileHit,             &MI_MissileHit,      MIS_EXBL2,           true,      2, MISR_NONE,      MFILE_EXBL2,     LS_FIRIMP2,  SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddMissileHit,             &MI_MissileHit,      MIS_EXBL3,           true,      2, MISR_NONE,      MFILE_EXBL3,     LS_FIRIMP2,  SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddMissileHit,             &MI_MissileHit,      MIS_EXORA1,          true,      2, MISR_NONE,      MFILE_EXORA1,    LS_FIRIMP2,  SFX_NONE,    MissileMovementDistrubution::Disabled    },
	// clang-format on
};

/** Data related to each missile graphic ID. */
MissileFileData MissileSpriteData[] = {
	// clang-format off
	// name,      animName,        animFAmt, flags,                          animDelay[16], animLen[16],                     animWidth, animWidth2
	{ "Arrows",   MFILE_ARROWS,     1,       MissileDataFlags::NotAnimated,  { 0 },         { 16 },                                 96,         16 },
	{ "Fireba",   MFILE_FIREBA,    16,       MissileDataFlags::None,         { 0 },         { 14 },                                 96,         16 },
	{ "Guard",    MFILE_GUARD,      3,       MissileDataFlags::None,         { 1 },         { 15, 14,  3 },                         96,         16 },
	{ "Lghning",  MFILE_LGHNING,    1,       MissileDataFlags::None,         { 0 },         {  8 },                                 96,         16 },
	{ "Firewal",  MFILE_FIREWAL,    2,       MissileDataFlags::None,         { 0 },         { 13, 11 },                            128,         32 },
	{ "MagBlos",  MFILE_MAGBLOS,    1,       MissileDataFlags::None,         { 1 },         { 10 },                                128,         32 },
	{ "Portal",   MFILE_PORTAL,     2,       MissileDataFlags::None,         { 0, 1 },      { 16 },                                 96,         16 },
	{ "Bluexfr",  MFILE_BLUEXFR,    1,       MissileDataFlags::None,         { 0 },         { 19 },                                160,         48 },
	{ "Bluexbk",  MFILE_BLUEXBK,    1,       MissileDataFlags::None,         { 0 },         { 19 },                                160,         48 },
	{ "Manashld", MFILE_MANASHLD,   1,       MissileDataFlags::NotAnimated,  { 0 },         {  1 },                                 96,         16 },
	{ nullptr,    MFILE_BLOOD,      4,       MissileDataFlags::None,         { 0 },         { 15 },                                 96,         16 },
	{ nullptr,    MFILE_BONE,       3,       MissileDataFlags::None,         { 2 },         {  8 },                                128,         32 },
	{ nullptr,    MFILE_METLHIT,    3,       MissileDataFlags::None,         { 2 },         { 10 },                                 96,         16 },
	{ "Farrow",   MFILE_FARROW,    16,       MissileDataFlags::None,         { 0 },         {  4 },                                 96,         16 },
	{ "Doom",     MFILE_DOOM,       9,       MissileDataFlags::MonsterOwned, { 1 },         { 15 },                                 96,         16 },
	{ nullptr,    MFILE_0F,         1,       MissileDataFlags::MonsterOwned, { 0 },         {  0 },                                  0,          0 },
	{ nullptr,    MFILE_BLODBUR,    2,       MissileDataFlags::None,         { 2 },         {  8 },                                128,         32 },
	{ "Newexp",   MFILE_NEWEXP,     1,       MissileDataFlags::None,         { 1 },         { 15 },                                 96,         16 },
	{ "Shatter1", MFILE_SHATTER1,   1,       MissileDataFlags::None,         { 1 },         { 12 },                                128,         32 },
	{ "Bigexp",   MFILE_BIGEXP,     1,       MissileDataFlags::None,         { 0 },         { 15 },                                160,         48 },
	{ "Inferno",  MFILE_INFERNO,    1,       MissileDataFlags::None,         { 0 },         { 20 },                                 96,         16 },
	{ "Thinlght", MFILE_THINLGHT,   1,       MissileDataFlags::MonsterOwned, { 0 },         {  8 },                                 96,         16 },
	{ "Flare",    MFILE_FLARE,      1,       MissileDataFlags::None,         { 0 },         { 16 },                                128,         32 },
	{ "Flareexp", MFILE_FLAREEXP,   1,       MissileDataFlags::None,         { 0 },         {  7 },                                128,         32 },
	{ "Magball",  MFILE_MAGBALL,    8,       MissileDataFlags::MonsterOwned, { 1 },         { 16 },                                128,         32 },
	{ "Krull",    MFILE_KRULL,      1,       MissileDataFlags::MonsterOwned, { 0 },         { 14 },                                 96,         16 },
	{ "Miniltng", MFILE_MINILTNG,   1,       MissileDataFlags::None,         { 1 },         {  8 },                                 64,          0 },
	{ "Holy",     MFILE_HOLY,      16,       MissileDataFlags::None,         { 1, 0 },      { 14 },                                 96,         16 },
	{ "Holyexpl", MFILE_HOLYEXPL,   1,       MissileDataFlags::None,         { 0 },         {  8 },                                160,         48 },
	{ "Larrow",   MFILE_LARROW,    16,       MissileDataFlags::None,         { 0 },         {  4 },                                 96,         16 },
	{ nullptr,    MFILE_FIRARWEX,   1,       MissileDataFlags::None,         { 0 },         {  6 },                                 64,          0 },
	{ "Acidbf",   MFILE_ACIDBF,    16,       MissileDataFlags::MonsterOwned, { 0 },         {  8 },                                 96,         16 },
	{ "Acidspla", MFILE_ACIDSPLA,   1,       MissileDataFlags::MonsterOwned, { 0 },         {  8 },                                 96,         16 },
	{ "Acidpud",  MFILE_ACIDPUD,    2,       MissileDataFlags::MonsterOwned, { 0 },         {  9,  4 },                             96,         16 },
	{ nullptr,    MFILE_ETHRSHLD,   1,       MissileDataFlags::None,         { 0 },         {  1 },                                 96,         16 },
	{ "Firerun",  MFILE_FIRERUN,    8,       MissileDataFlags::None,         { 1 },         { 12 },                                 96,         16 },
	{ "Ressur1",  MFILE_RESSUR1,    1,       MissileDataFlags::None,         { 0 },         { 16 },                                 96,         16 },
	{ "Sklball",  MFILE_SKLBALL,    9,       MissileDataFlags::None,         { 1 },         { 16, 16, 16, 16, 16, 16, 16, 16, 8 },  96,         16 },
	{ "Rportal",  MFILE_RPORTAL,    2,       MissileDataFlags::None,         { 0 },         { 16 },                                 96,         16 },
	{ "Fireplar", MFILE_FIREPLAR,   1,       MissileDataFlags::MonsterOwned, { 1 },         { 17 },                                160,         48 },
	{ "Scubmisb", MFILE_SCUBMISB,   1,       MissileDataFlags::MonsterOwned, { 0 },         { 16 },                                 96,         16 },
	{ "Scbsexpb", MFILE_SCBSEXPB,   1,       MissileDataFlags::MonsterOwned, { 0 },         {  6 },                                128,         32 },
	{ "Scubmisc", MFILE_SCUBMISC,   1,       MissileDataFlags::MonsterOwned, { 0 },         { 16 },                                 96,         16 },
	{ "Scbsexpc", MFILE_SCBSEXPC,   1,       MissileDataFlags::MonsterOwned, { 0 },         {  6 },                                128,         32 },
	{ "Scubmisd", MFILE_SCUBMISD,   1,       MissileDataFlags::MonsterOwned, { 0 },         { 16 },                                 96,         16 },
	{ "Scbsexpd", MFILE_SCBSEXPD,   1,       MissileDataFlags::MonsterOwned, { 0 },         {  6 },                                128,         32 },
	{ "spawns",   MFILE_SPAWNS,     8,       MissileDataFlags::MonsterOwned, { 0 },         {  9 },                                 96,         16 },
	{ "reflect",  MFILE_REFLECT,    1,       MissileDataFlags::NotAnimated,  { 0 },         {  1 },                                160,         64 },
	{ "ms_ora",   MFILE_LICH,      16,       MissileDataFlags::MonsterOwned, { 0 },         { 15 },                                 96,          8 },
	{ "ms_bla",   MFILE_MSBLA,     16,       MissileDataFlags::MonsterOwned, { 0 },         { 15 },                                 96,          8 },
	{ "ms_reb",   MFILE_NECROMORB, 16,       MissileDataFlags::MonsterOwned, { 0 },         { 15 },                                 96,          8 },
	{ "ms_yeb",   MFILE_ARCHLICH,  16,       MissileDataFlags::MonsterOwned, { 0 },         { 15 },                                 96,          8 },
	{ "rglows1",  MFILE_RUNE,       1,       MissileDataFlags::None,         { 0 },         { 10 },                                 96,          8 },
	{ "ex_yel2",  MFILE_EXYEL2,     1,       MissileDataFlags::MonsterOwned, { 0 },         { 10 },                                220,         78 },
	{ "ex_blu2",  MFILE_EXBL2,      1,       MissileDataFlags::MonsterOwned, { 0 },         { 10 },                                212,         86 },
	{ "ex_red3",  MFILE_EXRED3,     1,       MissileDataFlags::MonsterOwned, { 0 },         {  7 },                                292,        114 },
	{ "ms_blb",   MFILE_BONEDEMON, 16,       MissileDataFlags::MonsterOwned, { 0 },         { 15 },                                 96,          8 },
	{ "ex_ora1",  MFILE_EXORA1,     1,       MissileDataFlags::MonsterOwned, { 0 },         { 13 },                                 96,        -12 },
	{ "ex_blu3",  MFILE_EXBL3,      1,       MissileDataFlags::MonsterOwned, { 0 },         {  7 },                                292,        114 },
	{ "",         MFILE_NONE,       0,       MissileDataFlags::None,         {  },          { },                                     0,          0 },
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

MissileFileData::MissileFileData(const char *name, uint8_t animName, uint8_t animFAmt, MissileDataFlags flags,
    std::initializer_list<uint8_t> animDelay, std::initializer_list<uint8_t> animLen,
    int16_t animWidth, int16_t animWidth2)
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
	if (animData[0] != nullptr)
		return;

	if (name == nullptr)
		return;

	char pszName[256];
	if (animFAmt == 1) {
		sprintf(pszName, "Missiles\\%s.CL2", name);
		animData[0] = LoadFileInMem(pszName);
	} else {
		for (unsigned i = 0; i < animFAmt; i++) {
			sprintf(pszName, "Missiles\\%s%u.CL2", name, i + 1);
			animData[i] = LoadFileInMem(pszName);
		}
	}
}

void InitMissileGFX(bool loadHellfireGraphics)
{
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
