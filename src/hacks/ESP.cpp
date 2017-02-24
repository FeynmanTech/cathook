/*
 * HEsp.cpp
 *
 *  Created on: Oct 6, 2016
 *      Author: nullifiedcat
 */

#include "ESP.h"

#include "../common.h"
#include "../sdk.h"

const std::string tf_class_names_temporary[] = {
	"Scout", "Sniper",
	"Soldier", "Demoman",
	"Medic", "Heavy",
	"Pyro", "Spy",
	"Engineer", "Mercenary" };

namespace hacks { namespace shared { namespace esp {

CatEnum projectile_enum({ "OFF", "ALL", "CRIT" });

CatVar local(CV_SWITCH, "esp_local", "1", "ESP Local Player", NULL, "Shows local player ESP in thirdperson");
CatVar buildings(CV_SWITCH, "esp_buildings", "1", "Building ESP", NULL, "Show buildings");
CatVar enabled(CV_SWITCH, "esp_enabled", "0", "ESP", NULL, "Master ESP switch");
CatVar entity_info(CV_SWITCH, "esp_entity", "0", "Entity ESP", NULL, "Show entity info (debug)");
CatVar teammates(CV_SWITCH, "esp_teammates", "0", "ESP Teammates", NULL, "Teammate ESP");
CatVar item(CV_SWITCH, "esp_item", "1", "Item ESP", NULL, "Master Item ESP switch (health packs, etc.)");
CatVar item_weapons(CV_SWITCH, "esp_item_weapons", "0", "Dropped weapons", NULL, "Show dropped weapons");
CatVar item_ammo(CV_SWITCH, "esp_item_ammo", "0", "Ammo packs", NULL, "Show ammo packs");
CatVar item_health(CV_SWITCH, "esp_item_health", "1", "Health packs", NULL, "Show health packs");
CatVar item_powerup(CV_SWITCH, "esp_item_powerups", "1", "Powerups", NULL, "Show powerups");
CatVar item_money(CV_SWITCH, "esp_money", "1", "MvM money", NULL, "Show MvM money");
CatVar item_money_red(CV_SWITCH, "esp_money_red", "1", "Red MvM money", NULL, "Show/Hide red MvM money");
CatVar teammate_powerups(CV_SWITCH, "esp_powerup_team", "1", "Teammate powerups", NULL, "Show Mannpower powerups on your teammates");
CatVar entity_id(CV_SWITCH, "esp_entity_id", "1", "Entity ID", NULL, "Used with Entity ESP. Shows entityID");
CatVar tank(CV_SWITCH, "esp_show_tank", "1", "Show tank", NULL, "Show tank");
CatVar box(CV_SWITCH, "esp_box", "1", "Box", NULL, "Draw 2D box with healthbar. fancy.");
CatVar show_distance(CV_SWITCH, "esp_distance", "1", "Distance ESP", NULL, "Show distance to target");
CatVar show_friendid(CV_SWITCH, "esp_friendid", "0", "Show FriendID", NULL, "Show SteamID");
CatVar show_name(CV_SWITCH, "esp_name", "1", "Name ESP", NULL, "Show name");
CatVar show_class(CV_SWITCH, "esp_class", "1", "Class ESP", NULL, "Show class");
CatVar show_conds(CV_SWITCH, "esp_conds", "1", "Conditions ESP", NULL, "Show conditions");
CatVar vischeck(CV_SWITCH, "esp_vischeck", "1", "VisCheck (!)", NULL, "WARNING: Uses visibility checks! Can result in framerate drops. - makes esp behind walls darker");
CatVar legit(CV_SWITCH, "esp_legit", "0", "Legit Mode (!)", NULL, "WARNING: Uses visibility checks! Can result in framerate drops. - Don't show invisible enemies");
CatVar show_health(CV_SWITCH, "esp_health_num", "1", "Health numbers", NULL, "Show health in numbers");
//CatVar legit_ticks(CV_INT, "esp_legit_seenticks", "150", "Legit delay", NULL, "Delay after enemy gone behind a wall where you can still see them", true, 200.0);
CatVar proj_rockets(CV_ENUM, "esp_proj_rockets", "1", "Rockets", projectile_enum, "Rockets");
CatVar proj_arrows(CV_ENUM, "esp_proj_arrows", "1", "Arrows", projectile_enum, "Arrows");
CatVar proj_pipes(CV_ENUM, "esp_proj_pipes", "1", "Pipes", projectile_enum, "Pipebombs");
CatVar proj_stickies(CV_ENUM, "esp_proj_stickies", "1", "Stickies", projectile_enum, "Stickybombs");
CatVar proj_enemy(CV_SWITCH, "esp_proj_enemy", "1", "Only enemy projectiles", NULL, "Don't show friendly projectiles");
CatVar projectile(CV_SWITCH, "esp_proj", "1", "Projectile ESP", NULL, "Projectile ESP");
CatVar model_name(CV_SWITCH, "esp_model_name", "0", "Model name ESP", NULL, "Model name esp (DEBUG ONLY)");
CatVar item_dmweapons(CV_SWITCH, "esp_weapon_spawners", "1", "Show weapon spawners", NULL, "TF2C deathmatch weapon spawners");
CatVar item_adrenaline(CV_SWITCH, "esp_item_adrenaline", "0", "Show Adrenaline", NULL, "TF2C adrenaline pills");
CatVar show_powerups(CV_SWITCH, "esp_powerups", "1", "Show active powerups", NULL, "Shows powerups on players");

void DrawBox(CachedEntity* ent, float widthFactor, float addHeight, bool healthbar) {
	bool cloak = ent->clazz == g_pClassID->C_Player && IsPlayerInvisible(ent);
	Vector min, max;
	ent->entptr->GetRenderBounds(min, max);
	Vector origin = ent->Origin();
	Vector so;
	draw::WorldToScreen(origin, so);
	Vector omin, omax;
	omin = origin + Vector(0, 0, min.z);
	omax = origin + Vector(0, 0, max.z + addHeight);
	Vector smin, smax;
	bool a = draw::WorldToScreen(omin, smin);
	a = a && draw::WorldToScreen(omax, smax);
	if (!a) return;
	float height = abs(smax.y - smin.y);
	float width = height / widthFactor;
	ent->m_ESPOrigin.x = so.x + width / 2 + 1;
	ent->m_ESPOrigin.y = so.y - height;
	int clr = ent->ESPcolor;
	unsigned char alpha = clr >> 24;
	float trf = (float)((float)alpha / 255.0f);
	int border = cloak ? colors::Create(160, 160, 160, alpha) : colors::Transparent(colors::black, trf);
	draw::OutlineRect(so.x - width / 2 - 1, so.y - 1 - height, width + 2, height + 2, border);
	draw::OutlineRect(so.x - width / 2, so.y - height, width, height, clr);
	draw::OutlineRect(so.x - width / 2 + 1, so.y + 1 - height, width - 2, height - 2, border);
	if (healthbar) {
		int health = ent->Health();
		int healthmax = ent->MaxHealth();
		int hp = colors::Transparent(colors::Health(health, healthmax), trf);
		int hbh = (height) * min((float)health / (float)healthmax, 1.0f);
		draw::DrawRect(so.x - width / 2 - 7, so.y - 1 - height, 6, height + 2, border);
		draw::DrawRect(so.x - width / 2 - 6, so.y - hbh, 5, hbh, hp);
	}
}

void ProcessEntity(CUserCmd*, CachedEntity& entity) {
	if (!enabled) return;
	for (auto i : processors) {
		if (i.first(entity)) {
			i.second(entity);
			entity.data.esp_enabled = true;
		}
	}
	if (entity.data.esp_enabled) {
		if (show_distance) {
			entity.AddESPString(format(HU2M(entity.Distance()), " M"));
		}
	}
}

void DrawProcessEntity(CachedEntity& entity) {
	if (!enabled) return;
	if (!box) return;
	if (!entity.data.esp_enabled) return;
	switch (entity.Type()) {
	case ENTITY_PLAYER:
		DrawBox(&entity, 2.8f, -13.5f, true);
		break;
	case ENTITY_BUILDING:
		DrawBox(&entity, 1.2f, -2.0f, true);
		break;
	}
}

void CreateProcessors() {
	processors.push_back(entityprocessor_t(
		[this](CachedEntity*) -> bool {
			return entity_info;
		},
		[this](CachedEntity* entity) {
			entity->AddESPString(format('#', entity->m_IDX, ' ', '[', entity->clazz, ']', ' ', '"', entity->m_pClass->GetName()));
			if (model_name)
				entity->AddESPString(std::string(g_IModelInfo->GetModelName(entity->entptr->GetModel())));
		}
	));
	processors.push_back(entityprocessor_t(
		[this](CachedEntity* entity) -> bool {
			if (entity->Type() != ENTITY_PLAYER) return false;
			if (!teammates && !entity->Enemy()) return false;
			if (entity->var<bool>(netvar.iLifeState)) return false;
			if (entity->m_IDX == g_IEngine->GetLocalPlayer() && (!g_IInput->CAM_IsThirdPerson() || !local)) return false;
			if (legit && entity->Enemy() && (IsPlayerInvisible(entity) || !entity->IsVisible())) return false;
			return true;
		},
		[this](CachedEntity* entity) {
			powerup_type power = GetPowerupOnPlayer(entity);
			// If target is enemy, always show powerups, if player is teammate, show powerups
			// only if bTeammatePowerup or bTeammates is true
			if (power >= 0 && show_powerups) {
				entity->AddESPString(format("HAS ", powerups[power]));
			}
			player_info_s& info = entity->GetPlayerInfo();
			if (show_name)
				entity->AddESPString(info.name);
			if (show_friendid)
				entity->AddESPString(format(info.friendsID));
			if (show_class) {
				if (entity->Class() > 0 && entity->Class() <= 10)
					entity->AddESPString(tf_class_names_temporary[entity->Class() - 1]);
			}
			if (show_health) {
				entity->AddESPString(format(entity->Health(), '/', entity->MaxHealth(), " HP"));
				ESPStringCompound& string = entity->GetESPString(entity->string_count - 1);
				string.m_bColored = true;
				string.m_nColor = colors::Health(entity->Health(), entity->MaxHealth());
				if (vischeck && !entity->IsVisible())
					string.m_nColor = colors::Transparent(string.m_nColor);
			}
			if (TF && show_conds) {
				if (IsPlayerInvisible(ent)) {
					entity->AddESPString("INVISIBLE");
				}
				if (IsPlayerInvulnerable(ent)) {
					entity->AddESPString("INVULNERABLE");
				}
				if (HasCondition(entity, TFCond_UberBulletResist)) {
					entity->AddESPString("VACCINATOR ACTIVE");
				}
				if (HasCondition(entity, TFCond_SmallBulletResist)) {
					entity->AddESPString("VACCINATOR PASSIVE");
				}
				if (IsPlayerCritBoosted(entity)) {
					entity->AddESPString("CRIT BOOSTED");
				}
			}
		}
	));
	processors.push_back(entityprocessor_t(
		[this](CachedEntity* entity) -> bool {
			return entity->Item() != ITEM_NONE;
		},
		[this](CachedEntity* entity) {
			k_EItemType type = entity->Item();
			if (item_health && type >= ITEM_HEALTH_SMALL && type <= ITEM_HEALTH_LARGE) {
				switch (type) {
				case ITEM_HEALTH_SMALL:
					entity->AddESPString("[+]"); break;
				case ITEM_HEALTH_MEDIUM:
					entity->AddESPString("[++]"); break;
				case ITEM_HEALTH_LARGE:
					entity->AddESPString("[+++]"); break;
				}
			} else if (item_ammo && type >= ITEM_AMMO_SMALL && type <= ITEM_AMMO_LARGE) {
				switch (type) {
				case ITEM_AMMO_SMALL:
					entity->AddESPString("{i}"); break;
				case ITEM_AMMO_MEDIUM:
					entity->AddESPString("{ii}"); break;
				case ITEM_AMMO_LARGE:
					entity->AddESPString("{iii}"); break;
				}
			} else if (item_adrenaline && type == ITEM_TF2C_PILL) {
				entity->AddESPString("[A]");
			} else if (item_powerup && type >= ITEM_POWERUP_FIRST && type <= ITEM_POWERUP_LAST) {
				entity->AddESPString(format(powerup_names[type - ITEM_POWERUP_FIRST], " POWERUP"));
			} else if (item_dmweapons && type >= ITEM_TF2C_W_FIRST && type <= ITEM_TF2C_W_LAST) {
				entity->AddESPString(format(tf2c_weapon_names[type - ITEM_TF2C_W_FIRST], " SPAWNER"));
				if (entity->var<bool>(netvar.bRespawning))
					entity->AddESPString("-- RESPAWNING --");
			} else if (item_powerup && type == ITEM_HL_BATTERY) {
				entity->AddESPString("[Z]");
			}
		}
	));
	// TF entity processors
	processors.push_back(entityprocessor_t(
		[this](CachedEntity* entity) -> bool {
			if (!projectile) return false;
			if (entity->Type() != ENTITY_PROJECTILE) return false;
			if (!entity->Enemy()) {
				if (!teammates) return false;
				else if (proj_enemy) return false;
			}
			// TODO!!!
			bool critical = 0;
			CatVar* checker = 0;
			switch (entity->Projectile()) {
			case PROJ_ARROW:
				checker = &proj_arrows; break;
			case PROJ_PIPE:
				checker = &proj_pipes; break;
			case PROJ_ROCKET:
				checker = &proj_rockets; break;
			case PROJ_STICKY:
				checker = &proj_stickies; break;
			}
			if (checker) {
				int mode = checker->GetInt();
				return (mode == 1 || (mode == 2 && critical));
			} else return false;
		},
		[this](CachedEntity* entity) {
			switch (entity->Projectile()) {
			case PROJ_ARROW:
				entity->AddESPString("[ >>----> ]"); break;
			case PROJ_PIPE:
				entity->AddESPString("[ (=|=) ]"); break;
			case PROJ_STICKY:
				entity->AddESPString("[ {*S*} ]"); break;
			case PROJ_ROCKET:
				entity->AddESPString("[ ====> ]"); break;
			}
		}
	));
	processors.push_back(entityprocessor_t(
		[this](CachedEntity* entity) -> bool {
			return (buildings && entity->Building() != BUILD_NONE);
		},
		[this](CachedEntity* entity) {
			if (show_class) { // TODO show building type...
				switch (entity->Building()) {
				case BUILD_SENTRYGUN:
					entity->AddESPString(format("LV ", entity->var<int>(netvar.iUpgradeLevel), " SENTRY"));
					break;
				case BUILD_DISPENSER:
					entity->AddESPString(format("LV ", entity->var<int>(netvar.iUpgradeLevel), " DISPENSER"));
					break;
				case BUILD_TELEPORTER:
					entity->AddESPString(format("LV ", entity->var<int>(netvar.iUpgradeLevel), " TELEPORTER"));
					break;
				}
			}
			if (show_health) entity->AddESPString(format(entity->Health(), '/', entity->MaxHealth(), " HP"));
		}
	));
	// TF2 Entity Processors
	processors.push_back(entityprocessor_t(
		[this](CachedEntity* entity) -> bool {
			return (entity->clazz == g_pClassID->CTFTankBoss && tank);
		},
		[this](CachedEntity* entity) {
			entity->AddESPString("[: TANK :]");
		}
	));
	processors.push_back(entityprocessor_t(
		[this](CachedEntity* entity) {
			if (!item_money || entity->clazz != g_pClassID->CCurrencyPack) return false;
			if (entity->var<bool>(netvar.bDistributed)) {
				return item_money_red;
			} else return true;
		},
		[this](CachedEntity* entity) {
			entity->AddESPString("$$$");
		}
	));
}

}}}

//void ESP::ProcessEntity(CachedEntity* ent) {
//	if (!enabled) return;

	/*for (auto i : m_processors) {
		if (i.first(ent))
			i.Process(ent);
	}*/

//	if (HL2DM) {
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
		if (!(this->v_bSeeLocal->GetBool() && iinput->CAM_IsThirdPerson()) &&
			ent->m_IDX == engineClient->GetLocalPlayer()) return;
		int pclass = CE_INT(ent, netvar.iClass);
		player_info_t info;
		if (!engineClient->GetPlayerInfo(ent->m_IDX, &info)) return;
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
	//}
//}
