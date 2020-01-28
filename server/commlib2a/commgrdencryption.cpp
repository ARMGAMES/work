#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"

#include "commmsgbody.h"
#include "commqueue.h"
#include "commsrv.h"
#include "commgrdencryption.h"
static const unsigned char dhP[] ={0xDB,0xC6,0x40,0x2B,0x27,0xB1,0xDB,0xD9,
	                               0xD0,0x88,0xB3,0x8D,0x1A,0x7E,0xA5,0x8F,0xD,0x63,0xD4,
							       0x41,0x73,0xDB,0x44,0x6A,0xD0,0xD7,0xB8,0x11,0x77,0x20,0x18,0x2F};

static int dhLenP=32;


void CommClientGuardEncryption::composeFirstRequest( CommMsgBody& msgBody )
{
	if (dh)
		delete dh;
	if (ecdh)
		delete ecdh;

	secret.clear();
	PBlock publicKey;
	msgBody.composeINT32(myInitVersion);
	if (myInitVersion == _version_3)
	{
		ecdh = new CommSSLECDH();
		ecdh->generateKeys(publicKey);
	}
	else
	{
		dh = new CommSSLDH();
		dh->setPrime(dhP,dhLenP,5);
		msgBody._composeVarBlock(dhP,dhLenP);
		dh->generateKeys(publicKey);
	}
	msgBody._composeVarBlock(publicKey.ptr(),publicKey.size());
}
