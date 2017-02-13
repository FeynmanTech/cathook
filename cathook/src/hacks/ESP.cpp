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

void ESP::Draw() {
	if (CE_BAD(g_pLocalPlayer->entity)) return;
	for (int i = 0; i < HIGHEST_ENTITY; i++) {
		ProcessEntityPT(ENTITY(i));
	}
}

ESP::ESP() {
	this->v_bSeeLocal = new CatVar(CV_SWITCH, "esp_local", "1", "ESP Local Player", NULL, "Shows local player ESP in thirdperson");
	this->v_bEnabled = new CatVar(CV_SWITCH, "esp_enabled", "0", "ESP", NULL, "Master ESP switch");
	this->v_bEntityESP = new CatVar(CV_SWITCH, "esp_entity", "0", "Entity ESP", NULL, "Show entity info (debug)");
	this->v_bTeammates = new CatVar(CV_SWITCH, "esp_teammates", "0", "ESP Teammates", NULL, "Teammate ESP");
	this->v_bItemESP = new CatVar(CV_SWITCH, "esp_item", "1", "Item ESP", NULL, "Master Item ESP switch (health packs, etc.)");
	this->v_bTeammatePowerup = new CatVar(CV_SWITCH, "esp_powerup_team", "1", "Teammate powerups", NULL, "Show Mannpower powerups on your teammates");
	this->v_bShowEntityID = new CatVar(CV_SWITCH, "esp_entity_id", "1", "Entity ID", NULL, "Used with Entity ESP. Shows entityID");
	this->v_bShowDistance = new CatVar(CV_SWITCH, "esp_distance", "1", "Distance ESP", NULL, "Show distance to target");
	this->v_bBox = new CatVar(CV_SWITCH, "esp_box", "1", "Box", NULL, "Draw 2D box with healthbar. fancy.");
	this->v_bShowFriendID = new CatVar(CV_SWITCH, "esp_friendid", "0", "Show FriendID", NULL, "Show SteamID");
	this->v_bShowFriends = new CatVar(CV_SWITCH, "esp_friends", "1", "Show friends", NULL, "Always show friends");
	this->v_bVisCheck = new CatVar(CV_SWITCH, "esp_vischeck", "1", "VisCheck", NULL, "ESP visibility check - makes enemy info behind walls darker, disable this if you get FPS drops");
	this->v_bLegit = new CatVar(CV_SWITCH, "esp_legit", "0", "Legit Mode", NULL, "Don't show invisible enemies");
	this->v_iLegitSeenTicks = new CatVar(CV_INT, "esp_legit_seenticks", "150", "Legit delay", NULL, "Delay after enemy gone behind a wall where you can still see them", true, 200.0);
	v_bShowDroppedWeapons = new CatVar(CV_SWITCH, "esp_item_weapons", "0", "Dropped weapons", NULL, "Show dropped weapons");
	v_bShowAmmoPacks = new CatVar(CV_SWITCH, "esp_item_ammo", "0", "Ammo packs", NULL, "Show ammo packs");
	v_bShowHealthPacks = new CatVar(CV_SWITCH, "esp_item_health", "1", "Health packs", NULL, "Show health packs");
	v_bShowPowerups = new CatVar(CV_SWITCH, "esp_item_powerups", "1", "Powerups", NULL, "Show powerups");
	this->v_bShowTank = new CatVar(CV_SWITCH, "esp_show_tank", "1", "Show tank", NULL, "Show tank");
	v_bShowHealthNumbers = new CatVar(CV_SWITCH, "esp_health_num", "1", "Health numbers", NULL, "Show health in numbers");
	v_bShowMoney = new CatVar(CV_SWITCH, "esp_money", "1", "MvM money", NULL, "Show MvM money");
	v_bShowRedMoney = new CatVar(CV_SWITCH, "esp_money_red", "1", "Red MvM money", NULL, "Show/Hide red MvM money");
	CatEnum* proj = new CatEnum({ "OFF", "ALL", "CRIT" });
	this->v_iShowRockets = new CatVar(CV_ENUM, "esp_proj_rockets", "1", "Rockets", proj, "Rockets");
	this->v_iShowArrows = new CatVar(CV_ENUM, "esp_proj_arrows", "1", "Arrows", proj, "Arrows");
	this->v_iShowStickies = new CatVar(CV_ENUM, "esp_proj_stickies", "1", "Stickies", proj, "Stickybombs");
	this->v_iShowPipes = new CatVar(CV_ENUM, "esp_proj_pipes", "1", "Pipes", proj, "Pipebombs");
	this->v_bOnlyEnemyProjectiles = new CatVar(CV_SWITCH, "esp_proj_enemy", "1", "Only enemy projectiles", NULL, "Don't show friendly projectiles");
	this->v_bProjectileESP = new CatVar(CV_SWITCH, "esp_proj", "1", "Projectile ESP", NULL, "Projectile ESP");
	v_bShowName = new CatVar(CV_SWITCH, "esp_name", "1", "Name ESP", NULL, "Show name");
	v_bShowClass = new CatVar(CV_SWITCH, "esp_class", "1", "Class ESP", NULL, "Show class");
	v_bShowConditions = new CatVar(CV_SWITCH, "esp_conds", "1", "Conditions ESP", NULL, "Show conditions");
	this->v_bBuildingESP = new CatVar(CV_SWITCH, "esp_buildings", "1", "Building ESP", NULL, "Show buildings");
	this->v_bShowWeaponSpawners = new CatVar(CV_SWITCH, "esp_weapon_spawners", "1", "Show weapon spawners", NULL, "TF2C deathmatch weapon spawners");
	v_bShowAdrenaline = new CatVar(CV_SWITCH, "esp_item_adrenaline", "0", "Show Adrenaline", NULL, "TF2C adrenaline pills");

	v_bModelName = new CatVar(CV_SWITCH, "esp_model_name", "0", "Model name ESP", NULL, "Model name esp (DEBUG ONLY)");
}

