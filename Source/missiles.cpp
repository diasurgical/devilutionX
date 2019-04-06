//HEADER_GOES_HERE

#include "../types.h"

DEVILUTION_BEGIN_NAMESPACE

int missileactive[MAXMISSILES];
int missileavail[MAXMISSILES];
MissileStruct missile[MAXMISSILES];
int nummissiles; // idb
int ManashieldFlag;
ChainStruct chain[MAXMISSILES];
int MissilePreFlag; // weak
int numchains;      // weak

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
int XDirAdd[8] = { 1, 0, -1, -1, -1, 0, 1, 1 };
int YDirAdd[8] = { 1, 1, 1, 0, -1, -1, -1, 0 };

void __fastcall GetDamageAmt(int i, int *mind, int *maxd)
{
	int v3;         // eax
	int v4;         // esi
	int v5;         // eax
	int v6;         // ecx
	int v7;         // eax
	int *v8;        // eax
	signed int v9;  // ecx
	int v10;        // eax
	int v11;        // ecx
	int v12;        // eax
	int v13;        // eax
	int v14;        // eax
	int v15;        // ecx
	int *v16;       // ecx
	int v17;        // eax
	int v18;        // ecx
	int v19;        // eax
	int v20;        // ecx
	int v21;        // eax
	signed int v22; // eax
	signed int v23; // ecx
	int v24;        // eax
	int v25;        // ecx
	int v26;        // ecx
	int v27;        // eax
	signed int v28; // ecx

	v3 = myplr;
	v4 = plr[myplr]._pISplLvlAdd + plr[myplr]._pSplLvl[i];
	switch (i) {
	case SPL_FIREBOLT:
		*mind = (plr[v3]._pMagic >> 3) + v4 + 1;
		v5 = (plr[myplr]._pMagic >> 3) + v4 + 10;
		goto LABEL_73;
	case SPL_HEAL:
		v6 = plr[v3]._pLevel + v4 + 1;
		*mind = v6;
		v7 = myplr;
		if (plr[myplr]._pClass == PC_WARRIOR) {
			*mind = 2 * v6;
			v7 = myplr;
		}
		if (plr[v7]._pClass == PC_ROGUE)
			*mind += *mind >> 1;
		v8 = maxd;
		v9 = 0;
		*maxd = 10;
		if (plr[myplr]._pLevel > 0) {
			do {
				*maxd += 4;
				++v9;
			} while (v9 < plr[myplr]._pLevel);
		}
		goto LABEL_65;
	case SPL_LIGHTNING:
		v10 = 2;
		*mind = 2;
		v11 = plr[myplr]._pLevel;
		goto LABEL_43;
	case SPL_FLASH:
		v12 = plr[v3]._pLevel;
		*mind = v12;
		if (v4 > 0) {
			do {
				v12 += v12 >> 3;
				--v4;
			} while (v4);
			*mind = v12;
		}
		v13 = (*mind >> 1) + *mind;
		*mind = v13;
		goto LABEL_33;
	case SPL_IDENTIFY:
	case SPL_TOWN:
	case SPL_STONE:
	case SPL_INFRA:
	case SPL_RNDTELEPORT:
	case SPL_MANASHIELD:
	case SPL_DOOMSERP:
	case SPL_BLODRIT:
	case SPL_INVISIBIL:
	case SPL_BLODBOIL:
	case SPL_TELEPORT:
	case SPL_ETHEREALIZE:
	case SPL_REPAIR:
	case SPL_RECHARGE:
	case SPL_DISARM:
	case SPL_RESURRECT:
	case SPL_TELEKINESIS:
	case SPL_BONESPIRIT:
		v8 = maxd;
		goto LABEL_71;
	case SPL_FIREWALL:
		*mind = (4 * plr[v3]._pLevel + 8) >> 1;
		v5 = (4 * plr[myplr]._pLevel + 80) >> 1;
		goto LABEL_73;
	case SPL_FIREBALL:
		v14 = 2 * plr[v3]._pLevel + 4;
		*mind = v14;
		if (v4 > 0) {
			v15 = v4;
			do {
				v14 += v14 >> 3;
				--v15;
			} while (v15);
			*mind = v14;
		}
		v16 = maxd;
		v5 = 2 * plr[myplr]._pLevel + 40; /// BUGFIX: set to `2 * (plr[myplr]._pLevel + 20) + 4`
		*maxd = v5;
		if (v4 <= 0)
			return;
		do {
			v5 += v5 >> 3;
			--v4;
		} while (v4);
		goto LABEL_74;
	case SPL_GUARDIAN:
		v17 = (plr[v3]._pLevel >> 1) + 1;
		*mind = v17;
		if (v4 > 0) {
			v18 = v4;
			do {
				v17 += v17 >> 3;
				--v18;
			} while (v18);
			*mind = v17;
		}
		v16 = maxd;
		v5 = (plr[myplr]._pLevel >> 1) + 10;
		*maxd = v5;
		if (v4 <= 0)
			return;
		do {
			v5 += v5 >> 3;
			--v4;
		} while (v4);
		goto LABEL_74;
	case SPL_CHAIN:
		*mind = 4;
		v5 = 2 * plr[myplr]._pLevel + 4;
		goto LABEL_73;
	case SPL_WAVE:
		*mind = 6 * (plr[v3]._pLevel + 1);
		v13 = 3 * (plr[myplr]._pLevel + 10);
	LABEL_33:
		v5 = 2 * v13;
		goto LABEL_73;
	case SPL_NOVA:
		v19 = (plr[v3]._pLevel + 5) >> 1;
		*mind = v19;
		if (v4 > 0) {
			v20 = v4;
			do {
				v19 += v19 >> 3;
				--v20;
			} while (v20);
			*mind = v19;
		}
		v16 = maxd;
		*mind *= 5;
		v21 = (plr[myplr]._pLevel + 30) >> 1;
		*maxd = v21;
		if (v4 > 0) {
			do {
				v21 += v21 >> 3;
				--v4;
			} while (v4);
			*maxd = v21;
		}
		v5 = 5 * *maxd;
		goto LABEL_74;
	case SPL_FLAME:
		*mind = 3;
		v10 = plr[myplr]._pLevel + 4;
		v11 = v10 >> 1;
	LABEL_43:
		*maxd = v10 + v11;
		return;
	case SPL_GOLEM:
		*mind = 11;
		*maxd = 17;
		return;
	case SPL_APOCA:
		*mind = 0;
		v22 = 0;
		if (plr[myplr]._pLevel > 0) {
			do {
				++*mind;
				++v22;
			} while (v22 < plr[myplr]._pLevel);
		}
		v23 = 0;
		*maxd = 0;
		if (plr[myplr]._pLevel > 0) {
			do {
				*maxd += 6;
				++v23;
			} while (v23 < plr[myplr]._pLevel);
		}
		return;
	case SPL_ELEMENT:
		v24 = 2 * plr[v3]._pLevel + 4;
		*mind = v24;
		if (v4 > 0) {
			v25 = v4;
			do {
				v24 += v24 >> 3;
				--v25;
			} while (v25);
			*mind = v24;
		}
		v16 = maxd;
		v5 = 2 * plr[myplr]._pLevel + 40; /// BUGFIX: set to `2 * (plr[myplr]._pLevel + 20) + 4`
		*maxd = v5;
		if (v4 <= 0)
			return;
		do {
			v5 += v5 >> 3;
			--v4;
		} while (v4);
		goto LABEL_74;
	case SPL_CBOLT:
		*mind = 1;
		v5 = (plr[myplr]._pMagic >> 2) + 1;
		goto LABEL_73;
	case SPL_HBOLT:
		*mind = plr[v3]._pLevel + 9;
		v5 = plr[myplr]._pLevel + 18;
		goto LABEL_73;
	case SPL_HEALOTHER:
		v26 = plr[v3]._pLevel + v4 + 1;
		*mind = v26;
		v27 = myplr;
		if (plr[myplr]._pClass == PC_WARRIOR) {
			*mind = 2 * v26;
			v27 = myplr;
		}
		if (plr[v27]._pClass == PC_ROGUE)
			*mind += *mind >> 1;
		v8 = maxd;
		v28 = 0;
		*maxd = 10;
		if (plr[myplr]._pLevel > 0) {
			do {
				*maxd += 4;
				++v28;
			} while (v28 < plr[myplr]._pLevel);
		}
	LABEL_65:
		if (v4 > 0)
			*v8 += 6 * v4;
		if (plr[myplr]._pClass == PC_WARRIOR)
			*v8 *= 2;
		if (plr[myplr]._pClass == PC_ROGUE)
			*v8 += *v8 >> 1;
	LABEL_71:
		*mind = -1;
		*v8 = -1;
		break;
	case SPL_FLARE:
		v5 = 3 * v4 + (plr[v3]._pMagic >> 1) - (plr[v3]._pMagic >> 3);
		*mind = v5;
	LABEL_73:
		v16 = maxd;
	LABEL_74:
		*v16 = v5;
		break;
	}
}

BOOL __fastcall CheckBlock(int fx, int fy, int tx, int ty)
{
	int pn;
	BOOL coll;

	coll = FALSE;
	while (fx != tx || fy != ty) {
		pn = GetDirection(fx, fy, tx, ty);
		fx += XDirAdd[pn];
		fy += YDirAdd[pn];
		if (nSolidTable[dPiece[fx][fy]])
			coll = TRUE;
	}

	return coll;
}

int __fastcall FindClosest(int sx, int sy, int rad)
{
	int j, i, mid, tx, ty, cr;
	int CrawlNum[19] = { 0, 3, 12, 45, 94, 159, 240, 337, 450, 579, 724, 885, 1062, 1255, 1464, 1689, 1930, 2187, 2460 };

	if (rad > 19)
		rad = 19;

	for (i = 1; i < rad; i++) {
		cr = CrawlNum[i] + 2;
		for (j = (unsigned char)CrawlTable[CrawlNum[i]]; j > 0; j--) {
			tx = sx + CrawlTable[cr - 1];
			ty = sy + CrawlTable[cr];
			if (tx > 0 && tx < MAXDUNX && ty > 0 && ty < MAXDUNY) {
				mid = dMonster[tx][ty];
				if (mid > 0 && !CheckBlock(sx, sy, tx, ty))
					return mid - 1;
			}
			cr += 2;
		}
	}
	return -1;
}

int __fastcall GetSpellLevel(int id, int sn)
{
	int result;

	if (id == myplr)
		result = plr[id]._pISplLvlAdd + plr[id]._pSplLvl[sn];
	else
		result = 1;

	if (result < 0)
		result = 0;

	return result;
}

