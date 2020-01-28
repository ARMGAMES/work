// Copyright (C) 2012 PYR Software Ltd. All rights reserved.

#include "genRule.h"

//******************************************************************************
// CLPGByteCodeBlock definitions
//******************************************************************************
// Constructor
//******************************************************************************
CLPGByteCodeBlock::CLPGByteCodeBlock( CLPGByteCodeBlock* _owner, CLPGByteCodeGen* _gen ) 
:	owner(_owner)
,	gen(_gen)
,	pdata(0)
{
	UINT64* p = gen->allocCode(pdata);
	*p = CLPG_BC_GEN_BLOCK | CLPG_BC_BLK_BLOCK_BEGIN;
	if(NULL != owner)
		*p += owner->getPData() & CLPG_BC_BLK_ADDRMASK;
}

//******************************************************************************
// Block type
//******************************************************************************
const UINT64 CLPGByteCodeBlock::getType( void ) const
{
	return gen->getFirstData(pdata, CLPG_BC_BLK_TYPEMASK);
}

//******************************************************************************
// Address data
//******************************************************************************
const UINT64 CLPGByteCodeBlock::getAddrData( void ) const
{
	return gen->getFirstData(pdata, CLPG_BC_BLK_ADDRMASK);
}

//******************************************************************************
// Size
//******************************************************************************
const UINT64 CLPGByteCodeBlock::getSize( void ) const
{
	return gen->getFirstData(pdata, CLPG_BC_BLK_SIZEMASK);
}

//******************************************************************************
// End block set
//******************************************************************************
void CLPGByteCodeBlock::setEndBlock( void )
{
	UINT64 d;
	UINT64* p = gen->allocCode(d);
	UINT64 size = (((d - pdata) / sizeof(UINT64))<<32) & CLPG_BC_BLK_SIZEMASK;
	gen->setFirstData(pdata, size, CLPG_BC_BLK_SIZEMASK);
	*p = CLPG_BC_GEN_BLOCK | CLPG_BC_BLK_BLOCK_END | size | pdata;
}

//******************************************************************************
// Find var
//******************************************************************************
CLPGByteCodeVarData* CLPGByteCodeBlock::findVar( const char* varName )
{
	CLPGByteCodeVarData* obj = NULL;
	//PStringMap<CLPGByteCodeVarData*>::iterator iter;

	auto iter = vars.find(varName);
	if(iter != vars.end())
		obj = &(iter->second);
	else if(nullptr != owner)
		obj = owner->findVar(varName);

	return obj;
}

//******************************************************************************
// Create data object
//******************************************************************************
void CLPGByteCodeBlock::createDataObject(const char* type, const char* name, bool isConst, bool init)
{
	UINT64 addr, dataamount, data, first;
	UINT64* p;

	auto iter = vars.find(name);
	if(iter != vars.end())
	{
		gen->setError();
		WriteLog(string("Variable '").append(name).append("' multiple definition\n"));
	}
	else
	{
		gen->setCommand(CLPG_BC_CMD_DATA_ALLOC);
		p = gen->allocCode(addr, 2);

		first = gen->makeFirst(type, dataamount, isConst);
		*p = first;
		data = gen->reserveData(dataamount);
		*(p + 1) = data;

		auto obj = &(vars.insert(make_pair(name, CLPGByteCodeVarData(first, data, name))).first->second);

		if(init)
		{
			if(CLPG_BC_VAR_OBJECT == (first & CLPG_BC_VAR_TYPEMASK))
			{
				gen->setCommand(CLPG_BC_CMD_OBJINIT);
				gen->setVarRef(obj);
				p = gen->allocCode(addr, 1);
				*p = gen->getConstString(type, false);
			}
			else
				WriteLog(string("Variable '").append(name).append("' init ignored - only for objects\n"));
		}
	}
}

