#ifndef commdispatcher_h_included
#define commdispatcher_h_included

#include "ppinclude.h"
#include "ppstring.h"
#include "pperror.h"
#include "ppinifile.h"
#include "pplogfile.h"
#include "ppdaemon.h"

#include "commrtable.h"
#include "commsrv.h"
#include "commsrvcomposite.h"
#include "commtmlocal.h"
#include "commtmrw.h"
#include "commtmlm.h"
#include "commappfactory.h"

class CommDispatcherDaemon : public PDaemon
{
protected:
	CommRoutingTable theRoutingTable;
	/*	
	CommRWThreadManager rw;
	CommRWThreadManager rwMux;
	CommRWThreadManagerSSL rwSsl;
	CommLMThreadManager lmMux;
	CommRWThreadManagerProcess rwSslPr;
	*/
	CommLocalThreadManager local;
	vector <CommThreadManager *> managers;
	CommFactoryServerBase appFactory;


	//NEW 
	vector <CommCompositeServerObjectFactory *> factories;
	//
private:
	void _readCertificate( const PIniFile & iniFile, const char * sectionName, CommRWThreadManagerBaseSSL& rw);
	void _readCertificates( const PIniFile & iniFile, const char * sectionName, CommRWThreadManagerBaseSSL& rw);
	void _readOptions( const PIniFile & iniFile, const char * sectionName, CommThreadManager& rw);
public:
	CommDispatcherDaemon();
	~CommDispatcherDaemon()
	{
		vector <CommThreadManager *>::iterator itm;
		for (itm = managers.begin(); itm!=managers.end();++itm)
		{
			delete (*itm);
		}

		vector <CommCompositeServerObjectFactory *>::iterator itf;
		for (itf = factories.begin(); itf!=factories.end();++itf)
		{
			delete (*itf);
		}

	}
	void start( const char* dirName, int nParms, char** parms ) override;
	void stop() override;

public:
	static void composeInitMsg( CommMsgBody& ret, const char* s );
	static void composeInitMsgKVPairs(CommMsgBody& ret, const char* s); //PYR-121303
	static void composeSrvObjInitMsg(CommMsgBody& ret, const char* input, const PString& objectName); // PYR-124776
};

#endif
