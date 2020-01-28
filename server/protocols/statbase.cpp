#include "statbase.h"
#include "cards.h"
#include "dbm.h"

void Statistics::addHandStatHoldem( HoldemStats& handHoldem , UINT32 flags )
{
	if( flags & HandFlag )
	{
		++handHoldem.HandFlag;

		if( flags & SB_posted )
		{
			++handHoldem.SB_posted;
		}
		else if( flags & BB_posted )
		{
			++handHoldem.BB_posted;
		}
		else if( flags & GB_posted )
		{
			++handHoldem.GB_posted;
		}
		else 
		{
			++handHoldem.Other_Pos;
		}

		if( flags & SeenFlop )
		{
			++handHoldem.SeenFlop;
			if( flags & SB_posted )
			{
				++handHoldem.SeenFlop_SB_pos;
			}
			else if( flags & BB_posted )
			{
				++handHoldem.SeenFlop_BB_pos;
			}
			else if( flags & GB_posted )
			{
				++handHoldem.SeenFlop_GB_pos;
			}
			else
			{
				++handHoldem.SeenFlop_Other_pos;
			}
		}
		if( flags & SeenShowdown )
		{
			++handHoldem.SeenShowdown;
		}

		if( flags & BeenWinner )
		{
			++handHoldem.BeenWinner;
		}

		if( flags & BeenWinnerNoShowdown )
		{
			++handHoldem.BeenWinnerNoShowdown;
		}

		++_numHands;
	}
}

void Statistics::addHandStatStud( StudStats& handStud, UINT32 flags )
{
	if( flags & HandFlag )
	{
		++handStud.HandFlag;

		if( flags & Street_4th )
		{
			++handStud.Street_4th;
		}

		if( flags & Street_5th )
		{
			++handStud.Street_5th;
		}

		if( flags & Street_6th )
		{
			++handStud.Street_6th;
		}

		if( flags & SeenShowdown )
		{
			++handStud.SeenShowdown;
		}

		if( flags & BeenWinner )
		{
			++handStud.BeenWinner;
		}

		if( flags & BeenWinnerNoShowdown )
		{
			++handStud.BeenWinnerNoShowdown;
		}

		++_numHands;
	}
}

void Statistics::addHandStatDraw( DrawStats& handDraw, UINT32 flags )
{
	if( flags & HandFlag )
	{
		++handDraw.HandFlag;

		if( flags & CardsDrawn )
		{
			++handDraw.CardsDrawn;
		}

		if( (flags & CardsDrawn) && ((flags & BeenWinner) || (flags & BeenWinnerNoShowdown)) ) //Bug #9190.1
		{
			++handDraw.BeenWinnerIfDrawn;
		}

		if( flags & SeenShowdown )
		{
			++handDraw.SeenShowdown;
		}

		if( flags & BeenWinner )
		{
			++handDraw.BeenWinner;
		}

		if( flags & BeenWinnerNoShowdown )
		{
			++handDraw.BeenWinnerNoShowdown;
		}

		++_numHands;
	}
}

Statistics::StructureType Statistics::_getStructureForHandType( BYTE handType ) const
{
	switch( handType )
	{
		case HandTypeSixPlusHoldem:
			return Structure_ButtonBlind; // button blind
		case HandTypeDeepWaterHoldem:
		case HandTypeDeepWaterOmaha:
		case HandTypeTempestHoldem:
			return Structure_GiantBlind; // giant blind
		default:
			return Structure_Normal; // normal
	}
}

void Statistics::addHandStat( BYTE handType, bool isHiLo, bool oneToOne, BYTE chipsType, UINT32 flags )
{
	addHandStat( handType, isHiLo, oneToOne, chipsType, _getStructureForHandType( handType ), flags );
}

