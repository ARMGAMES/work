#ifndef MsgAuthPlayer_h_included
#define MsgAuthPlayer_h_included

#ifndef PROTOCOL_CLIENT_ONLY
#include "commlib.h"
#endif

#include "MsgCommon.h"
#include "auth.h"

// message IDs for player connection
#define AUTH_Q_LOGIN_RSA 1			// request to login with RSA passcode
#define AUTH_A_LOGIN_RSA 2			// response to the login with RSA passcode
#define AUTH_Q_LOGIN_OTP 3			// request to login with OTP
#define AUTH_A_LOGIN_OTP 4			// response to login with OTP
#define AUTH_Q_LOGIN_LTD 5			// request to login in limited mode
#define AUTH_A_LOGIN_LTD 6			// response to login in limited mode
#define AUTH_U_TOKEN_UPDATE 7		// new security tokens notification 
#define AUTH_U_LOGOUT_NOW 8			// player is about to log out (close the session immediately)

#define AUTH_Q_GET_LOGIN_METHOD 11	// request to retrieve the current login method
#define AUTH_A_GET_LOGIN_METHOD 12	// response to retrieve the current login method
#define AUTH_Q_SET_LOGIN_METHOD 13	// request to change the login method
#define AUTH_A_SET_LOGIN_METHOD 14	// response to change the login method
#define AUTH_Q_GENERATE_PSP 15		// request to generate a new PokerStars PIN
#define AUTH_A_GENERATE_PSP 16		// response to generate a new PokerStars PIN
#define AUTH_Q_ACTIVATE_RSA 17		// request to activate an RSA token
#define AUTH_A_ACTIVATE_RSA 18		// response to activate an RSA token
#define AUTH_Q_CHECK_RSA 19			// request to check if RSA token with specified serial number is available
#define AUTH_A_CHECK_RSA 20			// response to check if RSA token with specified serial number is available
#define AUTH_Q_SET_LOGIN_METHOD_EX 21 // same as AUTH_Q_SET_LOGIN_METHOD but the password is hashed by the client
// PYR-16777
#define AUTH_Q_GENERATE_OTP 23		// request to generate a new one time password
#define AUTH_A_GENERATE_OTP 24		// response to generate a new one time password
// PYR-16706
#define AUTH_DK_EXPLICIT_LOGIN 25	// result of explicit login to .DK site

// #21829
#define AUTH_Q_SET_NEWINSTALL_VALIDATION 26 // request (enable/disable) SMS validation for new install id 
#define AUTH_A_SET_NEWINSTALL_VALIDATION 27 // response (enable/disable) SMS validation for new install id

// PYR-27972
#define AUTH_Q_LOGIN_EMAIL_NOTIFICATION	28	// b - setOrRemove	request (enable/disable) Login Email Notification
#define AUTH_A_LOGIN_EMAIL_NOTIFICATION	29	// response (enable/disable) Login Email Notification

// PYR-54466
#define AUTH_Q_SET_PSP 30		// request to set a new user-chosen PokerStars PIN
#define AUTH_A_SET_PSP 31		// response to set a new user-chosen PokerStars PIN
#define AUTH_Q_CHANGE_PSP 32		// request to change the PokerStars PIN
#define AUTH_A_CHANGE_PSP 33		// response to change the PokerStars PIN

// result codes
#define AUTH_SUCCESS 0				// successful result of the request
#define AUTH_NEXT_CODE 1			// next token code is required (for RSA tokens)
#define AUTH_LOGIN_FAILED 2			// supplied credentials are invalid 
#define AUTH_TOKEN_DISABLED 3		// RSA token is not set up or disabled
#define AUTH_SESSION_INVALID 4		// the session has expired or ST1-RN did not match
#define AUTH_NOT_AUTHORIZED 5		// the requested operation is not authorized for this user
#define AUTH_INVALID_PARAM 6		// one or more parameters are not valid
#define AUTH_TOKEN_INUSE 7			// the requested token already in use by somebody else
#define AUTH_TOKEN_UNKNOWN 8		// the requested token not found in the database
#define AUTH_PSP_PATTERN_RESTRICTED 9	// PYR-54466 - The PSP chosen by the user matches a restricted pattern.
#define AUTH_FAILURE 100			// generic error 

