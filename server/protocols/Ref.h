#ifndef _Ref_h_
#define _Ref_h_

/* 
 * To introduce reference counting in your code, include REFERENCE_COUNTED macro in the class declaration.
 * Another approach is to derive your class from the RefCounted base class, which will provide
 * additional checking and PLog warning against the deletion of still referenced object. However, since multiple 
 * inheritance is not allowed, it is not always possible. Please note that this implementation is 
 * not thread safe and as such should be used in single thread context only.
 *
 * Also, there is a TRef<> smart pointer to use along with reference counted classes.
 * 
 * Example: 
 *
 *	class Beta: public Alpha 
 *	{
 *		REFERENCE_COUNTED; // use the macro
 *
 *	public:
 *		// your class declaration 	
 *	};
 *
 *	typedef TRef<Beta> PBeta;
 *  PBeta ptr;
 *  vector<PBeta> ptrList;
 *  ptr->myMethod();
 * 
 */

#include "pplogfile.h"


/**************************************************************************/
/* The following classes can only be used in single threaded applications */
/**************************************************************************/

// used for auto initialization of the counter  
struct RefInitCounterHolder 
{
	mutable int counter;
	RefInitCounterHolder(): counter(0) {}
};

// macro to include in your class declaration to make it reference counted
#define REFERENCE_COUNTED \
	public: void addRef() const {++ctr.counter;} \
	void release() const { if(--ctr.counter==0) delete this; } \
	int getRefCount() const { return ctr.counter; } \
	private: RefInitCounterHolder ctr;

// base reference counted class
class RefCounted
{ 
	REFERENCE_COUNTED;
protected:
	virtual ~RefCounted()
	{
		if( ctr.counter != 0 )
		{
			try
			{
				PLog("PError: deleting an object with reference count %d", ctr.counter);
			}
			catch(...)
			{ /* ignore */
			}
		}
	}
};


/**************************************************************************/
/* The following classes can be used in multi-threaded applications       */
/**************************************************************************/

// used for auto initialization of the counter  
struct RefInitInterlockedHolder 
{
	mutable PInterlocked interlocked;
	RefInitInterlockedHolder(): interlocked(0) {}
};

// macro to include in your class declaration to make it reference counted
#define REFERENCE_INTERLOCKED \
	public: void addRef() const {ctr.interlocked.increment();} \
	void release() const { if(ctr.interlocked.decrement()) delete this; } \
	private: RefInitInterlockedHolder ctr;

// base reference counted class
class RefInterlocked
{ 
	REFERENCE_INTERLOCKED;
};





// TRef<> is used for reference counting of objects derived from RefCounted
template <class T> class TRef
{
public:
	// default constructor
	TRef() 
		: pT( 0 ) 
	{}

	// constructor from plain pointer
	TRef(T* _ptr) 
		: pT( 0 ) 
	{ 
		assign(_ptr); 
	}

	// copy constructor
	TRef( const TRef& src )
		: pT( 0 )
	{
		assign(src.pT);
	}

	// destructor
	~TRef() 
	{ 
		assign(0); 
	}

	// assign a pointer
	TRef& operator= (T* _ptr)
	{
		assign(_ptr); 
		return *this; 
	}

	// assignment operator
	TRef& operator= (const TRef& src)
	{
		assign(src.pT); 
		return *this; 
	}

	// operator ->
	T* operator -> () const 
	{ 
		return pT; 
	}

	// return the plain pointer
	T* ptr() const
	{
		return pT; 
	}

	// comparison
	bool operator== (T* value) const
	{
		return pT == value;
	}

	// inverse comparison
	bool operator!= (T* value) const
	{
		return pT != value;
	}

private:
	T* pT;
	void assign(T* _pT)
	{
		if( _pT != 0 )
			_pT->addRef();
		if( pT != 0 )
			pT->release();
		pT = _pT;
	}
};


template<typename T>
class AutoReferenceCount
{
	T* v;
public:
	AutoReferenceCount(T* t) : v(t)
	{
		v->addRef();
	}
	~AutoReferenceCount()
	{
		v->release();
	}
private:  //prohibited
	AutoReferenceCount(const AutoReferenceCount& other);
	AutoReferenceCount& operator=(const AutoReferenceCount& other);
};



#endif // _Ref_h_

