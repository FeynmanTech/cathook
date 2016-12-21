/*
 * cvwrapper.h
 *
 *  Created on: Dec 17, 2016
 *      Author: nullifiedcat
 */

#ifndef CVWRAPPER_H_
#define CVWRAPPER_H_

class ConVar;

#define CREATE_CV(type, name, defaults, description) \
	new CatVar(CreateConVar(CON_PREFIX name, defaults, description), type);

enum CatVar_t {
	CV_SWITCH,
	CV_INT,
	CV_FLOAT,
	CV_STRING,
	CV_ENUM
};

class ICatEnum {
public:
	inline virtual ~ICatEnum() {}
	virtual const char* Name(int value) = 0;
	virtual int Minimum() = 0;
	virtual int Maximum() = 0;
};

class CatEnum : public ICatEnum {
public:
	CatEnum(const char** values, int size, int min = 0);
	virtual const char* Name(int value);
	virtual int Maximum();
	virtual int Minimum();
	const char** m_Values;
	int m_iMin;
	int m_iMax;
	int m_iLength;
};

class CatVar {
public:
	inline CatVar(ConVar* var, CatVar_t type) { m_iStep = 1; m_fStep = 0.5f; m_pConVar = var; m_Type = type; }
	inline CatVar(ConVar* var, ICatEnum* catenum) { m_pConVar = var; m_Type = CatVar_t::CV_ENUM; m_EnumType = catenum; }
	inline CatVar_t GetType() { return m_Type; }
	inline ICatEnum* GetEnum() { return 0; }
	inline ConVar* GetConVar() { return m_pConVar; }

	bool GetBool();
	int GetInt();
	float GetFloat();

	void Increment(int factor = 1);
	void Decrement(int factor = 1);

	inline void SetStep(int step) { m_iStep = step; }
	inline void SetStep(float step) { m_fStep = step; }

	int m_iStep;
	float m_fStep;

	ICatEnum* m_EnumType;
	CatVar_t m_Type;
	ConVar* m_pConVar;
};

#endif /* CVWRAPPER_H_ */
