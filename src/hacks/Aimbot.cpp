/*
 * HAimbot.cpp
 *
 *  Created on: Oct 9, 2016
 *      Author: nullifiedcat
 */

#include "../common.h"
#include "../trace.h"
#include "../targethelper.h"

#include "../sdk.h"
#include "../sdk/in_buttons.h"
#include "Aimbot.h"

DEFINE_HACK_SINGLETON(Aimbot);

enum TargetSystem_t {
	SMART = 0,
	FOV = 1,
	DISTANCE = 2
};

Aimbot::Aimbot() {
	m_bAimKeySwitch = false;
	this->v_eAimKeyMode = new CatVar_DEPRECATED(CV_ENUM, "aimbot_aimkey_mode", "1", "Aimkey mode", new CatEnum({ "DISABLED", "AIMKEY", "REVERSE", "TOGGLE" }),
			"DISABLED: aimbot is always active\nAIMKEY: aimbot is active when key is down\nREVERSE: aimbot is disabled when key is down\nTOGGLE: pressing key toggles aimbot", false);
	this->v_bEnabled = new CatVar_DEPRECATED(CV_SWITCH, "aimbot_enabled", "0", "Enable Aimbot", NULL,
			"Main aimbot switch");
	this->v_eHitbox = new CatVar_DEPRECATED(CV_ENUM, "aimbot_hitbox", "0", "Hitbox", new CatEnum({
		"HEAD", "PELVIS", "SPINE 0", "SPINE 1", "SPINE 2", "SPINE 3", "UPPER ARM L", "LOWER ARM L",
		"HAND L", "UPPER ARM R", "LOWER ARM R", "HAND R", "HIP L", "KNEE L", "FOOT L", "HIP R",
		"KNEE R", "FOOT R" }),
			"Hitbox to aim at. Ignored if AutoHitbox is on");
	/*this->v_bAutoHitbox = new CatVar(CV_SWITCH, "aimbot_autohitbox", "1", "Autohitbox", NULL,
			"Automatically decide the hitbox to aim at.\n"
			"For example: Sniper rifles and Ambassador always aim at head, "
			"rocket launchers aim at feet if enemy is standing and at body "
			"if enemy is midair for easy airshots");*/
	this->v_eHitboxMode = new CatVar_DEPRECATED(CV_ENUM, "aimbot_hitboxmode", "0", "Hitbox Mode", new CatEnum({
		"AUTO-HEAD", "AUTO-CLOSEST", "STATIC"
	}), "Defines hitbox selection mode");
	this->v_bInterpolation = new CatVar_DEPRECATED(CV_SWITCH, "aimbot_interp", "1", "Latency interpolation", NULL,
			"Enable basic latency interpolation");
	this->v_bAutoShoot = new CatVar_DEPRECATED(CV_SWITCH, "aimbot_autoshoot", "1", "Autoshoot", NULL,
			"Shoot automatically when the target is locked, isn't compatible with 'Enable when attacking'");
	this->v_bSilent = new CatVar_DEPRECATED(CV_SWITCH, "aimbot_silent", "1", "Silent", NULL,
			"Your screen doesn't get snapped to the point where aimbot aims at");
	this->v_bZoomedOnly = new CatVar_DEPRECATED(CV_SWITCH, "aimbot_zoomed", "1", "Zoomed only", NULL,
			"Don't autoshoot with unzoomed rifles");
	/*this->v_iAutoShootCharge = CREATE_CV(
			CV_FLOAT, "aimbot_autoshoot_charge", "0.0", "Autoshoot Charge");*/
	this->v_iMaxRange = new CatVar_DEPRECATED(CV_INT, "aimbot_maxrange", "0", "Max distance", NULL,
			"Max range for aimbot\n"
			"900-1100 range is efficient for scout/widowmaker engineer", true, 4096.0f);
	this->v_bRespectCloak = new CatVar_DEPRECATED(CV_SWITCH, "aimbot_respect_cloak", "1", "Respect cloak", NULL,
			"Don't aim at invisible enemies");
	this->v_bEnabledAttacking = new CatVar_DEPRECATED(CV_SWITCH, "aimbot_enable_attack_only", "0", "Active when attacking", NULL,
			"Basically makes Mouse1 an AimKey\n"
			"Isn't compatible with AutoShoot");
	this->v_bProjectileAimbot = new CatVar_DEPRECATED(CV_SWITCH, "aimbot_projectile", "1", "Projectile aimbot", NULL,
			"If you turn it off, aimbot won't try to aim "
			"with projectile weapons");
	this->v_fOverrideProjSpeed = new CatVar_DEPRECATED(CV_FLOAT, "aimbot_proj_speed", "0", "Projectile speed", NULL,
			"Force override projectile speed.\n"
			"Can be useful for playing with MvM upgrades or on x10 servers "
			"since there is no \"automatic\" projectile speed detection in "
			"cathook. Yet.");
	this->v_fOverrideProjGravity = new CatVar_DEPRECATED(CV_FLOAT, "aimbot_proj_gravity", "0", "Projectile gravity", NULL,
			"Force override projectile gravity. Useful for debugging.", true);
	this->v_fFOV = new CatVar_DEPRECATED(CV_FLOAT, "aimbot_fov", "0", "Aimbot FOV", NULL,
			"FOV range for aimbot to lock targets.\n"
			"\"Smart FOV\" coming soon.", true, 360.0f);
	this->v_fAutoShootHuntsmanCharge = new CatVar_DEPRECATED(CV_FLOAT, "aimbot_huntsman_charge", "0.5", "Huntsman autoshoot", NULL,
			"Minimum charge for autoshooting with huntsman.\n"
			"Set it to 0.01 if you want to shoot as soon as you start "
			"pulling the arrow", true, 1.0f, 0.01f);
	this->v_kAimKey = new CatVar_DEPRECATED(CV_KEY, "aimbot_aimkey", "0", "Aimkey", NULL,
			"Aimkey. Look at Aimkey Mode too!");
	this->v_ePriorityMode = new CatVar_DEPRECATED(CV_ENUM, "aimbot_prioritymode", "0", "Priority mode",
			new CatEnum({ "SMART", "FOV", "DISTANCE", "HEALTH" }),
			"Priority mode.\n"
			"SMART: Basically Auto-Threat. Will be tweakable eventually.\n"
			"FOV, DISTANCE, HEALTH are self-explainable.\n"
			"HEALTH picks the weakest enemy");
	v_bAimBuildings = new CatVar_DEPRECATED(CV_SWITCH, "aimbot_buildings", "1", "Aim at buildings", NULL,
			"Should aimbot aim at buildings?");
	v_bActiveOnlyWhenCanShoot = new CatVar_DEPRECATED(CV_SWITCH, "aimbot_only_when_can_shoot", "1", "Active when can shoot", NULL,
			"Aimbot only activates when you can instantly shoot, sometimes making the autoshoot invisible for spectators");
	//v_fSmoothAutoshootTreshold = new CatVar(CV_FLOAT, "aimbot_smooth_autoshoot_treshold", "0.01", "Smooth autoshoot");
	//this->v_fSmoothRandomness = CREATE_CV(CV_FLOAT, "aimbot_smooth_randomness", "1.0", "Smooth randomness");
//	this->v_iSeenDelay = new CatVar(CV_INT, "aimbot_delay", "0", "Aimbot delay", NULL,
//			"# of ticks that should've passed since you can see any hitbox of enemy before aimbot will aim at them", true, 300.0f);
	this->v_bProjPredVisibility = new CatVar_DEPRECATED(CV_SWITCH, "aimbot_proj_vispred", "0", "Projectile visibility prediction", NULL,
			"If disabled, aimbot won't lock at enemies that are behind walls, but will come out soon");
	this->v_bProjPredFOV = new CatVar_DEPRECATED(CV_SWITCH, "aimbot_proj_fovpred", "0", "Projectile FOV mode", NULL,
			"If disabled, FOV restrictions apply to current target position");
	this->v_bAimAtTeammates = new CatVar_DEPRECATED(CV_SWITCH, "aimbot_teammates", "0", "Aim at teammates", NULL,
			"Aim at your own team. Useful for HL2DM");
	//this->v_bAdvancedSapperAim = new CatVar(CV_SWITCH, "aimbot_sapper", "1", "Advanced Sapper Aim", NULL,
	//		"Aim at buildings while holding sapper");
}

