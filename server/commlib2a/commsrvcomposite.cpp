#include "ppinclude.h"
#include "ppstring.h"
#include "pperror.h"
#include "pplogfile.h"
#include "commmsgbody.h"
#include "commidmap.h"
#include "commqueue.h"
#include "commsrvcomposite.h"





bool CommCompositeServerObject::_safeProcessOutgoingPhysConnect(const PString & address )
{
	try
	{
		PSIGRETURN()
			processOutgoingPhysConnection(address);
	}
	catch( PError& err )
	{
		PLog( "PError caught/ignored in CommCompositeServerObject::processOutgoingPhysConnection: %s", err.why() );
	}
	catch( ... )
	{
		PLog( "... caught/ignored in CommCompositeServerObject::processOutgoingPhysConnection" );
	}
	return exited();
}

bool CommCompositeServerObject::_safeProcessPhysDisconnect( )
{
	try
	{
		PSIGRETURN()
			processPhysDisconnect();
	}
	catch( PError& err )
	{
		PLog( "PError caught/ignored in CommCompositeServerObject::processPhysDisconnect: %s", err.why() );
	}
	catch( ... )
	{
		PLog( "... caught/ignored in CommCompositeServerObject::processPhysDisconnect" );
	}
	return exited();
}

bool CommCompositeServerObject::_safeProcessPhysMessage(_CommMsg& msg )
{
	try
	{
		PSIGRETURN()
			processPhysMessage(msg);
	}
	catch( PError& err )
	{
		PLog( "PError caught/ignored in CommCompositeServerObject::_safeProcessPhysMessage ( %02X ): %s", msg.type, err.why() );
	}
	catch( ... )
	{
		PLog( "... caught/ignored in CommCompositeServerObject::_safeProcessPhysMessage ( %02X )",msg.type );
	}
	return exited();
}

void CommCompositeServerObject::createIncomingPhysConnection (const char * address)
{
	inter->_createIncomingPhysConnection(address);
}

void CommCompositeServerObject::postToPhysConnection (_CommMsg& msg, int srcMux, UINT32 srcMuxChannelId)
{
	inter->_postToPhysConnection(msg,srcMux,srcMuxChannelId);
}

void CommCompositeServerObject::disconnectPhysConnection ()
{
	inter->_disconnectPhysConnection();
}

void CommCompositeServerObject::physConnectionConnected (int isLocal)
{
	inter->_physConnectionConnected(isLocal);
}
