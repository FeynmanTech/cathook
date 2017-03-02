/*
 * cvwrapper.cpp
 *
 *  Created on: Dec 18, 2016
 *      Author: nullifiedcat
 */

#include "cvwrapper.h"

#include "common.h"
#include "sdk.h"

CatVar::CatVar(CatVar_t type, std::string name, std::string value, std::string help, CatEnum* enum_type, std::string long_description, bool hasminmax, float maxv, float minv)
	: type(type), name(name), defaults(value), desc_short(help), desc_long(long_description), enum_type(enum_type) {
	min = minv;
	max = maxv;
	restricted = hasminmax;
	g_UnregisteredCatVars.push(this);
}

CatVar::CatVar(CatVar_t type, std::string name, std::string defaults, std::string desc_short, std::string desc_long)
	: type(type), name(name), defaults(defaults), desc_short(desc_short), desc_long(desc_long), enum_type(nullptr), restricted(false) {
	// For some reason, adding min(0.0f), max(0.0f) gives a compilation error.
	min = 0.0f;
	max = 0.0f;
	g_UnregisteredCatVars.push(this);
}

CatVar::CatVar(CatVar_t type, std::string name, std::string defaults, std::string desc_short, std::string desc_long, float max_val)
	: type(type), name(name), defaults(defaults), desc_short(desc_short), desc_long(desc_long), enum_type(nullptr), restricted(true) {
	min = 0.0f;
	max = max_val;
	g_UnregisteredCatVars.push(this);
}

CatVar::CatVar(CatVar_t type, std::string name, std::string defaults, std::string desc_short, std::string desc_long, float min_val, float max_val)
	: type(type), name(name), defaults(defaults), desc_short(desc_short), desc_long(desc_long), enum_type(nullptr), restricted(true) {
	min = min_val;
	max = max_val;
	g_UnregisteredCatVars.push(this);
}

CatVar::CatVar(CatVar_t type, std::string name, std::string defaults, std::string desc_short, std::string desc_long, CatEnum& cat_enum)
	: type(type), name(name), defaults(defaults), desc_short(desc_short), desc_long(desc_long), enum_type(&cat_enum), restricted(true) {
	min = cat_enum.Minimum();
	max = cat_enum.Maximum();
	g_UnregisteredCatVars.push(this);
}

void CatVar::Register() {
	convar = CreateConVar(CON_PREFIX + name, defaults, desc_short);
	convar_parent = convar->m_pParent;
	while (!callbacks.empty()) {
		callbacks.top()(this);
		callbacks.pop();
	}
	registered = true;
}

std::stack<CatVar*> g_UnregisteredCatVars;
void RegisterCatVars() {
	while (g_UnregisteredCatVars.size()) {
		CatVar* var = g_UnregisteredCatVars.top();
		var->Register();
		g_UnregisteredCatVars.pop();
	}
}

CatEnum::CatEnum(std::vector<std::string> values, int min) : m_values(values) {
	m_iMin = min;
	m_iMax = min + values.size() - 1;
	m_iLength = values.size();
}

std::string CatEnum::Name(int value) {
	if (value - m_iMin >= 0 && value - m_iMin <= m_iMax) {
		return m_values.at(value - Minimum());
	}
	return "unknown";
}

int CatEnum::Maximum() const {
	return m_iMax;
}

int CatEnum::Minimum() const {
	return m_iMin;
}

