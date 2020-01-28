#include "httpshelper.h"//MUST be included first to ensure proper include order
#include "AAMSCommunicator.h"
#include "aamstransport.h"
#include "heLogger.h"

void parseAamsDate(const PBlock& aamsReply, UINT32 aamsDateOffset, SrvDate& aamsDate)
{
	aamsDate._year = CommMsgBody::readUINT16(aamsReply.ptr() + aamsDateOffset);
	aamsDate._month = CommMsgBody::readUINT16(aamsReply.ptr() + aamsDateOffset + TWO_BYTE_LEN);
	aamsDate._day = CommMsgBody::readUINT16(aamsReply.ptr() + aamsDateOffset + 2 * TWO_BYTE_LEN);
}

bool aamsRepliesWithResultAndDate(const char* messageType)
{
	if(PString::compare(messageType, CANCELLATION_OF_RIGHT_TO_PARTICIPATE) == 0)
		return true;
	if(PString::compare(messageType, CONFIRMATION_OF_THE_SESSION) == 0)
		return true;
	if(PString::compare(messageType, CANCELLATION_OF_SESSION) == 0)
		return true;
	return false;
}

INT16 AAMSCommunicator::submitToAams(const char* messageType, const PBlock& aamsRequest, PBlock& aamsReply)
{
	aamsRequests++;
	try
	{
		//
		// Sign with PKCS7
		//
		const PBlock* requestBlock;
		PBlock* replyBlock;

		PBlock aamsRequestSigned;
		PBlock aamsReplySigned;
		if(usePkcs7)
		{
			pkcs7Sign.sign(aamsRequest.ptr(), aamsRequest.size(), aamsRequestSigned);
			PLog(" aamsRequest size %d, aamsRequestSigned size %d", aamsRequest.size(), aamsRequestSigned.size());
			requestBlock = &aamsRequestSigned;
			replyBlock = &aamsReplySigned;
		}
		else
		{
			PLog(" unsigned aamsRequest size %d", aamsRequest.size());
			requestBlock = &aamsRequest;
			replyBlock = &aamsReply;
		}

		UINT16 aamsErrorCode = AAMS_PROTOCOL_NOT_AVAILABLE;
		if(aamsRequestReply(messageType, *requestBlock, *replyBlock))
		{
			//
			// Verify with PKCS7
			//
			bool pkcs7Ok = true;


			if(usePkcs7)
			{
				try
				{
//					pkcs7Ok = pkcs7Verify.verify(aamsReplySigned.ptr(), aamsReplySigned.size(), aamsReply);
					pkcs7Verify.verify(aamsReplySigned.ptr(), aamsReplySigned.size(), aamsReply);
					PLog(" pkcs7 %s, aamsReplySigned size %d, aamsReply size %d",
						pkcs7Ok ? "PASSED" : "FAILED", //-V547
						aamsReplySigned.size(), aamsReply.size());

				}
				catch(const PError& ex)
				{
					PString reply((const char*)aamsReplySigned.ptr(), aamsReplySigned.size());
					PLog(" pkcs7Verify.verify exception: %s\naamsReplySigned: %s", ex.why(), reply.c_str());
					pkcs7Ok = false;
				}
			}
			else
			{
				PLog(" unsigned aamsReply size %d", aamsReply.size());
			}
			if(!pkcs7Ok)
			{
				aamsProtocolErrors++;
				aamsErrorCode = AAMS_PROTOCOL_PKCS7_ERROR;
			}
			else
			{
				// Get error code from AAMS reply
				if(aamsReply.size() >= headerLength + RESULT_CODE_LEN)
				{
					aamsErrorCode = CommMsgBody::readUINT16(aamsReply.ptr() + headerLength);
					if(aamsErrorCode)
					{
						PString hackErrDescr((const char*)aamsReply.ptr() + headerLength + RESULT_CODE_LEN, aamsReply.size() - (headerLength + RESULT_CODE_LEN));
						PLog(" AAMS reply error: %d '%s'", aamsErrorCode, hackErrDescr.c_str());
						aamsErrors++;
					}
					else if(aamsRepliesWithResultAndDate(messageType))
					{
						if(aamsReply.size() >= headerLength + RESULT_CODE_LEN + DATE_LEN)
						{
							SrvDate aamsDate;
							parseAamsDate(aamsReply, headerLength + RESULT_CODE_LEN, aamsDate);
							PLog(" AAMS OK reply date: %02d/%02d/%02d", aamsDate._year, aamsDate._month, aamsDate._day);
						}
						else
						{
							PLog(" aamsReply.size() < HEADER_LENGTH + RESULT_CODE_LEN + DATE_LEN: %d != %d",
								aamsReply.size(), headerLength + RESULT_CODE_LEN + DATE_LEN);
							aamsProtocolErrors++;
						}
					}
				}
				else
				{
					PLog(" aamsReply.size() < HEADER_LENGTH + RESULT_CODE_LEN: %d != %d",
						aamsReply.size(), headerLength + RESULT_CODE_LEN);
					aamsProtocolErrors++;
				}
			}
		}
		return aamsErrorCode;
	}
	catch(const PError& ex)
	{
		PLog("submitToAams failed: %s", ex.why());
		return AAMS_PROTOCOL_ERROR;
	}
}

