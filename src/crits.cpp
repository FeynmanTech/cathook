/*
 * crits.cpp
 *
 *  Created on: Feb 25, 2017
 *      Author: nullifiedcat
 */

#include "crits.h"
#include "common.h"
#include "sharedobj.h"
#include "copypasted/CSignature.h"
#include <link.h>

bool RandomCrits() {
	static ConVar* tf_weapon_criticals = interfaces::cvar->FindVar("tf_weapon_criticals");
	return tf_weapon_criticals->GetBool();
}

bool WeaponCanCrit() {
	return TF && CE_GOOD(LOCAL_W) && vfunc<bool(*)(IClientEntity*)>(RAW_ENT(LOCAL_W), 465 + 21, 0)(RAW_ENT(LOCAL_W));
}

bool IsAttackACrit(CUserCmd* cmd) {
	if (CE_GOOD(LOCAL_W) && TF) {
		IClientEntity* weapon = RAW_ENT(LOCAL_W);
		if (TF2C) {
			if (vfunc<bool(*)(IClientEntity*)>(weapon, 1824 / 4, 0)(weapon)) {
				static uintptr_t CalcIsAttackCritical_s = gSignatures.GetClientSignature("55 89 E5 56 53 83 EC 10 8B 5D 08 89 1C 24 E8 ? ? ? ? 85 C0 89 C6 74 59 8B 00 89 34 24 FF 90 E0 02 00 00 84 C0 74 4A A1 ? ? ? ? 8B 40 04 3B 83 A8 09 00 00 74 3A");
				typedef void(*CalcIsAttackCritical_t)(IClientEntity*);
				CalcIsAttackCritical_t CIACFn = (CalcIsAttackCritical_t)(CalcIsAttackCritical_s);
				*(float*)((uintptr_t)weapon + 2468ul) = 0.0f;
				int tries = 0;
				static int lcmdn = 0;
				if (*(bool*)((uintptr_t)RAW_ENT(LOCAL_W) + 2454ul)) return true;
				static int& seed = *(int*)(sharedobj::client->lmap->l_addr + 0x00D53F68ul);
				bool cmds = false;
				seed = MD5_PseudoRandom(cmd->command_number) & 0x7fffffff;
				RandomSeed(seed);
				CIACFn(RAW_ENT(LOCAL_W));
				return *(bool*)((uintptr_t)RAW_ENT(LOCAL_W) + 2454ul);
			}
		} else if (TF2) {
			if (vfunc<bool(*)(IClientEntity*)>(weapon, 1944 / 4, 0)(weapon)) {
				static uintptr_t CalcIsAttackCritical_s = gSignatures.GetClientSignature("55 89 E5 83 EC 28 89 5D F4 8B 5D 08 89 75 F8 89 7D FC 89 1C 24 E8 ? ? ? ? 85 C0 89 C6 74 60 8B 00 89 34 24 FF 90 E0 02 00 00 84 C0 74 51 A1 ? ? ? ? 8B 40 04");
				typedef void(*CalcIsAttackCritical_t)(IClientEntity*);
				CalcIsAttackCritical_t CIACFn = (CalcIsAttackCritical_t)(CalcIsAttackCritical_s);
				if (cmd->command_number) {
					int tries = 0;
					static int cmdn = 0;
					bool chc = false;
					int md5seed = MD5_PseudoRandom(cmd->command_number) & 0x7fffffff;
					int rseed = md5seed;
					float bucket = *(float*)((uintptr_t)RAW_ENT(LOCAL_W) + 2612u);
					int& a = *(int*)((uintptr_t)(sharedobj::client->lmap->l_addr) + 0x01F8B228);
					a = md5seed;
					int c = LOCAL_W->m_IDX << 8;
					int b = LOCAL_E->m_IDX;
					rseed = rseed ^ (b | c);
					*(float*)(weapon + 2856ul) = 0.0f;
					RandomSeed(rseed);
					bool crits = vfunc<bool(*)(IClientEntity*)>(weapon, 1836 / 4, 0)(weapon);
					//if (!crits) *(float*)((uintptr_t)RAW_ENT(LOCAL_W) + 2612u) = bucket;
					return crits;
				}
			}
		}
	}
	return false;
}
