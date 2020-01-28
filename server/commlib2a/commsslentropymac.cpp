#include "ppinclude.h"
#include "commssl.h"

#if defined(P_MAC10X)
CommMac10XSSLEntropyCollector::CommMac10XSSLEntropyCollector( int maxBlock_ )
: CommSSLEntropyCollector( maxBlock_ )
	{
	}

void CommMac10XSSLEntropyCollector::addMouseMove( int x, int y )
	{
	int dx = x - lastx;
	int dy = y - lasty;
	if( x != lastx && y != lasty && dx != lastdx && dy != lastdy )
		{
		addEntropy( (BYTE*)&x, sizeof(x), .8 );
		addEntropy( (BYTE*)&y, sizeof(y), .8 );//1.6 bits total
		}
	lastx = x;
	lasty = y;
	lastdx = dx;
	lastdy = dy;
	}

void CommMac10XSSLEntropyCollector::addTimer()
	{
//	UINT32 t = RANDOMTIME();
//	addEntropy( (BYTE*)&t, sizeof(t), 0. );
	}
#endif