int __fastcall GetDirection8(int x1, int y1, int x2, int y2)
{
	unsigned char Dirs[16][16] = {
		{ 99, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 2, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 2, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 },
		{ 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0 },
		{ 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
		{ 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
	};
	unsigned char trans[4][3] = { { 3, 4, 5 },
		{ 3, 2, 1 },
		{ 7, 0, 1 },
		{ 7, 6, 5 } };
	int mx, my, md;

	mx = abs(x2 - x1);
	if (mx > 15)
		mx = 15;
	my = abs(y2 - y1);
	if (my > 15)
		my = 15;
	md = Dirs[my][mx];
	if (x1 > x2) {
		if (y1 > y2)
			md = trans[0][md];
		else
			md = trans[1][md];
	} else if (y1 > y2)
		md = trans[3][md];
	else
		md = trans[2][md];
	return md;
}

int __fastcall GetDirection16(int x1, int y1, int x2, int y2)
{
	BYTE Dirs[16][16] = {
		{ 99, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 4, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 4, 3, 2, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 4, 3, 3, 2, 2, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0 },
		{ 4, 4, 3, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 4, 4, 3, 3, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 4, 4, 3, 3, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1 },
		{ 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1 },
		{ 4, 4, 4, 3, 3, 3, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1 },
		{ 4, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 2, 1, 1, 1, 1 },
		{ 4, 4, 4, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 1, 1 },
		{ 4, 4, 4, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 1 },
		{ 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2 },
		{ 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2 },
		{ 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2 },
		{ 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2 }
	};
	BYTE urtoll[5] = { 6, 7, 8, 9, 10 };
	BYTE ultolr[5] = { 6, 5, 4, 3, 2 };
	BYTE lltour[5] = { 14, 13, 12, 11, 10 };
	BYTE lrtoul[5] = { 14, 15, 0, 1, 2 };
	int mx, my, md;

	mx = abs(x2 - x1);
	if (mx > 15)
		mx = 15;
	my = abs(y2 - y1);
	if (my > 15)
		my = 15;
	md = Dirs[my][mx];
	if (x1 > x2) {
		if (y1 > y2)
			md = urtoll[md];
		else
			md = ultolr[md];
	} else if (y1 > y2) {
		md = lltour[md];
	} else {
		md = lrtoul[md];
	}
	return md;
}

void __fastcall DeleteMissile(int mi, int i)
{
	int src;

	if (missile[mi]._mitype == MIS_MANASHIELD) {
		src = missile[mi]._misource;
		if (src == myplr)
			NetSendCmd(TRUE, CMD_REMSHIELD);
		plr[src].pManaShield = FALSE;
	}
	missileavail[MAXMISSILES - nummissiles] = mi;
	nummissiles--;
	if (nummissiles > 0 && i != nummissiles)
		missileactive[i] = missileactive[nummissiles];
}

void __fastcall GetMissileVel(int i, int sx, int sy, int dx, int dy, int v)
{
	double dxp, dyp, dr;

	if (dx != sx || dy != sy) {
		dxp = (dx + sy - sx - dy) << 21;
		dyp = (dy + dx - sx - sy) << 21;
		dr = sqrt(dxp * dxp + dyp * dyp);
		missile[i]._mixvel = (dxp * (v << 16)) / dr;
		missile[i]._miyvel = (dyp * (v << 15)) / dr;
	} else {
		missile[i]._mixvel = 0;
		missile[i]._miyvel = 0;
	}
}

void __fastcall PutMissile(int i)
{
	int x, y;

	x = missile[i]._mix;
	y = missile[i]._miy;
	if (x <= 0 || y <= 0 || x >= MAXDUNX || y >= MAXDUNY)
		missile[i]._miDelFlag = TRUE;
	if (!missile[i]._miDelFlag) {
		dFlags[x][y] |= DFLAG_MISSILE;
		if (dMissile[x][y] == 0)
			dMissile[x][y] = i + 1;
		else
			dMissile[x][y] = -1;
		if (missile[i]._miPreFlag)
			MissilePreFlag = 1;
	}
}
// 64CCD4: using guessed type int MissilePreFlag;

void __fastcall GetMissilePos(int i)
{
	int mx, my, dx, dy, lx, ly;

	mx = missile[i]._mitxoff >> 16;
	my = missile[i]._mityoff >> 16;
	dx = mx + 2 * my;
	dy = 2 * my - mx;
	if (dx < 0) {
		lx = -(-dx >> 3);
		dx = -(-dx >> 6);
	} else {
		lx = dx >> 3;
		dx = dx >> 6;
	}
	if (dy < 0) {
		ly = -(-dy >> 3);
		dy = -(-dy >> 6);
	} else {
		ly = dy >> 3;
		dy = dy >> 6;
	}
	missile[i]._mix = dx + missile[i]._misx;
	missile[i]._miy = dy + missile[i]._misy;
	missile[i]._mixoff = mx + (dy << 5) - (dx << 5);
	missile[i]._miyoff = my - (dx << 4) - (dy << 4);
	ChangeLightOff(missile[i]._mlid, lx - (dx << 3), ly - (dy << 3));
}

void __fastcall MoveMissilePos(int i)
{
	int dx, dy;

	switch (missile[i]._mimfnum) {
	case DIR_S:
		dx = 1;
		dy = 1;
		break;
	case DIR_SW:
		dx = 1;
		dy = 1;
		break;
	case DIR_W:
		dx = 0;
		dy = 1;
		break;
	case DIR_NW:
		dx = 0;
		dy = 0;
		break;
	case DIR_N:
		dx = 0;
		dy = 0;
		break;
	case DIR_NE:
		dx = 0;
		dy = 0;
		break;
	case DIR_E:
		dx = 1;
		dy = 0;
		break;
	case DIR_SE:
		dx = 1;
		dy = 1;
		break;
	}
	if (PosOkMonst(missile[i]._misource, missile[i]._mix + dx, missile[i]._miy + dy)) {
		missile[i]._mix += dx;
		missile[i]._miy += dy;
		missile[i]._mixoff += (dy << 5) - (dx << 5);
		missile[i]._miyoff -= (dy << 4) + (dx << 4);
	}
}

BOOL __fastcall MonsterTrapHit(int m, int mindam, int maxdam, int dist, int t, int shift)
{
	int v6;  // esi
	int v8;  // ecx
	int v9;  // eax
	int v10; // edi
	//int v11; // eax
	int v13;            // eax
	int v14;            // [esp+Ch] [ebp-10h]
	int v15;            // [esp+10h] [ebp-Ch]
	signed int v16;     // [esp+14h] [ebp-8h]
	signed int arglist; // [esp+18h] [ebp-4h]
	BOOL ret;

	v16 = 0;
	arglist = m;
	v6 = m;
	v15 = mindam;
	if (monster[m].mtalkmsg
	    || monster[v6]._mhitpoints >> 6 <= 0
	    || monster[v6].MType->mtype == MT_ILLWEAV && _LOBYTE(monster[v6]._mgoal) == MGOAL_RETREAT) {
		return 0;
	}
	if (monster[v6]._mmode == MM_CHARGE)
		return 0;
	v8 = _LOWORD(monster[v6].mMagicRes);
	v9 = missiledata[t].mResist;
	if (v8 & IMUNE_MAGIC) {
		if (v9 == MISR_MAGIC)
			return 0;
	}
	if (v8 & IMUNE_FIRE && v9 == MISR_FIRE || v8 & IMUNE_LIGHTNING && v9 == MISR_LIGHTNING)
		return 0;
	if (v8 & RESIST_MAGIC && v9 == MISR_MAGIC || v8 & 2 && v9 == MISR_FIRE || v8 & RESIST_LIGHTNING && v9 == MISR_LIGHTNING)
		v16 = 1;
	v14 = random(68, 100);
	v10 = 90 - (unsigned char)monster[v6].mArmorClass - dist;
	if (v10 < 5)
		v10 = 5;
	if (v10 > 95)
		v10 = 95;
	//_LOBYTE(v11) = CheckMonsterHit(arglist, (unsigned char *)&t);
	if (CheckMonsterHit(arglist, &ret))
		return ret;
#ifdef _DEBUG
	if (v14 >= v10 && !debug_mode_dollar_sign && !debug_mode_key_inverted_v && monster[v6]._mmode != MM_STONE)
		return 0;
#else
	if (v14 >= v10 && monster[v6]._mmode != MM_STONE)
		return 0;
#endif
	v13 = v15 + random(68, maxdam - v15 + 1);
	if (!(_BYTE)shift)
		v13 <<= 6;
	if (v16)
		monster[v6]._mhitpoints -= v13 >> 2;
	else
		monster[v6]._mhitpoints -= v13;
#ifdef _DEBUG
	if (debug_mode_dollar_sign || debug_mode_key_inverted_v)
		monster[v6]._mhitpoints = 0;
#endif
	if (monster[v6]._mhitpoints >> 6 > 0) {
		if (v16) {
			PlayEffect(arglist, 1);
			return 1;
		}
		if (monster[v6]._mmode != MM_STONE) {
			if (arglist > 3)
				M_StartHit(arglist, -1, v13);
			return 1;
		}
		if (arglist > 3)
			M_StartHit(arglist, -1, v13);
	} else {
		if (monster[v6]._mmode != MM_STONE) {
			M_StartKill(arglist, -1);
			return 1;
		}
		M_StartKill(arglist, -1);
	}
	monster[v6]._mmode = MM_STONE;
	return 1;
}

BOOLEAN __fastcall MonsterMHit(int pnum, int m, int mindam, int maxdam, int dist, int t, int shift)
{
	int v7;     // edi
	BOOLEAN v8; // zf
	short v9;   // ax
	int v10;    // ecx
	int v11;    // eax
	int v12;    // esi
	int v13;    // ebx
	int v15;    // eax
	//int v16; // eax
	int v19;              // ebx
	int v20;              // ebx
	int v21;              // edx
	int v22;              // eax
	int v23;              // [esp+Ch] [ebp-18h]
	BOOL ret;             // [esp+10h] [ebp-14h]
	int v25;              // [esp+14h] [ebp-10h]
	int v26;              // [esp+18h] [ebp-Ch]
	int pnuma;            // [esp+1Ch] [ebp-8h]
	int arglist;          // [esp+20h] [ebp-4h]
	unsigned char dist_3; // [esp+37h] [ebp+13h]

	arglist = m;
	v7 = m;
	v26 = 0;
	v8 = monster[m].mtalkmsg == 0;
	pnuma = pnum;
	if (!v8
	    || monster[v7]._mhitpoints >> 6 <= 0
	    || t == MIS_HBOLT && monster[v7].MType->mtype != MT_DIABLO && monster[v7].MData->mMonstClass) {
		return 0;
	}
	if (monster[v7].MType->mtype == MT_ILLWEAV && _LOBYTE(monster[v7]._mgoal) == MGOAL_RETREAT)
		return 0;
	if (monster[v7]._mmode == MM_CHARGE)
		return 0;
	v9 = monster[v7].mMagicRes;
	v10 = missiledata[t].mResist;
	v23 = t;
	if (v9 & IMUNE_MAGIC) {
		if (v10 == MISR_MAGIC)
			return 0;
	}
	if (v9 & IMUNE_FIRE && v10 == MISR_FIRE || v9 & IMUNE_LIGHTNING && v10 == MISR_LIGHTNING || (v9 & IMUNE_ACID) != 0 && v10 == MISR_ACID)
		return 0;
	if (v9 & RESIST_MAGIC && v10 == MISR_MAGIC || v9 & RESIST_FIRE && v10 == MISR_FIRE || v9 & RESIST_LIGHTNING && v10 == MISR_LIGHTNING)
		v26 = 1;
	v11 = random(69, 100);
	v8 = missiledata[t].mType == 0;
	v25 = v11;
	if (v8) {
		v12 = pnuma;
		v13 = plr[v12]._pDexterity
		    + plr[v12]._pIBonusToHit
		    + plr[v12]._pLevel
		    - (unsigned char)monster[v7].mArmorClass
		    - (dist * dist >> 1)
		    + plr[pnuma]._pIEnAc
		    + 50;
		if (plr[pnuma]._pClass == PC_ROGUE)
			v13 = plr[v12]._pDexterity
			    + plr[v12]._pIBonusToHit
			    + plr[v12]._pLevel
			    - (unsigned char)monster[v7].mArmorClass
			    - (dist * dist >> 1)
			    + plr[pnuma]._pIEnAc
			    + 70;
		if (plr[pnuma]._pClass == PC_WARRIOR)
			v13 += 10;
	} else {
		v12 = pnuma;
		v15 = 2 * SLOBYTE(monster[v7].mLevel);
		v13 = plr[pnuma]._pMagic - v15 - dist + 50;
		if (plr[pnuma]._pClass == PC_SORCERER)
			v13 = plr[v12]._pMagic - v15 - dist + 70;
	}
	if (v13 < 5)
		v13 = 5;
	if (v13 > 95)
		v13 = 95;
	if (monster[v7]._mmode == MM_STONE)
		v25 = 0;
	if (CheckMonsterHit(arglist, &ret))
		return ret;
#ifdef _DEBUG
	if (v25 >= v13 && !debug_mode_key_inverted_v && !debug_mode_dollar_sign)
		return 0;
#else
	if (v25 >= v13)
		return 0;
#endif
	if (t == MIS_BONESPIRIT) {
		v19 = monster[v7]._mhitpoints / 3 >> 6;
	} else {
		v19 = mindam + random(70, maxdam - mindam + 1);
	}
	dist_3 = missiledata[v23].mType;
	if (!missiledata[v23].mType) {
		v20 = plr[v12]._pIBonusDamMod + v19 * plr[v12]._pIBonusDam / 100 + v19;
		if (plr[v12]._pClass == PC_ROGUE)
			v19 = plr[v12]._pDamageMod + v20;
		else
			v19 = (plr[v12]._pDamageMod >> 1) + v20;
	}
	if (!(_BYTE)shift)
		v19 <<= 6;
	if (v26)
		v19 >>= 2;
	v21 = pnuma;
	if (pnuma == myplr)
		monster[v7]._mhitpoints -= v19;
	v22 = plr[v12]._pIFlags;
	if (v22 & 8)
		monster[v7]._mFlags |= MFLAG_NOHEAL;
	if (monster[v7]._mhitpoints >> 6 > 0) {
		if (v26) {
			PlayEffect(arglist, 1);
		} else if (monster[v7]._mmode == MM_STONE) {
			if (arglist > 3)
				M_StartHit(arglist, v21, v19);
			monster[v7]._mmode = MM_STONE;
		} else {
			if (!dist_3 && v22 & 0x800) {
				M_GetKnockback(arglist);
				v21 = pnuma;
			}
			if (arglist > 3)
				M_StartHit(arglist, v21, v19);
		}
	} else if (monster[v7]._mmode == MM_STONE) {
		M_StartKill(arglist, v21);
		monster[v7]._mmode = MM_STONE;
	} else {
		M_StartKill(arglist, v21);
	}
	if (!monster[v7]._msquelch) {
		monster[v7]._msquelch = -1;
		monster[v7]._lastx = plr[v12].WorldX;
		monster[v7]._lasty = plr[v12].WorldY;
	}
	return 1;
}

BOOLEAN __fastcall PlayerMHit(int pnum, int m, int dist, int mind, int maxd, int mtype, int shift, int earflag)
{
	int v8;            // ebx
	int v9;            // esi
	int v10;           // edi
	int v11;           // ecx
	int v12;           // eax
	int v13;           // edi
	int v14;           // edi
	int v15;           // eax
	int v16;           // eax
	int v17;           // ebx
	int v18;           // ebx
	unsigned char v19; // al
	int v20;           // eax
	int v21;           // ecx
	int v22;           // ecx
	int v23;           // ecx
	int v24;           // edi
	int v25;           // ecx
	int v26;           // eax
	int v29;           // eax
	int v30;           // eax
	int v32;           // [esp+Ch] [ebp-14h]
	int arglist;       // [esp+14h] [ebp-Ch]
	int v34;           // [esp+18h] [ebp-8h]
	int v35;           // [esp+1Ch] [ebp-4h]
	int dista;         // [esp+28h] [ebp+8h]

	v8 = m;
	arglist = pnum;
	v9 = pnum;
	v34 = m;
	if (plr[pnum]._pHitPoints >> 6 <= 0
	    || plr[v9]._pInvincible
	    || plr[v9]._pSpellFlags & 1 && !missiledata[mtype].mType) {
		return 0;
	}
	v10 = 100;
	v32 = random(72, 100);
#ifdef _DEBUG
	if (debug_mode_dollar_sign || debug_mode_key_inverted_v)
		v32 = 1000;
#endif
	if (!missiledata[mtype].mType) {
		v11 = 5;
		v12 = plr[v9]._pIAC + plr[v9]._pIBonusAC + plr[v9]._pDexterity / 5;
		if (v8 != -1) {
			v11 = 2 * dist;
			v13 = (unsigned char)monster[v8].mHit
			    + 2 * (SLOBYTE(monster[v8].mLevel) - plr[v9]._pLevel)
			    + 30
			    - 2 * dist;
		LABEL_8:
			v14 = v13 - v12;
			goto LABEL_14;
		}
		v15 = v12 >> 1;
	LABEL_12:
		v13 = v10 - v15;
		v12 = 2 * dist;
		goto LABEL_8;
	}
	if (v8 != -1) {
		v10 = 2 * SLOBYTE(monster[v8].mLevel) + 40;
		v15 = 2 * plr[v9]._pLevel;
		goto LABEL_12;
	}
	v14 = 40;
LABEL_14:
	if (v14 < 10)
		v14 = 10;
	if (currlevel == 14) {
		if (v14 >= 20)
			goto LABEL_25;
		v14 = 20;
	}
	if (currlevel == 15) {
		if (v14 >= 25)
			goto LABEL_25;
		v14 = 25;
	}
	if (currlevel == 16 && v14 < 30)
		v14 = 30;
LABEL_25:
	v16 = plr[v9]._pmode;
	if (v16 && v16 != 4 || !plr[v9]._pBlockFlag) {
		v35 = 100;
	} else {
		v35 = random(73, 100);
	}
	if ((_BYTE)shift == 1)
		v35 = 100;
	if (mtype == MIS_ACIDPUD)
		v35 = 100;
	if (v8 == -1)
		v17 = plr[v9]._pBaseToBlk;
	else
		v17 = plr[v9]._pBaseToBlk + 2 * plr[v9]._pLevel - 2 * SLOBYTE(monster[v8].mLevel);
	v18 = plr[v9]._pDexterity + v17;
	if (v18 < 0)
		v18 = 0;
	if (v18 > 100)
		v18 = 100;
	v19 = missiledata[mtype].mResist;
	if (v19 == MISR_FIRE) {
		v20 = plr[v9]._pFireResist;
	} else if (v19 == MISR_LIGHTNING) {
		v20 = plr[v9]._pLghtResist;
	} else {
		if (v19 <= MISR_LIGHTNING || v19 > MISR_ACID) {
			dista = 0;
			goto LABEL_50;
		}
		v20 = plr[v9]._pMagResist;
	}
	dista = v20;
LABEL_50:
	if (v32 < v14) {
		if (mtype == MIS_BONESPIRIT) {
			v21 = plr[v9]._pHitPoints / 3;
		} else {
			if ((_BYTE)shift) {
				v23 = mind + random(75, maxd - mind + 1);
				if (v34 == -1 && plr[v9]._pIFlags & ISPL_ABSHALFTRAP)
					v23 >>= 1;
				v21 = plr[v9]._pIGetHit + v23;
			} else {
				v22 = (mind << 6) + random(75, (maxd - mind + 1) << 6);
				if (v34 == -1 && plr[v9]._pIFlags & ISPL_ABSHALFTRAP)
					v22 >>= 1;
				v21 = (plr[v9]._pIGetHit << 6) + v22;
			}
			if (v21 < 64)
				v21 = 64;
		}
		if (dista <= 0) {
			if (v35 < v18) {
				if (v34 == -1)
					v29 = plr[v9]._pdir;
				else
					v29 = GetDirection(plr[v9].WorldX, plr[v9].WorldY, monster[v34]._mx, monster[v34]._my);
				StartPlrBlock(arglist, v29);
				return 1;
			}
			v24 = arglist;
			if (arglist == myplr) {
				plr[v9]._pHitPoints -= v21;
				plr[v9]._pHPBase -= v21;
			}
			v30 = plr[v9]._pMaxHP;
			if (plr[v9]._pHitPoints > v30) {
				plr[v9]._pHitPoints = v30;
				plr[v9]._pHPBase = plr[v9]._pMaxHPBase;
			}
			if (plr[v9]._pHitPoints >> 6 > 0) {
				StartPlrHit(arglist, v21, 0);
				return 1;
			}
			goto LABEL_70;
		}
		v24 = arglist;
		v25 = dista * v21 / -100 + v21;
		if (arglist == myplr) {
			plr[v9]._pHitPoints -= v25;
			plr[v9]._pHPBase -= v25;
		}
		v26 = plr[v9]._pMaxHP;
		if (plr[v9]._pHitPoints > v26) {
			plr[v9]._pHitPoints = v26;
			plr[v9]._pHPBase = plr[v9]._pMaxHPBase;
		}
		if (plr[v9]._pHitPoints >> 6 <= 0) {
		LABEL_70:
			SyncPlrKill(v24, earflag);
			return 1;
		}

		if (plr[v9]._pClass == PC_WARRIOR) {
			PlaySfxLoc(PS_WARR69, plr[v9].WorldX, plr[v9].WorldY);
		} else if (plr[v9]._pClass == PC_ROGUE) {
			PlaySfxLoc(PS_ROGUE69, plr[v9].WorldX, plr[v9].WorldY);
		} else if (plr[v9]._pClass == PC_SORCERER) {
			PlaySfxLoc(PS_MAGE69, plr[v9].WorldX, plr[v9].WorldY);
		}

		drawhpflag = TRUE;
		return 1;
	}
	return 0;
}

BOOLEAN __fastcall Plr2PlrMHit(int pnum, int p, int mindam, int maxdam, int dist, int mtype, int shift)
{
	int v7;           // edi
	unsigned char v8; // al
	int v9;           // eax
	int v10;          // esi
	int v11;          // eax
	int v12;          // ecx
	int v13;          // eax
	int v14;          // ecx
	BOOLEAN v15;      // sf
	int v16;          // ecx
	int v17;          // ebx
	int v20;          // eax
	int v22;          // [esp+Ch] [ebp-14h]
	int v23;          // [esp+10h] [ebp-10h]
	int v24;          // [esp+10h] [ebp-10h]
	int arglist;      // [esp+14h] [ebp-Ch]
	int v26;          // [esp+18h] [ebp-8h]
	int v27;          // [esp+1Ch] [ebp-4h]
	int dista;        // [esp+30h] [ebp+10h]

	arglist = p;
	v7 = p;
	v26 = pnum;
	if (plr[p]._pInvincible || mtype == MIS_HBOLT || plr[v7]._pSpellFlags & 1 && !missiledata[mtype].mType)
		return 0;
	v22 = mtype;
	v8 = missiledata[mtype].mResist;
	if (v8 == MISR_FIRE) {
		v9 = plr[v7]._pFireResist;
	} else if (v8 == MISR_LIGHTNING) {
		v9 = plr[v7]._pLghtResist;
	} else {
		if (v8 <= MISR_LIGHTNING || v8 > MISR_ACID) {
			v27 = 0;
			goto LABEL_14;
		}
		v9 = plr[v7]._pMagResist;
	}
	v27 = v9;
LABEL_14:
	v23 = random(69, 100);
	if (missiledata[mtype].mType) {
		v10 = v26;
		v12 = 2 * plr[v7]._pLevel;
		v11 = plr[v26]._pMagic - v12 - dist + 50;
		if (plr[v26]._pClass == PC_SORCERER)
			v11 = plr[v10]._pMagic - v12 - dist + 70;
	} else {
		v10 = v26;
		v12 = plr[v10]._pIBonusToHit
		    + plr[v10]._pLevel
		    - (dist * dist >> 1)
		    - plr[v7]._pDexterity / 5
		    - plr[v7]._pIBonusAC
		    - plr[v7]._pIAC;
		v11 = v12 + plr[v26]._pDexterity + 50;
		if (plr[v26]._pClass == PC_ROGUE)
			v11 += 20;
		if (plr[v26]._pClass == PC_WARRIOR)
			v11 += 10;
	}
	if (v11 < 5)
		v11 = 5;
	if (v11 > 95)
		v11 = 95;
	if (v23 < v11) {
		v13 = plr[v7]._pmode;
		if (v13 && v13 != 4 || !plr[v7]._pBlockFlag) {
			v24 = 100;
		} else {
			v24 = random(73, 100);
		}
		if ((_BYTE)shift == 1)
			v24 = 100;
		v14 = plr[v7]._pBaseToBlk + 2 * plr[v7]._pLevel - 2 * plr[v10]._pLevel;
		v15 = plr[v7]._pDexterity + v14 < 0;
		v16 = plr[v7]._pDexterity + v14;
		dista = v16;
		if (v15) {
			dista = 0;
			v16 = 0;
		}
		if (v16 > 100) {
			dista = 100;
			v16 = 100;
		}
		if (mtype == MIS_BONESPIRIT) {
			v17 = plr[v7]._pHitPoints / 3;
		} else {
			v17 = mindam + random(70, maxdam - mindam + 1);
			if (!missiledata[v22].mType)
				v17 += plr[v10]._pIBonusDamMod + plr[v10]._pDamageMod + v17 * plr[v10]._pIBonusDam / 100;
			v16 = dista;
			if (!(_BYTE)shift)
				v17 <<= 6;
		}
		if (missiledata[v22].mType)
			v17 >>= 1;
		if (v27 <= 0) {
			if (v24 >= v16) {
				if (v26 == myplr)
					NetSendCmdDamage(TRUE, arglist, v17);
				StartPlrHit(arglist, v17, 0);
			} else {
				v20 = GetDirection(plr[v7].WorldX, plr[v7].WorldY, plr[v10].WorldX, plr[v10].WorldY);
				StartPlrBlock(arglist, v20);
			}
			return 1;
		}
		if (v26 == myplr)
			NetSendCmdDamage(TRUE, arglist, v17 - v27 * v17 / 100);
		if (plr[v10]._pClass == PC_WARRIOR) {
			PlaySfxLoc(PS_WARR69, plr[v10].WorldX, plr[v10].WorldY);
		} else if (plr[v10]._pClass == PC_ROGUE) {
			PlaySfxLoc(PS_ROGUE69, plr[v10].WorldX, plr[v10].WorldY);
		} else if (plr[v10]._pClass == PC_SORCERER) {
			PlaySfxLoc(PS_MAGE69, plr[v10].WorldX, plr[v10].WorldY);
		}
		return 1;
	}
	return 0;
}

void __fastcall CheckMissileCol(int i, int mindam, int maxdam, BOOLEAN shift, int mx, int my, int nodel)
{
	int v7;      // ebx
	int v8;      // esi
	char v9;     // dl
	int v10;     // ecx
	int v11;     // edi
	int v12;     // eax
	BOOLEAN v13; // eax
	char v14;    // al
	int v15;     // ecx
	int v16;     // edx
	BOOLEAN v17; // eax
	int v18;     // eax
	BOOLEAN v19; // eax
	char v20;    // al
	int v21;     // eax
	int v22;     // eax
	char v23;    // al
	char v24;    // al
	int v25;     // edx
	int v26;     // ecx
	int v27;     // [esp-Ch] [ebp-1Ch]
	int v28;     // [esp-8h] [ebp-18h]
	int mindama; // [esp+Ch] [ebp-4h]

	v7 = mindam;
	v8 = i;
	mindama = mindam;
	v9 = missile[i]._miAnimType;
	if (v9 == 4 || (v10 = missile[v8]._misource, v10 == -1)) {
		v11 = 112 * mx + my;
		v21 = dMonster[0][v11];
		if (v21 > 0) {
			v28 = missile[v8]._mitype;
			v27 = missile[v8]._midist;
			v22 = v9 == 4 ? MonsterMHit(missile[v8]._misource, v21 - 1, v7, maxdam, v27, v28, shift) : MonsterTrapHit(v21 - 1, v7, maxdam, v27, v28, shift);
			if (v22) {
				if (!(_BYTE)nodel)
					missile[v8]._mirange = 0;
				missile[v8]._miHitFlag = TRUE;
			}
		}
		v23 = dPlayer[0][v11];
		if (v23 > 0) {
			v17 = PlayerMHit(
			    v23 - 1,
			    -1,
			    missile[v8]._midist,
			    v7,
			    maxdam,
			    missile[v8]._mitype,
			    shift,
			    _LOBYTE(missile[v8]._miAnimType) == MFILE_FIREWAL);
		LABEL_35:
			if (v17) {
				if (!(_BYTE)nodel)
					missile[v8]._mirange = 0;
				missile[v8]._miHitFlag = TRUE;
			}
			goto LABEL_39;
		}
	} else {
		if (!missile[v8]._micaster) {
			v11 = 112 * mx + my;
			v12 = dMonster[0][v11];
			if (v12 <= 0) {
				if (v12 >= 0 || monster[-(v12 + 1)]._mmode != MM_STONE) {
				LABEL_13:
					v14 = dPlayer[0][v11];
					if (v14 <= 0)
						goto LABEL_39;
					v15 = missile[v8]._misource;
					v16 = v14 - 1;
					if (v16 == v15)
						goto LABEL_39;
					v17 = Plr2PlrMHit(
					    v15,
					    v16,
					    mindama,
					    maxdam,
					    missile[v8]._midist,
					    missile[v8]._mitype,
					    shift);
					goto LABEL_35;
				}
				v13 = MonsterMHit(
				    v10,
				    -1 - v12,
				    mindama,
				    maxdam,
				    missile[v8]._midist,
				    missile[v8]._mitype,
				    shift);
			} else {
				v13 = MonsterMHit(v10, v12 - 1, v7, maxdam, missile[v8]._midist, missile[v8]._mitype, shift);
			}
			if (v13) {
				if (!(_BYTE)nodel)
					missile[v8]._mirange = 0;
				missile[v8]._miHitFlag = TRUE;
			}
			goto LABEL_13;
		}
		if (monster[v10]._mFlags & MFLAG_TARGETS_MONSTER) {
			v18 = dMonster[mx][my];
			if (v18 > 0) {
				if (monster[v18 - 1]._mFlags & MFLAG_GOLEM) /* fix */
				{
					v19 = MonsterTrapHit(
					    v18 - 1,
					    mindama,
					    maxdam,
					    missile[v8]._midist,
					    missile[v8]._mitype,
					    shift);
					if (v19) {
						if (!(_BYTE)nodel)
							missile[v8]._mirange = 0;
						missile[v8]._miHitFlag = TRUE;
					}
				}
			}
		}
		v11 = my + 112 * mx;
		v20 = dPlayer[0][v11];
		if (v20 > 0) {
			v17 = PlayerMHit(
			    v20 - 1,
			    missile[v8]._misource,
			    missile[v8]._midist,
			    mindama,
			    maxdam,
			    missile[v8]._mitype,
			    shift,
			    0);
			goto LABEL_35;
		}
	}
LABEL_39:
	v24 = dObject[0][v11];
	if (v24) {
		v25 = v24 <= 0 ? -1 - v24 : v24 - 1;
		if (!object[v25]._oMissFlag) {
			if (object[v25]._oBreak == 1)
				BreakObject(-1, v25);
			if (!(_BYTE)nodel)
				missile[v8]._mirange = 0;
			missile[v8]._miHitFlag = FALSE;
		}
	}
	if (nMissileTable[dPiece[0][v11]]) {
		if (!(_BYTE)nodel)
			missile[v8]._mirange = 0;
		missile[v8]._miHitFlag = FALSE;
	}
	if (!missile[v8]._mirange) {
		v26 = missiledata[missile[v8]._mitype].miSFX;
		if (v26 != -1)
			PlaySfxLoc(v26, missile[v8]._mix, missile[v8]._miy);
	}
}

void __fastcall SetMissAnim(int mi, int animtype)
{
	int dir = missile[mi]._mimfnum;

	missile[mi]._miAnimType = animtype;
	missile[mi]._miAnimFlags = misfiledata[animtype].mFlags;
	missile[mi]._miAnimData = misfiledata[animtype].mAnimData[dir];
	missile[mi]._miAnimDelay = misfiledata[animtype].mAnimDelay[dir];
	missile[mi]._miAnimLen = misfiledata[animtype].mAnimLen[dir];
	missile[mi]._miAnimWidth = misfiledata[animtype].mAnimWidth[dir];
	missile[mi]._miAnimWidth2 = misfiledata[animtype].mAnimWidth2[dir];
	missile[mi]._miAnimCnt = 0;
	missile[mi]._miAnimFrame = 1;
}

void __fastcall SetMissDir(int mi, int dir)
{
	missile[mi]._mimfnum = dir;
	SetMissAnim(mi, missile[mi]._miAnimType);
}

void __fastcall LoadMissileGFX(BYTE mi)
{
	char pszName[256];
	int i;
	BYTE *file;
	MisFileData *mfd;

	mfd = &misfiledata[mi];
	if (mfd->mFlags & MFLAG_ALLOW_SPECIAL) {
		sprintf(pszName, "Missiles\\%s.CL2", mfd->mName);
		file = LoadFileInMem(pszName, 0);
		for (i = 0; i < mfd->mAnimFAmt; i++)
			mfd->mAnimData[i] = &file[((int *)file)[i]];
	} else if (mfd->mAnimFAmt == 1) {
		sprintf(pszName, "Missiles\\%s.CL2", mfd->mName);
		if (!mfd->mAnimData[0])
			mfd->mAnimData[0] = LoadFileInMem(pszName, 0);
	} else {
		for (i = 0; i < mfd->mAnimFAmt; i++) {
			sprintf(pszName, "Missiles\\%s%i.CL2", mfd->mName, i + 1);
			if (!mfd->mAnimData[i]) {
				file = LoadFileInMem(pszName, 0);
				mfd->mAnimData[i] = file;
			}
		}
	}
}

void __cdecl InitMissileGFX()
{
	int mi;

	for (mi = 0; misfiledata[mi].mAnimFAmt; mi++) {
		if (!(misfiledata[mi].mFlags & MFLAG_HIDDEN))
			LoadMissileGFX(mi);
	}
}

void __fastcall FreeMissileGFX(int mi)
{
	int i;
	DWORD *pFrameTable;

	if (misfiledata[mi].mFlags & MFLAG_ALLOW_SPECIAL) {
		if (misfiledata[mi].mAnimData[0]) {
			pFrameTable = (DWORD *)misfiledata[mi].mAnimData[0];
			mem_free_dbg(&pFrameTable[-misfiledata[mi].mAnimFAmt]); // TODO find a cleaner way to access the offeset
			misfiledata[mi].mAnimData[0] = NULL;
		}
		return;
	}

	for (i = 0; i < misfiledata[mi].mAnimFAmt; i++) {
		if (misfiledata[mi].mAnimData[i]) {
			pFrameTable = (DWORD *)misfiledata[mi].mAnimData[i];
			misfiledata[mi].mAnimData[i] = NULL;
			mem_free_dbg(pFrameTable);
		}
	}
}

void __cdecl FreeMissiles()
{
	int mi;

	for (mi = 0; misfiledata[mi].mAnimFAmt; mi++) {
		if (!(misfiledata[mi].mFlags & MFLAG_HIDDEN))
			FreeMissileGFX(mi);
	}
}

void __cdecl FreeMissiles2()
{
	int mi;

	for (mi = 0; misfiledata[mi].mAnimFAmt; mi++) {
		if (misfiledata[mi].mFlags & MFLAG_HIDDEN)
			FreeMissileGFX(mi);
	}
}

void __cdecl InitMissiles()
{
	int mi, src, i, j;

	plr[myplr]._pSpellFlags &= ~0x1;
	if (plr[myplr]._pInfraFlag == TRUE) {
		for (i = 0; i < nummissiles; ++i) {
			mi = missileactive[i];
			if (missile[mi]._mitype == MIS_INFRA) {
				src = missile[mi]._misource;
				if (src == myplr)
					CalcPlrItemVals(src, 1);
			}
		}
	}
	nummissiles = 0;
	for (i = 0; i < MAXMISSILES; i++) {
		missileavail[i] = i;
		missileactive[i] = 0;
	}
	numchains = 0;
	for (i = 0; i < MAXMISSILES; i++) {
		chain[i].idx = -1;
		chain[i]._mitype = 0;
		chain[i]._mirange = 0;
	}
	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++) {
			dFlags[i][j] &= ~DFLAG_MISSILE;
		}
	}
}
// 64CCD8: using guessed type int numchains;

