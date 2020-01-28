////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2019 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef PdServerObject_h_included
#define PdServerObject_h_included

////////////////////////////////////////////////////////////////////////////////

#include "../securitytools/Generic/GenericSrvObject.h"

static constexpr const char* PDSERVER_CONFIG_FILE_NAME = "pdserver.ini";

////////////////////////////////////////////////////////////////////////////////

class PDServerObject : public GenericServerWithLoginAuthObjectBase
{
	using Parent_ = GenericServerWithLoginAuthObjectBase;

public:
	explicit PDServerObject(_CommInterface& inter_);
	virtual ~PDServerObject() { }

	virtual const char* getConfigFileName() const override;

protected:
	// "Must" overrides
	virtual const char* getLoginAuthDbmConnType() const override;
};

////////////////////////////////////////////////////////////////////////////////

class PDServerObjectFactory : public CommServerObjectFactory
{
public:
	/* new */ CommServerObject* createServerObject(_CommInterface& inter) const override
	{
		return new PDServerObject(inter);
	}
};

////////////////////////////////////////////////////////////////////////////////

#endif // PdServerObject_h_included