//******************************************************************************
//  +/- sign, put simple operand in stack
//******************************************************************************
UINT64 CLPGByteCodeBlock::setOperand(const char* varName, bool minus, UINT64 t)
{
	UINT64 ret = CLPG_BC_GEN_UNDEF;
	CLPGByteCodeVarData* obj;
	UINT64 d, buf;
	UINT64* p;

	if(!varName)
		throw exception("Null pointer in 'setOperand' varName, class'CLPGByteCodeGen'");

	switch (t)
	{
	case CLPG_BC_GEN_OP_VAR:
		obj = findVar(varName);
		if(NULL == obj)
		{
			gen->setError();
			WriteLog(string("Variable '").append(varName).append("' not found \n"));
		}
		else
		{
			gen->setCommand(CLPG_BC_CMD_COPY_DATA);
			p = gen->setVarRef(obj);
			if(minus)
			{
				if(CLPG_BC_VAR_INT == obj->getType())
					gen->setCommand(CLPG_BC_CMD_SIGNMINUS);
				else
					WriteLog(string("Variable '").append(varName).append(" '-' sign ignored\n"));
			}
			ret = obj->getFirst();
		}
		break;
	case CLPG_BC_GEN_OP_STR:
		gen->setCommand(CLPG_BC_CMD_COPY_DATA);
		p = gen->allocCode(d, 2);
		ret = *p = CLPG_BC_GEN_CONST | CLPG_BC_VAR_STRING | CLPG_BC_SET_DATAAMOUNT(1);
		*(p + 1) = gen->getConstString(varName, true);
		if(minus)
			WriteLog(string("Variable '").append(varName).append(" '-' sign ignored\n"));
		break;
	case CLPG_BC_GEN_OP_INT:
		gen->setCommand(CLPG_BC_CMD_COPY_DATA);
		p = gen->allocCode(d, 2);
		*p = CLPG_BC_GEN_CONST | CLPG_BC_VAR_INT | CLPG_BC_SET_DATAAMOUNT(1);;
		buf = _atoi64(varName);
		if(buf < 0x7F)
			*p |= CLPG_BC_VAR_INT8;
		else if((buf < 0xFF) && (!minus))
			*p |= CLPG_BC_VAR_BYTE;
		else if(buf < 0x7FFF)
			*p |= CLPG_BC_VAR_INT16;
		else if((buf < 0xFFFF) && (!minus))
			*p |= CLPG_BC_VAR_UINT16;
		else if(buf < 0x7FFFFFFF)
			*p |= CLPG_BC_VAR_INT32;
		else if((buf < 0xFFFFFFFF) && (!minus))
			*p |= CLPG_BC_VAR_UINT32;
		else if(buf < 0x7FFFFFFFFFFFFFFF)
			*p |= CLPG_BC_VAR_INT64;
		else if((buf < 0xFFFFFFFFFFFFFFFF) && (!minus))
			*p |= CLPG_BC_VAR_UINT64;
		else
		{
			gen->setError();
			WriteLog(string("Variable '").append(varName).append("' overload\n"));
		}
		ret = *p;
		if(minus)
			buf = ~buf + 1;
		*(p + 1) = buf;
		break;
	case CLPG_BC_GEN_OP_CHAR:
		gen->setCommand(CLPG_BC_CMD_COPY_DATA);
		p = gen->allocCode(d, 2);
		ret = *p = CLPG_BC_GEN_CONST | CLPG_BC_VAR_INT | CLPG_BC_VAR_INT8 | CLPG_BC_SET_DATAAMOUNT(1);
		buf = *(varName+1);
		if(minus)
			buf = ~buf + 1;
		*(p + 1) = buf;
		break;
	case CLPG_BC_GEN_OP_NULL:
		gen->setCommand(CLPG_BC_CMD_COPY_DATA);
		p = gen->allocCode(d, 2);
		ret = *p = CLPG_BC_GEN_CONST | CLPG_BC_VAR_OBJECT | CLPG_BC_VAR_OBJ_NULL | CLPG_BC_SET_DATAAMOUNT(1);
		*(p + 1) = 0;
		if(minus)
			WriteLog(string("Variable '").append(varName).append(" '-' sign ignored\n"));
		break;
	case CLPG_BC_GEN_OP_TRUE:
		gen->setCommand(CLPG_BC_CMD_COPY_DATA);
		p = gen->allocCode(d, 2);
		ret = *p = CLPG_BC_GEN_CONST | CLPG_BC_VAR_BOOL | CLPG_BC_SET_DATAAMOUNT(1);
		*(p + 1) = CLPG_BC_VAR_BOOL_TRUE;
		if(minus)
			WriteLog(string("Variable '").append(varName).append(" '-' sign ignored\n"));
		break;
	case CLPG_BC_GEN_OP_FALSE:
		gen->setCommand(CLPG_BC_CMD_COPY_DATA);
		p = gen->allocCode(d, 2);
		ret = *p = CLPG_BC_GEN_CONST | CLPG_BC_VAR_BOOL | CLPG_BC_SET_DATAAMOUNT(1);
		*(p + 1) = CLPG_BC_VAR_BOOL_FALSE;
		if(minus)
			WriteLog(string("Variable '").append(varName).append(" '-' sign ignored\n"));
		break;
	default:
		gen->setError();
		WriteLog(string("Variable '").append(varName).append("' type not found"));
		break;
	}
	return ret;
}

