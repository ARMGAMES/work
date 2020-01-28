#include "commoncrophandlers.h"
#include "commsrv.h"
#include "crop_type.h"


void GetNextCropDateHandler::process( CommMsgParser& parser, CommServerConnection* conn, CropDate& cd )
{
	UINT32 reqId, cropType, dependentOnCropType;
	parser.
		parseUINT32(reqId).
		parseUINT32(cropType).
		parseUINT32(dependentOnCropType) // PYR-28694
		;
	PString sqlErr;
	SrvDate nextCropDate, dependentOnCropDate;
	INT16 err_code = cd.getNextCropDate(cropType, nextCropDate, sqlErr);
	
	if( !err_code && dependentOnCropType != typeNone )
	{
		err_code = cd.getNextCropDate(dependentOnCropType, dependentOnCropDate, sqlErr);
	}
	CommMsgBody body;
	body.
		composeUINT32( reqId ).
		composeINT16( err_code );
	if(err_code)
	{
		body.composeString(sqlErr);
		PLog("DBM_Q_GET_NEXT_CROPDATE error: %d %s", err_code, sqlErr.c_str());
	}
	else
	{
		body.composeUINT32(cropType)
			.composeSrvDate(nextCropDate)
			.composeUINT32(dependentOnCropType)
			.composeSrvDate(dependentOnCropDate);
	}
	conn->post(DBM_A_GET_NEXT_CROPDATE, body);
}

void SaveLastCroppedHandler::process( CommMsgParser& parser, CommServerConnection* conn, CropDate& cd )
{
	UINT32 reqId, cropType;
	SrvDate cropDate;
	parser.
		parseUINT32(reqId).
		parseUINT32(cropType);
	cropDate.parse(parser);

	PString sqlErr;

	INT16 err_code = cd.saveCropDate(cropType, cropDate, sqlErr);
	CommMsgBody body;
	body.
		composeUINT32( reqId ).
		composeINT16( err_code );
	if(err_code)
	{
		body.composeString(sqlErr);
		PLog("DBM_Q_SAVE_LAST_CROPPED error: %d %s", err_code, sqlErr.c_str());
	}
	conn->post(DBM_A_SAVE_LAST_CROPPED, body);
}

//////////////////////////////////////////////////////////////////////////
// added for PYR-15607 Home Games
//static
void GetNextCropTimeHandler::process( CommMsgParser& parser, CommServerConnection* conn, CropTime& ct)
{
	UINT32 reqId, cropType;
	parser.parseUINT32(reqId)
		  .parseUINT32(cropType);

	PString sqlErr;
	SrvTime nextCropTime;
	INT16 err_code = ct.getNextCropTime(cropType, nextCropTime, sqlErr);
	if(err_code)
	{
		PLog("DBM_Q_GET_NEXT_CROPTIME_COMMON error: %d %s", err_code, sqlErr.c_str());
	}

	CommMsgBody body;
	body.
		composeUINT32(reqId).
		composeINT16(err_code);
	if(err_code)
	{
		body.composeString(sqlErr);
	}
	else
	{
		body.composeUINT32(cropType)
			.composeSrvTime(nextCropTime);
	}
	conn->post(DBM_A_GET_NEXT_CROPTIME_COMMON, body);
}
// static
void SaveLastCropTimeHandler::process( CommMsgParser& parser, CommServerConnection* conn, CropTime& ct )
{
	UINT32 reqId, cropType, index;
	SrvTime cropTime;

	parser.parseUINT32(reqId)
		  .parseUINT32(cropType)
		  .parseSrvTime(cropTime)
		  .parseUINT32(index);

	PString sqlErr;
	INT16 err_code = ct.saveCropTime(cropType, cropTime, sqlErr);
	if(err_code)
	{
		PLog("DBM_Q_SAVE_LAST_CROPPED_TIME_COMMON error: %d %s", err_code, sqlErr.c_str());
	}

	CommMsgBody body;
	body.
		composeUINT32(reqId).
		composeINT16(err_code);
	if(err_code)
	{
		body.composeString(sqlErr);
	}	
	else
	{
		body.composeUINT32(cropType)
			.composeUINT32(index);
	}
	conn->post(DBM_A_SAVE_LAST_CROPPED_TIME_COMMON, body);
}