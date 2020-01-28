// Copyright (C) 2012 PYR Software Ltd. All rights reserved.

#include "pplib.h"
#include "commlib.h"
#include "commextobject.h"

#include "DSLClassInventory.h"
#include "CLPGByteCodeInterpreter.h"

//******************************************************************************
// CLPGByteCodeInterpreter definitions
//******************************************************************************
// Init
//******************************************************************************
void CLPGByteCodeInterpreter::init(const BYTE* data, UINT64 datasize)
{
	UINT64 i, buf, blocks = 0, ver = 0, size;
	const UINT64* code;
	const BYTE* bytedata = data;
	const BYTE* p;

	if((*bytedata++ != 'b') || (*bytedata++ != 'c'))
		throw PError("Interpreter::Not byte-code block.");

	code = reinterpret_cast<const UINT64*>(bytedata);

	ver = *code++;
	if(ver > CLPG_BC_GEN_VERSION)
		PLog("Interpreter: Byte code has newer version than interpreter");

	blocks = *code++;

	size = 2 + (2 + 3 * blocks) * sizeof(UINT64);

	for(i = 0; i < blocks; ++i)
	{
		switch(*code++)
		{
		case 1:		// Code
			buf = *code++;
			CSize = buf/sizeof(UINT64);
			size += buf;
			p = data + *code++;
			CS = reinterpret_cast<const UINT64*>(p);
			break;
		case 2:		// Const strings
			buf = *code++;
			if(0 != buf)
			{
				size += buf;
				p = data + *code++;
				SS = reinterpret_cast<const char*>(p);
			}
			break;
		case 3:		// Data
			buf = *code++;
			if(0 != buf)
			{
				DS = reinterpret_cast<UINT64*>(dBlock.append(buf));
				memset(dBlock.ptr(), 0, dBlock.size());
				code++;
			}
			break;
		default:
			PLog("Interpreter: unsupported section code");
			code++;
			code++;
			break;
		}
		if(size > datasize)
			throw PError("Interpreter::Wrong input file size.");
	}

	if(NULL == CS)
		throw PError("Interpreter::No byte-code code section defined.");

	BS = IP = CS;
}

//******************************************************************************
// Clean data 
//******************************************************************************
void CLPGByteCodeInterpreter::clean( void )
{
	if(0 != dBlock.size())
	{
		UINT64 amount, size, i;
		size = dBlock.size() / sizeof(UINT64);
		for(i = 0; i < size; ++i)
		{
			amount = CLPG_BC_GET_DATAAMOUNT(*(DS + i));
			if((CLPG_BC_VAR_STRING == (*(DS + i) & CLPG_BC_VAR_TYPEMASK)) && (0 != (*(DS + i) & CLPG_BC_VAR_EXTERNAL)))
			{
				if(0 != *(DS + i + 1))
					delete reinterpret_cast<PString*>(*(DS + i + 1));
				*(DS + i) &= ~CLPG_BC_VAR_EXTERNAL;
			}
			i += amount;
		}
	}
}

//******************************************************************************
// Execute code
//******************************************************************************
RuleRetCode CLPGByteCodeInterpreter::exec(bool ignore, PString& errorMsg)
{
	UINT64 sort;
	const UINT64* edge = CS + CSize;

	try
	{
		while((IP < edge) && (*IP != CLPG_BC_CMD_STOP))
		{
			sort = *IP & CLPG_BC_GEN_SORTMASK;
			if(CLPG_BC_GEN_COMMAND == sort)
			{
				RuleRetCode r;
				r = command(errorMsg);
				if (r == RuleRetCode::ruleStop || ( r == RuleRetCode::ruleFailed && !ignore ))
				{
					clean();
					return r;
				}
			}
			else if(CLPG_BC_GEN_BLOCK == sort)
			{
				BS = IP++;
				// todo BS work
			}
			else
			{
				throw PError("Interpreter::Unsupported byte-code found.");
			}
		}
		if(IP >= edge)
			throw PError("Interpreter::Code outrange.");
	}
	catch( PError& err )
	{
		clean();
		throw err;
	}
	catch( ... )
	{
		clean();
		throw PError( "Interpreter::Unknown error. \n" );
	}
	return RuleRetCode::ruleOk;
}

//******************************************************************************
// Execute one command
//******************************************************************************
inline RuleRetCode CLPGByteCodeInterpreter::command(PString& errorMsg)
{
	RuleRetCode res = RuleRetCode::ruleOk;
	switch (*IP++)
	{
	case CLPG_BC_CMD_POP:
		commandPop();
		break;
	case CLPG_BC_CMD_CLEAR:
		commandClear();
		break;
	case CLPG_BC_CMD_COPY_DATA:
		commandCopyData();
		break;
	case CLPG_BC_CMD_COPY_REF:
		commandCopyRef();
		break;
	case CLPG_BC_CMD_DATA_ALLOC:
		commandDataAlloc();
		break;
	case CLPG_BC_CMD_PLUSPLUS:
		commandDoublePlusMinus(false);
		break;
	case CLPG_BC_CMD_MINUSMINUS:
		commandDoublePlusMinus(true);
		break;
	case CLPG_BC_CMD_SIGNMINUS:
		commandSignMinus();
		break;
	case CLPG_BC_CMD_ASSIGNMENT:
		commandAssignment();
		break;
	case CLPG_BC_CMD_OBJFIELD:
		commandObjField();
		break;
	case CLPG_BC_CMD_OBJFIELDIDX:
		commandObjFieldIdx();
		break;
	case CLPG_BC_CMD_OBJFIELDSIZE:
		commandObjFieldSize();
		break;
	case CLPG_BC_CMD_OBJINIT:
		commandObjInit();
		break;
	case CLPG_BC_CMD_PLUS:
		commandBinaryMath(1);
		break;
	case CLPG_BC_CMD_MINUS:
		commandBinaryMath(2);
		break;
	case CLPG_BC_CMD_MUL:
		commandBinaryMath(3);
		break;
	case CLPG_BC_CMD_DIV:
		commandBinaryMath(4);
		break;
	case CLPG_BC_CMD_BAND:
		commandBinaryMath(5);
		break;
	case CLPG_BC_CMD_BXOR:
		commandBinaryMath(6);
		break;
	case CLPG_BC_CMD_BOR:
		commandBinaryMath(7);
		break;
	case CLPG_BC_CMD_MOD:
		commandBinaryMath(8);
	case CLPG_BC_CMD_EQ:
		commandBinaryComp(1);
		break;
	case CLPG_BC_CMD_NOTEQ:
		commandBinaryComp(2);
		break;
	case CLPG_BC_CMD_MORE:
		commandBinaryComp(3);
		break;
	case CLPG_BC_CMD_MOREEQ:
		commandBinaryComp(4);
		break;
	case CLPG_BC_CMD_LESS:
		commandBinaryComp(5);
		break;
	case CLPG_BC_CMD_LESSEQ:
		commandBinaryComp(6);
		break;
	case CLPG_BC_CMD_AND:
		commandBinaryLogic(1);
		break;
	case CLPG_BC_CMD_OR:
		commandBinaryLogic(2);
		break;
	case CLPG_BC_CMD_GOTO:
		commandGoto(0);
		break;
	case CLPG_BC_CMD_GOTOFALSE:
		commandGoto(1);
		break;
	case CLPG_BC_CMD_GOTOTRUE:
		commandGoto(2);
		break;
	case CLPG_BC_CMD_STRPLUS:
		commandStringPlus();
		break;
	case CLPG_BC_CMD_STREQ:
		commandStringComp(1);
		break;
	case CLPG_BC_CMD_STRNOTEQ:
		commandStringComp(2);
		break;
	case CLPG_BC_CMD_PRINT:
		commandPrintf();
		break;
	case CLPG_BC_CMD_CALL:
		res = commandCall(errorMsg);
		break;
	case CLPG_BC_CMD_OBJEQ:
		commandObjComp(1);
		break;
	case CLPG_BC_CMD_OBJNOTEQ:
		commandObjComp(2);
		break;
	default:
		throw PError("Interpreter::Unrecognized command.");
		break;
	}
	return res;
}

