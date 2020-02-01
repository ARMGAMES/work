////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "AccumulatorStructs.h"
#include "ColossusConstants.h"
#include "ppiofile.h"
#include "shared.h"

////////////////////////////////////////////////////////////////////////////////


void AccumAuxMap::checkAccumStatsKey(const AccumStatsKey& accumStatsKey, bool& isStatsKeyInAux,  AccumAuxDateRange& endDate)
{
	isStatsKeyInAux = false;

	AccumAuxKey key(accumStatsKey.userid, accumStatsKey.currency, accumStatsKey.ringTournGame);
	Parent_::const_iterator cit = find(key);

	if (cit == Parent_::end())
	{
		return;
	}
	
	isStatsKeyInAux = true;
	endDate = cit->second;
}

///////////////////////////////////////////////////////////////////////////////

int AccumStatsKey::compare(const AccumStatsKey& other) const
{
	int res = PString::compare(userid, other.userid);
	if (res)
		return res;

	res = startDate.compareTo(other.startDate);
	if (res)
		return res;

	res = PString::compare(currency, other.currency);
	if (res)
		return res;

	res = int(ringTournGame) - int(other.ringTournGame);
	if (res)
		return res;

	return 0;
}

void AccumStatsTournData::clear()
{
	totalinvested = 0;
	totaltourns = 0;
	usertotalprofit = 0;
	rakepaid = 0;
	totaltournswonmoney = 0;
}

void AccumStatsTournData::operator+=(const AccumStatsTournData& other)
{
	totalinvested += other.totalinvested;
	totaltourns += other.totaltourns;
	usertotalprofit += other.usertotalprofit;
	rakepaid += other.rakepaid;
	totaltournswonmoney += other.totaltournswonmoney;
}

void AccumStatsHandData::clear()
{
	totalhands = 0;
	totalwin = 0;
	totalblindsantespaid = 0;
	totalrakepaid = 0;
	numbigblinds = 0;
	numsmallblinds = 0;
	numblindspaidoutofposition = 0;
	numvpip = 0;
	numvpipfromsmallblind = 0;
	numvpipfrombigblind = 0;
	numsawshowdown = 0;
	numwonshowdown = 0;
	numwonafterflop = 0;
	numlimp = 0;
	numlimpraise = 0;
	numlimpcall = 0;
	numlimpfold = 0;
	numallinpreflop = 0;
	numcoldcallopportunities = 0;
	numcoldcalls = 0;
	num3betopportunities = 0;
	num3bets = 0;
	numfacing3bet = 0;
	numfoldto3bet = 0;
	num4plusbet = 0;
	numsawflop = 0;
	numsawturn = 0;
	numsawriver = 0;
	numsaw7th = 0;
	numfacing4plusbet = 0;
	numfoldto4plusbet = 0;
	numturnspreflop = 0;
	numturnsfacingbetonflop = 0;
	numturnsfacingnobetonflop = 0;
	numturnsfacingbetonturn = 0;
	numturnsfacingnobetonturn = 0;
	numturnsfacingbetonriver = 0;
	numturnsfacingnobetonriver = 0;
	numturnsfacingbeton7th = 0;
	numturnsfacingnobeton7th = 0;
	numlastagressorpreflop = 0;
	numlastagressoronflop = 0;
	numlastagressoronturn = 0;
	numlastagressoronriver = 0;
	numcontbetonflop = 0;
	numcontbetonturn = 0;
	numcontbetonriver = 0;
	numcontbeton7th = 0;
	numbetpreflop = 0;
	numbetonflop = 0;
	numbetonturn = 0;
	numbetonriver = 0;
	numbeton7th = 0;
	numcheckonflop = 0;
	numcheckonturn = 0;
	numcheckonriver = 0;
	numcheckon7th = 0;
	numfoldpreflop = 0;
	numfoldonflop = 0;
	numfoldonturn = 0;
	numfoldonriver = 0;
	numfoldon7th = 0;
	numcallspreflop = 0;
	numcallsonflop = 0;
	numcallsonturn = 0;
	numcallsonriver = 0;
	numcallson7th = 0;
	numraisespreflop = 0;
	numraisesonflop = 0;
	numraisesonturn = 0;
	numraisesonriver = 0;
	numraiseson7th = 0;
	numcheckcallonflop = 0;
	numcheckcallonturn = 0;
	numcheckcallonriver = 0;
	numcheckcallon7th = 0;
	numcheckraiseonflop = 0;
	numcheckraiseonturn = 0;
	numcheckraiseonriver = 0;
	numcheckraiseon7th = 0;
	numcheckfoldonflop = 0;
	numcheckfoldonturn = 0;
	numcheckfoldonriver = 0;
	numcheckfoldon7th = 0;
	numopenfoldpowerfulhand = 0;
	numfoldtoextremeodds = 0;
	numreraisewithgarbage = 0;
	numcheckbehindonriver = 0;
	numfoldwhencouldcheck = 0;
	numattemptstealblinds = 0;
	numfoldtostealfromsbopps = 0;
	numfoldtostealfromsb = 0;
	numfoldtostealfrombbopps = 0;
	numfoldtostealfrombb = 0;
	numcheckraiseoppsonflop = 0;
	numcheckraiseoppsonturn = 0;
	numcheckraiseoppsonriver = 0;
	numcheckraiseoppson7th = 0;
	numattemptstealblindsopps = 0;
	num4betopportunities = 0;
	numhandsraisedpreflop = 0;
	numcallbetsonlypreflop = 0;
	numcallbetsonlyonflop = 0;
	numcallbetsonlyonturn = 0;
	numcallbetsonlyonriver = 0;
	numcallbetsonlyon7th = 0;
	numturnsfacingbetonlyonflop = 0;
	numturnsfacingbetonlyonturn = 0;
	numturnsfacingbetonlyonriver = 0;
	numturnsfacingbetonlyon7th = 0;
	numfoldtobetonlypreflop = 0;
	numfoldtobetonlyonflop = 0;
	numfoldtobetonlyonturn = 0;
	numfoldtobetonlyonriver = 0;
	numfoldtobetonlyon7th = 0;
	totalplayersinhands = 0;
	totalbet = 0;
	numfacing3bet_hem = 0;
	numfoldto3bet_hem = 0;
	numfacing4plusbet_hem = 0;
	numfoldto4plusbet_hem = 0;
	numcontbetoppsvs2inposonflop = 0;
	numcontbetoppsvs2inposonturn = 0;
	numcontbetoppsvs2inposonriver = 0;
	numcontbetoppsvs2inposon7th = 0;
	numcontbetvs2inposonflop = 0;
	numcontbetvs2inposonturn = 0;
	numcontbetvs2inposonriver = 0;
	numcontbetvs2inposon7th = 0;
}

