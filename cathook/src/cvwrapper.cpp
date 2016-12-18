/*
 * cvwrapper.cpp
 *
 *  Created on: Dec 18, 2016
 *      Author: nullifiedcat
 */

#include "cvwrapper.h"

#include "common.h"
#include "sdk.h"

void CatVar::Increment() {
	if (!m_pConVar) return;
	switch (m_Type) {
	case CatVar_t::CV_SWITCH: {
		m_pConVar->SetValue(!m_pConVar->GetInt());
	} break;
	case CatVar_t::CV_INT:
		m_pConVar->SetValue(m_pConVar->GetInt() + 1);
		break;
	case CatVar_t::CV_FLOAT:
		m_pConVar->SetValue(m_pConVar->GetFloat() + 0.5f);
		break;
	}
}

void CatVar::Decrement() {
	if (!m_pConVar) return;
	switch (m_Type) {
	case CatVar_t::CV_SWITCH:
		m_pConVar->SetValue((int)!m_pConVar->GetInt());
		break;
	case CatVar_t::CV_INT:
		m_pConVar->SetValue(m_pConVar->GetInt() - 1);
		break;
	case CatVar_t::CV_FLOAT:
		m_pConVar->SetValue(m_pConVar->GetFloat() - 0.5f);
		break;
	}
}

