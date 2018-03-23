
#include "singleton.h"

Singleton* Singleton::singleton = 0;

Singleton* Singleton::Instance()
{
	if (singleton == NULL)
	{
		singleton = new Singleton();
	}
	return singleton;
}