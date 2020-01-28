#ifndef	getservconnip_h_included
#define	getservconnip_h_included

#include "commsrv.h"
#include "commproto.h"

inline bool getServConnIp( const CommServerConnection& conn, PString& ipAddr, int& port )
{
	if( conn.connPathLen() )
	{
		const char* str_IP = conn.connPathPoint( 0 );
		if( CommSsl0ProtocolHandler::handles( str_IP ) )
		{
			CommSsl0ProtocolHandler::parseSocketAddr( ipAddr, port, str_IP );
			return true;
		}
	}
	ipAddr = "";
	port = 0;
	return false;
}

#endif
