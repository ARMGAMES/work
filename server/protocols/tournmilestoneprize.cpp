// ===========================================================
// Copyright (C) 2014 PYR Software Ltd. All rights reserved.
// ===========================================================

#include "tournmilestoneprize.h"
#include "i18n.locales.h"
#include "i18n.localutil.h"
#include "i18n.utils.h"
#include "shared.h"
#include "i18n.msg_srv.h"
#include "i18n.msg_int.h"


const char *TournMileStonePrize::toStringByLocale(UINT32 locale, const PString& tournCurrency, PString& outBuf) const
{
	PString lastItem;
	PString prevItems;
	int n = 0;

	PrizeFormatter formatter(*this);

	while(formatter.hasNext())
	{
		if(n > 0)
		{
			if(n > 1)
			{
				i18nAppend(prevItems, ", ");
			}
			i18nAppend(prevItems, lastItem);
			lastItem.cut(0);
		}
		formatter.formatNextPrizeItem(locale, tournCurrency, lastItem);
		++n;
	}

	if(n == 1)
	{
		i18nAppend(outBuf, lastItem);
	}
	else if(n > 1)
	{
		PMsg2(outBuf, LocalesTable, locale, TXTINT_LIST_Tail_and, prevItems, lastItem);
	}

	return outBuf.c_str();
}


TournMileStonePrize::PrizeFormatter::PrizeFormatter(const TournMileStonePrize& prize_)
	: prize(prize_), nextIndex(PI_NONE)
{
	gotoNextPrizeItem();
}

bool TournMileStonePrize::PrizeFormatter::hasNext() const
{
	return nextIndex < PI_LAST;
}

const char *TournMileStonePrize::PrizeFormatter::formatNextPrizeItem(UINT32 locale, const PString& currency, PString& buf)
{
	char uint64PrintBuffer[32];

	switch(nextIndex)
	{
	case PI_CASH:
		i18nFormatMoneyWithCurrency(buf, LocalesTable, locale, (UINT32)prize.cash, currency, eMoneyTypeCurr, eFormatCurrTypeSign, false, true);
		break;
	case PI_TOURNCASH:
		i18nFormatMoneyWithCurrency(buf, LocalesTable, locale, (UINT32)prize.tournCash, currency, eMoneyTypeT, eFormatCurrTypeSign, false, true);
		break;
	case PI_FPP:
		p_u64toa(prize.fpp, uint64PrintBuffer);
		PMsg1(buf, LocalesTable, locale, TXTSRV_N0_StarsCoin, uint64PrintBuffer);
		break;
	case PI_PLAYMONEY:
		p_u64toa(prize.playMoney, uint64PrintBuffer);
		PMsg1(buf, LocalesTable, locale, TXTSRV_N0_Play_Money_chips, uint64PrintBuffer);
		break;
	case PI_TICKET:
		if(prize.numTickets > 1)
		{
			i18nAppendUint(buf, prize.numTickets);
			i18nAppend(buf, " x ");
		}
		/*
		if(TicketType::isPlayMoneyTicket(prize.ticketTypeFlags))
		{
			p_u64toa(prize.ticketPrize, uint64PrintBuffer);
			PMsg1(buf, LocalesTable, locale, TXTSRV_N0_Play_Money_chips, uint64PrintBuffer);
		}
		else if(TicketType::isFppTicket(prize.ticketTypeFlags))
		{
			p_u64toa(prize.ticketPrize, uint64PrintBuffer);
			PMsg1(buf, LocalesTable, locale, TXTSRV_N0_StarsCoin, uint64PrintBuffer);
		}
		else
		{
			if(TicketType::isNoCurrencyTicket(prize.ticketTypeFlags))
			{
				i18nFormatMoney(buf, LocalesTable, locale, (UINT64)prize.ticketPrize, "", false, true);
			}
			else
			{
				i18nFormatMoneyWithCurrency(buf, LocalesTable, locale, (UINT64)prize.ticketPrize, prize.ticketCurrency, eMoneyTypeCurr, eFormatCurrTypeSign, false, true);
			}
		}
		i18nAppendChar(buf, ' ');
		*/
		PMsg1(buf, LocalesTable, locale, TXTSRV_N0_Ticket, prize.ticketName);
		break;
	default:
		PASSERT5(0);
	}

	gotoNextPrizeItem();

	return buf.c_str();
}

void TournMileStonePrize::PrizeFormatter::gotoNextPrizeItem()
{
	if(nextIndex == PI_LAST)
	{
		return;
	}

	switch(nextIndex)
	{
	case PI_NONE:
		if(prize.cash) { nextIndex = PI_CASH; break; }
	//lint -fallthrough
	case PI_CASH:			
		if(prize.tournCash) { nextIndex = PI_TOURNCASH; break; }
	//lint -fallthrough
	case PI_TOURNCASH:
		if(prize.fpp) { nextIndex = PI_FPP; break; }
	//lint -fallthrough
	case PI_FPP:
		if(prize.playMoney) { nextIndex = PI_PLAYMONEY; break; }
	//lint -fallthrough
	case PI_PLAYMONEY:
		if(prize.ticketTypeId) { nextIndex = PI_TICKET; break; }
	//lint -fallthrough
	case PI_TICKET:
		nextIndex = PI_LAST;
		break;
	default:
		PASSERT5(0);
		nextIndex = PI_LAST;
	}
}
