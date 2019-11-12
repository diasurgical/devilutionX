#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

MissileData missiledata[68] = {
	// clang-format off
	// mName,             mAddProc,          mProc,             mDraw, mType, mResist,        mFileNum,       miSFX,       mlSFX;
	{  MIS_ARROW,         &AddArrow,         &MI_Arrow,         TRUE,      0, 0,              MFILE_ARROWS,   -1,          -1          },
	{  MIS_FIREBOLT,      &AddFirebolt,      &MI_Firebolt,      TRUE,      1, MISR_FIRE,      MFILE_FIREBA,   LS_FBOLT1,   LS_FIRIMP2  },
	{  MIS_GUARDIAN,      &AddGuardian,      &MI_Guardian,      TRUE,      1, 0,              MFILE_GUARD,    LS_GUARD,    LS_GUARDLAN },
	{  MIS_RNDTELEPORT,   &AddRndTeleport,   &MI_Teleport,      FALSE,     1, 0,              MFILE_NONE,     LS_TELEPORT, -1          },
	{  MIS_LIGHTBALL,     &AddLightball,     &MI_Lightball,     TRUE,      1, MISR_LIGHTNING, MFILE_LGHNING,  -1,          -1          },
	{  MIS_FIREWALL,      &AddFirewall,      &MI_Firewall,      TRUE,      1, MISR_FIRE,      MFILE_FIREWAL,  LS_WALLLOOP, LS_FIRIMP2  },
	{  MIS_FIREBALL,      &AddFireball,      &MI_Fireball,      TRUE,      1, MISR_FIRE,      MFILE_FIREBA,   LS_FBOLT1,   LS_FIRIMP2  },
	{  MIS_LIGHTCTRL,     &AddLightctrl,     &MI_Lightctrl,     FALSE,     1, MISR_LIGHTNING, MFILE_LGHNING,  -1,          -1          },
	{  MIS_LIGHTNING,     &AddLightning,     &MI_Lightning,     TRUE,      1, MISR_LIGHTNING, MFILE_LGHNING,  LS_LNING1,   LS_ELECIMP1 },
	{  MIS_MISEXP,        &AddMisexp,        &MI_Misexp,        TRUE,      2, 0,              MFILE_MAGBLOS,  -1,          -1          },
	{  MIS_TOWN,          &AddTown,          &MI_Town,          TRUE,      1, MISR_MAGIC,     MFILE_PORTAL,   LS_SENTINEL, LS_ELEMENTL },
	{  MIS_FLASH,         &AddFlash,         &MI_Flash,         TRUE,      1, MISR_MAGIC,     MFILE_BLUEXFR,  LS_NOVA,     LS_ELECIMP1 },
	{  MIS_FLASH2,        &AddFlash2,        &MI_Flash2,        TRUE,      1, MISR_MAGIC,     MFILE_BLUEXBK,  -1,          -1          },
	{  MIS_MANASHIELD,    &AddManashield,    &MI_SetManashield, FALSE,     1, MISR_MAGIC,     MFILE_MANASHLD, LS_MSHIELD,  -1          },
	{  MIS_FIREMOVE,      &AddFiremove,      &MI_Firemove,      TRUE,      1, MISR_FIRE,      MFILE_FIREWAL,  -1,          -1          },
	{  MIS_CHAIN,         &AddChain,         &MI_Chain,         TRUE,      1, MISR_LIGHTNING, MFILE_LGHNING,  LS_LNING1,   LS_ELECIMP1 },
	{  MIS_SENTINAL,      NULL,              NULL,              TRUE,      1, MISR_LIGHTNING, MFILE_LGHNING,  -1,          -1          },
	{  MIS_BLODSTAR,      &miss_null_11,     &mi_null_11,       TRUE,      2, 0,              MFILE_BLOOD,    LS_BLODSTAR, LS_BLSIMPT  },
	{  MIS_BONE,          &miss_null_12,     &mi_null_11,       TRUE,      2, 0,              MFILE_BONE,     -1,          -1          },
	{  MIS_METLHIT,       &miss_null_13,     &mi_null_11,       TRUE,      2, 0,              MFILE_METLHIT,  -1,          -1          },
	{  MIS_RHINO,         &AddRhino,         &MI_Rhino,         TRUE,      2, 0,              MFILE_NONE,     -1,          -1          },
	{  MIS_MAGMABALL,     &AddMagmaball,     &MI_Firebolt,      TRUE,      1, MISR_FIRE,      MFILE_MAGBALL,  -1,          -1          },
	{  MIS_LIGHTCTRL2,    &AddLightctrl,     &MI_Lightctrl,     FALSE,     1, MISR_LIGHTNING, MFILE_THINLGHT, -1,          -1          },
	{  MIS_LIGHTNING2,    &AddLightning,     &MI_Lightning,     TRUE,      1, MISR_LIGHTNING, MFILE_THINLGHT, -1,          -1          },
	{  MIS_FLARE,         &AddFlare,         &MI_Firebolt,      TRUE,      1, MISR_MAGIC,     MFILE_FLARE,    -1,          -1          },
	{  MIS_MISEXP2,       &AddMisexp,        &MI_Misexp,        TRUE,      2, MISR_MAGIC,     MFILE_FLAREEXP, -1,          -1          },
	{  MIS_TELEPORT,      &AddTeleport,      &MI_Teleport,      FALSE,     1, 0,              MFILE_NONE,     LS_ELEMENTL, -1          },
	{  MIS_FARROW,        &AddLArrow,        &MI_LArrow,        TRUE,      0, MISR_FIRE,      MFILE_FARROW,   -1,          -1          },
	{  MIS_DOOMSERP,      NULL,              NULL,              FALSE,     1, MISR_MAGIC,     MFILE_DOOM,     LS_DSERP,    -1          },
	{  MIS_FIREWALLA,     &miss_null_1D,     &MI_Firewall,      TRUE,      2, MISR_FIRE,      MFILE_FIREWAL,  -1,          -1          },
	{  MIS_STONE,         &AddStone,         &MI_Stone,         FALSE,     1, MISR_MAGIC,     MFILE_NONE,     LS_SCURIMP,  -1          },
	{  MIS_NULL_1F,       &miss_null_1F,     &MI_Dummy,         TRUE,      1, 0,              MFILE_NONE,     -1,          -1          },
	{  MIS_INVISIBL,      NULL,              NULL,              FALSE,     1, 0,              MFILE_NONE,     LS_INVISIBL, -1          },
	{  MIS_GOLEM,         &AddGolem,         &MI_Golem,         FALSE,     1, 0,              MFILE_NONE,     LS_GOLUM,    -1          },
	{  MIS_ETHEREALIZE,   &AddEtherealize,   &MI_Etherealize,   TRUE,      1, 0,              MFILE_ETHRSHLD, LS_ETHEREAL, -1          },
	{  MIS_BLODBUR,       &miss_null_23,     &mi_null_11,       TRUE,      2, 0,              MFILE_BLODBUR,  -1,          -1          },
	{  MIS_BOOM,          &AddBoom,          &MI_Boom,          TRUE,      2, 0,              MFILE_NEWEXP,   -1,          -1          },
	{  MIS_HEAL,          &AddHeal,          &MI_Dummy,         FALSE,     1, 0,              MFILE_NONE,     -1,          -1          },
	{  MIS_FIREWALLC,     &AddFirewallC,     &MI_FirewallC,     FALSE,     1, MISR_FIRE,      MFILE_FIREWAL,  -1,          -1          },
	{  MIS_INFRA,         &AddInfra,         &MI_Infra,         FALSE,     1, 0,              MFILE_NONE,     LS_INFRAVIS, -1          },
	{  MIS_IDENTIFY,      &AddIdentify,      &MI_Dummy,         FALSE,     1, 0,              MFILE_NONE,     -1,          -1          },
	{  MIS_WAVE,          &AddWave,          &MI_Wave,          TRUE,      1, MISR_FIRE,      MFILE_FIREWAL,  LS_FLAMWAVE, -1          },
	{  MIS_NOVA,          &AddNova,          &MI_Nova,          TRUE,      1, MISR_LIGHTNING, MFILE_LGHNING,  LS_NOVA,     -1          },
	{  MIS_BLODBOIL,      &miss_null_1F,     &MI_Blodboil,      TRUE,      1, 0,              MFILE_NONE,     -1,          LS_BLODBOIL },
	{  MIS_APOCA,         &AddApoca,         &MI_Apoca,         TRUE,      1, MISR_MAGIC,     MFILE_NEWEXP,   LS_APOC,     -1          },
	{  MIS_REPAIR,        &AddRepair,        &MI_Dummy,         FALSE,     2, 0,              MFILE_NONE,     -1,          -1          },
	{  MIS_RECHARGE,      &AddRecharge,      &MI_Dummy,         FALSE,     2, 0,              MFILE_NONE,     -1,          -1          },
	{  MIS_DISARM,        &AddDisarm,        &MI_Dummy,         FALSE,     2, 0,              MFILE_NONE,     LS_TRAPDIS,  -1          },
	{  MIS_FLAME,         &AddFlame,         &MI_Flame,         TRUE,      1, MISR_FIRE,      MFILE_INFERNO,  LS_SPOUTSTR, -1          },
	{  MIS_FLAMEC,        &AddFlamec,        &MI_Flamec,        FALSE,     1, MISR_FIRE,      MFILE_NONE,     -1,          -1          },
	{  MIS_FIREMAN,       &miss_null_32,     &mi_null_32,       TRUE,      2, 0,              MFILE_NONE,     -1,          -1          },
	{  MIS_KRULL,         &miss_null_33,     &mi_null_33,       TRUE,      0, MISR_FIRE,      MFILE_KRULL,    -1,          -1          },
	{  MIS_CBOLT,         &AddCbolt,         &MI_Cbolt,         TRUE,      1, MISR_LIGHTNING, MFILE_MINILTNG, LS_CBOLT,    -1          },
	{  MIS_HBOLT,         &AddHbolt,         &MI_Hbolt,         TRUE,      1, 0,              MFILE_HOLY,     LS_HOLYBOLT, LS_ELECIMP1 },
	{  MIS_RESURRECT,     &AddResurrect,     &MI_Dummy,         FALSE,     1, MISR_MAGIC,     MFILE_NONE,     -1,          LS_RESUR    },
	{  MIS_TELEKINESIS,   &AddTelekinesis,   &MI_Dummy,         FALSE,     1, 0,              MFILE_NONE,     LS_ETHEREAL, -1          },
	{  MIS_LARROW,        &AddLArrow,        &MI_LArrow,        TRUE,      0, MISR_LIGHTNING, MFILE_LARROW,   -1,          -1          },
	{  MIS_ACID,          &AddAcid,          &MI_Firebolt,      TRUE,      1, MISR_ACID,      MFILE_ACIDBF,   LS_ACID,     -1          },
	{  MIS_MISEXP3,       &AddMisexp,        &MI_Acidsplat,     TRUE,      2, MISR_ACID,      MFILE_ACIDSPLA, -1,          -1          },
	{  MIS_ACIDPUD,       &AddAcidpud,       &MI_Acidpud,       TRUE,      2, MISR_ACID,      MFILE_ACIDPUD,  LS_PUDDLE,   -1          },
	{  MIS_HEALOTHER,     &AddHealOther,     &MI_Dummy,         FALSE,     1, 0,              MFILE_NONE,     -1,          -1          },
	{  MIS_ELEMENT,       &AddElement,       &MI_Element,       TRUE,      1, MISR_FIRE,      MFILE_FIRERUN,  LS_ELEMENTL, -1          },
	{  MIS_RESURRECTBEAM, &AddResurrectBeam, &MI_ResurrectBeam, TRUE,      1, 0,              MFILE_RESSUR1,  -1,          -1          },
	{  MIS_BONESPIRIT,    &AddBoneSpirit,    &MI_Bonespirit,    TRUE,      1, MISR_MAGIC,     MFILE_SKLBALL,  LS_BONESP,   LS_BSIMPCT  },
	{  MIS_WEAPEXP,       &AddWeapexp,       &MI_Weapexp,       TRUE,      2, 0,              MFILE_NONE,     -1,          -1          },
	{  MIS_RPORTAL,       &AddRportal,       &MI_Rportal,       TRUE,      2, 0,              MFILE_RPORTAL,  LS_SENTINEL, LS_ELEMENTL },
	{  MIS_BOOM2,         &AddBoom,          &MI_Boom,          TRUE,      2, 0,              MFILE_FIREPLAR, -1,          -1          },
	{  MIS_DIABAPOCA,     &AddDiabApoca,     &MI_Dummy,         FALSE,     2, 0,              MFILE_NONE,     -1,          -1          }
	// clang-format on
};

