
#ifndef STRATEGY_H
#define STRATEGY_H

#include <iostream>

using namespace std;

class FlyBehavior
{
public:
	virtual void Fly() = 0;
};

class FlyWithWings:public FlyBehavior
{
public:
	virtual void Fly();
};

class FlyNoWay: public FlyBehavior
{
public:
	virtual void Fly();
};

class Duck
{
public:
	Duck(){ cout<<"Duck::Constructor!"<<endl; }
	virtual ~Duck(){ delete FlyAble;}
	virtual void Display();
	virtual void PerformFlay();
	virtual void SetFly(FlyBehavior *flybehavior);
protected:
	FlyBehavior *FlyAble;
private:
};

class RedHeadDuck : public Duck
{
public:
	RedHeadDuck();

	virtual void Display();

};

#endif