// request to login with RSA passcode
class MsgAuthQLoginRsa
{
public:
	const char* rsaPin;
	const char* rsaCode;	

	// constructor for an incoming message
	MsgAuthQLoginRsa( CommMsgParser& parser )
	{
		parser.parseString(rsaPin);
		parser.parseString(rsaCode);
	}

	// constructor for outgoing request
	MsgAuthQLoginRsa( const char* _rsaPin, const char* _rsaCode )
	{
		rsaPin = _rsaPin;
		rsaCode = _rsaCode;
	}

	// compose outgoing message
	CommMsgBody& compose( CommMsgBody& msg ) const
	{
		msg.composeString(rsaPin);
		msg.composeString(rsaCode);
		return msg;
	}
};

// response to RSA login 
typedef MsgGenericResponse MsgAuthALoginRsa;


// request to login with One Time password
class MsgAuthQLoginOtp
{
public:
	const char* otp;	
	const char* pin;

	// constructor for an incoming message
	MsgAuthQLoginOtp( CommMsgParser& parser )
	{
		parser.parseString(otp);
		if( !parser.parseEnded() )
			parser.parseString(pin);
		else
			pin = "";
	}

	// constructor for outgoing request
	MsgAuthQLoginOtp( const char* _otp, const char* _pin = "" )
	{
		otp = _otp;
		pin = _pin;
	}

	// compose outgoing message
	CommMsgBody& compose( CommMsgBody& msg ) const
	{
		return msg.
			composeString(otp).
			composeString(pin);
	}
};

// OTP login request message
typedef MsgGenericResponse MsgAuthALoginOtp;

// request to login in limited mode
typedef MsgEmptyBody MsgAuthQLoginLtd;

// response to login in limited mode
typedef MsgGenericResponse MsgAuthALoginLtd;

// message to update security tokens
class MsgAuthUTokenUpdate
{
public:
	BYTE version;		
	CommMsgBody rsaBody;
	CommMsgBody aesBody;
	CommMsgBody tknBody;

	// constructor for incoming messages
	MsgAuthUTokenUpdate( CommMsgParser& parser )
	{
		parser.parseBYTE(version);
		PASSERT3(version==1);
		parser.parseMsgBody(rsaBody);
		parser.parseMsgBody(aesBody);
		parser.parseMsgBody(tknBody);
	}

	// constructor for outgoing messages
	MsgAuthUTokenUpdate()
	{
		version = 1;
	}

	// compose outgoing message
	CommMsgBody& compose( CommMsgBody& msg ) const
	{
		msg.composeBYTE(version);
		msg.composeMsgBody(rsaBody);
		msg.composeMsgBody(aesBody);
		msg.composeMsgBody(tknBody);
		return msg;
	}
};


// request to generate a PokerStars PIN
typedef MsgEmptyBody MsgAuthQGeneratePSP;

// response to generate a PokerStars PIN
typedef MsgGenericResponse MsgAuthAGeneratePSP;

// PYR-54466 - request to set a new user-chosen PokerStars PIN
class MsgAuthQSetPSP
{
public:
	const char* newPsp;

	// constructor for an incoming message
	MsgAuthQSetPSP(CommMsgParser& parser)
	{
		parser.parseString(newPsp);
	}

	// constructor for outgoing request
	MsgAuthQSetPSP(const char* _newPsp)
	{
		newPsp = _newPsp;
	}

	// compose outgoing message
	CommMsgBody& compose(CommMsgBody& msg) const
	{
		msg.composeString(newPsp);
		return msg;
	}
};
// PYR-54466 - response to set a new user-chosen PokerStars PIN
typedef MsgGenericResponse MsgAuthASetPSP;

