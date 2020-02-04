// ===========================================================
// Copyright (C) 2018 PYR Software Ltd. All rights reserved.
// ===========================================================

#include "ColCda.h"
#include "ColCdaObject.h"
#include "coldbm.h"
#include "composecolls.h"

void ColCdaServerObject::processColUtilMessage(UINT32 msgId, const CommMsgBody& body, CommServerConnection* conn)
{
	logServerMsg("CU", msgId, conn, "");

	CommMsgBody coldbmBody;
	coldbmBody.copyFromSkipReqId(body);

	CommMsgParser parser(body);
	UINT32 reqId = 0;
	parser.parseUINT32(reqId);

	switch(msgId)
	{
	case COLCDA_CREATE_RULE:
		{
			postToColDbmWriter(COL_Q_CCDA_CREATE_RULE, coldbmBody, new ColUtilSrvCall(this, conn->id(), reqId));
			return;
		}
	case COLCDA_MODIFY_RULE:
		{
			postToColDbmWriter(COL_Q_CCDA_MODIFY_RULE, coldbmBody, new ColUtilSrvCall(this, conn->id(), reqId));
			return;
		}
	case COLCDA_ACTIVATE_RULE:
		{
			postToColDbmWriter(COL_Q_CCDA_ACTIVATE_RULE, coldbmBody, new ColUtilSrvCall(this, conn->id(), reqId));
			return;
		}
	case COLCDA_REORDER_RULES:
		{
			postToColDbmWriter(COL_Q_CCDA_REORDER_RULE, coldbmBody, new ColUtilSrvCall(this, conn->id(), reqId));
			return;
		}
	case COLCDA_DELETE_RULE:
		{
			postToColDbmWriter(COL_Q_CCDA_DELETE_RULE, coldbmBody, new ColUtilSrvCall(this, conn->id(), reqId));
			return;
		}
	case COLCDA_READ_RULE:
		{
			postToColDbm(COL_Q_CCDA_READ_RULE, coldbmBody, new ColUtilSrvCall(this, conn->id(), reqId));
			return;
		}
	case COLCDA_READ_CURRENT_RULE_LIST:
		{
			postToColDbm(COL_Q_CCDA_READ_CURRENT_RULE_LIST, coldbmBody, new ColUtilSrvCall(this, conn->id(), reqId));
			return;
		}
	case COLCDA_READ_FULL_RULE_LIST:
		{
			postToColDbm(COL_Q_CCDA_READ_FULL_RULE_LIST, coldbmBody, new ColUtilSrvCall(this, conn->id(), reqId));
			return;
		}
	case COLCDA_GET_RULE_HISTORY:
		{
			postToColDbm(COL_Q_CCDA_GET_RULE_HISTORY, coldbmBody, new ColUtilSrvCall(this, conn->id(), reqId));
			return;
		}
	case COLWHITELIST_CREATE_RECORD:
		{
			processColWhitelistCreate(parser);
			break;
		}
	case COLWHITELIST_MODIFY_RECORD:
		{
			processColWhitelistModify(parser);
			break;
		}
	case COLWHITELIST_DELETE_RECORD:
		{
			processColWhitelistDelete(parser);
			break;
		}
	case COLCDA_GET_VELOCITY_CONTROL:
		{
			postToColDbm(COL_Q_CCDA_GET_VELOCITY_CONTROL_RULE, coldbmBody, new ColUtilSrvCall(this, conn->id(), reqId));
			return;
		}
	case COLCDA_SET_VELOCITY_CONTROL:
		{
			postToColDbmWriter(COL_Q_CCDA_SET_VELOCITY_CONTROL_RULE, coldbmBody, new ColUtilSrvCall(this, conn->id(), reqId));
			return;
		}
	case COLCDA_GET_EVALUATION_STATUS:
		{
			processGetEvaluationStatus(reqId, *conn);
			return;
		}
	case COLCDA_START_EVALUATION:
		{
			processStartEvaluation(reqId, *conn);
			return;
		}
	default:
		{
			PLog("Error: Unhandled ColUtil message #%u", msgId);
			return; 
		}
	}

	UINT32 replyId = msgId + 1;	// By local convention
	CommMsgBody reply;
	setReply(reqId, DBM_NO_ERROR, "", reply);
	postReply(conn, replyId, reply);
}

