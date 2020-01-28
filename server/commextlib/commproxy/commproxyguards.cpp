#include "commproxy.h"


// PYR-38736 #oleksandrb "Moved" from arjel folder

#define _COMM_PROXY_GUARD_NAME "GENERIC"

CommProxyServerGuard::CommProxyServerGuard(  ):
	CommServerGuard(_COMM_PROXY_GUARD_NAME)
{
	version=0;
}


const char * CommProxyServerGuard::currentName() const
{
	return cName.length()? cName.c_str():name();
}

CommServerGuard* CommProxyServerGuardFactory::createGuard( const vector< const char* >& guardNames )
{
	CommProxyServerGuard * gd =new CommProxyServerGuard();
	bool flag = false;

	for (int i=0; i<guardNames.size();i++)
	{
		if (!strcmp(guardNames[i],_COMM_PPWD_GUARD_NAME) ||
			!strcmp(guardNames[i],_COMM_NULL_GUARD_NAME) ||
			!strcmp(guardNames[i],_COMM_PPWDSID_GUARD_NAME) ||
			!strcmp(guardNames[i],_COMM_WEBTOKEN_GUARD_NAME) ||
			!strcmp(guardNames[i],_COMM_TOKEN_GUARD_NAME) ||
			!strcmp(guardNames[i],_COMM_WEBTOKENINT_GUARD_NAME) || // PYR-42014
			!strcmp(guardNames[i],_COMM_ENCRYPTEDPASSWORD_GUARD_NAME) || // PYR-44459
			!strcmp(guardNames[i],_COMM_ENCRYPTEDPASSWORDSID_GUARD_NAME) // PYR-44459
			)
		{
			flag = true;
			gd->add(guardNames[i]);
		}
	}
	if (flag == false)
	{
		delete gd;
		return 0;
	}
	else
		return gd;
}

void CommProxyClientGuardFactory :: composeGuardNameList( CommMsgBody& msgBody ) const
{
	serverGuard->composeGuardNameList(msgBody);
}
void CommProxyServerGuard::composeGuardNameList(CommMsgBody& msgBody) const
{
	list <PString>::const_iterator it; 
	for (it=names.begin(); it!=names.end(); ++it)
	{
		msgBody.composeString((*it).c_str());
	}

}
bool CommProxyServerGuard::checkGuard( const char* guardName) 
{
	list <PString>::iterator it; 
	for (it=names.begin(); it!=names.end();++it)
	{
		if (!strcmp(guardName,(*it).c_str()))
		{
			cName = guardName;
			break;
		}
	}
	return it != names.end();
 }

void CommProxyServerGuard::processFirstRequest(CommMsgParser & parser)
{
	CommProxyServerConnection *conn_ = (CommProxyServerConnection *)conn;
	CommMsgBody body;
	body.copyMsgBodyFromParser(parser);
	bool isEncPswGuard = false;
	// parse version, userId and clientExtraData for later use
	// each guard has a different protocol, see guard protocols in commlib
	if (cName.equals(_COMM_TOKEN_GUARD_NAME))
	{
		PBlock dummyTokenReceived;
		parser
			.parseINT32(version)
			.parseStringP(conn_->root->user)
			.parsePBlock(dummyTokenReceived);
		if( !parser.parseEnded() )//--- temporary; for compatibility - SSH: this is an exact copy of commlib code
		{
			parser.parseMsgBody(conn_->root->clientExtraDataBody);
		}
		else
		{
			PLog("WARN: no client extra data for grd=%s", cName.c_str()); // SSH: monitor logs for this message, remove parseEnded after BUILD 2016.08
		}
	}
	else if (cName.equals(_COMM_WEBTOKEN_GUARD_NAME))
	{
		const char * dummyToken;
		parser
			.parseINT32(version)
			.parseStringP(conn_->root->user)
			.parseString(dummyToken);
		if( !parser.parseEnded() )//--- temporary; for compatibility - SSH: this is an exact copy of commlib code
		{
			parser.parseMsgBody(conn_->root->clientExtraDataBody);
		}
		else
		{
			PLog("WARN: no client extra data for grd=%s", cName.c_str()); // SSH: monitor logs for this message, remove parseEnded after BUILD 2016.08
		}
	}
	else if (cName.equals(_COMM_PPWD_GUARD_NAME))
	{
		const char* dummyPsw;
		parser
			.parseStringP(conn_->root->user)
			.parseString(dummyPsw);
		if( !parser.parseEnded() )//--- temporary; for compatibility - SSH: this is an exact copy of commlib code
		{
			parser.parseMsgBody(conn_->root->clientExtraDataBody);
		}
		else
		{
			PLog("WARN: no client extra data for grd=%s", cName.c_str()); // SSH: monitor logs for this message, remove parseEnded after BUILD 2016.08
		}
		PLog("grd=%s is in use", cName.c_str()); // SSH: monitor logs for this message, if not found before BUILD 2016.08 support for this guard can be dropped
	}
	else if (cName.equals(_COMM_PPWDSID_GUARD_NAME))
	{
		const char* dummyPsw, *dummySid;
		parser
			.parseStringP(conn_->root->user)
			.parseString(dummyPsw)
			.parseString(dummySid);
		if( !parser.parseEnded() )//--- temporary; for compatibility - SSH: this is an exact copy of commlib code
		{
			parser.parseMsgBody(conn_->root->clientExtraDataBody);
		}
		else
		{
			PLog("WARN: no client extra data for grd=%s", cName.c_str()); // SSH: monitor logs for this message, remove parseEnded after BUILD 2016.08
		}
	}
	else if (cName.equals(_COMM_WEBTOKENINT_GUARD_NAME)) // PYR-42014
	{
		UINT32 userIntId;
		const char * dummyToken;
		parser
			.parseINT32(version)
			.parseUINT32(userIntId)
			.parseString(dummyToken)
			;
		if( !parser.parseEnded() )//--- temporary; for compatibility - SSH: this is an exact copy of commlib code
		{
			parser.parseMsgBody(conn_->root->clientExtraDataBody);
		}
		else
		{
			PLog("WARN: no client extra data for grd=%s", cName.c_str()); // SSH: monitor logs for this message, remove parseEnded after BUILD 2016.08
		}
		// init userId with userIntId + guard name
		conn_->root->user.assign("").appendUint(userIntId).append("_from_").append(cName);
		conn_->root->authenticatedByUserIntId = true;
	}
	else if (cName.equals(_COMM_ENCRYPTEDPASSWORD_GUARD_NAME) || 
			 cName.equals(_COMM_ENCRYPTEDPASSWORDSID_GUARD_NAME))
	{
		isEncPswGuard = true;
		parser
			.parseINT32(version);
	}
//=================
	if (!isEncPswGuard) // encrypted psw guards will be logged in processAddRequest() when userId is available
	{
		PLog("cid=%08X,g=%s,v=%d,u='%s',sz(cedb)=%d", conn_->id(), cName.c_str(), version, conn_->root->user.c_str(), conn_->root->clientExtraDataBody._size());
	}
	conn_->root->obj._clientPostGuardRequest(conn_->root,body);
}

