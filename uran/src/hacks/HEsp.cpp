/*
 * HEsp.cpp
 *
 *  Created on: Oct 6, 2016
 *      Author: nullifiedcat
 */

#include "HEsp.h"
#include "../fixsdk.h"
#include "../drawing.h"
#include "../interfaces.h"
#include "../logging.h"
#include "../enums.h"
#include "../helpers.h"
#include "../entity.h"
#include "../entitycache.h"
#include "../targethelper.h"
#include <client_class.h>
#include <icliententitylist.h>
#include <cdll_int.h>
#include <icliententity.h>
#include <Color.h>
#include <tier1/convar.h>
#include <cmodel.h>

const char* classes[] = {
	"Scout",
	"Sniper",
	"Soldier",
	"Demoman",
	"Medic",
	"Heavy",
	"Pyro",
	"Spy",
	"Engineer"
};

void HEsp::PaintTraverse(void*, unsigned int, bool, bool) {}

void HEsp::Create() {
	this->v_bEnabled = CreateConVar("u_esp_enabled", "1", "Enables ESP");
	this->v_bEntityESP = CreateConVar("u_esp_entity", "0", "Entity ESP (dev)");
	this->v_bTeammates = CreateConVar("u_esp_teammates", "0", "ESP own team");
	this->v_bItemESP = CreateConVar("u_esp_item", "1", "Item ESP (powerups, health packs, etc)");
	this->v_bTeammatePowerup = CreateConVar("u_esp_powerup_team", "1", "Show powerups on teammates if u_esp_teammates is 0");
	this->v_bShowTargetScore = CreateConVar("u_esp_threat", "1", "Shows target score aka threat value");
	this->v_bShowEntityID = CreateConVar("u_esp_entity_id", "0", "Shows EID");
}

#define ESP_HEIGHT 14

