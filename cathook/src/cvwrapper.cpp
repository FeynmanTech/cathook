/*
 * cvwrapper.cpp
 *
 *  Created on: Dec 18, 2016
 *      Author: nullifiedcat
 */

#include "cvwrapper.h"

#include "common.h"
#include "sdk.h"

std::stack<CatVar*> g_PromisedCatVars;
void InitPromisedCatVars() {
	while (g_PromisedCatVars.size()) {
		auto var = g_PromisedCatVars.top();
		g_PromisedCatVars.pop();
		var->BindConVar(CreateConVar(var->name, var->defaults, var->helpstring));
	}
}

inline explicit CatVar::operator bool() const {
	return !!m_pConVar->GetInt();
}

inline explicit CatVar::operator int() const {
	return m_pConVar->GetInt();
}

inline explicit CatVar::operator float() const {
	return m_pConVar->GetFloat();
}

CatVar::CatVar(CatVar_t type, std::string name, std::string value, std::string help, CatEnum* enum_type, std::string long_description, bool hasminmax, float maxv, float minv) {
	this->name = name;
	this->defaults = value;
	this->helpstring = help;

	m_Type = type;
	m_pConVar = nullptr;
	m_EnumType = enum_type;
	m_flMinValue = minv;
	m_flMaxValue = maxv;
	m_bHasMinmax = hasminmax;
	SetDescription(long_description);

	g_PromisedCatVars.push(this);
}

CatEnum::CatEnum(std::vector<std::string> values, int min) {
	m_values = values;
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

inline bool CatVar::GetBool() const { return m_pConVar->GetBool(); }
inline int CatVar::GetInt() const { return m_pConVar->GetInt(); }
inline float CatVar::GetFloat() const { return m_pConVar->GetFloat(); }
inline const char* CatVar::GetString() const { return m_pConVar->GetString(); }

void CatVar::SetValue(float value) {
	m_pConVar->SetValue(value);
}

void CatVar::SetValue(std::string value) {
	m_pConVar->SetValue(value.c_str());
}

void CatVar::SetValue(int value) {
	m_pConVar->SetValue(value);
}

void CatVar::Increment(int factor) {
	if (!m_pConVar) return;
	switch (m_Type) {
	case CatVar_t::CV_SWITCH: {
		m_pConVar->SetValue(!m_pConVar->GetInt());
	} break;
	case CatVar_t::CV_INT:
		m_pConVar->SetValue(m_pConVar->GetInt() + factor * m_fStep);
		break;
	case CatVar_t::CV_FLOAT:
		m_pConVar->SetValue(m_pConVar->GetFloat() + (float)factor * m_fStep);
		break;
	case CatVar_t::CV_ENUM: {
		int cur = m_pConVar->GetInt();
		int newv = cur + 1;
		if (newv > m_EnumType->Maximum()) {
			newv = m_EnumType->Minimum();
		}
		m_pConVar->SetValue(newv);
	} break;
	}
}

void CatVar::Decrement(int factor) {
	if (!m_pConVar) return;
	switch (m_Type) {
	case CatVar_t::CV_SWITCH:
		m_pConVar->SetValue((int)!m_pConVar->GetInt());
		break;
	case CatVar_t::CV_INT:
		m_pConVar->SetValue(m_pConVar->GetInt() - factor * m_fStep);
		break;
	case CatVar_t::CV_FLOAT:
		m_pConVar->SetValue(m_pConVar->GetFloat() - (float)factor * m_fStep);
		break;
	case CatVar_t::CV_ENUM: {
		int cur = m_pConVar->GetInt();
		int newv = cur - 1;
		if (newv < m_EnumType->Minimum()) {
			newv = m_EnumType->Maximum() - 1;
		}
		m_pConVar->SetValue(newv);
	} break;
	}
}

