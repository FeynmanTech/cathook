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

bf_read::bf_read()
{
	m_pData = NULL;
	m_nDataBytes = 0;
	m_nDataBits = -1; // set to -1 so we overflow on any operation
	m_iCurBit = 0;
	m_bOverflow = false;
	m_bAssertOnOverflow = true;
	m_pDebugName = NULL;
}

bf_read::bf_read( const void *pData, int nBytes, int nBits )
{
	m_bAssertOnOverflow = true;
	StartReading( pData, nBytes, 0, nBits );
}

bf_read::bf_read( const char *pDebugName, const void *pData, int nBytes, int nBits )
{
	m_bAssertOnOverflow = true;
	m_pDebugName = pDebugName;
	StartReading( pData, nBytes, 0, nBits );
}

void bf_read::StartReading( const void *pData, int nBytes, int iStartBit, int nBits )
{
	// Make sure we're dword aligned.
	Assert(((unsigned long)pData & 3) == 0);

	m_pData = (unsigned char*)pData;
	m_nDataBytes = nBytes;

	if ( nBits == -1 )
	{
		m_nDataBits = m_nDataBytes << 3;
	}
	else
	{
		Assert( nBits <= nBytes*8 );
		m_nDataBits = nBits;
	}

	m_iCurBit = iStartBit;
	m_bOverflow = false;
}


bool bf_read::ReadString( char *pStr, int maxLen, bool bLine, int *pOutNumChars )
{
	Assert( maxLen != 0 );

	bool bTooSmall = false;
	int iChar = 0;
	while(1)
	{
		char val = ReadChar();
		if ( val == 0 )
			break;
		else if ( bLine && val == '\n' )
			break;

		if ( iChar < (maxLen-1) )
		{
			pStr[iChar] = val;
			++iChar;
		}
		else
		{
			bTooSmall = true;
		}
	}

	// Make sure it's null-terminated.
	Assert( iChar < maxLen );
	pStr[iChar] = 0;

	if ( pOutNumChars )
		*pOutNumChars = iChar;

	return !IsOverflowed() && !bTooSmall;
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