void HEsp::ProcessEntity(CachedEntity* ent) {
	if (!this->v_bEnabled->GetBool()) return;
	if (ent->m_bNULL) return;
	if (ent->m_bDormant) return;

	switch (ent->m_iClassID) {
	case ClassID::CBaseAnimating: {
		if (!this->v_bItemESP->GetBool()) break;
		item_type type = GetItemType(ent->m_pEntity);
		if (type == item_type::item_null) break;
		if (type >= item_medkit_small && type <= item_medkit_large) {
			ent->AddESPString(draw::white, cstr("%s HEALTH", packs[type - item_medkit_small]));
		} else if (type >= item_ammo_small && type <= item_ammo_large) {
			ent->AddESPString(draw::white, cstr("%s AMMO", packs[type - item_ammo_small]));
		} else if (type >= item_mp_strength && type <= item_mp_crit) {
			int skin = ent->m_pEntity->GetSkin();
			Color pickupColor;
			if (skin == 1) {
				pickupColor = draw::red;
			} else if (skin == 2) {
				pickupColor = draw::blue;
			} else {
				pickupColor = draw::yellow;
			}
			ent->AddESPString(pickupColor, cstr("%s PICKUP", powerups[type - item_mp_strength]));
		}
		break;
	}
	case ClassID::CTFPlayer: {

		break;
	}
	case ClassID::CObjectSentrygun:
	case ClassID::CObjectDispenser:
	case ClassID::CObjectTeleporter: {
		break;
	}

	}


	/*if (!ent) return;
	if (ent->IsDormant()) return;
	int local = interfaces::engineClient->GetLocalPlayer();
	IClientEntity* me = interfaces::entityList->GetClientEntity(local);

	int my_team = GetEntityValue<int>(me, eoffsets.iTeamNum);

	Vector world;
	Vector min, max;
	ent->GetRenderBounds(min, max);
	float distance = (me->GetAbsOrigin() - ent->GetAbsOrigin()).Length();
	world = ent->GetAbsOrigin();
	world.z += (min.z + max.z) / 2;
	Vector scr;
	if (!draw::WorldToScreen(world, scr)) return;
	// 89 tele 88 sentry 86 disp
	int ClassID = ent->GetClientClass()->m_ClassID;
	scr.y -= 32;
	if (v_bShowEntityID->GetBool()) {
		//draw::DrawString(scr.x, scr.y, draw::white, true, "IDX %i CLASS %i", idx, ent->GetClientClass()->m_ClassID);
		//scr.y += ESP_HEIGHT;
	}
	switch (ClassID) {
	case 241: {
		int team = GetEntityValue<int>(ent, eoffsets.iTeamNum);
		int health = GetEntityValue<int>(ent, eoffsets.iHealth);
		int pclass = GetEntityValue<int>(ent, eoffsets.iClass);
		int pcond = GetEntityValue<int>(ent, eoffsets.iCond);
		if (GetEntityValue<char>(ent, eoffsets.iLifeState) || (team != 2 && team != 3)) return;
		Color clr = ((team == 2) ? draw::red : (team == 3 ? draw::blue : draw::white));
		player_info_t info;
		if (!interfaces::engineClient->GetPlayerInfo(idx, &info)) return;
		powerup_type power = GetPowerupOnPlayer(ent);
		bool teammate = (team == my_team);
		// If target is enemy, always show powerups, if player is teammate, show powerups
		// only if bTeammatePowerup or bTeammates is true
		if (power >= 0 && (!teammate || this->v_bTeammatePowerup->GetBool() || this->v_bTeammates->GetBool())) {
			Color clr = (team == 3 ? draw::blue : (team == 2 ? draw::red : draw::white));
			draw::DrawString(scr.x, scr.y, clr, true, "HAS [%s]", powerups[power]);
			scr.y += ESP_HEIGHT;
		}
		if (teammate && !this->v_bTeammates->GetBool()) return;
		if (v_bShowTargetScore->GetBool()) {
			draw::DrawString(scr.x, scr.y, clr, true, "Threat: %i", GetScoreForEntity(ent));
			scr.y += ESP_HEIGHT;
		}
		draw::DrawString(scr.x, scr.y, clr, true, "%im %s", (int)(distance / 64 * 1.22f), info.name);
		scr.y += ESP_HEIGHT;
		if (pclass < 10 && pclass > 0)
			draw::DrawString(scr.x, scr.y, clr, true, "%s - %i HP", classes[pclass - 1], health);
			scr.y += ESP_HEIGHT;
		if (pcond & cond::cloaked) {
			draw::DrawString(scr.x, scr.y, clr, true, "CLOAKED");
			scr.y += ESP_HEIGHT;
		}
		if (IsPlayerInvulnerable(ent)) {
			draw::DrawString(scr.x, scr.y, clr, true, "INVULNERABLE");
			scr.y += ESP_HEIGHT;
		}
		if (IsPlayerCritBoosted(ent)) {
			draw::DrawString(scr.x, scr.y, clr, true, "CRIT BOOSTED");
			scr.y += ESP_HEIGHT;
		}
	} break;
	case 89:
	case 88:
	case 86: { // builds
		int team = GetEntityValue<int>(ent, eoffsets.iTeamNum);
		if (team == my_team && !this->v_bTeammates->GetBool()) return;
		int health = GetEntityValue<int>(ent, eoffsets.iBuildingHealth);
		int level = GetEntityValue<int>(ent, eoffsets.iUpgradeLevel);
		const char* name = (ClassID == 89 ? "Teleporter" : (ClassID == 88 ? "Sentry Gun" : "Dispenser"));
		Color clr = ((team == 2) ? draw::red : (team == 3 ? draw::blue : draw::white));
		draw::DrawString(scr.x, scr.y, clr, true, "LV %i %s HP %i", level, name, health);
		scr.y += ESP_HEIGHT;
		draw::DrawString(scr.x, scr.y, clr, true, "%iu", (int)distance);
		scr.y += ESP_HEIGHT;
	} break;
	case 212: { // pipes
		int team = GetEntityValue<int>(ent, eoffsets.iTeamNum);
		if (team == my_team && !this->v_bTeammates->GetBool()) return;
		int type = GetEntityValue<int>(ent, eoffsets.iPipeType);
		Color clr = ((team == 2) ? draw::red : (team == 3 ? draw::blue : draw::white));
		draw::DrawString(scr.x, scr.y, clr, true, type ? "STICKY" : "PIPE", type);
		scr.y += ESP_HEIGHT;
		draw::DrawString(scr.x, scr.y, clr, true, "%iu", (int)distance);
		scr.y += ESP_HEIGHT;
	} break;
	}
	if (this->v_bEntityESP->GetBool()) {
		draw::DrawString(scr.x, scr.y, draw::white, true, "%s [%i]", ent->GetClientClass()->GetName(), ent->GetClientClass()->m_ClassID);
		scr.y += ESP_HEIGHT;
		draw::DrawString(scr.x, scr.y, draw::white, true, "%s %iu", GetModelPath(ent), (int)distance);
		scr.y += ESP_HEIGHT;
	}*/
}

bool HEsp::CreateMove(void*, float, CUserCmd*) {
	for (int i = 0; i < gEntityCache.m_nMax; i++) {
		ProcessEntity(gEntityCache.GetEntity(i));
	}
	return true;
};

void HEsp::Destroy() {};




