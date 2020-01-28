#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"

#include "commmsgbody.h"
#include "commqueue.h"
#include "commsrv.h"
#include "commgrdpftppwd.h"
#include "commssl.h"

CommServerFtpPasswordGuard::CommServerFtpPasswordGuard()
: CommServerGuard( _COMM_PFTPPWD_GUARD_NAME )
{
}

/* virtual */ void CommServerFtpPasswordGuard::processFirstRequest( CommMsgParser& parser )
{
	parser.parseStringP( user ).parseStringP( pwdReceived );

	CommMsgBody request;
	if( !parser.parseEnded() )//--- temporary; for compatibility
		parser.parseMsgBody( clientExtra );
	conn->getUserAuth( _COMM_PFTPPWD_GUARD_NAME, user, clientExtra, request );
}

/* virtual */ void CommServerFtpPasswordGuard::processAddRequest( CommMsgParser& parser )
{
	PASSERT3( 0 );
}

/* virtual */ void CommServerFtpPasswordGuard::processOutBlock( CommMsgBody& dst, CommMsgBody& src )
{
	dst._composeVarBlock( src._readPtr(), src._size() );
}

/* virtual */ void CommServerFtpPasswordGuard::processInBlock( CommMsgBody& dst, CommMsgParser& src )
{
	const BYTE* p;
	size_t sz;
	src._parseVarBlock( p, sz );
	memcpy( dst._alloc( sz ), p, sz );
}

int CommServerFtpPasswordGuard::BinaryToX64(const unsigned char* input, int len, char* output)
{
	static const unsigned char BinaryToX64Map[64]  =
	{
		   'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', //  7
		   'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', // 15
		   'q', 'r', 's', 't', 'u', 'v', 'w', 'x', // 23
		   'y', 'z', '0', '1', '2', '3', '4', '5', // 31
		   'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', // 39
		   'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', // 47
		   'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', // 55
		   'Y', 'Z', '6', '7', '8', '9', '.', '_'  // 63
	};
       int                                      incount              =      0;
       int                                      outcount         =   0;
       unsigned char              current;
       output[outcount] = '\0';
       if (len < 1) return 0;
       while (1)
       {
              current = ((input[incount] & ((unsigned char) 0xFC)) >> 2);
              output[outcount++] = BinaryToX64Map[(int) current];
              current = ((input[incount] & ((unsigned char) 0x03)) << 4);
              incount++;
              if (--len == 0)
              {
                     output[outcount++] = BinaryToX64Map[(int) current];
                     break;
              }
              current |= ((input[incount] & ((unsigned char) 0xF0)) >> 4);
              output[outcount++] = BinaryToX64Map[(int) current];
              current = ((input[incount] & ((unsigned char) 0x0F)) << 2);
              incount++;
              if (--len == 0)
              {
                     output[outcount++] = BinaryToX64Map[(int) current];
                     break;
              }
              current |= ((input[incount] & ((unsigned char) 0xC0)) >> 6);
              output[outcount++] = BinaryToX64Map[(int) current];
              current = ((input[incount] & ((unsigned char) 0x3F)));
              output[outcount++] = BinaryToX64Map[(int) current];
              incount++;
              if (--len == 0) break;
       }
       output[outcount] = '\0';
       return outcount;
}