#define ESP_HEIGHT 14

void ESP::DrawBox(CachedEntity* ent, int clr, float widthFactor, float addHeight, bool healthbar, int health, int healthmax) {
	if (CE_BAD(ent)) return;
	bool cloak = ent->m_iClassID == g_pClassID->C_Player && IsPlayerInvisible(ent);
	Vector min, max;
	RAW_ENT(ent)->GetRenderBounds(min, max);
	Vector origin = RAW_ENT(ent)->GetAbsOrigin();
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
	unsigned char alpha = clr >> 24;
	float trf = (float)((float)alpha / 255.0f);
	int border = cloak ? colors::Create(160, 160, 160, alpha) : colors::Transparent(colors::black, trf);

	draw::OutlineRect(so.x - width / 2 - 1, so.y - 1 - height, width + 2, height + 2, border);
	draw::OutlineRect(so.x - width / 2, so.y - height, width, height, clr);
	draw::OutlineRect(so.x - width / 2 + 1, so.y + 1 - height, width - 2, height - 2, border);
	if (healthbar) {
		int hp = colors::Transparent(colors::Health(health, healthmax), trf);
		int hbh = (height) * min((float)health / (float)healthmax, 1.0f);
		draw::DrawRect(so.x - width / 2 - 7, so.y - 1 - height, 6, height + 2, border);
		draw::DrawRect(so.x - width / 2 - 6, so.y - hbh, 5, hbh, hp);
	}
	//draw::OutlineRect(min(smin.x, smax.x) - 1, min(smin.y, smax.y) - 1, max(smin.x, smax.x), max(smin.y, smax.y), draw::black);
	//draw::OutlineRect(min(smin.x, smax.x), min(smin.y, smax.y), max(smin.x, smax.x), max(smin.y, smax.y), clr);
}

void ESP::ProcessEntityPT(CachedEntity* ent) {
	if (!this->v_bEnabled->GetBool()) return;
	if (!this->v_bBox->GetBool()) return;
	if (CE_BAD(ent)) return;
	if (!(this->v_bSeeLocal->GetBool() && interfaces::iinput->CAM_IsThirdPerson()) &&
		ent->m_IDX == interfaces::engineClient->GetLocalPlayer()) return;
	int fg = ent->m_ESPColorFG;
	switch (ent->m_Type) {
	case ENTITY_PLAYER: {
		bool cloak = IsPlayerInvisible(ent);
		if (v_bLegit->GetBool() && ent->m_iTeam != g_pLocalPlayer->team && !GetRelation(ent)) {
			if (cloak) return;
			if (ent->m_lLastSeen > v_iLegitSeenTicks->GetInt()) {
				return;
			}
		}
		if (!ent->m_bEnemy && !v_bTeammates->GetBool() && !GetRelation(ent)) break;
		if (!ent->m_bAlivePlayer) break;

		DrawBox(ent, fg, 3.0f, -15.0f, true, CE_INT(ent, netvar.iHealth), ent->m_iMaxHealth);
	break;
	}
	case ENTITY_BUILDING: {
		if (v_bLegit->GetBool() && ent->m_iTeam != g_pLocalPlayer->team) {
			if (ent->m_lLastSeen > v_iLegitSeenTicks->GetInt()) {
				return;
			}
		}
		if (CE_INT(ent, netvar.iTeamNum) == g_pLocalPlayer->team && !v_bTeammates->GetBool()) break;
		DrawBox(ent, fg, 1.0f, 0.0f, true, CE_INT(ent, netvar.iBuildingHealth), CE_INT(ent, netvar.iBuildingMaxHealth));
	break;
	}
	}
}

