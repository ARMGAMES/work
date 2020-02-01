#ifndef AccumulatorStructs_h_included
#define	AccumulatorStructs_h_included

////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "pyrtime.h"
#include "../shared/AccumulatorShared.h"

#define FILE_ACCUMSTATSDAY					"accumstatsday.txt"
#define FILE_ACCUMSTATSAUXDAY_INSERT		"accumstatsauxday_insert.txt"
#define FILE_ACCUMSTATSAUXDAY_UPDATE		"accumstatsauxday_update.txt"
#define FILE_ACCUMDAY_COUNT					"accumdaycount.txt"
#define FILE_ACCUMSTATSWEEK					"accumstatsweek.txt"
#define FILE_ACCUMSTATSAUXWEEK_INSERT		"accumstatsauxweek_insert.txt"
#define FILE_ACCUMSTATSAUXWEEK_UPDATE		"accumstatsauxweek_update.txt"
#define FILE_ACCUMWEEK_COUNT				"accumweekcount.txt"

class PIOFile;

enum LoadAuxTableOption
{
	NO_PRELOAD_AUXTABLE   = 0,
	PRELOAD_AUXTABLE = 1,
};

//////////////////////////////////////////////////////////////////////////////////
struct AccumStatsKey
{
	PString userid;
	SrvDate startDate;
	PString currency;
	UINT16  ringTournGame;
	
	AccumStatsKey()
	{
		ringTournGame = 0;
	}

	AccumStatsKey(const char* userid_, const SrvDate& startDate_, const char* currency_,  UINT16 ringTournGame_)
	{
		userid = userid_;
		startDate = startDate_;
		currency = currency_;
		ringTournGame = ringTournGame_;
	}

	bool operator==(const AccumStatsKey& other) const	{ return compare(other) == 0; }
	bool operator<(const AccumStatsKey& other) const	{ return compare(other) < 0; }

private:
	int compare(const AccumStatsKey& other) const;
};

struct AccumStatsTournData
{
	INT64 totalinvested;
	UINT32 totaltourns;
	UINT64 usertotalprofit;
	UINT64 rakepaid;
	UINT32 totaltournswonmoney;

	AccumStatsTournData() { clear(); }
	void clear();
	void operator+=(const AccumStatsTournData& other);
};

