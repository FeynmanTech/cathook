/*
 * cvwrapper.cpp
 *
 *  Created on: Dec 18, 2016
 *      Author: nullifiedcat
 */

#include "cvwrapper.h"

#include "common.h"
#include "sdk.h"

CatVar::CatVar(CatVar_t type, std::string name, std::string value, std::string help, ICatEnum* enum_type, std::string long_description, bool hasminmax, float maxv, float minv) {
	m_Type = type;
	m_pConVar = CreateConVar(CON_PREFIX + name, value, help);
	m_EnumType = enum_type;
	m_flMinValue = minv;
	m_flMaxValue = maxv;
	m_bHasMinmax = hasminmax;
	SetDescription(long_description);
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

int CatEnum::Maximum() {
	return m_iMax;
}

int CatEnum::Minimum() {
	return m_iMin;
}

bool CatVar::GetBool() { return m_pConVar->GetBool(); }
int CatVar::GetInt() { return m_pConVar->GetInt(); }
float CatVar::GetFloat() { return m_pConVar->GetFloat(); }
const char* CatVar::GetString() { return m_pConVar->GetString(); }

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

