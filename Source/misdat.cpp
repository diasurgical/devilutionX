/**
 * @file misdat.cpp
 *
 * Implementation of data related to missiles.
 */
#include "misdat.h"

#include "missiles.h"
#include "engine/load_file.hpp"
#include "engine/cel_header.hpp"

namespace devilution {

/** Data related to each missile ID. */
MissileDataStruct MissileData[] = {
	// clang-format off
	// mAddProc,                   mProc,              mName,             mDraw, mType, mResist,        mFileNum,        miSFX,       mlSFX,       MovementDistribution;
	{  &AddArrow,                  &MI_Arrow,          MIS_ARROW,         true,      0, MISR_NONE,      MFILE_ARROWS,    SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Blockable   },
	{  &AddFirebolt,               &MI_Firebolt,       MIS_FIREBOLT,      true,      1, MISR_FIRE,      MFILE_FIREBA,    LS_FBOLT1,   LS_FIRIMP2,  MissileMovementDistrubution::Blockable   },
	{  &AddGuardian,               &MI_Guardian,       MIS_GUARDIAN,      true,      1, MISR_NONE,      MFILE_GUARD,     LS_GUARD,    LS_GUARDLAN, MissileMovementDistrubution::Disabled    },
	{  &AddRndTeleport,            &MI_Teleport,       MIS_RNDTELEPORT,   false,     1, MISR_NONE,      MFILE_NONE,      LS_TELEPORT, SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddLightball,              &MI_Lightball,      MIS_LIGHTBALL,     true,      1, MISR_LIGHTNING, MFILE_LGHNING,   SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Unblockable },
	{  &AddFirewall,               &MI_Firewall,       MIS_FIREWALL,      true,      1, MISR_FIRE,      MFILE_FIREWAL,   LS_WALLLOOP, LS_FIRIMP2,  MissileMovementDistrubution::Disabled    },
	{  &AddFireball,               &MI_Fireball,       MIS_FIREBALL,      true,      1, MISR_FIRE,      MFILE_FIREBA,    LS_FBOLT1,   LS_FIRIMP2,  MissileMovementDistrubution::Blockable   },
	{  &AddLightctrl,              &MI_Lightctrl,      MIS_LIGHTCTRL,     false,     1, MISR_LIGHTNING, MFILE_LGHNING,   SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddLightning,              &MI_Lightning,      MIS_LIGHTNING,     true,      1, MISR_LIGHTNING, MFILE_LGHNING,   LS_LNING1,   LS_ELECIMP1, MissileMovementDistrubution::Disabled    },
	{  &AddMisexp,                 &MI_Misexp,         MIS_MISEXP,        true,      2, MISR_NONE,      MFILE_MAGBLOS,   SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddTown,                   &MI_Town,           MIS_TOWN,          true,      1, MISR_MAGIC,     MFILE_PORTAL,    LS_SENTINEL, LS_ELEMENTL, MissileMovementDistrubution::Disabled    },
	{  &AddFlash,                  &MI_Flash,          MIS_FLASH,         true,      1, MISR_MAGIC,     MFILE_BLUEXFR,   LS_NOVA,     LS_ELECIMP1, MissileMovementDistrubution::Disabled    },
	{  &AddFlash2,                 &MI_Flash2,         MIS_FLASH2,        true,      1, MISR_MAGIC,     MFILE_BLUEXBK,   SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddManashield,             &MI_Manashield,     MIS_MANASHIELD,    false,     1, MISR_MAGIC,     MFILE_MANASHLD,  LS_MSHIELD,  SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddFiremove,               &MI_Firemove,       MIS_FIREMOVE,      true,      1, MISR_FIRE,      MFILE_FIREWAL,   SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Unblockable },
	{  &AddChain,                  &MI_Chain,          MIS_CHAIN,         true,      1, MISR_LIGHTNING, MFILE_LGHNING,   LS_LNING1,   LS_ELECIMP1, MissileMovementDistrubution::Disabled    },
	{  nullptr,                    nullptr,            MIS_SENTINAL,      true,      1, MISR_LIGHTNING, MFILE_LGHNING,   SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddBloodStar,              &MI_Blood,          MIS_BLODSTAR,      true,      2, MISR_NONE,      MFILE_BLOOD,     LS_BLODSTAR, LS_BLSIMPT,  MissileMovementDistrubution::Disabled    },
	{  &AddBone,                   &MI_Blood,          MIS_BONE,          true,      2, MISR_NONE,      MFILE_BONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddMetlHit,                &MI_Blood,          MIS_METLHIT,       true,      2, MISR_NONE,      MFILE_METLHIT,   SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddRhino,                  &MI_Rhino,          MIS_RHINO,         true,      2, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Blockable   },
	{  &AddMagmaball,              &MI_Firebolt,       MIS_MAGMABALL,     true,      1, MISR_FIRE,      MFILE_MAGBALL,   SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Blockable   },
	{  &AddLightctrl,              &MI_Lightctrl,      MIS_LIGHTCTRL2,    false,     1, MISR_LIGHTNING, MFILE_THINLGHT,  SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddLightning,              &MI_Lightning,      MIS_LIGHTNING2,    true,      1, MISR_LIGHTNING, MFILE_THINLGHT,  SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddFlare,                  &MI_Firebolt,       MIS_FLARE,         true,      1, MISR_MAGIC,     MFILE_FLARE,     SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Blockable   },
	{  &AddMisexp,                 &MI_Misexp,         MIS_MISEXP2,       true,      2, MISR_MAGIC,     MFILE_FLAREEXP,  SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddTeleport,               &MI_Teleport,       MIS_TELEPORT,      false,     1, MISR_NONE,      MFILE_NONE,      LS_ELEMENTL, SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddLArrow,                 &MI_LArrow,         MIS_FARROW,        true,      0, MISR_FIRE,      MFILE_FARROW,    SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Blockable   },
	{  nullptr,                    nullptr,            MIS_DOOMSERP,      false,     1, MISR_MAGIC,     MFILE_DOOM,      LS_DSERP,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddFireWallA,              &MI_Firewall,       MIS_FIREWALLA,     true,      2, MISR_FIRE,      MFILE_FIREWAL,   SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddStone,                  &MI_Stone,          MIS_STONE,         false,     1, MISR_MAGIC,     MFILE_NONE,      LS_SCURIMP,  SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddDummy,                  &MI_Dummy,          MIS_NULL_1F,       true,      1, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  nullptr,                    nullptr,            MIS_INVISIBL,      false,     1, MISR_NONE,      MFILE_NONE,      LS_INVISIBL, SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddGolem,                  &MI_Golem,          MIS_GOLEM,         false,     1, MISR_NONE,      MFILE_NONE,      LS_GOLUM,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddEtherealize,            &MI_Etherealize,    MIS_ETHEREALIZE,   true,      1, MISR_NONE,      MFILE_ETHRSHLD,  LS_ETHEREAL, SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddBlodbur,                &MI_Blood,          MIS_BLODBUR,       true,      2, MISR_NONE,      MFILE_BLODBUR,   SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddBoom,                   &MI_Boom,           MIS_BOOM,          true,      2, MISR_NONE,      MFILE_NEWEXP,    SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddHeal,                   &MI_Dummy,          MIS_HEAL,          false,     1, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddFirewallC,              &MI_FirewallC,      MIS_FIREWALLC,     false,     1, MISR_FIRE,      MFILE_FIREWAL,   SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddInfra,                  &MI_Infra,          MIS_INFRA,         false,     1, MISR_NONE,      MFILE_NONE,      LS_INFRAVIS, SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddIdentify,               &MI_Dummy,          MIS_IDENTIFY,      false,     1, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddWave,                   &MI_Wave,           MIS_WAVE,          true,      1, MISR_FIRE,      MFILE_FIREWAL,   LS_FLAMWAVE, SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddNova,                   &MI_Nova,           MIS_NOVA,          true,      1, MISR_LIGHTNING, MFILE_LGHNING,   LS_NOVA,     SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddBlodboil,               &MI_Blodboil,       MIS_BLODBOIL,      false,     1, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddApoca,                  &MI_Apoca,          MIS_APOCA,         true,      1, MISR_MAGIC,     MFILE_NEWEXP,    LS_APOC,     SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddRepair,                 &MI_Dummy,          MIS_REPAIR,        false,     2, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddRecharge,               &MI_Dummy,          MIS_RECHARGE,      false,     2, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddDisarm,                 &MI_Dummy,          MIS_DISARM,        false,     2, MISR_NONE,      MFILE_NONE,      LS_TRAPDIS,  SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddFlame,                  &MI_Flame,          MIS_FLAME,         true,      1, MISR_FIRE,      MFILE_INFERNO,   LS_SPOUTSTR, SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddFlamec,                 &MI_Flamec,         MIS_FLAMEC,        false,     1, MISR_FIRE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddFireman,                &MI_Fireman,        MIS_FIREMAN,       true,      2, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Blockable   },
	{  &AddKrull,                  &MI_Krull,          MIS_KRULL,         true,      0, MISR_FIRE,      MFILE_KRULL,     SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Blockable   },
	{  &AddCbolt,                  &MI_Cbolt,          MIS_CBOLT,         true,      1, MISR_LIGHTNING, MFILE_MINILTNG,  LS_CBOLT,    SFX_NONE,    MissileMovementDistrubution::Blockable   },
	{  &AddHbolt,                  &MI_Hbolt,          MIS_HBOLT,         true,      1, MISR_NONE,      MFILE_HOLY,      LS_HOLYBOLT, LS_ELECIMP1, MissileMovementDistrubution::Blockable   },
	{  &AddResurrect,              &MI_Dummy,          MIS_RESURRECT,     false,     1, MISR_MAGIC,     MFILE_NONE,      SFX_NONE,    LS_RESUR,    MissileMovementDistrubution::Disabled    },
	{  &AddTelekinesis,            &MI_Dummy,          MIS_TELEKINESIS,   false,     1, MISR_NONE,      MFILE_NONE,      LS_ETHEREAL, SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddLArrow,                 &MI_LArrow,         MIS_LARROW,        true,      0, MISR_LIGHTNING, MFILE_LARROW,    SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Blockable   },
	{  &AddAcid,                   &MI_Firebolt,       MIS_ACID,          true,      1, MISR_ACID,      MFILE_ACIDBF,    LS_ACID,     SFX_NONE,    MissileMovementDistrubution::Blockable   },
	{  &AddMisexp,                 &MI_Acidsplat,      MIS_MISEXP3,       true,      2, MISR_ACID,      MFILE_ACIDSPLA,  SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddAcidpud,                &MI_Acidpud,        MIS_ACIDPUD,       true,      2, MISR_ACID,      MFILE_ACIDPUD,   LS_PUDDLE,   SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddHealOther,              &MI_Dummy,          MIS_HEALOTHER,     false,     1, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddElement,                &MI_Element,        MIS_ELEMENT,       true,      1, MISR_FIRE,      MFILE_FIRERUN,   LS_ELEMENTL, SFX_NONE,    MissileMovementDistrubution::Unblockable },
	{  &AddResurrectBeam,          &MI_ResurrectBeam,  MIS_RESURRECTBEAM, true,      1, MISR_NONE,      MFILE_RESSUR1,   SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddBoneSpirit,             &MI_Bonespirit,     MIS_BONESPIRIT,    true,      1, MISR_MAGIC,     MFILE_SKLBALL,   LS_BONESP,   LS_BSIMPCT,  MissileMovementDistrubution::Blockable   },
	{  &AddWeapexp,                &MI_Weapexp,        MIS_WEAPEXP,       true,      2, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddRportal,                &MI_Rportal,        MIS_RPORTAL,       true,      2, MISR_NONE,      MFILE_RPORTAL,   LS_SENTINEL, LS_ELEMENTL, MissileMovementDistrubution::Disabled    },
	{  &AddBoom,                   &MI_Boom,           MIS_BOOM2,         true,      2, MISR_NONE,      MFILE_FIREPLAR,  SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddDiabApoca,              &MI_Dummy,          MIS_DIABAPOCA,     false,     2, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddMana,                   &MI_Dummy,          MIS_MANA,          false,     1, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddMagi,                   &MI_Dummy,          MIS_MAGI,          false,     1, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddLightningWall,          &MI_LightningWall,  MIS_LIGHTWALL,     true,      1, MISR_LIGHTNING, MFILE_LGHNING,   LS_LMAG,     LS_ELECIMP1, MissileMovementDistrubution::Disabled    },
	{  &AddFirewallC,              &MI_LightningWallC, MIS_LIGHTNINGWALL, false,     1, MISR_LIGHTNING, MFILE_LGHNING,   SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddNova,                   &MI_FireNova,       MIS_IMMOLATION,    true,      1, MISR_FIRE,      MFILE_FIREBA,    LS_FBOLT1,   LS_FIRIMP2,  MissileMovementDistrubution::Disabled    },
	{  &AddSpecArrow,              &MI_SpecArrow,      MIS_SPECARROW,     true,      0, MISR_NONE,      MFILE_ARROWS,    SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddFireNova,               &MI_Fireball,       MIS_FIRENOVA,      true,      1, MISR_FIRE,      MFILE_FIREBA,    IS_FBALLBOW, LS_FIRIMP2,  MissileMovementDistrubution::Blockable   },
	{  &AddLightningArrow,         &MI_LightningArrow, MIS_LIGHTARROW,    false,     1, MISR_LIGHTNING, MFILE_LGHNING,   IS_FBALLBOW, SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddCboltArrow,             &MI_Cbolt,          MIS_CBOLTARROW,    true,      1, MISR_LIGHTNING, MFILE_MINILTNG,  LS_CBOLT,    SFX_NONE,    MissileMovementDistrubution::Blockable   },
	{  &AddHbolt,                  &MI_Hbolt,          MIS_HBOLTARROW,    true,      1, MISR_NONE,      MFILE_HOLY,      LS_HOLYBOLT, LS_ELECIMP1, MissileMovementDistrubution::Blockable   },
	{  &AddWarp,                   &MI_Teleport,       MIS_WARP,          false,     1, MISR_NONE,      MFILE_NONE,      LS_ETHEREAL, SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddReflection,             &MI_Reflect,        MIS_REFLECT,       false,     1, MISR_NONE,      MFILE_REFLECT,   LS_MSHIELD,  SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddBerserk,                &MI_Dummy,          MIS_BERSERK,       false,     1, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddRing,                   &MI_FireRing,       MIS_FIRERING,      false,     1, MISR_FIRE,      MFILE_FIREWAL,   SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddStealPotions,           &MI_Dummy,          MIS_STEALPOTS,     false,     1, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddManaTrap,               &MI_Dummy,          MIS_MANATRAP,      false,     1, MISR_NONE,      MFILE_NONE,      IS_CAST7,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddRing,                   &MI_LightningRing,  MIS_LIGHTRING,     false,     1, MISR_LIGHTNING, MFILE_LGHNING,   SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddSearch,                 &MI_Search,         MIS_SEARCH,        false,     1, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  nullptr,                    nullptr,            MIS_FLASHFR,       false,     1, MISR_MAGIC,     MFILE_BLUEXFR,   SFX_NONE,    LS_ELECIMP1, MissileMovementDistrubution::Disabled    },
	{  nullptr,                    nullptr,            MIS_FLASHBK,       false,     1, MISR_MAGIC,     MFILE_BLUEXBK,   SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddImmolation,             &MI_Immolation,     MIS_IMMOLATION2,   true,      1, MISR_FIRE,      MFILE_FIREBA,    LS_FBOLT1,   LS_FIRIMP2,  MissileMovementDistrubution::Disabled    },
	{  &AddFireRune,               &MI_Rune,           MIS_RUNEFIRE,      true,      1, MISR_NONE,      MFILE_RUNE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddLightningRune,          &MI_Rune,           MIS_RUNELIGHT,     true,      1, MISR_NONE,      MFILE_RUNE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddGreatLightningRune,     &MI_Rune,           MIS_RUNENOVA,      true,      1, MISR_NONE,      MFILE_RUNE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddImmolationRune,         &MI_Rune,           MIS_RUNEIMMOLAT,   true,      1, MISR_NONE,      MFILE_RUNE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddStoneRune,              &MI_Rune,           MIS_RUNESTONE,     true,      1, MISR_NONE,      MFILE_RUNE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddRuneExplosion,          &MI_HiveExplode,    MIS_HIVEEXP,       true,      1, MISR_FIRE,      MFILE_BIGEXP,    LS_NESTXPLD, LS_NESTXPLD, MissileMovementDistrubution::Disabled    },
	{  &AddHorkSpawn,              &MI_HorkSpawn,      MIS_HORKDMN,       true,      2, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddJester,                 &MI_Dummy,          MIS_JESTER,        false,     2, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddHiveExplosion,          &MI_Dummy,          MIS_HIVEEXP2,      false,     2, MISR_NONE,      MFILE_NONE,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddFlare,                  &MI_Firebolt,       MIS_LICH,          true,      1, MISR_MAGIC,     MFILE_LICH,      SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Blockable   },
	{  &AddFlare,                  &MI_Firebolt,       MIS_PSYCHORB,      true,      1, MISR_MAGIC,     MFILE_BONEDEMON, SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Blockable   },
	{  &AddFlare,                  &MI_Firebolt,       MIS_NECROMORB,     true,      1, MISR_MAGIC,     MFILE_NECROMORB, SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Blockable   },
	{  &AddFlare,                  &MI_Firebolt,       MIS_ARCHLICH,      true,      1, MISR_MAGIC,     MFILE_ARCHLICH,  SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Blockable   },
	{  &AddFlare,                  &MI_Firebolt,       MIS_BONEDEMON,     true,      1, MISR_MAGIC,     MFILE_BONEDEMON, SFX_NONE,    SFX_NONE,    MissileMovementDistrubution::Blockable   },
	{  &AddMisexp,                 &MI_Misexp,         MIS_EXYEL2,        true,      2, MISR_NONE,      MFILE_EXYEL2,    LS_FIRIMP2,  SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddMisexp,                 &MI_Misexp,         MIS_EXRED3,        true,      2, MISR_NONE,      MFILE_EXRED3,    LS_FIRIMP2,  SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddMisexp,                 &MI_Misexp,         MIS_EXBL2,         true,      2, MISR_NONE,      MFILE_EXBL2,     LS_FIRIMP2,  SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddMisexp,                 &MI_Misexp,         MIS_EXBL3,         true,      2, MISR_NONE,      MFILE_EXBL3,     LS_FIRIMP2,  SFX_NONE,    MissileMovementDistrubution::Disabled    },
	{  &AddMisexp,                 &MI_Misexp,         MIS_EXORA1,        true,      2, MISR_NONE,      MFILE_EXORA1,    LS_FIRIMP2,  SFX_NONE,    MissileMovementDistrubution::Disabled    },
	// clang-format on
};

/** Data related to each missile graphic ID. */
MisFileData MissileSpriteData[] = {
	// clang-format off
	// name,      animName,        animFAmt, flags,                animDelay[16], animLen[16],                           animWidth[16],          animWidth2[16]
	{ "Arrows",   MFILE_ARROWS,     1,       MFLAG_LOCK_ANIMATION,    0,            16,                                     96,                     16               },
	{ "Fireba",   MFILE_FIREBA,    16,       0,                       0,            14,                                     96,                     16               },
	{ "Guard",    MFILE_GUARD,      3,       0,                       1,          { 15, 14,  3 },                           96,                     16               },
	{ "Lghning",  MFILE_LGHNING,    1,       0,                       0,             8,                                     96,                     16               },
	{ "Firewal",  MFILE_FIREWAL,    2,       0,                       0,          { 13, 11 },                              128,                     32               },
	{ "MagBlos",  MFILE_MAGBLOS,    1,       0,                       1,            10,                                    128,                     32               },
	{ "Portal",   MFILE_PORTAL,     2,       0,                    {  0, 1 },       16,                                     96,                     16               },
	{ "Bluexfr",  MFILE_BLUEXFR,    1,       0,                       0,            19,                                    160,                     48               },
	{ "Bluexbk",  MFILE_BLUEXBK,    1,       0,                       0,            19,                                    160,                     48               },
	{ "Manashld", MFILE_MANASHLD,   1,       MFLAG_LOCK_ANIMATION,    0,             1,                                     96,                     16               },
	{ "Blood",    MFILE_BLOOD,      4,       0,                       0,          { 15,  8,  8,  8 },                    {  96, 128, 128, 128 }, {  16, 32, 32, 32 } },
	{ "Bone",     MFILE_BONE,       3,       0,                       2,             8,                                    128,                     32               },
	{ "Metlhit",  MFILE_METLHIT,    3,       0,                       2,            10,                                     96,                     16               },
	{ "Farrow",   MFILE_FARROW,    16,       0,                       0,             4,                                     96,                     16               },
	{ "Doom",     MFILE_DOOM,       9,       MFLAG_HIDDEN,            1,            15,                                     96,                     16               },
	{ " ",        MFILE_0F,         1,       MFLAG_HIDDEN,            0,             0,                                      0,                      0               },
	{ "Blodbur",  MFILE_BLODBUR,    2,       0,                       2,             8,                                    128,                     32               },
	{ "Newexp",   MFILE_NEWEXP,     1,       0,                       1,            15,                                     96,                     16               },
	{ "Shatter1", MFILE_SHATTER1,   1,       0,                       1,            12,                                    128,                     32               },
	{ "Bigexp",   MFILE_BIGEXP,     1,       0,                       0,            15,                                    160,                     48               },
	{ "Inferno",  MFILE_INFERNO,    1,       0,                       0,            20,                                     96,                     16               },
	{ "Thinlght", MFILE_THINLGHT,   1,       MFLAG_HIDDEN,            0,             8,                                     96,                     16               },
	{ "Flare",    MFILE_FLARE,      1,       0,                       0,            16,                                    128,                     32               },
	{ "Flareexp", MFILE_FLAREEXP,   1,       0,                       0,             7,                                    128,                     32               },
	{ "Magball",  MFILE_MAGBALL,    8,       MFLAG_HIDDEN,            1,            16,                                    128,                     32               },
	{ "Krull",    MFILE_KRULL,      1,       MFLAG_HIDDEN,            0,            14,                                     96,                     16               },
	{ "Miniltng", MFILE_MINILTNG,   1,       0,                       1,             8,                                     64,                      0               },
	{ "Holy",     MFILE_HOLY,      16,       0,                    {  1, 0 },       14,                                     96,                     16               },
	{ "Holyexpl", MFILE_HOLYEXPL,   1,       0,                       0,             8,                                    160,                     48               },
	{ "Larrow",   MFILE_LARROW,    16,       0,                       0,             4,                                     96,                     16               },
	{ "Firarwex", MFILE_FIRARWEX,   1,       0,                       0,             6,                                     64,                      0               },
	{ "Acidbf",   MFILE_ACIDBF,    16,       MFLAG_HIDDEN,            0,             8,                                     96,                     16               },
	{ "Acidspla", MFILE_ACIDSPLA,   1,       MFLAG_HIDDEN,            0,             8,                                     96,                     16               },
	{ "Acidpud",  MFILE_ACIDPUD,    2,       MFLAG_HIDDEN,            0,          {  9,  4 },                               96,                     16               },
	{ "Ethrshld", MFILE_ETHRSHLD,   1,       0,                       0,             1,                                     96,                     16               },
	{ "Firerun",  MFILE_FIRERUN,    8,       0,                       1,            12,                                     96,                     16               },
	{ "Ressur1",  MFILE_RESSUR1,    1,       0,                       0,            16,                                     96,                     16               },
	{ "Sklball",  MFILE_SKLBALL,    9,       0,                       1,          { 16, 16, 16, 16, 16, 16, 16, 16, 8 },    96,                     16               },
	{ "Rportal",  MFILE_RPORTAL,    2,       0,                       0,            16,                                     96,                     16               },
	{ "Fireplar", MFILE_FIREPLAR,   1,       MFLAG_HIDDEN,            1,            17,                                    160,                     48               },
	{ "Scubmisb", MFILE_SCUBMISB,   1,       MFLAG_HIDDEN,            0,            16,                                     96,                     16               },
	{ "Scbsexpb", MFILE_SCBSEXPB,   1,       MFLAG_HIDDEN,            0,             6,                                    128,                     32               },
	{ "Scubmisc", MFILE_SCUBMISC,   1,       MFLAG_HIDDEN,            0,            16,                                     96,                     16               },
	{ "Scbsexpc", MFILE_SCBSEXPC,   1,       MFLAG_HIDDEN,            0,             6,                                    128,                     32               },
	{ "Scubmisd", MFILE_SCUBMISD,   1,       MFLAG_HIDDEN,            0,            16,                                     96,                     16               },
	{ "Scbsexpd", MFILE_SCBSEXPD,   1,       MFLAG_HIDDEN,            0,             6,                                    128,                     32               },
	{ "spawns",   MFILE_SPAWNS,     8,       MFLAG_HIDDEN,            0,             9,                                     96,                     16               },
	{ "reflect",  MFILE_REFLECT,    1,       MFLAG_LOCK_ANIMATION,    0,             1,                                    160,                     64               },
	{ "ms_ora",   MFILE_LICH,      16,       MFLAG_HIDDEN,            0,            15,                                     96,                      8               },
	{ "ms_bla",   MFILE_MSBLA,     16,       MFLAG_HIDDEN,            0,            15,                                     96,                      8               },
	{ "ms_reb",   MFILE_NECROMORB, 16,       MFLAG_HIDDEN,            0,            15,                                     96,                      8               },
	{ "ms_yeb",   MFILE_ARCHLICH,  16,       MFLAG_HIDDEN,            0,            15,                                     96,                      8               },
	{ "rglows1",  MFILE_RUNE,       1,       0,                       0,            10,                                     96,                      8               },
	{ "ex_yel2",  MFILE_EXYEL2,     1,       MFLAG_HIDDEN,            0,            10,                                    220,                     78               },
	{ "ex_blu2",  MFILE_EXBL2,      1,       MFLAG_HIDDEN,            0,            10,                                    212,                     86               },
	{ "ex_red3",  MFILE_EXRED3,     1,       MFLAG_HIDDEN,            0,             7,                                    292,                    114               },
	{ "ms_blb",   MFILE_BONEDEMON, 16,       MFLAG_HIDDEN,            0,            15,                                     96,                      8               },
	{ "ex_ora1",  MFILE_EXORA1,     1,       MFLAG_HIDDEN,            0,            13,                                     96,                    -12               },
	{ "ex_blu3",  MFILE_EXBL3,      1,       MFLAG_HIDDEN,            0,             7,                                    292,                    114               },
	{ "",         MFILE_NONE,       0,       0                                                                                                                       },
	// clang-format on
};

MisFileData::MisFileData(const char *name, uint8_t animName, uint8_t animFAmt, uint32_t flags,
			 AutofillArray<uint8_t, 16> animDelay, AutofillArray<uint8_t, 16> animLen,
			 AutofillArray<int16_t, 16> animWidth, AutofillArray<int16_t, 16> animWidth2)
	: name(name)
	, animName(animName)
	, animFAmt(animFAmt)
	, flags(flags)
	, animDelay(animDelay)
	, animLen(animLen)
	, animWidth(animWidth)
	, animWidth2(animWidth2)
{
	if (flags & MFLAG_ALLOW_SPECIAL)
		pinnedMem.reserve(1);
	else
		pinnedMem.reserve(animFAmt);

	for (int i = animFAmt; i < 16; i++) {
		animDelay[i] = 0;
		animLen[i] = 0;
		animWidth = 0;
		animWidth2 = 0;
	}
}

void MisFileData::LoadGFX()
{
	if (animData[0] != nullptr)
		return;

	if ((flags & MFLAG_HIDDEN) != 0)
		return;

	char pszName[256];
	if ((flags & MFLAG_ALLOW_SPECIAL) != 0) {
		sprintf(pszName, "Missiles\\%s.CL2", name);
		auto file = LoadFileInMem(pszName);
		for (int i = 0; i < animFAmt; i++)
			animData[i] = CelGetFrame(file.get(), i);
		pinnedMem.push_back(std::move(file));
	} else if (animFAmt == 1) {
		sprintf(pszName, "Missiles\\%s.CL2", name);
		auto file = LoadFileInMem(pszName);
		animData[0] = file.get();
		pinnedMem.push_back(std::move(file));
	} else {
		for (unsigned i = 0; i < animFAmt; i++) {
			sprintf(pszName, "Missiles\\%s%u.CL2", name, i + 1);
			auto file = LoadFileInMem(pszName);
			animData[i] = file.get();
			pinnedMem.push_back(std::move(file));
		}
	}
}

} // namespace devilution