// PYR-54466 - request to change a user-chosen PokerStars PIN
class MsgAuthQChangePSP
{
public:
	const char* oldPsp;
	const char* newPsp;

	// constructor for an incoming message
	MsgAuthQChangePSP(CommMsgParser& parser)
	{
		parser.parseString(oldPsp);
		parser.parseString(newPsp);
	}

	// constructor for outgoing request
	MsgAuthQChangePSP(const char* _oldPsp, const char* _newPsp)
	{
		oldPsp = _oldPsp;
		newPsp = _newPsp;
	}

	// compose outgoing message
	CommMsgBody& compose(CommMsgBody& msg) const
	{
		msg.composeString(oldPsp);
		msg.composeString(newPsp);
		return msg;
	}
};
// PYR-54466 - response to change a new user-chosen PokerStars PIN
typedef MsgGenericResponse MsgAuthAChangePSP;

// activate an RSA token
class MsgAuthQActivateRsa 
{
public:
	UINT32 serialNumber;
	const char* rsaPin;
	const char* rsaCode;

	// constructor for an incoming message
	MsgAuthQActivateRsa( CommMsgParser& parser )
	{
		parser.parseUINT32(serialNumber);
		parser.parseString(rsaPin);
		parser.parseString(rsaCode);
	}

	// constructor for outgoing RSA request
	MsgAuthQActivateRsa( UINT32 _serialNumber, const char* _rsaPin, const char* _rsaCode )
	{
		serialNumber = _serialNumber;
		rsaPin = _rsaPin;
		rsaCode = _rsaCode;
	}

	// compose outgoing message
	CommMsgBody& compose( CommMsgBody& msg ) const
	{
		msg.composeUINT32(serialNumber);
		msg.composeString(rsaPin);
		msg.composeString(rsaCode);
		return msg;
	}
};

// response to generate a PSP
typedef MsgGenericResponse MsgAuthAActivateRsa;

// request to get current login method for the user
typedef MsgEmptyBody MsgAuthQGetLoginMethod;

// response to get current login method for the user
class MsgAuthAGetLoginMethod: public MsgGenericResponse
{
public:
	UINT32 loginMethod;		// see AUTH_LOGINTYPE_* constants
	UINT32 loginPriv;		// bit mask of available login methods

	// constructor for an incoming message
	MsgAuthAGetLoginMethod( CommMsgParser& parser )
	: MsgGenericResponse( parser )
	{
		if( resultCode == AUTH_SUCCESS )
		{
			parser.parseUINT32(loginMethod);
			parser.parseUINT32(loginPriv);
		}
		else
		{
			loginMethod = 0;
			loginPriv = 0;
		}
	}

	// constructor for outgoing message in success scenario
	MsgAuthAGetLoginMethod( UINT32 _loginMethod, UINT32 _loginPriv )
	: MsgGenericResponse( AUTH_SUCCESS )
	{
		loginMethod = _loginMethod;
		loginPriv = _loginPriv;
	}

	// constructor for outgoing message in negative scenario
	MsgAuthAGetLoginMethod( UINT16 _errorCode, const char* _errorStr )
	: MsgGenericResponse( _errorCode, _errorStr )
	{
		loginMethod = 0;
		loginPriv = 0;
	}

	// compose outgoing message
	CommMsgBody& compose( CommMsgBody& msg ) const
	{
		MsgGenericResponse::compose(msg);
		if( resultCode == AUTH_SUCCESS )
		{
			msg.composeUINT32(loginMethod);
			msg.composeUINT32(loginPriv);
		}
		return msg;
	}
};


// request to set current login method for the user
class MsgAuthQSetLoginMethod
{
public:
	UINT32 loginMethod; // new loginMethod mask
	const char* password; // user's (hashed) password
	const char* sid; // PokerStars PIN
	const char* rsaPin; // RSA PIN
	const char* rsaCode; // RSA PIN

