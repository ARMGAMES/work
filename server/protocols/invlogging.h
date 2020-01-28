#ifndef invlogging_h_included
#define invlogging_h_included

////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2009 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// debug output - a small set of functions which works well for types of 
// collections that I need to print. when I use something that's not covered by  
// it I'll add more stuff.
// 

namespace InvLogging
{
	// print collection through a call to PrintIteratorValueFunctor.print
	// which is responsible for dereferencing correct value from the
	// collection iterator
	//
	template<class T_it, class PrintIteratorValueFunctor>
	void printColl( const T_it& ifirst_, const T_it& ilast, const PrintIteratorValueFunctor& pf)
	{ 
		for( T_it ifirst = ifirst_; ifirst != ilast; ++ifirst)
		{
			pf.print(ifirst);
		}
	}
	template<class T_it>
	void printSimplePrintableColl( const T_it& ifirst_, const T_it& ilast)
	{ 
		for( T_it ifirst = ifirst_; ifirst != ilast; ++ifirst)
		{
			ifirst->print();
		}
	}

	template<class T_it, class PrintIteratorValueFunctor>
	void printCollColl( const T_it& ifirst_, const T_it& ilast, const PrintIteratorValueFunctor& pf)
	{
		PLog("----------------");
		for( T_it ifirst = ifirst_; ifirst != ilast; ++ifirst)
		{
			printColl( (*ifirst).begin(), (*ifirst).end(), pf);
			PLog("---");
		}
	}

	// dereference value as (*it)

	template< class PrintValueFunctor>
	class PIVFVal : public PrintValueFunctor
	{
	public:
		template<class T_it>
		void print( const T_it& it) const { PLog( PrintValueFunctor::format(), (*this)(*it));} //-V510
	};

	class PIVFValPrintable
	{
	public:
		template<class T_it>
		void print( const T_it& it) const { (*it).print();}
	};
	class PIVFPtrPrintable
	{
	public:
		template<class T_it>
		void print( const T_it& it) const { (*it)->print();}
	};

	// dereference value as (*it).second
	template< class PrintValueFunctor>
	class PIVFPairSecondVal : public PrintValueFunctor
	{
	public:
		template<class T_it>
		void print(const T_it& it) const {	PLog( PrintValueFunctor::format(), (*this)((*it).second));} //-V510
	};

	template< class PrintFirst, class PrintSecond>
	class PIVFPairMapVal
	{
		PrintFirst pf;
		PrintSecond ps;
	public:
		template<class T_it>
		void print( const T_it& it) const 
		{
			PLog( pf.format(), pf((*it).first));
			PLog( ps.format(), ps((*it).second));
		}
	};
	template< class PrintFirst>
	class PIVFPairMapPrintable
	{
		PrintFirst pf;
	public:
		template<class T_it>
		void print( const T_it& it) const 
		{
			PLog( pf.format(), pf((*it).first));
			(*it).second.print();
		}
	};
	template< class PrintFirst>
	class PIVFPairMapGetPrintable
	{
		PrintFirst pf;
	public:
		template<class T_it>
		void print( const T_it& it) const 
		{
			PLog( pf.format(), pf((*it).first));
			(*it).second.getPrintable().print();
		}
	};

	// print PString
	class PVFPString
	{
	public:
		const char* operator()(const PString& val) const { return val.c_str(); }
		const char* operator()(const PString* val) const { return val->c_str(); }
		const char* format() const { return "(%s)"; }
	};

	// print any number that can be formatted as "%d"
	template<typename T>
	class PVFNumber
	{
	public:
		int operator() (const T& val) const{ return val; }
		const char* format() const { return "(%d)"; }
	};
}

#endif //invlogging
