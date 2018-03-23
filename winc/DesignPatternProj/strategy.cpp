
#include <iostream>
#include "strategy.h"

using namespace std;

void FlyWithWings::Fly()
{
	cout<<"Flying with wings"<<endl;
}

void FlyNoWay::Fly()
{
	cout<<"Fly no way!!"<<endl;
}

void Duck::PerformFlay()
{ 
	this->FlyAble->Fly();
}

void Duck::Display()
{
	cout<<"Duck::Display base duck!!"<<endl;
}

void Duck::SetFly(FlyBehavior* flybehavior)
{
	if (this->FlyAble != 0) 
	{
		delete FlyAble;
	}
	FlyAble = flybehavior;
}

RedHeadDuck::RedHeadDuck()
{
	this->FlyAble = new FlyWithWings();
	cout<<"RedHeadDuck::Constructor"<<endl;
}

void RedHeadDuck::Display()
{
	cout<<"RedHeadDuck::Display: flying redhead duck."<<endl;
}