void ColCdaServerObject::processColWhitelistCreate(CommMsgParser& parser)
{
	PLog("processColWhitelistCreate");
	ColWhitelist whitelistRecord;
	whitelistRecord.parse(parser);

	colWhitelistCache.addWhitelist(whitelistRecord, ColWhitelistModule_CCDA);
}

void ColCdaServerObject::processColWhitelistModify(CommMsgParser& parser)
{
	PLog("processColWhitelistModify");
	ColWhitelist whitelistRecord;
	whitelistRecord.parse(parser);

	colWhitelistCache.modifyWhitelist(whitelistRecord, ColWhitelistModule_CCDA);
}

void ColCdaServerObject::processColWhitelistDelete(CommMsgParser& parser)
{
	PLog("processColWhitelistDelete");
	ColWhitelist whitelistRecord;
	whitelistRecord.parse(parser);

	colWhitelistCache.deleteWhitelist(whitelistRecord, ColWhitelistModule_CCDA);
}

void ColCdaServerObject::processGetEvaluationStatus(UINT32 reqId, CommServerConnection& conn)
{
	PLog("processGetEvaluationStatus");

	CommMsgBody body;
	body
		.composeUINT32(reqId)
		.composeINT16(DBM_NO_ERROR)
		.composeUINT32(evaluationStatus);

	postReply(&conn, COLCDA_GET_EVALUATION_STATUS_REPLY, body);
}

void ColCdaServerObject::processStartEvaluation(UINT32 reqId, CommServerConnection& conn)
{
	PLog("processStartEvaluation");

	CommMsgBody body;
	body.composeUINT32(reqId);

	if (evaluationStatus == CCDA_EVAL_STATUS_RUNNING)
	{
		body
			.composeINT16(1)
			.composeString("CCDA is already running.");
	}
	else
	{
		evaluationStatus = CCDA_EVAL_STATUS_RUNNING;
		body
			.composeINT16(DBM_NO_ERROR)
			.composeUINT32(evaluationStatus);
		startEvaluation();
	}
	postReply(&conn, COLCDA_START_EVALUATION_REPLY, body);
}

void ColCdaServerObject::gotGetVelocityControlReply(CommMsgParser& parser, UINT32 connId, UINT32 reqId)
{
	PLog("gotGetVelocityControlReply");

	CommMsgBody body;

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	body.composeUINT32(reqId).composeINT16(errCode);

	if (errCode)
	{
		const char* errStr;
		parser.parseString(errStr);
		PLog("DBM error[%d]-%s", errCode, errStr);
		body.composeString(errStr);
	}
	else
	{
		CCDARule ccdaRule;
		ccdaRule.parse(parser);
		ccdaRule.logVelocityControl();

		ccdaRule.compose(body);

		if (connId == 0)
		{
			// local server call to load velocity control rule
			velocityControlRule = ccdaRule;
			PLog("Load velocity control rule=%u.", velocityControlRule.ruleId);
		}
	}

	if (connId != 0)
	{
		replyMsg(connId, COLCDA_GET_VELOCITY_CONTROL_REPLY, body);
	}
}

void ColCdaServerObject::gotSetVelocityControlReply(CommMsgParser& parser, UINT32 connId, UINT32 reqId)
{
	PLog("gotSetVelocityControlReply");

	CommMsgBody body;

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	body.composeUINT32(reqId).composeINT16(errCode);

	if (errCode)
	{
		const char* errStr;
		parser.parseString(errStr);
		PLog("DBM error[%d]-%s", errCode, errStr);
		body.composeString(errStr);
	}
	else
	{
		CCDARule ccdaRule;
		ccdaRule.parse(parser);
		ccdaRule.logVelocityControl();
		ccdaRule.compose(body);

		velocityControlRule = ccdaRule;
	}

	replyMsg(connId, COLCDA_SET_VELOCITY_CONTROL_REPLY, body);
}

void ColCdaServerObject::gotCreateRuleReply(CommMsgParser& parser, UINT32 connId, UINT32 reqId)
{
	PLog("gotCreateRuleReply");

	CommMsgBody body;

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	body.composeUINT32(reqId).composeINT16(errCode);

	if (errCode)
	{
		const char* errStr;
		parser.parseString(errStr);
		PLog("DBM error[%d]-%s", errCode, errStr);

		body.composeString(errStr);
	}
	else
	{
		CCDARule ccdaRule;
		ccdaRule.parse(parser);
		ccdaRule.compose(body);

		addCCDARule(ccdaRule);
	}
	
	replyMsg(connId, COLCDA_CREATE_RULE_REPLY, body);
}

