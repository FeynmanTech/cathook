/*
 * interfaces.h
 *
 *  Created on: Oct 3, 2016
 *      Author: nullifiedcat
 */

#ifndef INTERFACES_H_
#define INTERFACES_H_

namespace vgui {
class ISurface;
class IPanel;
}

class ISteamClient;
class ISteamFriends;
class IVEngineClient013;
class IClientEntityList;
class ICenterPrint;
class ICvar;
class IGameEventManager2;
class IBaseClientDLL;
class ClientModeShared;
class IEngineTrace;
class IVModelInfoClient;
class IInputSystem;
class IClient;
class CGlobalVarsBase;
class IPrediction;
class IGameMovement;
class IInput;
class IMatSystemSurface;
class ISteamUser;
class IAchievementMgr;
class ISteamUserStats;

namespace interfaces {

extern ISteamClient* steamClient;
extern ISteamFriends* steamFriends;
extern IVEngineClient013* engineClient;
extern vgui::ISurface* surface;
extern vgui::IPanel* panel;
extern IClientEntityList* entityList;
extern ICenterPrint* centerPrint;
extern ICvar* cvar;
extern IGameEventManager2* eventManager;
extern IBaseClientDLL* baseClient;
extern IEngineTrace* trace;
extern IVModelInfoClient* model;
extern IInputSystem* input;
extern CGlobalVarsBase* gvars;
extern IPrediction* prediction;
extern IGameMovement* gamemovement;
extern IInput* iinput;
extern IMatSystemSurface* matsurface;
extern ISteamUser* user;
extern IAchievementMgr* achievements;
extern ISteamUserStats* stats;

void CreateInterfaces();

}

#endif /* INTERFACES_H_ */
