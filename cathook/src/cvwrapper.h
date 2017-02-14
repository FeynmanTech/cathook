/*
 * cvwrapper.h
 *
 *  Created on: Dec 17, 2016
 *      Author: nullifiedcat
 */

#ifndef CVWRAPPER_H_
#define CVWRAPPER_H_

class CatVar;
class CatCommand;

#include "beforecheaders.h"
#include <string>
#include <vector>
#include <stack>
#include "aftercheaders.h"

#include <tier1/convar.h>

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

class CatEnum {
public:
	CatEnum(std::vector<std::string> values, int min = 0);
	std::string Name(int value);
	int Maximum() const;
	int Minimum() const;
	std::vector<std::string> m_values;
	int m_iMin;
	int m_iMax;
	int m_iLength;
};

extern std::stack<CatCommand*> g_PromisedCatCommands;
extern std::stack<CatVar*> g_PromisedCatVars;
void InitPromisedCatVars();

class CatCommand {
public:
	CatCommand(std::string name, std::string help, FnCommandCallback_t callback);
	CatCommand(std::string name, std::string help, FnCommandCallbackVoid_t callback);

	inline void BindCommand(ConCommand* cmd) {
		this->cmd = cmd;
	}

	std::string name;
	std::string help;
	FnCommandCallback_t callback;
	FnCommandCallbackVoid_t callback_void;
	ConCommand* cmd;
};

class CatVar {
public:
	CatVar(CatVar_t type, std::string name, std::string value, std::string help, CatEnum* enum_type = 0, std::string long_description = "no description", bool hasminmax = false, float max = 1.0f, float min = 0.0f);
	void BindConVar(ConVar* cvar);
	inline CatVar_t GetType() const { return m_Type; }
	inline CatEnum* GetEnum() const { return m_EnumType; }
	inline ConVar* GetConVar() const { return m_pConVar; }
	inline void SetDescription(std::string description) { m_strDescription = description; }
	inline const std::string& Description() const { return m_strDescription; };

	explicit operator bool() const;
	explicit operator int() const;
	explicit operator float() const;

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

	std::string name;
	std::string defaults;
	std::string helpstring;

	std::string m_strDescription;
	CatEnum* m_EnumType;
	CatVar_t m_Type;
	ConVar* m_pConVar;
};

#endif /* CVWRAPPER_H_ */
