#include "ppinclude.h"
#include "commmail.h"
#include "commurl.h"
#include "_commmime.h"
#include "commtok822.h"

void CommMail::addAddress( int type, const char* addr )
	{
	vector <PString> ad,ad_n;
	CommAddressNormalize(ad,ad_n,addr);
	for (int i=0;i<ad.size();i++)
		{
		Address& add = *addrs.insert( addrs.end(), Address() );
		add.addrType = type;
		add.addr = ad[i];
		add.addrNormalized = ad_n[i];
		}
	}
void CommMail::setReplyToAndNormalize( const char* f )
	{
	CommAddressNormalize(replyTo_.addr,replyTo_.addrNormalized,f);
	}
void CommMail::setFromAndNormalize( const char* f )
	{
	CommAddressNormalize(from_.addr,from_.addrNormalized,f);
	}
