#ifndef commoncrophandlers_h_included
#define commoncrophandlers_h_included

#include "dbcommon.h"
#include "pyrtime.h"
#include "cropdate.h"

class CommServerConnection;

class GetNextCropDateHandler
{
public:
	void process(CommMsgParser& parser, CommServerConnection* conn, CropDate& cd);
};

class SaveLastCroppedHandler
{
public:
	void process(CommMsgParser& parser, CommServerConnection* conn, CropDate& cd);
};

// added for PYR-15607 Home Games
class GetNextCropTimeHandler
{
public:
	static void process(CommMsgParser& parser, CommServerConnection* conn, CropTime& ct);
};

class SaveLastCropTimeHandler
{
public:
	static void process(CommMsgParser& parser, CommServerConnection* conn, CropTime& ct);
};



#endif // commoncrophandlers_h_included