bool Aimbot::ShouldAim(CUserCmd* cmd) {
	if (this->v_kAimKey->GetBool() && this->v_eAimKeyMode->GetBool()) {
		bool key_down = interfaces::input->IsButtonDown((ButtonCode_t)this->v_kAimKey->GetInt());
		switch (this->v_eAimKeyMode->GetInt()) {
		case AimKeyMode_t::PRESS_TO_ENABLE:
			if (key_down) break;
			else return false;
		case AimKeyMode_t::PRESS_TO_DISABLE:
			if (key_down) return false;
			else break;
		case AimKeyMode_t::PRESS_TO_TOGGLE:
			m_bAimKeySwitch = !m_bAimKeySwitch;
			if (!m_bAimKeySwitch) return false;
		}
	}
	if (this->v_bActiveOnlyWhenCanShoot->GetBool()) {
		// Miniguns should shoot and aim continiously. TODO smg
		if (g_pLocalPlayer->weapon()->m_iClassID != g_pClassID->CTFMinigun) {
			// Melees are weird, they should aim continiously like miniguns too.
			if (GetWeaponMode(g_pLocalPlayer->entity) != weaponmode::weapon_melee) {
				// Finally, CanShoot() check.
				if (!CanShoot()) return false;
			}
		}
	}
	if (this->v_bEnabledAttacking->GetBool() && !(cmd->buttons & IN_ATTACK)) {
		return false;
	}
	if (g_pLocalPlayer->weapon()->m_iClassID == g_pClassID->CTFMinigun) {
		if (!HasCondition(g_pLocalPlayer->entity, TFCond_Slowed)) {
			return false;
		}
		if (!(cmd->buttons & IN_ATTACK2)) {
			return false;
		}
		if (m_nMinigunFixTicks > 0) {
			m_nMinigunFixTicks--;
			cmd->buttons |= IN_ATTACK;
		}
	}

	if (IsAmbassador(g_pLocalPlayer->weapon())) { // TODO AmbassadorCanHeadshot()
		if ((interfaces::gvars->curtime - CE_FLOAT(g_pLocalPlayer->weapon(), netvar.flLastFireTime)) <= 1.0) {
			return false;
		}
	}
	if (g_pLocalPlayer->bZoomed) {
		// TODO IsSniperRifle()
		if (g_pLocalPlayer->weapon()->m_iClassID == g_pClassID->CTFSniperRifle ||
			g_pLocalPlayer->weapon()->m_iClassID == g_pClassID->CTFSniperRifleDecap) {
			if (!CanHeadshot()) return false;
		}
	}
	if (g_phMisc->v_bCritHack->GetBool()) {
		if (RandomCrits() && WeaponCanCrit() && !IsAttackACrit(cmd)) return false;
	}
	return true;
}

