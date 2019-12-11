#include "pch.h"
#include "mordernc.h"


template<class T>
void mySwap(T& a, T& b)
{
	T tmp(a);
	a = b;
	b = tmp;
}

// rvalue
class example1
{
public:
	char *s;
	vector<int> intVec;

public:
	// default constructor
	example1()
	{
		s = nullptr;
	}

	// Parameterized constructor
	example1(const vector<int>& value_, const char *str)
	{
		intVec = value_;
		s = new char(strlen(str) + 1);
		strcpy_s(s, strlen(str), str);
	}
	
	// copy constructor
	// Default constructor does only shallow copy. Deep copy is possible only with user defined copy constructor
	example1(const example1& other)
	{
		intVec = other.intVec;

		delete[]s;
		s = new char(strlen(other.s) + 1);
		strcpy_s(s, strlen(other.s), other.s);
	}

	example1& operator=(example1& other)
	{
		intVec = other.intVec;

		delete[]s;
		s = new char(strlen(other.s) + 1);
		strcpy_s(s, strlen(other.s), other.s);

		return *this;
	}

	example1& operator=(example1&& other)
	{
		// Perform a cleanup that takes care of at least those parts of the
		// destructor that have side effects. Be sure to leave the object
		// in a destructible and assignable state.

		// Move semantics: exchange content between this and rhs
		return *this;
	}

	void setValue(const vector<int>& value_)
	{
		intVec = value_;

	}

};



void testRvalue()
{
	vector<int> intvec1 = {1, 2, 3};
	vector<int> intvec2 = { 4, 5, 6 };

	example1 r1(intvec1, "r1");
	example1 r2(intvec1, "r2");

	r1 = move(r2);

	swap(r1, r2);
	mySwap<example1>(r1, r2);
}

void testMordernC()
{
	testRvalue();
}