void __fastcall AddLArrow(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	if (sx == dx && sy == dy) {
		dx += XDirAdd[midir];
		dy += YDirAdd[midir];
	}
	if (!mienemy) {
		if (plr[id]._pClass == PC_ROGUE)
			GetMissileVel(mi, sx, sy, dx, dy, (plr[id]._pLevel >> 2) + 31);
		else if (plr[id]._pClass == PC_WARRIOR)
			GetMissileVel(mi, sx, sy, dx, dy, (plr[id]._pLevel >> 3) + 31);
		else
			GetMissileVel(mi, sx, sy, dx, dy, 32);
	} else
		GetMissileVel(mi, sx, sy, dx, dy, 32);

	SetMissDir(mi, GetDirection16(sx, sy, dx, dy));
	missile[mi]._mirange = 256;
	missile[mi]._miVar1 = sx;
	missile[mi]._miVar2 = sy;
	missile[mi]._mlid = AddLight(sx, sy, 5);
}

void __fastcall AddArrow(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int av;

	if (sx == dx && sy == dy) {
		dx += XDirAdd[midir];
		dy += YDirAdd[midir];
	}
	if (!mienemy) {
		av = 32;
		if (plr[id]._pIFlags & 4) {
			av = random(64, 32) + 16;
		}
		if (plr[id]._pClass == PC_ROGUE)
			av += (plr[id]._pLevel - 1) >> 2;
		if (plr[id]._pClass == PC_WARRIOR)
			av += (plr[id]._pLevel - 1) >> 3;
		GetMissileVel(mi, sx, sy, dx, dy, av);
	} else {
		GetMissileVel(mi, sx, sy, dx, dy, 32);
	}
	missile[mi]._miAnimFrame = GetDirection16(sx, sy, dx, dy) + 1;
	missile[mi]._mirange = 256;
}

void __fastcall GetVileMissPos(int mi, int dx, int dy)
{
	int xx, yy, k, j, i;

	for (k = 1; k < 50; k++) {
		for (j = -k; j <= k; j++) {
			yy = j + dy;
			for (i = -k; i <= k; i++) {
				xx = i + dx;
				if (PosOkPlayer(myplr, xx, yy)) {
					missile[mi]._mix = xx;
					missile[mi]._miy = yy;
					return;
				}
			}
		}
	}
	missile[mi]._mix = dx;
	missile[mi]._miy = dy;
}

void __fastcall AddRndTeleport(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int pn, r1, r2, nTries;

	nTries = 0;
	do {
		nTries++;
		if (nTries > 500) {
			r1 = 0;
			r2 = 0;
			break;
		}
		r1 = random(58, 3) + 4;
		r2 = random(58, 3) + 4;
		if (random(58, 2) == 1)
			r1 = -r1;
		if (random(58, 2) == 1)
			r2 = -r2;

	} while (nSolidTable[dPiece[r1 + sx][sy + r2]] || dObject[r1 + sx][sy + r2] || dMonster[r1 + sx][sy + r2]);

	missile[mi]._miVar1 = 0;
	missile[mi]._mirange = 2;
	if (setlevel == 0 || setlvlnum != SL_VILEBETRAYER) {
		missile[mi]._mix = sx + r1;
		missile[mi]._miy = sy + r2;
		if (!mienemy)
			UseMana(id, 10);
	} else {
		pn = dObject[dx][dy] - 1;
		if (object[pn]._otype == OBJ_MCIRCLE1 || object[pn]._otype == OBJ_MCIRCLE2) {
			missile[mi]._mix = dx;
			missile[mi]._miy = dy;
			if (!PosOkPlayer(myplr, dx, dy))
				GetVileMissPos(mi, dx, dy);
		}
	}
}
// 5CF31D: using guessed type char setlevel;

void __fastcall AddFirebolt(int mi, int sx, int sy, int dx, int dy, int midir, char micaster, int id, int dam)
{
	int i, mx, sp;

	if (sx == dx && sy == dy) {
		dx += XDirAdd[midir];
		dy += YDirAdd[midir];
	}
	if (!micaster) {
		for (i = 0; i < nummissiles; i++) {
			mx = missileactive[i];
			if (missile[mx]._mitype == MIS_GUARDIAN && missile[mx]._misource == id && missile[mx]._miVar3 == mi)
				break;
		}
		if (i == nummissiles)
			UseMana(id, 1);
		if (id != -1) {
			sp = 2 * missile[mi]._mispllvl + 16;
			if (sp >= 63)
				sp = 63;
		} else {
			sp = 16;
		}
	} else {
		sp = 26;
	}
	GetMissileVel(mi, sx, sy, dx, dy, sp);
	SetMissDir(mi, GetDirection16(sx, sy, dx, dy));
	missile[mi]._mirange = 256;
	missile[mi]._miVar1 = sx;
	missile[mi]._miVar2 = sy;
	missile[mi]._mlid = AddLight(sx, sy, 8);
}

void __fastcall AddMagmaball(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	GetMissileVel(mi, sx, sy, dx, dy, 16);
	missile[mi]._mitxoff += 3 * missile[mi]._mixvel;
	missile[mi]._mityoff += 3 * missile[mi]._miyvel;
	GetMissilePos(mi);
	missile[mi]._mirange = 256;
	missile[mi]._miVar1 = sx;
	missile[mi]._miVar2 = sy;
	missile[mi]._mlid = AddLight(sx, sy, 8);
}

void __fastcall miss_null_33(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	GetMissileVel(mi, sx, sy, dx, dy, 16);
	missile[mi]._mirange = 256;
	missile[mi]._miVar1 = sx;
	missile[mi]._miVar2 = sy;
	PutMissile(mi);
}

void __fastcall AddTeleport(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int i, pn, k, j, tx, ty;
	int CrawlNum[6] = { 0, 3, 12, 45, 94, 159 };

	missile[mi]._miDelFlag = TRUE;
	for (i = 0; i < 6; i++) {
		k = CrawlNum[i];
		pn = k + 2;
		for (j = (unsigned char)CrawlTable[k]; j > 0; j--) {
			tx = dx + CrawlTable[pn - 1];
			ty = dy + CrawlTable[pn];
			if (0 < tx && tx < MAXDUNX && 0 < ty && ty < MAXDUNY) {
				if (!(dMonster[tx][ty] | dObject[tx][ty] | dPlayer[tx][ty] | nSolidTable[dPiece[tx][ty]])) {
					missile[mi]._miDelFlag = FALSE;
					missile[mi]._mix = tx;
					missile[mi]._miy = ty;
					missile[mi]._misx = tx;
					missile[mi]._misy = ty;
					i = 6;
					break;
				}
			}
			pn += 2;
		}
	}

	if (!missile[mi]._miDelFlag) {
		UseMana(id, 23);
		missile[mi]._mirange = 2;
	}
}

void __fastcall AddLightball(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	GetMissileVel(mi, sx, sy, dx, dy, 16);
	missile[mi]._midam = dam;
	missile[mi]._miAnimFrame = random(63, 8) + 1;
	missile[mi]._mirange = 255;
	if (id < 0) {
		missile[mi]._miVar1 = sx;
		missile[mi]._miVar2 = sy;
	} else {
		missile[mi]._miVar1 = plr[id].WorldX;
		missile[mi]._miVar2 = plr[id].WorldY;
	}
}

void __fastcall AddFirewall(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int i;

	missile[mi]._midam = 16 * (random(53, 10) + random(53, 10) + plr[id]._pLevel + 2) >> 1;
	GetMissileVel(mi, sx, sy, dx, dy, 16);
	missile[mi]._mirange = 10;
	i = missile[mi]._mispllvl;
	if (i > 0)
		missile[mi]._mirange = 10 * (i + 1);
	missile[mi]._mirange = ((missile[mi]._mirange * plr[id]._pISplDur >> 3) & 0xFFFFFFF0) + 16 * missile[mi]._mirange;
	missile[mi]._miVar1 = missile[mi]._mirange - missile[mi]._miAnimLen;
	missile[mi]._miVar2 = 0;
}

void __fastcall AddFireball(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int i;

	if (sx == dx && sy == dy) {
		dx += XDirAdd[midir];
		dy += YDirAdd[midir];
	}
	if (!mienemy) {
		missile[mi]._midam = 2 * (plr[id]._pLevel + random(60, 10) + random(60, 10)) + 4;
		for (i = 0; i < missile[mi]._mispllvl; i++) {
			missile[mi]._midam += missile[mi]._midam >> 3;
		}
		i = 2 * missile[mi]._mispllvl + 16;
		if (i > 50)
			i = 50;
		UseMana(id, 12);
	} else {
		i = 16;
	}
	GetMissileVel(mi, sx, sy, dx, dy, i);
	SetMissDir(mi, GetDirection16(sx, sy, dx, dy));
	missile[mi]._mirange = 256;
	missile[mi]._miVar1 = sx;
	missile[mi]._miVar2 = sy;
	missile[mi]._miVar3 = 0;
	missile[mi]._miVar4 = sx;
	missile[mi]._miVar5 = sy;
	missile[mi]._mlid = AddLight(sx, sy, 8);
}

void __fastcall AddLightctrl(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	if (!dam && !mienemy)
		UseMana(id, 3);
	missile[mi]._miVar1 = sx;
	missile[mi]._miVar2 = sy;
	GetMissileVel(mi, sx, sy, dx, dy, 32);
	missile[mi]._miAnimFrame = random(52, 8) + 1;
	missile[mi]._mirange = 256;
}

void __fastcall AddLightning(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	missile[mi]._misx = dx;
	missile[mi]._misy = dy;
	if (midir >= 0) {
		missile[mi]._mixoff = missile[midir]._mixoff;
		missile[mi]._miyoff = missile[midir]._miyoff;
		missile[mi]._mitxoff = missile[midir]._mitxoff;
		missile[mi]._mityoff = missile[midir]._mityoff;
	}
	missile[mi]._miAnimFrame = random(52, 8) + 1;

	if (midir < 0 || mienemy == 1 || id == -1) {
		if (midir < 0 || id == -1)
			missile[mi]._mirange = 8;
		else
			missile[mi]._mirange = 10;
	} else {
		missile[mi]._mirange = (missile[mi]._mispllvl >> 1) + 6;
	}
	missile[mi]._mlid = AddLight(missile[mi]._mix, missile[mi]._miy, 4);
}

void __fastcall AddMisexp(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	CMonster *mon;

	if (mienemy && id > 0) {
		mon = monster[id].MType;
		if (mon->mtype == MT_SUCCUBUS)
			SetMissAnim(mi, MFILE_FLAREEXP);
		if (mon->mtype == MT_SNOWWICH)
			SetMissAnim(mi, MFILE_SCBSEXPB);
		if (mon->mtype == MT_HLSPWN)
			SetMissAnim(mi, MFILE_SCBSEXPD);
		if (mon->mtype == MT_SOLBRNR)
			SetMissAnim(mi, MFILE_SCBSEXPC);
	}

	missile[mi]._mix = missile[dx]._mix;
	missile[mi]._miy = missile[dx]._miy;
	missile[mi]._misx = missile[dx]._misx;
	missile[mi]._misy = missile[dx]._misy;
	missile[mi]._mixoff = missile[dx]._mixoff;
	missile[mi]._miyoff = missile[dx]._miyoff;
	missile[mi]._mitxoff = missile[dx]._mitxoff;
	missile[mi]._mityoff = missile[dx]._mityoff;
	missile[mi]._mixvel = 0;
	missile[mi]._miyvel = 0;
	missile[mi]._mirange = missile[mi]._miAnimLen;
	missile[mi]._miVar1 = 0;
}

void __fastcall AddWeapexp(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	missile[mi]._mix = sx;
	missile[mi]._miy = sy;
	missile[mi]._misx = sx;
	missile[mi]._misy = sy;
	missile[mi]._mixvel = 0;
	missile[mi]._miyvel = 0;
	missile[mi]._miVar1 = 0;
	missile[mi]._miVar2 = dx;
	missile[mi]._mimfnum = 0;
	if (dx == 1)
		SetMissAnim(mi, MFILE_MAGBLOS);
	else
		SetMissAnim(mi, MFILE_MINILTNG);
	missile[mi]._mirange = missile[mi]._miAnimLen - 1;
}

BOOL __fastcall CheckIfTrig(int x, int y)
{
	int i;

	for (i = 0; i < trigflag[4]; i++) {
		if ((x == trigs[i]._tx && y == trigs[i]._ty) || (abs(trigs[i]._tx - x) < 2 && abs(trigs[i]._ty - y) < 2))
			return TRUE;
	}
	return FALSE;
}

void __fastcall AddTown(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int i, j, k, mx, tx, ty;
	int CrawlNum[6] = { 0, 3, 12, 45, 94, 159 };

	tx = dx;
	if (currlevel) {
		missile[mi]._miDelFlag = TRUE;
		for (j = 0; j < 6; j++) {
			k = CrawlNum[j] + 2;
			for (i = (unsigned char)CrawlTable[CrawlNum[j]]; i > 0; i--) {
				tx = dx + CrawlTable[k - 1];
				ty = dy + CrawlTable[k];
				if (tx > 0 && tx < MAXDUNX && ty > 0 && ty < MAXDUNY) {
					if (!(dObject[tx][ty] | dPlayer[tx][ty] | dMissile[tx][ty] | nSolidTable[dPiece[tx][ty]] | (unsigned char)nMissileTable[dPiece[tx][ty]])) {
						if (!CheckIfTrig(tx, ty)) {
							missile[mi]._miDelFlag = FALSE;
							missile[mi]._mix = tx;
							missile[mi]._miy = ty;
							missile[mi]._misx = tx;
							missile[mi]._misy = ty;
							j = 6;
							break;
						}
					}
				}
				k += 2;
			}
		}
	} else {
		ty = dy;
		missile[mi]._mix = tx;
		missile[mi]._miy = ty;
		missile[mi]._misx = tx;
		missile[mi]._misy = ty;
		missile[mi]._miDelFlag = FALSE;
	}
	missile[mi]._mirange = 100;
	missile[mi]._miVar1 = 100 - missile[mi]._miAnimLen;
	missile[mi]._miVar2 = 0;
	for (i = 0; i < nummissiles; i++) {
		mx = missileactive[i];
		if (missile[mx]._mitype == MIS_TOWN && mx != mi && missile[mx]._misource == id)
			missile[mx]._mirange = 0;
	}
	PutMissile(mi);
	if (id == myplr && !missile[mi]._miDelFlag && currlevel) {
		if (!setlevel) {
			NetSendCmdLocParam3(TRUE, CMD_ACTIVATEPORTAL, tx, ty, currlevel, leveltype, 0);
		} else {
			NetSendCmdLocParam3(TRUE, CMD_ACTIVATEPORTAL, tx, ty, setlvlnum, leveltype, 1);
		}
	}
}
// 5CF31D: using guessed type char setlevel;

