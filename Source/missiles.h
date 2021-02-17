/**
 * @file missiles.h
 *
 * Interface of missile functionality.
 */
#ifndef __MISSILES_H__
#define __MISSILES_H__

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

extern int missileactive[MAXMISSILES];
extern int missileavail[MAXMISSILES];
extern MissileStruct missile[MAXMISSILES];
extern int nummissiles;
extern BOOL MissilePreFlag;

void GetDamageAmt(int i, int *mind, int *maxd);
int GetSpellLevel(int id, int sn);
void DeleteMissile(int mi, int i);
BOOL MonsterTrapHit(int m, int mindam, int maxdam, int dist, int t, BOOLEAN shift);
BOOL PlayerMHit(int pnum, int m, int dist, int mind, int maxd, int mtype, BOOLEAN shift, int earflag, BOOLEAN *blocked);
void SetMissAnim(int mi, int animtype);
void SetMissDir(int mi, int dir);
void LoadMissileGFX(BYTE mi);
void InitMissileGFX();
void FreeMissiles();
void FreeMissiles2();
void InitMissiles();
void missiles_hive_explosion(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void missiles_fire_rune(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void missiles_light_rune(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void missiles_great_light_rune(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void missiles_immolation_rune(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void missiles_stone_rune(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void missiles_reflection(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void missiles_berserk(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void missiles_430624(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void missiles_jester(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void missiles_steal_pots(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void missiles_mana_trap(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void missiles_spec_arrow(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void missiles_warp(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void missiles_light_wall(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void missiles_rune_explosion(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void missiles_immo_1(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void missiles_immo_2(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void missiles_larrow(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void missiles_43303D(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void missiles_433040(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void missiles_rech_mana(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void missiles_magi(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void missiles_ring(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void missiles_search(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void missiles_cbolt_arrow(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void missiles_hbolt_arrow(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void AddLArrow(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void AddArrow(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void AddRndTeleport(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void AddFirebolt(int mi, int sx, int sy, int dx, int dy, int midir, char micaster, int id, int dam);
void AddMagmaball(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void miss_null_33(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void AddTeleport(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void AddLightball(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void AddFirewall(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void AddFireball(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void AddLightctrl(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void AddLightning(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void AddMisexp(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void AddWeapexp(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void AddTown(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void AddFlash(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void AddFlash2(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void AddManashield(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void AddFiremove(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void AddGuardian(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void AddChain(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void miss_null_11(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void miss_null_12(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void miss_null_13(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void AddRhino(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void miss_null_32(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void AddFlare(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void AddAcid(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void miss_null_1D(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void AddAcidpud(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void AddStone(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void AddGolem(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void AddEtherealize(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void miss_null_1F(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void miss_null_23(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void AddBoom(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void AddHeal(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void AddHealOther(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void AddElement(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void AddIdentify(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void AddFirewallC(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void AddInfra(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void AddWave(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void AddNova(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void AddBlodboil(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void AddRepair(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void AddRecharge(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void AddDisarm(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void AddApoca(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void AddFlame(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void AddFlamec(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void AddCbolt(int mi, int sx, int sy, int dx, int dy, int midir, char micaster, int id, int dam);
void AddHbolt(int mi, int sx, int sy, int dx, int dy, int midir, char micaster, int id, int dam);
void AddResurrect(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void AddResurrectBeam(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void AddTelekinesis(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void AddBoneSpirit(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void AddRportal(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
void AddDiabApoca(int mi, int sx, int sy, int dx, int dy, int midir, char mienemy, int id, int dam);
int AddMissile(int sx, int sy, int dx, int dy, int midir, int mitype, char micaster, int id, int midam, int spllvl);
void MI_Dummy(int i);
void MI_Golem(int i);
void MI_SetManashield(int i);
void MI_LArrow(int i);
void MI_Arrow(int i);
void MI_Firebolt(int i);
void MI_Lightball(int i);
void mi_null_33(int i);
void MI_Acidpud(int i);
void MI_Firewall(int i);
void MI_Fireball(int i);
void missiles_4359A0(int i);
void MI_Rune(int i);
void mi_light_wall(int i);
void mi_hive_explode(int i);
void mi_immolation(int i);
void mi_light_arrow(int i);
void mi_flashfr(int i);
void mi_flashbk(int i);
void mi_reflect(int i);
void mi_fire_ring(int i);
void mi_light_ring(int i);
void mi_search(int i);
void mi_lightning_wall(int i);
void mi_fire_nova(int i);
void mi_spec_arrow(int i);
void MI_Lightctrl(int i);
void MI_Lightning(int i);
void MI_Town(int i);
void MI_Flash(int i);
void MI_Flash2(int i);
void MI_Etherealize(int i);
void MI_Firemove(int i);
void MI_Guardian(int i);
void MI_Chain(int i);
void mi_null_11(int i);
void MI_Weapexp(int i);
void MI_Misexp(int i);
void MI_Acidsplat(int i);
void MI_Teleport(int i);
void MI_Stone(int i);
void MI_Boom(int i);
void MI_Rhino(int i);
void mi_null_32(int i);
void MI_FirewallC(int i);
void MI_Infra(int i);
void MI_Apoca(int i);
void MI_Wave(int i);
void MI_Nova(int i);
void MI_Blodboil(int i);
void MI_Flame(int i);
void MI_Flamec(int i);
void MI_Cbolt(int i);
void MI_Hbolt(int i);
void MI_Element(int i);
void MI_Bonespirit(int i);
void MI_ResurrectBeam(int i);
void MI_Rportal(int i);
void ProcessMissiles();
void missiles_process_charge();
void ClearMissileSpot(int mi);

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __MISSILES_H__ */
