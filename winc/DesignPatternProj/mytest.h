
#ifndef MYTEST_H
#define MYTEST_H

#include <iostream>

using namespace std;

class Base
{
public:
	Base() {cout<<"Base::Constructor."<<endl; }
	virtual ~Base() {cout<<"Base::Destructor."<<endl;}
	void f(){ cout<<"Base:f()"<<endl;}
	virtual void vf() {cout<<"Base::vf()"<<endl;}
protected:

};

class Derived : public Base
{
public:
	Derived(){ cout<<"Derived::Constructor."<<endl; }
	~Derived() { cout<<"Derived::Destructor."<<endl; }
	void f(){ cout<<"Derived:f()"<<endl;}
	void vf(){cout<<"Drived:vf()"<<endl;}
protected:
};


#endif