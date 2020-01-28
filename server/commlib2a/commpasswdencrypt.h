#ifndef commpasswdencrypt_h
#define commpasswdencrypt_h

class CommPasswdEncryptDecrypt
{
private:
	enum {SALT_SIZE=8};
	static const char spwd[];
public:
	static void encrypt(const char *clearPasswd, PString & encryptedPasswd);
	static bool decrypt(const char *encryptedPasswd, PString & clearPasswd);

};

#endif

