/*
 * netmessage.h
 *
 *  Created on: Dec 3, 2016
 *      Author: nullifiedcat
 */

#ifndef NETMESSAGE_H_
#define NETMESSAGE_H_

#include "fixsdk.h"
#include <bitbuf.h>
#include <utlvector.h>
#include <inetchannel.h>
#include <inetmessage.h>
#include <inetmsghandler.h>

#define DECLARE_BASE_MESSAGE( msgtype )						\
	public:													\
		bool			ReadFromBuffer( bf_read &buffer );	\
		bool			WriteToBuffer( bf_write &buffer );	\
		const char		*ToString() const;					\
		int				GetType() const { return msgtype; } \
		const char		*GetName() const { return #msgtype;}\

#define DECLARE_NET_MESSAGE( name )			\
	DECLARE_BASE_MESSAGE( net_##name );		\
	INetMessageHandler *m_pMessageHandler;	\
	bool Process() { return false; }\

#define DECLARE_SVC_MESSAGE( name )		\
	DECLARE_BASE_MESSAGE( svc_##name );	\
	IServerMessageHandler *m_pMessageHandler;\
	bool Process() { return m_pMessageHandler->Process##name( this ); }\

#define DECLARE_CLC_MESSAGE( name )		\
	DECLARE_BASE_MESSAGE( clc_##name );	\
	IClientMessageHandler *m_pMessageHandler;\
	bool Process() { return m_pMessageHandler->Process##name( this ); }\

#define DECLARE_MM_MESSAGE( name )		\
	DECLARE_BASE_MESSAGE( mm_##name );	\
	IMatchmakingMessageHandler *m_pMessageHandler;\
	bool Process() { return m_pMessageHandler->Process##name( this ); }\

class CNetMessage : public INetMessage
{
public:
	CNetMessage() {	m_bReliable = true;
					m_NetChannel = 0; }

	virtual ~CNetMessage() {};

	virtual int		GetGroup() const { return INetChannelInfo::GENERIC; }
	INetChannel		*GetNetChannel() const { return m_NetChannel; }

	virtual void	SetReliable( bool state) {m_bReliable = state;};
	virtual bool	IsReliable() const { return m_bReliable; };
	virtual void    SetNetChannel(INetChannel * netchan) { m_NetChannel = netchan; }
	virtual bool	Process() { return false; };	// no handler set

protected:
	bool				m_bReliable;	// true if message should be send reliable
	INetChannel			*m_NetChannel;	// netchannel this message is from/for
};

#define net_SetConVar 5
#define MAX_OSPATH 256

class NET_SetConVar : public CNetMessage
{
	DECLARE_NET_MESSAGE( SetConVar );

	int	GetGroup() const { return INetChannelInfo::STRINGCMD; }

	NET_SetConVar() {}
	NET_SetConVar( const char * name, const char * value)
	{
		cvar_t cvar;
		strncpy( cvar.name, name, MAX_OSPATH );
		strncpy( cvar.value, value, MAX_OSPATH );
		m_ConVars.AddToTail( cvar );
	}

public:

	typedef struct cvar_s
	{
		char	name[MAX_OSPATH];
		char	value[MAX_OSPATH];
	} cvar_t;

	CUtlVector<cvar_t> m_ConVars;
};

#endif /* NETMESSAGE_H_ */