//******************************************************************************
// ++ 
//******************************************************************************
UINT64 CLPGByteCodeBlock::plusPlus(const char* varName, bool prefix)
{
	if(!varName)
		throw exception("Null pointer in 'plusPlus' varName, class'CLPGByteCodeGen'");

	UINT64 ret = CLPG_BC_GEN_UNDEF;
	CLPGByteCodeVarData* obj = findVar(varName);

	if(NULL == obj)
	{
		gen->setError();
		WriteLog(string("Variable '").append(varName).append("' in '++' operation not found \n"));
	}
	else if(CLPG_BC_VAR_INT != obj->getType())
	{
		gen->setError();
		WriteLog(string("Variable '").append(varName).append("' - '++' operation supported only for integer value \n"));
	}
	else if(prefix)
	{
		gen->setCommand(CLPG_BC_CMD_COPY_DATA);
		gen->setVarRef(obj);
		gen->setCommand(CLPG_BC_CMD_PLUSPLUS);
		ret = obj->getFirst();
	}
	else
	{
		gen->setCommand(CLPG_BC_CMD_COPY_REF);
		gen->setVarRef(obj);
		gen->setCommand(CLPG_BC_CMD_COPY_DATA);
		gen->setVarRef(obj);
		gen->setCommand(CLPG_BC_CMD_PLUSPLUS);
		gen->setCommand(CLPG_BC_CMD_POP);
		ret = obj->getFirst();
	}
	return ret;
}

//******************************************************************************
// --
//******************************************************************************
UINT64 CLPGByteCodeBlock::minusMinus(const char* varName, bool prefix)
{
	if(!varName)
		throw exception("Null pointer in 'minusMinus' varName, class'CLPGByteCodeGen'");

	UINT64 ret = CLPG_BC_GEN_UNDEF;
	CLPGByteCodeVarData* obj = findVar(varName);

	if(NULL == obj)
	{
		gen->setError();
		WriteLog(string("Variable '").append(varName).append("' in '--' operation not found \n"));
	}
	else if(CLPG_BC_VAR_INT != obj->getType())
	{
		gen->setError();
		WriteLog(string("Variable '").append(varName).append("' - '--' operation supported only for integer value \n"));
	}
	else if(prefix)
	{
		gen->setCommand(CLPG_BC_CMD_COPY_DATA);
		gen->setVarRef(obj);
		gen->setCommand(CLPG_BC_CMD_MINUSMINUS);
		ret = obj->getFirst();
	}
	else
	{
		gen->setCommand(CLPG_BC_CMD_COPY_REF);
		gen->setVarRef(obj);
		gen->setCommand(CLPG_BC_CMD_COPY_DATA);
		gen->setVarRef(obj);
		gen->setCommand(CLPG_BC_CMD_MINUSMINUS);
		gen->setCommand(CLPG_BC_CMD_POP);
		ret = obj->getFirst();
	}
	return ret;
}

//******************************************************************************
// Assignment
//******************************************************************************
UINT64 CLPGByteCodeBlock::assignment(const char* varName, UINT64 t, bool useconst)
{
	if(!varName)
		throw exception("Null pointer in 'assignment' varName, class'CLPGByteCodeGen'");
	UINT64 ret = CLPG_BC_GEN_UNDEF;

	CLPGByteCodeVarData* obj = findVar(varName);

	if(NULL == obj)
	{
		gen->setError();
		WriteLog(string("Variable '").append(varName).append("' in assignment operation not correct \n"));
	}
	else if(obj->isConst() && !useconst)
	{
		gen->setError();
		WriteLog(string("Variable '").append(varName).append("' - assignment operation cannot be done for const types \n"));
	}
	else if(((t & CLPG_BC_GEN_SORTMASK) != CLPG_BC_GEN_VAR) && ((t & CLPG_BC_GEN_SORTMASK) != CLPG_BC_GEN_CONST))
	{
		gen->setError();
		WriteLog(string("Variable '").append(varName).append("' - assignment operation supported only for variables and constants \n"));
	}
	else if((t & CLPG_BC_VAR_TYPEMASK) != obj->getType())
	{
		gen->setError();
		WriteLog(string("Variable '").append(varName).append("' - assignment operation supported only for same types \n"));
	}
	else if((CLPG_BC_VAR_OBJECT == obj->getType()) && ((t & CLPG_BC_VAR_SUBTYPEMASK) > CLPG_BC_VAR_OBJ_BASECLASS) && (((t & CLPG_BC_VAR_SUBTYPEMASK) != obj->getSubtype()) || ((t & CLPG_BC_VAR_EXTERNAL) != obj->isExternal())))
	{
		gen->setError();
		WriteLog(string("Variable '").append(varName).append("' - assignment operation supported only for same types \n"));
	}
	else
	{
		if((CLPG_BC_VAR_INT == obj->getType()) && ((t & CLPG_BC_VAR_SUBTYPEMASK) > obj->getSubtype()))
			WriteLog(string("Integer '").append(varName).append("' initialized by bigger size - may lose data\n"));
		gen->setCommand(CLPG_BC_CMD_ASSIGNMENT);
		gen->setVarRef(obj);
		ret = obj->getFirst();
	}
	return ret;
}