void Statistics::addHandStat( BYTE handType, bool isHiLo, bool oneToOne, BYTE chipsType, StructureType structureType, UINT32 flags )
{
	PASSERT( chipsType <= 2 );
   	if( isHoldemStat( handType ) )
	{
		HoldemStats* stat = _getStatHoldem( handType, isHiLo, oneToOne, chipsType, structureType );
		if( stat == nullptr )
		{
			stat = _addStatHoldem( handType, isHiLo, oneToOne, chipsType, structureType );
		}
        addHandStatHoldem( *stat, flags );
	}
	else if( isStudStat( handType ) )
	{
		StudStats* stat = _getStatStud( handType, isHiLo, oneToOne, chipsType, structureType );
		if( stat == nullptr )
		{
			stat = _addStatStud( handType, isHiLo, oneToOne, chipsType, structureType );
		}
        addHandStatStud( *stat, flags );
	}
	else if( isDrawStat( handType ) )
	{
		DrawStats* stat = _getStatDraw( handType, isHiLo, oneToOne, chipsType, structureType );
		if( stat == nullptr )
		{
			stat = _addStatDraw( handType, isHiLo, oneToOne, chipsType, structureType );
		}
        addHandStatDraw( *stat, flags );
	}
    else
    {
		PLog( "Unexpected Game type in 'StatBase.cpp' game = '%d'", handType );
        PASSERT( 0 );
    }
}

const Statistics::HoldemStats* Statistics::getStatHoldem( BYTE handType, bool isHiLo, bool oneToOne, BYTE chipsType ) const
{
	return getStatHoldem( handType, isHiLo, oneToOne, chipsType, _getStructureForHandType( handType ) );
}

const Statistics::HoldemStats* Statistics::getStatHoldem( BYTE handType, bool isHiLo, bool oneToOne, BYTE chipsType, StructureType structureType ) const
{
	PASSERT( chipsType <= 2 );
	PASSERT( structureType < Structure_Count );
	
	const HoldemStats* stats = _getStatHoldem( handType, isHiLo, oneToOne, chipsType, structureType );
	return (stats == nullptr) ? &blankHoldemStats : stats;
}

const Statistics::StudStats* Statistics::getStatStud( BYTE handType, bool isHiLo, bool oneToOne, BYTE chipsType ) const
{
	return getStatStud( handType, isHiLo, oneToOne, chipsType, _getStructureForHandType( handType ) );
}

const Statistics::StudStats* Statistics::getStatStud( BYTE handType, bool isHiLo, bool oneToOne, BYTE chipsType, StructureType structureType ) const
{
	PASSERT( chipsType <= 2 );
	PASSERT( structureType < Structure_Count );

	const StudStats* stats = _getStatStud( handType, isHiLo, oneToOne, chipsType, structureType );
	return (stats == nullptr) ? &blankStudStats : stats;
}

const Statistics::DrawStats* Statistics::getStatDraw( BYTE handType, bool isHiLo, bool oneToOne, BYTE chipsType ) const
{
	return getStatDraw( handType, isHiLo, oneToOne, chipsType, _getStructureForHandType( handType ) );
}

const Statistics::DrawStats* Statistics::getStatDraw( BYTE handType, bool isHiLo, bool oneToOne, BYTE chipsType, StructureType structureType ) const
{
	PASSERT( chipsType <= 2 );
	PASSERT( structureType < Structure_Count );

	const DrawStats* stats = _getStatDraw( handType, isHiLo, oneToOne, chipsType, structureType );
	return (stats == nullptr) ? &blankDrawStats : stats;
}

void Statistics::addHandVpp( BYTE handType, bool isHiLo, bool oneToOne, BYTE chipsType, UINT32 vpp )
{
	addHandVpp( handType, isHiLo, oneToOne, chipsType, _getStructureForHandType( handType ), vpp );
}

void Statistics::addHandVpp( BYTE handType, bool isHiLo, bool oneToOne, BYTE chipsType, StructureType structureType, UINT32 vpp )
{
	Statistics::BaseStats* stats = _getStat( handType, isHiLo, oneToOne, chipsType, structureType );
	if ( stats )
	{
		stats->vpp += vpp;
	}
	else
	{
		PLog( "Unexpected error, addHandVpp is called before calling addHandStat, game = '%d'", handType );
	} 
}

// holdem and omaha are similar enough games to share the same stats structure
bool Statistics::isHoldemStat( BYTE handType )
{
	return _isHoldemGame(handType) || _isOmahaGame(handType);
}

bool Statistics::isStudStat( BYTE handType )
{
	switch( handType )
	{
		case HandTypeStud:
		case HandTypeRazz:
			return true;
	}
	return false;
}

bool Statistics::isDrawStat( BYTE handType )
{
	switch( handType )
	{
		case HandTypeDraw:
		case HandType3Draw27:
		case HandTypeSingleDraw27:
		case HandTypeBadugi :
		case HandType3DrawA5:
			return true;
	}
	return false;
}

