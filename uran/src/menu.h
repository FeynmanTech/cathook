/*
 * menu.h
 *
 *  Created on: Nov 5, 2016
 *      Author: nullifiedcat
 */

#ifndef MENU_H_
#define MENU_H_

#include <vector>

class ConVar;

class Changer {
public:
	virtual ~Changer();
	virtual void NextValue(ConVar* var);
	virtual void PrevValue(ConVar* var);
};

class BoolChanger : public Changer {
public:
	virtual void NextValue(ConVar* var);
	virtual void PrevValue(ConVar* var);
};

class IntChanger : public Changer {
public:
	virtual void NextValue(ConVar* var);
	virtual void PrevValue(ConVar* var);
	int m_iMinValue;
	int m_iMaxValue;
	int m_iStep;
};

class FloatChanger : public Changer {
public:
	virtual void NextValue(ConVar* var);
	virtual void PrevValue(ConVar* var);
	float m_iMinValue;
	float m_iMaxValue;
	float m_iStep;
};

class MenuEntry {
public:
	MenuEntry(ConVar* variable, Changer changer);
	Changer m_Changer;
	ConVar* m_ConVar;
};

class MenuArray {
public:
	void AddEntry(MenuEntry entry);
	const char* GetName();
	std::vector<MenuEntry> m_Entries;
	const char* m_Name;
};

class Menu {
public:

};

Menu g_Menu;


#endif /* MENU_H_ */
