#ifndef AccumulatorPrepStmt_h_included
#define	AccumulatorPrepStmt_h_included

////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "dbm.h"
#include "dbmstruct.h"
#include "dbmstmt.h"
#include "ColossusScheduler.h"
#include "ColossusConstants.h"
#include "AccumulatorStructs.h"

class AccumulatorDbManager;

////////////////////////////////////////////////////////////////////////////////

struct AccumTournDataCols
{
	PSqlBigInt	totalinvested;
	PSqlInt	totaltourns;
	PSqlBigInt	usertotalprofit;
	PSqlBigInt	rakepaid;
	PSqlInt	totaltournswonmoney;

	void getTournCols(AccumStatsTournData& tournData, UINT32& accumOverflowCount)
	{
		tournData.totalinvested			= totalinvested.value;
		tournData.totaltourns			= totaltourns.value;
		tournData.usertotalprofit		= usertotalprofit.value;
		tournData.rakepaid				= rakepaid.value;
		
		// totaltournswonmoney was introduced in 2015.  Old data has default value -1
		// If it's less than 0, change it as 0 for accumulation.
		if (totaltournswonmoney.value < 0)
		{
			tournData.totaltournswonmoney = 0;
		}
		else
		{
			tournData.totaltournswonmoney = totaltournswonmoney.value;
		}

		if (tournData.totalinvested > PINT32_MAX || tournData.usertotalprofit > PINT32_MAX || tournData.rakepaid > PINT32_MAX)
		{
			accumOverflowCount++;
		}
	}

	void setTournCols(const AccumStatsTournData& tournData)
	{
		totalinvested = tournData.totalinvested;
		totaltourns = tournData.totaltourns;
		usertotalprofit = tournData.usertotalprofit;
		rakepaid = tournData.rakepaid;
		totaltournswonmoney = tournData.totaltournswonmoney;
	}
};

