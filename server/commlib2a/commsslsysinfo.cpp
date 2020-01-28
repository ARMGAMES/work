#include "ppinclude.h"
#include "commssl.h"

/* virtual */ void CommSSLSystemInfo::generateInstallId( UINT32& a, UINT32& b )
{
	BYTE p[ 8 ];
	CommSSL::generateRandomBytes( p, 8 );
	a = *(UINT32*)p;//don't care about byte order
	b = *(UINT32*)( p + 4 );//don't care about byte order
}

/* virtual */ void CommSSLSystemInfo::generateMD5( const PString& s, PString& md5hash )
{
	generateMD5((const BYTE*)s.c_str(), s.length(), md5hash);
}

void CommSSLSystemInfo::generateMD5( const BYTE* ptr, size_t len, PString& md5hash )
{
	CommSSLChksumMD5 sumMd5;
	sumMd5.addData(ptr, len);
	PBlock md5Chksum;
	sumMd5.chksum(md5Chksum);

	char str_md5[33];
	for (int i = 0; (i < 16) && (i < md5Chksum.size()); i++)
	{
		sprintf(str_md5 + (i*2),"%02X",(unsigned int)*(md5Chksum.ptr() + i));
	}
	str_md5[32] = '\0';
	md5hash = str_md5;
}

/* static */ void CommSSLSystemInfo::encryptPassword( PBlock& ret, const vector< PNameValue >& sysInfo, const char* password )
{
	PString sysPwd;
	_makeSysPwd( sysPwd, sysInfo );
	CommSSLEncrypt enc( sysPwd );
	enc.addData( (const BYTE*)password, strlen( password ) + 1 );
	enc.encrypt( ret );
}

/* static */ void CommSSLSystemInfo::decryptPassword( PString& ret, const vector< PNameValue >& sysInfo, const BYTE* p, size_t sz )
{
	PString sysPwd;
	_makeSysPwd( sysPwd, sysInfo );
	CommSSLDecrypt dec( sysPwd );
	dec.addData( p, sz );
	PBlock block;
	dec.decrypt( block );
	if( block.size() == 0 || *( block.ptr() + block.size() - 1 ) != 0 )
		throw PInternalError( "CommSSLSystemInfo::decryptPassword(): decryption error" );
	ret = "";
	int l = block.size() - 1;
	for( int i=0; i < l ; ++i )
	{
		BYTE c = block.ptr()[ i ];
		if( c == 0 )
			throw PInternalError( "CommSSLSystemInfo::decryptPassword(): decryption error" );
		else
			ret.append( (char)c );
	}

}

/* static */ void CommSSLSystemInfo::encryptPasswordToString( PString& ret, const vector< PNameValue >& sysInfo, const char* password )
{
	PBlock data;
	encryptPassword( data, sysInfo, password );
	CommSSLBase64::encode( ret, data.ptr(), data.size() );
}

/* static */ void CommSSLSystemInfo::decryptPasswordFromString( PString& ret, const vector< PNameValue >& sysInfo, const char* s )
{
	PBlock data;
	CommSSLBase64::decode( data, s );
	CommSSLSystemInfo::decryptPassword( ret, sysInfo, data.ptr(), data.size() );
}

/* static */ void CommSSLSystemInfo::decodeIniFilePassword( PString& ret, const char* sectionName, const char* itemName, const char* itemValue )
{
	if( *itemValue == '@' )
	{//encrypted
		const char* end;
		int ver = strtoul( itemValue + 1, (char**)&end, 10 );
		PASSERT( *end == '@' );//sic! - PASSERT rather than PASSERT3

		vector< PNameValue > sysInfo;
		CommSSLSystemInfo si;
		int ver2 = si.getVersion();
		if( ver2 != ver )
			si.setVersion( ver );

		si.getSystemInfo( sysInfo );
		CommSSLSystemInfo::decryptPasswordFromString( ret, sysInfo, end + 1 );

		if( ver2 != ver )
		{
			vector< PNameValue > sysInfo2;
			CommSSLSystemInfo si2;
			//ver2 assumed
			si.getSystemInfo( sysInfo2 );

			PString encrypted2;
			CommSSLSystemInfo::encryptPasswordToString( encrypted2, sysInfo2, ret );

			PLog( "Warning: old-PSysInfo-version-encrypted %s; new encrypted version is:", itemName );
			PLog( "[%s]", sectionName );
			PLog( "%s=@%d@%s", itemName, ver2, encrypted2.c_str() );
		}

	}
	else//plain
	{
		ret = itemValue;

		vector< PNameValue > sysInfo;
		CommSSLSystemInfo si;
		int ver = si.getVersion();
		si.getSystemInfo( sysInfo );

		PString encrypted;
		CommSSLSystemInfo::encryptPasswordToString( encrypted, sysInfo, ret );

		PLog( "Warning: plain text %s; encrypted version is:", itemName );
		PLog( "[%s]", sectionName );
		PLog( "%s=@%d@%s", itemName, ver, encrypted.c_str() );
	}
}