void Aimbot::ProcessUserCmd(CUserCmd* cmd) {
	if (!this->v_bEnabled->GetBool()) return;
	if (CE_BAD(g_pLocalPlayer->entity) || CE_BAD(g_pLocalPlayer->weapon())) return;
	if (g_pLocalPlayer->life_state) return;
	//this->m_iLastTarget = -1

	if (HasCondition(g_pLocalPlayer->entity, TFCond_Taunting)) return;

	switch (GetWeaponMode(g_pLocalPlayer->entity)) {
	case weapon_medigun:
	case weapon_pda:
	case weapon_consumable:
	case weapon_throwable:
	case weapon_invalid:
		return;
	case weapon_projectile:
		if (!v_bProjectileAimbot->GetBool()) return;
	};

	if (HasCondition(g_pLocalPlayer->entity, TFCond_Cloaked)) return; // TODO other kinds of cloak
	// TODO m_bFeignDeathReady no aim

	if(cmd->buttons & IN_USE) return;

	/*if (this->v_bTriggerMode->GetBool() ) {
		cmd->buttons = cmd->buttons &~ IN_ATTACK;
	}*/

	m_bHeadOnly = false;

	m_iPreferredHitbox = this->v_eHitbox->GetInt();
	if (g_pLocalPlayer->weapon()->m_iClassID == g_pClassID->CTFGrapplingHook) return;

	m_bProjectileMode = (GetProjectileData(g_pLocalPlayer->weapon(), m_flProjSpeed, m_flProjGravity));
	if (v_fOverrideProjSpeed->GetBool())
		m_flProjSpeed = v_fOverrideProjSpeed->GetFloat();
	if (v_fOverrideProjGravity->GetBool())
		m_flProjGravity = v_fOverrideProjGravity->GetFloat();

	// TODO priority modes (FOV, Smart, Distance, etc)
	CachedEntity* target_highest = 0;
	float target_highest_score = -256;
	for (int i = 0; i < HIGHEST_ENTITY; i++) {
		CachedEntity* ent = ENTITY(i);
		if (CE_BAD(ent)) continue;
		int tg = ShouldTarget(ent);
		if (!tg) {
			if (GetWeaponMode(g_pLocalPlayer->entity) == weaponmode::weapon_melee || this->v_ePriorityMode->GetInt() == 2) {
				Vector result;
				if (ent->m_Type == ENTITY_BUILDING) {
					result = GetBuildingPosition(ent);
				} else {
					GetHitbox(ent, BestHitbox(ent, m_iPreferredHitbox), result);
				}
				float scr = 4096.0f - result.DistTo(g_pLocalPlayer->v_Eye);
				if (scr > target_highest_score) {
					target_highest_score = scr;
					target_highest = ent;
				}
			} else {
				switch (this->v_ePriorityMode->GetInt()) {
				case 0: {
					int scr = GetScoreForEntity(ent);
					if (scr > target_highest_score) {
						target_highest_score = scr;
						target_highest = ent;
					}
				} break;
				case 1: {
					Vector result;
					if (ent->m_Type == ENTITY_BUILDING) {
						result = GetBuildingPosition(ent);
					} else {
						GetHitbox(ent, BestHitbox(ent, m_iPreferredHitbox), result);
					}
					float scr = 360.0f - GetFov(g_pLocalPlayer->v_OrigViewangles, g_pLocalPlayer->v_Eye, result);
					if (scr > target_highest_score) {
						target_highest_score = scr;
						target_highest = ent;
					}
				} break;
				case 3: {
					float scr;
					if (ent->m_Type == ENTITY_BUILDING) {
						scr = 450.0f - CE_INT(ent, netvar.iBuildingHealth);
					} else {
						scr = 450.0f - CE_INT(ent, netvar.iHealth);
					}
					if (scr > target_highest_score) {
						target_highest_score = scr;
						target_highest = ent;
					}
				}
				}
			}
		} else {
			//if (tg != 26)
			//logging::Info("Shouldn't target ent %i %i", ent->m_IDX, tg);
		}
	}
	if (CE_GOOD(target_highest)) {
		target_highest->m_ESPColorFG = colors::pink;
		if (ShouldAim(cmd)) {
			this->m_iLastTarget = target_highest->m_IDX;
			if (g_pLocalPlayer->weapon()->m_iClassID == g_pClassID->CTFCompoundBow) { // There is no Huntsman in TF2C.
				float begincharge = CE_FLOAT(g_pLocalPlayer->weapon(), netvar.flChargeBeginTime);
				float charge = 0;
				if (begincharge != 0) {
					charge = interfaces::gvars->curtime - begincharge;
					if (charge > 1.0f) charge = 1.0f;
					m_bSilentHuntsman = true;
				}
				if (charge >= v_fAutoShootHuntsmanCharge->GetFloat()) {
					cmd->buttons &= ~IN_ATTACK;
					g_phAntiAim->AddSafeTicks(3);
				}
				if (!(cmd->buttons & IN_ATTACK) && m_bSilentHuntsman) {
					Aim(target_highest, cmd);
					m_bSilentHuntsman = false;
				}
			} else {
				Aim(target_highest, cmd);
			}
			if (g_pLocalPlayer->weapon()->m_iClassID == g_pClassID->CTFMinigun)
				m_nMinigunFixTicks = 40;
		}
	}
	if (g_pLocalPlayer->weapon()->m_iClassID == g_pClassID->CTFMinigun &&
			target_highest == 0 &&
			IDX_GOOD(m_iLastTarget) &&
			m_nMinigunFixTicks && ShouldAim(cmd)) {
		Aim(ENTITY(m_iLastTarget), cmd);
	}
	if (this->v_bSilent->GetBool()) g_pLocalPlayer->bUseSilentAngles = true;
	return;
}
// FIXME move
int ClosestHitbox(CachedEntity* target) {
	int closest = -1;
	float closest_fov = 256;
	for (int i = 0; i < target->m_pHitboxCache->GetNumHitboxes(); i++) {
		float fov = GetFov(g_pLocalPlayer->v_OrigViewangles, g_pLocalPlayer->v_Eye, target->m_pHitboxCache->GetHitbox(i)->center);
		if (fov < closest_fov || closest == -1) {
			closest = i;
			closest_fov = fov;
		}
	}
	return closest;
}