struct AccumHandDataCols
{
	PSqlInt totalhands;
	PSqlBigInt totalwin;
	PSqlBigInt totalblindsantespaid;
	PSqlBigInt totalrakepaid;
	PSqlInt numbigblinds;
	PSqlInt numsmallblinds;
	PSqlInt numblindspaidoutofposition;
	PSqlInt numvpip;
	PSqlInt numvpipfromsmallblind;
	PSqlInt numvpipfrombigblind;
	PSqlInt numsawshowdown;
	PSqlInt numwonshowdown;
	PSqlInt numwonafterflop;
	PSqlInt numlimp;
	PSqlInt numlimpraise;
	PSqlInt numlimpcall;
	PSqlInt numlimpfold;
	PSqlInt numallinpreflop;
	PSqlInt numcoldcallopportunities;
	PSqlInt numcoldcalls;
	PSqlInt num3betopportunities;
	PSqlInt num3bets;
	PSqlInt numfacing3bet;
	PSqlInt numfoldto3bet;
	PSqlInt num4plusbet;
	PSqlInt numsawflop;
	PSqlInt numsawturn;
	PSqlInt numsawriver;
	PSqlInt numsaw7th;
	PSqlInt numfacing4plusbet;
	PSqlInt numfoldto4plusbet;
	PSqlInt numturnspreflop;
	PSqlInt numturnsfacingbetonflop;
	PSqlInt numturnsfacingnobetonflop;
	PSqlInt numturnsfacingbetonturn;
	PSqlInt numturnsfacingnobetonturn;
	PSqlInt numturnsfacingbetonriver;
	PSqlInt numturnsfacingnobetonriver;
	PSqlInt numturnsfacingbeton7th;
	PSqlInt numturnsfacingnobeton7th;
	PSqlInt numlastagressorpreflop;
	PSqlInt numlastagressoronflop;
	PSqlInt numlastagressoronturn;
	PSqlInt numlastagressoronriver;
	PSqlInt numcontbetonflop;
	PSqlInt numcontbetonturn;
	PSqlInt numcontbetonriver;
	PSqlInt numcontbeton7th;
	PSqlInt numbetpreflop;
	PSqlInt numbetonflop;
	PSqlInt numbetonturn;
	PSqlInt numbetonriver;
	PSqlInt numbeton7th;
	PSqlInt numcheckonflop;
	PSqlInt numcheckonturn;
	PSqlInt numcheckonriver;
	PSqlInt numcheckon7th;
	PSqlInt numfoldpreflop;
	PSqlInt numfoldonflop;
	PSqlInt numfoldonturn;
	PSqlInt numfoldonriver;
	PSqlInt numfoldon7th;
	PSqlInt numcallspreflop;
	PSqlInt numcallsonflop;
	PSqlInt numcallsonturn;
	PSqlInt numcallsonriver;
	PSqlInt numcallson7th;
	PSqlInt numraisespreflop;
	PSqlInt numraisesonflop;
	PSqlInt numraisesonturn;
	PSqlInt numraisesonriver;
	PSqlInt numraiseson7th;
	PSqlInt numcheckcallonflop;
	PSqlInt numcheckcallonturn;
	PSqlInt numcheckcallonriver;
	PSqlInt numcheckcallon7th;
	PSqlInt numcheckraiseonflop;
	PSqlInt numcheckraiseonturn;
	PSqlInt numcheckraiseonriver;
	PSqlInt numcheckraiseon7th;
	PSqlInt numcheckfoldonflop;
	PSqlInt numcheckfoldonturn;
	PSqlInt numcheckfoldonriver;
	PSqlInt numcheckfoldon7th;
	PSqlInt numopenfoldpowerfulhand;
	PSqlInt numfoldtoextremeodds;
	PSqlInt numreraisewithgarbage;
	PSqlInt numcheckbehindonriver;
	PSqlInt numfoldwhencouldcheck;
	PSqlInt numattemptstealblinds;
	PSqlInt numfoldtostealfromsbopps;
	PSqlInt numfoldtostealfromsb;
	PSqlInt numfoldtostealfrombbopps;
	PSqlInt numfoldtostealfrombb;
	PSqlInt numcheckraiseoppsonflop;
	PSqlInt numcheckraiseoppsonturn;
	PSqlInt numcheckraiseoppsonriver;
	PSqlInt numcheckraiseoppson7th;
	PSqlInt numattemptstealblindsopps;
	PSqlInt num4betopportunities;
	PSqlInt numhandsraisedpreflop;
	PSqlInt numcallbetsonlypreflop;
	PSqlInt numcallbetsonlyonflop;
	PSqlInt numcallbetsonlyonturn;
	PSqlInt numcallbetsonlyonriver;
	PSqlInt numcallbetsonlyon7th;
	PSqlInt numturnsfacingbetonlyonflop;
	PSqlInt numturnsfacingbetonlyonturn;
	PSqlInt numturnsfacingbetonlyonriver;
	PSqlInt numturnsfacingbetonlyon7th;
	PSqlInt numfoldtobetonlypreflop;
	PSqlInt numfoldtobetonlyonflop;
	PSqlInt numfoldtobetonlyonturn;
	PSqlInt numfoldtobetonlyonriver;
	PSqlInt numfoldtobetonlyon7th;
	PSqlInt totalplayersinhands;
	PSqlBigInt totalbet;
	PSqlInt numfacing3bet_hem;
	PSqlInt numfoldto3bet_hem;
	PSqlInt numfacing4plusbet_hem;
	PSqlInt numfoldto4plusbet_hem;
	PSqlInt numcontbetoppsvs2inposonflop;
	PSqlInt numcontbetoppsvs2inposonturn;
	PSqlInt numcontbetoppsvs2inposonriver;
	PSqlInt numcontbetoppsvs2inposon7th;
	PSqlInt numcontbetvs2inposonflop;
	PSqlInt numcontbetvs2inposonturn;
	PSqlInt numcontbetvs2inposonriver;
	PSqlInt numcontbetvs2inposon7th;

