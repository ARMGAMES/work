
#ifndef DECORATOR_H
#define DECORATOR_H

#include <iostream>

using namespace std;

const int BUFFERSIZE = 10;

class Beverage
{
public:
	virtual ~Beverage(){}
	virtual float cost() = 0;
	virtual char* GetDescription() =0;

protected:
	char* _description;
};

class DarkRoast : public Beverage
{
public:
	DarkRoast() {  _description = "DarkRoast"; }

	float cost() { return 1.3;}
	char* GetDescription() { return _description; }
};

class Expresso : public Beverage
{
public:
	Expresso() { _description = "Expresso";}
	float cost() { return 1.5; }
	char* GetDescription() { return _description; }
};

class CondimentDecorator:public Beverage
{
public:
	CondimentDecorator(Beverage* beverage) { _beverage = beverage;}
	virtual float cost() { return _beverage->cost(); }
	virtual char* GetDescription()  { return _beverage->GetDescription(); }
	virtual ~CondimentDecorator(){}

private:
	Beverage* _beverage;

};

class Morcha : public CondimentDecorator
{
public:
	Morcha(Beverage* beverage):CondimentDecorator(beverage) {}

	float cost() { return CondimentDecorator::cost() + 0.3; }

	char* GetDescription()
	{
		char* description = new char[128];
		strcpy(description, CondimentDecorator::GetDescription());
		strcat(description, " morcha");
		return description;
	}

};


class Whip : public CondimentDecorator
{
public:
	Whip(Beverage* beverage):CondimentDecorator(beverage) {}

	float cost()
	{
		return CondimentDecorator::cost() + 0.4;
	}

	char* GetDescription()
	{
		char* description = new char[128];
		strcpy(description, CondimentDecorator::GetDescription());
		strcat(description, " whip");
		return description;
	}

};

#endif