void ESP::AddProcessor(ESPEntityProcessor processor) {
	m_processors.push_back(processor);
}

void ESP::CreateProcessors() {
	// Common entity processors.
	AddProcessor(ESPEntityProcessor(
		[this](CachedEntity*) -> bool {
			return v_bEntityESP->GetBool();
		},
		[this](CachedEntity* entity) -> bool {
			entity->AddESPString(format('#', entity->m_IDX, ' ', '[', entity->m_iClassID, ']', ' ', '"', entity->m_pClass->GetName()));
			if (v_bModelName->GetBool())
				entity->AddESPString(std::string(interfaces::model->GetModelName(RAW_ENT(entity)->GetModel())));
			return true;
		}
	));
	AddProcessor(ESPEntityProcessor(
		[this](CachedEntity* entity) -> bool {
			return entity->GetItemType() != ITEM_NONE;
		},
		[this](CachedEntity* entity) -> bool {
			k_EItemType type = entity->GetItemType();
			if (v_bShowHealthPacks->GetBool() && type >= ITEM_HEALTH_SMALL && type <= ITEM_HEALTH_LARGE) {
				switch (type) {
				case ITEM_HEALTH_SMALL:
					entity->AddESPString("[+]"); break;
				case ITEM_HEALTH_MEDIUM:
					entity->AddESPString("[++]"); break;
				case ITEM_HEALTH_LARGE:
					entity->AddESPString("[+++]"); break;
				}
			} else if (v_bShowAmmoPacks->GetBool() && type >= ITEM_AMMO_SMALL && type <= ITEM_AMMO_LARGE) {
				switch (type) {
				case ITEM_AMMO_SMALL:
					entity->AddESPString("{i}"); break;
				case ITEM_AMMO_MEDIUM:
					entity->AddESPString("{ii}"); break;
				case ITEM_AMMO_LARGE:
					entity->AddESPString("{iii}"); break;
				}
			} else if (v_bShowAdrenaline->GetBool() && type == ITEM_TF2C_PILL) {
				entity->AddESPString("[A]");
			} else if (v_bShowPowerups->GetBool() && type >= ITEM_POWERUP_FIRST && type <= ITEM_POWERUP_LAST) {
				entity->AddESPString(format(powerup_names[type - ITEM_POWERUP_FIRST], " POWERUP"));
			} else if (v_bShowWeaponSpawners->GetBool() && type >= ITEM_TF2C_W_FIRST && type <= ITEM_TF2C_W_LAST) {
				entity->AddESPString(format(tf2c_weapon_names[type - ITEM_TF2C_W_FIRST], " SPAWNER"));
				if (CE_BYTE(entity, netvar.bRespawning))
					entity->AddESPString("-- RESPAWNING --");
			} else if (v_bShowPowerups->GetBool() && type == ITEM_HL_BATTERY) {
				entity->AddESPString("[Z]");
			} else return false;
			return true;
		}
	));
	// TF entity processors
	AddProcessor(ESPEntityProcessor(
		[this](CachedEntity* entity) -> bool {
			if (entity->m_Type != ENTITY_PROJECTILE) return false;
			if (!v_bProjectileESP->GetBool()) return false;
			if (!entity->m_bEnemy) {
				if (!v_bTeammates->GetBool()) return false;
				else if (v_bOnlyEnemyProjectiles->GetBool()) return false;
			}
			bool critical = entity->m_bCritProjectile;
			CatVar* checker = 0;
			switch (entity->GetProjectileType()) {
			case PROJ_ARROW:
				checker = v_iShowArrows; break;
			case PROJ_PIPE:
				checker = v_iShowPipes; break;
			case PROJ_ROCKET:
				checker = v_iShowRockets; break;
			case PROJ_STICKY:
				checker = v_iShowStickies; break;
			}
			if (checker) {
				int mode = checker->GetInt();
				return (mode == 1 || (mode == 2 && critical));
			} else return false;
		},
		[this](CachedEntity* entity) -> bool {
			switch (entity->GetProjectileType()) {
			case PROJ_ARROW:
				entity->AddESPString("[ >>----> ]"); break;
			case PROJ_PIPE:
				entity->AddESPString("[ (=|=) ]"); break;
			case PROJ_STICKY:
				entity->AddESPString("[ {*S*} ]"); break;
			case PROJ_ROCKET:
				entity->AddESPString("[ ====> ]"); break;
			}
			return true;
		}
	));
	AddProcessor(ESPEntityProcessor(
		[this](CachedEntity* entity) -> bool {
			return (v_bBuildingESP->GetBool() && entity->GetBuildingType() != BUILD_NONE);
		},
		[this](CachedEntity* entity) -> bool {
			if (v_bShowClass->GetBool()) { // TODO show building type...
				switch (entity->GetBuildingType()) {
				case BUILD_SENTRYGUN:
					entity->AddESPString(format("LV ", CE_INT(entity, netvar.iUpgradeLevel), " SENTRY"));
					break;
				case BUILD_DISPENSER:
					entity->AddESPString(format("LV ", CE_INT(entity, netvar.iUpgradeLevel), " DISPENSER"));
					break;
				case BUILD_TELEPORTER:
					entity->AddESPString(format("LV ", CE_INT(entity, netvar.iUpgradeLevel), " TELEPORTER"));
					break;
				}
			}
			if (v_bShowHealthNumbers->GetBool()) entity->AddESPString(format(entity->m_iHealth, '/', entity->m_iMaxHealth, " HP"));
		}
	));
	// TF2 Entity Processors
	AddProcessor(ESPEntityProcessor(
		[this](CachedEntity* entity) -> bool {
			return (entity->m_iClassID == g_pClassID->CTFTankBoss && v_bShowTank->GetBool());
		},
		[this](CachedEntity* entity) -> bool {
			entity->AddESPString("[: TANK :]");
			return true;
		}
	));
	AddProcessor(ESPEntityProcessor(
		[this](CachedEntity* entity) {
			if (!v_bShowMoney->GetBool() || entity->m_iClassID != g_pClassID->CCurrencyPack) return false;
			if (CE_BYTE(entity, netvar.bDistributed)) {
				return v_bShowRedMoney->GetBool();
			} else return true;
		},
		[this](CachedEntity* entity) {
			entity->AddESPString("$$$");
			return true;
		}
	));

}

