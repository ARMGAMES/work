#ifndef AAMSCommunicator_h_included
#define AAMSCommunicator_h_included

#include "ppsocket.h"
#include "pyrtime.h"
#include "CommPKCS7.h"
#include "aamstransport.h"

class CommMonitorLoggerObject;

class AAMSCommunicator
{
public:
	CommPKCS7_Sign pkcs7Sign;
	CommPKCS7_Verify pkcs7Verify;
	bool usePkcs7;

	PString aamsUrl;
	PString aamsUrl_780;
	PString aamsUrl_580;	//CAS-3400
	UINT32 httpTimeout;
	UINT32 numRetries;
	UINT32 headerLength;

	// Monitor parameters
	UINT32 aamsRequests;
	UINT32 aamsRequestRetries;
	UINT32 aamsConnectionErrors;
	UINT32 aamsConsecutiveConnectionErrors;
	UINT32 aamsErrors;
	UINT32 aamsProtocolErrors;
	SrvTime lastTransmission;

	AAMSCommunicator()
	{
		usePkcs7 = false;
		httpTimeout = numRetries = 0;
		headerLength = HEADER_LENGTH;
		aamsRequests = aamsRequestRetries = aamsConnectionErrors = aamsConsecutiveConnectionErrors = aamsErrors = aamsProtocolErrors = 0;
	}

	INT16 submitToAams(const char* messageType, const PBlock& aamsRequest, PBlock& aamsReply);
	void reportPerformance(CommMonitorLoggerObject* heLogger, CommMsgBody& reportBody, UINT32 transportIndex) const;

private:
	int readAamsSocket(PIPSocket& sock, BYTE * p, size_t rd) const;
	int httpRequestResponse(const char* host, int port, const char* rqStr, const PBlock& rqBin, PBlock& replyBody);
	bool aamsRequestReply(const char* messageType, const PBlock& aamsRequest, PBlock& aamsReply);
};

void parseAamsDate(const PBlock& aamsReply, UINT32 aamsDateOffset, SrvDate& aamsDate);
bool aamsRepliesWithResultAndDate(const char* messageType);

#endif // AAMSCommunicator_h_included