//******************************************************************************
// Fields
//******************************************************************************
UINT64 CLPGByteCodeBlock::firstField(const char* varName, const char* fieldName)
{
	if(!varName)
		throw exception("Null pointer in 'firstField' varName, class'CLPGByteCodeGen'");
	if(!fieldName)
		throw exception("Null pointer in 'firstField' fieldName, class'CLPGByteCodeGen'");

	UINT64* p;
	UINT64 d, idx, dataamount;
	const CGstruct* cls;
	string clstype, fieldtype, subtype;
	UINT64 ret = CLPG_BC_GEN_UNDEF;
	CGmain* mainObj = gen->getMainDoc();
	const CGfield* fldOut = nullptr;
	CLPGByteCodeVarData* obj = findVar(varName);

	if(NULL == obj)
	{
		gen->setError();
		WriteLog(string("Object '").append(varName).append("' in get field operation not found \n"));
	}
	else if((CLPG_BC_VAR_OBJECT != obj->getType()) || (obj->isExternal() != 0))
	{
		gen->setError();
		WriteLog(string("Variable '").append(varName).append(".").append(fieldName).append("' - get field operation supported only for registered classes \n"));
	}
	else
	{
		idx = obj->getSubtype();
		cls = mainObj->findClass(idx);
		if(NULL != cls)
		{
			clstype = cls->getName();
			fldOut = dynamic_cast<const CGfield* >(cls->findObject(objTypes::gTypeField, fieldName));
			if(NULL != fldOut)
			{
				fieldtype = fldOut->getFieldType()->getFieldTypeStr().c_str();
				gen->setCommand(CLPG_BC_CMD_COPY_DATA);
				gen->setVarRef(obj);
				gen->setCommand(CLPG_BC_CMD_OBJFIELD);
				p = gen->allocCode(d, 3);
				*p = gen->getConstString(clstype.c_str(), false);
				*(p + 1) = gen->getConstString(fieldName, false);
				*(p + 2) = ret = gen->makeFirst(fieldtype.c_str(), dataamount, false);
			}
			else
			{
				gen->setError();
				WriteLog(string("Field '").append(fieldName).append("' in class '").append(clstype).append("' - not found \n"));
			}
		}
		else
		{
			gen->setError();
			WriteLog(string("Class for '").append(fieldName).append("' field - not found \n"));
		}
	}

	return ret;
}

//******************************************************************************
// Fields - index
//******************************************************************************
UINT64 CLPGByteCodeBlock::indexField(UINT64 index, const char* varName, const char* fieldName)
{
	if(!varName)
		throw exception("Null pointer in 'indexField' varName, class'CLPGByteCodeGen'");
	if(!fieldName)
		throw exception("Null pointer in 'indexField' fieldName, class'CLPGByteCodeGen'");

	UINT64* p;
	UINT64 d, idx, dataamount;
	const CGstruct* cls;
	string clstype, fieldtype, subtype;
	UINT64 ret = CLPG_BC_GEN_UNDEF;
	CGmain* mainObj = gen->getMainDoc();
	const CGfield* fldOut = nullptr;
	CLPGByteCodeVarData* obj = findVar(varName);

	if((CLPG_BC_GEN_VAR != (index & CLPG_BC_GEN_SORTMASK)) && (CLPG_BC_GEN_CONST != (index & CLPG_BC_GEN_SORTMASK)))
	{
		gen->setError();
		WriteLog("Index field operation supported only for constant and variable indexes\n");
	}
	else if(CLPG_BC_VAR_INT != (index & CLPG_BC_VAR_TYPEMASK))
	{
		gen->setError();
		WriteLog("Index field operation supported only for integer indexes\n");
	}
	else
	{
		idx = obj->getSubtype();
		cls = mainObj->findClass(idx);
		if(NULL != cls)
		{
			//cls = dynamic_cast<const CGstruct* >(mainObj->findObject(objTypes::gTypeStruct, clstype.c_str()));
			clstype = cls->getName().c_str();
			fldOut = dynamic_cast<const CGfield* >(cls->findObject(objTypes::gTypeField, fieldName));
			if (fldOut->getFieldType()->getZeroSubType())
				subtype = fldOut->getFieldType()->getZeroSubType()->getFieldTypeStr().c_str();
			//if(fldOut->getFieldType()->getSubTypes().size())
			//	subtype = fldOut->getFieldType()->getSubTypes()[0]->getFieldTypeStr().c_str();
			if(fldOut->getFieldType()->isDirectIndex())
			{
				gen->setCommand(CLPG_BC_CMD_OBJFIELDIDX);
				p = gen->allocCode(d, 3);
				*p = gen->getConstString(clstype.c_str(), false);
				*(p + 1) = gen->getConstString(fieldName, false);
				*(p + 2) = ret = gen->makeFirst(subtype.c_str(), dataamount, false);
			}
			else
			{
				gen->setError();
				WriteLog(string("Class '").append(fieldtype).append("' - not container, index not valid \n"));
			}
		}
		else
		{
			gen->setError();
			WriteLog(string("Class for '").append(fieldName).append("' field - not found \n"));
		}
	}

	return ret;
}