void AccumStatsHandData::operator+=(const AccumStatsHandData& other)
{
	totalhands += other.totalhands;
	totalwin += other.totalwin;
	totalblindsantespaid += other.totalblindsantespaid;
	totalrakepaid += other.totalrakepaid;
	numbigblinds += other.numbigblinds;
	numsmallblinds += other.numsmallblinds;
	numblindspaidoutofposition += other.numblindspaidoutofposition;
	numvpip += other.numvpip;
	numvpipfromsmallblind += other.numvpipfromsmallblind;
	numvpipfrombigblind += other.numvpipfrombigblind;
	numsawshowdown += other.numsawshowdown;
	numwonshowdown += other.numwonshowdown;
	numwonafterflop += other.numwonafterflop;
	numlimp += other.numlimp;
	numlimpraise += other.numlimpraise;
	numlimpcall += other.numlimpcall;
	numlimpfold += other.numlimpfold;
	numallinpreflop += other.numallinpreflop;
	numcoldcallopportunities += other.numcoldcallopportunities;
	numcoldcalls += other.numcoldcalls;
	num3betopportunities += other.num3betopportunities;
	num3bets += other.num3bets;
	numfacing3bet += other.numfacing3bet;
	numfoldto3bet += other.numfoldto3bet;
	num4plusbet += other.num4plusbet;
	numsawflop += other.numsawflop;
	numsawturn += other.numsawturn;
	numsawriver += other.numsawriver;
	numsaw7th += other.numsaw7th;
	numfacing4plusbet += other.numfacing4plusbet;
	numfoldto4plusbet += other.numfoldto4plusbet;
	numturnspreflop += other.numturnspreflop;
	numturnsfacingbetonflop += other.numturnsfacingbetonflop;
	numturnsfacingnobetonflop += other.numturnsfacingnobetonflop;
	numturnsfacingbetonturn += other.numturnsfacingbetonturn;
	numturnsfacingnobetonturn += other.numturnsfacingnobetonturn;
	numturnsfacingbetonriver += other.numturnsfacingbetonriver;
	numturnsfacingnobetonriver += other.numturnsfacingnobetonriver;
	numturnsfacingbeton7th += other.numturnsfacingbeton7th;
	numturnsfacingnobeton7th += other.numturnsfacingnobeton7th;
	numlastagressorpreflop += other.numlastagressorpreflop;
	numlastagressoronflop += other.numlastagressoronflop;
	numlastagressoronturn += other.numlastagressoronturn;
	numlastagressoronriver += other.numlastagressoronriver;
	numcontbetonflop += other.numcontbetonflop;
	numcontbetonturn += other.numcontbetonturn;
	numcontbetonriver += other.numcontbetonriver;
	numcontbeton7th += other.numcontbeton7th;
	numbetpreflop += other.numbetpreflop;
	numbetonflop += other.numbetonflop;
	numbetonturn += other.numbetonturn;
	numbetonriver += other.numbetonriver;
	numbeton7th += other.numbeton7th;
	numcheckonflop += other.numcheckonflop;
	numcheckonturn += other.numcheckonturn;
	numcheckonriver += other.numcheckonriver;
	numcheckon7th += other.numcheckon7th;
	numfoldpreflop += other.numfoldpreflop;
	numfoldonflop += other.numfoldonflop;
	numfoldonturn += other.numfoldonturn;
	numfoldonriver += other.numfoldonriver;
	numfoldon7th += other.numfoldon7th;
	numcallspreflop += other.numcallspreflop;
	numcallsonflop += other.numcallsonflop;
	numcallsonturn += other.numcallsonturn;
	numcallsonriver += other.numcallsonriver;
	numcallson7th += other.numcallson7th;
	numraisespreflop += other.numraisespreflop;
	numraisesonflop += other.numraisesonflop;
	numraisesonturn += other.numraisesonturn;
	numraisesonriver += other.numraisesonriver;
	numraiseson7th += other.numraiseson7th;
	numcheckcallonflop += other.numcheckcallonflop;
	numcheckcallonturn += other.numcheckcallonturn;
	numcheckcallonriver += other.numcheckcallonriver;
	numcheckcallon7th += other.numcheckcallon7th;
	numcheckraiseonflop += other.numcheckraiseonflop;
	numcheckraiseonturn += other.numcheckraiseonturn;
	numcheckraiseonriver += other.numcheckraiseonriver;
	numcheckraiseon7th += other.numcheckraiseon7th;
	numcheckfoldonflop += other.numcheckfoldonflop;
	numcheckfoldonturn += other.numcheckfoldonturn;
	numcheckfoldonriver += other.numcheckfoldonriver;
	numcheckfoldon7th += other.numcheckfoldon7th;
	numopenfoldpowerfulhand += other.numopenfoldpowerfulhand;
	numfoldtoextremeodds += other.numfoldtoextremeodds;
	numreraisewithgarbage += other.numreraisewithgarbage;
	numcheckbehindonriver += other.numcheckbehindonriver;
	numfoldwhencouldcheck += other.numfoldwhencouldcheck;
	numattemptstealblinds += other.numattemptstealblinds;
	numfoldtostealfromsbopps += other.numfoldtostealfromsbopps;
	numfoldtostealfromsb += other.numfoldtostealfromsb;
	numfoldtostealfrombbopps += other.numfoldtostealfrombbopps;
	numfoldtostealfrombb += other.numfoldtostealfrombb;
	numcheckraiseoppsonflop += other.numcheckraiseoppsonflop;
	numcheckraiseoppsonturn += other.numcheckraiseoppsonturn;
	numcheckraiseoppsonriver += other.numcheckraiseoppsonriver;
	numcheckraiseoppson7th += other.numcheckraiseoppson7th;
	numattemptstealblindsopps += other.numattemptstealblindsopps;
	num4betopportunities += other.num4betopportunities;
	numhandsraisedpreflop += other.numhandsraisedpreflop;
	numcallbetsonlypreflop += other.numcallbetsonlypreflop;
	numcallbetsonlyonflop += other.numcallbetsonlyonflop;
	numcallbetsonlyonturn += other.numcallbetsonlyonturn;
	numcallbetsonlyonriver += other.numcallbetsonlyonriver;
	numcallbetsonlyon7th += other.numcallbetsonlyon7th;
	numturnsfacingbetonlyonflop += other.numturnsfacingbetonlyonflop;
	numturnsfacingbetonlyonturn += other.numturnsfacingbetonlyonturn;
	numturnsfacingbetonlyonriver += other.numturnsfacingbetonlyonriver;
	numturnsfacingbetonlyon7th += other.numturnsfacingbetonlyon7th;
	numfoldtobetonlypreflop += other.numfoldtobetonlypreflop;
	numfoldtobetonlyonflop += other.numfoldtobetonlyonflop;
	numfoldtobetonlyonturn += other.numfoldtobetonlyonturn;
	numfoldtobetonlyonriver += other.numfoldtobetonlyonriver;
	numfoldtobetonlyon7th += other.numfoldtobetonlyon7th;
	totalplayersinhands += other.totalplayersinhands;
	totalbet += other.totalbet;
	numfacing3bet_hem += other.numfacing3bet_hem;
	numfoldto3bet_hem += other.numfoldto3bet_hem;
	numfacing4plusbet_hem += other.numfacing4plusbet_hem;
	numfoldto4plusbet_hem += other.numfoldto4plusbet_hem;
	numcontbetoppsvs2inposonflop += other.numcontbetoppsvs2inposonflop;
	numcontbetoppsvs2inposonturn += other.numcontbetoppsvs2inposonturn;
	numcontbetoppsvs2inposonriver += other.numcontbetoppsvs2inposonriver;
	numcontbetoppsvs2inposon7th += other.numcontbetoppsvs2inposon7th;
	numcontbetvs2inposonflop += other.numcontbetvs2inposonflop;
	numcontbetvs2inposonturn += other.numcontbetvs2inposonturn;
	numcontbetvs2inposonriver += other.numcontbetvs2inposonriver;
	numcontbetvs2inposon7th += other.numcontbetvs2inposon7th;
}