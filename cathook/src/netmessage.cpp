/*
 * netmessage.cpp
 *
 *  Created on: Dec 3, 2016
 *      Author: nullifiedcat
 */

#include "netmessage.h"
#include "logging.h"

bool bf_write::WriteString(const char *pStr)
{
	if(pStr)
	{
		do
		{
			WriteChar( *pStr );
			++pStr;
		} while( *(pStr-1) != 0 );
	}
	else
	{
		WriteChar( 0 );
	}

	return !IsOverflowed();
}

void bf_write::WriteSBitLong( int data, int numbits )
{
	// Do we have a valid # of bits to encode with?

	// Note: it does this wierdness here so it's bit-compatible with regular integer data in the buffer.
	// (Some old code writes direct integers right into the buffer).
	if(data < 0)
	{
		WriteUBitLong( (unsigned int)(0x80000000 + data), numbits - 1, false );
		WriteOneBit( 1 );
	}
	else
	{
		WriteUBitLong((unsigned int)data, numbits - 1);
		WriteOneBit( 0 );
	}
}

void bf_write::WriteChar(int val)
{
	WriteSBitLong(val, sizeof(char) << 3);
}

void bf_write::WriteByte(int val)
{
	WriteUBitLong(val, sizeof(unsigned char) << 3);
}

void bf_write::WriteLong(long val)
{
	WriteSBitLong(val, sizeof(long) << 3);
}

bool NET_NOP::WriteToBuffer( bf_write &buffer )
{
	buffer.WriteUBitLong( GetType(), 6 );
	return !buffer.IsOverflowed();
}

bool NET_NOP::ReadFromBuffer( bf_read &buffer )
{
	return true;
}

const char *NET_NOP::ToString(void) const
{
	return "(null)";
}


bool NET_SignonState::WriteToBuffer( bf_write &buffer )
{
	buffer.WriteUBitLong( GetType(), 6 );
	buffer.WriteByte( m_nSignonState );
	buffer.WriteLong( m_nSpawnCount );

	return !buffer.IsOverflowed();
}

bool NET_SignonState::ReadFromBuffer( bf_read &buffer )
{
	/*m_nSignonState = buffer.ReadByte();
	m_nSpawnCount = buffer.ReadLong();
*/
	return true;
}

const char *NET_SignonState::ToString(void) const
{
	return "(null)";
}


bool NET_SetConVar::WriteToBuffer( bf_write &buffer )
{
	//logging::Info("Writing to buffer 0x%08x!", buf);
	buffer.WriteUBitLong( GetType(), 6 );
	//logging::Info("A");
	int numvars = 1;//m_ConVars.Count();
	//logging::Info("B");
	// Note how many we're sending
	buffer.WriteByte( numvars );
	//logging::Info("C");
	//for (int i=0; i< numvars; i++ )
	//{
		//cvar_t * cvar = &m_ConVars[i];
		buffer.WriteString( convar.name  );
		buffer.WriteString( convar.value );
	//}
	//logging::Info("D");
	return !buffer.IsOverflowed();
}

bool NET_SetConVar::ReadFromBuffer( bf_read &buffer )
{
	int numvars = buffer.ReadByte();

	//m_ConVars.RemoveAll();

	for (int i=0; i< numvars; i++ )
	{
		cvar_t cvar;
		buffer.ReadString( cvar.name, sizeof(cvar.name) );
		buffer.ReadString( cvar.value, sizeof(cvar.value) );
		//m_ConVars.AddToTail( cvar );

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

bool NET_StringCmd::WriteToBuffer( bf_write &buffer )
{
	buffer.WriteUBitLong( GetType(), 6 );
	return buffer.WriteString( m_szCommand ? m_szCommand : " NET_StringCmd NULL" );
}

bool NET_StringCmd::ReadFromBuffer( bf_read &buffer )
{
	m_szCommand = m_szCommandBuffer;
	return buffer.ReadString(m_szCommandBuffer, sizeof(m_szCommandBuffer) );
}

const char *NET_StringCmd::ToString(void) const
{
	return "STRINGCMD";
}

