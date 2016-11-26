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
#include "../localplayer.h"
#include <client_class.h>
#include <icliententitylist.h>
#include <cdll_int.h>
#include <icliententity.h>
#include <Color.h>
#include <tier1/convar.h>
#include <cmodel.h>

const char* HEsp::GetName() {
	return "ESP";
}

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
	for (int i = 0; i < gEntityCache.m_nMax; i++) {
		ProcessEntityPT(gEntityCache.GetEntity(i));
	}
}

void HEsp::Create() {
	this->v_bEnabled = CreateConVar("u_esp_enabled", "1", "Enables ESP");
	this->v_bEntityESP = CreateConVar("u_esp_entity", "0", "Entity ESP (dev)");
	this->v_bTeammates = CreateConVar("u_esp_teammates", "0", "ESP own team");
	this->v_bItemESP = CreateConVar("u_esp_item", "0", "Item ESP (powerups, health packs, etc)");
	this->v_bTeammatePowerup = CreateConVar("u_esp_powerup_team", "1", "Show powerups on teammates if u_esp_teammates is 0");
	this->v_bShowEntityID = CreateConVar("u_esp_entity_id", "0", "Shows EID");
	this->v_bShowDistance = CreateConVar("u_esp_distance", "1", "Distance ESP");
	this->v_bBox = CreateConVar("u_esp_box", "1", "Box");
	this->v_bShowFriendID = CreateConVar("u_esp_friendid", "0", "Show friend ID");
	this->v_bShowFriends = CreateConVar("u_esp_friends", "1", "Show friends");
	this->v_bVisCheck = CreateConVar("u_esp_vischeck", "1", "Visibility Checking");
	this->v_bLegit = CreateConVar("u_esp_legit", "0", "'legit' esp mode");
	this->v_iLegitSeenTicks = CreateConVar("u_esp_legit_seenticks", "150", "Ticks the entity is still shown after being hidden");
	v_bShowDroppedWeapons = CreateConVar("u_esp_item_weapons", "0", "Show dropped weapons");
	v_bShowAmmoPacks = CreateConVar("u_esp_item_ammo", "0", "Show ammo packs");
	v_bShowHealthPacks = CreateConVar("u_esp_item_health", "1", "Show health packs");
	v_bShowPowerups = CreateConVar("u_esp_item_powerups", "1", "Show powerups");
	v_bShowHealthNumbers = CreateConVar("u_esp_health_num", "1", "Show health number");
}

#define ESP_HEIGHT 14

void HEsp::DrawBox(CachedEntity* ent, Color clr, float widthFactor, float addHeight, bool healthbar, int health, int healthmax) {
	if (!CheckCE(ent)) return;
	Vector min, max;
	ent->m_pEntity->GetRenderBounds(min, max);
	Vector origin = ent->m_pEntity->GetAbsOrigin();
	Vector so;
	draw::WorldToScreen(origin, so);
	//if (!a) return;
	//logging::Info("%f %f", so.x, so.y);
	Vector omin, omax;
	omin = origin + Vector(0, 0, min.z);
	omax = origin + Vector(0, 0, max.z + addHeight);
	Vector smin, smax;
	bool a = draw::WorldToScreen(omin, smin);
	a = a && draw::WorldToScreen(omax, smax);
	if (!a) return;
	float height = abs(smax.y - smin.y);
	//logging::Info("height: %f", height);
	float width = height / widthFactor;
	//bool a = draw::WorldToScreen(omin, smin);
	//a = a && draw::WorldToScreen(omax, smax);
	//if (!a) return;
	//draw::DrawString(min(smin.x, smax.x), min(smin.y, smax.y), clr, false, "min");
	//draw::DrawString(max(smin.x, smax.x), max(smin.y, smax.y), clr, false, "max");
	//draw::DrawString((int)so.x, (int)so.y, draw::white, false, "origin");
	ent->m_ESPOrigin.x = so.x + width / 2 + 1;
	ent->m_ESPOrigin.y = so.y - height;
	draw::OutlineRect(so.x - width / 2 - 1, so.y - 1 - height, width + 2, height + 2, draw::black);
	draw::OutlineRect(so.x - width / 2, so.y - height, width, height, clr);
	draw::OutlineRect(so.x - width / 2 + 1, so.y + 1 - height, width - 2, height - 2, draw::black);
	if (healthbar) {
		Color hp = colors::GetHealthColor(health, healthmax);
		int hbh = (height) * min((float)health / (float)healthmax, 1.0f);
		draw::DrawRect(so.x - width / 2 - 7, so.y - 1 - height, 6, height + 2, draw::black);
		draw::DrawRect(so.x - width / 2 - 6, so.y - hbh, 5, hbh, hp);
	}
	//draw::OutlineRect(min(smin.x, smax.x) - 1, min(smin.y, smax.y) - 1, max(smin.x, smax.x), max(smin.y, smax.y), draw::black);
	//draw::OutlineRect(min(smin.x, smax.x), min(smin.y, smax.y), max(smin.x, smax.x), max(smin.y, smax.y), clr);
}