void __fastcall AddFlash(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int i;

	if (!mienemy && id != -1) {
		missile[mi]._midam = 0;
		for (i = 0; i <= plr[id]._pLevel; i++) {
			missile[mi]._midam += random(55, 20) + 1;
		}
		for (i = 0; i < missile[mi]._mispllvl; i++) {
			missile[mi]._midam += missile[mi]._midam >> 3;
		}
		missile[mi]._midam += missile[mi]._midam >> 1;
		UseMana(id, 4);
	} else {
		if (!mienemy) {
			missile[mi]._midam = currlevel >> 1;
		} else {
			missile[mi]._midam = monster[id].mLevel << 1;
		}
	}
	missile[mi]._mirange = 19;
}

void __fastcall AddFlash2(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int i;

	if (!mienemy) {
		if (id != -1) {
			missile[mi]._midam = 0;
			for (i = 0; i <= plr[id]._pLevel; i++) {
				missile[mi]._midam += random(56, 2) + 1;
			}
			for (i = 0; i < missile[mi]._mispllvl; i++) {
				missile[mi]._midam += missile[mi]._midam >> 3;
			}
			missile[mi]._midam += missile[mi]._midam >> 1;
		} else {
			missile[mi]._midam = currlevel >> 1;
		}
	}
	missile[mi]._miPreFlag = TRUE;
	missile[mi]._mirange = 19;
}

void __fastcall AddManashield(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	missile[mi]._mirange = 48 * plr[id]._pLevel;
	missile[mi]._miVar1 = plr[id]._pHitPoints;
	missile[mi]._miVar2 = plr[id]._pHPBase;
	missile[mi]._miVar8 = -1;
	if (!mienemy)
		UseMana(id, 11);
	if (id == myplr)
		NetSendCmd(TRUE, CMD_SETSHIELD);
	plr[id].pManaShield = TRUE;
}

void __fastcall AddFiremove(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	missile[mi]._midam = random(59, 10) + plr[id]._pLevel + 1;
	GetMissileVel(mi, sx, sy, dx, dy, 16);
	missile[mi]._mirange = 255;
	missile[mi]._miVar1 = 0;
	missile[mi]._miVar2 = 0;
	missile[mi]._mix++;
	missile[mi]._miy++;
	missile[mi]._miyoff -= 32;
}

void __fastcall AddGuardian(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int i, pn, k, j, tx, ty;
	int CrawlNum[6] = { 0, 3, 12, 45, 94, 159 };

	missile[mi]._midam = random(62, 10) + (plr[id]._pLevel >> 1) + 1;
	for (i = missile[mi]._mispllvl; i > 0; i--) {
		missile[mi]._midam += missile[mi]._midam >> 3;
	}

	missile[mi]._miDelFlag = TRUE;
	for (i = 0; i < 6; i++) {
		pn = CrawlNum[i];
		k = pn + 2;
		for (j = (unsigned char)CrawlTable[pn]; j > 0; j--) {
			tx = dx + CrawlTable[k - 1];
			ty = dy + CrawlTable[k];
			pn = dPiece[tx][ty];
			if (tx > 0 && tx < MAXDUNX && ty > 0 && ty < MAXDUNY) {
				if (LineClear(sx, sy, tx, ty)) {
					if (!(dMonster[tx][ty] | dObject[tx][ty] | dMissile[tx][ty] | nSolidTable[pn] | (unsigned char)nMissileTable[pn])) {
						missile[mi]._miDelFlag = FALSE;
						missile[mi]._mix = tx;
						missile[mi]._miy = ty;
						missile[mi]._misx = tx;
						missile[mi]._misy = ty;
						UseMana(id, 13);
						i = 6;
						break;
					}
				}
			}
			k += 2;
		}
	}

	if (missile[mi]._miDelFlag != TRUE) {
		missile[mi]._misource = id;
		missile[mi]._mlid = AddLight(missile[mi]._mix, missile[mi]._miy, 1);

		missile[mi]._mirange = plr[id]._pLevel >> 1;
		missile[mi]._mirange += missile[mi]._mispllvl;
		missile[mi]._mirange += (missile[mi]._mirange * plr[id]._pISplDur) >> 7;

		if (missile[mi]._mirange > 30)
			missile[mi]._mirange = 30;
		missile[mi]._mirange <<= 4;
		if (missile[mi]._mirange < 30)
			missile[mi]._mirange = 30;

		missile[mi]._miVar1 = missile[mi]._mirange - missile[mi]._miAnimLen;
		missile[mi]._miVar3 = 1;
		missile[mi]._miVar2 = 0;
	}
}

void __fastcall AddChain(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	missile[mi]._miVar1 = dx;
	missile[mi]._miVar2 = dy;
	missile[mi]._mirange = 1;
	UseMana(id, 14);
}

void __fastcall miss_null_11(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	SetMissDir(mi, dx);
	missile[mi]._midam = 0;
	missile[mi]._miLightFlag = TRUE;
	missile[mi]._mirange = 250;
}

void __fastcall miss_null_12(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	if (dx > 3)
		dx = 2;
	SetMissDir(mi, dx);
	missile[mi]._midam = 0;
	missile[mi]._miLightFlag = TRUE;
	missile[mi]._mirange = 250;
}

void __fastcall miss_null_13(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	if (dx > 3)
		dx = 2;
	SetMissDir(mi, dx);
	missile[mi]._midam = 0;
	missile[mi]._miLightFlag = TRUE;
	missile[mi]._mirange = missile[mi]._miAnimLen;
}

void __fastcall AddRhino(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	AnimStruct *anim;

	if (monster[id].MType->mtype < MT_HORNED || monster[id].MType->mtype > MT_OBLORD) {
		if (monster[id].MType->mtype < MT_NSNAKE || monster[id].MType->mtype > MT_GSNAKE) {
			anim = &monster[id].MType->Anims[MA_WALK];
		} else {
			anim = &monster[id].MType->Anims[MA_ATTACK];
		}
	} else {
		anim = &monster[id].MType->Anims[MA_SPECIAL];
	}
	GetMissileVel(mi, sx, sy, dx, dy, 18);
	missile[mi]._miAnimFlags = 0;
	missile[mi]._mimfnum = midir;
	missile[mi]._miAnimData = anim->Data[midir];
	missile[mi]._miAnimDelay = anim->Rate;
	missile[mi]._miAnimLen = anim->Frames;
	missile[mi]._miAnimWidth = monster[id].MType->width;
	missile[mi]._miAnimWidth2 = monster[id].MType->width2;
	missile[mi]._miAnimAdd = 1;
	if (monster[id].MType->mtype >= MT_NSNAKE && monster[id].MType->mtype <= MT_GSNAKE)
		missile[mi]._miAnimFrame = 7;
	missile[mi]._miVar1 = 0;
	missile[mi]._miVar2 = 0;
	missile[mi]._miLightFlag = TRUE;
	if (monster[id]._uniqtype != 0) {
		missile[mi]._miUniqTrans = monster[id]._uniqtrans + 1;
		missile[mi]._mlid = monster[id].mlid;
	}
	missile[mi]._mirange = 256;
	PutMissile(mi);
}

void __fastcall miss_null_32(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	AnimStruct *anim;

	anim = &monster[id].MType->Anims[MA_WALK];
	GetMissileVel(mi, sx, sy, dx, dy, 16);
	missile[mi]._mimfnum = midir;
	missile[mi]._miAnimFlags = 0;
	missile[mi]._miAnimData = anim->Data[midir];
	missile[mi]._miAnimDelay = anim->Rate;
	missile[mi]._miAnimLen = anim->Frames;
	missile[mi]._miAnimWidth = monster[id].MType->width;
	missile[mi]._miAnimWidth2 = monster[id].MType->width2;
	missile[mi]._miAnimAdd = 1;
	missile[mi]._miVar1 = 0;
	missile[mi]._miVar2 = 0;
	missile[mi]._miLightFlag = TRUE;
	if (monster[id]._uniqtype != 0)
		missile[mi]._miUniqTrans = monster[id]._uniqtrans + 1;
	dMonster[monster[id]._mx][monster[id]._my] = 0;
	missile[mi]._mirange = 256;
	PutMissile(mi);
}

void __fastcall AddFlare(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	if (sx == dx && sy == dy) {
		dx += XDirAdd[midir];
		dy += YDirAdd[midir];
	}
	GetMissileVel(mi, sx, sy, dx, dy, 16);
	missile[mi]._mirange = 256;
	missile[mi]._miVar1 = sx;
	missile[mi]._miVar2 = sy;
	missile[mi]._mlid = AddLight(sx, sy, 8);
	if (!mienemy) {
		UseMana(id, 35);
		drawhpflag = TRUE;
		plr[id]._pHitPoints -= 320;
		plr[id]._pHPBase -= 320;
		if (plr[id]._pHitPoints <= 0)
			SyncPlrKill(id, 0);
	} else {
		if (id > 0) {
			if (monster[id].MType->mtype == MT_SUCCUBUS)
				SetMissAnim(mi, MFILE_FLARE);
			if (monster[id].MType->mtype == MT_SNOWWICH)
				SetMissAnim(mi, MFILE_SCUBMISB);
			if (monster[id].MType->mtype == MT_HLSPWN)
				SetMissAnim(mi, MFILE_SCUBMISD);
			if (monster[id].MType->mtype == MT_SOLBRNR)
				SetMissAnim(mi, MFILE_SCUBMISC);
		}
	}
}

void __fastcall AddAcid(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	GetMissileVel(mi, sx, sy, dx, dy, 16);
	SetMissDir(mi, GetDirection16(sx, sy, dx, dy));
	missile[mi]._mlid = -1;
	missile[mi]._miVar1 = sx;
	missile[mi]._miVar2 = sy;
	missile[mi]._mirange = 5 * (monster[id]._mint + 4);
	PutMissile(mi);
}

void __fastcall miss_null_1D(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	missile[mi]._midam = dam;
	missile[mi]._mirange = 50;
	missile[mi]._mixvel = 0;
	missile[mi]._miyvel = 0;
	missile[mi]._miVar1 = 50 - missile[mi]._miAnimLen;
	missile[mi]._miVar2 = 0;
}

void __fastcall AddAcidpud(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int monst;

	monst = missile[mi]._misource;
	missile[mi]._mixvel = 0;
	missile[mi]._miyvel = 0;
	missile[mi]._mixoff = 0;
	missile[mi]._miyoff = 0;
	missile[mi]._miLightFlag = TRUE;
	missile[mi]._mirange = random(50, 15) + 40 * (monster[monst]._mint + 1);
	missile[mi]._miPreFlag = TRUE;
}

void __fastcall AddStone(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int i, j, k, l, tx, ty, mid;
	int CrawlNum[6] = { 0, 3, 12, 45, 94, 159 };

	missile[mi]._misource = id;
	for (i = 0; i < 6; i++) {
		k = CrawlNum[i];
		l = k + 2;
		for (j = (unsigned char)CrawlTable[k]; j > 0; j--) {
			tx = dx + CrawlTable[l - 1];
			ty = dy + CrawlTable[l];
			if (tx > 0 && tx < MAXDUNX && ty > 0 && ty < MAXDUNY) {
				mid = dMonster[tx][ty];
				mid = mid > 0 ? mid - 1 : -1 - mid;
				if (mid > 3 && monster[mid]._mAi != AI_DIABLO) {
					if (monster[mid]._mmode != MM_FADEIN && monster[mid]._mmode != MM_FADEOUT && monster[mid]._mmode != MM_CHARGE) {
						j = -99;
						i = 6;
						missile[mi]._miVar1 = monster[mid]._mmode;
						missile[mi]._miVar2 = mid;
						monster[mid]._mmode = MM_STONE;
						break;
					}
				}
			}
			l += 2;
		}
	}

	if (j != -99) {
		missile[mi]._miDelFlag = TRUE;
	} else {
		missile[mi]._mix = tx;
		missile[mi]._miy = ty;
		missile[mi]._misx = tx;
		missile[mi]._misy = ty;
		missile[mi]._mirange = 6;
		missile[mi]._mirange += missile[mi]._mispllvl;
		missile[mi]._mirange += (missile[mi]._mirange * plr[id]._pISplDur) >> 7;

		if (missile[mi]._mirange > 15)
			missile[mi]._mirange = 15;
		missile[mi]._mirange <<= 4;
		UseMana(id, 8);
	}
}

void __fastcall AddGolem(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int i;
	int mx;

	missile[mi]._miDelFlag = FALSE;
	for (i = 0; i < nummissiles; i++) {
		mx = missileactive[i];
		if (missile[mx]._mitype == MIS_GOLEM) {
			if (mx != mi && missile[mx]._misource == id) {
				missile[mi]._miDelFlag = TRUE;
				return;
			}
		}
	}
	missile[mi]._miVar1 = sx;
	missile[mi]._miVar2 = sy;
	missile[mi]._miVar4 = dx;
	missile[mi]._miVar5 = dy;
	if ((monster[id]._mx != 1 || monster[id]._my) && id == myplr)
		M_StartKill(id, id);
	UseMana(id, 21);
}

void __fastcall AddEtherealize(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int i;

	missile[mi]._mirange = 16 * plr[id]._pLevel >> 1;
	for (i = 0; i < missile[mi]._mispllvl; i++) {
		missile[mi]._mirange += missile[mi]._mirange >> 3;
	}
	missile[mi]._mirange += missile[mi]._mirange * plr[id]._pISplDur >> 7;
	missile[mi]._miVar1 = plr[id]._pHitPoints;
	missile[mi]._miVar2 = plr[id]._pHPBase;
	if (!mienemy)
		UseMana(id, 25);
}

void __fastcall miss_null_1F(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	missile[mi]._miDelFlag = TRUE;
}

void __fastcall miss_null_23(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	missile[mi]._midam = dam;
	missile[mi]._mix = sx;
	missile[mi]._miy = sy;
	missile[mi]._misx = sx;
	missile[mi]._misy = sy;
	missile[mi]._misource = id;
	if (dam == 1)
		SetMissDir(mi, 0);
	else
		SetMissDir(mi, 1);
	missile[mi]._miLightFlag = TRUE;
	missile[mi]._mirange = missile[mi]._miAnimLen;
}

void __fastcall AddBoom(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	missile[mi]._mix = dx;
	missile[mi]._miy = dy;
	missile[mi]._misx = dx;
	missile[mi]._misy = dy;
	missile[mi]._mixvel = 0;
	missile[mi]._miyvel = 0;
	missile[mi]._midam = dam;
	missile[mi]._mirange = missile[mi]._miAnimLen;
	missile[mi]._miVar1 = 0;
}

void __fastcall AddHeal(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int i;
	int HealAmount;

	HealAmount = (random(57, 10) + 1) << 6;
	for (i = 0; i < plr[id]._pLevel; i++) {
		HealAmount += (random(57, 4) + 1) << 6;
	}
	for (i = 0; i < missile[mi]._mispllvl; i++) {
		HealAmount += (random(57, 6) + 1) << 6;
	}

	if (plr[id]._pClass == PC_WARRIOR)
		HealAmount += HealAmount;

	if (plr[id]._pClass == PC_ROGUE)
		HealAmount += HealAmount >> 1;

	plr[id]._pHitPoints += HealAmount;
	if (plr[id]._pHitPoints > plr[id]._pMaxHP)
		plr[id]._pHitPoints = plr[id]._pMaxHP;

	plr[id]._pHPBase += HealAmount;
	if (plr[id]._pHPBase > plr[id]._pMaxHPBase)
		plr[id]._pHPBase = plr[id]._pMaxHPBase;

	UseMana(id, 2);
	missile[mi]._miDelFlag = TRUE;
	drawhpflag = TRUE;
}

void __fastcall AddHealOther(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	missile[mi]._miDelFlag = TRUE;
	UseMana(id, 34);
	if (id == myplr)
		SetCursor_(CURSOR_HEALOTHER);
}

void __fastcall AddElement(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int i;

	if (sx == dx && sy == dy) {
		dx += XDirAdd[midir];
		dy += YDirAdd[midir];
	}
	missile[mi]._midam = 2 * (plr[id]._pLevel + random(60, 10) + random(60, 10)) + 4;
	for (i = 0; i < missile[mi]._mispllvl; i++) {
		missile[mi]._midam += missile[mi]._midam >> 3;
	}
	missile[mi]._midam >>= 1;
	GetMissileVel(mi, sx, sy, dx, dy, 16);
	SetMissDir(mi, GetDirection8(sx, sy, dx, dy));
	missile[mi]._mirange = 256;
	missile[mi]._miVar1 = sx;
	missile[mi]._miVar2 = sy;
	missile[mi]._miVar3 = 0;
	missile[mi]._miVar4 = dx;
	missile[mi]._miVar5 = dy;
	missile[mi]._mlid = AddLight(sx, sy, 8);
	UseMana(id, 29);
}

void __fastcall AddIdentify(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	missile[mi]._miDelFlag = TRUE;
	UseMana(id, 5);
	if (id == myplr) {
		if (sbookflag)
			sbookflag = 0;
		if (!invflag)
			invflag = 1;
		SetCursor_(CURSOR_IDENTIFY);
	}
}
// 4B8968: using guessed type int sbookflag;

void __fastcall AddFirewallC(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int i, j, k, tx, ty, pn;
	int CrawlNum[6] = { 0, 3, 12, 45, 94, 159 };

	missile[mi]._miDelFlag = TRUE;
	for (i = 0; i < 6; i++) {
		k = CrawlNum[i];
		pn = k + 2;
		for (j = (unsigned char)CrawlTable[k]; j > 0; j--) {
			tx = dx + CrawlTable[pn - 1];
			ty = dy + CrawlTable[pn];
			if (0 < tx && tx < MAXDUNX && 0 < ty && ty < MAXDUNY) {
				k = dPiece[tx][ty];
				if (LineClear(sx, sy, tx, ty)) {
					if ((sx != tx || sy != ty) && !(nSolidTable[k] | dObject[tx][ty])) {
						missile[mi]._miDelFlag = FALSE;
						missile[mi]._miVar1 = tx;
						missile[mi]._miVar2 = ty;
						missile[mi]._miVar5 = tx;
						missile[mi]._miVar6 = ty;
						i = 6;
						break;
					}
				}
			}
			pn += 2;
		}
	}

	if (missile[mi]._miDelFlag != TRUE) {
		missile[mi]._miVar7 = 0;
		missile[mi]._miVar8 = 0;
		missile[mi]._miVar3 = (midir - 2) & 7;
		missile[mi]._miVar4 = (midir + 2) & 7;
		missile[mi]._mirange = 7;
		UseMana(id, 6);
	}
}

void __fastcall AddInfra(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int i;

	missile[mi]._mirange = 1584;
	for (i = 0; i < missile[mi]._mispllvl; i++) {
		missile[mi]._mirange += missile[mi]._mirange >> 3;
	}
	missile[mi]._mirange += missile[mi]._mirange * plr[id]._pISplDur >> 7;
	if (!mienemy)
		UseMana(id, 9);
}

void __fastcall AddWave(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	missile[mi]._miVar1 = dx;
	missile[mi]._miVar2 = dy;
	missile[mi]._miVar3 = 0;
	missile[mi]._miVar4 = 0;
	missile[mi]._mirange = 1;
	missile[mi]._miAnimFrame = 4;
	UseMana(id, 15);
}

void __fastcall AddNova(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int k;

	missile[mi]._miVar1 = dx;
	missile[mi]._miVar2 = dy;
	if (id != -1) {
		missile[mi]._midam = (random(66, 6) + random(66, 6) + random(66, 6) + random(66, 6) + random(66, 6) + plr[id]._pLevel + 5) >> 1;
		for (k = 0; k < missile[mi]._mispllvl; k++) {
			missile[mi]._midam += missile[mi]._midam >> 3;
		}
		if (!mienemy)
			UseMana(id, 18);
	} else {
		missile[mi]._midam = ((DWORD)currlevel >> 1) + random(66, 3) + random(66, 3) + random(66, 3);
	}
	missile[mi]._mirange = 1;
}

