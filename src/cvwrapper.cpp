/*
 * cvwrapper.cpp
 *
 *  Created on: Dec 18, 2016
 *      Author: nullifiedcat
 */

#include "cvwrapper.h"

#include "common.h"
#include "sdk.h"

CatVar::CatVar(CatVar_t type, std::string name, std::string value, std::string help, CatEnum* enum_type, std::string long_description, bool hasminmax, float maxv, float minv) {
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