int AAMSCommunicator::readAamsSocket(PIPSocket& sock, BYTE * p, size_t rd) const
{
	try
	{
		return sock.recvStream(p, rd, httpTimeout*1000);
	}
	catch(const PError& e)
	{
		PLog("recvStream error: %s", e.why());
		return -2;
	}
}

int AAMSCommunicator::httpRequestResponse(const char* host, int port, const char* rqStr, const PBlock& rqBin, PBlock& replyBody)
{
	PLog( "Entering httpRequestResponse..." );
	PIPSocket sock;
	bool Ok = sock.enableKeepAlive();
	if( !Ok )
		PLog( "httpRequestResponse: unable to enable keepAlive" );

	PLog( "Connecting to %s:%d", host, port );
	sock.connect( host, port );
	PLog( "Connected to %s:%d", host, port );

	PLog( "Sending header data..." );
	sock.sendComplete( (const BYTE*)rqStr, strlen( rqStr ) );
	PLog( "Sending binary data..." );
	sock.sendComplete( rqBin.ptr(), rqBin.size() );

	PLog( "Receiving data..." );
	PBlock ret;
	CommHttpResponse resp;
	bool headerReady = false;
	UINT32 contentLength = 0;
	UINT32 headerLength = 0;
	for(;;)
	{
		BYTE buf[ 1024 ];
		size_t bytes2read = contentLength ? (headerLength + contentLength) - ret.size() : sizeof(buf);
		if(bytes2read > sizeof(buf))
		{
			bytes2read = sizeof(buf);
		}
		if(!bytes2read)
		{
			break; // and of data
		}
		int rd = readAamsSocket(sock, buf, bytes2read);
		if(rd == -1)
		{
			aamsConnectionErrors++;
			aamsConsecutiveConnectionErrors++;
			PLog(" AAMS request timeout: %d sec", httpTimeout);
			sock.close();
			return -1;
		}
		if(rd == -2)
		{
			aamsConnectionErrors++;
			aamsConsecutiveConnectionErrors++;
			PLog(" AAMS request exception");
			sock.close();
			return -2;
		}
		aamsConsecutiveConnectionErrors = 0;
		if(!rd)
		{
			break; // and of stream
		}
		if(!headerReady)
		{
			const BYTE* p = buf;
			headerReady = resp.read(p, rd);
			if(headerReady)
			{
				const char* contentLength_ = resp.getHeader("Content-Length");
				headerLength = ret.size() + p - buf;
				if(contentLength_)
				{
					contentLength = atoi(contentLength_);
					PLog("Content-Length: %d, header sz: %d, total sz: %d",
						contentLength, headerLength, headerLength + contentLength);
				}
				else
				{
					PLog("Content-Length: n/a, header sz: %d, total sz: n/a", headerLength);
				}
			}
		}

		// TODO: optimize to read directly into replyBody
		memcpy( ret.append( rd ), buf, rd );
	}
	sock.close();

	if(!headerReady)
	{
		throw PInternalError( "Incomplete HTTP response header" );
	}

	memcpy( replyBody.append(ret.size() - headerLength), ret.ptr() + headerLength, ret.size() - headerLength );
	lastTransmission.currentLocalTime();

	PLog( "Exiting httpRequestResponse. replyBody sz: %d", replyBody.size() );
	return resp.getRespCode();
}