	void getHandCols(AccumStatsHandData& handData, UINT32& accumOverflowCount)
	{
		handData.totalhands = totalhands.value;
		handData.totalwin = totalwin.value;
		handData.totalblindsantespaid = totalblindsantespaid.value;
		handData.totalrakepaid = totalrakepaid.value;
		handData.numbigblinds = numbigblinds.value;
		handData.numsmallblinds = numsmallblinds.value;
		handData.numblindspaidoutofposition = numblindspaidoutofposition.value;
		handData.numvpip = numvpip.value;
		handData.numvpipfromsmallblind = numvpipfromsmallblind.value;
		handData.numvpipfrombigblind = numvpipfrombigblind.value;
		handData.numsawshowdown = numsawshowdown.value;
		handData.numwonshowdown = numwonshowdown.value;
		handData.numwonafterflop = numwonafterflop.value;
		handData.numlimp = numlimp.value;
		handData.numlimpraise = numlimpraise.value;
		handData.numlimpcall = numlimpcall.value;
		handData.numlimpfold = numlimpfold.value;
		handData.numallinpreflop = numallinpreflop.value;
		handData.numcoldcallopportunities = numcoldcallopportunities.value;
		handData.numcoldcalls = numcoldcalls.value;
		handData.num3betopportunities = num3betopportunities.value;
		handData.num3bets = num3bets.value;
		handData.numfacing3bet = numfacing3bet.value;
		handData.numfoldto3bet = numfoldto3bet.value;
		handData.num4plusbet = num4plusbet.value;
		handData.numsawflop = numsawflop.value;
		handData.numsawturn = numsawturn.value;
		handData.numsawriver = numsawriver.value;
		handData.numsaw7th = numsaw7th.value;
		handData.numfacing4plusbet = numfacing4plusbet.value;
		handData.numfoldto4plusbet = numfoldto4plusbet.value;
		handData.numturnspreflop = numturnspreflop.value;
		handData.numturnsfacingbetonflop = numturnsfacingbetonflop.value;
		handData.numturnsfacingnobetonflop = numturnsfacingnobetonflop.value;
		handData.numturnsfacingbetonturn = numturnsfacingbetonturn.value;
		handData.numturnsfacingnobetonturn = numturnsfacingnobetonturn.value;
		handData.numturnsfacingbetonriver = numturnsfacingbetonriver.value;
		handData.numturnsfacingnobetonriver = numturnsfacingnobetonriver.value;
		handData.numturnsfacingbeton7th = numturnsfacingbeton7th.value;
		handData.numturnsfacingnobeton7th = numturnsfacingnobeton7th.value;
		handData.numlastagressorpreflop = numlastagressorpreflop.value;
		handData.numlastagressoronflop = numlastagressoronflop.value;
		handData.numlastagressoronturn = numlastagressoronturn.value;
		handData.numlastagressoronriver = numlastagressoronriver.value;
		handData.numcontbetonflop = numcontbetonflop.value;
		handData.numcontbetonturn = numcontbetonturn.value;
		handData.numcontbetonriver = numcontbetonriver.value;
		handData.numcontbeton7th = numcontbeton7th.value;
		handData.numbetpreflop = numbetpreflop.value;
		handData.numbetonflop = numbetonflop.value;
		handData.numbetonturn = numbetonturn.value;
		handData.numbetonriver = numbetonriver.value;
		handData.numbeton7th = numbeton7th.value;
		handData.numcheckonflop = numcheckonflop.value;
		handData.numcheckonturn = numcheckonturn.value;
		handData.numcheckonriver = numcheckonriver.value;
		handData.numcheckon7th = numcheckon7th.value;
		handData.numfoldpreflop = numfoldpreflop.value;
		handData.numfoldonflop = numfoldonflop.value;
		handData.numfoldonturn = numfoldonturn.value;
		handData.numfoldonriver = numfoldonriver.value;
		handData.numfoldon7th = numfoldon7th.value;
		handData.numcallspreflop = numcallspreflop.value;
		handData.numcallsonflop = numcallsonflop.value;
		handData.numcallsonturn = numcallsonturn.value;
		handData.numcallsonriver = numcallsonriver.value;
		handData.numcallson7th = numcallson7th.value;
		handData.numraisespreflop = numraisespreflop.value;
		handData.numraisesonflop = numraisesonflop.value;
		handData.numraisesonturn = numraisesonturn.value;
		handData.numraisesonriver = numraisesonriver.value;
		handData.numraiseson7th = numraiseson7th.value;
		handData.numcheckcallonflop = numcheckcallonflop.value;
		handData.numcheckcallonturn = numcheckcallonturn.value;
		handData.numcheckcallonriver = numcheckcallonriver.value;
		handData.numcheckcallon7th = numcheckcallon7th.value;
		handData.numcheckraiseonflop = numcheckraiseonflop.value;
		handData.numcheckraiseonturn = numcheckraiseonturn.value;
		handData.numcheckraiseonriver = numcheckraiseonriver.value;
		handData.numcheckraiseon7th = numcheckraiseon7th.value;
		handData.numcheckfoldonflop = numcheckfoldonflop.value;
		handData.numcheckfoldonturn = numcheckfoldonturn.value;
		handData.numcheckfoldonriver = numcheckfoldonriver.value;
		handData.numcheckfoldon7th = numcheckfoldon7th.value;
		handData.numopenfoldpowerfulhand = numopenfoldpowerfulhand.value;
		handData.numfoldtoextremeodds = numfoldtoextremeodds.value;
		handData.numreraisewithgarbage = numreraisewithgarbage.value;
		handData.numcheckbehindonriver = numcheckbehindonriver.value;
		handData.numfoldwhencouldcheck = numfoldwhencouldcheck.value;
		handData.numattemptstealblinds = numattemptstealblinds.value;
		handData.numfoldtostealfromsbopps = numfoldtostealfromsbopps.value;
		handData.numfoldtostealfromsb = numfoldtostealfromsb.value;
		handData.numfoldtostealfrombbopps = numfoldtostealfrombbopps.value;
		handData.numfoldtostealfrombb = numfoldtostealfrombb.value;
		handData.numcheckraiseoppsonflop = numcheckraiseoppsonflop.value;
		handData.numcheckraiseoppsonturn = numcheckraiseoppsonturn.value;
		handData.numcheckraiseoppsonriver = numcheckraiseoppsonriver.value;
		handData.numcheckraiseoppson7th = numcheckraiseoppson7th.value;
		handData.numattemptstealblindsopps = numattemptstealblindsopps.value;
		handData.num4betopportunities = num4betopportunities.value;
		handData.numhandsraisedpreflop = numhandsraisedpreflop.value;
		handData.numcallbetsonlypreflop = numcallbetsonlypreflop.value;
		handData.numcallbetsonlyonflop = numcallbetsonlyonflop.value;
		handData.numcallbetsonlyonturn = numcallbetsonlyonturn.value;
		handData.numcallbetsonlyonriver = numcallbetsonlyonriver.value;
		handData.numcallbetsonlyon7th = numcallbetsonlyon7th.value;
		handData.numturnsfacingbetonlyonflop = numturnsfacingbetonlyonflop.value;
		handData.numturnsfacingbetonlyonturn = numturnsfacingbetonlyonturn.value;
		handData.numturnsfacingbetonlyonriver = numturnsfacingbetonlyonriver.value;
		handData.numturnsfacingbetonlyon7th = numturnsfacingbetonlyon7th.value;
		handData.numfoldtobetonlypreflop = numfoldtobetonlypreflop.value;
		handData.numfoldtobetonlyonflop = numfoldtobetonlyonflop.value;
		handData.numfoldtobetonlyonturn = numfoldtobetonlyonturn.value;
		handData.numfoldtobetonlyonriver = numfoldtobetonlyonriver.value;
		handData.numfoldtobetonlyon7th = numfoldtobetonlyon7th.value;
		handData.totalplayersinhands = totalplayersinhands.value;
		handData.totalbet = totalbet.value;
		handData.numfacing3bet_hem = numfacing3bet_hem.value;
		handData.numfoldto3bet_hem = numfoldto3bet_hem.value;
		handData.numfacing4plusbet_hem = numfacing4plusbet_hem.value;
		handData.numfoldto4plusbet_hem = numfoldto4plusbet_hem.value;
		handData.numcontbetoppsvs2inposonflop = numcontbetoppsvs2inposonflop.value;
		handData.numcontbetoppsvs2inposonturn = numcontbetoppsvs2inposonturn.value;
		handData.numcontbetoppsvs2inposonriver = numcontbetoppsvs2inposonriver.value;
		handData.numcontbetoppsvs2inposon7th = numcontbetoppsvs2inposon7th.value;
		handData.numcontbetvs2inposonflop = numcontbetvs2inposonflop.value;
		handData.numcontbetvs2inposonturn = numcontbetvs2inposonturn.value;
		handData.numcontbetvs2inposonriver = numcontbetvs2inposonriver.value;
		handData.numcontbetvs2inposon7th = numcontbetvs2inposon7th.value;

		if (handData.totalwin > PINT32_MAX || handData.totalblindsantespaid > PINT32_MAX || handData.totalrakepaid  > PINT32_MAX)
		{
			accumOverflowCount++;
		}
	}

