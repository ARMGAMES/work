#ifndef etcdbmBDUAprepstmt_h_included
#define	etcdbmBDUAprepstmt_h_included

////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include	"dbmstmt.h"
#include	"dbmstmtolap.h"
#include	"dbmolap.h"
#include	"etc\EtcDbmBDUACommon.h"

#define BUSINESS_DEFINED_USER_ATTRIBUTES_TABLE_NAME				"BDUSERATTRIBUTES"
#define BUSINESS_DEFINED_USER_ATTRIBUTES_AUDIT_TABLE_NAME		"BDUSERATTRIBUTESAUDIT"

//////////////////////////////////////////////////////////////////////////

class BaseBDUserAttributeStmt : public DbmStatementWithLog
{
	const PString tableName;

public:
	BaseBDUserAttributeStmt ( DatabaseManagerCommon& man, OlapMsgSysInfo::eOlapDataGroup dataGroup_, const char* tableName_ ) 
		: DbmStatementWithLog( man, dataGroup_ ), tableName( tableName_ )
	{
	}
	
	void composeOlapLogMessage()
	{
		initLog();

		setIndexWithinGroup(userIntId.value); // PYR-61555

		composeLog( userIntId	);
		composeLog( attrType	);
		composeLog( attrValue	);
		composeLog( container	);
	}
	void parseOlapMessage( CommMsgParser& parser ) override
	{
		userIntId	.parse( parser );
		attrType	.parse( parser );
		attrValue	.parse( parser );
		container	.parse( parser );
	}
	
protected:
	PSqlInt userIntId;
	PSqlInt attrType;
	PSqlInt attrValue;
	PSqlString<CONTAINER_LEN> container; // PYR-54580

	const char* getTableName() const { return tableName; }
};

class InsertBDUserAttributeStmt : public BaseBDUserAttributeStmt
{
	void prepareStmt()
	{ // DBA_REVIEWED #PYR-54580 NikolaiC. 2017.main08
		PString stmt;
		stmt.assign( "INSERT INTO " )
			.append( getTableName() )
			.append( " (USERINTID, ATTRTYPE, ATTRVALUE, CONTAINER) VALUES (?,?,?,?)" );

		prepare( stmt );

		bindFirstParam( userIntId );
		bindNextParam( attrType );
		bindNextParam( attrValue );
		bindNextParam( container );
	}

public:
	InsertBDUserAttributeStmt( DatabaseManagerCommon& man, OlapMsgSysInfo::eOlapDataGroup dataGroup_, const char* tableName_ = BUSINESS_DEFINED_USER_ATTRIBUTES_TABLE_NAME )
		: BaseBDUserAttributeStmt( man, dataGroup_, tableName_ )
	{
		prepareStmt();
	}

	void init( UINT32 userIntId_, INT32 attrType_, INT32 attrValue_, const char* container_ )
	{
		userIntId	= userIntId_;
		attrType	= attrType_;
		attrValue	= attrValue_;
		container.initCut(container_);

		composeOlapLogMessage();
	}

	UINT32 getMsgCode() const override
	{
		return DBM_Q_OLAP_INSERT_BUSINESS_DEFINED_USER_ATTRIBUTE;
	}
};

class UpdateBDUserAttributeStmt : public BaseBDUserAttributeStmt
{
	void prepareStmt()
	{ // DBA_REVIEWED #PYR-54580 NikolaiC. 2017.main08
		PString stmt;
		stmt.assign( "UPDATE " )
			.append( getTableName() )
			.append( " SET ATTRVALUE = ?, CONTAINER=? WHERE USERINTID = ? AND ATTRTYPE = ?" );

		prepare( stmt );

		bindFirstParam( attrValue );
		bindNextParam( container );
		// where
		bindNextParam( userIntId );
		bindNextParam( attrType );
	}

public:
	UpdateBDUserAttributeStmt( DatabaseManagerCommon& man, OlapMsgSysInfo::eOlapDataGroup dataGroup_, const char* tableName_ = BUSINESS_DEFINED_USER_ATTRIBUTES_TABLE_NAME )
		: BaseBDUserAttributeStmt( man, dataGroup_, tableName_ )
	{
		prepareStmt();
	}

