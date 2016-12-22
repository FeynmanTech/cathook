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
struct player_info_s;
class Vector;

#define PI 3.14159265358979323846f
#define RADPI 57.295779513082f
#define DEG2RAD(x) x * (PI / 180.0f)

#include "enums.h"

#include "fixsdk.h"
#include <tier1/convar.h>

#define HIGHEST_ENTITY gEntityCache.m_nMax
#define ENTITY(idx) gEntityCache.GetEntity(idx)->m_pEntity

//typedef void ( *FnCommandCallback_t )( const CCommand &command );

// TODO split this shit

extern FILE* hConVarsFile;
void BeginConVars();
void EndConVars();

bool IsPlayerCritBoosted(IClientEntity* player);
bool IsPlayerInvulnerable(IClientEntity* player);
bool IsPlayerInvisible(IClientEntity* player);
bool IsBuilding(IClientEntity* ent);
bool IsPlayer(IClientEntity* ent);
const char* GetBuildingType(IClientEntity* ent);
Vector GetBuildingPosition(IClientEntity* ent);
bool IsBuildingVisible(IClientEntity* ent);
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
weaponmode GetWeaponMode(IClientEntity* player);
void FixMovement(CUserCmd& cmd, Vector& viewangles);
void VectorAngles(Vector &forward, Vector &angles);
bool IsEntityVisible(IClientEntity* entity, int hb);
float DistToSqr(IClientEntity* entity);
void fClampAngle(Vector& qaAng);
void fVectorAngles(Vector &forward, Vector &angles);
float deg2rad(float deg);
const char* MakeInfoString(IClientEntity* player);
bool GetProjectileData(IClientEntity* weapon, float& speed, float& gravity);
bool IsVectorVisible(Vector a, Vector b);
//bool PredictProjectileAim(Vector origin, IClientEntity* target, hitbox_t hb, float speed, bool arc, float gravity, Vector& result);
relation GetRelation(IClientEntity* ent);
bool IsSentryBuster(IClientEntity* ent);
char* strfmt(const char* fmt, ...);
bool IsAmbassador(IClientEntity* ent);

void Patch(void* address, void* patch, size_t length);

void AimAt(Vector origin, Vector target, CUserCmd* cmd);
void AimAtHitbox(IClientEntity* ent, int hitbox, CUserCmd* cmd);

bool IsMeleeWeapon(IClientEntity* ent);
bool IsProjectile(IClientEntity* ent);
bool IsProjectileCrit(IClientEntity* ent);

QAngle VectorToQAngle(Vector in);
Vector QAngleToVector(QAngle in);

bool CanHeadshot(IClientEntity* player);
bool CheckCE(CachedEntity* entity);

bool BulletTime();

bool IsEntityVisiblePenetration(IClientEntity* entity, int hb);

void RunEnginePrediction(IClientEntity* ent, CUserCmd *ucmd = NULL);

void StartPrediction(CUserCmd* cmd);
void EndPrediction();
float RandFloatRange(float min, float max);

player_info_s* PlayerInfo(IClientEntity* ent);
bool Developer(IClientEntity* ent);
Vector CalcAngle(Vector src, Vector dst);
void MakeVector(Vector ang, Vector& out);
float GetFov(Vector ang, Vector src, Vector dst);

extern const char* tfclasses[10];
extern const char* powerups[POWERUP_COUNT];
extern const char* packs[PACK_COUNT];
extern uint32 friends[256];
extern int n_friends;
extern int n_rage;
extern uint32 rage[256];

#endif /* HELPERS_H_ */
