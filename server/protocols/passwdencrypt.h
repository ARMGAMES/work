#ifndef passwdencrypt_h
#define passwdencrypt_h

class PasswdEncryptDecrypt
{
private:
	enum {SALT_SIZE=8};
	static const char spwd[];
public:
	static void encrypt(const char *clearPasswd, PString & encryptedPasswd);
	static bool decrypt(const char *encryptedPasswd, PString & clearPasswd);

};

#endif