bool Statistics::_isHoldemGame(BYTE handType)
{
	switch( handType )
	{
		case HandTypeHoldem:
		case HandTypeSplitHoldem:
		case HandTypeShowtimeHoldem:
		case HandTypeUnfoldClassic:
		case HandTypeUnfoldMax:
		case HandTypeSixPlusHoldem:
		case HandTypeDeepWaterHoldem:
		case HandTypeSwapHoldem:
		case HandTypeTempestHoldem:
			return true;
	}
	return false;
}

bool Statistics::_isOmahaGame(BYTE handType)
{
	switch( handType )
	{
		case HandTypeOmaha:
		case HandType5Omaha:
		case HandType6Omaha:
		case HandTypeCourchevel:
		case HandTypeSplitOmaha:
		case HandTypeShowtimeOmaha:
		case HandTypeFusion:
		case HandTypeDeepWaterOmaha:
			return true;
	}
	return false;
}

Statistics::BaseStats* Statistics::_getStat( BYTE handType, bool isHiLo, bool oneToOne, BYTE chipsType, StructureType structureType ) const
{
	Statistics::BaseStats* stats = nullptr;
	if( isHoldemStat( handType ) )
		stats = _getStatHoldem( handType, isHiLo, oneToOne, chipsType, structureType );
	else if( isStudStat( handType ) )
		stats = _getStatStud( handType, isHiLo, oneToOne, chipsType, structureType );
	else if( isDrawStat( handType ) )
		stats = _getStatDraw( handType, isHiLo, oneToOne, chipsType, structureType );

	// else
	// don't assert since we might add a new game that doesn't have a mapping but we don't want this to break
	return stats;
}

Statistics::HoldemStats* Statistics::_getStatHoldem( BYTE handType, bool isHiLo, bool oneToOne, BYTE chipsType, StructureType structureType ) const
{
	auto& statMap = statCounters[ oneToOne ? 1 : 0 ][ isHiLo ? 1 : 0 ][ chipsType ][ structureType ].holdemStats;
	auto item = statMap.find( handType );
	if( item != statMap.end() )
	{
		return item->second;
	}

	return nullptr;
}

Statistics::StudStats* Statistics::_getStatStud( BYTE handType, bool isHiLo, bool oneToOne, BYTE chipsType, StructureType structureType ) const
{
	auto& statMap = statCounters[ oneToOne ? 1 : 0 ][ isHiLo ? 1 : 0 ][ chipsType ][ structureType ].studStats;
	auto item = statMap.find( handType );
	if( item != statMap.end() )
	{
		return item->second;
	}

	return nullptr;
}

Statistics::DrawStats* Statistics::_getStatDraw( BYTE handType, bool isHiLo, bool oneToOne, BYTE chipsType, StructureType structureType ) const
{
	auto& statMap = statCounters[ oneToOne ? 1 : 0 ][ isHiLo ? 1 : 0 ][ chipsType ][ structureType ].drawStats;
	auto item = statMap.find( handType );
	if( item != statMap.end() )
	{
		return item->second;
	}

	return nullptr;
}

Statistics::HoldemStats* Statistics::_addStatHoldem( BYTE handType, bool isHiLo, bool oneToOne, BYTE chipsType, StructureType structureType )
{
	auto& statMap = statCounters[ oneToOne ? 1 : 0 ][ isHiLo ? 1 : 0 ][ chipsType ][ structureType ].holdemStats;
	PASSERT( statMap.find( handType ) == statMap.cend() );
	
	statMap[ handType ] = new HoldemStats();

	return statMap[ handType ];
}

Statistics::StudStats* Statistics::_addStatStud( BYTE handType, bool isHiLo, bool oneToOne, BYTE chipsType, StructureType structureType )
{
	auto& statMap = statCounters[ oneToOne ? 1 : 0 ][ isHiLo ? 1 : 0 ][ chipsType ][ structureType ].studStats;
	PASSERT( statMap.find( handType ) == statMap.cend() );

	statMap[ handType ] = new StudStats();

	return statMap[ handType ];
}

Statistics::DrawStats* Statistics::_addStatDraw( BYTE handType, bool isHiLo, bool oneToOne, BYTE chipsType, StructureType structureType )
{
	auto& statMap = statCounters[ oneToOne ? 1 : 0 ][ isHiLo ? 1 : 0 ][ chipsType ][ structureType ].drawStats;
	PASSERT( statMap.find( handType ) == statMap.cend() );

	statMap[ handType ] = new DrawStats();

	return statMap[ handType ];
}
