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
	CV_INT,
	CV_FLOAT,
	CV_STRING
};

class ICatVar {
public:
	virtual ~ICatVar();
	virtual CatVar_t GetType() = 0;
	virtual ConVar* GetConVar() = 0;
};

class CatVar_INT : public ICatVar {
public:
	CatVar_t GetType() { return CatVar_t::CV_INT; }
	ConVar* GetConVar() { return m_pConVar; }

	ConVar* m_pConVar;
};

#endif /* CVWRAPPER_H_ */