	void setHandCols(const AccumStatsHandData& handData)
	{
		totalhands = handData.totalhands;
		totalwin = handData.totalwin;
		totalblindsantespaid = handData.totalblindsantespaid;
		totalrakepaid = handData.totalrakepaid;
		numbigblinds = handData.numbigblinds;
		numsmallblinds = handData.numsmallblinds;
		numblindspaidoutofposition = handData.numblindspaidoutofposition;
		numvpip = handData.numvpip;
		numvpipfromsmallblind = handData.numvpipfromsmallblind;
		numvpipfrombigblind = handData.numvpipfrombigblind;
		numsawshowdown = handData.numsawshowdown;
		numwonshowdown = handData.numwonshowdown;
		numwonafterflop = handData.numwonafterflop;
		numlimp = handData.numlimp;
		numlimpraise = handData.numlimpraise;
		numlimpcall = handData.numlimpcall;
		numlimpfold = handData.numlimpfold;
		numallinpreflop = handData.numallinpreflop;
		numcoldcallopportunities = handData.numcoldcallopportunities;
		numcoldcalls = handData.numcoldcalls;
		num3betopportunities = handData.num3betopportunities;
		num3bets = handData.num3bets;
		numfacing3bet = handData.numfacing3bet;
		numfoldto3bet = handData.numfoldto3bet;
		num4plusbet = handData.num4plusbet;
		numsawflop = handData.numsawflop;
		numsawturn = handData.numsawturn;
		numsawriver = handData.numsawriver;
		numsaw7th = handData.numsaw7th;
		numfacing4plusbet = handData.numfacing4plusbet;
		numfoldto4plusbet = handData.numfoldto4plusbet;
		numturnspreflop = handData.numturnspreflop;
		numturnsfacingbetonflop = handData.numturnsfacingbetonflop;
		numturnsfacingnobetonflop = handData.numturnsfacingnobetonflop;
		numturnsfacingbetonturn = handData.numturnsfacingbetonturn;
		numturnsfacingnobetonturn = handData.numturnsfacingnobetonturn;
		numturnsfacingbetonriver = handData.numturnsfacingbetonriver;
		numturnsfacingnobetonriver = handData.numturnsfacingnobetonriver;
		numturnsfacingbeton7th = handData.numturnsfacingbeton7th;
		numturnsfacingnobeton7th = handData.numturnsfacingnobeton7th;
		numlastagressorpreflop = handData.numlastagressorpreflop;
		numlastagressoronflop = handData.numlastagressoronflop;
		numlastagressoronturn = handData.numlastagressoronturn;
		numlastagressoronriver = handData.numlastagressoronriver;
		numcontbetonflop = handData.numcontbetonflop;
		numcontbetonturn = handData.numcontbetonturn;
		numcontbetonriver = handData.numcontbetonriver;
		numcontbeton7th = handData.numcontbeton7th;
		numbetpreflop = handData.numbetpreflop;
		numbetonflop = handData.numbetonflop;
		numbetonturn = handData.numbetonturn;
		numbetonriver = handData.numbetonriver;
		numbeton7th = handData.numbeton7th;
		numcheckonflop = handData.numcheckonflop;
		numcheckonturn = handData.numcheckonturn;
		numcheckonriver = handData.numcheckonriver;
		numcheckon7th = handData.numcheckon7th;
		numfoldpreflop = handData.numfoldpreflop;
		numfoldonflop = handData.numfoldonflop;
		numfoldonturn = handData.numfoldonturn;
		numfoldonriver = handData.numfoldonriver;
		numfoldon7th = handData.numfoldon7th;
		numcallspreflop = handData.numcallspreflop;
		numcallsonflop = handData.numcallsonflop;
		numcallsonturn = handData.numcallsonturn;
		numcallsonriver = handData.numcallsonriver;
		numcallson7th = handData.numcallson7th;
		numraisespreflop = handData.numraisespreflop;
		numraisesonflop = handData.numraisesonflop;
		numraisesonturn = handData.numraisesonturn;
		numraisesonriver = handData.numraisesonriver;
		numraiseson7th = handData.numraiseson7th;
		numcheckcallonflop = handData.numcheckcallonflop;
		numcheckcallonturn = handData.numcheckcallonturn;
		numcheckcallonriver = handData.numcheckcallonriver;
		numcheckcallon7th = handData.numcheckcallon7th;
		numcheckraiseonflop = handData.numcheckraiseonflop;
		numcheckraiseonturn = handData.numcheckraiseonturn;
		numcheckraiseonriver = handData.numcheckraiseonriver;
		numcheckraiseon7th = handData.numcheckraiseon7th;
		numcheckfoldonflop = handData.numcheckfoldonflop;
		numcheckfoldonturn = handData.numcheckfoldonturn;
		numcheckfoldonriver = handData.numcheckfoldonriver;
		numcheckfoldon7th = handData.numcheckfoldon7th;
		numopenfoldpowerfulhand = handData.numopenfoldpowerfulhand;
		numfoldtoextremeodds = handData.numfoldtoextremeodds;
		numreraisewithgarbage = handData.numreraisewithgarbage;
		numcheckbehindonriver = handData.numcheckbehindonriver;
		numfoldwhencouldcheck = handData.numfoldwhencouldcheck;
		numattemptstealblinds = handData.numattemptstealblinds;
		numfoldtostealfromsbopps = handData.numfoldtostealfromsbopps;
		numfoldtostealfromsb = handData.numfoldtostealfromsb;
		numfoldtostealfrombbopps = handData.numfoldtostealfrombbopps;
		numfoldtostealfrombb = handData.numfoldtostealfrombb;
		numcheckraiseoppsonflop = handData.numcheckraiseoppsonflop;
		numcheckraiseoppsonturn = handData.numcheckraiseoppsonturn;
		numcheckraiseoppsonriver = handData.numcheckraiseoppsonriver;
		numcheckraiseoppson7th = handData.numcheckraiseoppson7th;
		numattemptstealblindsopps = handData.numattemptstealblindsopps;
		num4betopportunities = handData.num4betopportunities;
		numhandsraisedpreflop = handData.numhandsraisedpreflop;
		numcallbetsonlypreflop = handData.numcallbetsonlypreflop;
		numcallbetsonlyonflop = handData.numcallbetsonlyonflop;
		numcallbetsonlyonturn = handData.numcallbetsonlyonturn;
		numcallbetsonlyonriver = handData.numcallbetsonlyonriver;
		numcallbetsonlyon7th = handData.numcallbetsonlyon7th;
		numturnsfacingbetonlyonflop = handData.numturnsfacingbetonlyonflop;
		numturnsfacingbetonlyonturn = handData.numturnsfacingbetonlyonturn;
		numturnsfacingbetonlyonriver = handData.numturnsfacingbetonlyonriver;
		numturnsfacingbetonlyon7th = handData.numturnsfacingbetonlyon7th;
		numfoldtobetonlypreflop = handData.numfoldtobetonlypreflop;
		numfoldtobetonlyonflop = handData.numfoldtobetonlyonflop;
		numfoldtobetonlyonturn = handData.numfoldtobetonlyonturn;
		numfoldtobetonlyonriver = handData.numfoldtobetonlyonriver;
		numfoldtobetonlyon7th = handData.numfoldtobetonlyon7th;
		totalplayersinhands = handData.totalplayersinhands;
		totalbet = handData.totalbet;
		numfacing3bet_hem = handData.numfacing3bet_hem;
		numfoldto3bet_hem = handData.numfoldto3bet_hem;
		numfacing4plusbet_hem = handData.numfacing4plusbet_hem;
		numfoldto4plusbet_hem = handData.numfoldto4plusbet_hem;
		numcontbetoppsvs2inposonflop = handData.numcontbetoppsvs2inposonflop;
		numcontbetoppsvs2inposonturn = handData.numcontbetoppsvs2inposonturn;
		numcontbetoppsvs2inposonriver = handData.numcontbetoppsvs2inposonriver;
		numcontbetoppsvs2inposon7th = handData.numcontbetoppsvs2inposon7th;
		numcontbetvs2inposonflop = handData.numcontbetvs2inposonflop;
		numcontbetvs2inposonturn = handData.numcontbetvs2inposonturn;
		numcontbetvs2inposonriver = handData.numcontbetvs2inposonriver;
		numcontbetvs2inposon7th = handData.numcontbetvs2inposon7th;
	}
};

