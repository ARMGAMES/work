//dbmanager.h (moved from dbm/hgdbm)

////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2010 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef dbmpropertymanagement_h_included
#define	dbmpropertymanagement_h_included

#include	"ppstring.h"
#include	"commlib.h"
#include	"dbcommon.h"
#include    "dbm.h"
#include	"dbmolap.h"
#include	"dbmstmtolap.h"
#include	"dbmstmtcommon.h"

////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2010 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// This file contains definition for PropertyManagement class which encapsulates
// work with PROPERTIES table required for OLAP
// in order for this class to be able to use commit and rollback it required
// the project database manager class to implement DbManagerTransControlInterface
// that exposes project specific db managed commit, rollback and setAutoCommit functions 
// 
// I plan to make it available for other projects by moving it to the protocols folder
// later on

class DbManagerTransControlInterface
{
public:
	DbManagerTransControlInterface(){}
	virtual void extCommit()	= 0;
	virtual void extRollback()	= 0;
	virtual void extSetAutoCommit(const bool isAutoCommit) = 0;
	virtual ~DbManagerTransControlInterface(){}
};

// instances of this class should not be long lived because it's just a way to
// encapsulate algorithms of working with properties. 
// The object does not keep any state, only references to the DbManager and
// its get/insert/update property statements so it's really cheap to instantiate
// the object itself and it's a bad idea to keep it longer than the duration of the
// call if outer statement can be recreated
//
class PropertyManagement
{
	DbManagerTransControlInterface&	dbmManPxy;
	GetPropertyStmt&				getPropertyStmt;
	UpdatePropertyStmt&				updatePropertyStmt;
	InsertPropertyStmt&				insertPropertyStmt;

	void setAutoCommitAndFlag(const bool isAutoCommit, bool& isInTransaction);
	
public:
	PropertyManagement(	DbManagerTransControlInterface&	dbmManPxy_,
						GetPropertyStmt&				getPropertyStmt_,
						UpdatePropertyStmt&				updatePropertyStmt_,
						InsertPropertyStmt&				insertPropertyStmt_)
						:
						dbmManPxy(dbmManPxy_),
						getPropertyStmt(getPropertyStmt_),
						updatePropertyStmt(updatePropertyStmt_),
						insertPropertyStmt(insertPropertyStmt_)
	{
	}
public:
	INT16 getIntProperty(const char* propName, int& propValue, PString& sqlErr);
	INT16 saveIntProperty(const char* propName, int propValue, PString& sqlErr);

	void getProperty(const char* propName, Property& property, bool& found);
	INT16 saveProperty(const char* propName, Property& property, PString& sqlErr);
	bool getProperty(const char* propName, Property& property);
	void saveProperty(const char* propName, Property& property);
	void updateProperty(const char* propName, Property& property);
	void insertProperty(const char* propName, Property& property);
};

#endif //dbmpropertymanagement_h_included
