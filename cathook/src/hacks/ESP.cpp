/*
 * HEsp.cpp
 *
 *  Created on: Oct 6, 2016
 *      Author: nullifiedcat
 */

#include "ESP.h"

#include "../common.h"
#include "../sdk.h"

DEFINE_HACK_SINGLETON(ESP);

const char* ESP::GetName() {
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

void ESP::PaintTraverse(void*, unsigned int, bool, bool) {
	for (int i = 0; i < gEntityCache.m_nMax; i++) {
		ProcessEntityPT(gEntityCache.GetEntity(i));
	}
}

ESP::ESP() {
	this->v_bEnabled = CreateConVar(CON_PREFIX "esp_enabled", "0", "ESP");
	this->v_bEntityESP = CreateConVar(CON_PREFIX "esp_entity", "0", "Entity ESP");
	this->v_bTeammates = CreateConVar(CON_PREFIX "esp_teammates", "0", "ESP Teammates");
	this->v_bItemESP = CreateConVar(CON_PREFIX "esp_item", "1", "Item ESP");
	this->v_bTeammatePowerup = CreateConVar(CON_PREFIX "esp_powerup_team", "1", "Teammate powerups");
	this->v_bShowEntityID = CreateConVar(CON_PREFIX "esp_entity_id", "0", "Entity ID");
	this->v_bShowDistance = CreateConVar(CON_PREFIX "esp_distance", "1", "Distance ESP");
	this->v_bBox = CreateConVar(CON_PREFIX "esp_box", "1", "Box");
	this->v_bShowFriendID = CreateConVar(CON_PREFIX "esp_friendid", "0", "Show FriendID");
	this->v_bShowFriends = CreateConVar(CON_PREFIX "esp_friends", "1", "Show friends");
	this->v_bVisCheck = CreateConVar(CON_PREFIX "esp_vischeck", "1", "VisCheck");
	this->v_bLegit = CreateConVar(CON_PREFIX "esp_legit", "0", "Legit Mode");
	this->v_iLegitSeenTicks = CreateConVar(CON_PREFIX "esp_legit_seenticks", "150", "Legit delay");
	v_bShowDroppedWeapons = CreateConVar(CON_PREFIX "esp_item_weapons", "0", "Dropped weapons");
	v_bShowAmmoPacks = CreateConVar(CON_PREFIX "esp_item_ammo", "0", "Ammo packs");
	v_bShowHealthPacks = CreateConVar(CON_PREFIX "esp_item_health", "1", "Health packs");
	v_bShowPowerups = CreateConVar(CON_PREFIX "esp_item_powerups", "1", "Powerups");
	this->v_bShowTank = CreateConVar(CON_PREFIX "esp_show_tank", "1", "Show tank");
	v_bShowHealthNumbers = CreateConVar(CON_PREFIX "esp_health_num", "1", "Health in numbers");
	v_bShowMoney = CreateConVar(CON_PREFIX "esp_money", "1", "MvM money");
	v_bShowRedMoney = CreateConVar(CON_PREFIX "esp_money_red", "1", "Red MvM money");
}

#define ESP_HEIGHT 14

void ESP::DrawBox(CachedEntity* ent, Color clr, float widthFactor, float addHeight, bool healthbar, int health, int healthmax) {
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
	unsigned char opacity = clr._color[3];
	Color blacko = draw::black;
	blacko._color[3] = opacity;
	draw::OutlineRect(so.x - width / 2 - 1, so.y - 1 - height, width + 2, height + 2, blacko);
	draw::OutlineRect(so.x - width / 2, so.y - height, width, height, clr);
	draw::OutlineRect(so.x - width / 2 + 1, so.y + 1 - height, width - 2, height - 2, blacko);
	if (healthbar) {
		Color hp = colors::GetHealthColor(health, healthmax);
		hp._color[3] = opacity;
		int hbh = (height) * min((float)health / (float)healthmax, 1.0f);
		draw::DrawRect(so.x - width / 2 - 7, so.y - 1 - height, 6, height + 2, blacko);
		draw::DrawRect(so.x - width / 2 - 6, so.y - hbh, 5, hbh, hp);
	}
	//draw::OutlineRect(min(smin.x, smax.x) - 1, min(smin.y, smax.y) - 1, max(smin.x, smax.x), max(smin.y, smax.y), draw::black);
	//draw::OutlineRect(min(smin.x, smax.x), min(smin.y, smax.y), max(smin.x, smax.x), max(smin.y, smax.y), clr);
}

void ESP::ProcessEntityPT(CachedEntity* ent) {
	if (!this->v_bEnabled->GetBool()) return;
	if (!this->v_bBox->GetBool()) return;
	if (!CheckCE(ent)) return;
	if (ent->m_IDX == interfaces::engineClient->GetLocalPlayer()) return;
	Color color;
	switch (ent->m_iClassID) {
	case ClassID::CTFPlayer: {
		if (v_bLegit->GetBool() && ent->m_iTeam != g_pLocalPlayer->team && !GetRelation(ent->m_pEntity)) {
			if (ent->Var<int>(netvar.iCond) & cond::cloaked) return;
			if (ent->m_lLastSeen > v_iLegitSeenTicks->GetInt()) {
				return;
			}
		}
		if (ent->Var<int>(netvar.iTeamNum) == g_pLocalPlayer->team && !v_bTeammates->GetBool() && !GetRelation(ent->m_pEntity)) break;
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
		DrawBox(ent, color, 3.0f, -15.0f, true, ent->Var<int>(netvar.iHealth), ent->m_iMaxHealth);
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
		if (ent->Var<int>(netvar.iTeamNum) == g_pLocalPlayer->team && !v_bTeammates->GetBool()) break;
		color = colors::GetTeamColor(ent->Var<int>(netvar.iTeamNum), !ent->m_bIsVisible);
		DrawBox(ent, color, 1.0f, 0.0f, true, ent->Var<int>(netvar.iBuildingHealth), ent->Var<int>(netvar.iBuildingMaxHealth));
	break;
	}
	}
}

void ESP::ProcessEntity(CachedEntity* ent) {
	if (!this->v_bEnabled->GetBool()) return;
	if (!CheckCE(ent)) return;

	Color color = draw::white;
	Color bgclr = draw::black;

	if (v_bEntityESP->GetBool()) {
		ent->AddESPString(color, bgclr, "%s [%i]", ent->m_pEntity->GetClientClass()->m_pNetworkName, ent->m_iClassID);
	}

	switch (ent->m_iClassID) {
	case ClassID::CTFTankBoss: {
		if (!this->v_bShowTank->GetBool()) break;
		ent->AddESPString(color, bgclr, "Tank");
	} break;
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
	case ClassID::CCurrencyPack: {
		if (!v_bShowMoney->GetBool()) break;
		if (false && ent->Var<int>(netvar.bDistributed)) {
			if (this->v_bShowRedMoney->GetBool()) {
				color = colors::tf_red;
				ent->AddESPString(color, bgclr, "$$$");
				ent->AddESPString(color, bgclr, "%im", (int)(ent->m_flDistance / 64 * 1.22f));
			}
		} else {
			color = colors::green;
			ent->AddESPString(color, bgclr, "$$$");
			ent->AddESPString(color, bgclr, "%im", (int)(ent->m_flDistance / 64 * 1.22f));
		}
	} break;
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
		int pclass = ent->Var<int>(netvar.iClass);
		int pcond = ent->Var<int>(netvar.iCond);
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
		int level = ent->Var<int>(netvar.iUpgradeLevel);
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
}

bool ESP::CreateMove(void*, float, CUserCmd*) {
	for (int i = 0; i < gEntityCache.m_nMax; i++) {
		ProcessEntity(gEntityCache.GetEntity(i));
	}
	return true;
};