// AccumHandStmtColumns should match accumHandColumnNames
struct AccumHandStmtCols : public AccumHandDataCols
{
	PSqlString<USER_NAME_LEN>	userId;
	PSqlString<CURRENCY_LEN>	currency;
	PSqlSmallInt				ringTournGame;
};

struct AccumTournStmtCols : public AccumTournDataCols
{
	PSqlString<USER_NAME_LEN>	userId;
	PSqlString<CURRENCY_LEN>	currency;
};

struct AccumStatsStmtCols
{
	AccumTournDataCols tournCols;
	AccumHandDataCols  handCols;
};

///////////////////////////////////////////////////////////////////////////////

class NoExecStmt : public DbmStatement
{
public:
	explicit NoExecStmt(DatabaseManagerCommon& man) : DbmStatement(man) {}

protected: // Disallow invocation of execute methods
	using DbmStatement::execute;
	using DbmStatement::executeRet;
	using DbmStatement::execUpdateDelete;
};

//////////////////////////////////////////////////////////////////////////////
class GetMaintJobEndDateStmt : public NoExecStmt
{
public:
	GetMaintJobEndDateStmt(DatabaseManagerCommon& man);
	void execGet(const char* tableName_, SrvDate& endDate_);

private:

	// input columns
	PSqlString<MAINTJOBSETUP_TBNAME_LEN> tableName;