int Aimbot::BestHitbox(CachedEntity* target, int preferred) {
	switch (v_eHitboxMode->GetInt()) {
	case 0: { // AUTO-HEAD
		int ci = g_pLocalPlayer->weapon()->m_iClassID;
		if (ci == g_pClassID->CTFSniperRifle ||
			ci == g_pClassID->CTFSniperRifleDecap) {
			m_bHeadOnly = CanHeadshot();
		} else if (ci == g_pClassID->CTFCompoundBow) {
			m_bHeadOnly = true;
		} else if (ci == g_pClassID->CTFRevolver) {
			m_bHeadOnly = IsAmbassador(g_pLocalPlayer->weapon());
		} else if (ci == g_pClassID->CTFRocketLauncher ||
				ci == g_pClassID->CTFRocketLauncher_AirStrike ||
				ci == g_pClassID->CTFRocketLauncher_DirectHit ||
				ci == g_pClassID->CTFRocketLauncher_Mortar) {
			preferred = hitbox_t::foot_L;
		} else {
			preferred = hitbox_t::pelvis;
		}
		int flags = CE_INT(target, netvar.iFlags);
		bool ground = (flags & (1 << 0));
		if (!ground) {
			if (GetWeaponMode(g_pLocalPlayer->entity) == weaponmode::weapon_projectile) {
				if (g_pLocalPlayer->weapon()->m_iClassID != g_pClassID->CTFCompoundBow) {
					preferred = hitbox_t::spine_3;
				}
			}
		}
		if (LOCAL_W->m_iClassID == g_pClassID->CTFSniperRifle || LOCAL_W->m_iClassID == g_pClassID->CTFSniperRifleDecap) {
			float cdmg = CE_FLOAT(LOCAL_W, netvar.flChargedDamage);
			if (CanHeadshot() && cdmg > target->m_iHealth) {
				preferred = ClosestHitbox(target);
				m_bHeadOnly = false;
			}
		}
		if (m_bHeadOnly) return hitbox_t::head;
		if (target->m_pHitboxCache->VisibilityCheck(preferred)) return preferred;
		for (int i = m_bProjectileMode ? 1 : 0; i < target->m_pHitboxCache->GetNumHitboxes(); i++) {
			if (target->m_pHitboxCache->VisibilityCheck(i)) return i;
		}
	} break;
	case 1: { // AUTO-CLOSEST
		return ClosestHitbox(target);
	} break;
	case 2: { // STATIC
		return v_eHitbox->GetInt();;
	} break;
	}
	return -1;
}

