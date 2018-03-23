
#ifndef COMMAND_H
#define COMMAND_H

#include <iostream>

using namespace std;

class Command
{
public:
	virtual ~Command(){}
	virtual void Execute() = 0;

protected:
	Command(){}
};

class Light
{
public:
	void TurnonLight()
	{
		cout<< "turn on the light!!"<<endl;
	}
};


class LightOnCommand : public Command
{
public:
	LightOnCommand(Light* light):_light(light) {}
	virtual void Execute()
	{
		_light->TurnonLight();				
	}
private:
	Light* _light;
};


#endif