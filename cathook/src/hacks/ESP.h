/*
 * HEsp.h
 *
 *  Created on: Oct 6, 2016
 *      Author: nullifiedcat
 */

#ifndef HESP_H_
#define HESP_H_

#include "../beforecheaders.h"
#include <functional>
#include <vector>
#include "../aftercheaders.h"

namespace hacks { namespace shared { namespace esp {

typedef std::pair<std::function<bool(CachedEntity&)>, std::function<bool(CachedEntity&)>> entityprocessor_t;

void ProcessEntity(CUserCmd*, CachedEntity&);
void DrawProcessEntity(CachedEntity&);
void CreateProcessors();
void DrawBox(CachedEntity* ent, int clr, float widthFactor, float addHeight, bool healthbar, int health, int healthmax);

extern CatVar local;
extern CatVar buildings;
extern CatVar enabled;
extern CatVar entity_info;
extern CatVar teammates;
extern CatVar item;
extern CatVar item_weapons;
extern CatVar item_ammo;
extern CatVar item_health;
extern CatVar item_powerup;
extern CatVar item_money;
extern CatVar item_money_red;
extern CatVar teammate_powerups;
extern CatVar entity_id;
extern CatVar tank;
extern CatVar box;
extern CatVar show_distance;
extern CatVar show_friendid;
extern CatVar show_name;
extern CatVar show_class;
extern CatVar show_conds;
extern CatVar vischeck;
extern CatVar legit;
extern CatVar show_health;
extern CatVar legit_ticks;
extern CatVar proj_rockets;
extern CatVar proj_arrows;
extern CatVar proj_pipes;
extern CatVar proj_stickies;
extern CatVar proj_enemy;
extern CatVar projectile;
extern CatVar model_name;
extern CatVar item_dmweapons;
extern CatVar item_adrenaline;

std::vector<entityprocessor_t> processors;

}}}

#endif /* HESP_H_ */