//******************************************************************************
// data alloc
//******************************************************************************
void CLPGByteCodeInterpreter::commandDataAlloc( void )
{
	UINT64 code = *IP++;
	UINT64 offset = *IP++;
	*(DS + offset) = code;
}

//******************************************************************************
// copy data to stack
//******************************************************************************
void CLPGByteCodeInterpreter::commandCopyData( void )
{
	const UINT64* i;
	UINT64 amount, sort	= *IP & CLPG_BC_GEN_SORTMASK;

	if((CLPG_BC_GEN_VAR == sort) || (CLPG_BC_GEN_CONST == sort))
	{
		amount = CLPG_BC_GET_DATAAMOUNT(*IP);
		for(i = IP + amount; i >= IP; --i)
			stack.push(*i);
		IP += amount + 1;
	}
	else if(CLPG_BC_GEN_REFERENCE == sort)
	{
		stack.push(*IP++);
	}
	else
	{
		throw PError("Interpreter::Copy data command wrong operand.");
	}
}

//******************************************************************************
// copy data by reference to stack
//******************************************************************************
void CLPGByteCodeInterpreter::commandCopyRef( void )
{
	const UINT64  *i, *pdata;
	UINT64 amount, sort	= *IP & CLPG_BC_GEN_SORTMASK;

	if(CLPG_BC_GEN_REFERENCE == sort)
	{
		pdata = DS + (*IP & CLPG_BC_VAR_SUBTYPEMASK);
		IP++;
	}
	else 
	{
		pdata = IP;
		IP += CLPG_BC_GET_DATAAMOUNT(*IP) + 1;
	}

	sort = *pdata & CLPG_BC_GEN_SORTMASK;
	if((CLPG_BC_GEN_VAR == sort) || (CLPG_BC_GEN_CONST == sort))
	{
		amount = CLPG_BC_GET_DATAAMOUNT(*pdata);
		for(i = pdata + amount; i >= pdata; --i)
			stack.push(*i);
	}
	else
	{
		throw PError("Interpreter::Copy data by reference command wrong operand.");
	}
}

//******************************************************************************
// delete one data from stack
//******************************************************************************
void CLPGByteCodeInterpreter::commandPop( void )
{
	UINT64 i, amount, data, sort;

	data = stack.get();
	sort = data & CLPG_BC_GEN_SORTMASK;
	if((CLPG_BC_GEN_VAR == sort) || (CLPG_BC_GEN_CONST == sort))
	{
		amount = CLPG_BC_GET_DATAAMOUNT(data);
		if((CLPG_BC_VAR_STRING == (data & CLPG_BC_VAR_TYPEMASK)) && (0 != (data & CLPG_BC_VAR_EXTERNAL)))
		{
			data = stack.top();
			if(0 != data)
				delete reinterpret_cast<PString*>(data);
		}
		for(i = 0; i < amount; ++i)
			stack.pop();
	}
	else if(CLPG_BC_GEN_REFERENCE == sort)
	{
		;
	}
	else
	{
		throw PError("Interpreter::Command pop wrong operand.");
	}

}

//******************************************************************************
// erase stack data
//******************************************************************************
void CLPGByteCodeInterpreter::commandClear( void )
{
	while(!stack.empty())
	{
		commandPop();
	}
}

//******************************************************************************
// Binary math
//******************************************************************************
void CLPGByteCodeInterpreter::commandAssignment( void )
{
	UINT64  *pdata1, *pdata2;
	UINT64  i, target, source, amount;

	target = *IP++;
	if(CLPG_BC_GEN_REFERENCE == (target & CLPG_BC_GEN_SORTMASK))
	{
		pdata1 = DS + (target & CLPG_BC_VAR_SUBTYPEMASK);
		amount = CLPG_BC_GET_DATAAMOUNT(*pdata1);
	}
	else
	{
		throw PError("Interpreter::Command assignment wrong target operand.");
	}

	if((CLPG_BC_VAR_STRING == (*pdata1 & CLPG_BC_VAR_TYPEMASK)) && (0 != (*pdata1 & CLPG_BC_VAR_EXTERNAL)))
	{
		if(0 != *(pdata1 + 1))
			delete reinterpret_cast<PString*>(*(pdata1 + 1));
		*(pdata1 + 1) = 0;
		*pdata1 &= ~CLPG_BC_VAR_EXTERNAL;
	}

	source = stack.get();
	if((CLPG_BC_GEN_VAR == (source & CLPG_BC_GEN_SORTMASK)) || (CLPG_BC_GEN_CONST == (source & CLPG_BC_GEN_SORTMASK)))
	{
		if(0 != (source & CLPG_BC_VAR_EXTERNAL))
		{
			*pdata1 |= CLPG_BC_VAR_EXTERNAL;
		}
		for(i = 1; i <= amount; ++i)
		{
			*(pdata1 + i) = stack.get();
		}
	}
	else if(CLPG_BC_GEN_REFERENCE == (source & CLPG_BC_GEN_SORTMASK))
	{
		pdata2 = DS + (source & CLPG_BC_VAR_SUBTYPEMASK);
		if(0 != (*pdata2 & CLPG_BC_VAR_EXTERNAL))
		{
			*pdata1 |= CLPG_BC_VAR_EXTERNAL;
		}
		for(i = 1; i <= amount; ++i)
		{
			*(pdata1 + i) = *(pdata2 + i);
		}
	}
	else
	{
		throw PError("Interpreter::Command assignment wrong source operand.");
	}
	//stack.push(target);

}