void __fastcall AddRepair(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	missile[mi]._miDelFlag = TRUE;
	UseMana(id, 26);
	if (id == myplr) {
		if (sbookflag)
			sbookflag = 0;
		if (!invflag)
			invflag = 1;
		SetCursor_(CURSOR_REPAIR);
	}
}
// 4B8968: using guessed type int sbookflag;

void __fastcall AddRecharge(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	missile[mi]._miDelFlag = TRUE;
	UseMana(id, 27);
	if (id == myplr) {
		if (sbookflag)
			sbookflag = 0;
		if (!invflag)
			invflag = 1;
		SetCursor_(CURSOR_RECHARGE);
	}
}
// 4B8968: using guessed type int sbookflag;

void __fastcall AddDisarm(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	missile[mi]._miDelFlag = TRUE;
	UseMana(id, 28);
	if (id == myplr)
		SetCursor_(CURSOR_DISARM);
}

void __fastcall AddApoca(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int i;

	missile[mi]._miVar1 = 8;
	missile[mi]._miVar2 = sy - 8;
	missile[mi]._miVar3 = sy + 8;
	missile[mi]._miVar4 = sx - 8;
	missile[mi]._miVar5 = sx + 8;
	missile[mi]._miVar6 = sx - 8;
	if (sy - 8 <= 0)
		missile[mi]._miVar2 = 1;
	if (sy + 8 >= MAXDUNY)
		missile[mi]._miVar3 = MAXDUNY - 1;
	if (sx - 8 <= 0)
		missile[mi]._miVar4 = 1;
	if (sx + 8 >= MAXDUNX)
		missile[mi]._miVar5 = MAXDUNX - 1;
	for (i = 0; i < plr[id]._pLevel; i++) {
		missile[mi]._midam += random(67, 6) + 1;
	}
	missile[mi]._miDelFlag = FALSE;
	missile[mi]._mirange = 255;
	UseMana(id, 24);
}

void __fastcall AddFlame(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int i;

	missile[mi]._miVar2 = 0;
	if (dam > 0)
		missile[mi]._miVar2 = 5 * dam;
	missile[mi]._misx = dx;
	missile[mi]._misy = dy;
	missile[mi]._mixoff = missile[midir]._mixoff;
	missile[mi]._miyoff = missile[midir]._miyoff;
	missile[mi]._mitxoff = missile[midir]._mitxoff;
	missile[mi]._mityoff = missile[midir]._mityoff;
	missile[mi]._mirange = missile[mi]._miVar2 + 20;
	missile[mi]._mlid = AddLight(sx, sy, 1);
	if (!mienemy) {
		i = random(79, plr[id]._pLevel) + random(79, 2);
		missile[mi]._midam = 8 * i + 16 + ((8 * i + 16) >> 1);
	} else {
		missile[mi]._midam = monster[id].mMinDamage + random(77, monster[id].mMaxDamage - monster[id].mMinDamage + 1);
	}
}

void __fastcall AddFlamec(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	if (sx == dx && sy == dy) {
		dx += XDirAdd[midir];
		dy += YDirAdd[midir];
	}
	GetMissileVel(mi, sx, sy, dx, dy, 32);
	if (!mienemy)
		UseMana(id, 20);
	missile[mi]._miVar3 = 0;
	missile[mi]._miVar2 = sy;
	missile[mi]._miVar1 = sx;
	missile[mi]._mirange = 256;
}

void __fastcall AddCbolt(int mi, int sx, int sy, int dx, int dy, int midir, char micaster, int id, int dam)
{
	int v9;  // esi
	int v10; // eax
	int v12; // edx
	int v13; // eax
	int v14; // ecx
	int i;   // [esp+Ch] [ebp-8h]
	int x;   // [esp+10h] [ebp-4h]

	i = mi;
	v9 = mi;
	x = sx;
	if ((_BYTE)micaster) {
		v13 = random(63, 15);
		missile[v9]._midam = 15;
		missile[v9]._mirnd = v13 + 1;
	} else {
		v10 = random(63, 15);
		v12 = plr[id]._pMagic;
		missile[v9]._mirnd = v10 + 1;
		missile[v9]._midam = random(68, v12 >> 2) + 1;
	}
	v14 = dx;
	if (x == dx && sy == dy) {
		v14 = XDirAdd[midir] + dx;
		dx += XDirAdd[midir];
		dy += YDirAdd[midir];
	}
	missile[v9]._miAnimFrame = random(63, 8) + 1;
	missile[v9]._mlid = AddLight(x, sy, 5);
	GetMissileVel(i, x, sy, dx, dy, 8);
	missile[v9]._miVar3 = 0;
	missile[v9]._miVar1 = 5;
	missile[v9]._miVar2 = midir;
	missile[v9]._mirange = 256;
}

void __fastcall AddHbolt(int mi, int sx, int sy, int dx, int dy, int midir, char micaster, int id, int dam)
{
	int sp;

	if (sx == dx && sy == dy) {
		dx += XDirAdd[midir];
		dy += YDirAdd[midir];
	}
	if (id != -1) {
		sp = 2 * missile[mi]._mispllvl + 16;
		if (sp >= 63) {
			sp = 63;
		}
	} else {
		sp = 16;
	}
	GetMissileVel(mi, sx, sy, dx, dy, sp);
	SetMissDir(mi, GetDirection16(sx, sy, dx, dy));
	missile[mi]._mirange = 256;
	missile[mi]._miVar1 = sx;
	missile[mi]._miVar2 = sy;
	missile[mi]._mlid = AddLight(sx, sy, 8);
	missile[mi]._midam = random(69, 10) + plr[id]._pLevel + 9;
	UseMana(id, 31);
}

void __fastcall AddResurrect(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	UseMana(id, 32);
	if (id == myplr)
		SetCursor_(CURSOR_RESURRECT);
	missile[mi]._miDelFlag = TRUE;
}

void __fastcall AddResurrectBeam(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	missile[mi]._mix = dx;
	missile[mi]._miy = dy;
	missile[mi]._misx = dx;
	missile[mi]._misy = dy;
	missile[mi]._mixvel = 0;
	missile[mi]._miyvel = 0;
	missile[mi]._mirange = misfiledata[36].mAnimLen[0];
}

void __fastcall AddTelekinesis(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	missile[mi]._miDelFlag = TRUE;
	UseMana(id, 33);
	if (id == myplr)
		SetCursor_(CURSOR_TELEKINESIS);
}

void __fastcall AddBoneSpirit(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	if (sx == dx && sy == dy) {
		dx = XDirAdd[midir] + dx;
		dy = YDirAdd[midir] + dy;
	}
	missile[mi]._midam = 0;
	GetMissileVel(mi, sx, sy, dx, dy, 16);
	SetMissDir(mi, GetDirection8(sx, sy, dx, dy));
	missile[mi]._mirange = 256;
	missile[mi]._miVar1 = sx;
	missile[mi]._miVar2 = sy;
	missile[mi]._miVar3 = 0;
	missile[mi]._miVar4 = dx;
	missile[mi]._miVar5 = dy;
	missile[mi]._mlid = AddLight(sx, sy, 8);
	if (!mienemy) {
		UseMana(id, 36);
		drawhpflag = TRUE;
		plr[id]._pHitPoints -= 384;
		plr[id]._pHPBase -= 384;
		if (plr[id]._pHitPoints <= 0)
			SyncPlrKill(id, 0);
	}
}

void __fastcall AddRportal(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	missile[mi]._mix = sx;
	missile[mi]._miy = sy;
	missile[mi]._misx = sx;
	missile[mi]._misy = sy;
	missile[mi]._miVar1 = 100 - missile[mi]._miAnimLen;
	missile[mi]._miVar2 = 0;
	missile[mi]._mirange = 100;
	PutMissile(mi);
}

void __fastcall AddDiabApoca(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam)
{
	int pnum;

	for (pnum = 0; pnum < gbMaxPlayers; pnum++) {
		if (plr[pnum].plractive) {
			if (LineClear(sx, sy, plr[pnum]._px, plr[pnum]._py)) {
				AddMissile(0, 0, plr[pnum]._px, plr[pnum]._py, 0, MIS_BOOM2, mienemy, id, dam, 0);
			}
		}
	}
	missile[mi]._miDelFlag = TRUE;
}
// 679660: using guessed type char gbMaxPlayers;

int __fastcall AddMissile(int sx, int sy, int dx, int dy, int midir, int mitype, char micaster, int id, int midam, int spllvl)
{
	int i, mi;

	if (nummissiles >= MAXMISSILES)
		return -1;

	if (mitype == MIS_MANASHIELD && plr[id].pManaShield == TRUE) {
		if (currlevel != plr[id].plrlevel)
			return -1;

		for (i = 0; i < nummissiles; i++) {
			mi = missileactive[i];
			if (missile[mi]._mitype == MIS_MANASHIELD && missile[mi]._misource == id)
				return -1;
		}
	}

	mi = missileavail[0];

	missileavail[0] = missileavail[MAXMISSILES - nummissiles - 1];
	missileactive[nummissiles] = mi;
	nummissiles++;

	missile[mi]._mitype = mitype;
	missile[mi]._micaster = micaster;
	missile[mi]._misource = id;
	missile[mi]._miAnimType = missiledata[mitype].mFileNum;
	missile[mi]._miDrawFlag = missiledata[mitype].mDraw;
	missile[mi]._mimfnum = midir;
	missile[mi]._mispllvl = spllvl;

	if (missile[mi]._miAnimType == 255 || misfiledata[missile[mi]._miAnimType].mAnimFAmt < 8)
		SetMissDir(mi, 0);
	else
		SetMissDir(mi, midir);

	missile[mi]._mix = sx;
	missile[mi]._miy = sy;
	missile[mi]._mixoff = 0;
	missile[mi]._miyoff = 0;
	missile[mi]._misx = sx;
	missile[mi]._misy = sy;
	missile[mi]._mitxoff = 0;
	missile[mi]._mityoff = 0;
	missile[mi]._miDelFlag = FALSE;
	missile[mi]._miAnimAdd = 1;
	missile[mi]._miLightFlag = FALSE;
	missile[mi]._miPreFlag = FALSE;
	missile[mi]._miUniqTrans = 0;
	missile[mi]._midam = midam;
	missile[mi]._miHitFlag = FALSE;
	missile[mi]._midist = 0;
	missile[mi]._mlid = -1;
	missile[mi]._mirnd = 0;

	if (missiledata[mitype].mlSFX != -1) {
		PlaySfxLoc(missiledata[mitype].mlSFX, sx, sy);
	}

	missiledata[mitype].mAddProc(mi, sx, sy, dx, dy, midir, micaster, id, midam);

	return mi;
}

int __fastcall Sentfire(int i, int sx, int sy)
{
	int ex, dir;

	ex = 0;
	if (LineClear(missile[i]._mix, missile[i]._miy, sx, sy)) {
		if (dMonster[sx][sy] > 0 && monster[dMonster[sx][sy] - 1]._mhitpoints >> 6 > 0 && dMonster[sx][sy] - 1 > 3) {
			dir = GetDirection(missile[i]._mix, missile[i]._miy, sx, sy);
			missile[i]._miVar3 = missileavail[0];
			AddMissile(missile[i]._mix, missile[i]._miy, sx, sy, dir, MIS_FIREBOLT, 0, missile[i]._misource, missile[i]._midam, GetSpellLevel(missile[i]._misource, 1));
			ex = -1;
			SetMissDir(i, 2);
			missile[i]._miVar2 = 3;
		}
	}

	return ex;
}

void __fastcall MI_Dummy(int i)
{
	return;
}

void __fastcall MI_Golem(int i)
{
	int CrawlNum[6] = { 0, 3, 12, 45, 94, 159 };
	int tx, ty, dp, l, m, src, k, tid;

	src = missile[i]._misource;
	if (monster[src]._mx == 1 && !monster[src]._my) {
		for (l = 0; l < 6; l++) {
			k = CrawlNum[l];
			tid = k + 2;
			for (m = (unsigned char)CrawlTable[k]; m > 0; m--) {
				tx = missile[i]._miVar4 + CrawlTable[tid - 1];
				ty = missile[i]._miVar5 + CrawlTable[tid];
				if (0 < tx && tx < MAXDUNX && 0 < ty && ty < MAXDUNY) {
					dp = dPiece[tx][ty];
					if (LineClear(missile[i]._miVar1, missile[i]._miVar2, tx, ty)) {
						if (!(dMonster[tx][ty] | nSolidTable[dp] | dObject[tx][ty])) {
							l = 6;
							SpawnGolum(src, tx, ty, i);
							break;
						}
					}
				}
				tid += 2;
			}
		}
	}
	missile[i]._miDelFlag = TRUE;
}

void __fastcall MI_SetManashield(int i)
{
	ManashieldFlag = 1;
}

void __fastcall MI_LArrow(int i)
{
	int p, mind, maxd, rst;

	missile[i]._mirange--;
	p = missile[i]._misource;
	if (missile[i]._miAnimType == MFILE_MINILTNG || missile[i]._miAnimType == MFILE_MAGBLOS) {
		ChangeLight(missile[i]._mlid, missile[i]._mix, missile[i]._miy, missile[i]._miAnimFrame + 5);
		rst = missiledata[missile[i]._mitype].mResist;
		if (missile[i]._mitype == MIS_LARROW) {
			if (p != -1) {
				mind = plr[p]._pILMinDam;
				maxd = plr[p]._pILMaxDam;
			} else {
				mind = random(68, 10) + 1 + currlevel;
				maxd = random(68, 10) + 1 + currlevel * 2;
			}
			missiledata[MIS_LARROW].mResist = MISR_LIGHTNING;
			CheckMissileCol(i, mind, maxd, 0, missile[i]._mix, missile[i]._miy, 1);
		}
		if (missile[i]._mitype == MIS_FARROW) {
			if (p != -1) {
				mind = plr[p]._pIFMinDam;
				maxd = plr[p]._pIFMaxDam;
			} else {
				mind = random(68, 10) + 1 + currlevel;
				maxd = random(68, 10) + 1 + currlevel * 2;
			}
			missiledata[MIS_FARROW].mResist = MISR_FIRE;
			CheckMissileCol(i, mind, maxd, 0, missile[i]._mix, missile[i]._miy, 1);
		}
		missiledata[missile[i]._mitype].mResist = rst;
	} else {
		missile[i]._midist++;
		missile[i]._mitxoff += missile[i]._mixvel;
		missile[i]._mityoff += missile[i]._miyvel;
		GetMissilePos(i);

		if (p != -1) {
			if (!missile[i]._micaster) {
				mind = plr[p]._pIMinDam;
				maxd = plr[p]._pIMaxDam;
			} else {
				mind = monster[p].mMinDamage;
				maxd = monster[p].mMaxDamage;
			}
		} else {
			mind = random(68, 10) + 1 + currlevel;
			maxd = random(68, 10) + 1 + currlevel * 2;
		}

		if (missile[i]._mix != missile[i]._misx || missile[i]._miy != missile[i]._misy) {
			rst = missiledata[missile[i]._mitype].mResist;
			missiledata[missile[i]._mitype].mResist = 0;
			CheckMissileCol(i, mind, maxd, 0, missile[i]._mix, missile[i]._miy, 0);
			missiledata[missile[i]._mitype].mResist = rst;
		}
		if (!missile[i]._mirange) {
			missile[i]._mimfnum = 0;
			missile[i]._mitxoff -= missile[i]._mixvel;
			missile[i]._mityoff -= missile[i]._miyvel;
			GetMissilePos(i);
			if (missile[i]._mitype == MIS_LARROW)
				SetMissAnim(i, MFILE_MINILTNG);
			else
				SetMissAnim(i, MFILE_MAGBLOS);
			missile[i]._mirange = missile[i]._miAnimLen - 1;
		} else {
			if (missile[i]._mix != missile[i]._miVar1 || missile[i]._miy != missile[i]._miVar2) {
				missile[i]._miVar1 = missile[i]._mix;
				missile[i]._miVar2 = missile[i]._miy;
				ChangeLight(missile[i]._mlid, missile[i]._mix, missile[i]._miy, 5);
			}
		}
	}
	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = TRUE;
		AddUnLight(missile[i]._mlid);
	}
	PutMissile(i);
}

void __fastcall MI_Arrow(int i)
{
	int p, mind, maxd;

	missile[i]._mirange--;
	missile[i]._midist++;
	missile[i]._mitxoff += missile[i]._mixvel;
	missile[i]._mityoff += missile[i]._miyvel;
	GetMissilePos(i);
	p = missile[i]._misource;
	if (p != -1) {
		if (!missile[i]._micaster) {
			mind = plr[p]._pIMinDam;
			maxd = plr[p]._pIMaxDam;
		} else {
			mind = monster[p].mMinDamage;
			maxd = monster[p].mMaxDamage;
		}
	} else {
		mind = currlevel;
		maxd = 2 * currlevel;
	}
	if (missile[i]._mix != missile[i]._misx || missile[i]._miy != missile[i]._misy)
		CheckMissileCol(i, mind, maxd, 0, missile[i]._mix, missile[i]._miy, 0);
	if (!missile[i]._mirange)
		missile[i]._miDelFlag = TRUE;
	PutMissile(i);
}

void __fastcall MI_Firebolt(int i)
{
	int v1;  // edi
	int v2;  // esi
	int v3;  // ecx
	int v4;  // ST1C_4
	int v5;  // edx
	int v6;  // ecx
	int v7;  // eax
	int v9;  // edi
	int v10; // eax
	int v11; // edi
	int v12; // eax
	int v13; // ecx
	int v14; // ecx
	int v15; // eax
	int v16; // esi
	int v17; // edx
	int v18; // eax
	int v19; // esi
	int v21; // [esp+Ch] [ebp-Ch]
	int v22; // [esp+10h] [ebp-8h]
	int ia;  // [esp+14h] [ebp-4h]

	v1 = i;
	ia = i;
	v2 = i;
	--missile[v2]._mirange;
	if (missile[i]._mitype == MIS_BONESPIRIT && missile[v2]._mimfnum == 8) {
		if (!missile[i]._mirange) {
			v3 = missile[v2]._mlid;
			if (v3 >= 0)
				AddUnLight(v3);
			v4 = missile[v2]._miy;
			v5 = missile[v2]._mix;
			missile[v2]._miDelFlag = TRUE;
			PlaySfxLoc(LS_BSIMPCT, v5, v4);
		}
		goto LABEL_39;
	}
	v6 = missile[v2]._mityoff;
	v22 = missile[v2]._mitxoff;
	v21 = v6;
	v7 = v6 + missile[v2]._miyvel;
	missile[v2]._mitxoff = v22 + missile[v2]._mixvel;
	missile[v2]._mityoff = v7;
	GetMissilePos(v1);
	v9 = missile[v2]._misource;
	if (v9 == -1) {
		v12 = random(78, 2 * currlevel);
		v13 = currlevel;
		goto LABEL_17;
	}
	if (missile[v2]._micaster) {
		v11 = v9;
		v12 = random(77, (unsigned char)monster[v11].mMaxDamage - (unsigned char)monster[v11].mMinDamage + 1);
		v13 = (unsigned char)monster[v11].mMinDamage;
	LABEL_17:
		v10 = v13 + v12;
		goto LABEL_19;
	}
	switch (missile[v2]._mitype) {
	case 1:
		v10 = (plr[v9]._pMagic >> 3) + random(75, 10) + missile[v2]._mispllvl + 1;
		break;
	case 0x18:
		v10 = (plr[v9]._pMagic >> 1) + 3 * missile[v2]._mispllvl - (plr[v9]._pMagic >> 3);
		break;
	case 0x3F:
		v10 = 0;
		break;
	default:
		v10 = v21;
		break;
	}
LABEL_19:
	v14 = missile[v2]._mix;
	if (v14 == missile[v2]._misx && missile[v2]._miy == missile[v2]._misy) {
		v1 = ia;
	} else {
		v1 = ia;
		CheckMissileCol(ia, v10, v10, 0, v14, missile[v2]._miy, 0);
	}
	if (missile[v2]._mirange) {
		v17 = missile[v2]._mix;
		if (v17 != missile[v2]._miVar1 || missile[v2]._miy != missile[v2]._miVar2) {
			missile[v2]._miVar1 = v17;
			v18 = missile[v2]._miy;
			missile[v2]._miVar2 = v18;
			v19 = missile[v2]._mlid;
			if (v19 >= 0)
				ChangeLight(v19, v17, v18, 8);
		}
	} else {
		missile[v2]._mitxoff = v22;
		missile[v2]._miDelFlag = TRUE;
		missile[v2]._mityoff = v21;
		GetMissilePos(v1);
		v15 = missile[v2]._mitype - 1;
		if (missile[v2]._mitype == MIS_FIREBOLT || (v15 = missile[v2]._mitype - 21, missile[v2]._mitype == MIS_MAGMABALL)) {
			_LOBYTE(v15) = missile[v2]._micaster;
			AddMissile(
			    missile[v2]._mix,
			    missile[v2]._miy,
			    v1,
			    0,
			    missile[v2]._mimfnum,
			    MIS_MISEXP,
			    v15,
			    missile[v2]._misource,
			    0,
			    0);
		} else {
			switch (missile[v2]._mitype) {
			case MIS_FLARE:
				AddMissile(
				    missile[v2]._mix,
				    missile[v2]._miy,
				    v1,
				    0,
				    missile[v2]._mimfnum,
				    MIS_MISEXP2,
				    _LOBYTE(missile[v2]._micaster),
				    missile[v2]._misource,
				    0,
				    0);
				break;
			case MIS_ACID:
				AddMissile(
				    missile[v2]._mix,
				    missile[v2]._miy,
				    v1,
				    0,
				    missile[v2]._mimfnum,
				    MIS_MISEXP3,
				    _LOBYTE(missile[v2]._micaster),
				    missile[v2]._misource,
				    0,
				    0);
				break;
			case MIS_BONESPIRIT:
				SetMissDir(v1, 8);
				missile[v2]._mirange = 7;
				missile[v2]._miDelFlag = FALSE;
				goto LABEL_39;
			}
		}
		v16 = missile[v2]._mlid;
		if (v16 >= 0)
			AddUnLight(v16);
	}
LABEL_39:
	PutMissile(v1);
}