struct AccumStatsHandData
{
	UINT32 totalhands; 
	INT64  totalwin; 
	UINT64 totalblindsantespaid; 
	UINT64 totalrakepaid; 
	UINT32 numbigblinds; 
	UINT32 numsmallblinds; 
	UINT32 numblindspaidoutofposition; 
	UINT32 numvpip; 
	UINT32 numvpipfromsmallblind; 
	UINT32 numvpipfrombigblind; 
	UINT32 numsawshowdown; 
	UINT32 numwonshowdown; 
	UINT32 numwonafterflop; 
	UINT32 numlimp; 
	UINT32 numlimpraise; 
	UINT32 numlimpcall; 
	UINT32 numlimpfold; 
	UINT32 numallinpreflop; 
	UINT32 numcoldcallopportunities; 
	UINT32 numcoldcalls; 
	UINT32 num3betopportunities; 
	UINT32 num3bets; 
	UINT32 numfacing3bet; 
	UINT32 numfoldto3bet; 
	UINT32 num4plusbet; 
	UINT32 numsawflop; 
	UINT32 numsawturn; 
	UINT32 numsawriver; 
	UINT32 numsaw7th; 
	UINT32 numfacing4plusbet;
	UINT32 numfoldto4plusbet; 
	UINT32 numturnspreflop; 
	UINT32 numturnsfacingbetonflop; 
	UINT32 numturnsfacingnobetonflop; 
	UINT32 numturnsfacingbetonturn; 
	UINT32 numturnsfacingnobetonturn; 
	UINT32 numturnsfacingbetonriver;
	UINT32 numturnsfacingnobetonriver;
	UINT32 numturnsfacingbeton7th;
	UINT32 numturnsfacingnobeton7th;
	UINT32 numlastagressorpreflop;
	UINT32 numlastagressoronflop;
	UINT32 numlastagressoronturn;
	UINT32 numlastagressoronriver;
	UINT32 numcontbetonflop;
	UINT32 numcontbetonturn;
	UINT32 numcontbetonriver;
	UINT32 numcontbeton7th;
	UINT32 numbetpreflop;
	UINT32 numbetonflop;
	UINT32 numbetonturn;
	UINT32 numbetonriver;
	UINT32 numbeton7th;
	UINT32 numcheckonflop; 
	UINT32 numcheckonturn; 
	UINT32 numcheckonriver; 
	UINT32 numcheckon7th; 
	UINT32 numfoldpreflop; 
	UINT32 numfoldonflop; 
	UINT32 numfoldonturn; 
	UINT32 numfoldonriver; 
	UINT32 numfoldon7th; 
	UINT32 numcallspreflop; 
	UINT32 numcallsonflop; 
	UINT32 numcallsonturn; 
	UINT32 numcallsonriver; 
	UINT32 numcallson7th; 
	UINT32 numraisespreflop; 
	UINT32 numraisesonflop; 
	UINT32 numraisesonturn; 
	UINT32 numraisesonriver; 
	UINT32 numraiseson7th; 
	UINT32 numcheckcallonflop; 
	UINT32 numcheckcallonturn; 
	UINT32 numcheckcallonriver; 
	UINT32 numcheckcallon7th; 
	UINT32 numcheckraiseonflop; 
	UINT32 numcheckraiseonturn; 
	UINT32 numcheckraiseonriver; 
	UINT32 numcheckraiseon7th; 
	UINT32 numcheckfoldonflop; 
	UINT32 numcheckfoldonturn; 
	UINT32 numcheckfoldonriver; 
	UINT32 numcheckfoldon7th; 
	UINT32 numopenfoldpowerfulhand;
	UINT32 numfoldtoextremeodds; 
	UINT32 numreraisewithgarbage; 
	UINT32 numcheckbehindonriver; 
	UINT32 numfoldwhencouldcheck; 
	UINT32 numattemptstealblinds; 
	UINT32 numfoldtostealfromsbopps; 
	UINT32 numfoldtostealfromsb; 
	UINT32 numfoldtostealfrombbopps; 
	UINT32 numfoldtostealfrombb; 
	UINT32 numcheckraiseoppsonflop; 
	UINT32 numcheckraiseoppsonturn; 
	UINT32 numcheckraiseoppsonriver; 
	UINT32 numcheckraiseoppson7th; 
	UINT32 numattemptstealblindsopps; 
	UINT32 num4betopportunities; 
	UINT32 numhandsraisedpreflop; 
	UINT32 numcallbetsonlypreflop; 
	UINT32 numcallbetsonlyonflop; 
	UINT32 numcallbetsonlyonturn; 
	UINT32 numcallbetsonlyonriver; 
	UINT32 numcallbetsonlyon7th; 
	UINT32 numturnsfacingbetonlyonflop; 
	UINT32 numturnsfacingbetonlyonturn; 
	UINT32 numturnsfacingbetonlyonriver; 
	UINT32 numturnsfacingbetonlyon7th; 
	UINT32 numfoldtobetonlypreflop; 
	UINT32 numfoldtobetonlyonflop; 
	UINT32 numfoldtobetonlyonturn;
	UINT32 numfoldtobetonlyonriver; 
	UINT32 numfoldtobetonlyon7th; 
	UINT32 totalplayersinhands; 
	UINT64 totalbet;
	UINT32 numfacing3bet_hem; 
	UINT32 numfoldto3bet_hem; 
	UINT32 numfacing4plusbet_hem; 
	UINT32 numfoldto4plusbet_hem; 
	UINT32 numcontbetoppsvs2inposonflop; 
	UINT32 numcontbetoppsvs2inposonturn; 
	UINT32 numcontbetoppsvs2inposonriver; 
	UINT32 numcontbetoppsvs2inposon7th;
	UINT32 numcontbetvs2inposonflop; 
	UINT32 numcontbetvs2inposonturn; 
	UINT32 numcontbetvs2inposonriver; 
	UINT32 numcontbetvs2inposon7th; 

	AccumStatsHandData() { clear(); }
	void clear();
	void operator+=(const AccumStatsHandData& other);
};

struct AccumStatsData
{
	AccumStatsTournData tournData;
	AccumStatsHandData  handData;

	AccumStatsData()
	{}

	void operator+=(const AccumStatsData& other)
	{
		tournData += other.tournData;
		handData += other.handData;
	}

};

typedef map<AccumStatsKey, AccumStatsData> AccumStatsMap;

//////////////////////////////////////////////////////////////////////////////////


class AccumAuxMap : public map<AccumAuxKey, AccumAuxDateRange>
{
	typedef map<AccumAuxKey, AccumAuxDateRange> Parent_;

public:
	void checkAccumStatsKey(const AccumStatsKey& accumStatsKey, bool& isStatsKeyInAux, AccumAuxDateRange& endDate);
};

#endif // AccumulatorStructs_h_included
