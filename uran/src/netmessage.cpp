/*
 * netmessage.cpp
 *
 *  Created on: Dec 3, 2016
 *      Author: nullifiedcat
 */

#include "netmessage.h"

bool NET_SetConVar::WriteToBuffer( bf_write &buffer )
{
	buffer.WriteUBitLong( GetType(), 5 );

	int numvars = m_ConVars.Count();

	// Note how many we're sending
	buffer.WriteByte( numvars );

	for (int i=0; i< numvars; i++ )
	{
		cvar_t * cvar = &m_ConVars[i];
		buffer.WriteString( cvar->name  );
		buffer.WriteString( cvar->value );
	}

	return !buffer.IsOverflowed();
}

bool NET_SetConVar::ReadFromBuffer( bf_read &buffer )
{
	int numvars = buffer.ReadByte();

	m_ConVars.RemoveAll();

	for (int i=0; i< numvars; i++ )
	{
		cvar_t cvar;
		buffer.ReadString( cvar.name, sizeof(cvar.name) );
		buffer.ReadString( cvar.value, sizeof(cvar.value) );
		m_ConVars.AddToTail( cvar );

	}
	return !buffer.IsOverflowed();
}

const char *NET_SetConVar::ToString(void) const
{
	/*snprintf(s_text, sizeof(s_text), "%s: %i cvars, \"%s\"=\"%s\"",
		GetName(), m_ConVars.Count(),
		m_ConVars[0].name, m_ConVars[0].value );
	return s_text;*/
	return "(NULL)";
}