/* static */ void CommSSLSystemInfo::encodePasswordEx( PString& ret, int encodingId, const char* passwrd)
{
	CommSSL::startup();
	ret.assign("@").appendInt(encodingId).append('@');
	PString salt;
	const char table[]="1234567890qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM";
	srand((unsigned int )time(0));
	for (int i=0;i<8;i++)
	{
		salt.append(table[rand()%(sizeof(table)-1)]);
	}
	ret.append(salt).append('@');
	vector< PNameValue > sysInfo;
	sysInfo.push_back( PNameValue( "SALT", salt ) );
	switch(encodingId)
	{
	case 0: // using master password only
		{
			sysInfo.push_back(PNameValue("1e436sdjkhf","URkll;hoH't9;'wne'p9pqvgb"));
			sysInfo.push_back(PNameValue("wewvp344o59","djfvo&*$)(*%)_(%^83tfi)_@(+$bpv9pavp09"));
			sysInfo.push_back(PNameValue("w4egafr","92843ka)(*&)#hbxslvwqp0bv]['"));
		}
		break;
	case 1:  //master password plus system info
		{
			sysInfo.push_back(PNameValue("alisvven36","jkgliO*^O*t087653to*&Go75"));
			sysInfo.push_back(PNameValue("ji45utguiebvds","IUt(&6R&*r987v*&T"));
			sysInfo.push_back(PNameValue("vbieieyt93","KJLHljkt*O&%wc65eJuyr"));
			CommSSLSystemInfo si;
			si.getSystemInfoEx( sysInfo );
		}
		break;
	default:
		PASSERT3(0);
	}
	PString encr;
	CommSSLSystemInfo::encryptPasswordToString( encr, sysInfo, passwrd );
	ret.append(encr);
	CommSSL::cleanup();
}
/* static */ void CommSSLSystemInfo::decodePasswordEx( PString& ret, const char* passwrd)
{
	if( *passwrd == '@' )
	{//encrypted
		CommSSL::startup();
		const char* end;
		int encodingId = strtoul( passwrd + 1, (char**)&end, 10 );
		PASSERT( *end == '@' );//sic! - PASSERT rather than PASSERT3
		end++;
		const char * pass = strchr(end,'@');
		PASSERT(pass);
		PString salt;
		salt.assign(end,pass);
		pass++;
		vector< PNameValue > sysInfo;
		sysInfo.push_back( PNameValue( "SALT", salt ) );
		switch(encodingId)
		{
		case 0: // using master password only
			{
				sysInfo.push_back(PNameValue("1e436sdjkhf","URkll;hoH't9;'wne'p9pqvgb"));
				sysInfo.push_back(PNameValue("wewvp344o59","djfvo&*$)(*%)_(%^83tfi)_@(+$bpv9pavp09"));
				sysInfo.push_back(PNameValue("w4egafr","92843ka)(*&)#hbxslvwqp0bv]['"));
			}
			break;
		case 1:  //master password plus system info
			{
				sysInfo.push_back(PNameValue("alisvven36","jkgliO*^O*t087653to*&Go75"));
				sysInfo.push_back(PNameValue("ji45utguiebvds","IUt(&6R&*r987v*&T"));
				sysInfo.push_back(PNameValue("vbieieyt93","KJLHljkt*O&%wc65eJuyr"));
				CommSSLSystemInfo si;
				si.getSystemInfoEx( sysInfo );
			}
			break;
		default:
			PASSERT3(0);
		}
		CommSSLSystemInfo::decryptPasswordFromString( ret, sysInfo, pass );
		CommSSL::cleanup();
	}
	else//plain
	{
		ret = passwrd;
	}
}

struct _CommSSLSysInfoCmp : public binary_function< const PNameValue&, const PNameValue&, bool >
{
public:
	bool operator() ( const PNameValue& a, const PNameValue& b ) const
	{
		int cmp = strcmp( a.name, b.name );
		if( cmp )
			return cmp < 0;
		return strcmp( a.value, b.value ) < 0;
	}
};

/* static */ void CommSSLSystemInfo::_makeSysPwd( PString& ret, const vector< PNameValue >& sysInfo_ )
{
	vector< PNameValue > sysInfo;
	int i, n = sysInfo_.size();
	for( i=0; i < n ; ++i )
		sysInfo.push_back( sysInfo_[ i ] );
	sort( sysInfo.begin(), sysInfo.end(), _CommSSLSysInfoCmp() );

	ret = "";
	n = sysInfo.size();
	for( i=0; i < n ; ++i )
		ret.append( sysInfo[ i ].name ).append( '=' ).append( sysInfo[ i ].value ).append( ';' );
}

bool CommSSLGetSystemInfo( vector< PNameValue >& ret )
{
	CommSSLSystemInfo info;
	return info.getSystemInfo( ret );
}
