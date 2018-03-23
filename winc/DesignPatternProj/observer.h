

#ifndef OBSERVER_H
#define OBSERVER_H

#include <list>

using namespace std;

class Subject;
class WeatherData;

class Observer
{
public:
	virtual ~Observer();
	virtual void UpdateObserver(Subject* s) = 0;
protected:
	Observer(){}
};

class Subject
{
public:
	virtual ~Subject();
	virtual void AttachObserver(Observer* observer);
	virtual void DettachObserver(Observer* observer);

protected:
	virtual void Notify();
	Subject();
private:
	list<Observer*> *_observers;
};

class WeatherData:public Subject
{
public:
	WeatherData();
	float GetTemperature();
	float GetHumidity();
	float GetPressure();

	void SetMeasurement(float temp, float hum, float pressure);
private:
	float _temperature;
	float _humidity;
	float _pressure;
};

class CurrentConditionDisplay:public Observer
{
public:
	CurrentConditionDisplay(WeatherData* wd);
	virtual ~CurrentConditionDisplay();

	virtual void UpdateObserver(Subject *s);
	void Display();

	Subject* GetSubject() const {return _wd;} // not good idea to expose weather data.
protected:
	CurrentConditionDisplay(){}

private:
	WeatherData* _wd;
};

class ForecastDisplay:public Observer
{
public:
	ForecastDisplay(WeatherData* wd);
	virtual ~ForecastDisplay();

	virtual void UpdateObserver(Subject *s);
	void Display();

	Subject* GetSubject() const {return _wd;}

protected:
	ForecastDisplay(){}

private:
	WeatherData* _wd;
};



#endif