	void init( UINT32 userIntId_, INT32 attrType_, INT32 attrValue_, const char* container_ )
	{
		userIntId	= userIntId_;
		attrType	= attrType_;
		attrValue	= attrValue_;
		container.initCut(container_);

		composeOlapLogMessage();
	}

	UINT32 getMsgCode() const override
	{
		return DBM_Q_OLAP_UPDATE_BUSINESS_DEFINED_USER_ATTRIBUTE;
	}
};

class SelectSingleBDUserAttributeStmt : public BaseBDUserAttributeStmt
{
	// this select can be used by 2 writable dbms: etc and main
	// by LZ request explicit specification of isolation level to guarantee no locks
	void prepareStmt()
	{ // #DBA_REVIEWED #PYR-54580 Nikolai C. 09/20/2017 2017.Main.08
		PString stmt;
		stmt.assign( "SELECT ATTRVALUE, CONTAINER FROM " )
			.append( getTableName() )
			.append( " WHERE USERINTID = ? AND ATTRTYPE = ? WITH UR" ); // EXPLICIT 'UR' to guarantee no locks for 2 writable dbms: main and etc
		prepare( stmt );

		bindFirstCol( attrValue );
		bindNextCol( container ); // PYR-54580
		// where
		bindFirstParam( userIntId );
		bindNextParam( attrType );
	}

public:
	SelectSingleBDUserAttributeStmt( DatabaseManagerCommon& man, OlapMsgSysInfo::eOlapDataGroup dataGroup_, const char* tableName_ = BUSINESS_DEFINED_USER_ATTRIBUTES_TABLE_NAME )
		: BaseBDUserAttributeStmt( man, dataGroup_, tableName_ )
	{
		prepareStmt();
		setLogActive( false );
	}

	void init( UINT32 userIntId_, INT32 attrType_ )
	{
		userIntId	= userIntId_;
		attrType	= attrType_;
	}

	void getData( UINT32& userIntId_, INT32& attrType_, INT32& attrValue_, PString& container_ ) const
	{
		userIntId_	= userIntId.value;
		attrType_	= attrType.value;
		attrValue_	= attrValue.value;
		container_	= container.c_str(); // PYR-54580
	}
};

class SelectFilteredBDUserAttributeStmt : public BaseBDUserAttributeStmt
{
	PSqlInt attrTypeLower;
	PSqlInt attrTypeUpper;

	// #DBA_REVIEWED #PYR-46610 NikolaiC 2016.Main.10
	void prepareStmt()
	{
		PString stmt;
		stmt.assign( "SELECT ATTRTYPE, ATTRVALUE FROM " )
			.append( getTableName() )
			.append( " WHERE USERINTID = ? AND ATTRTYPE >= ? AND ATTRTYPE <= ?" );
		prepare( stmt );

		bindFirstCol( attrType );
		bindNextCol( attrValue );
		// where
		bindFirstParam( userIntId );
		bindNextParam( attrTypeLower );
		bindNextParam( attrTypeUpper );
	}

public:
	SelectFilteredBDUserAttributeStmt( DatabaseManagerCommon& man, OlapMsgSysInfo::eOlapDataGroup dataGroup_, const char* tableName_ = BUSINESS_DEFINED_USER_ATTRIBUTES_TABLE_NAME )
		: BaseBDUserAttributeStmt( man, dataGroup_, tableName_ )
	{
		prepareStmt();
		setLogActive( false );
	}

	void init( UINT32 userIntId_, INT32 attrTypeLower_, INT32 attrTypeUpper_ )
	{
		userIntId		= userIntId_;
		attrTypeLower	= attrTypeLower_;
		attrTypeUpper	= attrTypeUpper_;
	}

	void getData( BDUAData& data )
	{
		data.attrType	= attrType.value;
		data.attrValue	= attrValue.value;
	}
};

class InsertBDUserAttributeAuditStmt : public DbmStatementWithLog
{
	PString tableName;

