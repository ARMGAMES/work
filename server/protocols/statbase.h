#ifndef	statbase_h_included
#define	statbase_h_included

#include "ppinclude.h"
#include "commmsgbody.h"
#include "gamedefs.h"

//********************** class Statistics *****************************************

class Statistics
{
public:
	enum StructureType
	{
		Structure_Normal = 0
		, Structure_ButtonBlind = 1
		, Structure_GiantBlind = 2

		, Structure_Count
	};

	struct BaseStats
	{
		UINT32 vpp = 0;	  // PYR-28810, currently used in client only, *not* included in compose() and parse() function

		void addToVpp( UINT32 v )
		{
			vpp += v;
		}
	};

	struct HoldemStats : public BaseStats
	{
		UINT32 HandFlag = 0;			//Count_0
		UINT32 SB_posted = 0;			//Count_2	
		UINT32 BB_posted = 0;			//Count_1
		UINT32 Other_Pos = 0;			//Count_3
		UINT32 SeenFlop = 0;			//Count_4
		UINT32 SeenFlop_SB_pos = 0;		//Count_5
		UINT32 SeenFlop_BB_pos = 0;		//Count_6
		UINT32 SeenFlop_Other_pos = 0;	//Count_7
		UINT32 SeenShowdown = 0;		//Count_8
		UINT32 BeenWinner = 0;			//Count_9		
		UINT32 BeenWinnerNoShowdown = 0;//Count_A
		UINT32 GB_posted = 0;			//Count_B  // PYR-82834
		UINT32 SeenFlop_GB_pos = 0;		//Count_C  // PYR-82834

		void compose(CommMsgBody& body) const;
		void parse(CommMsgParser& parser);
	};

	struct StudStats : public BaseStats
	{
		UINT32 HandFlag = 0;	//Count_0
		UINT32 Street_4th = 0;	//Count_1
		UINT32 Street_5th = 0;	//Count_2
		UINT32 Street_6th = 0;	//Count_3
		UINT32 SeenShowdown = 0;//Count_4
		UINT32 BeenWinner = 0;	//Count_5
		UINT32 BeenWinnerNoShowdown = 0;//Count_6

		void compose(CommMsgBody& body) const;
		void parse(CommMsgParser& parser);
	};

	struct DrawStats : public BaseStats
	{
		/*  taken from Paradise:
			Session Games: 15
			Games won: 13%
			Showdown won: 22%
			Cards drawn: 73%
			Win % if drawing: 18%*/
		UINT32 HandFlag = 0;			//Count_0
		UINT32 CardsDrawn = 0;			//Count_1
		UINT32 BeenWinnerIfDrawn = 0;	//Count_2
		UINT32 SeenShowdown = 0;		//Count_3
		UINT32 BeenWinner = 0;			//Count_4
		UINT32 BeenWinnerNoShowdown = 0;//Count_5

		void compose(CommMsgBody& body) const;
		void parse(CommMsgParser& parser);
	};

	struct StatsCounter
	{
		map<int /*handType*/, HoldemStats*> holdemStats;
		map<int /*handType*/, StudStats*> studStats;
		map<int /*handType*/, DrawStats*> drawStats;

		void reset()
		{
			for( auto& item : holdemStats )
			{
				delete item.second;
			}
			holdemStats.clear();

			for( auto& item : studStats )
			{
				delete item.second;
			}
			studStats.clear();

			for( auto& item : drawStats )
			{
				delete item.second;
			}
			drawStats.clear();
		}

		~StatsCounter()
		{
			reset();
		}
		StatsCounter()
		{}

	private:
		void init( const StatsCounter& obj )
		{
			for( auto& item : obj.holdemStats )
			{
				PASSERT( item.second );
				holdemStats.insert( { item.first, new HoldemStats( *item.second ) } );
			}

			for( auto& item : obj.studStats )
			{
				PASSERT( item.second );
				studStats.insert( { item.first, new StudStats( *item.second ) } );
			}

			for( auto& item : obj.drawStats )
			{
				PASSERT( item.second );
				drawStats.insert( { item.first, new DrawStats( *item.second ) } );
			}
		}

	public:
		StatsCounter( const StatsCounter& obj )
		{
			init( obj );
		}

		StatsCounter& operator=( const StatsCounter& obj )
		{
			if (this != &obj)
			{
				reset();
				init( obj );
			}
			return *this;
		}
	};

