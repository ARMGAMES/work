////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2014 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef	misc_h_included
#define	misc_h_included

template<char YEP, char NOPE>
inline char boolAsChar(bool arg)
{
	return arg? YEP : NOPE;
}
// bool as +/- char
inline char boolAsPlusMinus(bool arg){ return boolAsChar< '+', '-' >(arg); }
// bool as y/n char
inline char boolAsYN(bool arg){ return boolAsChar< 'y', 'n' >(arg); }

// functor for string comparison
struct stringLessIgnoreCase : public binary_function < const char* , const char* , bool >
{
	bool operator() (const char* firstString, const char* secondString) const 
	{
		return (PString::compareIgnoreCase(firstString, secondString) < 0) ;
	}
};

// append to a PString a string as a hex dump
inline PString& appendMemoryDumpAsHex(PString& out, const char* str)
{
	while( str && *str )
	{
		INT32 byte = *str++;
		if( byte < 0 ) byte += 0x100;

		out.appendHexInt( byte ).append(' ');
	}
	return out;
}

// append to a PString a memory buffer as a hex dump
inline PString& appendMemoryDumpAsHex(PString& out, const char* str, INT32 length)
{
	while( str && length-- )
	{
		INT32 byte = *str++;
		if( byte < 0 ) byte += 0x100;

		out.appendHexInt( byte ).append(' ');
	}
	return out;
}

// append to a PString a PString as a hex dump
inline PString& appendMemoryDumpAsHex(PString& out, const PString& str)
{
	return appendMemoryDumpAsHex( out, str.c_str(), static_cast< INT32 >( str.length() ) );
}

// append to a PString a message as a hex dump
inline PString& appendMemoryDumpAsHex(PString& out, const CommMsgBody& msg)
{
	CommMsgParser parser(msg);
	while( !parser.parseEnded() )
	{
		BYTE b;
		parser.parseBYTE(b);
		out.appendHexInt(b).append(' ');
	}
	return out;
}

#endif	//misc_h_included