//******************************************************************************
// Fields - size
//******************************************************************************
UINT64 CLPGByteCodeBlock::sizeField(const char* varName, const char* fieldName)
{
	if(!varName)
		throw exception("Null pointer in 'sizeField' varName, class'CLPGByteCodeGen'");
	if(!fieldName)
		throw exception("Null pointer in 'sizeField' fieldName, class'CLPGByteCodeGen'");

	UINT64* p;
	UINT64 d, idx, dataamount;
	const CGstruct* cls;
	string clstype, fieldtype, subtype;
	UINT64 ret = CLPG_BC_GEN_UNDEF;
	CGmain* mainObj = gen->getMainDoc();
	const CGfield* fldOut = nullptr;
	CLPGByteCodeVarData* obj = findVar(varName);

	if(NULL == obj)
	{
		gen->setError();
		WriteLog(string("Object '").append(varName).append("' in get field operation not found \n"));
	}
	else if((CLPG_BC_VAR_OBJECT != obj->getType()) || (obj->isExternal() != 0))
	{
		gen->setError();
		WriteLog(string("Variable '").append(varName).append(".").append(fieldName).append("' - get field operation supported only for registered classes \n"));
	}
	else
	{
		idx = obj->getSubtype();
		cls = mainObj->findClass(idx);
		if(NULL != cls)
		{
			clstype = cls->getName().c_str();
			fldOut = dynamic_cast<const CGfield* >(cls->findObject(objTypes::gTypeField, fieldName));
			if(fldOut->getFieldType()->isDirectIndex())
			{
				fieldtype = fldOut->getFieldType()->getFieldTypeStr().c_str();

				gen->setCommand(CLPG_BC_CMD_OBJFIELDSIZE);
				p = gen->allocCode(d, 3);
				*p = gen->getConstString(clstype.c_str(), false);
				*(p + 1) = gen->getConstString(fieldName, false);
				*(p + 2) = ret = gen->makeFirst("UINT64", dataamount, false);
			}
			else
			{
				gen->setError();
				WriteLog(string("Class '").append(fieldtype).append("' - not container, size not valid \n"));
			}
		}
		else
		{
			gen->setError();
			WriteLog(string("Class for '").append(fieldName).append("' field - not found \n"));
		}
	}

	return ret;
}

//******************************************************************************
// Fields - next
//******************************************************************************
UINT64 CLPGByteCodeBlock::nextField(const char* fieldName, UINT64 t)
{
	if(!fieldName)
		throw exception("Null pointer in 'nextField' fieldName, class'CLPGByteCodeGen'");

	UINT64* p;
	UINT64 d, dataamount;
	const CGstruct* cls;
	string clstype, fieldtype, subtype;
	UINT64 ret = CLPG_BC_GEN_UNDEF;
	CGmain* mainObj = gen->getMainDoc();
	const CGfield* fldOut = nullptr;

	if((CLPG_BC_VAR_OBJECT != (t & CLPG_BC_VAR_TYPEMASK)) || ((t & CLPG_BC_VAR_EXTERNAL) != 0))
	{
		gen->setError();
		WriteLog(string("Variable '").append(fieldName).append("' - get field operation supported only for registered classes \n"));
	}
	else
	{
		cls = mainObj->findClass((t & CLPG_BC_VAR_SUBTYPEMASK));
		if(NULL != cls)
		{
			clstype = cls->getName().c_str();
			fldOut = dynamic_cast<const CGfield* >(cls->findObject(objTypes::gTypeField, fieldName));
			if(NULL != fldOut)
			{
				fieldtype = fldOut->getFieldType()->getFieldTypeStr().c_str();

				gen->setCommand(CLPG_BC_CMD_OBJFIELD);
				p = gen->allocCode(d, 3);
				*p = gen->getConstString(clstype.c_str(), false);
				*(p + 1) = gen->getConstString(fieldName, false);
				*(p + 2) = ret = gen->makeFirst(fieldtype.c_str(), dataamount, false);
			}
			else
			{
				gen->setError();
				WriteLog(string("Field '").append(fieldName).append("' in class '").append(clstype).append("' - not found \n"));
			}
		}
		else
		{
			gen->setError();
			WriteLog(string("Class for '").append(fieldName).append("' field - not found \n"));
		}
	}

	return ret;
}