static UINT32 getRetryDelay(UINT32 retry)
{
	switch(retry)
	{
	case 1:
		return 30;
	case 2:
		return 60;
	default:
		return 90;
	}
}

#define MAX_NUM_REGISTRATION_REQUEST_RETRIES 3

bool AAMSCommunicator::aamsRequestReply(const char* messageType, const PBlock& aamsRequest, PBlock& aamsReply)
{
	CommHttpRequest rq;
	rq.setNoPortInHostName();

	if( PString::compare(messageType, REPORT_GAME_BEING_PLAYED /*780*/) == 0 )
	{
		rq.setUrl( aamsUrl_780 );
	}
	else if( PString::compare(messageType, AUX_REPORT_GAME_BEING_PLAYED /*580*/) == 0 ) //CAS-3400: message 580 use different url
	{
		rq.setUrl( aamsUrl_580 );
	}
	else
	{
		rq.setUrl( aamsUrl );
	}

	PString rqStr;
	rq.createPostRequestHeader( rqStr, aamsRequest.size(), "application/binary" );

	UINT32 retries = 0;
	const UINT32 maxNumRetries = 
		( (PString::compare(messageType, "200") == 0) || (PString::compare(messageType, "220") == 0))
		? MAX_NUM_REGISTRATION_REQUEST_RETRIES : numRetries;

	bool resultOk = false;
	while(retries <= maxNumRetries)
	{
		try
		{
			if(retries)
			{
				aamsRequestRetries++;
				PLog(" AAMS request retry #%d out of total %d will happen in %d seconds...", retries, maxNumRetries, getRetryDelay(retries));
				PThread::sleep(getRetryDelay(retries) * 1000);
			}
			retries++;

			aamsReply.clear();
			int httpResponseCode = httpRequestResponse( rq.socketHost(), rq.socketPort(), rqStr, aamsRequest, aamsReply );

			PLog( "HTTP response: %d", httpResponseCode );

			resultOk = httpResponseCode == 200;
			if(resultOk)
			{
				break;
			}
		}
		catch(const PError& ex)
		{
			aamsConnectionErrors++;
			aamsConsecutiveConnectionErrors++;
			PLog("PError: %s", ex.why());
		}
	}

	return resultOk;
}

static void formatSrvTimeAAMS(const SrvTime& srvTime, PString& strValue)
{
	if(srvTime.isNull())
		strValue = "Not available";
	else
	{
		char buf[64];
		sprintf( buf, "%d-%02d-%02d %02d:%02d:%02d",
			srvTime._year, srvTime._month, srvTime._day,
			srvTime._hour, srvTime._minute, srvTime._second);
		strValue = buf;
	}
}

void AAMSCommunicator::reportPerformance( CommMonitorLoggerObject* heLogger, CommMsgBody& reportBody, UINT32 transportIndex ) const
{
	PString strValue;
	PString strName;

	strValue.assign("").appendInt(aamsRequests);
	heLogger->compose(reportBody, strName.assign("T").appendUint(transportIndex).append("-aamsRequests"), strValue);

	strValue.assign("").appendInt(aamsRequestRetries);
	heLogger->compose(reportBody, strName.assign("T").appendUint(transportIndex).append("-aamsRequestRetries"), strValue);

	strValue.assign("").appendInt(aamsConnectionErrors);
	heLogger->compose(reportBody, strName.assign("T").appendUint(transportIndex).append("-aamsConnectionErrors"), strValue);
	strValue.assign("").appendInt(aamsConsecutiveConnectionErrors);
	heLogger->compose(reportBody, strName.assign("T").appendUint(transportIndex).append("-aamsConsecutiveConnectionErrors"), strValue);

	strValue.assign("").appendInt(aamsErrors);
	heLogger->compose(reportBody, strName.assign("T").appendUint(transportIndex).append("-aamsErrors"), strValue);

	strValue.assign("").appendInt(aamsProtocolErrors);
	heLogger->compose(reportBody, strName.assign("T").appendUint(transportIndex).append("-aamsProtocolErrors"), strValue);

	formatSrvTimeAAMS(lastTransmission, strValue);
	heLogger->compose(reportBody, strName.assign("T").appendUint(transportIndex).append("-lastTransmission"), strValue);
}
