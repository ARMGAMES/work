#include "ppinclude.h"
#include "commssl.h"
#include "commpasswdencrypt.h"

/*static*/ const char CommPasswdEncryptDecrypt::spwd[] = "f4â-seyvjF§¦ëà?HnïdìjHnFyT-ëà?54dDn½d-ñsDÆÑsñ?@âs-D";

/*static*/ void CommPasswdEncryptDecrypt::encrypt(const char *clearPasswd, PString & encryptedPasswd)
{
	CommSSLEncryptAES enc( (const BYTE *)spwd, sizeof(spwd), true );
	char salt[SALT_SIZE];
	CommSSL::generateRandomBytes( (BYTE*)salt, sizeof(salt));
	enc.addData( (const BYTE*)salt, sizeof(salt) );
	enc.addData( (const BYTE*)clearPasswd, strlen( clearPasswd ) + 1 );
	PBlock tmp;
	enc.encrypt( tmp );
	memcpy(tmp.append(sizeof(salt)),salt,sizeof(salt));
	CommSSLBase64::encode( encryptedPasswd, tmp.ptr(), tmp.size() );
}

/*static*/ bool CommPasswdEncryptDecrypt::decrypt(const char *encryptedPasswd, PString & clearPasswd)
{
	PBlock tmp;
	CommSSLBase64::decode( tmp, encryptedPasswd );

	CommSSLDecryptAES dec( (const BYTE *) spwd, sizeof(spwd), true );
	dec.addData( tmp.ptr(), tmp.size() - SALT_SIZE );
	PBlock block;
	dec.decrypt( block );
	if( block.size() == 0 || *( block.ptr() + block.size() - 1 ) != 0 )
	{
		return false;
	}
	if (memcmp(block.ptr(),tmp.ptr() + tmp.size() - SALT_SIZE, SALT_SIZE))
	{
		return false;
	}
	clearPasswd = "";
	int l = block.size() - 1;
	for( int i = SALT_SIZE; i < l ; ++i )
	{
		BYTE c = block.ptr()[ i ];
		if( c == 0 )
		{
			return false;
		}
		else
		{
			clearPasswd.append( (char)c );
		}
	}
	return true;
}