//******************************************************************************
// ++/--
//******************************************************************************
void CLPGByteCodeInterpreter::commandDoublePlusMinus( bool minus )
{
	UINT64  *pdata;
	UINT64 data = stack.top();
	if(CLPG_BC_GEN_REFERENCE == (data & CLPG_BC_GEN_SORTMASK))
	{
		pdata = DS + (data & CLPG_BC_VAR_SUBTYPEMASK);
		if(minus)
			--(*(pdata + 1));
		else
			++(*(pdata + 1));

	}
	else
	{
		throw PError("Interpreter::Command ++/-- wrong source operand.");
	}
}

//******************************************************************************
// - for sign
//******************************************************************************
void CLPGByteCodeInterpreter::commandSignMinus( void )
{
	UINT64  *pdata;
	UINT64 data1, data2;
	data1 = stack.get();
	if((CLPG_BC_GEN_VAR == (data1 & CLPG_BC_GEN_SORTMASK)) || (CLPG_BC_GEN_CONST == (data1 & CLPG_BC_GEN_SORTMASK)))
	{
		data2 = stack.get();
		data2 = ~data2 + 1;
		stack.push(data2);
	}
	else if(CLPG_BC_GEN_REFERENCE == (data1 & CLPG_BC_GEN_SORTMASK))
	{
		pdata = DS + (data1 & CLPG_BC_VAR_SUBTYPEMASK);
		*(pdata + 1) = ~(*(pdata + 1)) + 1;
	}
	else
	{
		throw PError("Interpreter::Command sign '-' wrong source operand.");
	}
	stack.push(data1);
}

//******************************************************************************
// class field
//******************************************************************************
void CLPGByteCodeInterpreter::commandObjField( void )
{
	UINT64  *pdata;
	UINT64 data1in, data2in, data1out, data2out;
	const char *type, *name;

	data1in = stack.get();
	if(CLPG_BC_GEN_VAR == (data1in & CLPG_BC_GEN_SORTMASK))
	{
		data2in = stack.get();
	}
	else if(CLPG_BC_GEN_REFERENCE == (data1in & CLPG_BC_GEN_SORTMASK))
	{
		pdata = DS + (data1in & CLPG_BC_VAR_SUBTYPEMASK);
		data2in = *(pdata + 1);
	}
	else
	{
		throw PError("Interpreter::Command get object field wrong source operand.");
	}

	type = SS + *IP++;
	name = SS + *IP++;
	data1out = *IP++;
	data2out = reinterpret_cast<UINT64>(getFieldAddress(reinterpret_cast<void*>(data2in), type, name));
	if(NULL == data2out)
	{
		throw PError("Interpreter::Command get object field returns NULL address.");
	}
	if(CLPG_BC_VAR_INT == (data1out & CLPG_BC_BLK_TYPEMASK))
	{
		UINT32 offset; 
		PString fieldtype, fieldsubtype, fullfieldtype;
		getFieldInfo(offset, fieldtype, fieldsubtype, fullfieldtype, type, name);
		switch((data1out & CLPG_BC_VAR_SUBTYPEMASK))
		{
		case CLPG_BC_VAR_INT8:
			data2out = *reinterpret_cast<INT8*>(data2out);
			break;
		case CLPG_BC_VAR_BYTE:
			data2out = *reinterpret_cast<UINT8*>(data2out);
			break;
		case CLPG_BC_VAR_INT16:
			data2out = *reinterpret_cast<INT16*>(data2out);
			break;
		case CLPG_BC_VAR_UINT16:
			data2out = *reinterpret_cast<UINT16*>(data2out);
			break;
		case CLPG_BC_VAR_INT32:
			data2out = *reinterpret_cast<INT32*>(data2out);
			break;
		case CLPG_BC_VAR_UINT32:
			data2out = *reinterpret_cast<UINT32*>(data2out);
			break;
		case CLPG_BC_VAR_INT64:
			data2out = *reinterpret_cast<INT64*>(data2out);
			break;
		case CLPG_BC_VAR_UINT64:
			data2out = *reinterpret_cast<UINT64*>(data2out);
			break;
		default:
			throw PError("Interpreter::Command get object field unknown type result.");
			break;
		}
	}
	if(CLPG_BC_VAR_BOOL == (data1out & CLPG_BC_BLK_TYPEMASK))
	{
		data2out = *reinterpret_cast<bool*>(data2out);
	}
	else if(CLPG_BC_VAR_STRING == (data1out & CLPG_BC_BLK_TYPEMASK))
	{
		data1out |= CLPG_BC_VAR_EXTERNAL;
		PString* s = new PString;
		PASSERT3(NULL != s);
		s->assign(*reinterpret_cast<PString*>(data2out));
		data2out = reinterpret_cast<UINT64>(s);
	}
	stack.push(data2out);
	stack.push(data1out);
}

