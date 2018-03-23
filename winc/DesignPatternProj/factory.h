
#ifndef FACTORY_H
#define FACTORY_H

#include <iostream>
#include <string>
#include <list>

using namespace std;

class Pizza
{
public:
	virtual ~Pizza() {}
	virtual void Prepare()
	{
		cout<<"Preparing pizza:"<<_name<<endl;
		cout<<"Tossing dough:"<<_dough<<endl;
		cout<<"Adding sauce:"<<_sauce<<endl;
		cout<<"Adding toppings:";
		for(list<string >::iterator it = _toppings.begin(); it != _toppings.end(); it++)
		{
			cout<< *it <<" ";
		}
		cout<<endl;
	}
	virtual void Bake() 
	{ cout<<"Pizza::Bake - Bake for 30 minutes."<<endl; }
	virtual void Cut()
	{ cout<<"Pizza::Cut - Cut pizza to diagonal slices."<<endl;}
	virtual void Box()
	{ cout<<"Pizza::Box - place pizza into box"<<endl; }
	string GetName() const
	{ return _name; }

protected:
	Pizza() {}

	string _name;
	string _dough;
	string _sauce;
	list<string> _toppings;

};

class PizzaStore
{
public:
	virtual ~PizzaStore(){}
	virtual Pizza* CreatePizza(string type) = 0;
	virtual Pizza* OrderPizza(string type); 
protected:
	PizzaStore(){}
private:
};

class NYPizzaStore:public PizzaStore
{
public:
	virtual Pizza* CreatePizza(string type);
};

class NYCheesePizza : public Pizza
{
public:
	NYCheesePizza()
	{
		_name = "New York Cheese Pizza";
		_dough = "Thin Crust Dough";
		_sauce = "Marinana Sauce";
		_toppings.push_back("Regaino cheese");
		_toppings.push_back("Bacon");
	}

	virtual void Cut()
	{ cout<<"NYCheesePizza::Cut - Cut into round slices"<<endl; }
};

class NYMeatPizza: public Pizza
{
public:
	NYMeatPizza()
	{
		_name = "New York Meat Pizza";
		_dough = "Thick Douah";
		_sauce = "Sausage";
		_toppings.push_back("meat ball");
		_toppings.push_back("beef");
	}

	virtual void Bake()
	{
		cout<<"NYMeatPizza::Bake - Bake 1 hour"<<endl;
	}
};

#endif