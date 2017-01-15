/*
 * conditions.h
 *
 *  Created on: Jan 15, 2017
 *      Author: nullifiedcat
 */

#ifndef CONDITIONS_H_
#define CONDITIONS_H_

class CachedEntity;

struct condition_data_s {
	unsigned int cond_0;
	unsigned int cond_1;
	unsigned int cond_2;
	unsigned int cond_3;
};

enum condition {
	TFCond_Slowed = 0,
	TFCond_Zoomed,
	TFCond_Disguising,
	TFCond_Disguised,
	TFCond_Cloaked,
	TFCond_Ubercharged,
	TFCond_TeleportedGlow,
	TFCond_Taunting,
	TFCond_UberchargeFading,
	TFCond_Unknown1, //9
	TFCond_CloakFlicker = 9,
	TFCond_Teleporting,
	TFCond_Kritzkrieged,
	TFCond_Unknown2, //12
	TFCond_TmpDamageBonus = 12,
	TFCond_DeadRingered,
	TFCond_Bonked,
	TFCond_Dazed,
	TFCond_Buffed,
	TFCond_Charging,
	TFCond_DemoBuff,
	TFCond_CritCola,
	TFCond_InHealRadius,
	TFCond_Healing,
	TFCond_OnFire,
	TFCond_Overhealed,
	TFCond_Jarated,
	TFCond_Bleeding,
	TFCond_DefenseBuffed,
	TFCond_Milked,
	TFCond_MegaHeal,
	TFCond_RegenBuffed,
	TFCond_MarkedForDeath,
	TFCond_NoHealingDamageBuff,
	TFCond_SpeedBuffAlly, // 32
	TFCond_HalloweenCritCandy,
	TFCond_CritCanteen,
	TFCond_CritDemoCharge,
	TFCond_CritHype,
	TFCond_CritOnFirstBlood,
	TFCond_CritOnWin,
	TFCond_CritOnFlagCapture,
	TFCond_CritOnKill,
	TFCond_RestrictToMelee,
	TFCond_DefenseBuffNoCritBlock,
	TFCond_Reprogrammed,
	TFCond_CritMmmph,
	TFCond_DefenseBuffMmmph,
	TFCond_FocusBuff,
	TFCond_DisguiseRemoved,
	TFCond_MarkedForDeathSilent,
	TFCond_DisguisedAsDispenser,
	TFCond_Sapped,
	TFCond_UberchargedHidden,
	TFCond_UberchargedCanteen,
	TFCond_HalloweenBombHead,
	TFCond_HalloweenThriller,
	TFCond_RadiusHealOnDamage,
	TFCond_CritOnDamage,
	TFCond_UberchargedOnTakeDamage,
	TFCond_UberBulletResist,
	TFCond_UberBlastResist,
	TFCond_UberFireResist,
	TFCond_SmallBulletResist,
	TFCond_SmallBlastResist,
	TFCond_SmallFireResist,
	TFCond_Stealthed, // 64
	TFCond_MedigunDebuff,
	TFCond_StealthedUserBuffFade,
	TFCond_BulletImmune,
	TFCond_BlastImmune,
	TFCond_FireImmune,
	TFCond_PreventDeath,
	TFCond_MVMBotRadiowave,
	TFCond_HalloweenSpeedBoost,
	TFCond_HalloweenQuickHeal,
	TFCond_HalloweenGiant,
	TFCond_HalloweenTiny,
	TFCond_HalloweenInHell,
	TFCond_HalloweenGhostMode,
	TFCond_MiniCritOnKill,
	TFCond_DodgeChance, //79
	TFCond_ObscuredSmoke = 79,
	TFCond_Parachute,
	TFCond_BlastJumping,
	TFCond_HalloweenKart,
	TFCond_HalloweenKartDash,
	TFCond_BalloonHead,
	TFCond_MeleeOnly,
	TFCond_SwimmingCurse,
	TFCond_HalloweenKartNoTurn, //87
	TFCond_FreezeInput = 87,
	TFCond_HalloweenKartCage,
	TFCond_HasRune,
	TFCond_RuneStrength,
	TFCond_RuneHaste,
	TFCond_RuneRegen,
	TFCond_RuneResist,
	TFCond_RuneVampire,
	TFCond_RuneWarlock,
	TFCond_RunePrecision, // 96
	TFCond_RuneAgility,
	TFCond_GrapplingHook,
	TFCond_GrapplingHookSafeFall,
	TFCond_GrapplingHookLatched,
	TFCond_GrapplingHookBleeding,
	TFCond_AfterburnImmune,
	TFCond_RuneKnockout,
	TFCond_RuneImbalance,
	TFCond_CritRuneTemp,
	TFCond_PasstimeInterception,
	TFCond_SwimmingNoEffects,
	TFCond_EyeaductUnderworld,
	TFCond_KingRune,
	TFCond_PlagueRune,
	TFCond_SupernovaRune,
	TFCond_Plague,
	TFCond_KingAura,
	TFCond_SpawnOutline, //114
	TFCond_KnockedIntoAir,
	TFCond_CompetitiveWinner,
	TFCond_CompetitiveLoser,
	TFCond_NoTaunting
};

bool CondBitCheck(condition_data_s data, unsigned cond);
void CondBitSet(condition_data_s data, condition cond, bool state);
void OldCondSet(CachedEntity* ent, condition cond, bool state);
condition_data_s FromOldNetvars(CachedEntity* ent);
bool HasCondition(CachedEntity* ent, condition cond);
void AddCondition(CachedEntity* ent, condition cond);
void RemoveCondition(CachedEntity* ent, condition cond);

#endif /* CONDITIONS_H_ */