//******************************************************************************
// class field vector index
//******************************************************************************
void CLPGByteCodeInterpreter::commandObjFieldIdx( void )
{
	UINT64  *pdata;
	UINT64 data1in, data2in, data1out, data2out, index1, index2;
	const char *type, *name;

	index1 = stack.get();
	if((CLPG_BC_GEN_VAR == (index1 & CLPG_BC_GEN_SORTMASK)) || (CLPG_BC_GEN_CONST == (index1 & CLPG_BC_GEN_SORTMASK)))
	{
		index2 = stack.get();
	}
	else if(CLPG_BC_GEN_REFERENCE == (index1 & CLPG_BC_GEN_SORTMASK))
	{
		pdata = DS + (index1 & CLPG_BC_VAR_SUBTYPEMASK);
		index2 = *(pdata + 1);
	}
	else
	{
		throw PError("Interpreter::Command get object field wrong index operand.");
	}

	data1in = stack.get();
	if(CLPG_BC_GEN_VAR == (data1in & CLPG_BC_GEN_SORTMASK))
	{
		data2in = stack.get();
	}
	else if(CLPG_BC_GEN_REFERENCE == (data1in & CLPG_BC_GEN_SORTMASK))
	{
		pdata = DS + (data1in & CLPG_BC_VAR_SUBTYPEMASK);
		data2in = *(pdata + 1);
	}
	else
	{
		throw PError("Interpreter::Command get object field container index wrong source operand.");
	}

	type = SS + *IP++;
	name = SS + *IP++;
	data1out = *IP++;

	data2out = reinterpret_cast<UINT64>(getFieldContainerElement(reinterpret_cast<void*>(data2in), type, name, index2));
	if(NULL == data2out)
	{
		throw PError("Interpreter::Command get object field container index returns NULL address.");
	}
	if(CLPG_BC_VAR_INT == (data1out & CLPG_BC_BLK_TYPEMASK))
	{
		switch((data1out & CLPG_BC_VAR_SUBTYPEMASK))
		{
		case CLPG_BC_VAR_INT8:
			data2out = *reinterpret_cast<INT8*>(data2out);
			break;
		case CLPG_BC_VAR_BYTE:
			data2out = *reinterpret_cast<UINT8*>(data2out);
			break;
		case CLPG_BC_VAR_INT16:
			data2out = *reinterpret_cast<INT16*>(data2out);
			break;
		case CLPG_BC_VAR_UINT16:
			data2out = *reinterpret_cast<UINT16*>(data2out);
			break;
		case CLPG_BC_VAR_INT32:
			data2out = *reinterpret_cast<INT32*>(data2out);
			break;
		case CLPG_BC_VAR_UINT32:
			data2out = *reinterpret_cast<UINT32*>(data2out);
			break;
		case CLPG_BC_VAR_INT64:
			data2out = *reinterpret_cast<INT64*>(data2out);
			break;
		case CLPG_BC_VAR_UINT64:
			data2out = *reinterpret_cast<UINT64*>(data2out);
			break;
		default:
			throw PError("Interpreter::Command get object field container index unknown type result.");
			break;
		}
	}
	if(CLPG_BC_VAR_BOOL == (data1out & CLPG_BC_BLK_TYPEMASK))
	{
		data2out = *reinterpret_cast<bool*>(data2out);
	}
	else if(CLPG_BC_VAR_STRING == (data1out & CLPG_BC_BLK_TYPEMASK))
	{
		data1out |= CLPG_BC_VAR_EXTERNAL;
		PString* s = new PString;
		PASSERT3(NULL != s);
		s->assign(*reinterpret_cast<PString*>(data2out));
		data2out = reinterpret_cast<UINT64>(s);
	}
	stack.push(data2out);
	stack.push(data1out);
}

//******************************************************************************
// class field vector size
//******************************************************************************
void CLPGByteCodeInterpreter::commandObjFieldSize( void )
{
	bool res;
	UINT32 size = 0;
	UINT64  *pdata;
	UINT64 data1in, data2in, data1out, data2out = 0;
	const char *type, *name;

	data1in = stack.get();
	if(CLPG_BC_GEN_VAR == (data1in & CLPG_BC_GEN_SORTMASK))
	{
		data2in = stack.get();
	}
	else if(CLPG_BC_GEN_REFERENCE == (data1in & CLPG_BC_GEN_SORTMASK))
	{
		pdata = DS + (data1in & CLPG_BC_VAR_SUBTYPEMASK);
		data2in = *(pdata + 1);
	}
	else
	{
		throw PError("Interpreter::Command get object field container size wrong source operand.");
	}

	type = SS + *IP++;
	name = SS + *IP++;
	data1out = *IP++;
	res = getFieldContainerSize(size, reinterpret_cast<void*>(data2in), type, name);
	if(res)
		data2out = size;
	else
		PLog("Interpreter::Command get object field container size returns error.");
	stack.push(data2out);
	stack.push(data1out);
}

//******************************************************************************
// standard class object init
//******************************************************************************
void CLPGByteCodeInterpreter::commandObjInit( void )
{
	UINT64  *pdata;
	UINT64 datain;
	const char *type;

	datain = *IP++;
	if(CLPG_BC_GEN_REFERENCE == (datain & CLPG_BC_GEN_SORTMASK))
	{
		pdata = DS + (datain & CLPG_BC_VAR_SUBTYPEMASK);
	}
	else
	{
		throw PError("Interpreter::Command object init wrong source operand.");
	}

	type = SS + *IP++;
	*(pdata + 1) = reinterpret_cast<UINT64>(initObj(type));
}

