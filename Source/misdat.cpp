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
	// mAddProc,                   mProc,              name,             mDraw,  mType, mResist,        mFileNum,        mlSFX,       miSFX,       MovementDistribution;
	{  &AddArrow,                  &MI_Arrow,          MIS_ARROW,         true,      0, MISR_NONE,      MFILE_ARROWS,    SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
	{  &AddFirebolt,               &MI_Firebolt,       MIS_FIREBOLT,      true,      1, MISR_FIRE,      MFILE_FIREBA,    LS_FBOLT1,   LS_FIRIMP2,  MissileMovementDistribution::Blockable   },
	{  &AddGuardian,               &MI_Guardian,       MIS_GUARDIAN,      true,      1, MISR_NONE,      MFILE_GUARD,     LS_GUARD,    LS_GUARDLAN, MissileMovementDistribution::Disabled    },
	{  &AddRndTeleport,            &MI_Teleport,       MIS_PHASING,   false,     1, MISR_NONE,      MFILE_NONE,      LS_TELEPORT, SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddLightball,              &MI_Lightball,      MIS_NOVA_SEGEMENT,     true,      1, MISR_LIGHTNING, MFILE_LGHNING,   SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Unblockable },
	{  &AddFirewall,               &MI_Firewall,       MIS_FIREWALL_SEGMENT,      true,      1, MISR_FIRE,      MFILE_FIREWAL,   LS_WALLLOOP, LS_FIRIMP2,  MissileMovementDistribution::Disabled    },
	{  &AddFireball,               &MI_Fireball,       MIS_FIREBALL,      true,      1, MISR_FIRE,      MFILE_FIREBA,    LS_FBOLT1,   LS_FIRIMP2,  MissileMovementDistribution::Blockable   },
	{  &AddLightctrl,              &MI_Lightctrl,      MIS_LIGHTNING_CTRL,     false,     1, MISR_LIGHTNING, MFILE_LGHNING,   SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddLightning,              &MI_Lightning,      MIS_LIGHTNING_SEGMENT,     true,      1, MISR_LIGHTNING, MFILE_LGHNING,   LS_LNING1,   LS_ELECIMP1, MissileMovementDistribution::Disabled    },
	{  &AddMisexp,                 &MI_Misexp,         MIS_MAGMABALL_EXP,        true,      2, MISR_NONE,      MFILE_MAGBLOS,   SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddTown,                   &MI_Town,           MIS_TOWNPORTAL,          true,      1, MISR_MAGIC,     MFILE_PORTAL,    LS_SENTINEL, LS_ELEMENTL, MissileMovementDistribution::Disabled    },
	{  &AddFlash,                  &MI_Flash,          MIS_FLASH_SEGMENT,         true,      1, MISR_MAGIC,     MFILE_BLUEXFR,   LS_NOVA,     LS_ELECIMP1, MissileMovementDistribution::Disabled    },
	{  &AddFlash2,                 &MI_Flash2,         MIS_FLASH_SEGMENT_2,        true,      1, MISR_MAGIC,     MFILE_BLUEXBK,   SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddManashield,             nullptr,            MIS_MANASHIELD,    false,     1, MISR_MAGIC,     MFILE_MANASHLD,  LS_MSHIELD,  SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddFiremove,               &MI_Firemove,       MIS_FLAMEWAVE_SEGMENT,      true,      1, MISR_FIRE,      MFILE_FIREWAL,   SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Unblockable },
	{  &AddChain,                  &MI_Chain,          MIS_CHAINLIGHTNING_SEGMENT,         true,      1, MISR_LIGHTNING, MFILE_LGHNING,   LS_LNING1,   LS_ELECIMP1, MissileMovementDistribution::Disabled    },
	{  nullptr,                    nullptr,            MIS_NULL_16,      true,      1, MISR_LIGHTNING, MFILE_LGHNING,   SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  nullptr,                    nullptr,            MIS_NULL_17,      true,      2, MISR_NONE,      MFILE_BLOOD,     LS_BLODSTAR, LS_BLSIMPT,  MissileMovementDistribution::Disabled    },
	{  nullptr,                    nullptr,            MIS_NULL_18,          true,      2, MISR_NONE,      MFILE_BONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  nullptr,                    nullptr,            MIS_NULL_19,       true,      2, MISR_NONE,      MFILE_METLHIT,   SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddRhino,                  &MI_Rhino,          MIS_RHINO,         true,      2, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
	{  &AddMagmaball,              &MI_Firebolt,       MIS_MAGMABALL,     true,      1, MISR_FIRE,      MFILE_MAGBALL,   SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
	{  &AddLightctrl,              &MI_Lightctrl,      MIS_LIGHTNING_CTRL_MONST,    false,     1, MISR_LIGHTNING, MFILE_THINLGHT,  SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddLightning,              &MI_Lightning,      MIS_LIGHTNING_SEGEMENT_MONST,    true,      1, MISR_LIGHTNING, MFILE_THINLGHT,  SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddFlare,                  &MI_Firebolt,       MIS_BLOODSTAR,         true,      1, MISR_MAGIC,     MFILE_FLARE,     SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
	{  &AddMisexp,                 &MI_Misexp,         MIS_BLOODSTAR_EXP,       true,      2, MISR_MAGIC,     MFILE_FLAREEXP,  SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddTeleport,               &MI_Teleport,       MIS_TELEPORT,      false,     1, MISR_NONE,      MFILE_NONE,      LS_ELEMENTL, SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddLArrow,                 &MI_LArrow,         MIS_FIREARROW,        true,      0, MISR_FIRE,      MFILE_FARROW,    SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
	{  nullptr,                    nullptr,            MIS_NULL_28,      false,     1, MISR_MAGIC,     MFILE_DOOM,      LS_DSERP,    SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  nullptr,                    nullptr,            MIS_NULL_29,     true,      2, MISR_FIRE,      MFILE_FIREWAL,   SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddStone,                  &MI_Stone,          MIS_STONECURSE,         false,     1, MISR_MAGIC,     MFILE_NONE,      LS_SCURIMP,  SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  nullptr,                    nullptr,            MIS_NULL_31,       true,      1, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  nullptr,                    nullptr,            MIS_NULL_32,      false,     1, MISR_NONE,      MFILE_NONE,      LS_INVISIBL, SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddGolem,                  nullptr,            MIS_GOLEM,         false,     1, MISR_NONE,      MFILE_NONE,      LS_GOLUM,    SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  nullptr,                    nullptr,            MIS_NULL_34,   true,      1, MISR_NONE,      MFILE_ETHRSHLD,  LS_ETHEREAL, SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  nullptr,                    nullptr,            MIS_NULL_35,       true,      2, MISR_NONE,      MFILE_BLODBUR,   SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddBoom,                   &MI_Boom,           MIS_APOCALYPSE_EXP,          true,      2, MISR_NONE,      MFILE_NEWEXP,    SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddHeal,                   nullptr,            MIS_HEALING,          false,     1, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddFirewallC,              &MI_FirewallC,      MIS_FIREWALL_CAST,     false,     1, MISR_FIRE,      MFILE_FIREWAL,   SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddInfra,                  &MI_Infra,          MIS_INFRAVISION,         false,     1, MISR_NONE,      MFILE_NONE,      LS_INFRAVIS, SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddIdentify,               nullptr,            MIS_IDENTIFY,      false,     1, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddWave,                   &MI_Wave,           MIS_FLAMEWAVE,          true,      1, MISR_FIRE,      MFILE_FIREWAL,   LS_FLAMWAVE, SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddNova,                   &MI_Nova,           MIS_NOVA,          true,      1, MISR_LIGHTNING, MFILE_LGHNING,   LS_NOVA,     SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddBlodboil,               &MI_Blodboil,       MIS_RAGE,      false,     1, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddApoca,                  &MI_Apoca,          MIS_APOCALYPSE,         true,      1, MISR_MAGIC,     MFILE_NEWEXP,    LS_APOC,     SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddRepair,                 nullptr,            MIS_ITEMREPAIR,        false,     2, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddRecharge,               nullptr,            MIS_STAFFRECHARGE,      false,     2, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddDisarm,                 nullptr,            MIS_TRAPDISARM,        false,     2, MISR_NONE,      MFILE_NONE,      LS_TRAPDIS,  SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddFlame,                  &MI_Flame,          MIS_INFERNO,         true,      1, MISR_FIRE,      MFILE_INFERNO,   LS_SPOUTSTR, SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddFlamec,                 &MI_Flamec,         MIS_INFERNO_CAST,        false,     1, MISR_FIRE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  nullptr,                    nullptr,            MIS_NULL_50,       true,      2, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
	{  nullptr,                    nullptr,            MIS_NULL_51,         true,      0, MISR_FIRE,      MFILE_KRULL,     SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
	{  &AddCbolt,                  &MI_Cbolt,          MIS_CHARGEDBOLT,         true,      1, MISR_LIGHTNING, MFILE_MINILTNG,  LS_CBOLT,    SFX_NONE,    MissileMovementDistribution::Blockable   },
	{  &AddHbolt,                  &MI_Hbolt,          MIS_HOLYBOLT,         true,      1, MISR_NONE,      MFILE_HOLY,      LS_HOLYBOLT, LS_ELECIMP1, MissileMovementDistribution::Blockable   },
	{  &AddResurrect,              nullptr,            MIS_RESURRECT,     false,     1, MISR_MAGIC,     MFILE_NONE,      SFX_NONE,    LS_RESUR,    MissileMovementDistribution::Disabled    },
	{  &AddTelekinesis,            nullptr,            MIS_TELEKINESIS,   false,     1, MISR_NONE,      MFILE_NONE,      LS_ETHEREAL, SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddLArrow,                 &MI_LArrow,         MIS_LIGHTNINGARROW,        true,      0, MISR_LIGHTNING, MFILE_LARROW,    SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
	{  &AddAcid,                   &MI_Firebolt,       MIS_ACID,          true,      1, MISR_ACID,      MFILE_ACIDBF,    LS_ACID,     SFX_NONE,    MissileMovementDistribution::Blockable   },
	{  &AddMisexp,                 &MI_Acidsplat,      MIS_ACID_EXP,       true,      2, MISR_ACID,      MFILE_ACIDSPLA,  SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddAcidpud,                &MI_Acidpud,        MIS_ACID_PUDDLE,       true,      2, MISR_ACID,      MFILE_ACIDPUD,   LS_PUDDLE,   SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddHealOther,              nullptr,            MIS_HEALOTHER,     false,     1, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddElement,                &MI_Element,        MIS_ELEMENTAL,       true,      1, MISR_FIRE,      MFILE_FIRERUN,   LS_ELEMENTL, SFX_NONE,    MissileMovementDistribution::Unblockable },
	{  &AddResurrectBeam,          &MI_ResurrectBeam,  MIS_RESURRECT_BEAM, true,      1, MISR_NONE,      MFILE_RESSUR1,   SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddBoneSpirit,             &MI_Bonespirit,     MIS_BONESPIRIT,    true,      1, MISR_MAGIC,     MFILE_SKLBALL,   LS_BONESP,   LS_BSIMPCT,  MissileMovementDistribution::Blockable   },
	{  &AddWeapexp,                &MI_Weapexp,        MIS_WEAPON_EXP,       true,      2, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddRportal,                &MI_Rportal,        MIS_REDPORTAL,       true,      2, MISR_NONE,      MFILE_RPORTAL,   LS_SENTINEL, LS_ELEMENTL, MissileMovementDistribution::Disabled    },
	{  &AddBoom,                   &MI_Boom,           MIS_APOCALYPSE_EXP_MONST,         true,      2, MISR_NONE,      MFILE_FIREPLAR,  SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddDiabApoca,              nullptr,            MIS_APOCALYPSE_MONST,     false,     2, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddMana,                   nullptr,            MIS_MANA,          false,     1, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddMagi,                   nullptr,            MIS_MAGI,          false,     1, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddLightningWall,          &MI_LightningWall,  MIS_LIGHTNINGWALL_SEGMENT,     true,      1, MISR_LIGHTNING, MFILE_LGHNING,   LS_LMAG,     LS_ELECIMP1, MissileMovementDistribution::Disabled    },
	{  &AddFirewallC,              &MI_LightningWallC, MIS_LIGHTNINGWALL, false,     1, MISR_LIGHTNING, MFILE_LGHNING,   SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddNova,                   &MI_FireNova,       MIS_IMMOLATION_CAST,    true,      1, MISR_FIRE,      MFILE_FIREBA,    LS_FBOLT1,   LS_FIRIMP2,  MissileMovementDistribution::Disabled    },
	{  &AddSpecArrow,              &MI_SpecArrow,      MIS_SPECTRALARROW,     true,      0, MISR_NONE,      MFILE_ARROWS,    SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddFireNova,               &MI_Fireball,       MIS_IMMOLATION,      true,      1, MISR_FIRE,      MFILE_FIREBA,    IS_FBALLBOW, LS_FIRIMP2,  MissileMovementDistribution::Blockable   },
	{  &AddLightningArrow,         &MI_LightningArrow, MIS_LIGHTNING_BOW,    false,     1, MISR_LIGHTNING, MFILE_LGHNING,   IS_FBALLBOW, SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddCboltArrow,             &MI_Cbolt,          MIS_CHARGEDBOLT_BOW,    true,      1, MISR_LIGHTNING, MFILE_MINILTNG,  LS_CBOLT,    SFX_NONE,    MissileMovementDistribution::Blockable   },
	{  &AddHbolt,                  &MI_Hbolt,          MIS_HOLYBOLT_BOW,    true,      1, MISR_NONE,      MFILE_HOLY,      LS_HOLYBOLT, LS_ELECIMP1, MissileMovementDistribution::Blockable   },
	{  &AddWarp,                   &MI_Teleport,       MIS_WARP,          false,     1, MISR_NONE,      MFILE_NONE,      LS_ETHEREAL, SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddReflection,             nullptr,            MIS_REFLECT,       false,     1, MISR_NONE,      MFILE_REFLECT,   LS_MSHIELD,  SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddBerserk,                nullptr,            MIS_BERSERK,       false,     1, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddRing,                   &MI_FireRing,       MIS_RINGOFFIRE,      false,     1, MISR_FIRE,      MFILE_FIREWAL,   SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddStealPotions,           nullptr,            MIS_TRAP_POTIONS,     false,     1, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddManaTrap,               nullptr,            MIS_TRAP_MANA,      false,     1, MISR_NONE,      MFILE_NONE,      IS_CAST7,    SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  nullptr,                    nullptr,            MIS_NULL_84,     false,     1, MISR_LIGHTNING, MFILE_LGHNING,   SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddSearch,                 &MI_Search,         MIS_SEARCH,        false,     1, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  nullptr,                    nullptr,            MIS_NULL_86,       false,     1, MISR_MAGIC,     MFILE_BLUEXFR,   SFX_NONE,    LS_ELECIMP1, MissileMovementDistribution::Disabled    },
	{  nullptr,                    nullptr,            MIS_NULL_87,       false,     1, MISR_MAGIC,     MFILE_BLUEXBK,   SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  nullptr,                    nullptr,            MIS_NULL_88,   true,      1, MISR_FIRE,      MFILE_FIREBA,    LS_FBOLT1,   LS_FIRIMP2,  MissileMovementDistribution::Disabled    },
	{  &AddFireRune,               &MI_Rune,           MIS_RUNEOFFIRE,      true,      1, MISR_NONE,      MFILE_RUNE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddLightningRune,          &MI_Rune,           MIS_RUNEOFLIGHTNING,     true,      1, MISR_NONE,      MFILE_RUNE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddGreatLightningRune,     &MI_Rune,           MIS_RUNEOFNOVA,      true,      1, MISR_NONE,      MFILE_RUNE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddImmolationRune,         &MI_Rune,           MIS_RUNEOFIMMOLATION,   true,      1, MISR_NONE,      MFILE_RUNE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddStoneRune,              &MI_Rune,           MIS_RUNEOFSTONE,     true,      1, MISR_NONE,      MFILE_RUNE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddRuneExplosion,          &MI_HiveExplode,    MIS_RUNEBOMB_EXP,       true,      1, MISR_FIRE,      MFILE_BIGEXP,    LS_NESTXPLD, LS_NESTXPLD, MissileMovementDistribution::Disabled    },
	{  &AddHorkSpawn,              &MI_HorkSpawn,      MIS_HORKDEMON,       false,     2, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddJester,                 nullptr,            MIS_JESTER,        false,     2, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddHiveExplosion,          nullptr,            MIS_RUNEBOMB_EXP_2,      false,     2, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddFlare,                  &MI_Firebolt,       MIS_LICH,          true,      1, MISR_MAGIC,     MFILE_LICH,      SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
	{  &AddFlare,                  &MI_Firebolt,       MIS_PSYCHORB,      true,      1, MISR_MAGIC,     MFILE_BONEDEMON, SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
	{  &AddFlare,                  &MI_Firebolt,       MIS_NECROMORB,     true,      1, MISR_MAGIC,     MFILE_NECROMORB, SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
	{  &AddFlare,                  &MI_Firebolt,       MIS_ARCHLICH,      true,      1, MISR_MAGIC,     MFILE_ARCHLICH,  SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
	{  &AddFlare,                  &MI_Firebolt,       MIS_BONEDEMON,     true,      1, MISR_MAGIC,     MFILE_BONEDEMON, SFX_NONE,    SFX_NONE,    MissileMovementDistribution::Blockable   },
	{  &AddMisexp,                 &MI_Misexp,         MIS_ARCHLICH_EXP,        true,      2, MISR_NONE,      MFILE_EXYEL2,    LS_FIRIMP2,  SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddMisexp,                 &MI_Misexp,         MIS_NECROMORB_EXP,        true,      2, MISR_NONE,      MFILE_EXRED3,    LS_FIRIMP2,  SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddMisexp,                 &MI_Misexp,         MIS_PSYCHORB_EXP,         true,      2, MISR_NONE,      MFILE_EXBL2,     LS_FIRIMP2,  SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddMisexp,                 &MI_Misexp,         MIS_BONEDEMON_EXP,         true,      2, MISR_NONE,      MFILE_EXBL3,     LS_FIRIMP2,  SFX_NONE,    MissileMovementDistribution::Disabled    },
	{  &AddMisexp,                 &MI_Misexp,         MIS_LICH_EXP,        true,      2, MISR_NONE,      MFILE_EXORA1,    LS_FIRIMP2,  SFX_NONE,    MissileMovementDistribution::Disabled    },
	// clang-format on
};

/** Data related to each missile graphic ID. */
MissileFileData MissileSpriteData[] = {
	// clang-format off
	// name,      animName,        animFAmt, flags,                          animDelay[16], animLen[16],                     animWidth, animWidth2
	{ "arrows",   MFILE_ARROWS,     1,       MissileDataFlags::NotAnimated,  { 0 },         { 16 },                                 96,         16 },
	{ "fireba",   MFILE_FIREBA,    16,       MissileDataFlags::None,         { 0 },         { 14 },                                 96,         16 },
	{ "guard",    MFILE_GUARD,      3,       MissileDataFlags::None,         { 1 },         { 15, 14,  3 },                         96,         16 },
	{ "lghning",  MFILE_LGHNING,    1,       MissileDataFlags::None,         { 0 },         {  8 },                                 96,         16 },
	{ "firewal",  MFILE_FIREWAL,    2,       MissileDataFlags::None,         { 0 },         { 13, 11 },                            128,         32 },
	{ "magblos",  MFILE_MAGBLOS,    1,       MissileDataFlags::None,         { 1 },         { 10 },                                128,         32 },
	{ "portal",   MFILE_PORTAL,     2,       MissileDataFlags::None,         { 0, 1 },      { 16 },                                 96,         16 },
	{ "bluexfr",  MFILE_BLUEXFR,    1,       MissileDataFlags::None,         { 0 },         { 19 },                                160,         48 },
	{ "bluexbk",  MFILE_BLUEXBK,    1,       MissileDataFlags::None,         { 0 },         { 19 },                                160,         48 },
	{ "manashld", MFILE_MANASHLD,   1,       MissileDataFlags::NotAnimated,  { 0 },         {  1 },                                 96,         16 },
	{ {},         MFILE_BLOOD,      4,       MissileDataFlags::None,         { 0 },         { 15 },                                 96,         16 },
	{ {},         MFILE_BONE,       3,       MissileDataFlags::None,         { 2 },         {  8 },                                128,         32 },
	{ {},         MFILE_METLHIT,    3,       MissileDataFlags::None,         { 2 },         { 10 },                                 96,         16 },
	{ "farrow",   MFILE_FARROW,    16,       MissileDataFlags::None,         { 0 },         {  4 },                                 96,         16 },
	{ "doom",     MFILE_DOOM,       9,       MissileDataFlags::MonsterOwned, { 1 },         { 15 },                                 96,         16 },
	{ {},         MFILE_0F,         1,       MissileDataFlags::MonsterOwned, { 0 },         {  0 },                                  0,          0 },
	{ {},         MFILE_BLODBUR,    2,       MissileDataFlags::None,         { 2 },         {  8 },                                128,         32 },
	{ "newexp",   MFILE_NEWEXP,     1,       MissileDataFlags::None,         { 1 },         { 15 },                                 96,         16 },
	{ "shatter1", MFILE_SHATTER1,   1,       MissileDataFlags::None,         { 1 },         { 12 },                                128,         32 },
	{ "bigexp",   MFILE_BIGEXP,     1,       MissileDataFlags::None,         { 0 },         { 15 },                                160,         48 },
	{ "inferno",  MFILE_INFERNO,    1,       MissileDataFlags::None,         { 0 },         { 20 },                                 96,         16 },
	{ "thinlght", MFILE_THINLGHT,   1,       MissileDataFlags::MonsterOwned, { 0 },         {  8 },                                 96,         16 },
	{ "flare",    MFILE_FLARE,      1,       MissileDataFlags::None,         { 0 },         { 16 },                                128,         32 },
	{ "flareexp", MFILE_FLAREEXP,   1,       MissileDataFlags::None,         { 0 },         {  7 },                                128,         32 },
	{ "magball",  MFILE_MAGBALL,    8,       MissileDataFlags::MonsterOwned, { 1 },         { 16 },                                128,         32 },
	{ "krull",    MFILE_KRULL,      1,       MissileDataFlags::MonsterOwned, { 0 },         { 14 },                                 96,         16 },
	{ "miniltng", MFILE_MINILTNG,   1,       MissileDataFlags::None,         { 1 },         {  8 },                                 64,          0 },
	{ "holy",     MFILE_HOLY,      16,       MissileDataFlags::None,         { 1, 0 },      { 14 },                                 96,         16 },
	{ "holyexpl", MFILE_HOLYEXPL,   1,       MissileDataFlags::None,         { 0 },         {  8 },                                160,         48 },
	{ "larrow",   MFILE_LARROW,    16,       MissileDataFlags::None,         { 0 },         {  4 },                                 96,         16 },
	{ {},         MFILE_FIRARWEX,   1,       MissileDataFlags::None,         { 0 },         {  6 },                                 64,          0 },
	{ "acidbf",   MFILE_ACIDBF,    16,       MissileDataFlags::MonsterOwned, { 0 },         {  8 },                                 96,         16 },
	{ "acidspla", MFILE_ACIDSPLA,   1,       MissileDataFlags::MonsterOwned, { 0 },         {  8 },                                 96,         16 },
	{ "acidpud",  MFILE_ACIDPUD,    2,       MissileDataFlags::MonsterOwned, { 0 },         {  9,  4 },                             96,         16 },
	{ {},         MFILE_ETHRSHLD,   1,       MissileDataFlags::None,         { 0 },         {  1 },                                 96,         16 },
	{ "firerun",  MFILE_FIRERUN,    8,       MissileDataFlags::None,         { 1 },         { 12 },                                 96,         16 },
	{ "ressur1",  MFILE_RESSUR1,    1,       MissileDataFlags::None,         { 0 },         { 16 },                                 96,         16 },
	{ "sklball",  MFILE_SKLBALL,    9,       MissileDataFlags::None,         { 1 },         { 16, 16, 16, 16, 16, 16, 16, 16, 8 },  96,         16 },
	{ "rportal",  MFILE_RPORTAL,    2,       MissileDataFlags::None,         { 0 },         { 16 },                                 96,         16 },
	{ "fireplar", MFILE_FIREPLAR,   1,       MissileDataFlags::MonsterOwned, { 1 },         { 17 },                                160,         48 },
	{ "scubmisb", MFILE_SCUBMISB,   1,       MissileDataFlags::MonsterOwned, { 0 },         { 16 },                                 96,         16 },
	{ "scbsexpb", MFILE_SCBSEXPB,   1,       MissileDataFlags::MonsterOwned, { 0 },         {  6 },                                128,         32 },
	{ "scubmisc", MFILE_SCUBMISC,   1,       MissileDataFlags::MonsterOwned, { 0 },         { 16 },                                 96,         16 },
	{ "scbsexpc", MFILE_SCBSEXPC,   1,       MissileDataFlags::MonsterOwned, { 0 },         {  6 },                                128,         32 },
	{ "scubmisd", MFILE_SCUBMISD,   1,       MissileDataFlags::MonsterOwned, { 0 },         { 16 },                                 96,         16 },
	{ "scbsexpd", MFILE_SCBSEXPD,   1,       MissileDataFlags::MonsterOwned, { 0 },         {  6 },                                128,         32 },
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
	{ {},         MFILE_NONE,       0,       MissileDataFlags::None,         {  },          { },                                     0,          0 },
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