	// constructor for an incoming message
	MsgAuthQSetLoginMethod( CommMsgParser& parser )
	{
		parser.parseUINT32(loginMethod);
		parser.parseString(password);
		parser.parseString(sid);
		parser.parseString(rsaPin);
		parser.parseString(rsaCode);
	}

	// constructor for outgoing RSA request
	MsgAuthQSetLoginMethod( UINT32 _loginMethod, const char* _password, const char* _sid, 
		const char* _rsaPin, const char* _rsaCode )
	{
		loginMethod = _loginMethod;
		password = _password;
		sid = _sid;
		rsaPin = _rsaPin;
		rsaCode = _rsaCode;
	}

	// compose outgoing message
	CommMsgBody& compose( CommMsgBody& msg ) const
	{
		msg.composeUINT32(loginMethod);
		msg.composeString(password);
		msg.composeString(sid);
		msg.composeString(rsaPin);
		msg.composeString(rsaCode);
		return msg;
	}
};

// response to set current login method for the user
typedef MsgGenericResponse MsgAuthASetLoginMethod;


// check the availability of an RSA token
class MsgAuthQCheckRsa 
{
public:
	UINT32 serialNumber;

	// constructor for an incoming message
	MsgAuthQCheckRsa( CommMsgParser& parser )
	{
		parser.parseUINT32(serialNumber);
	}

	// constructor for outgoing RSA request
	MsgAuthQCheckRsa( UINT32 _serialNumber )
	{
		serialNumber = _serialNumber;
	}

	// compose outgoing message
	CommMsgBody& compose( CommMsgBody& msg ) const
	{
		return msg.composeUINT32(serialNumber);
	}
};

// response to generate a PSP
typedef MsgGenericResponse MsgAuthACheckRsa;

// PYR-16777

// request to generate OTP and send it via SMS
typedef MsgEmptyBody MsgAuthQGenOTP;

// response to generate OTP and send it via SMS
class MsgAuthAGenOTP : public MsgGenericResponse
{
public:
	PString mobile;

	// constructor for an incoming message
	MsgAuthAGenOTP( CommMsgParser& parser )
		: MsgGenericResponse( parser )
	{
		if( resultCode == AUTH_SUCCESS )
		{
			parser.parseStringP(mobile);
		}
	}

	// constructor for outgoing message in success scenario
	MsgAuthAGenOTP( const PString& mobile_ )
		: MsgGenericResponse( AUTH_SUCCESS )
	{
		 mobile =  mobile_;
	}

	// constructor for outgoing message in negative scenario
	MsgAuthAGenOTP( UINT16 _errorCode, const char* _errorStr )
		: MsgGenericResponse( _errorCode, _errorStr )
	{}

	// compose outgoing message
	CommMsgBody& compose( CommMsgBody& msg ) const
	{
		MsgGenericResponse::compose(msg);
		if( resultCode == AUTH_SUCCESS )
		{
			msg.composeString( mobile);
		}
		return msg;
	}
};

// result of explicit login to .DK site
typedef MsgGenericResponse MsgAuthDkExplicitLogin;

// PYR-21829
class MsgAuthQSetEnableSmsValidation
{
public:
	bool enable;

	// constructor for an incoming message
	MsgAuthQSetEnableSmsValidation( CommMsgParser& parser )
	{
		parser.parseBOOL(enable);
	}

	// constructor for outgoing SMS request
	MsgAuthQSetEnableSmsValidation( bool _enable) 
	{
		enable = _enable;
	}

	// compose outgoing message
	CommMsgBody& compose( CommMsgBody& msg ) const
	{
		msg.composeBOOL(enable);
		return msg;
	}
};

// response to enable/disable SMS validation
typedef MsgGenericResponse MsgAuthASetEnableSmsValidation;

#endif