	// protected:
	StatsCounter statCounters[ 2 ][ 2 ][ 3 ][ Structure_Count ]; // 2 one-to-one, 2 hilo, 3 - PM,RM,T-chips, 3 - structure (normal, button blind, giant blind)
	UINT32 _numHands;
	// These are used to give a "blank" set of stats for a game that we haven't got any stats for
	const HoldemStats blankHoldemStats;
	const StudStats blankStudStats;
	const DrawStats blankDrawStats;
	
	void addHandStatHoldem( HoldemStats& handHoldem, UINT32 flags );
	void addHandStatStud( StudStats& handStud, UINT32 flags );
	void addHandStatDraw( DrawStats& handDraw, UINT32 flags );

 public:
	Statistics()
	{
		reset();
		//memset( statCounters, 0, 2 * 3 * sizeof( StatsCounter ) );
		//_numHands = 0;
		//_isTourn = isTourn;
	}

	void reset()
	{
		for( int i = 0; i < 2; ++i )
		{
			for( int j = 0; j < 2; ++j )
			{
				for( int k = 0; k < 3; ++k )
				{
					for( int l = 0; l < Structure_Count; ++l )
					{
						statCounters[ i ][ j ][ k ][ l ].reset();
					}
				}
			}
		}
		_numHands = 0;
	}

	UINT32 getNumHandsCounted() const { return _numHands; }

	// old interface that doesn't pass in structureType. Will assume Normal unless handType is one of the following:
	// - SixPlus -> assume button blind
	// - Deepwater -> assume giant blind
	void addHandStat( BYTE handType, bool isHiLo, bool oneToOne, BYTE chipsType, UINT32 flags );
	void addHandVpp( BYTE handType, bool isHiLo, bool oneToOne, BYTE chipsType, UINT32 vpp );   // PYR-28810
	const HoldemStats* getStatHoldem( BYTE handType, bool isHiLo, bool oneToOne, BYTE chipsType ) const;
	const StudStats* getStatStud( BYTE handType, bool isHiLo, bool oneToOne, BYTE chipsType ) const;
	const DrawStats* getStatDraw( BYTE handType, bool isHiLo, bool oneToOne, BYTE chipsType ) const;

	// new interface: clients should update to these when possible
	void addHandStat( BYTE handType, bool isHiLo, bool oneToOne, BYTE chipsType, StructureType structureType, UINT32 flags );
	void addHandVpp( BYTE handType, bool isHiLo, bool oneToOne, BYTE chipsType, StructureType structureType,  UINT32 vpp );   // PYR-28810
	const HoldemStats* getStatHoldem( BYTE handType, bool isHiLo, bool oneToOne, BYTE chipsType, StructureType structureType ) const;
	const StudStats* getStatStud( BYTE handType, bool isHiLo, bool oneToOne, BYTE chipsType, StructureType structureType ) const;
	const DrawStats* getStatDraw( BYTE handType, bool isHiLo, bool oneToOne, BYTE chipsType, StructureType structureType ) const;

	void compose(CommMsgBody& body) const;
	void parse(CommMsgParser& parser);

public:
	static bool isHoldemStat( BYTE handType );
	static bool isStudStat( BYTE handType );
	static bool isDrawStat( BYTE handType );
private:
	static bool _isHoldemGame( BYTE handType );
	static bool _isOmahaGame( BYTE handType );

	BaseStats* _getStat( BYTE handType, bool isHiLo, bool oneToOne, BYTE chipsType, StructureType structureType ) const;
	HoldemStats* _getStatHoldem( BYTE handType, bool isHiLo, bool oneToOne, BYTE chipsType, StructureType structureType ) const;
	StudStats* _getStatStud( BYTE handType, bool isHiLo, bool oneToOne, BYTE chipsType, StructureType structureType ) const;
	DrawStats* _getStatDraw( BYTE handType, bool isHiLo, bool oneToOne, BYTE chipsType, StructureType structureType ) const;

	HoldemStats* _addStatHoldem( BYTE handType, bool isHiLo, bool oneToOne, BYTE chipsType, StructureType structureType );
	StudStats* _addStatStud( BYTE handType, bool isHiLo, bool oneToOne, BYTE chipsType, StructureType structureType );
	DrawStats* _addStatDraw( BYTE handType, bool isHiLo, bool oneToOne, BYTE chipsType, StructureType structureType );

	// function to assume strucvtureType based on the handtype. Can be removed once old interface is removed
	StructureType _getStructureForHandType( BYTE handType ) const;
public:
	static bool isHoldemGame( BYTE handType ) { return _isHoldemGame( handType ); }
	static bool isOmahaGame( BYTE handType ) { return _isOmahaGame( handType ); }
};

#endif