//******************************************************************************
// +, -, *, /, &, ^, | 
//******************************************************************************
void CLPGByteCodeInterpreter::commandBinaryMath( UINT64 code )
{
	UINT64  *pdata;
	UINT64 data1in1, data2in1, data1in2, data2in2, data2out;

	data1in1 = stack.get();
	if((CLPG_BC_GEN_VAR == (data1in1 & CLPG_BC_GEN_SORTMASK)) || (CLPG_BC_GEN_CONST == (data1in1 & CLPG_BC_GEN_SORTMASK)))
	{
		data2in1 = stack.get();
	}
	else if(CLPG_BC_GEN_REFERENCE == (data1in1 & CLPG_BC_GEN_SORTMASK))
	{
		pdata = DS + (data1in1 & CLPG_BC_VAR_SUBTYPEMASK);
		data2in1 = *(pdata + 1);
		data1in1 = *pdata;
	}
	else
	{
		throw PError("Interpreter::Command binary math wrong operand.");
	}
	data1in2 = stack.get();
	if((CLPG_BC_GEN_VAR == (data1in2 & CLPG_BC_GEN_SORTMASK)) || (CLPG_BC_GEN_CONST == (data1in2 & CLPG_BC_GEN_SORTMASK)))
	{
		data2in2 = stack.get();
	}
	else if(CLPG_BC_GEN_REFERENCE == (data1in2 & CLPG_BC_GEN_SORTMASK))
	{
		pdata = DS + (data1in2 & CLPG_BC_VAR_SUBTYPEMASK);
		data2in2 = *(pdata + 1);
		data1in2 = *pdata;
	}
	else
	{
		throw PError("Interpreter::Command binary math wrong operand.");
	}

	switch (code)
	{
	case 1:	// CLPG_BC_CMD_PLUS
		data2out = data2in2 + data2in1;
		break;
	case 2:	// CLPG_BC_CMD_MINUS
		data2out = data2in2 - data2in1;
		break;
	case 3:	// CLPG_BC_CMD_MUL
		data2out = data2in2 * data2in1;
		break;
	case 4:	// CLPG_BC_CMD_DIV
		data2out = data2in2 / data2in1;
		break;
	case 5:	// CLPG_BC_CMD_BAND
		data2out = data2in2 & data2in1;
		break;
	case 6:	// CLPG_BC_CMD_BXOR
		data2out = data2in2 ^ data2in1;
		break;
	case 7:	// CLPG_BC_CMD_BOR
		data2out = data2in2 | data2in1;
		break;
	case 8:	// CLPG_BC_CMD_MOD
		data2out = data2in2 % data2in1;
	default:
		throw PError("Binary math operation unrecognized \n");
		break;
	}
	stack.push(data2out);

	if((data1in1 & CLPG_BC_VAR_SUBTYPEMASK) > (data1in2 & CLPG_BC_VAR_SUBTYPEMASK))
		stack.push(data1in1);
	else
		stack.push(data1in2);
}

//******************************************************************************
// ==, !=, >, <, >=, <=
//******************************************************************************
void CLPGByteCodeInterpreter::commandBinaryComp( UINT64 code )
{
	bool res;
	UINT64  *pdata;
	UINT64 data1in1, data2in1, data1in2, data2in2;

	data1in1 = stack.get();
	if((CLPG_BC_GEN_VAR == (data1in1 & CLPG_BC_GEN_SORTMASK)) || (CLPG_BC_GEN_CONST == (data1in1 & CLPG_BC_GEN_SORTMASK)))
	{
		data2in1 = stack.get();
	}
	else if(CLPG_BC_GEN_REFERENCE == (data1in1 & CLPG_BC_GEN_SORTMASK))
	{
		pdata = DS + (data1in1 & CLPG_BC_VAR_SUBTYPEMASK);
		data2in1 = *(pdata + 1);
		data1in1 = *pdata;
	}
	else
	{
		throw PError("Interpreter::Command conditional wrong operand.");
	}
	data1in2 = stack.get();
	if((CLPG_BC_GEN_VAR == (data1in2 & CLPG_BC_GEN_SORTMASK)) || (CLPG_BC_GEN_CONST == (data1in2 & CLPG_BC_GEN_SORTMASK)))
	{
		data2in2 = stack.get();
	}
	else if(CLPG_BC_GEN_REFERENCE == (data1in2 & CLPG_BC_GEN_SORTMASK))
	{
		pdata = DS + (data1in2 & CLPG_BC_VAR_SUBTYPEMASK);
		data2in2 = *(pdata + 1);
		data1in2 = *pdata;
	}
	else
	{
		throw PError("Interpreter::Command conditional wrong operand.");
	}

	switch (code)
	{
	case 1:	// CLPG_BC_CMD_EQ
		res = data2in2 == data2in1;
		break;
	case 2:	// CLPG_BC_CMD_NOTEQ
		res = data2in2 != data2in1;
		break;
	case 3:	// CLPG_BC_CMD_MORE
		res = data2in2 > data2in1;
		break;
	case 4:	// CLPG_BC_CMD_MOREEQ
		res = data2in2 >= data2in1;
		break;
	case 5:	// CLPG_BC_CMD_LESS
		res = data2in2 < data2in1;
		break;
	case 6:	// CLPG_BC_CMD_LESSEQ
		res = data2in2 <= data2in1;
		break;
	default:
		throw PError("Conditional operation unrecognized \n");
		break;
	}
	if(res)
		stack.push(CLPG_BC_VAR_BOOL_TRUE);
	else
		stack.push(CLPG_BC_VAR_BOOL_FALSE);
	stack.push(CLPG_BC_GEN_CONST | CLPG_BC_VAR_BOOL | CLPG_BC_SET_DATAAMOUNT(1));
}

//******************************************************************************
// ==, != for strings
//******************************************************************************
void CLPGByteCodeInterpreter::commandStringComp( UINT64 code )
{
	bool res;
	bool del;
	UINT64  *pdata;
	UINT64 data1in1, data2in1, data1in2, data2in2;
	PString str1, str2;

	data1in1 = stack.get();
	del = false;
	if((CLPG_BC_GEN_VAR == (data1in1 & CLPG_BC_GEN_SORTMASK)) || (CLPG_BC_GEN_CONST == (data1in1 & CLPG_BC_GEN_SORTMASK)))
	{
		data2in1 = stack.get();
		del = true;
	}
	else if(CLPG_BC_GEN_REFERENCE == (data1in1 & CLPG_BC_GEN_SORTMASK))
	{
		pdata = DS + (data1in1 & CLPG_BC_VAR_SUBTYPEMASK);
		data2in1 = *(pdata + 1);
		data1in1 = *pdata;
	}
	else
	{
		throw PError("Interpreter::Command string ==/!= wrong operand.");
	}

	if(0 != (data1in1 & CLPG_BC_VAR_EXTERNAL))
	{
		str1.assign(*reinterpret_cast<PString*>(data2in1));
		if(del)
		{
			str1.moveFrom(*reinterpret_cast<PString*>(data2in1));
			delete reinterpret_cast<PString*>(data2in1);
		}
		else
		{
			str1.assign(*reinterpret_cast<PString*>(data2in1));
		}
	}
	else
	{
		str1.assign(SS + data2in1);
	}

	data1in2 = stack.get();
	del = false;
	if((CLPG_BC_GEN_VAR == (data1in2 & CLPG_BC_GEN_SORTMASK)) || (CLPG_BC_GEN_CONST == (data1in2 & CLPG_BC_GEN_SORTMASK)))
	{
		data2in2 = stack.get();
		del = true;
	}
	else if(CLPG_BC_GEN_REFERENCE == (data1in2 & CLPG_BC_GEN_SORTMASK))
	{
		pdata = DS + (data1in2 & CLPG_BC_VAR_SUBTYPEMASK);
		data2in2 = *(pdata + 1);
		data1in2 = *pdata;
	}
	else
	{
		throw PError("Interpreter::Command string ==/!= wrong operand.");
	}

	if(0 != (data1in2 & CLPG_BC_VAR_EXTERNAL))
	{
		str2.assign(*reinterpret_cast<PString*>(data2in2));
		if(del)
		{
			str2.moveFrom(*reinterpret_cast<PString*>(data2in2));
			delete reinterpret_cast<PString*>(data2in2);
		}
		else
		{
			str2.assign(*reinterpret_cast<PString*>(data2in2));
		}
	}
	else
	{
		str2.assign(SS + data2in2);
	}

	switch (code)
	{
	case 1:	// CLPG_BC_CMD_EQ
		res = (0 == strcmp(str1, str2));
		break;
	case 2:	// CLPG_BC_CMD_NOTEQ
		res = (0 != strcmp(str1, str2));
		break;
	default:
		throw PError("Conditional operation for strings unrecognized \n");
		break;
	}
	if(res)
		stack.push(CLPG_BC_VAR_BOOL_TRUE);
	else
		stack.push(CLPG_BC_VAR_BOOL_FALSE);
	stack.push(CLPG_BC_GEN_CONST | CLPG_BC_VAR_BOOL | CLPG_BC_SET_DATAAMOUNT(1));
}

