// dbmpropertymanagement.cpp (moved from dbm/hgdbm)

////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2010 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include	"dbmpropman.h"

INT16 PropertyManagement::getIntProperty(const char* propName, int& propValue, PString& sqlErr )
{
	try
	{
		Property property;
		bool found;
		getProperty( propName, property, found );
		if (found)
		{
			propValue = property.propInt;
		}
		else propValue = 0;
		return DBM_NO_ERROR;
	}
	catch(const PSqlError& er )
	{
		sqlErr = er.why();
		return er.code();
	}
}

INT16 PropertyManagement::saveIntProperty(const char* propName, int propValue, PString& sqlErr )
{
	try
	{
		Property property;
		property.propInt = propValue;
		saveProperty( propName, property );
		return DBM_NO_ERROR;
	}
	catch(const PSqlError& er )
	{
		sqlErr = er.why();
		return er.code();
	}
}

void PropertyManagement::getProperty( const char* propName, Property& property, bool& found  )
{
	found = getProperty( propName, property );
}

bool PropertyManagement::getProperty( const char* propName, Property& property )
{
	getPropertyStmt.setPropertyId( propName );
	getPropertyStmt.execute();
	bool found = false;
	if( getPropertyStmt.fetch() )
	{
		found = true;
		property.propInt = getPropertyStmt.getPropInt();
		property.propStr.assign(getPropertyStmt.getPropStr() );
		property.isWhenSet = getPropertyStmt.isWhenSet();
		if(property.isWhenSet)
		{
			getPropertyStmt.getWhen( property.propWhen );
		}
	}
	getPropertyStmt.closeCursor();
	return found;
}

INT16 PropertyManagement::saveProperty( const char* propName, Property& property, PString& sqlErr )
{
	bool transStarted = false;
	try
	{
		setAutoCommitAndFlag(false, transStarted);
		saveProperty( propName, property );
		dbmManPxy.extCommit();
		setAutoCommitAndFlag(true, transStarted);
		return DBM_NO_ERROR;
	}
	catch(const PSqlError& er )
	{
		if(transStarted)
		{
			dbmManPxy.extRollback();
			setAutoCommitAndFlag(true, transStarted);
		}
		sqlErr = er.why();
		return er.code();
	}
}

void PropertyManagement::saveProperty( const char* propName, Property& property )
{
	Property propTmp;
	bool found;
	getProperty( propName, propTmp, found );
	if( found )
	{
		updateProperty(propName, property);
	}
	else
	{
		insertProperty( propName, property );
	}
}

void PropertyManagement::updateProperty( const char* propName, Property& property )
{
	updatePropertyStmt.setPropertyId( propName );
	updatePropertyStmt.setStrProperty( property.propStr );
	updatePropertyStmt.setPropertyInt( property.propInt);
	if(!property.propWhen.isNull())
	{
		updatePropertyStmt.setWhen(property.propWhen);
	}
	else
	{
		updatePropertyStmt.setWhenNull();
	}
	updatePropertyStmt.execUpdateComposeLog();
}

void PropertyManagement::insertProperty( const char* propName, Property& property )
{
	insertPropertyStmt.setPropertyId( propName );
	insertPropertyStmt.setStrProperty( property.propStr );
	insertPropertyStmt.setPropertyInt(property.propInt);
	if(!property.propWhen.isNull())
	{
		insertPropertyStmt.setWhen(property.propWhen);
	}
	else
	{
		insertPropertyStmt.setWhenNull();
	}
	insertPropertyStmt.executeComposeLog();
}

void PropertyManagement::setAutoCommitAndFlag( const bool isAutoCommit, bool& isInTransaction )
{
	dbmManPxy.extSetAutoCommit(isAutoCommit);
	isInTransaction = !isAutoCommit;
}