void ColCdaServerObject::gotModifyRuleReply(CommMsgParser& parser, UINT32 connId, UINT32 reqId)
{
	PLog("gotModifyRuleReply");

	CommMsgBody body;

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	body.composeUINT32(reqId).composeINT16(errCode);

	if (errCode)
	{
		const char* errStr;
		parser.parseString(errStr);
		PLog("DBM error[%d]-%s", errCode, errStr);

		body.composeString(errStr);
	}
	else
	{
		CCDARule ccdaRule;
		ccdaRule.parse(parser);
		ccdaRule.compose(body);

		addCCDARule(ccdaRule);
	}
	
	replyMsg(connId, COLCDA_MODIFY_RULE_REPLY, body);
}

void ColCdaServerObject::gotActivateRuleReply(CommMsgParser& parser, UINT32 connId, UINT32 reqId)
{
	PLog("gotActivateRuleReply");

	CommMsgBody body;

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	body.composeUINT32(reqId).composeINT16(errCode);

	if (errCode)
	{
		const char* errStr;
		parser.parseString(errStr);
		PLog("DBM error[%d]-%s", errCode, errStr);

		body.composeString(errStr);
	}
	else
	{
		CCDARule ccdaRule;
		ccdaRule.parse(parser);
		ccdaRule.compose(body);

		addCCDARule(ccdaRule);
	}
	
	replyMsg(connId, COLCDA_ACTIVATE_RULE_REPLY, body);
}

void ColCdaServerObject::gotReorderRuleReply(CommMsgParser& parser, UINT32 connId, UINT32 reqId)
{
	PLog("gotReorderRuleReply");

	CommMsgBody body;

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	body.composeUINT32(reqId).composeINT16(errCode);

	if (errCode)
	{
		const char* errStr;
		parser.parseString(errStr);
		PLog("DBM error[%d]-%s", errCode, errStr);

		body.composeString(errStr);
	}
	else
	{
		vector<CCDARule> ccdaRuleVec;
		parseVectorOfParsableMessageBodies(ccdaRuleVec, parser);
		composeVectorOfMessageBodiesOfComposableValues(ccdaRuleVec, body);

		reorderCCDARule(ccdaRuleVec);
	}
	
	replyMsg(connId, COLCDA_REORDER_RULES_REPLY, body);
}

void ColCdaServerObject::gotDeleteRuleReply(CommMsgParser& parser, UINT32 connId, UINT32 reqId)
{
	PLog("gotDeleteRuleReply");

	CommMsgBody body;

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	body.composeUINT32(reqId).composeINT16(errCode);

	if (errCode)
	{
		const char* errStr;
		parser.parseString(errStr);
		PLog("DBM error[%d]-%s", errCode, errStr);

		body.composeString(errStr);
	}
	else
	{
		CCDARule ccdaRule;
		ccdaRule.parse(parser);
		ccdaRule.compose(body);

		deleteCCDARule(ccdaRule);
	}
	
	replyMsg(connId, COLCDA_DELETE_RULE_REPLY, body);
}

void ColCdaServerObject::gotReadRuleReply(CommMsgParser& parser, UINT32 connId, UINT32 reqId)
{
	PLog("gotReadRuleReply");

	CommMsgBody body;

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	body.composeUINT32(reqId).composeINT16(errCode);

	if (errCode)
	{
		const char* errStr;
		parser.parseString(errStr);
		PLog("DBM error[%d]-%s", errCode, errStr);

		body.composeString(errStr);
	}
	else
	{
		CCDARule ccdaRule;
		ccdaRule.parse(parser);

		ccdaRule.compose(body);
	}
	
	replyMsg(connId, COLCDA_READ_RULE_REPLY, body);
}

void ColCdaServerObject::gotReadCurrRuleListReply(CommMsgParser& parser, UINT32 connId, UINT32 reqId)
{
	PLog("gotReadCurrRuleListReply");

	CommMsgBody body;

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	body.composeUINT32(reqId).composeINT16(errCode);

	if (errCode)
	{
		const char* errStr;
		parser.parseString(errStr);
		PLog("DBM error[%d]-%s", errCode, errStr);

		body.composeString(errStr);

	}
	else
	{
		vector<CCDARule> ccdaRuleVec;
		parseVectorOfParsableMessageBodies(ccdaRuleVec, parser);

		composeVectorOfMessageBodiesOfComposableValues(ccdaRuleVec, body);

		if (connId == 0)
		{
			// local server call to load rule list
			for (int i = 0; i < ccdaRuleVec.size(); ++i)
			{
				addCCDARule(ccdaRuleVec[i]);
			}
			PLog("Found #%u CCDA rules.", ccdaRuleVec.size());
		}
	}
	
	if (connId != 0)
	{
		replyMsg(connId, COLCDA_READ_CURRENT_RULE_LIST_REPLY, body);
	}
}

