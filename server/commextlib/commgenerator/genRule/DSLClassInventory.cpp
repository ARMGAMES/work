// Copyright (C) 2012 PYR Software Ltd. All rights reserved.

#include "commlib.h"

//***********************************************************************
// This file contains definition of functions for an interpreter support
//***********************************************************************
// WARNING!!! All class field operations are valid ONLY for PUBLIC fields.
//***********************************************************************

// Function typedefs
typedef void* (*getIndexValueType)(void*, UINT32);
typedef UINT32 (*getSizeType)(const void*);

// Vector functions
template<class T>
inline static void* getIndexVectorTempl(void* obj, UINT32 idx)
{
	vector<T>* p = reinterpret_cast<vector<T>*>(obj);
	if((NULL != p) && (idx < p->size()))
	{
		return &(p->at(idx));
	}
	else
		return NULL;
}

template<class T>
inline static UINT32 getSizeVectorTempl(const void* obj)
{
	if(NULL != obj)
		return static_cast<UINT32>(reinterpret_cast<const vector<T>*>(obj)->size());
	else
		return 0;
}

// Data structure for a class field
struct ClassesOffsetDataType
{
	ClassesOffsetDataType(const char* _fullfieldtype, const char* _fieldtype, const char* _fieldsubtype, UINT32 _offset, getIndexValueType _getIndexValue, getSizeType _getSize)
	:	fullfieldtype(_fullfieldtype)
	,	fieldtype(_fieldtype)
	,	fieldsubtype(_fieldsubtype)
	,	offset(_offset)
	,	getIndexValue(_getIndexValue)
	,	getSize(_getSize)
	{}
	const char*			fullfieldtype;
	const char*			fieldtype;
	const char*			fieldsubtype;
	UINT32				offset;
	getIndexValueType	getIndexValue;
	getSizeType			getSize;
};

// Main container with information
static PStringMap<PStringMap<ClassesOffsetDataType>*> classesOffsets;

// Find field - helper function, DON'T CALL directly
inline ClassesOffsetDataType* __CLPG_getField(const char* classname, const char* fieldname)
{
	ClassesOffsetDataType* codt = NULL;
	PStringMap<ClassesOffsetDataType>::iterator iter;
	PStringMap<PStringMap<ClassesOffsetDataType>*>::iterator iterMap;
	iterMap = classesOffsets.find(classname);
	if(iterMap != classesOffsets.end())
	{
		iter = iterMap->second->find(fieldname);
		if(iter != iterMap->second->end())
		{
			codt = &(iter->second);
		}
	}
	return codt;
}

// Get class field info. Returns true if the field was found and the result is valid.
// 'UINT32& offset' is this field offset, 'PString& fieldtype' is this field type
// 'PString& fieldsubtype' is this field subtype - container template type, 'PString& fullfieldtype' is this field type as it's printed in output file
bool getFieldInfo(UINT32& offset, PString& fieldtype, PString& fieldsubtype, PString& fullfieldtype, const char* classname, const char* fieldname)
{
	ClassesOffsetDataType* codt = __CLPG_getField(classname, fieldname);
	if(NULL != codt)
	{
		offset = codt->offset;
		fieldtype.assign(codt->fieldtype);
		fieldsubtype.assign(codt->fieldsubtype);
		fullfieldtype.assign(codt->fullfieldtype);
		return true;
	}
	else
		return false;
}

// Get class field offset. Returns true if the field was found and the result is valid.
bool getFieldOffset(UINT32& offset, const char* classname, const char* fieldname)
{
	ClassesOffsetDataType* codt = __CLPG_getField(classname, fieldname);
	if(NULL != codt)
	{
		offset = codt->offset;
		return true;
	}
	else
		return false;
}

// Returns class field direct pointer. NULL - if not found
void* getFieldAddress(void* obj, const char* classname, const char* fieldname)
{
	UINT32 offset;
	if((NULL != obj) && (getFieldOffset(offset, classname, fieldname)))
		return reinterpret_cast<BYTE*>(obj) + offset;
	else
		return NULL;
}

// Returns pointer to container element by index. NULL - if the index is out of range or this field is not container. 
// 'void* obj' is an input pointer for the object which field is used.
void*  getFieldContainerElement(void* obj, const char* classname, const char* fieldname, UINT32 index)
{
	ClassesOffsetDataType* codt = __CLPG_getField(classname, fieldname);
	if((NULL != codt) && (NULL != codt->getIndexValue))
	{
		return codt->getIndexValue(obj, index);
	}
	else
		return NULL;
}