	// output columns
	PSqlDate endDate;
};

class GetMaintJobStartDateStmt : public NoExecStmt
{
public:
	GetMaintJobStartDateStmt(DatabaseManagerCommon& man);
	void execGet(const char* tableName_, SrvDate& startDate_);

private:

	// input columns
	PSqlString<MAINTJOBSETUP_TBNAME_LEN> tableName;

	// output columns
	PSqlDate startDate;
};

class ReadAccumAuxStmt : public NoExecStmt
{
public:
	ReadAccumAuxStmt(DatabaseManagerCommon& man, const char* auxTableName);
	void execGet(AccumAuxMap& accumStatsAuxMap);

private:
	// output columns
	PSqlString<USER_NAME_LEN>	userId;
	PSqlDate					startDate;
	PSqlString<CURRENCY_LEN>	currency;
	PSqlSmallInt				ringTournGame;
	PSqlDate					endDate;
};

class ReadLatestAccumAuxDataStmt : public NoExecStmt
{
public:
	ReadLatestAccumAuxDataStmt(DatabaseManagerCommon& man, const char* auxTableName);
	bool execGet(const AccumAuxKey& key, AccumAuxDateRange& data);

private:
	// input columns
	PSqlString<USER_NAME_LEN>	userId;
	PSqlString<CURRENCY_LEN>	currency;
	PSqlSmallInt				ringTournGame;