void ColCdaServerObject::gotReadFullRuleListReply(CommMsgParser& parser, UINT32 connId, UINT32 reqId)
{
	PLog("gotReadFullRuleListReply");

	CommMsgBody body;

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	body.composeUINT32(reqId).composeINT16(errCode);

	if (errCode)
	{
		const char* errStr;
		parser.parseString(errStr);
		PLog("DBM error[%d]-%s", errCode, errStr);

		body.composeString(errStr);
	}
	else
	{
		vector<CCDARuleStateData> ruleStateVec;
		parseVectorOfParsableMessageBodies(ruleStateVec, parser);

		composeVectorOfMessageBodiesOfComposableValues(ruleStateVec, body);
	}
	
	replyMsg(connId, COLCDA_READ_FULL_RULE_LIST_REPLY, body);
}

// PYR-81917
void ColCdaServerObject::gotRuleHistoryReply(CommMsgParser& parser, UINT32 connId, UINT32 reqId)
{
	PLog("gotRuleHistoryReply");

	CommMsgBody body;

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	body.composeUINT32(reqId).composeINT16(errCode);

	if (errCode)
	{
		const char* errStr;
		parser.parseString(errStr);
		PLog("DBM error[%d]-%s", errCode, errStr);

		body.composeString(errStr);

	}
	else
	{
		vector<CCDARule> ccdaRuleVec;
		parseVectorOfParsableMessageBodies(ccdaRuleVec, parser);

		composeVectorOfMessageBodiesOfComposableValues(ccdaRuleVec, body);
	}

	if (connId != 0)
	{
		replyMsg(connId, COLCDA_GET_RULE_HISTORY_REPLY, body);
	}
}

void ColCdaServerObject::gotCurrColWhitelist(CommMsgParser& parser, UINT32 connId_, UINT32 reqId)
{
	PLog("gotCurrColWhitelist");

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	if (errCode)
	{
		const char* errStr;
		parser.parseString(errStr);
		PLog("DBM error[%d]-%s", errCode, errStr);
	}
	else
	{
		vector<ColWhitelist> whitelistVec;
		parseVectorOfParsableMessageBodies(whitelistVec, parser);
		colWhitelistCache.loadWhitelist(whitelistVec, ColWhitelistModule_CCDA);
	}
}

void ColCdaServerObject::addCCDARule(const CCDARule& rule)
{
	if (rule.ruleType == CCDA_RuleType_RingGame)
	{
		ringGameRules.addRule(rule);
	}
	else if (rule.ruleType == CCDA_RuleType_SNGHU)
	{
		sngHUGameRules.addRule(rule);
	}
	else
	{
		PLog(LOG_TRACE_FAULTY_LOGIC" unknown rule type:%u", rule.ruleType);
	}
}

void ColCdaServerObject::deleteCCDARule(const CCDARule& rule)
{
	if (rule.ruleType == CCDA_RuleType_RingGame)
	{
		ringGameRules.deleteRule(rule);
	}
	else if (rule.ruleType == CCDA_RuleType_SNGHU)
	{
		sngHUGameRules.deleteRule(rule);
	}
	else
	{
		PLog(LOG_TRACE_FAULTY_LOGIC" unknown rule type:%u", rule.ruleType);
	}
}

void ColCdaServerObject::reorderCCDARule(const vector<CCDARule>& ccdaRuleVec)
{
	if (ccdaRuleVec.size() == 0)
		return;

	if (ccdaRuleVec[0].ruleType == CCDA_RuleType_RingGame)
	{
		ringGameRules.reorderRules(ccdaRuleVec);
	}
	else if (ccdaRuleVec[0].ruleType == CCDA_RuleType_SNGHU)
	{
		sngHUGameRules.reorderRules(ccdaRuleVec);
	}
	else
	{
		PLog(LOG_TRACE_FAULTY_LOGIC" unknown rule type:%u", ccdaRuleVec[0].ruleType);
	}
}