void __fastcall MI_Lightball(int i)
{
	int tx, ty, j, oi;
	char obj;

	tx = missile[i]._miVar1;
	missile[i]._mitxoff += missile[i]._mixvel;
	ty = missile[i]._miVar2;
	missile[i]._mirange--;
	missile[i]._mityoff += missile[i]._miyvel;
	GetMissilePos(i);
	j = missile[i]._mirange;
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 0, missile[i]._mix, missile[i]._miy, 0);
	if (missile[i]._miHitFlag == TRUE)
		missile[i]._mirange = j;
	obj = dObject[tx][ty];
	if (obj && tx == missile[i]._mix && ty == missile[i]._miy) {
		if (obj > 0) {
			oi = obj - 1;
		} else {
			oi = -1 - obj;
		}
		if (object[oi]._otype == OBJ_SHRINEL || object[oi]._otype == OBJ_SHRINER)
			missile[i]._mirange = j;
	}
	if (!missile[i]._mirange)
		missile[i]._miDelFlag = TRUE;
	PutMissile(i);
}

void __fastcall mi_null_33(int i)
{
	missile[i]._mirange--;
	missile[i]._mitxoff += missile[i]._mixvel;
	missile[i]._mityoff += missile[i]._miyvel;
	GetMissilePos(i);
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 0, missile[i]._mix, missile[i]._miy, 0);
	if (!missile[i]._mirange)
		missile[i]._miDelFlag = TRUE;
	PutMissile(i);
}

void __fastcall MI_Acidpud(int i)
{
	int range;

	missile[i]._mirange--;
	range = missile[i]._mirange;
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 1, missile[i]._mix, missile[i]._miy, 0);
	missile[i]._mirange = range;
	if (!range) {
		if (missile[i]._mimfnum) {
			missile[i]._miDelFlag = TRUE;
		} else {
			SetMissDir(i, 1);
			missile[i]._mirange = missile[i]._miAnimLen;
		}
	}
	PutMissile(i);
}

void __fastcall MI_Firewall(int i)
{
	int ExpLight[14] = { 2, 3, 4, 5, 5, 6, 7, 8, 9, 10, 11, 12, 12 };

	missile[i]._mirange--;
	if (missile[i]._mirange == missile[i]._miVar1) {
		SetMissDir(i, 1);
		missile[i]._miAnimFrame = random(83, 11) + 1;
	}
	if (missile[i]._mirange == missile[i]._miAnimLen - 1) {
		SetMissDir(i, 0);
		missile[i]._miAnimAdd = -1;
		missile[i]._miAnimFrame = 13;
	}
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 1, missile[i]._mix, missile[i]._miy, 1);
	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = TRUE;
		AddUnLight(missile[i]._mlid);
	}
	if (missile[i]._mimfnum && missile[i]._mirange && missile[i]._miAnimAdd != -1 && missile[i]._miVar2 < 12) {
		if (!missile[i]._miVar2)
			missile[i]._mlid = AddLight(missile[i]._mix, missile[i]._miy, ExpLight[0]);
		ChangeLight(missile[i]._mlid, missile[i]._mix, missile[i]._miy, ExpLight[missile[i]._miVar2]);
		missile[i]._miVar2++;
	}
	PutMissile(i);
}

void __fastcall MI_Fireball(int i)
{
	int dam, id, px, py, mx, my;

	dam = missile[i]._midam;
	id = missile[i]._misource;
	missile[i]._mirange--;

	if (missile[i]._micaster == 0) {
		px = plr[id].WorldX;
		py = plr[id].WorldY;
	} else {
		px = monster[id]._mx;
		py = monster[id]._my;
	}

	if (missile[i]._miAnimType == MFILE_BIGEXP) {
		if (!missile[i]._mirange) {
			missile[i]._miDelFlag = TRUE;
			AddUnLight(missile[i]._mlid);
		}
	} else {
		missile[i]._mitxoff += missile[i]._mixvel;
		missile[i]._mityoff += missile[i]._miyvel;
		GetMissilePos(i);
		if (missile[i]._mix != missile[i]._misx || missile[i]._miy != missile[i]._misy)
			CheckMissileCol(i, dam, dam, 0, missile[i]._mix, missile[i]._miy, 0);
		if (!missile[i]._mirange) {
			mx = missile[i]._mix;
			my = missile[i]._miy;
			ChangeLight(missile[i]._mlid, missile[i]._mix, my, missile[i]._miAnimFrame);
			if (!CheckBlock(px, py, mx, my))
				CheckMissileCol(i, dam, dam, 0, mx, my, 1);
			if (!CheckBlock(px, py, mx, my + 1))
				CheckMissileCol(i, dam, dam, 0, mx, my + 1, 1);
			if (!CheckBlock(px, py, mx, my - 1))
				CheckMissileCol(i, dam, dam, 0, mx, my - 1, 1);
			if (!CheckBlock(px, py, mx + 1, my))
				CheckMissileCol(i, dam, dam, 0, mx + 1, my, 1);
			if (!CheckBlock(px, py, mx + 1, my - 1))
				CheckMissileCol(i, dam, dam, 0, mx + 1, my - 1, 1);
			if (!CheckBlock(px, py, mx + 1, my + 1))
				CheckMissileCol(i, dam, dam, 0, mx + 1, my + 1, 1);
			if (!CheckBlock(px, py, mx - 1, my))
				CheckMissileCol(i, dam, dam, 0, mx - 1, my, 1);
			if (!CheckBlock(px, py, mx - 1, my + 1))
				CheckMissileCol(i, dam, dam, 0, mx - 1, my + 1, 1);
			if (!CheckBlock(px, py, mx - 1, my - 1))
				CheckMissileCol(i, dam, dam, 0, mx - 1, my - 1, 1);
			if (!TransList[dTransVal[mx][my]]
			    || (missile[i]._mixvel < 0 && ((TransList[dTransVal[mx][my + 1]] && nSolidTable[dPiece[mx][my + 1]]) || (TransList[dTransVal[mx][my - 1]] && nSolidTable[dPiece[mx][my - 1]])))) {
				missile[i]._mix++;
				missile[i]._miy++;
				missile[i]._miyoff -= 32;
			}
			if (missile[i]._miyvel > 0
			    && (TransList[dTransVal[mx + 1][my]] && nSolidTable[dPiece[mx + 1][my]]
			           || TransList[dTransVal[mx - 1][my]] && nSolidTable[dPiece[mx - 1][my]])) {
				missile[i]._miyoff -= 32;
			}
			if (missile[i]._mixvel > 0
			    && (TransList[dTransVal[mx][my + 1]] && nSolidTable[dPiece[mx][my + 1]]
			           || TransList[dTransVal[mx][my - 1]] && nSolidTable[dPiece[mx][my - 1]])) {
				missile[i]._mixoff -= 32;
			}
			missile[i]._mimfnum = 0;
			SetMissAnim(i, MFILE_BIGEXP);
			missile[i]._mirange = missile[i]._miAnimLen - 1;
		} else if (missile[i]._mix != missile[i]._miVar1 || missile[i]._miy != missile[i]._miVar2) {
			missile[i]._miVar1 = missile[i]._mix;
			missile[i]._miVar2 = missile[i]._miy;
			ChangeLight(missile[i]._mlid, missile[i]._mix, missile[i]._miy, 8);
		}
	}

	PutMissile(i);
}

void __fastcall MI_Lightctrl(int i)
{
	int v1;        // esi
	int v2;        // eax
	int v3;        // eax
	int v5;        // edi
	signed int v6; // ebx
	signed int v7; // edx
	int v8;        // ecx
	int v9;        // eax
	int v10;       // [esp-10h] [ebp-24h]
	int v11;       // [esp-Ch] [ebp-20h]
	int v12;       // [esp-8h] [ebp-1Ch]
	int v13;       // [esp+Ch] [ebp-8h]
	int ia;        // [esp+10h] [ebp-4h]

	ia = i;
	v1 = i;
	v2 = missile[i]._misource;
	--missile[v1]._mirange;
	if (v2 == -1) {
		v5 = random(81, currlevel) + 2 * currlevel;
	} else if (missile[v1]._micaster) {
		v5 = 2
		    * ((unsigned char)monster[v2].mMinDamage
		          + random(80, (unsigned char)monster[v2].mMaxDamage - (unsigned char)monster[v2].mMinDamage + 1));
	} else {
		v3 = random(79, plr[v2]._pLevel);
		v5 = (v3 + random(79, 2) + 2) << 6;
	}
	missile[v1]._mitxoff += missile[v1]._mixvel;
	missile[v1]._mityoff += missile[v1]._miyvel;
	GetMissilePos(ia);
	v6 = missile[v1]._mix;
	v7 = missile[v1]._miy;
	v8 = missile[v1]._misource;
	v13 = missile[v1]._miy;
	v9 = dPiece[missile[v1]._mix][v7];
	if (v8 != -1 || v6 != missile[v1]._misx || v7 != missile[v1]._misy) {
		if (!nMissileTable[v9])
			goto LABEL_12;
		missile[v1]._mirange = 0;
	}
	if (!nMissileTable[v9]) {
	LABEL_12:
		if (v6 == missile[v1]._miVar1 && v7 == missile[v1]._miVar2 || v6 <= 0 || v7 <= 0 || v6 >= MAXDUNX || v7 >= MAXDUNY)
			goto LABEL_27;
		if (v8 == -1) {
			v12 = missile[v1]._mispllvl;
			v11 = v5;
			v10 = -1;
		} else {
			if (missile[v1]._micaster == 1) {
				v9 = (int)monster[v8].MType;
				_LOBYTE(v9) = *(_BYTE *)v9;
				if ((unsigned char)v9 >= MT_STORM && (unsigned char)v9 <= MT_MAEL) {
					_LOBYTE(v9) = missile[v1]._micaster;
					AddMissile(v6, v7, missile[v1]._misx, missile[v1]._misy, ia, MIS_LIGHTNING2, v9, v8, v5, missile[v1]._mispllvl);
				LABEL_26:
					v7 = v13;
					missile[v1]._miVar1 = missile[v1]._mix;
					missile[v1]._miVar2 = missile[v1]._miy;
					goto LABEL_27;
				}
			}
			v12 = missile[v1]._mispllvl;
			v11 = v5;
			v10 = v8;
		}
		_LOBYTE(v9) = missile[v1]._micaster;
		AddMissile(v6, v7, missile[v1]._misx, missile[v1]._misy, ia, MIS_LIGHTNING, v9, v10, v11, v12);
		goto LABEL_26;
	}
LABEL_27:
	if (!missile[v1]._mirange || v6 <= 0 || v7 <= 0 || v6 >= MAXDUNX || v7 > MAXDUNY)
		missile[v1]._miDelFlag = TRUE;
}

void __fastcall MI_Lightning(int i)
{
	int j;

	missile[i]._mirange--;
	j = missile[i]._mirange;
	if (missile[i]._mix != missile[i]._misx || missile[i]._miy != missile[i]._misy)
		CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 1, missile[i]._mix, missile[i]._miy, 0);
	if (missile[i]._miHitFlag == TRUE)
		missile[i]._mirange = j;
	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = TRUE;
		AddUnLight(missile[i]._mlid);
	}
	PutMissile(i);
}

void __fastcall MI_Town(int i)
{
	int ExpLight[17] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 15, 15 };
	int p;

	if (missile[i]._mirange > 1)
		missile[i]._mirange--;
	if (missile[i]._mirange == missile[i]._miVar1)
		SetMissDir(i, 1);
	if (currlevel && missile[i]._mimfnum != 1 && missile[i]._mirange) {
		if (!missile[i]._miVar2)
			missile[i]._mlid = AddLight(missile[i]._mix, missile[i]._miy, 1);
		ChangeLight(missile[i]._mlid, missile[i]._mix, missile[i]._miy, ExpLight[missile[i]._miVar2]);
		missile[i]._miVar2++;
	}

	for (p = 0; p < 4; p++) {
		if (plr[p].plractive && currlevel == plr[p].plrlevel && !plr[p]._pLvlChanging && !plr[p]._pmode && plr[p].WorldX == missile[i]._mix && plr[p].WorldY == missile[i]._miy) {
			ClrPlrPath(p);
			if (p == myplr) {
				NetSendCmdParam1(TRUE, CMD_WARP, missile[i]._misource);
				plr[p]._pmode = 10;
			}
		}
	}

	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = TRUE;
		AddUnLight(missile[i]._mlid);
	}
	PutMissile(i);
}

void __fastcall MI_Flash(int i)
{
	if (!missile[i]._micaster) {
		if (missile[i]._misource != -1)
			plr[missile[i]._misource]._pInvincible = TRUE;
	}
	missile[i]._mirange--;
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 1, missile[i]._mix - 1, missile[i]._miy, 1);
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 1, missile[i]._mix, missile[i]._miy, 1);
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 1, missile[i]._mix + 1, missile[i]._miy, 1);
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 1, missile[i]._mix - 1, missile[i]._miy + 1, 1);
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 1, missile[i]._mix, missile[i]._miy + 1, 1);
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 1, missile[i]._mix + 1, missile[i]._miy + 1, 1);
	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = TRUE;
		if (missile[i]._micaster == 0) {
			if (missile[i]._misource != -1)
				plr[missile[i]._misource]._pInvincible = FALSE;
		}
	}
	PutMissile(i);
}

void __fastcall MI_Flash2(int i)
{
	if (!missile[i]._micaster) {
		if (missile[i]._misource != -1)
			plr[missile[i]._misource]._pInvincible = TRUE;
	}
	missile[i]._mirange--;
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 1, missile[i]._mix - 1, missile[i]._miy - 1, 1);
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 1, missile[i]._mix, missile[i]._miy - 1, 1);
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 1, missile[i]._mix + 1, missile[i]._miy - 1, 1);
	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = TRUE;
		if (missile[i]._micaster == 0) {
			if (missile[i]._misource != -1)
				plr[missile[i]._misource]._pInvincible = FALSE;
		}
	}
	PutMissile(i);
}

void __fastcall MI_Manashield(int i)
{
	int v1;      // edi
	int v2;      // esi
	int v3;      // edx
	int v4;      // eax
	int v5;      // ecx
	int v6;      // edx
	BOOLEAN v7;  // zf
	int v8;      // eax
	int v9;      // ecx
	int v10;     // edx
	int v11;     // ecx
	int v12;     // ecx
	BOOLEAN v13; // sf
	int v14;     // [esp+Ch] [ebp-10h]
	int ia;      // [esp+14h] [ebp-8h]
	int arglist; // [esp+18h] [ebp-4h]

	ia = i;
	v1 = i;
	arglist = missile[i]._misource;
	v2 = arglist;
	v3 = plr[arglist]._pxoff;
	v4 = plr[arglist].WorldX;
	v5 = plr[arglist].WorldY;
	missile[v1]._mix = v4;
	missile[v1]._mitxoff = v3 << 16;
	v6 = plr[arglist]._pyoff << 16;
	v7 = plr[arglist]._pmode == PM_WALK3;
	missile[v1]._miy = v5;
	missile[v1]._mityoff = v6;
	if (v7) {
		missile[v1]._misx = plr[v2]._px;
		missile[v1]._misy = plr[v2]._py;
	} else {
		missile[v1]._misx = v4;
		missile[v1]._misy = v5;
	}
	GetMissilePos(ia);
	if (plr[v2]._pmode == PM_WALK3) {
		if (plr[v2]._pdir == 2)
			++missile[v1]._mix;
		else
			++missile[v1]._miy;
	}
	if (arglist != myplr) {
		if (currlevel != plr[v2].plrlevel)
			missile[v1]._miDelFlag = TRUE;
		goto LABEL_33;
	}
	v8 = plr[v2]._pMana;
	v14 = plr[v2]._pMana;
	if (v8 <= 0 || !plr[v2].plractive)
		missile[v1]._mirange = 0;
	v9 = missile[v1]._miVar1;
	if (plr[v2]._pHitPoints >= v9)
		goto LABEL_26;
	v10 = v9 - plr[v2]._pHitPoints;
	if (missile[v1]._mispllvl > 0) {
		v10 = v10 / -3 + v9 - plr[v2]._pHitPoints;
		v8 = v14;
	}
	if (v10 < 0)
		v10 = 0;
	drawmanaflag = TRUE;
	drawhpflag = TRUE;
	if (v8 >= v10) {
		plr[v2]._pHitPoints = v9;
		v11 = missile[v1]._miVar2;
		plr[v2]._pManaBase -= v10;
		plr[v2]._pHPBase = v11;
		plr[v2]._pMana = v8 - v10;
	LABEL_26:
		if (arglist == myplr && !plr[v2]._pHitPoints && !missile[v1]._miVar1 && plr[v2]._pmode != PM_DEATH) {
			missile[v1]._mirange = 0;
			missile[v1]._miDelFlag = TRUE;
			SyncPlrKill(arglist, -1);
		}
		goto LABEL_31;
	}
	missile[v1]._miDelFlag = TRUE;
	plr[v2]._pHitPoints = v8 + v9 - v10;
	plr[v2]._pHPBase = v8 + missile[v1]._miVar2 - v10;
	v12 = plr[v2]._pMaxManaBase - plr[v2]._pMaxMana;
	v13 = plr[v2]._pHitPoints < 0;
	plr[v2]._pMana = 0;
	missile[v1]._mirange = 0;
	plr[v2]._pManaBase = v12;
	if (v13)
		SetPlayerHitPoints(arglist, 0);
	if (plr[v2]._pHitPoints >> 6)
		goto LABEL_26;
	if (arglist == myplr) {
		SyncPlrKill(arglist, missile[v1]._miVar8);
		goto LABEL_26;
	}
LABEL_31:
	v7 = missile[v1]._mirange == 0;
	missile[v1]._miVar1 = plr[v2]._pHitPoints;
	missile[v1]._miVar2 = plr[v2]._pHPBase;
	if (v7) {
		missile[v1]._miDelFlag = TRUE;
		NetSendCmd(TRUE, CMD_ENDSHIELD);
	}
LABEL_33:
	PutMissile(ia);
}

