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

void HEsp::PaintTraverse(void*, unsigned int, bool, bool) {
	for (int i = 0; i < interfaces::entityList->GetHighestEntityIndex(); i++) {
		IClientEntity* ent = interfaces::entityList->GetClientEntity(i);
		if (ent != 0 && i != interfaces::engineClient->GetLocalPlayer()) {
			//if (ent->GetClientClass()->m_ClassID == 241)
			this->ProcessEntity(ent, i);
		}
	}
}

void HEsp::Create() {
	this->v_bEnabled = CreateConVar("u_esp_enabled", "1", "Enables ESP");
	this->v_bBoxESP = CreateConVar("u_esp_box", "0", "Box ESP (NOT IMPLEMENTED)");
	this->v_bEntityESP = CreateConVar("u_esp_entity", "0", "Entity ESP (dev)");
	this->v_bTeammates = CreateConVar("u_esp_teammates", "0", "ESP own team");
	this->v_bItemESP = CreateConVar("u_esp_item", "1", "Item ESP (powerups, health packs, etc)");
	this->v_bTeammatePowerup = CreateConVar("u_esp_powerup_team", "1", "Show powerups on teammates if u_esp_teammates is 0");
	this->v_bShowHead = CreateConVar("u_esp_head", "0", "Shows head (EXPERIMENTAL)");
	this->v_bShowTargetScore = CreateConVar("u_esp_threat", "1", "Shows target score aka threat value");
	//this->v_bModelInfo = CreateConVar("u_esp_model", "0", "ESP model info");
	/*this->v_bEnabled = new ConVar("u_esp_enabled", "1");
	this->v_bShowPacks = new ConVar("u_esp_showpacks", "1");
	interfaces::cvar->RegisterConCommand(v_bEnabled);
	interfaces::cvar->RegisterConCommand(v_bShowPacks);*/
}

void HEsp::ProcessEntity(IClientEntity* ent, int idx) {
	if (!this->v_bEnabled->GetBool()) return;
	if (!ent) return;
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
	switch (ClassID) {
	case 1: {
		if (!this->v_bItemESP->GetBool()) break;
		item_type type = GetItemType(ent);
		if (type == item_type::item_null) break;
		if (type >= item_medkit_small && type <= item_medkit_large) {
			draw::DrawString(scr.x, scr.y, draw::white, true, "%s HEALTH", packs[type - item_medkit_small]);
			scr.y += 16;
		} else if (type >= item_ammo_small && type <= item_ammo_large) {
			draw::DrawString(scr.x, scr.y, draw::white, true, "%s AMMO", packs[type - item_ammo_small]);
			scr.y += 16;
		} else if (type >= item_mp_strength && type <= item_mp_crit) {
			int skin = ent->GetSkin();
			Color pickupColor;
			if (skin == 1) {
				pickupColor = draw::red;
			} else if (skin == 2) {
				pickupColor = draw::blue;
			} else {
				pickupColor = draw::yellow;
			}
			draw::DrawString(scr.x, scr.y, pickupColor, true, "%s PICKUP", powerups[type - item_mp_strength]);
			scr.y += 16;
		}
	} break;
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
		/* If target is enemy, always show powerups, if player is teammate, show powerups
		 * only if bTeammatePowerup or bTeammates is true */
		if (power >= 0 && (!teammate || this->v_bTeammatePowerup->GetBool() || this->v_bTeammates->GetBool())) {
			Color clr = (team == 3 ? draw::blue : (team == 2 ? draw::red : draw::white));
			draw::DrawString(scr.x, scr.y, clr, true, "HAS [%s]", powerups[power]);
			scr.y += 16;
		}
		if (teammate && !this->v_bTeammates->GetBool()) return;
		if (v_bShowTargetScore->GetBool()) {
			draw::DrawString(scr.x, scr.y, clr, true, "Threat: %i", GetScoreForEntity(ent));
			scr.y += 16;
		}
		if (v_bShowHead->GetBool()) {
			Vector pos_head;
			int ret = GetHitboxPosition(ent, hitbox::hb_head, pos_head);
			if (ret) {
				draw::DrawString(scr.x, scr.y, clr, true, "FAILED TO GET HEAD POSITION: %i", ret);
				scr.y += 16;
			} else {
				//draw::DrawString(scr.x, scr.y, clr, true, "HEAD: %f %f %f", pos_head.x, pos_head.y, pos_head.z);
				scr.y += 16;
				Vector screen_head;
				if (draw::WorldToScreen(pos_head, screen_head)) {
					draw::DrawString(screen_head.x, screen_head.y, clr, true, "HEAD");
					//draw::DrawString(screen_head.x, screen_head.y, 32, 32, clr);
				}
			}
		}
		draw::DrawString(scr.x, scr.y, clr, true, "%iu %s", (int)distance, info.name);
		scr.y += 16;
		if (pclass < 10 && pclass > 0)
			draw::DrawString(scr.x, scr.y, clr, true, "%s - %i HP", classes[pclass - 1], health);
			scr.y += 16;
		if (pcond & cond::cloaked) {
			draw::DrawString(scr.x, scr.y, clr, true, "CLOAKED");
			scr.y += 16;
		}
		if (IsPlayerInvulnerable(ent)) {
			draw::DrawString(scr.x, scr.y, clr, true, "INVULNERABLE");
			scr.y += 16;
		}
		if (IsPlayerCritBoosted(ent)) {
			draw::DrawString(scr.x, scr.y, clr, true, "CRIT BOOSTED");
			scr.y += 16;
		}
	} break;
	case 89:
	case 88:
	case 86: { /* Engi Buildings */
		int team = GetEntityValue<int>(ent, eoffsets.iTeamNum);
		if (team == my_team && !this->v_bTeammates->GetBool()) return;
		int health = GetEntityValue<int>(ent, eoffsets.iBuildingHealth);
		int level = GetEntityValue<int>(ent, eoffsets.iUpgradeLevel);
		const char* name = (ClassID == 89 ? "Teleporter" : (ClassID == 88 ? "Sentry Gun" : "Dispenser"));
		Color clr = ((team == 2) ? draw::red : (team == 3 ? draw::blue : draw::white));
		draw::DrawString(scr.x, scr.y, clr, true, "LV %i %s HP %i", level, name, health);
		scr.y += 16;
		draw::DrawString(scr.x, scr.y, clr, true, "%iu", (int)distance);
		scr.y += 16;
	} break;
	case 212: { /* Pipes and Stickies */
		int team = GetEntityValue<int>(ent, eoffsets.iTeamNum);
		if (team == my_team && !this->v_bTeammates->GetBool()) return;
		int type = GetEntityValue<int>(ent, eoffsets.iPipeType);
		Color clr = ((team == 2) ? draw::red : (team == 3 ? draw::blue : draw::white));
		draw::DrawString(scr.x, scr.y, clr, true, type ? "STICKY" : "PIPE", type);
		scr.y += 16;
		draw::DrawString(scr.x, scr.y, clr, true, "%iu", (int)distance);
		scr.y += 16;
	} break;
	}
	if (this->v_bEntityESP->GetBool()) {
		draw::DrawString(scr.x, scr.y, draw::white, true, "%s [%i]", ent->GetClientClass()->GetName(), ent->GetClientClass()->m_ClassID);
		scr.y += 16;
		draw::DrawString(scr.x, scr.y, draw::white, true, "%s %iu", GetModelPath(ent), (int)distance);
		scr.y += 16;
	}
}

bool HEsp::CreateMove(void*, float, CUserCmd*) { return true; };
void HEsp::Destroy() {};




