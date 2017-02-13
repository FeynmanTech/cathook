/*
 * CDumper.h
 *
 *  Created on: Oct 5, 2016
 *      Author: nullifiedcat
 */

#ifndef CDUMPER_H_
#define CDUMPER_H_

#include <cstring>
#include <fstream>

#include "logging.h"
#include "fixsdk.h"
#include <dt_common.h>
#include <client_class.h>

class CDumper
{
	std::fstream m_file;

	char* TypeToString(SendPropType type)
	{
		//logging::Info("inside...");
		char* ret = (char *)"UNKNOWN";
		//logging::Info("oh my");
		//logging::Info("ret.. %s", ret);
		if (type == 0) {
			ret = (char *)"INT";
		} else if (type == 1) {
			ret = (char *)"FLOAT";
		} else if (type == 2) {
			ret = (char *)"VECTOR3";
		} else if (type == 3) {
			ret = (char *)"VECTOR2";
		} else if (type == 4) {
			ret = (char *)"STRING";
		} else if (type == 5) {
			ret = (char *)"ARRAY";
		} else if (type == 6) {
			ret = (char *)"TABLE";
		}
		//logging::Info("returning %s", ret);
		return ret;
	}

public:
	CDumper()
	{
		//logging::Info("opening file...");
		m_file.open("/tmp/netdump.txt", std::ios::out | std::ios::trunc);
		//logging::Info("file: %i", m_file.good());
	}

	~CDumper()
	{
		m_file.close();
	}

	void SaveDump()
	{
		ClientClass *pList = g_IBaseClient->GetAllClasses();
		//logging::Info("iterating...");
		while(pList)
		{
			DumpTable(pList->m_pRecvTable, 0);

			pList = pList->m_pNext;
		}
		m_file.close();
	}

	void DumpTable(RecvTable *pTable, int iLevel)
	{
		if(!pTable)
			return;

		//logging::Info("dumping table.. %s", pTable->GetName());

		for(int j = 0; j < iLevel; j++)
			m_file << " ";

		m_file << pTable->GetName() << std::endl;

		iLevel += 2;

		for(int i = 0; i < pTable->GetNumProps(); ++i)
		{
			//logging::Info("dumping prop.. %i out of %i..", i, pTable->GetNumProps());
			RecvProp *pProp = pTable->GetProp(i);
			//logging::Info("it has name %s", pProp->GetName());
			if(!pProp)
				continue;

			if(isdigit(pProp->GetName()[0]))
				continue;

			if(pProp->GetDataTable())
			{
				DumpTable(pProp->GetDataTable(), iLevel + 1);
			}

			for(int j = 0; j < iLevel; j++)
				m_file << " ";

			int offset = pProp->GetOffset();
			//logging::Info("offset %i", offset);
			SendPropType type = pProp->GetType();
			//logging::Info("type.. %i", type);
			char* typestr = TypeToString(type);
			//logging::Info("back from tts");
			//logging::Info("type str %s", typestr);

			m_file << pProp->GetName() << " : 0x" << std::hex << offset << " [" << typestr << "]" << std::endl;
		}

		if(iLevel == 2)
			m_file << std::endl;
	}
};


#endif /* CDUMPER_H_ */