void __fastcall MI_Etherealize(int i)
{
	int src;

	src = missile[i]._misource;
	missile[i]._mirange--;
	missile[i]._mix = plr[src].WorldX;
	missile[i]._miy = plr[src].WorldY;
	missile[i]._mitxoff = plr[src]._pxoff << 16;
	missile[i]._mityoff = plr[src]._pyoff << 16;
	if (plr[src]._pmode == PM_WALK3) {
		missile[i]._misx = plr[src]._px;
		missile[i]._misy = plr[src]._py;
	} else {
		missile[i]._misx = missile[i]._mix;
		missile[i]._misy = missile[i]._miy;
	}
	GetMissilePos(i);
	if (plr[src]._pmode == PM_WALK3) {
		if (plr[src]._pdir == 2)
			missile[i]._mix++;
		else
			missile[i]._miy++;
	}
	plr[src]._pSpellFlags |= 1;
	if (!missile[i]._mirange || plr[src]._pHitPoints <= 0) {
		missile[i]._miDelFlag = TRUE;
		plr[src]._pSpellFlags &= ~0x1;
	}
	PutMissile(i);
}

void __fastcall MI_Firemove(int i)
{
	int j;
	int ExpLight[14] = { 2, 3, 4, 5, 5, 6, 7, 8, 9, 10, 11, 12, 12 };

	missile[i]._mix--;
	missile[i]._miy--;
	missile[i]._miyoff += 32;
	missile[i]._miVar1++;
	if (missile[i]._miVar1 == missile[i]._miAnimLen) {
		SetMissDir(i, 1);
		missile[i]._miAnimFrame = random(82, 11) + 1;
	}
	missile[i]._mitxoff += missile[i]._mixvel;
	missile[i]._mityoff += missile[i]._miyvel;
	GetMissilePos(i);
	j = missile[i]._mirange;
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 0, missile[i]._mix, missile[i]._miy, 0);
	if (missile[i]._miHitFlag == TRUE)
		missile[i]._mirange = j;
	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = TRUE;
		AddUnLight(missile[i]._mlid);
	}
	if (missile[i]._mimfnum || !missile[i]._mirange) {
		if (missile[i]._mix != missile[i]._miVar3 || missile[i]._miy != missile[i]._miVar4) {
			missile[i]._miVar3 = missile[i]._mix;
			missile[i]._miVar4 = missile[i]._miy;
			ChangeLight(missile[i]._mlid, missile[i]._mix, missile[i]._miy, 8);
		}
	} else {
		if (!missile[i]._miVar2)
			missile[i]._mlid = AddLight(missile[i]._mix, missile[i]._miy, ExpLight[0]);
		ChangeLight(missile[i]._mlid, missile[i]._mix, missile[i]._miy, ExpLight[missile[i]._miVar2]);
		missile[i]._miVar2++;
	}
	missile[i]._mix++;
	missile[i]._miy++;
	missile[i]._miyoff -= 32;
	PutMissile(i);
}

void __fastcall MI_Guardian(int i)
{
	int v1;             // esi
	int v2;             // eax
	int v3;             // ecx
	unsigned char *v4;  // edi
	int v5;             // eax
	signed int v6;      // ecx
	unsigned char *v7;  // ebx
	unsigned char v8;   // dl
	unsigned char *v9;  // edi
	int v10;            // ecx
	int *v11;           // eax
	int v12;            // ecx
	int v13;            // ecx
	signed int v14;     // [esp+Ch] [ebp-14h]
	int v15;            // [esp+10h] [ebp-10h]
	int v16;            // [esp+14h] [ebp-Ch]
	unsigned char *v17; // [esp+18h] [ebp-8h]
	int ia;             // [esp+1Ch] [ebp-4h]

	ia = i;
	v1 = i;
	v2 = missile[i]._miVar2;
	--missile[v1]._mirange;
	v3 = missile[i]._mirange;
	v16 = 0;
	v15 = 0;
	if (v2 > 0)
		missile[v1]._miVar2 = v2 - 1;
	if (v3 == missile[v1]._miVar1 || missile[v1]._mimfnum == 2 && !missile[v1]._miVar2)
		SetMissDir(ia, 1);
	if (!(missile[v1]._mirange % 16)) {
		v4 = &vCrawlTable[0][1];
		v5 = 0;
		v17 = &vCrawlTable[0][1];
		do {
			if (v5 == -1)
				break;
			v6 = 10;
			v14 = 10;
			do {
				v7 = &v4[v6 - 1];
				v8 = *v7;
				if (!*v7 && !v4[v6])
					break;
				if (v16 != v8 || v15 != v4[v6]) {
					v9 = &v4[v6];
					v5 = Sentfire(ia, v8 + missile[v1]._mix, missile[v1]._miy + *v9);
					if (v5 == -1
					    || (v5 = Sentfire(ia, missile[v1]._mix - *v7, missile[v1]._miy - *v9), v5 == -1)
					    || (v5 = Sentfire(ia, missile[v1]._mix + *v7, missile[v1]._miy - *v9), v5 == -1)
					    || (v5 = Sentfire(ia, missile[v1]._mix - *v7, missile[v1]._miy + *v9), v5 == -1)) {
						v4 = v17;
						break;
					}
					v16 = *v7;
					v10 = *v9;
					v4 = v17;
					v15 = v10;
					v6 = v14;
				}
				v6 -= 2;
				v14 = v6;
			} while (v6 >= 0);
			v4 += 30;
			v17 = v4;
		} while ((signed int)v4 < (signed int)&vCrawlTable[23][1]);
	}
	if (missile[v1]._mirange == 14) {
		SetMissDir(ia, 0);
		missile[v1]._miAnimAdd = -1;
		missile[v1]._miAnimFrame = 15;
	}
	v11 = &missile[v1]._miVar3;
	*v11 += missile[v1]._miAnimAdd;
	v12 = missile[v1]._miVar3;
	if (v12 <= 15) {
		if (v12 > 0)
			ChangeLight(missile[v1]._mlid, missile[v1]._mix, missile[v1]._miy, missile[v1]._miVar3);
	} else {
		*v11 = 15;
	}
	if (!missile[v1]._mirange) {
		v13 = missile[v1]._mlid;
		missile[v1]._miDelFlag = TRUE;
		AddUnLight(v13);
	}
	PutMissile(ia);
}

void __fastcall MI_Chain(int i)
{
	int sx, sy, id, l, n, m, k, rad, tx, ty, dir;
	int CrawlNum[19] = { 0, 3, 12, 45, 94, 159, 240, 337, 450, 579, 724, 885, 1062, 1255, 1464, 1689, 1930, 2187, 2460 };

	id = missile[i]._misource;
	sx = missile[i]._mix;
	sy = missile[i]._miy;
	dir = GetDirection(sx, sy, missile[i]._miVar1, missile[i]._miVar2);
	AddMissile(sx, sy, missile[i]._miVar1, missile[i]._miVar2, dir, MIS_LIGHTCTRL, 0, id, 1, missile[i]._mispllvl);
	rad = missile[i]._mispllvl + 3;
	if (rad > 19)
		rad = 19;
	for (m = 1; m < rad; m++) {
		k = CrawlNum[m];
		l = k + 2;
		for (n = (unsigned char)CrawlTable[k]; n > 0; n--) {
			tx = sx + CrawlTable[l - 1];
			ty = sy + CrawlTable[l];
			if (tx > 0 && tx < MAXDUNX && ty > 0 && ty < MAXDUNY && dMonster[tx][ty] > 0) {
				dir = GetDirection(sx, sy, tx, ty);
				AddMissile(sx, sy, tx, ty, dir, MIS_LIGHTCTRL, 0, id, 1, missile[i]._mispllvl);
			}
			l += 2;
		}
	}
	missile[i]._mirange--;
	if (missile[i]._mirange == 0)
		missile[i]._miDelFlag = TRUE;
}

void __fastcall mi_null_11(int i)
{
	missile[i]._mirange--;
	if (!missile[i]._mirange)
		missile[i]._miDelFlag = TRUE;
	if (missile[i]._miAnimFrame == missile[i]._miAnimLen)
		missile[i]._miPreFlag = TRUE;
	PutMissile(i);
}

void __fastcall MI_Weapexp(int i)
{
	int id, mind, maxd;
	int ExpLight[10] = { 9, 10, 11, 12, 11, 10, 8, 6, 4, 2 };

	missile[i]._mirange--;
	id = missile[i]._misource;
	if (missile[i]._miVar2 == 1) {
		mind = plr[id]._pIFMinDam;
		maxd = plr[id]._pIFMaxDam;
		missiledata[missile[i]._mitype].mResist = MISR_FIRE;
	} else {
		mind = plr[id]._pILMinDam;
		maxd = plr[id]._pILMaxDam;
		missiledata[missile[i]._mitype].mResist = MISR_LIGHTNING;
	}
	CheckMissileCol(i, mind, maxd, 0, missile[i]._mix, missile[i]._miy, 0);
	if (!missile[i]._miVar1) {
		missile[i]._mlid = AddLight(missile[i]._mix, missile[i]._miy, 9);
	} else {
		if (missile[i]._mirange)
			ChangeLight(missile[i]._mlid, missile[i]._mix, missile[i]._miy, ExpLight[missile[i]._miVar1]);
	}
	missile[i]._miVar1++;
	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = TRUE;
		AddUnLight(missile[i]._mlid);
	} else {
		PutMissile(i);
	}
}

void __fastcall MI_Misexp(int i)
{
	int ExpLight[10] = { 9, 10, 11, 12, 11, 10, 8, 6, 4, 2 };

	missile[i]._mirange--;
	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = TRUE;
		AddUnLight(missile[i]._mlid);
	} else {
		if (!missile[i]._miVar1)
			missile[i]._mlid = AddLight(missile[i]._mix, missile[i]._miy, 9);
		else
			ChangeLight(missile[i]._mlid, missile[i]._mix, missile[i]._miy, ExpLight[missile[i]._miVar1]);
		missile[i]._miVar1++;
		PutMissile(i);
	}
}

void __fastcall MI_Acidsplat(int i)
{
	int monst, dam;

	if (missile[i]._mirange == missile[i]._miAnimLen) {
		missile[i]._mix++;
		missile[i]._miy++;
		missile[i]._miyoff -= 32;
	}
	missile[i]._mirange--;
	if (!missile[i]._mirange) {
		monst = missile[i]._misource;
		dam = missile[i]._mispllvl;
		missile[i]._miDelFlag = TRUE;
		AddMissile(missile[i]._mix, missile[i]._miy, i, 0, missile[i]._mimfnum, MIS_ACIDPUD, 1, monst, (monster[monst].MData->mLevel >= 2) + 1, dam);
	} else {
		PutMissile(i);
	}
}

void __fastcall MI_Teleport(int i)
{
	int id;

	id = missile[i]._misource;
	missile[i]._mirange--;
	if (missile[i]._mirange <= 0) {
		missile[i]._miDelFlag = TRUE;
	} else {
		dPlayer[plr[id].WorldX][plr[id].WorldY] = 0;
		PlrClrTrans(plr[id].WorldX, plr[id].WorldY);
		plr[id].WorldX = missile[i]._mix;
		plr[id].WorldY = missile[i]._miy;
		plr[id]._px = missile[i]._mix;
		plr[id]._py = missile[i]._miy;
		plr[id]._poldx = missile[i]._mix;
		plr[id]._poldy = missile[i]._miy;
		PlrDoTrans(missile[i]._mix, missile[i]._miy);
		missile[i]._miVar1 = 1;
		dPlayer[plr[id].WorldX][plr[id].WorldY] = id + 1;
		if (leveltype != DTYPE_TOWN) {
			ChangeLightXY(plr[id]._plid, plr[id].WorldX, plr[id].WorldY);
			ChangeVisionXY(plr[id]._pvid, plr[id].WorldX, plr[id].WorldY);
		}
		if (id == myplr) {
			ViewX = plr[id].WorldX - ScrollInfo._sdx;
			ViewY = plr[id].WorldY - ScrollInfo._sdy;
		}
	}
}

void __fastcall MI_Stone(int i)
{
	int m;

	missile[i]._mirange--;
	m = missile[i]._miVar2;
	if (!monster[m]._mhitpoints && missile[i]._miAnimType != MFILE_SHATTER1) {
		missile[i]._mimfnum = 0;
		missile[i]._miDrawFlag = TRUE;
		SetMissAnim(i, MFILE_SHATTER1);
		missile[i]._mirange = 11;
	}
	if (monster[m]._mmode != MM_STONE) {
		missile[i]._miDelFlag = TRUE;
	} else {
		if (!missile[i]._mirange) {
			missile[i]._miDelFlag = TRUE;
			if (monster[m]._mhitpoints > 0)
				monster[m]._mmode = missile[i]._miVar1;
			else
				AddDead(monster[m]._mx, monster[m]._my, stonendx, (direction)monster[m]._mdir);
		}
		if (missile[i]._miAnimType == MFILE_SHATTER1)
			PutMissile(i);
	}
}

void __fastcall MI_Boom(int i)
{
	missile[i]._mirange--;
	if (!missile[i]._miVar1)
		CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 0, missile[i]._mix, missile[i]._miy, 1);
	if (missile[i]._miHitFlag == TRUE)
		missile[i]._miVar1 = 1;
	if (!missile[i]._mirange)
		missile[i]._miDelFlag = TRUE;
	PutMissile(i);
}

void __fastcall MI_Rhino(int i)
{
	int mix, miy, mix2, miy2, omx, omy, monst;

	monst = missile[i]._misource;
	if (monster[monst]._mmode != MM_CHARGE) {
		missile[i]._miDelFlag = TRUE;
		return;
	}
	GetMissilePos(i);
	mix = missile[i]._mix;
	miy = missile[i]._miy;
	dMonster[mix][miy] = 0;
	if (monster[monst]._mAi == AI_SNAKE) {
		missile[i]._mitxoff += 2 * missile[i]._mixvel;
		missile[i]._mityoff += 2 * missile[i]._miyvel;
		GetMissilePos(i);
		mix2 = missile[i]._mix;
		miy2 = missile[i]._miy;
		missile[i]._mitxoff -= missile[i]._mixvel;
		missile[i]._mityoff -= missile[i]._miyvel;
	} else {
		missile[i]._mitxoff += missile[i]._mixvel;
		missile[i]._mityoff += missile[i]._miyvel;
	}
	GetMissilePos(i);
	omx = missile[i]._mix;
	omy = missile[i]._miy;
	if (!PosOkMonst(monst, missile[i]._mix, missile[i]._miy) || (monster[monst]._mAi == AI_SNAKE && !PosOkMonst(monst, mix2, miy2))) {
		MissToMonst(i, mix, miy);
		missile[i]._miDelFlag = TRUE;
		return;
	}
	monster[monst]._mfutx = omx;
	monster[monst]._moldx = omx;
	dMonster[omx][omy] = -1 - monst;
	monster[monst]._mx = omx;
	monster[monst]._mfuty = omy;
	monster[monst]._moldy = omy;
	monster[monst]._my = omy;
	if (monster[monst]._uniqtype != 0)
		ChangeLightXY(missile[i]._mlid, omx, omy);
	MoveMissilePos(i);
	PutMissile(i);
}

void __fastcall mi_null_32(int i)
{
	int src, enemy, ax, ay, bx, by, cx, cy, j;

	GetMissilePos(i);
	ax = missile[i]._mix;
	ay = missile[i]._miy;
	missile[i]._mitxoff += missile[i]._mixvel;
	missile[i]._mityoff += missile[i]._miyvel;
	GetMissilePos(i);
	src = missile[i]._misource;
	bx = missile[i]._mix;
	by = missile[i]._miy;
	enemy = monster[src]._menemy;
	if (!(monster[src]._mFlags & MFLAG_TARGETS_MONSTER)) {
		cx = plr[enemy].WorldX;
		cy = plr[enemy].WorldY;
	} else {
		cx = monster[enemy]._mx;
		cy = monster[enemy]._my;
	}
	if ((bx != ax || by != ay) && (missile[i]._miVar1 & 1 && (abs(ax - cx) >= 4 || abs(ay - cy) >= 4) || missile[i]._miVar2 > 1) && PosOkMonst(missile[i]._misource, ax, ay)) {
		MissToMonst(i, ax, ay);
		missile[i]._miDelFlag = TRUE;
	} else if (!(monster[src]._mFlags & MFLAG_TARGETS_MONSTER)) {
		j = dPlayer[bx][by];
	} else {
		j = dMonster[bx][by];
	}
	if (!PosOkMissile(bx, by) || j > 0 && !(missile[i]._miVar1 & 1)) {
		missile[i]._mixvel *= -1;
		missile[i]._miyvel *= -1;
		missile[i]._mimfnum = opposite[missile[i]._mimfnum];
		missile[i]._miAnimData = monster[src].MType->Anims[MA_WALK].Data[missile[i]._mimfnum];
		missile[i]._miVar2++;
		if (j > 0)
			missile[i]._miVar1 |= 1;
	}
	MoveMissilePos(i);
	PutMissile(i);
}

void __fastcall MI_FirewallC(int i)
{
	int tx, ty, id;

	id = missile[i]._misource;
	missile[i]._mirange--;
	if (missile[i]._mirange == 0) {
		missile[i]._miDelFlag = TRUE;
	} else {
		tx = missile[i]._miVar1 + XDirAdd[missile[i]._miVar3];
		ty = missile[i]._miVar2 + YDirAdd[missile[i]._miVar3];
		if (!nMissileTable[dPiece[missile[i]._miVar1][missile[i]._miVar2]] && !missile[i]._miVar8 && tx > 0 && tx < MAXDUNX && ty > 0 && ty < MAXDUNY) {
			AddMissile(missile[i]._miVar1, missile[i]._miVar2, missile[i]._miVar1, missile[i]._miVar2, plr[id]._pdir, MIS_FIREWALL, 0, id, 0, missile[i]._mispllvl);
			missile[i]._miVar1 = tx;
			missile[i]._miVar2 = ty;
		} else {
			missile[i]._miVar8 = 1;
		}
		tx = missile[i]._miVar5 + XDirAdd[missile[i]._miVar4];
		ty = missile[i]._miVar6 + YDirAdd[missile[i]._miVar4];
		if (!nMissileTable[dPiece[missile[i]._miVar5][missile[i]._miVar6]] && !missile[i]._miVar7 && tx > 0 && tx < MAXDUNX && ty > 0 && ty < MAXDUNY) {
			AddMissile(missile[i]._miVar5, missile[i]._miVar6, missile[i]._miVar5, missile[i]._miVar6, plr[id]._pdir, MIS_FIREWALL, 0, id, 0, missile[i]._mispllvl);
			missile[i]._miVar5 = tx;
			missile[i]._miVar6 = ty;
		} else {
			missile[i]._miVar7 = 1;
		}
	}
}

void __fastcall MI_Infra(int i)
{
	missile[i]._mirange--;
	plr[missile[i]._misource]._pInfraFlag = TRUE;
	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = TRUE;
		CalcPlrItemVals(missile[i]._misource, TRUE);
	}
}

void __fastcall MI_Apoca(int i)
{
	int j, k, id;
	BOOL exit;

	id = missile[i]._misource;
	exit = FALSE;
	for (j = missile[i]._miVar2; j < missile[i]._miVar3 && !exit; j++) {
		for (k = missile[i]._miVar4; k < missile[i]._miVar5 && !exit; k++) {
			if (dMonster[k][j] > 3 && !nSolidTable[dPiece[k][j]]) {
				AddMissile(k, j, k, j, plr[id]._pdir, MIS_BOOM, 0, id, missile[i]._midam, 0);
				exit = TRUE;
			}
		}
		if (!exit) {
			missile[i]._miVar4 = missile[i]._miVar6;
		}
	}

	if (exit == TRUE) {
		missile[i]._miVar2 = j - 1;
		missile[i]._miVar4 = k;
	} else {
		missile[i]._miDelFlag = TRUE;
	}
}

