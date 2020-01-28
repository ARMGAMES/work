// ===========================================================
// Copyright (C) 2011 PYR Software Ltd. All rights reserved.
// ===========================================================

#ifndef AuthRedirector_h_included
#define AuthRedirector_h_included

#include "ppstring.h"
#include "commmsgbody.h"

////////////////////////////////////////////////////////////////////////////////
// PYR-18884/ PYR-19591

#ifndef AUTHRDR_REPLY_DELTA
#define AUTHRDR_REPLY_DELTA	1000
#endif

// Authenticaiton Redirector always connects to this connection factory
#define TRUSTED_ADMIN_CONNTYPE "trustedAdmin"

static inline int authrdrReplyId(int msgId) { return msgId + AUTHRDR_REPLY_DELTA; }

//
// Messages
//

// From Admin (kill, prepare for shutdown, etc.)
#define AUTHRDR_Q_ADMIN_TO_GATEWAY  100 // 4s4M - reqId, gatewayName, msgId, requestBody
#define AUTHRDR_A_ADMIN_TO_GATEWAY  (AUTHRDR_Q_ADMIN_TO_GATEWAY + AUTHRDR_REPLY_DELTA) // 46[s|...] - reqId, errCode[errDescr|...]
// PYR-20012 - create the new instance of the gateway via dispatcher AppFactory
#define AUTHRDR_Q_CREATE_OBJECT     200 // 4s - reqId, gatewayName
#define AUTHRDR_A_CREATE_OBJECT     (AUTHRDR_Q_CREATE_OBJECT + AUTHRDR_REPLY_DELTA) // 46[s|...] - reqId, errCode[errDescr|...]

#define AUTHRDR_Q_ADMIN_TO_DYNAMIC_OBJECT  300 // 4ss4M - reqId, address, instance, msgId, requestBody
#define AUTHRDR_A_ADMIN_TO_DYNAMIC_OBJECT  (AUTHRDR_Q_ADMIN_TO_DYNAMIC_OBJECT + AUTHRDR_REPLY_DELTA) // 46[s|...] - reqId, errCode[errDescr|...]

// message from Admin to Authentication Redirector
struct MsgAdminToAuthRedirector
{
    PString gatewayName;
    UINT32 gwMsgId;
    CommMsgBody gwMsgBody;

    // ctor for outgoing requests
    MsgAdminToAuthRedirector( const char* gatewayName_, UINT32 gwMsgId_, /*non-const!*/ CommMsgBody& gwMsgBody_ )
        : gatewayName( gatewayName_ )
        , gwMsgId( gwMsgId_ )
        , gwMsgBody( gwMsgBody_, true /*move*/)
    {}
    // ctor for incoming requests
    MsgAdminToAuthRedirector( CommMsgParser& parser )
    {
        parser
            .parseStringP( gatewayName )
            .parseUINT32( gwMsgId )
            .parseMsgBody( gwMsgBody )
            ;
    }

    CommMsgBody& compose( CommMsgBody& body ) const
    {
        return body
                .composeString( gatewayName )
                .composeUINT32( gwMsgId )
                .composeMsgBody( gwMsgBody )
                ;
    }
};
// message from Authentication Redirector to gateway
struct MsgAuthRedirectorToGateway
{
    PString adminId;
	UINT32 adminIntId;
    vector<PString> adminRights;
    CommMsgBody msgBody;

    // ctor for outgoing requests
    MsgAuthRedirectorToGateway( const char* adminId_, UINT32 adminIntId_, const vector<PString>& adminRights_, /*non-const!*/ CommMsgBody& msgBody_ )
        : adminId( adminId_ )
		, adminIntId( adminIntId_ )
        , adminRights( adminRights_ )
        , msgBody( msgBody_, true /*move*/)
    {}
    // ctor for incoming requests
    MsgAuthRedirectorToGateway( CommMsgParser& parser )
    {
        UINT32 size;
        parser
            .parseStringP( adminId )
			.parseUINT32( adminIntId )
            .parseUINT32( size );
        adminRights.resize( size );
        for( int i = 0; i < size; ++i )
        {
            parser.parseStringP( adminRights[i] );
        }
        parser.parseMsgBody( msgBody );
    }

    CommMsgBody& compose( CommMsgBody& body ) const
    {
        body
            .composeString( adminId )
			.composeUINT32( adminIntId )
            .composeUINT32( adminRights.size() );
        for( int i = 0; i < adminRights.size(); ++i )
        {
            body.composeString( adminRights[i] );
        }
        return body.composeMsgBody( msgBody );
    }
};

// message from Admin to Authentication Redirector
struct MsgAdminToAuthRedirectorDynamic
{
	PString address;
	PString instance;
	UINT32 gwMsgId;
	CommMsgBody gwMsgBody;

	// ctor for incoming requests
	MsgAdminToAuthRedirectorDynamic( CommMsgParser& parser )
	{
		parser
			.parseStringP( address )
			.parseStringP( instance )
			.parseUINT32( gwMsgId )
			.parseMsgBody( gwMsgBody )
			;
	}
};

#endif // AuthRedirector_h_included
