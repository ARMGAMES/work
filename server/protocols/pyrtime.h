//pyrtime.h
#ifndef	pyrtime_h_included
#define	pyrtime_h_included

#include "commtime.h"
#define SrvTime CommSrvTime
#define SrvDate CommSrvDate
/*
struct SrvTime : public CommSrvTime
{
	bool before(const SrvTime& another) const
	{
		return (compareTo(another) < 0);
	}
	bool after(const SrvTime& another) const
	{
		return (compareTo(another) > 0);
	}
};

struct SrvDate : public CommSrvDate
{
	bool before(const SrvDate& another) const
	{
		return (compareTo(another) < 0);
	}
	bool after(const SrvDate& another) const
	{
		return (compareTo(another) > 0);
	}
};
*/
#define SrvInterval CommSrvInterval


#endif	//pyrtime_h_included