//******************************************************************************
// Fields - index
//******************************************************************************
UINT64 CLPGByteCodeBlock::indexFieldNext(UINT64 index, const char* fieldName, UINT64 t)
{
	if(!fieldName)
		throw exception("Null pointer in 'indexFieldNext' fieldName, class'CLPGByteCodeGen'");

	UINT64* p;
	UINT64 d, dataamount;
	const CGstruct* cls;
	string clstype, fieldtype, subtype;
	UINT64 ret = CLPG_BC_GEN_UNDEF;
	CGmain* mainObj = gen->getMainDoc();
	const CGfield* fldOut = nullptr;

	if((CLPG_BC_GEN_VAR != (index & CLPG_BC_GEN_SORTMASK)) && (CLPG_BC_GEN_CONST != (index & CLPG_BC_GEN_SORTMASK)))
	{
		gen->setError();
		WriteLog("Index field operation supported only for constant and variable indexes\n");
	}
	else if(CLPG_BC_VAR_INT != (index & CLPG_BC_VAR_TYPEMASK))
	{
		gen->setError();
		WriteLog("Index field operation supported only for integer indexes\n");
	}
	else
	{
		cls = mainObj->findClass((t & CLPG_BC_VAR_SUBTYPEMASK));
		if(NULL != cls)
		{
			clstype = cls->getName().c_str();
			fldOut = dynamic_cast<const CGfield* >(cls->findObject(objTypes::gTypeField, fieldName));
			fieldtype = fldOut->getFieldType()->getFieldTypeStr().c_str();
			if (fldOut->getFieldType()->getZeroSubType())
				subtype = fldOut->getFieldType()->getZeroSubType()->getFieldTypeStr().c_str();
			//if(fldOut->getFieldType()->getSubTypes().size())
			//	subtype = fldOut->getFieldType()->getSubTypes()[0]->getFieldTypeStr().c_str();
			if(fldOut->getFieldType()->isDirectIndex())
			{

				gen->setCommand(CLPG_BC_CMD_OBJFIELDIDX);
				p = gen->allocCode(d, 3);
				*p = gen->getConstString(clstype.c_str(), false);
				*(p + 1) = gen->getConstString(fieldName, false);
				*(p + 2) = ret = gen->makeFirst(subtype.c_str(), dataamount, false);
			}
			else
			{
				gen->setError();
				WriteLog(string("Class '").append(fieldtype).append("' - not container, index not valid \n"));
			}
		}
		else
		{
			gen->setError();
			WriteLog(string("Class for '").append(fieldName).append("' field - not found \n"));
		}
	}

	return ret;
}

//******************************************************************************
// Fields - size
//******************************************************************************
UINT64 CLPGByteCodeBlock::sizeFieldNext(const char* fieldName, UINT64 t)
{
	if(!fieldName)
		throw exception("Null pointer in 'sizeFieldNext' fieldName, class'CLPGByteCodeGen'");

	UINT64* p;
	UINT64 d, dataamount;
	const CGstruct* cls;
	string clstype, fieldtype, subtype;
	UINT64 ret = CLPG_BC_GEN_UNDEF;
	CGmain* mainObj = gen->getMainDoc();
	const CGfield* fldOut = nullptr;

	if((CLPG_BC_VAR_OBJECT != (t & CLPG_BC_VAR_TYPEMASK)) || ((t & CLPG_BC_VAR_EXTERNAL) != 0))
	{
		gen->setError();
		WriteLog(string("Variable '").append(fieldName).append("' - get field operation supported only for registered classes \n"));
	}
	else
	{
		cls = mainObj->findClass((t & CLPG_BC_VAR_SUBTYPEMASK));
		if(NULL != cls)
		{
			clstype = cls->getName().c_str();
			fldOut = dynamic_cast<const CGfield* >(cls->findObject(objTypes::gTypeField, fieldName));
			fieldtype = fldOut->getFieldType()->getFieldTypeStr().c_str();
			if(fldOut->getFieldType()->isDirectIndex())
			{

				gen->setCommand(CLPG_BC_CMD_OBJFIELDSIZE);
				p = gen->allocCode(d, 3);
				*p = gen->getConstString(clstype.c_str(), false);
				*(p + 1) = gen->getConstString(fieldName, false);
				*(p + 2) = ret = gen->makeFirst("UINT64", dataamount, false);
			}
			else
			{
				gen->setError();
				WriteLog(string("Class '").append(fieldtype).append("' - not container, index not valid \n"));
			}
		}
		else
		{
			gen->setError();
			WriteLog(string("Class for '").append(fieldName).append("' field - not found \n"));
		}
	}

	return ret;
}