void CommProxyServerGuard::processAddRequest( CommMsgParser& parser )
{
	CommProxyServerConnection *conn_ = (CommProxyServerConnection *)conn;
	CommMsgBody body;
	body.copyMsgBodyFromParser(parser);
	// parse userId and clientExtraData for later use
	if (cName.equals(_COMM_ENCRYPTEDPASSWORD_GUARD_NAME))
	{
		const BYTE* dummyEncUser, *dummyEncPsw;
		size_t dummyEncUserLen, dummyEncPswLen;
		parser
			.parseStringP(conn_->root->user)
			._parseVarBlock(dummyEncUser, dummyEncUserLen)
			._parseVarBlock(dummyEncPsw, dummyEncPswLen)
			;
		if( !parser.parseEnded() )//--- temporary; for compatibility - SSH: this is an exact copy of commlib code
		{
			parser.parseMsgBody(conn_->root->clientExtraDataBody);
		}
		else
		{
			PLog("WARN: no client extra data for grd=%s", cName.c_str()); // SSH: monitor logs for this message, remove parseEnded after BUILD 2016.08
		}
		PLog("grd=%s is in use", cName.c_str()); // SSH: monitor logs for this message, if not found before BUILD 2016.08 support for this guard can be dropped
	}
	else if (cName.equals(_COMM_ENCRYPTEDPASSWORDSID_GUARD_NAME))
	{
		const BYTE* dummyEncUser, *dummyEncPsw, *dummyEncSid;
		size_t dummyEncUserLen, dummyEncPswLen, dummyEncSidLen;
		parser
			.parseStringP(conn_->root->user)
			._parseVarBlock(dummyEncUser, dummyEncUserLen)
			._parseVarBlock(dummyEncPsw, dummyEncPswLen)
			._parseVarBlock(dummyEncSid, dummyEncSidLen);
			;
		if( !parser.parseEnded() )//--- temporary; for compatibility - SSH: this is an exact copy of commlib code
		{
			parser.parseMsgBody(conn_->root->clientExtraDataBody);
		}
		else
		{
			PLog("WARN: no client extra data for grd=%s", cName.c_str()); // SSH: monitor logs for this message, remove parseEnded after BUILD 2016.08
		}
	}
	else
	{
		PLog("unexpected guard: %s", cName.c_str());
	}
	PLog("cid=%08X,g=%s,v=%d,u='%s',sz(cedb)=%d", conn_->id(), cName.c_str(), version, conn_->root->user.c_str(), conn_->root->clientExtraDataBody._size());
	conn_->root->obj._clientPostGuardAddRequest(conn_->root,body);
}