//******************************************************************************
// ==, != for objects
//******************************************************************************
void CLPGByteCodeInterpreter::commandObjComp( UINT64 code )
{
	bool res;
	UINT64  *pdata;
	UINT64 data1in1, data2in1, data1in2, data2in2;

	data1in1 = stack.get();
	if((CLPG_BC_GEN_VAR == (data1in1 & CLPG_BC_GEN_SORTMASK)) || (CLPG_BC_GEN_CONST == (data1in1 & CLPG_BC_GEN_SORTMASK)))
	{
		data2in1 = stack.get();
	}
	else if(CLPG_BC_GEN_REFERENCE == (data1in1 & CLPG_BC_GEN_SORTMASK))
	{
		pdata = DS + (data1in1 & CLPG_BC_VAR_SUBTYPEMASK);
		data2in1 = *(pdata + 1);
		data1in1 = *pdata;
	}
	else
	{
		throw PError("Interpreter::Command object ==/!= wrong operand.");
	}

	data1in2 = stack.get();
	if((CLPG_BC_GEN_VAR == (data1in2 & CLPG_BC_GEN_SORTMASK)) || (CLPG_BC_GEN_CONST == (data1in2 & CLPG_BC_GEN_SORTMASK)))
	{
		data2in2 = stack.get();
	}
	else if(CLPG_BC_GEN_REFERENCE == (data1in2 & CLPG_BC_GEN_SORTMASK))
	{
		pdata = DS + (data1in2 & CLPG_BC_VAR_SUBTYPEMASK);
		data2in2 = *(pdata + 1);
		data1in2 = *pdata;
	}
	else
	{
		throw PError("Interpreter::Command object ==/!= wrong operand.");
	}

	if((data1in2 & CLPG_BC_VAR_EXTERNAL) == (data1in1 & CLPG_BC_VAR_EXTERNAL))
	{
		switch (code)
		{
		case 1:	// CLPG_BC_CMD_EQ
			res = (data2in2 == data2in1);
			break;
		case 2:	// CLPG_BC_CMD_NOTEQ
			res = (data2in2 != data2in1);
			break;
		default:
			throw PError("Conditional operation for object unrecognized \n");
			break;
		}
	}
	else
	{
		res = false;
	}


	if(res)
		stack.push(CLPG_BC_VAR_BOOL_TRUE);
	else
		stack.push(CLPG_BC_VAR_BOOL_FALSE);
	stack.push(CLPG_BC_GEN_CONST | CLPG_BC_VAR_BOOL | CLPG_BC_SET_DATAAMOUNT(1));
}

//******************************************************************************
// &&, ||
//******************************************************************************
void CLPGByteCodeInterpreter::commandBinaryLogic( UINT64 code )
{
	bool res, data2in1, data2in2;
	UINT64  *pdata;
	UINT64 data1in1, data1in2;

	data1in1 = stack.get();
	if((CLPG_BC_GEN_VAR == (data1in1 & CLPG_BC_GEN_SORTMASK)) || (CLPG_BC_GEN_CONST == (data1in1 & CLPG_BC_GEN_SORTMASK)))
	{
		data2in1 = stack.get() == CLPG_BC_VAR_BOOL_TRUE;
	}
	else if(CLPG_BC_GEN_REFERENCE == (data1in1 & CLPG_BC_GEN_SORTMASK))
	{
		pdata = DS + (data1in1 & CLPG_BC_VAR_SUBTYPEMASK);
		data2in1 = *(pdata + 1) == CLPG_BC_VAR_BOOL_TRUE;
		data1in1 = *pdata;
	}
	else
	{
		throw PError("Interpreter::Command logic wrong operand.");
	}
	data1in2 = stack.get();
	if((CLPG_BC_GEN_VAR == (data1in2 & CLPG_BC_GEN_SORTMASK)) || (CLPG_BC_GEN_CONST == (data1in2 & CLPG_BC_GEN_SORTMASK)))
	{
		data2in2 = stack.get() == CLPG_BC_VAR_BOOL_TRUE;
	}
	else if(CLPG_BC_GEN_REFERENCE == (data1in2 & CLPG_BC_GEN_SORTMASK))
	{
		pdata = DS + (data1in2 & CLPG_BC_VAR_SUBTYPEMASK);
		data2in2 = *(pdata + 1) == CLPG_BC_VAR_BOOL_TRUE;
		data1in2 = *pdata;
	}
	else
	{
		throw PError("Interpreter::Command logic wrong operand.");
	}

	switch (code)
	{
	case 1:	// CLPG_BC_CMD_AND
		res = data2in2 && data2in1;
		break;
	case 2:	// CLPG_BC_CMD_OR
		res = data2in2 || data2in1;
		break;
	default:
		throw PError("Logic operation unrecognized \n");
		break;
	}
	if(res)
		stack.push(CLPG_BC_VAR_BOOL_TRUE);
	else
		stack.push(CLPG_BC_VAR_BOOL_FALSE);
	stack.push(CLPG_BC_GEN_CONST | CLPG_BC_VAR_BOOL | CLPG_BC_SET_DATAAMOUNT(1));
}