MisFileData misfiledata[47] = {
	// clang-format off
	// mAnimName, mAnimFAmt, mName, mFlags, mAnimData[16],                                      mAnimDelay[16],                                     mAnimLen[16],                                                       mAnimWidth[16],                                                             mAnimWidth2[16]
	{  MFILE_ARROWS,      1, "Arrows",   2, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 16,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }, {  96,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0 }, { 16,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } },
	{  MFILE_FIREBA,     16, "Fireba",   0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 }, {  96,  96,  96,  96,  96,  96,  96,  96, 96, 96, 96, 96, 96, 96, 96, 96 }, { 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16 } },
	{  MFILE_GUARD,       3, "Guard",    0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 15, 14,  3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }, {  96,  96,  96,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0 }, { 16, 16, 16,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } },
	{  MFILE_LGHNING,     1, "Lghning",  0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, {  8,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }, {  96,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0 }, { 16,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } },
	{  MFILE_FIREWAL,     2, "Firewal",  0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 13, 11,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }, { 128, 128,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0 }, { 32, 32,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } },
	{  MFILE_MAGBLOS,     1, "MagBlos",  0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 10,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }, { 128,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0 }, { 32,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } },
	{  MFILE_PORTAL,      2, "Portal",   0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 16, 16,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }, {  96,  96,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0 }, { 16, 16,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } },
	{  MFILE_BLUEXFR,     1, "Bluexfr",  0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 19,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }, { 160,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0 }, { 48,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } },
	{  MFILE_BLUEXBK,     1, "Bluexbk",  0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 19,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }, { 160,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0 }, { 48,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } },
	{  MFILE_MANASHLD,    1, "Manashld", 2, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, {  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }, {  96,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0 }, { 16,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } },
	{  MFILE_BLOOD,       4, "Blood",    0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 15,  8,  8,  8,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }, {  96, 128, 128, 128,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0 }, { 16, 32, 32, 32,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } },
	{  MFILE_BONE,        3, "Bone",     0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, {  8,  8,  8,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }, { 128, 128, 128,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0 }, { 32, 32, 32,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } },
	{  MFILE_METLHIT,     3, "Metlhit",  0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 10, 10, 10,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }, {  96,  96,  96,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0 }, { 16, 16, 16,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } },
	{  MFILE_FARROW,     16, "Farrow",   0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, {  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4 }, {  96,  96,  96,  96,  96,  96,  96,  96, 96, 96, 96, 96, 96, 96, 96, 96 }, { 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16 } },
	{  MFILE_DOOM,        9, "Doom",     1, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 }, { 15, 15, 15, 15, 15, 15, 15, 15, 15,  0,  0,  0,  0,  0,  0,  0 }, {  96,  96,  96,  96,  96,  96,  96,  96, 96,  0,  0,  0,  0,  0,  0,  0 }, { 16, 16, 16, 16, 16, 16, 16, 16, 16,  0,  0,  0,  0,  0,  0,  0 } },
	{  MFILE_0F,          1, " ",        1, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }, {   0,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0 }, {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } },
	{  MFILE_BLODBUR,     2, "Blodbur",  0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, {  8,  8,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }, { 128, 128,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0 }, { 32, 32,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } },
	{  MFILE_NEWEXP,      1, "Newexp",   0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 15,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }, {  96,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0 }, { 16,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } },
	{  MFILE_SHATTER1,    1, "Shatter1", 0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 12,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }, { 128,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0 }, { 32,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } },
	{  MFILE_BIGEXP,      1, "Bigexp",   0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 15,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }, { 160,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0 }, { 48,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } },
	{  MFILE_INFERNO,     1, "Inferno",  0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 20,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }, {  96,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0 }, { 16,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } },
	{  MFILE_THINLGHT,    1, "Thinlght", 1, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, {  8,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }, {  96,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0 }, { 16,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } },
	{  MFILE_FLARE,       1, "Flare",    0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 16,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }, { 128,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0 }, { 32,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } },
	{  MFILE_FLAREEXP,    1, "Flareexp", 0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, {  7,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }, { 128,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0 }, { 32,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } },
	{  MFILE_MAGBALL,     8, "Magball",  1, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 }, { 16, 16, 16, 16, 16, 16, 16, 16,  0,  0,  0,  0,  0,  0,  0,  0 }, { 128, 128, 128, 128, 128, 128, 128, 128,  0,  0,  0,  0,  0,  0,  0,  0 }, { 32, 32, 32, 32, 32, 32, 32, 32,  0,  0,  0,  0,  0,  0,  0,  0 } },
	{  MFILE_KRULL,       1, "Krull",    1, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 14,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }, {  96,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0 }, { 16,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } },
	{  MFILE_MINILTNG,    1, "Miniltng", 0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, {  8,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }, {  64,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0 }, {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } },
	{  MFILE_HOLY,       16, "Holy",     0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 }, {  96,  96,  96,  96,  96,  96,  96,  96, 96, 96, 96, 96, 96, 96, 96, 96 }, { 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16 } },
	{  MFILE_HOLYEXPL,    1, "Holyexpl", 0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, {  8,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }, { 160,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0 }, { 48,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } },
	{  MFILE_LARROW,     16, "Larrow",   0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, {  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4 }, {  96,  96,  96,  96,  96,  96,  96,  96, 96, 96, 96, 96, 96, 96, 96, 96 }, { 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16 } },
	{  MFILE_FIRARWEX,    1, "Firarwex", 0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, {  6,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }, {  64,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0 }, {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } },
	{  MFILE_ACIDBF,     16, "Acidbf",   1, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, {  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8 }, {  96,  96,  96,  96,  96,  96,  96,  96, 96, 96, 96, 96, 96, 96, 96, 96 }, { 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16 } },
	{  MFILE_ACIDSPLA,    1, "Acidspla", 1, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, {  8,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }, {  96,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0 }, { 16,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } },
	{  MFILE_ACIDPUD,     2, "Acidpud",  1, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, {  9,  4,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }, {  96,  96,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0 }, { 16, 16,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } },
	{  MFILE_ETHRSHLD,    1, "Ethrshld", 0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, {  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }, {  96,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0 }, { 16,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } },
	{  MFILE_FIRERUN,     8, "Firerun",  0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 }, { 12, 12, 12, 12, 12, 12, 12, 12,  0,  0,  0,  0,  0,  0,  0,  0 }, {  96,  96,  96,  96,  96,  96,  96,  96,  0,  0,  0,  0,  0,  0,  0,  0 }, { 16, 16, 16, 16, 16, 16, 16, 16,  0,  0,  0,  0,  0,  0,  0,  0 } },
	{  MFILE_RESSUR1,     1, "Ressur1",  0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 16,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }, {  96,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0 }, { 16,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } },
	{  MFILE_SKLBALL,     9, "Sklball",  0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 }, { 16, 16, 16, 16, 16, 16, 16, 16,  8,  0,  0,  0,  0,  0,  0,  0 }, {  96,  96,  96,  96,  96,  96,  96,  96, 96,  0,  0,  0,  0,  0,  0,  0 }, { 16, 16, 16, 16, 16, 16, 16, 16, 16,  0,  0,  0,  0,  0,  0,  0 } },
	{  MFILE_RPORTAL,     2, "Rportal",  0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 16, 16,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }, {  96,  96,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0 }, { 16, 16,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } },
	{  MFILE_FIREPLAR,    1, "Fireplar", 1, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 17,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }, { 160,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0 }, { 48,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } },
	{  MFILE_SCUBMISB,    1, "Scubmisb", 1, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 16,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }, {  96,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0 }, { 16,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } },
	{  MFILE_SCBSEXPB,    1, "Scbsexpb", 1, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, {  6,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }, { 128,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0 }, { 32,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } },
	{  MFILE_SCUBMISC,    1, "Scubmisc", 1, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 16,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }, {  96,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0 }, { 16,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } },
	{  MFILE_SCBSEXPC,    1, "Scbsexpc", 1, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, {  6,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }, { 128,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0 }, { 32,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } },
	{  MFILE_SCUBMISD,    1, "Scubmisd", 1, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 16,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }, {  96,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0 }, { 16,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } },
	{  MFILE_SCBSEXPD,    1, "Scbsexpd", 1, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, {  6,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }, { 128,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0 }, { 32,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } },
	{  MFILE_NONE,        0, "",         0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }, {   0,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0 }, {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } }
	// clang-format on
};

DEVILUTION_END_NAMESPACE