void CommServerFtpPasswordGuard::_crPassword(PString & outPwd, const char * pwd, UINT32 salt)
{

	static const BYTE abSBF[256] =
	{
		   0x62, 0x0E, 0xAA, 0xBB, 0x7D, 0x2B, 0x35, 0x5B, 0x06, 0xEB, 0x82, 0x08, 0x19, 0xA8, 0x95, 0xDE, 
		   0xC6, 0x07, 0x26, 0x51, 0xBC, 0x32, 0x81, 0x20, 0x65, 0x46, 0x10, 0xC9, 0x02, 0xAD, 0xE1, 0x1B, 
		   0x47, 0x43, 0x56, 0xAF, 0x14, 0x4B, 0x66, 0x9B, 0x48, 0x2C, 0xF4, 0x4C, 0x34, 0x24, 0x39, 0x1A, 
		   0x3D, 0xEA, 0x1F, 0xCE, 0x54, 0x91, 0x6A, 0x5E, 0x0B, 0x6F, 0x3E, 0x22, 0x09, 0x59, 0x25, 0x76, 
		   0x41, 0x37, 0x7A, 0x61, 0xA9, 0xCD, 0x72, 0xEC, 0x58, 0xB7, 0x3C, 0xAE, 0xBD, 0xEE, 0xD4, 0x50, 
		   0xE6, 0x01, 0x98, 0x67, 0x99, 0x3F, 0x5C, 0x9D, 0x9A, 0x9C, 0x8F, 0xBA, 0xE3, 0x04, 0x6E, 0xA5, 
		   0x38, 0x4F, 0x8E, 0x5F, 0xD0, 0x68, 0xDF, 0xAC, 0x0D, 0xD9, 0x80, 0xE7, 0xE9, 0xC5, 0x0F, 0x96, 
		   0x2F, 0xA2, 0xF8, 0x8D, 0xBE, 0x49, 0x85, 0xD8, 0xDA, 0xB2, 0x94, 0x4A, 0xC3, 0x23, 0xF5, 0xD2, 
		   0xCC, 0xDB, 0x75, 0xDD, 0x7C, 0xF2, 0xD7, 0xFE, 0xB9, 0x0C, 0x36, 0xB6, 0xB1, 0x2E, 0x87, 0xC0, 
		   0x97, 0x6B, 0xF9, 0x63, 0xFC, 0xA1, 0xC2, 0xB3, 0xA7, 0x2A, 0x30, 0x1C, 0xFD, 0xC7, 0xCF, 0x86, 
		   0x7B, 0xFA, 0xCB, 0x18, 0xA3, 0x6C, 0x8B, 0x0A, 0xC8, 0xE0, 0x33, 0xB8, 0x00, 0x69, 0x70, 0x55, 
		   0xD5, 0xD6, 0xA4, 0x4E, 0x53, 0xBF, 0xB5, 0xFB, 0xA6, 0xF3, 0xAB, 0xC1, 0x6D, 0xF7, 0xE8, 0xC4, 
		   0x78, 0xB4, 0x28, 0x42, 0x88, 0xCA, 0xE4, 0x40, 0x2D, 0x9F, 0xB0, 0x52, 0xE2, 0xEF, 0xD1, 0x44, 
		   0xED, 0xF6, 0x64, 0x93, 0x21, 0x1D, 0x1E, 0x12, 0x8C, 0xD3, 0x71, 0x60, 0x29, 0xE5, 0x83, 0xDC, 
		   0x73, 0x90, 0x4D, 0x3B, 0x89, 0x16, 0x8A, 0x13, 0x31, 0x74, 0x03, 0x15, 0x57, 0x84, 0x7F, 0x05, 
		   0x77, 0x9E, 0x5A, 0x7E, 0x45, 0xF0, 0x92, 0x3A, 0xA0, 0x11, 0x27, 0x5D, 0xF1, 0xFF, 0x17, 0x79
	};
	PBlock hs;
	CommSSLChksum sha1;
	sha1.addData((const BYTE*) "FTP!",4);
	sha1.addData((const BYTE*) &salt,4);  
	sha1.chksum(hs);
	BYTE bSalt[8];
	bSalt[0]=hs.ptr()[3];
	bSalt[1]=hs.ptr()[4];
	bSalt[2]=abSBF[((BYTE *)&salt)[1]];
	bSalt[3]=hs.ptr()[5];
	bSalt[4]=hs.ptr()[6];
	bSalt[5]=hs.ptr()[7];
	bSalt[6]=hs.ptr()[8];
	bSalt[7]=hs.ptr()[9];

	char salt64[16];
	BinaryToX64(bSalt,8,salt64);
	CommSSLChksum sha1_1;
	PBlock passwHs;
	sha1_1.addData((const BYTE*) salt64,strlen(salt64));
	sha1_1.addData((const BYTE*) pwd,strlen(pwd));  
	sha1_1.chksum(passwHs);
	char pwd64[40];
	BinaryToX64(passwHs.ptr(),20,pwd64);
	outPwd = pwd64;
}

bool CommServerFtpPasswordGuard::_checkPassword(const char * pwd, const char * db_pwd, UINT32 salt)
{
	PString pwdOut;
	_crPassword(pwdOut,pwd,salt);
	return pwdOut.equals(db_pwd);

}

void CommServerFtpPasswordGuard::encodePassword(const char * pwd, UINT32 salt, PString & pwdOut)
{
	_crPassword(pwdOut,pwd,salt);
}


/* virtual */ bool CommServerFtpPasswordGuard::gotUserAuth( CommMsgParser* parser, UINT16 errCode, const char* errMsg, const CommMsgBody* errMsgBody )
{
	if( parser )
	{
		const char* encrPassword;
		UINT32 salt;
		bool ok = false;
		parser->parseString( encrPassword ).parseUINT32(salt);
		ok = _checkPassword(pwdReceived,encrPassword,salt);
		if( ok )
		{
			CommMsgBody body;
			_postGuardRequestGranted( body,errMsgBody );
			return true;
		}
	}

	_postGuardError( errCode ? errCode : _COMM_ERROR_AUTHFAILED, errMsg ? errMsg : "Authentication failed: no such user or invalid password", errMsgBody );
	return false;
}

/* virtual */ void CommServerFtpPasswordGuard::delegateUserAuth(  UINT16 errCode, const char* errMsg, const CommMsgBody* errMsgBody )
{
	if (errCode)
	{
		_postGuardError(  errCode , errMsg , errMsgBody );
	}
	else
	{
		CommMsgBody body;
		_postGuardRequestGranted( body, errMsgBody );
	}
}

/* virtual */ bool CommServerFtpPasswordGuard::checkUserAuth( CommMsgParser* parser ) const
{
	if( parser )
	{
		const char* encrPassword;
		bool ok = false;
		UINT32 salt;
		parser->parseString( encrPassword ).parseUINT32(salt);
		ok = _checkPassword(pwdReceived,encrPassword,salt);

		if( ok )
		{
			return true;
		}
	}
	return false;
}


//****************************************************************************

/* virtual */ /* new */ CommServerGuard* CommServerFtpPasswordGuardFactory::createGuard( const vector< const char* >& guardNames )
{
	int n = guardNames.size();
	for( int i=0; i < n ; ++i )
	{
		if( strcmp( guardNames[ i ], _COMM_PFTPPWD_GUARD_NAME ) == 0 )
			return new CommServerFtpPasswordGuard();
	}
	return 0;
}
