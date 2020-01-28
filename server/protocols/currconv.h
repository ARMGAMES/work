//"currconv.h"
#ifndef	currconv_h_included
#define	currconv_h_included

class CurrencyConverter
{
	const UINT64 convRate;

public:
	CurrencyConverter( UINT64 conv_rate )
		: convRate( conv_rate )
	{
		PASSERT( convRate );
	}
	UINT32 convertToCurrency( UINT64 amount ) const
	{
		return UINT32( amount * UINT64(CONV_RATE_BASE) / convRate );
	}
	UINT32 convertFromCurrency( UINT64 amountCurrency ) const
	{
		return UINT32( amountCurrency * convRate / CONV_RATE_BASE );
	}
	UINT32 convertToCurrencyReal( UINT64 amount ) const
	{
		const UINT32 m = static_cast< UINT32 >( ( amount * UINT64(CONV_RATE_BASE) + convRate / 2 ) / convRate );
		return amount <= convertFromCurrency( m ) ? m : m + 1;
	}
	UINT64 getRate() const { return convRate; }
};
#endif	///currconv_h_included