//******************************************************************************
// +, -, *, /, &, ^, | 
//******************************************************************************
UINT64 CLPGByteCodeBlock::binaryMath(UINT64 t1, UINT64 t2, UINT64 code)
{
	UINT64 ret = CLPG_BC_GEN_UNDEF;

	if((((t1 & CLPG_BC_GEN_SORTMASK) != CLPG_BC_GEN_VAR) && ((t1 & CLPG_BC_GEN_SORTMASK) != CLPG_BC_GEN_CONST)) || (((t2 & CLPG_BC_GEN_SORTMASK) != CLPG_BC_GEN_VAR) && ((t2 & CLPG_BC_GEN_SORTMASK) != CLPG_BC_GEN_CONST)))
	{
		gen->setError();
		WriteLog("Math operation supported only for variables and constants \n");
	}
	else if(((t1 & CLPG_BC_VAR_TYPEMASK) == CLPG_BC_VAR_STRING) && ((t2 & CLPG_BC_VAR_TYPEMASK) == CLPG_BC_VAR_STRING))
	{
		gen->setCommand(CLPG_BC_CMD_STRPLUS);
		ret = CLPG_BC_GEN_VAR | CLPG_BC_VAR_STRING | CLPG_BC_SET_DATAAMOUNT(1) | CLPG_BC_VAR_EXTERNAL;
	}
	else if(((t1 & CLPG_BC_VAR_TYPEMASK) == CLPG_BC_VAR_INT) && ((t2 & CLPG_BC_VAR_TYPEMASK) == CLPG_BC_VAR_INT))
	{
		switch (code)
		{
		case 1:
			gen->setCommand(CLPG_BC_CMD_PLUS);
			break;
		case 2:
			gen->setCommand(CLPG_BC_CMD_MINUS);
			break;
		case 3:
			gen->setCommand(CLPG_BC_CMD_MUL);
			break;
		case 4:
			gen->setCommand(CLPG_BC_CMD_DIV);
			break;
		case 5:
			gen->setCommand(CLPG_BC_CMD_BAND);
			break;
		case 6:
			gen->setCommand(CLPG_BC_CMD_BXOR);
			break;
		case 7:
			gen->setCommand(CLPG_BC_CMD_BOR);
			break;
		case 8:
			gen->setCommand(CLPG_BC_CMD_MOD);
			break;
		default:
			gen->setError();
			WriteLog("Binary math operation unrecognized \n");
			break;
		}

		if((t1 & CLPG_BC_VAR_SUBTYPEMASK) > (t2 & CLPG_BC_VAR_SUBTYPEMASK))
			ret = t1;
		else
			ret = t2;
		ret &= ~CLPG_BC_GEN_SORTMASK;
		ret |= CLPG_BC_GEN_CONST;
	}
	else
	{
		gen->setError();
		WriteLog("Math operation supported only for integer types or  two strings for addition \n");
	}
	return ret;
}

