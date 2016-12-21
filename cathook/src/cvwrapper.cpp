/*
 * cvwrapper.cpp
 *
 *  Created on: Dec 18, 2016
 *      Author: nullifiedcat
 */

#include "cvwrapper.h"

#include "common.h"
#include "sdk.h"

CatEnum::CatEnum(const char** values, int size, int min) {
	m_Values = values;
	m_iMin = min;
	m_iMax = min + size - 1;
	m_iLength = size;
	logging::Info("Created enum with size %i", size);
}

const char* CatEnum::Name(int value) {
	if (value + m_iMin >= 0 && value + m_iMin < m_iMax) {
		return m_Values[value + m_iMin];
	}
	return (const char*)0;
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

void CatVar::Increment(int factor) {
	if (!m_pConVar) return;
	switch (m_Type) {
	case CatVar_t::CV_SWITCH: {
		m_pConVar->SetValue(!m_pConVar->GetInt());
	} break;
	case CatVar_t::CV_INT:
		m_pConVar->SetValue(m_pConVar->GetInt() + factor * m_iStep);
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
		m_pConVar->SetValue(m_pConVar->GetInt() - factor * m_iStep);
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