void __fastcall MI_Wave(int i)
{
	int v1;         // esi
	int v2;         // ebx
	int v3;         // eax
	int v4;         // edi
	int v5;         // ecx
	int v6;         // eax
	int v7;         // ebx
	int v8;         // eax
	int v9;         // ebx
	int v10;        // eax
	int v11;        // ebx
	BOOLEAN v12;    // zf
	int v13;        // [esp+Ch] [ebp-2Ch]
	int v14;        // [esp+10h] [ebp-28h]
	int v15;        // [esp+14h] [ebp-24h]
	int v16;        // [esp+14h] [ebp-24h]
	signed int v17; // [esp+18h] [ebp-20h]
	int *v18;       // [esp+1Ch] [ebp-1Ch]
	signed int v19; // [esp+20h] [ebp-18h]
	int v20;        // [esp+24h] [ebp-14h]
	int v21;        // [esp+24h] [ebp-14h]
	int v22;        // [esp+28h] [ebp-10h]
	int j;          // [esp+28h] [ebp-10h]
	int id;         // [esp+2Ch] [ebp-Ch]
	int sx;         // [esp+30h] [ebp-8h]
	int sy;         // [esp+34h] [ebp-4h]
	int sya;        // [esp+34h] [ebp-4h]

	v19 = 0;
	v1 = i;
	v17 = 0;
	v2 = missile[i]._mix;
	id = missile[i]._misource;
	v14 = v2;
	v20 = missile[i]._miy;
	v3 = GetDirection(v2, v20, missile[i]._miVar1, missile[i]._miVar2);
	v22 = ((_BYTE)v3 - 2) & 7;
	v4 = v3;
	v15 = ((_BYTE)v3 + 2) & 7;
	v5 = YDirAdd[v3];
	v6 = XDirAdd[v3];
	v7 = v6 + v2;
	sy = v5 + v20;
	if (!nMissileTable[dPiece[v7][v5 + v20]]) {
		v18 = &plr[id]._pdir;
		AddMissile(v7, sy, v7 + v6, sy + v5, *v18, MIS_FIREMOVE, 0, id, 0, missile[v1]._mispllvl);
		v13 = v22;
		sya = YDirAdd[v22] + sy;
		v8 = v15;
		sx = XDirAdd[v22] + v7;
		v16 = v8 * 4;
		v9 = XDirAdd[v8];
		v10 = v20 + YDirAdd[v4] + YDirAdd[v8];
		v11 = v14 + XDirAdd[v4] + v9;
		v21 = 0;
		for (j = v10; v21 < (missile[v1]._mispllvl >> 1) + 2; ++v21) {
			if (nMissileTable[dPiece[sx][sya]] || v19 || sx <= 0 || sx >= MAXDUNX || sya <= 0 || sya >= MAXDUNY) {
				v19 = 1;
			} else {
				AddMissile(sx, sya, sx + XDirAdd[v4], sya + YDirAdd[v4], *v18, MIS_FIREMOVE, 0, id, 0, missile[v1]._mispllvl);
				sx += XDirAdd[v13];
				sya += YDirAdd[v13];
				v10 = j;
			}
			if (nMissileTable[dPiece[v11][v10]] || v17 || v11 <= 0 || v11 >= MAXDUNX || v10 <= 0 || v10 >= MAXDUNY) {
				v17 = 1;
			} else {
				AddMissile(v11, v10, v11 + XDirAdd[v4], v10 + YDirAdd[v4], *v18, MIS_FIREMOVE, 0, id, 0, missile[v1]._mispllvl);
				v11 += *(int *)((char *)XDirAdd + v16);
				j += *(int *)((char *)YDirAdd + v16);
				v10 = j;
			}
		}
	}
	v12 = missile[v1]._mirange-- == 1;
	if (v12)
		missile[v1]._miDelFlag = TRUE;
}

void __fastcall MI_Nova(int i)
{
	int k, id, sx, sy, dir, en, sx1, sy1, dam;

	sx1 = 0;
	sy1 = 0;
	id = missile[i]._misource;
	dam = missile[i]._midam;
	sx = missile[i]._mix;
	sy = missile[i]._miy;
	if (id != -1) {
		en = 0;
		dir = plr[id]._pdir;
	} else {
		dir = 0;
		en = 1;
	}
	for (k = 0; k < 23; k++) {
		if (sx1 != vCrawlTable[k][6] || sy1 != vCrawlTable[k][7]) {
			AddMissile(sx, sy, sx + vCrawlTable[k][6], sy + vCrawlTable[k][7], dir, MIS_LIGHTBALL, en, id, dam, missile[i]._mispllvl);
			AddMissile(sx, sy, sx - vCrawlTable[k][6], sy - vCrawlTable[k][7], dir, MIS_LIGHTBALL, en, id, dam, missile[i]._mispllvl);
			AddMissile(sx, sy, sx - vCrawlTable[k][6], sy + vCrawlTable[k][7], dir, MIS_LIGHTBALL, en, id, dam, missile[i]._mispllvl);
			AddMissile(sx, sy, sx + vCrawlTable[k][6], sy - vCrawlTable[k][7], dir, MIS_LIGHTBALL, en, id, dam, missile[i]._mispllvl);
			sx1 = vCrawlTable[k][6];
			sy1 = vCrawlTable[k][7];
		}
	}
	missile[i]._mirange--;
	if (missile[i]._mirange == 0)
		missile[i]._miDelFlag = TRUE;
}

void __fastcall MI_Blodboil(int i)
{
	missile[i]._miDelFlag = TRUE;
}

void __fastcall MI_Flame(int i)
{
	int k;

	missile[i]._mirange--;
	k = missile[i]._mirange;
	missile[i]._miVar2--;
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 1, missile[i]._mix, missile[i]._miy, 0);
	if (missile[i]._mirange == 0 && missile[i]._miHitFlag == TRUE)
		missile[i]._mirange = k;
	if (!missile[i]._miVar2)
		missile[i]._miAnimFrame = 20;
	if (missile[i]._miVar2 <= 0) {
		k = missile[i]._miAnimFrame;
		if (k > 11)
			k = 24 - k;
		ChangeLight(missile[i]._mlid, missile[i]._mix, missile[i]._miy, k);
	}
	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = TRUE;
		AddUnLight(missile[i]._mlid);
	}
	if (missile[i]._miVar2 <= 0)
		PutMissile(i);
}

void __fastcall MI_Flamec(int i)
{
	int id, src;

	missile[i]._mirange--;
	missile[i]._mitxoff += missile[i]._mixvel;
	src = missile[i]._misource;
	missile[i]._mityoff += missile[i]._miyvel;
	GetMissilePos(i);
	if (missile[i]._mix != missile[i]._miVar1 || missile[i]._miy != missile[i]._miVar2) {
		id = dPiece[missile[i]._mix][missile[i]._miy];
		if (!nMissileTable[id]) {
			AddMissile(
			    missile[i]._mix,
			    missile[i]._miy,
			    missile[i]._misx,
			    missile[i]._misy,
			    i,
			    MIS_FLAME,
			    missile[i]._micaster,
			    src,
			    missile[i]._miVar3,
			    missile[i]._mispllvl);
		} else {
			missile[i]._mirange = 0;
		}
		missile[i]._miVar1 = missile[i]._mix;
		missile[i]._miVar2 = missile[i]._miy;
		missile[i]._miVar3++;
	}
	if (!missile[i]._mirange || missile[i]._miVar3 == 3)
		missile[i]._miDelFlag = TRUE;
}

void __fastcall MI_Cbolt(int i)
{
	int md;
	int bpath[16] = { -1, 0, 1, -1, 0, 1, -1, -1, 0, 0, 1, 1, 0, 1, -1, 0 };

	missile[i]._mirange--;
	if (missile[i]._miAnimType != MFILE_LGHNING) {
		if (!missile[i]._miVar3) {
			md = (missile[i]._miVar2 + bpath[missile[i]._mirnd]) & 7;
			missile[i]._mirnd = (missile[i]._mirnd + 1) & 0xF;
			GetMissileVel(i, missile[i]._mix, missile[i]._miy, missile[i]._mix + XDirAdd[md], missile[i]._miy + YDirAdd[md], 8);
			missile[i]._miVar3 = 16;
		} else {
			missile[i]._miVar3--;
		}
		missile[i]._mitxoff += missile[i]._mixvel;
		missile[i]._mityoff += missile[i]._miyvel;
		GetMissilePos(i);
		CheckMissileCol(i, missile[i]._midam, missile[i]._midam, 0, missile[i]._mix, missile[i]._miy, 0);
		if (missile[i]._miHitFlag == TRUE) {
			missile[i]._miVar1 = 8;
			missile[i]._mimfnum = 0;
			missile[i]._mixoff = 0;
			missile[i]._miyoff = 0;
			SetMissAnim(i, MFILE_LGHNING);
			missile[i]._mirange = missile[i]._miAnimLen;
			GetMissilePos(i);
		}
		ChangeLight(missile[i]._mlid, missile[i]._mix, missile[i]._miy, missile[i]._miVar1);
	}
	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = TRUE;
		AddUnLight(missile[i]._mlid);
	}
	PutMissile(i);
}

void __fastcall MI_Hbolt(int i)
{
	int dam;

	missile[i]._mirange--;
	if (missile[i]._miAnimType != MFILE_HOLYEXPL) {
		missile[i]._mitxoff += missile[i]._mixvel;
		missile[i]._mityoff += missile[i]._miyvel;
		GetMissilePos(i);
		dam = missile[i]._midam;
		if (missile[i]._mix != missile[i]._misx || missile[i]._miy != missile[i]._misy) {
			CheckMissileCol(i, dam, dam, 0, missile[i]._mix, missile[i]._miy, 0);
		}
		if (!missile[i]._mirange) {
			missile[i]._mitxoff -= missile[i]._mixvel;
			missile[i]._mityoff -= missile[i]._miyvel;
			GetMissilePos(i);
			missile[i]._mimfnum = 0;
			SetMissAnim(i, MFILE_HOLYEXPL);
			missile[i]._mirange = missile[i]._miAnimLen - 1;
		} else {
			if (missile[i]._mix != missile[i]._miVar1 || missile[i]._miy != missile[i]._miVar2) {
				missile[i]._miVar1 = missile[i]._mix;
				missile[i]._miVar2 = missile[i]._miy;
				ChangeLight(missile[i]._mlid, missile[i]._mix, missile[i]._miy, 8);
			}
		}
	} else {
		ChangeLight(missile[i]._mlid, missile[i]._mix, missile[i]._miy, missile[i]._miAnimFrame + 7);
		if (!missile[i]._mirange) {
			missile[i]._miDelFlag = TRUE;
			AddUnLight(missile[i]._mlid);
		}
	}
	PutMissile(i);
}

void __fastcall MI_Element(int i)
{
	int mid, sd, dam, cx, cy, px, py, id;

	missile[i]._mirange--;
	dam = missile[i]._midam;
	id = missile[i]._misource;
	if (missile[i]._miAnimType == MFILE_BIGEXP) {
		cx = missile[i]._mix;
		cy = missile[i]._miy;
		px = plr[id].WorldX;
		py = plr[id].WorldY;
		ChangeLight(missile[i]._mlid, cx, cy, missile[i]._miAnimFrame);
		if (!CheckBlock(px, py, cx, cy))
			CheckMissileCol(i, dam, dam, 1, cx, cy, 1);
		if (!CheckBlock(px, py, cx, cy + 1))
			CheckMissileCol(i, dam, dam, 1, cx, cy + 1, 1);
		if (!CheckBlock(px, py, cx, cy - 1))
			CheckMissileCol(i, dam, dam, 1, cx, cy - 1, 1);
		if (!CheckBlock(px, py, cx + 1, cy))
			CheckMissileCol(i, dam, dam, 1, cx + 1, cy, 1); /* check x/y */
		if (!CheckBlock(px, py, cx + 1, cy - 1))
			CheckMissileCol(i, dam, dam, 1, cx + 1, cy - 1, 1);
		if (!CheckBlock(px, py, cx + 1, cy + 1))
			CheckMissileCol(i, dam, dam, 1, cx + 1, cy + 1, 1);
		if (!CheckBlock(px, py, cx - 1, cy))
			CheckMissileCol(i, dam, dam, 1, cx - 1, cy, 1);
		if (!CheckBlock(px, py, cx - 1, cy + 1))
			CheckMissileCol(i, dam, dam, 1, cx - 1, cy + 1, 1);
		if (!CheckBlock(px, py, cx - 1, cy - 1))
			CheckMissileCol(i, dam, dam, 1, cx - 1, cy - 1, 1);
		if (!missile[i]._mirange) {
			missile[i]._miDelFlag = TRUE;
			AddUnLight(missile[i]._mlid);
		}
	} else {
		missile[i]._mitxoff += missile[i]._mixvel;
		missile[i]._mityoff += missile[i]._miyvel;
		GetMissilePos(i);
		cx = missile[i]._mix;
		cy = missile[i]._miy;
		CheckMissileCol(i, dam, dam, 0, cx, cy, 0);
		if (!missile[i]._miVar3 && cx == missile[i]._miVar4 && cy == missile[i]._miVar5)
			missile[i]._miVar3 = 1;
		if (missile[i]._miVar3 == 1) {
			missile[i]._miVar3 = 2;
			missile[i]._mirange = 255;
			mid = FindClosest(cx, cy, 19);
			if (mid > 0) {
				SetMissDir(i, GetDirection8(cx, cy, monster[mid]._mx, monster[mid]._my));
				GetMissileVel(i, cx, cy, monster[mid]._mx, monster[mid]._my, 16);
			} else {
				sd = plr[id]._pdir;
				SetMissDir(i, sd);
				GetMissileVel(i, cx, cy, cx + XDirAdd[sd], cy + YDirAdd[sd], 16);
			}
		}
		if (cx != missile[i]._miVar1 || cy != missile[i]._miVar2) {
			missile[i]._miVar1 = cx;
			missile[i]._miVar2 = cy;
			ChangeLight(missile[i]._mlid, cx, cy, 8);
		}
		if (!missile[i]._mirange) {
			missile[i]._mimfnum = 0;
			SetMissAnim(i, MFILE_BIGEXP);
			missile[i]._mirange = missile[i]._miAnimLen - 1;
		}
	}
	PutMissile(i);
}

void __fastcall MI_Bonespirit(int i)
{
	int v1;     // ebx
	int v2;     // esi
	int v3;     // eax
	int v4;     // ecx
	int v5;     // ecx
	int v6;     // edi
	int v7;     // ebx
	int v8;     // eax
	int v9;     // edi
	int v10;    // ST14_4
	int v11;    // ST10_4
	int v12;    // eax
	int v13;    // ST24_4
	int v14;    // ecx
	int v16;    // [esp+Ch] [ebp-10h]
	int maxdam; // [esp+10h] [ebp-Ch]
	int y1;     // [esp+14h] [ebp-8h]
	int ia;     // [esp+18h] [ebp-4h]

	v1 = i;
	v2 = i;
	ia = i;
	v3 = missile[i]._midam;
	--missile[v2]._mirange;
	maxdam = v3;
	v16 = missile[i]._misource;
	if (missile[i]._mimfnum == 8) {
		ChangeLight(missile[v2]._mlid, missile[v2]._mix, missile[v2]._miy, missile[v2]._miAnimFrame);
		if (!missile[v2]._mirange) {
			v4 = missile[v2]._mlid;
			missile[v2]._miDelFlag = TRUE;
			AddUnLight(v4);
		}
		v5 = v1;
	} else {
		missile[v2]._mitxoff += missile[v2]._mixvel;
		missile[v2]._mityoff += missile[v2]._miyvel;
		GetMissilePos(i);
		v6 = missile[v2]._miy;
		v7 = missile[v2]._mix;
		y1 = missile[v2]._miy;
		CheckMissileCol(ia, maxdam, maxdam, 0, missile[v2]._mix, v6, 0);
		if (!missile[v2]._miVar3 && v7 == missile[v2]._miVar4 && v6 == missile[v2]._miVar5)
			missile[v2]._miVar3 = 1;
		if (missile[v2]._miVar3 == 1) {
			missile[v2]._miVar3 = 2;
			missile[v2]._mirange = 255;
			v8 = FindClosest(v7, v6, 19);
			if (v8 <= 0) {
				v13 = plr[v16]._pdir;
				SetMissDir(ia, v13);
				GetMissileVel(ia, v7, v6, v7 + XDirAdd[v13], v6 + YDirAdd[v13], 16);
			} else {
				v9 = v8;
				v10 = monster[v8]._my;
				v11 = monster[v8]._mx;
				missile[v2]._midam = monster[v8]._mhitpoints >> 7;
				v12 = GetDirection8(v7, y1, v11, v10);
				SetMissDir(ia, v12);
				GetMissileVel(ia, v7, y1, monster[v9]._mx, monster[v9]._my, 16);
				v6 = y1;
			}
		}
		if (v7 != missile[v2]._miVar1 || v6 != missile[v2]._miVar2) {
			v14 = missile[v2]._mlid;
			missile[v2]._miVar1 = v7;
			missile[v2]._miVar2 = v6;
			ChangeLight(v14, v7, v6, 8);
		}
		if (!missile[v2]._mirange) {
			SetMissDir(ia, 8);
			missile[v2]._mirange = 7;
		}
		v5 = ia;
	}
	PutMissile(v5);
}

void __fastcall MI_ResurrectBeam(int i)
{
	missile[i]._mirange--;
	if (missile[i]._mirange == 0)
		missile[i]._miDelFlag = TRUE;
	PutMissile(i);
}

void __fastcall MI_Rportal(int i)
{
	int ExpLight[17] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 15, 15 };

	if (missile[i]._mirange > 1)
		missile[i]._mirange--;
	if (missile[i]._mirange == missile[i]._miVar1)
		SetMissDir(i, 1);

	if (currlevel && missile[i]._mimfnum != 1 && missile[i]._mirange != 0) {
		if (!missile[i]._miVar2)
			missile[i]._mlid = AddLight(missile[i]._mix, missile[i]._miy, 1);
		ChangeLight(missile[i]._mlid, missile[i]._mix, missile[i]._miy, ExpLight[missile[i]._miVar2]);
		missile[i]._miVar2++;
	}
	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = TRUE;
		AddUnLight(missile[i]._mlid);
	}
	PutMissile(i);
}

void __cdecl ProcessMissiles()
{
	int i, mi;

	for (i = 0; i < nummissiles; i++) {
		dFlags[missile[missileactive[i]]._mix][missile[missileactive[i]]._miy] &= ~DFLAG_MISSILE;
		dMissile[missile[missileactive[i]]._mix][missile[missileactive[i]]._miy] = 0;
	}

	i = 0;
	while (i < nummissiles) {
		if (missile[missileactive[i]]._miDelFlag) {
			DeleteMissile(missileactive[i], i);
			i = 0;
		} else {
			i++;
		}
	}

	MissilePreFlag = 0;
	ManashieldFlag = 0;

	for (i = 0; i < nummissiles; i++) {
		mi = missileactive[i];
		missiledata[missile[mi]._mitype].mProc(missileactive[i]);
		if (!(missile[mi]._miAnimFlags & MFLAG_LOCK_ANIMATION)) {
			missile[mi]._miAnimCnt++;
			if (missile[mi]._miAnimCnt >= missile[mi]._miAnimDelay) {
				missile[mi]._miAnimCnt = 0;
				missile[mi]._miAnimFrame += missile[mi]._miAnimAdd;
				if (missile[mi]._miAnimFrame > missile[mi]._miAnimLen)
					missile[mi]._miAnimFrame = 1;
				if (missile[mi]._miAnimFrame < 1)
					missile[mi]._miAnimFrame = missile[mi]._miAnimLen;
			}
		}
	}

	if (ManashieldFlag) {
		for (i = 0; i < nummissiles; i++) {
			if (missile[missileactive[i]]._mitype == MIS_MANASHIELD) {
				MI_Manashield(missileactive[i]);
			}
		}
	}

	i = 0;
	while (i < nummissiles) {
		if (missile[missileactive[i]]._miDelFlag) {
			DeleteMissile(missileactive[i], i);
			i = 0;
		} else {
			i++;
		}
	}
}
// 64CCD4: using guessed type int MissilePreFlag;

void __cdecl missiles_process_charge()
{
	CMonster *mon;
	AnimStruct *anim;
	MissileStruct *mis;
	int i, mi;

	for (i = 0; i < nummissiles; i++) {
		mi = missileactive[i];
		mis = &missile[mi];
		mis->_miAnimData = misfiledata[mis->_miAnimType].mAnimData[mis->_mimfnum];
		if (mis->_mitype == MIS_RHINO) {
			mon = monster[mis->_misource].MType;
			if (mon->mtype >= MT_HORNED && mon->mtype <= MT_OBLORD) {
				anim = &mon->Anims[MA_SPECIAL];
			} else {
				if (mon->mtype >= MT_NSNAKE && mon->mtype <= MT_GSNAKE)
					anim = &mon->Anims[MA_ATTACK];
				else
					anim = &mon->Anims[MA_WALK];
			}
			missile[mi]._miAnimData = anim->Data[mis->_mimfnum];
		}
	}
}

void __fastcall ClearMissileSpot(int mi)
{
	dFlags[missile[mi]._mix][missile[mi]._miy] &= ~DFLAG_MISSILE;
	dMissile[missile[mi]._mix][missile[mi]._miy] = 0;
}

DEVILUTION_END_NAMESPACE