//******************************************************************************
// ==, !=, >, <, >=, <=
//******************************************************************************
UINT64 CLPGByteCodeBlock::binaryCompare( UINT64 t1, UINT64 t2, UINT64 code )
{
	UINT64 ret = CLPG_BC_GEN_UNDEF;

	if((((t1 & CLPG_BC_GEN_SORTMASK) != CLPG_BC_GEN_VAR) && ((t1 & CLPG_BC_GEN_SORTMASK) != CLPG_BC_GEN_CONST)) || (((t2 & CLPG_BC_GEN_SORTMASK) != CLPG_BC_GEN_VAR) && ((t2 & CLPG_BC_GEN_SORTMASK) != CLPG_BC_GEN_CONST)))
	{
		gen->setError();
		WriteLog("Conditional operation supported only for variables and constants \n");
	}
	else if(((t1 & CLPG_BC_VAR_TYPEMASK) == CLPG_BC_VAR_STRING) && ((t2 & CLPG_BC_VAR_TYPEMASK) == CLPG_BC_VAR_STRING))
	{
		switch (code)
		{
		case 1:
			gen->setCommand(CLPG_BC_CMD_STREQ);
			break;
		case 2:
			gen->setCommand(CLPG_BC_CMD_STRNOTEQ);
			break;
		default:
			gen->setError();
			WriteLog("Binary logical operation for strings supports only equal/not equal \n");
			break;
		}

		ret = CLPG_BC_GEN_CONST | CLPG_BC_VAR_BOOL | CLPG_BC_SET_DATAAMOUNT(1);
	}
	else if(((t1 & CLPG_BC_VAR_TYPEMASK) == CLPG_BC_VAR_OBJECT) && ((t2 & CLPG_BC_VAR_TYPEMASK) == CLPG_BC_VAR_OBJECT))
	{
		if(((t1 & CLPG_BC_VAR_SUBTYPEMASK) == (t2 & CLPG_BC_VAR_SUBTYPEMASK)) || ((t1 & CLPG_BC_VAR_SUBTYPEMASK) == CLPG_BC_VAR_OBJ_NULL) || ((t2 & CLPG_BC_VAR_SUBTYPEMASK) == CLPG_BC_VAR_OBJ_NULL))
		{
			switch (code)
			{
			case 1:
				gen->setCommand(CLPG_BC_CMD_OBJEQ);
				break;
			case 2:
				gen->setCommand(CLPG_BC_CMD_OBJNOTEQ);
				break;
			default:
				gen->setError();
				WriteLog("Binary logical operation for objects supports only equal/not equal \n");
				break;
			}

			ret = CLPG_BC_GEN_CONST | CLPG_BC_VAR_BOOL | CLPG_BC_SET_DATAAMOUNT(1);
		}
	}
	else if(((t1 & CLPG_BC_VAR_TYPEMASK) == CLPG_BC_VAR_INT) && ((t2 & CLPG_BC_VAR_TYPEMASK) == CLPG_BC_VAR_INT))
	{
		switch (code)
		{
		case 1:
			gen->setCommand(CLPG_BC_CMD_EQ);
			break;
		case 2:
			gen->setCommand(CLPG_BC_CMD_NOTEQ);
			break;
		case 3:
			gen->setCommand(CLPG_BC_CMD_MORE);
			break;
		case 4:
			gen->setCommand(CLPG_BC_CMD_MOREEQ);
			break;
		case 5:
			gen->setCommand(CLPG_BC_CMD_LESS);
			break;
		case 6:
			gen->setCommand(CLPG_BC_CMD_LESSEQ);
			break;
		default:
			gen->setError();
			WriteLog("Binary logical operation unrecognized \n");
			break;
		}

		ret = CLPG_BC_GEN_CONST | CLPG_BC_VAR_BOOL | CLPG_BC_SET_DATAAMOUNT(1);
	}
	else
	{
		gen->setError();
		WriteLog("Conditional operation supported only for integer types \n");
	}
	return ret;
}

//******************************************************************************
// &&, ||
//******************************************************************************
UINT64 CLPGByteCodeBlock::binaryLogic( UINT64 t1, UINT64 t2, UINT64 code )
{
	UINT64 ret = CLPG_BC_GEN_UNDEF;

	if((((t1 & CLPG_BC_GEN_SORTMASK) != CLPG_BC_GEN_VAR) && ((t1 & CLPG_BC_GEN_SORTMASK) != CLPG_BC_GEN_CONST)) || (((t2 & CLPG_BC_GEN_SORTMASK) != CLPG_BC_GEN_VAR) && ((t2 & CLPG_BC_GEN_SORTMASK) != CLPG_BC_GEN_CONST)))
	{
		gen->setError();
		WriteLog("Logical operation supported only for variables and constants \n");
	}
	else if(((t1 & CLPG_BC_VAR_TYPEMASK) != CLPG_BC_VAR_BOOL) || ((t2 & CLPG_BC_VAR_TYPEMASK) != CLPG_BC_VAR_BOOL))
	{
		gen->setError();
		WriteLog("Logical operation supported only for boolean types \n");
	}
	else
	{
		switch (code)
		{
		case 1:
			gen->setCommand(CLPG_BC_CMD_AND);
			break;
		case 2:
			gen->setCommand(CLPG_BC_CMD_OR);
			break;
		default:
			gen->setError();
			WriteLog("Logical operation unrecognized \n");
			break;
		}

		ret = CLPG_BC_GEN_CONST | CLPG_BC_VAR_BOOL | CLPG_BC_SET_DATAAMOUNT(1);
	}
	return ret;
}


