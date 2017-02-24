/*
 * Misc.h
 *
 *  Created on: Nov 5, 2016
 *      Author: nullifiedcat
 */

#ifndef HACKS_MISC_H_
#define HACKS_MISC_H_

namespace hacks { namespace shared { namespace misc {

void SetName(std::string name);
extern CatCommand setname;
void Save(std::string file);
extern CatCommand save;
void SayLines(std::string message);
extern CatCommand say_lines;
void Disconnect(std::string reason);
extern CatCommand disconnect;
extern CatCommand disconnect_vac;

extern CatVar remove_unprintables;
extern CatVar flashlight_spam;
extern CatVar minigun_jump;
extern CatVar anti_afk;
extern CatVar fakelag;
extern CatVar tauntslide;
extern CatVar debug_info;
extern CatVar force_fov;
extern CatVar force_fov_zoomed;
extern CatVar fov_zoomed_override;
extern CatVar no_zoom;
extern CatVar show_angles;
extern CatVar no_visuals;
extern CatVar roll_speedhack;
extern CatVar clean_screenshots;
extern CatVar thirdperson;
extern CatVar disconnect_msg;

}}}

/*class Misc : public IHack {
public:
	Misc();

	virtual void ProcessUserCmd(CUserCmd*) override;
	virtual void Draw() override;

	//ConVar* v_bNoShootUntilCanHeadshot;
	CatVar* v_bDebugInfo;
	ConCommand* c_Name;
	ConVar* v_bInfoSpam;
	ConVar* v_bFastCrouch;
	CatVar* v_bFlashlightSpam;
	CatVar* v_bMinigunJump; // TF2C
	CatVar* v_bDebugCrits; // TF2C
	CatVar* v_bAntiAFK;
	CatVar* v_bHookInspect;
	CatVar* v_iFakeLag;
	CatVar* v_bCritHack;
	CatVar* v_bSuppressCrits;
	CatVar* v_bTauntSlide;
	//ConVar* v_bDumpEventInfo;
	ConCommand* c_SaveSettings;
	ConCommand* c_Unrestricted;
	ConCommand* c_DumpItemAttributes;
	ConCommand* c_SayLine;
	ConCommand* c_Shutdown;
	ConCommand* c_AddFriend;
	ConCommand* c_AddRage;
	ConCommand* c_DumpVars;
	ConCommand* c_DumpPlayers;
	ConCommand* c_Teamname;
	ConCommand* c_Lockee;
	ConCommand* c_Info;
	ConCommand* c_DumpConds;
	ConCommand* c_Reset;
	ConCommand* c_Disconnect;
	ConCommand* c_Schema;
	ConCommand* c_DisconnectVAC;

	CatVar* v_bCleanChat;
};*/

#endif /* HACKS_MISC_H_ */