	// #DBA_REVIEWED #PYR-54580 Nikolai C. 09/20/2017 2017.Main.08
	void prepareStmt()
	{
		PString stmt;
		stmt.assign( "INSERT INTO " )
			.append( tableName )
			.append( " (AUDITID, ADMININTID, WHEN, ACTION, BATCHTYPE, BATCHID, USERINTID, ATTRTYPE, OLDATTRVALUE, NEWATTRVALUE, MODULERUNID, OLDCONTAINER, NEWCONTAINER) " )
			.append( "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)" );
		prepare( stmt );

		bindFirstParam( auditId );
		bindNextParam( adminIntId );
		bindNextParam( when );
		bindNextParam( action );
		bindNextParam( batchType );
		bindNextParam( batchId );
		bindNextParam( userIntId );
		bindNextParam( attrType );
		bindNextParam( oldAttrValue );
		bindNextParam( newAttrValue );
		bindNextParam( modelRunId );
		bindNextParam( oldContainer );
		bindNextParam( newContainer );
	}

protected:
	PSqlBigInt auditId;
	PSqlInt adminIntId;
	PSqlTimestamp when;
	PSqlSmallInt action;
	PSqlSmallInt batchType;
	PSqlInt batchId;
	PSqlInt userIntId;
	PSqlInt attrType;
	PSqlInt oldAttrValue;
	PSqlInt newAttrValue;
	PSqlInt modelRunId; // PYR-50432
	PSqlString<CONTAINER_LEN> oldContainer; // PYR-54580
	PSqlString<CONTAINER_LEN> newContainer; // PYR-54580

	void composeOlapLogMessage()
	{
		initLog();

		composeLog( auditId			);
		composeLog( adminIntId		);
		composeLog( when			);
		composeLog( action			);
		composeLog( batchType		);
		composeLog( batchId			);
		composeLog( userIntId		);
		composeLog( attrType		);
		composeLog( oldAttrValue	);
		composeLog( newAttrValue	);
		composeLog( modelRunId		);
		composeLog( oldContainer	);
		composeLog( newContainer	);
	}
	
	void parseOlapMessage( CommMsgParser& parser ) override
	{
		auditId			.parse( parser );
		adminIntId		.parse( parser );
		when			.parse( parser );
		action			.parse( parser );
		batchType		.parse( parser );
		batchId			.parse( parser );
		userIntId		.parse( parser );
		attrType		.parse( parser );
		oldAttrValue	.parse( parser );
		newAttrValue	.parse( parser );
		// TODO: remove conditional parsing after NJ build 03 release
		modelRunId = 0;
		if (!parser.parseEnded())
		{
			modelRunId.parse( parser );
		}
		// PYR-54580
		oldContainer.parse( parser );
		newContainer.parse( parser );
	}

	UINT32 getMsgCode() const override
	{
		return DBM_Q_OLAP_INSERT_BUSINESS_DEFINED_USER_ATTRIBUTE_AUDIT; 
	}

public:
	InsertBDUserAttributeAuditStmt( DatabaseManagerCommon& man, OlapMsgSysInfo::eOlapDataGroup dataGroup_, const char* tableName_ = BUSINESS_DEFINED_USER_ATTRIBUTES_AUDIT_TABLE_NAME )
		: DbmStatementWithLog( man, dataGroup_ )
		, tableName( tableName_ )
	{
		prepareStmt();
	}

	void init(	UINT64 auditId_,
				UINT32 adminIntId_,
				const SrvTime& when_,
				INT16 action_,
				INT16 batchType_,
				INT32 batchId_,
				UINT32 userIntId_,
				INT32 attrType_,
				INT32 oldAttrValue_,
				INT32 newAttrValue_ ,
				INT32 modelRunId_,
				const char* oldContainer_,
				const char* newContainer_)
	{
		auditId			= auditId_;
		adminIntId		= adminIntId_;
		when			= when_;
		action			= action_;
		batchType		= batchType_;
		batchId			= batchId_;
		userIntId		= userIntId_;
		attrType		= attrType_;
		oldAttrValue	= oldAttrValue_;
		newAttrValue	= newAttrValue_ ;
		modelRunId		= modelRunId_;
		oldContainer	= oldContainer_;
		newContainer	= newContainer_;

		composeOlapLogMessage();
	}
};

#endif // etcdbmBDUAprepstmt_h_included

