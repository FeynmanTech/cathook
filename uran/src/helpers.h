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
class CUserCmd;
class Vector;

#define PI 3.14159265358979323846f
#define DEG2RAD(x) x * (PI / 180.0f)

#include "enums.h"

#include "fixsdk.h"
#include <tier1/convar.h>

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
void FixMovement(CUserCmd& cmd, Vector& viewangles);
void VectorAngles(Vector &forward, Vector &angles);

extern const char* powerups[POWERUP_COUNT];
extern const char* packs[PACK_COUNT];

#endif /* HELPERS_H_ */
