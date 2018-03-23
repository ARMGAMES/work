
#ifndef SINGLETON_H
#define SINGLETON_H

#include <iostream>
using namespace std;

class Singleton
{
public:
	static Singleton* Instance();

private:
	static Singleton* singleton;
	Singleton(){}
	Singleton(const Singleton&){}
};

#endif