//******************************************************************************
// 0- goto, 1-if false, 2-if true
//******************************************************************************
void CLPGByteCodeInterpreter::commandGoto( UINT64 code )
{
	UINT64  *pdata;
	UINT64 data1, offset; 
	bool data2;

	offset = *IP++;
	if(0 == code) // CLPG_BC_CMD_GOTO
	{
		IP = CS + offset;
		stack.clear();
	}
	else
	{
		data1 = stack.get();
		if((CLPG_BC_GEN_VAR == (data1 & CLPG_BC_GEN_SORTMASK)) || (CLPG_BC_GEN_CONST == (data1 & CLPG_BC_GEN_SORTMASK)))
		{
			data2 = stack.top() == CLPG_BC_VAR_BOOL_TRUE;
		}
		else if(CLPG_BC_GEN_REFERENCE == (data1 & CLPG_BC_GEN_SORTMASK))
		{
			pdata = DS + (data1 & CLPG_BC_VAR_SUBTYPEMASK);
			data2 = *(pdata + 1) == CLPG_BC_VAR_BOOL_TRUE;
		}
		else
		{
			throw PError("Interpreter::Command goto wrong operand.");
		}
		stack.push(data1);

		if(1 == code)	// CLPG_BC_CMD_GOTOFALSE
		{
			if(!data2)
			{
				IP = CS + offset;
				stack.clear();
			}
		}
		else if(2 == code)	// CLPG_BC_CMD_GOTOTRUE
		{
			if(data2)
			{
				IP = CS + offset;
				stack.clear();
			}
		}
		else
		{
			throw PError("Goto operation unrecognized \n");
		}
	}
}

//******************************************************************************
// + for strings
//******************************************************************************
void CLPGByteCodeInterpreter::commandStringPlus(void)
{
	bool del;
	UINT64  *pdata;
	UINT64 data1in1, data2in1, data1in2, data2in2;
	PString* str = NULL;
	PString str1, str2;

	data1in1 = stack.get();
	del = false;
	if((CLPG_BC_GEN_VAR == (data1in1 & CLPG_BC_GEN_SORTMASK)) || (CLPG_BC_GEN_CONST == (data1in1 & CLPG_BC_GEN_SORTMASK)))
	{
		data2in1 = stack.get();
		del = true;
	}
	else if(CLPG_BC_GEN_REFERENCE == (data1in1 & CLPG_BC_GEN_SORTMASK))
	{
		pdata = DS + (data1in1 & CLPG_BC_VAR_SUBTYPEMASK);
		data2in1 = *(pdata + 1);
		data1in1 = *pdata;
	}
	else
	{
		throw PError("Interpreter::Command string + wrong operand.");
	}

	if(0 != (data1in1 & CLPG_BC_VAR_EXTERNAL))
	{
		if(del)
		{
			str1.moveFrom(*reinterpret_cast<PString*>(data2in1));
			delete reinterpret_cast<PString*>(data2in1);
		}
		else
		{
			str1.assign(*reinterpret_cast<PString*>(data2in1));
		}
	}
	else
	{
		str1.assign(SS + data2in1);
	}

	data1in2 = stack.get();
	del = false;
	if((CLPG_BC_GEN_VAR == (data1in2 & CLPG_BC_GEN_SORTMASK)) || (CLPG_BC_GEN_CONST == (data1in2 & CLPG_BC_GEN_SORTMASK)))
	{
		data2in2 = stack.get();
		del = true;
	}
	else if(CLPG_BC_GEN_REFERENCE == (data1in2 & CLPG_BC_GEN_SORTMASK))
	{
		pdata = DS + (data1in2 & CLPG_BC_VAR_SUBTYPEMASK);
		data2in2 = *(pdata + 1);
		data1in2 = *pdata;
	}
	else
	{
		throw PError("Interpreter::Command string + wrong operand.");
	}

	if(0 != (data1in2 & CLPG_BC_VAR_EXTERNAL))
	{
		if(del)
		{
			str2.moveFrom(*reinterpret_cast<PString*>(data2in2));
			delete reinterpret_cast<PString*>(data2in2);
		}
		else
		{
			str2.assign(*reinterpret_cast<PString*>(data2in2));
		}
	}
	else
	{
		str2.assign(SS + data2in2);
	}

	str = new PString;
	PASSERT3(NULL != str);

	str->assign(str2);
	str->append(str1);

	stack.push(reinterpret_cast<UINT64>(str));

	stack.push(CLPG_BC_GEN_VAR | CLPG_BC_VAR_STRING | CLPG_BC_SET_DATAAMOUNT(1) | CLPG_BC_VAR_EXTERNAL);
}