int Aimbot::ShouldTarget(CachedEntity* entity) {
	// Just assuming CE is good
	if (entity->m_Type == ENTITY_PLAYER) {
		if (TF) {
			if (g_Settings.bIgnoreTaunting->GetBool() && HasCondition(entity, TFCond_Taunting)) return 1;
			if (IsPlayerInvulnerable(entity)) return 4;
			if (v_bRespectCloak->GetBool() && IsPlayerInvisible(entity)) return 6;
			weaponmode mode = GetWeaponMode(LOCAL_E);
			if (mode == weaponmode::weapon_hitscan || LOCAL_W->m_iClassID == g_pClassID->CTFCompoundBow)
				if (HasCondition(entity, TFCond_UberBulletResist)) return 10;
		}

#if NO_DEVIGNORE != true
		if (Developer(entity)) return 2; // TODO developer relation
#endif
		//if (entity->m_lSeenTicks < (unsigned)this->v_iSeenDelay->GetInt()) return 3;
		if (!entity->m_bAlivePlayer) return 5;
		if (!entity->m_bEnemy && !v_bAimAtTeammates->GetBool()) return 7;
		if (v_iMaxRange->GetInt() > 0) {
			if (entity->m_flDistance > v_iMaxRange->GetInt()) return 8;
		}
		if (GetWeaponMode(g_pLocalPlayer->entity) == weaponmode::weapon_melee) {
			if (entity->m_flDistance > 95) return 9;
		}
		if (GetRelation(entity) == relation::FRIEND) return 11;
		Vector resultAim;
		int hitbox = BestHitbox(entity, m_iPreferredHitbox);
		//if (m_bHeadOnly && hitbox) return 12;
		if (m_bProjectileMode) {
			if (v_bProjPredFOV->GetBool()) {
				if (v_bProjPredVisibility->GetBool()) {
					if (!GetHitbox(entity, hitbox, resultAim)) return 13;
					if (!IsEntityVisible(entity, hitbox)) return 14;
				}
				resultAim = ProjectilePrediction(entity, hitbox, m_flProjSpeed, m_flProjGravity, PlayerGravityMod(entity));
			} else {
				if (!GetHitbox(entity, hitbox, resultAim)) return 15;
			}
			if (!IsVectorVisible(g_pLocalPlayer->v_Eye, resultAim)) return 16;
		} else {
			/*if (v_bMachinaPenetration->GetBool()) {
				if (!GetHitbox(entity, hitbox, resultAim)) return false;
				if (!IsEntityVisiblePenetration(entity, v_eHitbox->GetInt())) return false;
			} else*/ {
				if (!GetHitbox(entity, hitbox, resultAim)) return 17;
				if (!IsEntityVisible(entity, hitbox)) return 18;
			}
		}
		if (v_fFOV->GetFloat() > 0.0f && (GetFov(g_pLocalPlayer->v_OrigViewangles, g_pLocalPlayer->v_Eye, resultAim) > v_fFOV->GetFloat())) return 25;
		return false;
	} else if (entity->m_Type == ENTITY_BUILDING) {
		if (!v_bAimBuildings->GetBool()) return 19;
		int team = CE_INT(entity, netvar.iTeamNum);
		if (team == g_pLocalPlayer->team) return 20;
		if (v_iMaxRange->GetInt() > 0) {
			if (entity->m_flDistance > v_iMaxRange->GetInt()) return 21;
		}
		if (GetWeaponMode(g_pLocalPlayer->entity) == weaponmode::weapon_melee) {
			if (entity->m_flDistance > 95) return 22;
		}
		Vector resultAim;
		// TODO fix proj buildings
		if (m_bProjectileMode) {
			if (!IsBuildingVisible(entity)) return 23;
			resultAim = GetBuildingPosition(entity);
			//resultAim = entity->GetAbsOrigin();
			//if (!PredictProjectileAim(g_pLocalPlayer->v_Eye, entity, (hitbox_t)m_iHitbox, m_flProjSpeed, m_bProjArc, m_flProjGravity, resultAim)) return false;
		} else {
			//logging::Info("IsVisible?");
			resultAim = GetBuildingPosition(entity);
			if (!IsBuildingVisible(entity)) return 24;
		}
		//logging::Info("IsFOV?");
		if (v_fFOV->GetFloat() > 0.0f && (GetFov(g_pLocalPlayer->v_OrigViewangles, g_pLocalPlayer->v_Eye, resultAim) > v_fFOV->GetFloat())) return 25;
		//logging::Info("Tru");
		return 0;
	} else {
		return 26;
	}
	return 27;
}