	// output columns
	PSqlDate					startDate;
	PSqlDate					endDate;
};

class ReadAccumTournROIStmt : public NoExecStmt
{
public:
	ReadAccumTournROIStmt(DatabaseManagerCommon& man, const SrvDate& date, const char* tournROITalbeName);
	void execGet(AccumStatsMap& accumStatsMap, UINT32& count, UINT32& accumOverflowCount);

private:
	SrvDate				  startDate;

	// output columns
	AccumTournStmtCols    cols;
};

class ReadAccumStatsStmt : public NoExecStmt
{
public:
	ReadAccumStatsStmt(DatabaseManagerCommon& man, const char* accumStatsTableName);
	bool execGet(const AccumStatsKey& key, AccumStatsData& data, UINT32& accumOverflowCount);

private:

	// input columns
	PSqlString<USER_NAME_LEN>	userId;
	PSqlDate					startOfPeriod;
	PSqlString<CURRENCY_LEN>	currency;
	PSqlSmallInt				ringTournGame;

	// output columns
	AccumStatsStmtCols			cols;
};

//////////////////////////////////////////////////////////////////////////////

class ReadAccumHandStmt : public NoExecStmt
{
public:
	ReadAccumHandStmt(DatabaseManagerCommon& man, const SrvDate& date, const char* handStatsTableName);
	void execGet(AccumStatsMap& accumStatsMap, UINT32& count, UINT32& accumOverflowCount);

private:

	SrvDate				  startDate;

	// output columns
	AccumHandStmtCols     cols;
};

//////////////////////////////////////////////////////////////////////////////

