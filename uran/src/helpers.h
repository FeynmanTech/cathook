/*
 * helpers.h
 *
 *  Created on: Oct 8, 2016
 *      Author: nullifiedcat
 */

#ifndef HELPERS_H_
#define HELPERS_H_

class IClientEntity;
class ConVar;
class ConCommand;

#include "enums.h"

#include "fixsdk.h"
#include <tier1/convar.h>

class Vector;

bool IsPlayerCritBoosted(IClientEntity* player);
bool IsPlayerInvulnerable(IClientEntity* player);
//bool SpyIsVisible(IClientEntity* spy);
ConVar* CreateConVar(const char* name, const char* value, const char* help);
ConCommand* CreateConCommand(const char* name, FnCommandCallback_t callback, const char* help);
pack_type GetHealthPackType(IClientEntity* ent);
pack_type GetAmmoPackType(IClientEntity* ent);
powerup_type GetPowerupType(IClientEntity* ent);
powerup_type GetPowerupOnPlayer(IClientEntity* player);
item_type GetItemType(IClientEntity* entity);
const char* GetModelPath(IClientEntity* entity);
int GetHitboxPosition(IClientEntity* entity, int hb, Vector& out);
void FixMovement(float& forwardmove, float& sidemove, float& upmove, Vector& viewangles, Vector& angles);

extern const char* powerups[POWERUP_COUNT];
extern const char* packs[PACK_COUNT];

#endif /* HELPERS_H_ */