void HEsp::ProcessEntityPT(CachedEntity* ent) {
	if (!this->v_bEnabled->GetBool()) return;
	if (!this->v_bBox->GetBool()) return;
	if (!CheckCE(ent)) return;
	if (ent->m_IDX == interfaces::engineClient->GetLocalPlayer()) return;
	Color color;
	switch (ent->m_iClassID) {
	case ClassID::CTFPlayer: {
		if (v_bLegit->GetBool() && ent->m_iTeam != g_pLocalPlayer->team && !GetRelation(ent->m_pEntity)) {
			if (ent->Var<int>(eoffsets.iCond) & cond::cloaked) return;
			if (ent->m_lLastSeen > v_iLegitSeenTicks->GetInt()) {
				return;
			}
		}
		if (ent->Var<int>(eoffsets.iTeamNum) == g_pLocalPlayer->team && !v_bTeammates->GetBool() && !GetRelation(ent->m_pEntity)) break;
		if (!ent->m_bAlivePlayer) break;
		color = colors::GetTeamColor(ent->m_iTeam, !ent->m_bIsVisible);
		switch (GetRelation(ent->m_pEntity)) {
		case relation::FRIEND:
			color = colors::green;
			break;
		case relation::RAGE:
			color = colors::yellow;
			break;
		}
		DrawBox(ent, color, 3.0f, -15.0f, true, ent->Var<int>(eoffsets.iHealth), ent->m_iMaxHealth);
	break;
	}
	case ClassID::CObjectSentrygun:
	case ClassID::CObjectDispenser:
	case ClassID::CObjectTeleporter: {
		if (v_bLegit->GetBool() && ent->m_iTeam != g_pLocalPlayer->team) {
			if (ent->m_lLastSeen > v_iLegitSeenTicks->GetInt()) {
				return;
			}
		}
		if (ent->Var<int>(eoffsets.iTeamNum) == g_pLocalPlayer->team && !v_bTeammates->GetBool()) break;
		color = colors::GetTeamColor(ent->Var<int>(eoffsets.iTeamNum), false);
		DrawBox(ent, color, 1.0f, 0.0f, true, ent->Var<int>(eoffsets.iBuildingHealth), ent->Var<int>(eoffsets.iBuildingMaxHealth));
	break;
	}
	}
}