class AccumulateDataBaseStmt : public DbmStatement
{
public:
	AccumulateDataBaseStmt(DatabaseManagerCommon& man) : DbmStatement(man)
	{
	}
	void getAccumData(AccumStatsKey& key, AccumAuxDateRange& dateRange, AccumStatsData& data, UINT32& accumOverflowCount);

protected:
	void prepareAndBind();
	PString query;

	// output columns
	PSqlString<USER_NAME_LEN>	userId;
	PSqlString<CURRENCY_LEN>	currency;
	PSqlDate					startOfPeriod;
	PSqlDate					lastStartDate;
	PSqlDate					lastEndDate;
	PSqlSmallInt				ringTournGame;

	AccumStatsStmtCols			cols;
};
 
class AccumulateDailyDataStmt : public AccumulateDataBaseStmt
{
public:
	AccumulateDailyDataStmt(DatabaseManagerCommon& man, const SrvDate& date);
};

class AccumulateWeeklyDataStmt : public AccumulateDataBaseStmt
{
public:
	AccumulateWeeklyDataStmt(DatabaseManagerCommon& man, const SrvDate& date);
};

class AccumulateUserDataStmt : public AccumulateDataBaseStmt
{
public:
	AccumulateUserDataStmt(DatabaseManagerCommon& man);
	void init(const char* userid, const SrvDate& date);

protected:

	// inputs
	PSqlString<USER_NAME_LEN>	input_userId;
	PSqlDate					input_startOfPeriod;
};

class AccumulateUserDailyDataStmt : public AccumulateUserDataStmt
{
public:
	AccumulateUserDailyDataStmt(DatabaseManagerCommon& man);
};

class AccumulateUserWeeklyDataStmt : public AccumulateUserDataStmt
{
public:
	AccumulateUserWeeklyDataStmt(DatabaseManagerCommon& man);
};

class InsertAccumStatsStmt : public AccumulateDataBaseStmt
{
public:
	InsertAccumStatsStmt(DatabaseManagerCommon& man, const char* tableName);
	void exec(const AccumStatsKey& key, const AccumStatsData& data);
};

//////////////////////////////////////////////////////////////////////////////

class DeleteAccumAuxStmt : public DbmStatement
{
public:
	DeleteAccumAuxStmt(DatabaseManagerCommon& man, const char* tableName);
	void exec(const char* userid, const SrvDate& startDate);
private:

	PSqlString<USER_NAME_LEN>	userId;
	PSqlDate					startDate;
};

class FixAccumAuxEndDateStmt : public DbmStatement
{
public:
	FixAccumAuxEndDateStmt(DatabaseManagerCommon& man, const char* tableName);
	void exec(const char* userid, const SrvDate& endDate, const SrvDate& newEndDate);
private:

	PSqlString<USER_NAME_LEN>	userId;
	PSqlDate					endDate;
	PSqlDate					updatedEndDate;
};

class UpdateAccumAuxEndDateStmt : public DbmStatement
{
public:
	UpdateAccumAuxEndDateStmt(DatabaseManagerCommon& man, const char* tableName);
	void exec(const char* userid, const SrvDate& startDate, const PString& currency, UINT16 ringTournGame, const SrvDate& endDate);
private:

	PSqlString<USER_NAME_LEN>	userId;
	PSqlDate					startDate;
	PSqlString<CURRENCY_LEN>	currency;
	PSqlSmallInt				ringTournGame;
	PSqlDate					endDate;

};

class InsertAccumAuxStmt : public DbmStatement
{
public:
	InsertAccumAuxStmt(DatabaseManagerCommon& man, const char* tableName);
	void exec(const char* userid, const SrvDate& startDate, const PString& currency, UINT16 ringTournGame, const SrvDate& endDate);
private:

	PSqlString<USER_NAME_LEN>	userId;
	PSqlDate					startDate;
	PSqlString<CURRENCY_LEN>	currency;
	PSqlSmallInt				ringTournGame;
	PSqlDate					endDate;

};

class DeleteAccumDataStmt : public DbmStatement
{
public:
	DeleteAccumDataStmt(DatabaseManagerCommon& man, const char* tableName);
	void exec(const char* userid, const SrvDate& startDate);
private:

	PSqlString<USER_NAME_LEN>	userId;
	PSqlDate					startOfPeriod;
};

#endif // AccumulatorPrepStmt_h_included