// Get container size. Returns true if the field was found as container and the result is valid.
// 'void* obj' is an input pointer for the object which field is used.
bool getFieldContainerSize(UINT32& size, void* obj, const char* classname, const char* fieldname)
{
	ClassesOffsetDataType* codt = __CLPG_getField(classname, fieldname);
	if((NULL != codt) && (NULL != codt->getSize))
	{
		size = codt->getSize(obj);
		return true;
	}
	else
		return false;
}

// Vector functions for pointers, DON'T CALL directly
static void* getIndex_PTR(void* obj, unsigned __int32 idx)
{
	vector<void*>* p = reinterpret_cast<vector<void*>*>(obj);
	if((NULL != p) && (idx < p->size()))
	{
		return &(p->at(idx));
	}
	else
		return NULL;
}
static UINT32 getSize_PTR(const void* obj)
{
	if(NULL != obj)
		return static_cast<UINT32>(reinterpret_cast<const vector<void*>*>(obj)->size());
	else
		return 0;
}

// Vector helper functions, DON'T CALL directly
static void* getIndexVectorINT8(void* obj, UINT32 idx) {return getIndexVectorTempl<INT8>(obj, idx);}
static UINT32 getSizeVectorINT8(const void* obj) {return getSizeVectorTempl<INT8>(obj);}
static void* getIndexVectorBYTE(void* obj, UINT32 idx) {return getIndexVectorTempl<BYTE>(obj, idx);}
static UINT32 getSizeVectorBYTE(const void* obj) {return getSizeVectorTempl<BYTE>(obj);}
static void* getIndexVectorbool(void* obj, UINT32 idx) {return getIndexVectorTempl<bool>(obj, idx);}
static UINT32 getSizeVectorbool(const void* obj) {return  getSizeVectorTempl<bool>(obj);}
static void* getIndexVectorINT16(void* obj, UINT32 idx) {return getIndexVectorTempl<INT16>(obj, idx);}
static UINT32 getSizeVectorINT16(const void* obj) {return getSizeVectorTempl<INT16>(obj);}
static void* getIndexVectorUINT16(void* obj, UINT32 idx) {return getIndexVectorTempl<UINT16>(obj, idx);}
static UINT32 getSizeVectorUINT16(const void* obj) {return getSizeVectorTempl<UINT16>(obj);}
static void* getIndexVectorINT32(void* obj, UINT32 idx) {return getIndexVectorTempl<INT32>(obj, idx);}
static UINT32 getSizeVectorINT32(const void* obj) {return getSizeVectorTempl<INT32>(obj);}
static void* getIndexVectorUINT32(void* obj, UINT32 idx) {return getIndexVectorTempl<UINT32>(obj, idx);}
static UINT32 getSizeVectorUINT32(const void* obj) {return getSizeVectorTempl<UINT32>(obj);}
static void* getIndexVectorINT64(void* obj, UINT32 idx) {return getIndexVectorTempl<INT64>(obj, idx);}
static UINT32 getSizeVectorINT64(const void* obj) {return getSizeVectorTempl<INT64>(obj);}
static void* getIndexVectorUINT64(void* obj, UINT32 idx) {return getIndexVectorTempl<UINT64>(obj, idx);}
static UINT32 getSizeVectorUINT64(const void* obj) {return getSizeVectorTempl<UINT64>(obj);}
static void* getIndexVectorPString(void* obj, UINT32 idx) {return getIndexVectorTempl<PString>(obj, idx);}
static UINT32 getSizeVectorPString(const void* obj) {return getSizeVectorTempl<PString>(obj);}
static void* getIndexVectorCommMsgBody(void* obj, UINT32 idx) {return getIndexVectorTempl<CommMsgBody>(obj, idx);}
static UINT32 getSizeVectorCommMsgBody(const void* obj) {return getSizeVectorTempl<CommMsgBody>(obj);}
static void* getIndexVectorPBlock(void* obj, UINT32 idx) {return getIndexVectorTempl<PBlock>(obj, idx);}
static UINT32 getSizeVectorPBlock(const void* obj) {return getSizeVectorTempl<PBlock>(obj);}

// Initialization. Must be called ones at start
void initClassInventory(void)
{
	PStringMap<PStringMap<ClassesOffsetDataType>*>::iterator iterMap;
}