//******************************************************************************
// log output
//******************************************************************************
void CLPGByteCodeInterpreter::commandPrintf( void )
{
	bool del;
	UINT64 data, data1, amount, size;
	UINT64* pdata;
	INT64 i;
	vector<CLPGByteCodeDATATYPE> params;
	vector<const char*> strs;
	char* format = NULL;
	//char seps[]   = "%";
	PString outStr = "Interpreter print: ";

	data = stack.get();
	if(CLPG_BC_VAR_INT == (data & CLPG_BC_VAR_TYPEMASK))
	{
		amount = stack.get();
	}
	else
	{
		throw PError("Interpreter::Command print wrong amount type operand.");
	}

	if(0 == amount)
		throw PError("Interpreter::Command print must have format string.");

	params.resize(amount);
	try
	{
		for(i = amount - 1; i >= 0; --i)
		{
			data = stack.get();
			if((CLPG_BC_GEN_VAR == (data & CLPG_BC_GEN_SORTMASK)) || (CLPG_BC_GEN_CONST == (data & CLPG_BC_GEN_SORTMASK)))
			{
				data1 = stack.get();
				del = true;
			}
			else if(CLPG_BC_GEN_REFERENCE == (data & CLPG_BC_GEN_SORTMASK))
			{
				pdata = DS + (data & CLPG_BC_VAR_SUBTYPEMASK);
				data1 = *(pdata + 1);
				data = *pdata;
				del = false;
			}
			else
			{
				throw PError("Interpreter::Command print wrong operand.");
			}

			if(CLPG_BC_VAR_STRING == (data & CLPG_BC_VAR_TYPEMASK))
			{
				if(0 != (data & CLPG_BC_VAR_EXTERNAL))
				{
					size = reinterpret_cast<PString*>(data1)->length() + 1;
					char* ps = new char[size];
					memset(ps, 0, size);
					strcpy(ps, reinterpret_cast<PString*>(data1)->c_str());
					strs.push_back(ps);
					if(del)
						delete reinterpret_cast<PString*>(data1);
					params[i].s = *(strs.end() - 1);
				}
				else
				{
					params[i].s = SS + data1;
				}
			}
			else
			{
				params[i].ui64 = data1;
			}
		}

		size = strlen(params[0].s) + 1;
		format = new char[size];
		memset(format, 0, size);
		strcpy(format, params[0].s);

		char* tok1 = format;
		char* tok2 = NULL;
		UINT64 counter = 1;

		tok2 = strchr(tok1, '%');
		while(NULL != tok2)
		{
			if(amount <= counter)
				break;
			*tok2 = 0;
			outStr.append(tok1);
			tok1 = tok2 + 1;
			if(*tok1 != 0)
			{
				switch(*tok1)
				{
				case 'd':
					outStr.appendInt64(params[counter].i64);
					++counter;
					break;
				case 'u':
					outStr.appendUint64(params[counter].ui64);
					++counter;
						break;
				case 'x':
					outStr.append("0x").appendHexInt64(params[counter].ui64);
					++counter;
						break;
				case 's':
					outStr.append(params[counter].s);
					++counter;
					break;
				default:
					outStr.append('%').append(*tok1);
					break;
				}
				++tok1;
			}
			tok2 = strchr(tok1, '%');
		}
		outStr.append(tok1).append("\n");
		PLog(outStr.c_str());
		//OutputDebugString(outStr.c_str());
	}
	catch( PError& err )
	{
		if(NULL != format)
			delete[] format;
		for(i = 0; i < strs.size(); ++i)
			delete[] strs[i];
		throw err;
	}
	catch( ... )
	{
		if(NULL != format)
			delete[] format;
		for(i = 0; i < strs.size(); ++i)
			delete[] strs[i];
		throw PError( "Interpreter::Unknown error in print function call \n" );
	}
	if(NULL != format)
		delete[] format;
	for(i = 0; i < strs.size(); ++i)
		delete[] strs[i];
}

//******************************************************************************
// external function call
//******************************************************************************
RuleRetCode CLPGByteCodeInterpreter::commandCall(PString& errorMsg)
{
	bool del;
	RuleRetCode reserr = RuleRetCode::ruleOk;
	PString name;
	UINT64* pdata;
	vector<CLPGByteCodeDATATYPE> params;
	CLPGByteCodeDATATYPE result;
	UINT64 amount, data, data1, ret; 
	INT64 i;
	vector<const char*> strs;

	data = stack.get();
	if((CLPG_BC_VAR_STRING == (data & CLPG_BC_VAR_TYPEMASK)) && (0 == (data & CLPG_BC_VAR_EXTERNAL)))
	{
		name.assign(SS + stack.get());
	}
	else
	{
		throw PError("Interpreter::Command function call wrong function name type operand.");
	}

	ret = stack.get();
	if((CLPG_BC_GEN_VAR != (ret & CLPG_BC_GEN_SORTMASK)) && (CLPG_BC_GEN_CONST != (ret & CLPG_BC_GEN_SORTMASK)))
	{
		throw PError("Interpreter::Command function call wrong return type operand.");
	}

	stack.get();
	data = stack.get();
	if(CLPG_BC_VAR_INT == (data & CLPG_BC_VAR_TYPEMASK))
	{
		amount = stack.get();
	}
	else
	{
		throw PError("Interpreter::Command function call wrong amount type operand.");
	}

	params.resize(amount);
	try
	{
		for(i = amount - 1; i >= 0; --i)
		{
			data = stack.get();
			if((CLPG_BC_GEN_VAR == (data & CLPG_BC_GEN_SORTMASK)) || (CLPG_BC_GEN_CONST == (data & CLPG_BC_GEN_SORTMASK)))
			{
				data1 = stack.get();
				del = true;
			}
			else if(CLPG_BC_GEN_REFERENCE == (data & CLPG_BC_GEN_SORTMASK))
			{
				pdata = DS + (data & CLPG_BC_VAR_SUBTYPEMASK);
				data1 = *(pdata + 1);
				data = *pdata;
				del = false;
			}
			else
			{
				throw PError("Interpreter::Command function call wrong operand.");
			}

			if(CLPG_BC_VAR_STRING == (data & CLPG_BC_VAR_TYPEMASK))
			{
				if(0 != (data & CLPG_BC_VAR_EXTERNAL))
				{
					UINT64 size = reinterpret_cast<PString*>(data1)->length() + 1;
					char* ps = new char[size];
					memset(ps, 0, size);
					strcpy(ps, reinterpret_cast<PString*>(data1)->c_str());
					strs.push_back(ps);
					if(del)
						delete reinterpret_cast<PString*>(data1);
					params[i].s = *(strs.end() - 1);
				}
				else
				{
					params[i].s = SS + data1;
				}
			}
			else
			{
				params[i].ui64 = data1;
			}
		}

		reserr = callFunction(name, params, result, errorMsg);
	}
	catch( PError& err )
	{
		for(i = 0; i < strs.size(); ++i)
			delete strs[i];
		throw err;
	}
	catch( ... )
	{
		for(i = 0; i < strs.size(); ++i)
			delete strs[i];
		throw PError( "Interpreter::Unknown error in function call \n" );
	}
	for(i = 0; i < strs.size(); ++i)
		delete strs[i];
	if(CLPG_BC_VAR_VOID != (ret & CLPG_BC_VAR_TYPEMASK))
	{
		stack.push(result.ui64);
		stack.push(ret);
	}

	return reserr;
}


