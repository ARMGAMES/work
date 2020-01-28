#ifndef viprules_h_included
#define viprules_h_included

#include "dbm.h"

class VipRules
{
public:

	static UINT32 vipStatusWeight(UINT32 status)
	{
		return status == eVipChrome ? 50 : status * 100;
	}

	static UINT32 getPreviousStatus(UINT32 status)
	{
		switch(status)
		{
			case eVipChrome:
				return eVipBase;
			case eVipSilver:
				return eVipChrome;
			case eVipGold:
				return eVipSilver;
			case eVipPlatinum:
				return eVipGold;
			case eVipSuperNova:
				return eVipPlatinum;
			case eVipEliteSuperNova:
				return eVipSuperNova;
			default:
			{
				PLog( LOG_TRACE_FAULTY_LOGIC " getPreviousStatus - unexpected vip status %u", status );
				return eVipBase;
			}
		}
	}

	static UINT32 getNextStatus(UINT32 status)
	{
		switch(status)
		{
			case eVipBase:
				return eVipChrome;
			case eVipChrome:
				return eVipSilver;
			case eVipSilver:
				return eVipGold;
			case eVipGold:
				return eVipPlatinum;
			case eVipPlatinum:
				return eVipSuperNova;
			case eVipSuperNova:
			case eVipEliteSuperNova:
				return eVipEliteSuperNova;
			default:
			{
				PLog( LOG_TRACE_FAULTY_LOGIC " getNextStatus - unexpected vip status %u", status );
				return eVipBase;
			}
		}
	}
};

class WeightedVipStatus
{
	INT32 wStatus;
public:
	WeightedVipStatus(INT32 status_)
	{
		PASSERT5((eVipBase <= status_ && status_ <= eVipEliteSuperNova) || (status_ == eVipChrome));
		wStatus = VipRules::vipStatusWeight(status_);
	}
	bool operator <  (const WeightedVipStatus& another) const { return wStatus <  another.wStatus; }
	bool operator <= (const WeightedVipStatus& another) const { return wStatus <= another.wStatus; }
	bool operator >  (const WeightedVipStatus& another) const { return wStatus >  another.wStatus; }
	bool operator >= (const WeightedVipStatus& another) const { return wStatus >= another.wStatus; }

	static bool isStatusHigher(INT32 baseStatus, INT32 status) { return WeightedVipStatus(status) > WeightedVipStatus(baseStatus); }
};



#endif // viprules_h_included
