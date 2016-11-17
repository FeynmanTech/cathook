/*
 * helpers.h
 *
 *  Created on: Oct 8, 2016
 *      Author: nullifiedcat
 */

#ifndef HELPERS_H_
#define HELPERS_H_

class CachedEntity;
class IClientEntity;
class ConVar;
class ConCommand;
class CUserCmd;
class CCommand;
class Vector;

#define PI 3.14159265358979323846f
#define RADPI 57.295779513082f
#define DEG2RAD(x) x * (PI / 180.0f)

#include "enums.h"

#include "fixsdk.h"
#include <tier1/convar.h>

//typedef void ( *FnCommandCallback_t )( const CCommand &command );

bool IsPlayerCritBoosted(IClientEntity* player);
bool IsPlayerInvulnerable(IClientEntity* player);
bool IsPlayerInvisible(IClientEntity* player);
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
bool IsEntityVisible(IClientEntity* entity, int hb);
float DistToSqr(IClientEntity* entity);
void fClampAngle(Vector& qaAng);
void fVectorAngles(Vector &forward, Vector &angles);
float deg2rad(float deg);
bool GetProjectileData(IClientEntity* weapon, float& speed, bool& arc);
bool IsVectorVisible(Vector a, Vector b);
bool PredictProjectileAim(Vector origin, IClientEntity* target, hitbox hb, float speed, bool arc, Vector& result);
relation GetRelation(IClientEntity* ent);
// TODO deprecated
//int ClassMaxHealth(int clazz);
bool CheckCE(CachedEntity* entity);

bool BulletTime(IClientEntity* ent, bool interval);
bool CanShoot(IClientEntity* entity);

bool IsEntityVisiblePenetration(IClientEntity* entity, int hb);

// Stolen Code

// F1 c&p
Vector CalcAngle(Vector src, Vector dst);
void MakeVector(Vector ang, Vector& out);
float GetFov(Vector ang, Vector src, Vector dst);

extern const char* powerups[POWERUP_COUNT];
extern const char* packs[PACK_COUNT];
extern uint32 friends[256];
extern int n_friends;
extern int n_rage;
extern uint32 rage[256];

#endif /* HELPERS_H_ */
