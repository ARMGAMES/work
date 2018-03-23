
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "strategy.h"
#include "observer.h"
#include "decorator.h"
#include "mytest.h"
#include "factory.h"
//#include "algorithm.h"
#include "singleton.h"
#include "command.h"


using namespace  std;

void TestVirtualDestructor()
{
	Base* pb = new Derived();
	pb->f();
	pb->vf();

	delete pb;
}

void TestStrategy()
{
		// stratety pattern 
	Duck *dk = new RedHeadDuck();

	dk->PerformFlay();
	dk->Display();

	dk->SetFly(new FlyNoWay());
	dk->PerformFlay();
}

void TestObserver()
{
	// observer pattern
	WeatherData *wd = new WeatherData();
	CurrentConditionDisplay *cd = new CurrentConditionDisplay(wd);
	ForecastDisplay *fd = new ForecastDisplay(wd);

	wd->SetMeasurement(1.2,2.3,3.4);

	fd->GetSubject()->DettachObserver(fd);

	wd->SetMeasurement(2.2,3.3,4.4);
}

void TestDecorator()
{
	// test Decorator pattern
	Beverage* bg = new Morcha( new Morcha(new Whip( new DarkRoast())));
	cout<<bg->GetDescription()<<", "<<bg->cost()<<endl;
}

void TestFactory()
{
	PizzaStore *ps = new NYPizzaStore();
	ps->OrderPizza("cheese");

	ps->OrderPizza("meat");

}

void TestString()
{
	string string1("12345");
	// Set the iterator indicate the first element
	string::iterator it = string1.begin();
	// This may copy the first to the second or
	// use reference counting to simulate a copy
	string string2 = string1;
	// Either way, this statement may ONLY modify first
	*it = '0';
	cout << "string1 = " << string1 << ", &string1 = "<<&string1<<endl;
	cout << "string2 = " << string2 << ", &string2 = "<<&string2<<endl;

	string s1
		("What is the sound of one clam napping?");
	string s2
		("Anything worth doing is worth overdoing.");
	string s3("I saw Elvis in a UFO.");
	// Copy the first 8 chars
	string s4(s1, 0, 8);
	// Copy 6 chars from the middle of the source
	string s5(s2, 15, 6);
	// Copy from middle to end
	string s6(s3, 6, 15);
	// Copy all sorts of stuff
	string quoteMe = s4 + "that" +
		// substr() copies 10 chars at element 20
		s1.substr(20, 10) + s5 +
		// substr() copies up to either 100 char
		// or eos starting at element 5
		"with" + s3.substr(5, 100) +
		// OK to copy a single char this way
		s1.substr(0, 1);
	cout << quoteMe << endl;

	s1 = "abcabcabc";
	cout<<s1<<endl;

	// STL replace algrithm
	replace(s1.begin(), s1.end(), 'a', 'h');
	cout<<s1<<endl;

	string replacement = "b";
	s1.replace(s1.size(), replacement.size(), replacement);

	cout<<s1<<endl;
}

void TestSingleton()
{
	Singleton::Instance();
}

void TestCommand()
{
	Light* light = new Light;
	LightOnCommand* lightoncommand = new LightOnCommand(light);

	lightoncommand->Execute();
}


void TestSTL()
{
	vector<int> v;
	v.push_back(15);
	cout << v.size();
}

bool Foo(const char*  s, bool x)
{
	std::cout <<s;
	return x;
}

class A
{
public:
	A() {cout <<1;}
	~A() {cout<<2;}
	virtual void foo(){}
};

class B : public A
{
public:
	B() {cout<<3;}
	~B() {cout <<4;}
	void foo(){}
};

void Foo(int x) {cout << "1";}
void Foo(int* x) {cout << "2";}
void Foo(int** x) {cout << "3";}
void Bar(int & x)
{
	Foo(&x);
	int* y = (int*)&x;
	Foo(y);
	Foo(*y);
	Foo(&y);
}

int main()
{
	B* b = new B();
	b->foo();
	
	//TestStrategy();

	//TestObserver();

	//TestDecorator();

	//TestString();

  //TestFactory();

	//TestSingleton();

//	TestCommand();

//	TestSTL();

/*	if ((Foo("1", true) || Foo("2", false)) && Foo("3", false))
	{
		Foo("4", false);
	}
	if ((Foo("5", false) && Foo("6", true) )|| Foo("7", true))
	{
		Foo("8", true);
	}
	*/

	return 0;
}