// TODO Vector objects
bool Aimbot::Aim(CachedEntity* entity, CUserCmd* cmd) {
	//logging::Info("Aiming!");
	Vector hit;
	Vector angles;
	if (CE_BAD(entity)) return false;
	int hitbox = BestHitbox(entity, m_iPreferredHitbox);
	//if (m_bHeadOnly) hitbox = 0;
	if (entity->m_Type == ENTITY_PLAYER) {
		//logging::Info("A");
		GetHitbox(entity, hitbox, hit);
		//logging::Info("B");
		if (this->v_bInterpolation->GetBool()) SimpleLatencyPrediction(entity, hitbox);
		//logging::Info("C");
	} else if (entity->m_Type == ENTITY_BUILDING) {
		hit = GetBuildingPosition(entity);
	}
	if (m_bProjectileMode) {
		if (v_fOverrideProjSpeed->GetBool())
			m_flProjSpeed = v_fOverrideProjSpeed->GetFloat();
		if (v_fOverrideProjGravity->GetBool())
			m_flProjGravity = v_fOverrideProjGravity->GetFloat();
		bool succ = false;
		hit = ProjectilePrediction(entity, hitbox, m_flProjSpeed, m_flProjGravity, PlayerGravityMod(entity));
	}
	//logging::Info("ayyming!");
	Vector tr = (hit - g_pLocalPlayer->v_Eye);
	fVectorAngles(tr, angles);
	bool smoothed = false;
	/*if (this->v_bSmooth->GetBool()) {
		Vector da = (angles - g_pLocalPlayer->v_OrigViewangles);
		fClampAngle(da);
		smoothed = true;
		if (da.IsZero(v_fSmoothAutoshootTreshold->GetFloat())) smoothed = false;
		da *= this->v_fSmoothValue->GetFloat() * (((float)rand() / (float)RAND_MAX) * this->v_fSmoothRandomness->GetFloat());
		angles = g_pLocalPlayer->v_OrigViewangles + da;
	}*/
	fClampAngle(angles);
	cmd->viewangles = angles;
	if (this->v_bSilent->GetBool()) {
		g_pLocalPlayer->bUseSilentAngles = true;
	}
	if (this->v_bAutoShoot->GetBool()) {
		if (g_pLocalPlayer->clazz == tf_class::tf_sniper) {
			if (g_pLocalPlayer->weapon()->m_iClassID == g_pClassID->CTFSniperRifle || g_pLocalPlayer->weapon()->m_iClassID == g_pClassID->CTFSniperRifleDecap) {
				if (v_bZoomedOnly->GetBool() && !CanHeadshot()) return true;
			}
		}
		if (g_pLocalPlayer->weapon()->m_iClassID != g_pClassID->CTFCompoundBow) {
			cmd->buttons |= IN_ATTACK;
		}
	}
	return true;
}

void Aimbot::OnLevelInit() {
	m_iLastTarget = -1;
	m_bProjectileMode = false;
}