void ESP::ProcessEntity(CachedEntity* ent) {
	if (!this->v_bEnabled->GetBool()) return;
	if (CE_BAD(ent)) return;

	ent->m_ESPColorFG = colors::EntityF(ent);

	for (auto i : m_processors) {
		if (i.Validate(ent))
			i.Process(ent);
	}

	if (HL2DM) {
		/*if (v_bItemESP->GetBool() && v_bShowDroppedWeapons->GetBool()) {
			if (CE_BYTE(ent, netvar.hOwner) == (unsigned char)-1) {
				int a = ent->m_nESPStrings;
				if (ent->m_iClassID == g_pClassID->CWeapon_SLAM) ent->AddESPString("SLAM");
				else if (ent->m_iClassID == g_pClassID->CWeapon357) ent->AddESPString(".357");
				else if (ent->m_iClassID == g_pClassID->CWeaponAR2) ent->AddESPString("AR2");
				else if (ent->m_iClassID == g_pClassID->CWeaponAlyxGun) ent->AddESPString("Alyx Gun");
				else if (ent->m_iClassID == g_pClassID->CWeaponAnnabelle) ent->AddESPString("Annabelle");
				else if (ent->m_iClassID == g_pClassID->CWeaponBinoculars) ent->AddESPString("Binoculars");
				else if (ent->m_iClassID == g_pClassID->CWeaponBugBait) ent->AddESPString("Bug Bait");
				else if (ent->m_iClassID == g_pClassID->CWeaponCrossbow) ent->AddESPString("Crossbow");
				else if (ent->m_iClassID == g_pClassID->CWeaponShotgun) ent->AddESPString("Shotgun");
				else if (ent->m_iClassID == g_pClassID->CWeaponSMG1) ent->AddESPString("SMG");
				else if (ent->m_iClassID == g_pClassID->CWeaponRPG) ent->AddESPString("RPG");
				if (a != ent->m_nESPStrings) {
					ent->m_ESPColorFG = colors::yellow;
					if (this->v_bShowDistance->GetBool()) {
						ent->AddESPString("%im", (int)(ent->m_flDistance / 64 * 1.22f));
					}
				}
			}
		}
		if (v_bItemESP->GetBool() && v_bShowHealthPacks->GetBool()) {
			//if (ent->m_iClassID == g_pClassID->)
		}*/
	/*} else if (ent->m_Type == ENTITY_PLAYER && ent->m_bAlivePlayer) {
		if (!(this->v_bSeeLocal->GetBool() && interfaces::iinput->CAM_IsThirdPerson()) &&
			ent->m_IDX == interfaces::engineClient->GetLocalPlayer()) return;
		int pclass = CE_INT(ent, netvar.iClass);
		player_info_t info;
		if (!interfaces::engineClient->GetPlayerInfo(ent->m_IDX, &info)) return;
		powerup_type power = GetPowerupOnPlayer(ent);
		// If target is enemy, always show powerups, if player is teammate, show powerups
		// only if bTeammatePowerup or bTeammates is true
		if (v_bLegit->GetBool() && ent->m_iTeam != g_pLocalPlayer->team  && !GetRelation(ent)) {
			if (IsPlayerInvisible(ent)) return;
			if (ent->m_lLastSeen > (unsigned)v_iLegitSeenTicks->GetInt()) {
				return;
			}
		}
		if (power >= 0 && (ent->m_bEnemy || this->v_bTeammatePowerup->GetBool() || this->v_bTeammates->GetBool())) {
			ent->AddESPString("HAS [%s]", powerups[power]);
		}
		if (ent->m_bEnemy || v_bTeammates->GetBool() || GetRelation(ent)) {
			if (v_bShowName->GetBool())
				ent->AddESPString("%s", info.name);
			if (v_bShowFriendID->GetBool()) {
				ent->AddESPString("%lu", info.friendsID);
			}
			if (v_bShowClass->GetBool()) {
				if (pclass > 0 && pclass < 10)
					ent->AddESPString("%s", classes[pclass - 1]);
			}
			if (this->v_bShowHealthNumbers->GetBool()) {
				ent->AddESPString("%i / %i HP", ent->m_iHealth, ent->m_iMaxHealth);
				ent->GetESPString(ent->m_nESPStrings - 1).m_bColored = true;
				if (v_bVisCheck->GetBool()) {
					ent->GetESPString(ent->m_nESPStrings - 1).m_nColor = colors::Transparent(colors::Health(ent->m_iHealth, ent->m_iMaxHealth), ent->IsVisible() ? 1.0 : 0.5f);
				} else {
					ent->GetESPString(ent->m_nESPStrings - 1).m_nColor = colors::Health(ent->m_iHealth, ent->m_iMaxHealth);
				}
			}
			if (v_bShowConditions->GetBool() && TF) {
				if (IsPlayerInvisible(ent)) {
					ent->AddESPString("INVISIBLE");
				}
				if (IsPlayerInvulnerable(ent)) {
					ent->AddESPString("INVULNERABLE");
				}
				if (HasCondition(ent, TFCond_UberBulletResist)) {
					ent->AddESPString("VACCINATOR ACTIVE");
				}
				if (HasCondition(ent, TFCond_SmallBulletResist)) {
					ent->AddESPString("VACCINATOR PASSIVE");
				}
				if (IsPlayerCritBoosted(ent)) {
					ent->AddESPString("CRIT BOOSTED");
				}
			}
			if (this->v_bShowDistance->GetBool()) {
				ent->AddESPString("%im", (int)(ent->m_flDistance / 64 * 1.22f));
			}
		}
		return;
	}*/
	}
}

void ESP::ProcessUserCmd(CUserCmd*) {
	if (CE_BAD(g_pLocalPlayer->entity)) return;
	for (int i = 0; i < HIGHEST_ENTITY; i++) {
		CachedEntity* ent = ENTITY(i);
		if (CE_GOOD(ent)) { ProcessEntity(ent); }
	}
};
