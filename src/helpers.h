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

#define HU2M(x) ((int)((x) / 64.0f * 1.22f))
//#define DEG2RAD(x) (float)(x) * (float)(PI / 180.0f)

#include "enums.h"

#include "beforecheaders.h"
#include <string>
#include <sstream>
#include <vector>
#include "aftercheaders.h"

#include <tier1/convar.h>
#include <inputsystem/ButtonCode.h>

//typedef void ( *FnCommandCallback_t )( const CCommand &command );

// TODO split this shit

extern std::vector<ConVar*> g_ConVars;
extern FILE* hConVarsFile;
void BeginConVars();

// Shouldn't be called directly!!!
ConVar* CreateConVar(const std::string& name, const std::string& value, const std::string& help);
ConCommand* CreateConCommand(const std::string& name, FnCommandCallback_t callback, const std::string& help);
void EndConVars();

bool IsPlayerCritBoosted(CachedEntity* player); // TODO use CTFPlayerShared::IsCritBoosted()..
bool IsPlayerInvulnerable(CachedEntity* player);
bool IsPlayerInvisible(CachedEntity* player);

const char* GetBuildingName(CachedEntity* ent);
Vector GetBuildingPosition(CachedEntity* ent);
bool IsBuildingVisible(CachedEntity* ent);

powerup_type GetPowerupOnPlayer(CachedEntity* player);
// GetHitbox() is being called really frequently.
// It's better if it won't create a new object each time it gets called.
// So it returns a success state, and the values are stored in out reference.
k_EWeaponmode GetWeaponMode(CachedEntity* player);

void FixMovement(CUserCmd& cmd, Vector& viewangles);
void VectorAngles(Vector &forward, Vector &angles);

bool IsEntityVisible(CachedEntity* entity, int hb);
bool IsEntityVectorVisible(CachedEntity* entity, Vector endpos);

bool LineIntersectsBox(Vector& bmin, Vector& bmax, Vector& lmin, Vector& lmax);

float DistToSqr(CachedEntity* entity);
void fClampAngle(Vector& qaAng);
void fVectorAngles(Vector &forward, Vector &angles);
//const char* MakeInfoString(IClientEntity* player);
bool GetProjectileData(CachedEntity* weapon, float& speed, float& gravity);
bool IsVectorVisible(Vector a, Vector b);
relation GetRelation(CachedEntity* ent); // TODO new relations
bool IsSentryBuster(CachedEntity* ent);
char* strfmt(const char* fmt, ...);
void ReplaceString(char* target, char* what, char* with_what);
// TODO move that to weaponid.h
bool IsAmbassador(CachedEntity* ent);

template<typename... Args>
std::string format(const Args&...);

void Patch(void* address, void* patch, size_t length);

void AimAt(Vector origin, Vector target, CUserCmd* cmd);
void AimAtHitbox(CachedEntity* ent, int hitbox, CUserCmd* cmd);

std::string WordWrap(std::string& in, int max);

bool IsMeleeWeapon(CachedEntity* ent);
bool IsProjectileCrit(CachedEntity* ent);

QAngle VectorToQAngle(Vector in);
Vector QAngleToVector(QAngle in);

bool CanHeadshot();
bool CanShoot();

char GetUpperChar(ButtonCode_t button);
char GetChar(ButtonCode_t button);

bool IsEntityVisiblePenetration(CachedEntity* entity, int hb);

//void RunEnginePrediction(IClientEntity* ent, CUserCmd *ucmd = NULL);
//void StartPrediction(CUserCmd* cmd);
//void EndPrediction();

float RandFloatRange(float min, float max);

bool Developer(CachedEntity* ent);

Vector CalcAngle(Vector src, Vector dst);
void MakeVector(Vector ang, Vector& out);
float GetFov(Vector ang, Vector src, Vector dst);

const std::string tf_class_names[] = { "" };
const std::string tf_powerup_names[] = { "" };

extern const char* tfclasses[10];
extern const char* powerups[POWERUP_COUNT];

#endif /* HELPERS_H_ */