void HEsp::ProcessEntity(CachedEntity* ent) {
	if (!this->v_bEnabled->GetBool()) return;
	if (!CheckCE(ent)) return;

	Color color = draw::white;
	Color bgclr = draw::black;

	if (v_bEntityESP->GetBool()) {
		ent->AddESPString(color, bgclr, "%s [%i]", ent->m_pEntity->GetClientClass()->m_pNetworkName, ent->m_iClassID);
	}

	switch (ent->m_iClassID) {
	case ClassID::CTFDroppedWeapon: {
		if (!this->v_bItemESP->GetBool()) break;
		if (!this->v_bShowDroppedWeapons->GetBool()) break;
		ent->AddESPString(color, bgclr, "WEAPON");
		if (this->v_bShowDistance) {
			ent->AddESPString(color, bgclr, "%im", (int)(ent->m_flDistance / 64 * 1.22f));
		}
		break;
	}
	case ClassID::CTFAmmoPack: {
		if (!this->v_bItemESP->GetBool()) break;
		if (!this->v_bShowAmmoPacks->GetBool()) break;
		ent->AddESPString(color, bgclr, "++ AMMO");
		if (this->v_bShowDistance) {
			ent->AddESPString(color, bgclr, "%im", (int)(ent->m_flDistance / 64 * 1.22f));
		}
		break;
	}
	case ClassID::CBaseAnimating: {
		if (!this->v_bItemESP->GetBool()) break;
		item_type type = GetItemType(ent->m_pEntity);
		if (type == item_type::item_null) break;
		bool shown = false;
		if (type >= item_medkit_small && type <= item_medkit_large && this->v_bShowHealthPacks->GetBool()) {
			color = colors::green;
			ent->AddESPString(color, bgclr,"%s HEALTH", packs[type - item_medkit_small]);
			shown = true;
		} else if (type >= item_ammo_small && type <= item_ammo_large && this->v_bShowAmmoPacks->GetBool()) {
			ent->AddESPString(color, bgclr,"%s AMMO", packs[type - item_ammo_small]);
			shown = true;
		} else if (type >= item_mp_strength && type <= item_mp_crit && this->v_bShowPowerups->GetBool()) {
			int skin = ent->m_pEntity->GetSkin();
			if (skin == 1) {
				color = draw::red;
			} else if (skin == 2) {
				color = draw::blue;
			} else {
				color = draw::yellow;
			}
			ent->AddESPString(color, bgclr, "%s PICKUP", powerups[type - item_mp_strength]);
			shown = true;
		}
		if (this->v_bShowDistance && shown) {
			ent->AddESPString(color, bgclr, "%im", (int)(ent->m_flDistance / 64 * 1.22f));
		}
		break;
	}
	case ClassID::CTFPlayer: {
		if (!ent->m_bAlivePlayer) break;
		if (ent->m_IDX == interfaces::engineClient->GetLocalPlayer()) break;
		int pclass = ent->Var<int>(eoffsets.iClass);
		int pcond = ent->Var<int>(eoffsets.iCond);
		player_info_t info;
		if (!interfaces::engineClient->GetPlayerInfo(ent->m_IDX, &info)) return;
		powerup_type power = GetPowerupOnPlayer(ent->m_pEntity);
		// If target is enemy, always show powerups, if player is teammate, show powerups
		// only if bTeammatePowerup or bTeammates is true
		color = colors::GetTeamColor(ent->m_iTeam, !ent->m_bIsVisible);
		switch (GetRelation(ent->m_pEntity)) {
		case relation::FRIEND:
			color = colors::green;
			break;
		case relation::RAGE:
			color = colors::yellow;
			break;
		}
		bgclr = colors::GetTeamBgColor(ent->m_iTeam, !ent->m_bIsVisible);

		// TODO TEMP
		//ent->AddESPString(color, "FOV %f", GetFov(g_pLocalPlayer->v_OrigViewangles, g_pLocalPlayer->v_Eye, ent->m_pEntity->GetAbsOrigin()));
		if (v_bLegit->GetBool() && ent->m_iTeam != g_pLocalPlayer->team  && !GetRelation(ent->m_pEntity)) {
			if (pcond & cond::cloaked) return;
			if (ent->m_lLastSeen > (unsigned)v_iLegitSeenTicks->GetInt()) {
				return;
			}
		}
		if (power >= 0 && (ent->m_bEnemy || this->v_bTeammatePowerup->GetBool() || this->v_bTeammates->GetBool())) {
			ent->AddESPString(color, bgclr, "HAS [%s]", powerups[power]);
		}
		if (ent->m_bEnemy || v_bTeammates->GetBool() || GetRelation(ent->m_pEntity)) {
			ent->AddESPString(color, bgclr, "%s", info.name);
			if (v_bShowFriendID->GetBool()) {
				ent->AddESPString(color, bgclr, "%lu", info.friendsID);
			}
			if (pclass > 0 && pclass < 10)
				ent->AddESPString(color, bgclr, "%s", classes[pclass - 1]);
			if (this->v_bShowHealthNumbers->GetBool()) {
				ent->AddESPString(color, bgclr, "%i / %i HP", ent->m_iHealth, ent->m_iMaxHealth);
			}
			if (pcond & cond::cloaked) {
				ent->AddESPString(color, bgclr, "CLOAKED");
			}
			if (IsPlayerInvulnerable(ent->m_pEntity)) {
				ent->AddESPString(color, bgclr, "INVULNERABLE");
			}
			if (IsPlayerCritBoosted(ent->m_pEntity)) {
				ent->AddESPString(color, bgclr, "CRIT BOOSTED");
			}
			if (this->v_bShowDistance) {
				ent->AddESPString(color, bgclr, "%im", (int)(ent->m_flDistance / 64 * 1.22f));
			}
		}
		break;
	}
	case ClassID::CObjectSentrygun:
	case ClassID::CObjectDispenser:
	case ClassID::CObjectTeleporter: {
		if (!ent->m_bEnemy && !v_bTeammates->GetBool()) break;
		int level = ent->Var<int>(eoffsets.iUpgradeLevel);
		const char* name = (ent->m_iClassID == 89 ? "Teleporter" : (ent->m_iClassID == 88 ? "Sentry Gun" : "Dispenser"));
		color = colors::GetTeamColor(ent->m_iTeam, !ent->m_bIsVisible);
		if (v_bLegit->GetBool() && ent->m_iTeam != g_pLocalPlayer->team) {
			if (ent->m_lLastSeen > v_iLegitSeenTicks->GetInt()) {
				return;
			}
		}
		bgclr = colors::GetTeamBgColor(ent->m_iTeam, !ent->m_bIsVisible);

		ent->AddESPString(color, bgclr, "LV %i %s", level, name);
		if (this->v_bShowHealthNumbers->GetBool()) {
			ent->AddESPString(color, bgclr, "%i / %i HP", ent->m_iHealth, ent->m_iMaxHealth);
		}
		if (this->v_bShowDistance) {
			ent->AddESPString(color, bgclr, "%im", (int)(ent->m_flDistance / 64 * 1.22f));
		}
		break;
	}
	}
	/*
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

HEsp* g_phEsp;

