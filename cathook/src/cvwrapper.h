/*
 * cvwrapper.h
 *
 *  Created on: Dec 17, 2016
 *      Author: nullifiedcat
 */

#ifndef CVWRAPPER_H_
#define CVWRAPPER_H_

class ConVar;

#include "beforecheaders.h"
#include <string>
#include <vector>
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

class ICatEnum {
public:
	inline virtual ~ICatEnum() {}
	virtual std::string Name(int value) = 0;
	virtual int Minimum() const = 0;
	virtual int Maximum() const = 0;
};

class CatEnum : public ICatEnum {
public:
	CatEnum(std::vector<std::string> values, int min = 0);
	virtual std::string Name(int value);
	virtual int Maximum() const;
	virtual int Minimum() const;
	std::vector<std::string> m_values;
	int m_iMin;
	int m_iMax;
	int m_iLength;
};

class CatVar {
public:
	CatVar(CatVar_t type, std::string name, std::string value, std::string help, ICatEnum* enum_type = 0, std::string long_description = "no description", bool hasminmax = false, float max = 1.0f, float min = 0.0f);
	//inline CatVar(ConVar* var, CatVar_t type, std::string desc = "") { m_fStep = 1; m_fStep = 0.5f; m_pConVar = var; m_Type = type; SetDescription(desc); }
	//inline CatVar(ConVar* var, ICatEnum* catenum, std::string desc = "") { m_pConVar = var; m_Type = CatVar_t::CV_ENUM; m_EnumType = catenum; SetDescription(desc); }
	inline CatVar_t GetType() const { return m_Type; }
	inline ICatEnum* GetEnum() const { return m_EnumType; }
	inline ConVar* GetConVar() const { return m_pConVar; }
	inline void SetDescription(std::string description) { m_strDescription = description; }
	inline std::string& Description() { return m_strDescription; }

	bool GetBool() const;
	int GetInt() const;
	float GetFloat() const;
	const char* GetString() const;
	void SetValue(float value);
	void SetValue(std::string value);
	void SetValue(int value);

	void Increment(int factor = 1);
	void Decrement(int factor = 1);

	inline void SetStep(float step) { m_fStep = step; }

	bool m_bHasMinmax;
	float m_flMaxValue;
	float m_flMinValue;
	float m_fStep;

	std::string m_strDescription;
	ICatEnum* m_EnumType;
	CatVar_t m_Type;
	ConVar* m_pConVar;
};

#endif /* CVWRAPPER_H_ */
