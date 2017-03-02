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
#include <functional>
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
	int Maximum() const ;
	int Minimum() const ;
	const std::vector<std::string> m_values;
	int m_iMin;
	int m_iMax;
	int m_iLength;
};

class CatVar {
public:
	[[deprecated]]
	CatVar(CatVar_t type, std::string name, std::string value, std::string help, CatEnum* enum_type = 0, std::string long_description = "no description", bool hasminmax = false, float max = 1.0f, float min = 0.0f);

	CatVar(CatVar_t type, std::string name, std::string defaults, std::string desc_short, std::string desc_long);
	CatVar(CatVar_t type, std::string name, std::string defaults, std::string desc_short, std::string desc_long, float max_val);
	CatVar(CatVar_t type, std::string name, std::string defaults, std::string desc_short, std::string desc_long, float min_val, float max_val);
	CatVar(CatVar_t type, std::string name, std::string defaults, std::string desc_short, std::string desc_long, CatEnum& cat_enum);

	inline explicit operator bool() const { return !!convar_parent->m_nValue; }
	inline explicit operator int() const { return convar_parent->m_nValue; }
	inline explicit operator float() const { return convar_parent->m_fValue; }
	inline void operator =(const int& value) { convar_parent->InternalSetIntValue(value); }
	inline void operator =(const float& value) { convar_parent->InternalSetFloatValue(value); }
	inline bool operator ==(const int& value) const { return convar_parent->m_nValue == value; }
	inline bool operator ==(const float& value) const { return convar_parent->m_fValue == value; }

	void Register();
	typedef std::function<void(CatVar*)> RegisterCallbackFn;
	std::stack<RegisterCallbackFn> callbacks;
	inline void OnRegister(RegisterCallbackFn fn) {
		if (registered) fn(this);
		else callbacks.push(fn);
	}

	[[deprecated]]
	inline bool GetBool() const { return this->operator bool();  }
	[[deprecated]]
	inline int GetInt() const { return this->operator int(); }
	[[deprecated]]
	inline float GetFloat() const { return this->operator float(); };
	inline const char* GetString() const { return convar_parent->GetString(); }
	[[deprecated]]
	inline void SetValue(float value) { this->operator =(value); }
	inline void SetValue(std::string value) { convar_parent->SetValue(value.c_str()); }
	[[deprecated]]
	inline void SetValue(int value) { this->operator =(value); }

	bool restricted;
	float max;
	float min;
	bool registered;

	const CatVar_t type;
	const std::string name;
	const std::string defaults;
	const std::string desc_short;
	const std::string desc_long;
	CatEnum* enum_type;
	ConVar* convar;
	ConVar* convar_parent;
};

extern std::stack<CatVar*> g_UnregisteredCatVars;
void RegisterCatVars();

#endif /* CVWRAPPER_H_ */
