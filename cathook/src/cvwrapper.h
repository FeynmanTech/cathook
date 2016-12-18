/*
 * cvwrapper.h
 *
 *  Created on: Dec 17, 2016
 *      Author: nullifiedcat
 */

#ifndef CVWRAPPER_H_
#define CVWRAPPER_H_

class ConVar;

enum CatVar_t {
	CV_SWITCH,
	CV_INT,
	CV_FLOAT,
	CV_STRING
};

class CatVar {
public:
	inline CatVar(ConVar* var, CatVar_t type) { m_pConVar = var; m_Type = type; }
	inline CatVar_t GetType() { return m_Type; }
	inline ConVar* GetConVar() { return m_pConVar; }
	void Increment();
	void Decrement();

	CatVar_t m_Type;
	ConVar* m_pConVar;
};

#endif /* CVWRAPPER_H_ */
