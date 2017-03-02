/*
 * cvwrapper.h
 *
 *  Created on: Dec 17, 2016
 *      Author: nullifiedcat
 */

#ifndef CVWRAPPER_H_
#define CVWRAPPER_H_

class ConVar;

#include "sdk.h"

#include "beforecheaders.h"
#include <string>
#include <vector>
#include <stack>
#include "aftercheaders.h"

//#define CREATE_CV(type, name, defaults, description) \
//	new CatVar(CreateConVar(CON_PREFIX name, defaults, description), type);

//#define CREATE_CV_DESC(type, name, defaults, description, detailed) \
//	new CatVar(CreateConVar(CON_PREFIX name, defaults, description), type, detailed);

enum CatVar_t {
	CV_SWITCH,
	CV_INT,
	CV_FLOAT,
	CV_STRING,
	CV_ENUM,
	CV_KEY
};

class CatEnum {
public:
	CatEnum(std::vector<std::string> values, int min = 0);
	std::string Name(int value);
	int Maximum();
	int Minimum();
	std::vector<std::string> m_values;
	int m_iMin;
	int m_iMax;
	int m_iLength;
};

class CatVar {
public:
	CatVar(CatVar_t type, std::string name, std::string value, std::string help, CatEnum* enum_type = 0, std::string long_description = "no description", bool hasminmax = false, float max = 1.0f, float min = 0.0f);
	inline CatVar_t GetType() { return m_Type; }
	inline CatEnum* GetEnum() { return m_EnumType; }
	inline ConVar* GetConVar() { return m_pConVar; }
	inline void SetDescription(std::string description) { m_strDescription = description; }
	inline std::string Description() { return m_strDescription; }

	inline explicit operator bool() const { return !!convar_parent->m_nValue; }
	inline explicit operator int() const { return convar_parent->m_nValue; }
	inline explicit operator float() const { return convar_parent->m_fValue; }
	inline void operator =(const int& value) { convar_parent->InternalSetIntValue(value); }
	inline void operator =(const float& value) { convar_parent->InternalSetFloatValue(value); }
	inline bool operator ==(const int& value) { return convar_parent->m_nValue == value; }
	inline bool operator ==(const float& value) { return convar_parent->m_fValue == value; }

	[[deprecated]]
	inline bool GetBool() const { return m_pConVar->GetBool(); }
	[[deprecated]]
	inline int GetInt() const { return m_pConVar->GetInt(); }
	[[deprecated]]
	inline float GetFloat() const { return m_pConVar->GetFloat(); };
	inline const char* GetString() const { return m_pConVar->GetString(); }
	[[deprecated]]
	inline void SetValue(float value) { m_pConVar->SetValue(value); }
	inline void SetValue(std::string value) { m_pConVar->SetValue(value.c_str()); }
	[[deprecated]]
	inline void SetValue(int value) { m_pConVar->SetValue(value); }

	bool m_bHasMinmax;
	float m_flMaxValue;
	float m_flMinValue;

	std::string m_strDescription;
	CatEnum* m_EnumType;
	CatVar_t m_Type;
	ConVar* m_pConVar;
	ConVar* convar_parent;
};

extern std::stack<CatVar*> g_UnregisteredCatVars;
void RegisterCatVars();

#endif /* CVWRAPPER_H_ */
