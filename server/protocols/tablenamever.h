#ifndef TABLEVERNAME_H
#define TABLEVERNAME_H

#include "pplib.h"
#include "commlib.h"

// 1. Client uses only macros
// 2. Dispatcher uses a single function call
// 3. Client implementation for current release only
// 4. Dispatcher implementation keeps all previous releases

// Version table
// Version    Prefix      Encoding    Checksum  Encryption 
// ---------- ----------- ----------- --------- ---------- 
//       0    table.      none        none      none       
//       1    tablen.1.   base64      S(bytes)  RC4/SSL    
//       2    tablen.2.   ...         ...        
//

//
// Version constants
//

#define TNV_VERSION_0           0

#define TNV_VERSION_1           1
#define TNV_KEY_1               "tmMejs74p"
#define TNV_SALT_1              "zeiDnc8JK"

// ...

// 
// Client calling interface - this is only a template, any amount of
// obfuscation is welcomed; test the result with main() in cpp file
//
//TNV_ENCRYPT(const char* in, PString& out)
//

#define TNV_ENCRYPT(in, out) \
{ \
	char ver[16]; \
	UINT32 sum = 0; \
	for (int i = 0; in[i]; ++i) \
		sum += (unsigned int) in[i]; \
	PBlock crypted; \
	BYTE* salt = (BYTE*) TNV_SALT_1; \
	CommSSLStreamEncrypt cryptor(TNV_KEY_1, salt); \
	cryptor.encrypt((const BYTE*) in, strlen(in), crypted); \
	crypted.insert(0, 1)[0] = (BYTE)(sum % 256); \
	PString encoded; \
	CommSSLBase64::encode(encoded, crypted.ptr(), crypted.size()); \
	out.assign("tablen.").append(p_utoa(TNV_VERSION_1, ver)).append(".").append(encoded); \
}

#define BNV_ENCRYPT(in, out) \
{ \
	out.assign("blitz.").append(in); \
}

//
// Dispatcher calling interface
//

extern int tnv_decrypt(const char* in, PString& out);

#endif
