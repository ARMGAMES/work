
#include "observer.h"
#include <iostream>

using namespace std;

Subject::Subject()
{
	_observers = new list<Observer*>();
}

Subject::~Subject()
{
	delete _observers;
}

void Subject::AttachObserver(Observer *observer)
{
	_observers->push_back(observer);
}

void Subject::DettachObserver(Observer *observer)
{
	_observers->remove(observer);
}

void Subject::Notify()
{
	for( list<Observer*>::iterator it = _observers->begin(); it != _observers->end(); it++)
	{
		(*it)->UpdateObserver(this);
	}
}

Observer::~Observer()
{
}

WeatherData::WeatherData()
{
}

float WeatherData::GetTemperature()
{
	return this->_temperature;
}

float WeatherData::GetHumidity()
{
	return this->_humidity;
}

float WeatherData::GetPressure()
{
	return this->_pressure;
}

void WeatherData::SetMeasurement(float temp, float hum, float pressure)
{
	_temperature = temp;
	_humidity = hum;
	_pressure = pressure;
	Notify();
}

CurrentConditionDisplay::CurrentConditionDisplay(WeatherData *wd)
{
	_wd = wd;
	_wd->AttachObserver(this);
}

CurrentConditionDisplay::~CurrentConditionDisplay()
{
	_wd->DettachObserver(this);
	delete _wd;
}

void CurrentConditionDisplay::UpdateObserver(Subject *s)
{
	if (s == _wd)
	{
		Display();
	}
}

void CurrentConditionDisplay::Display()
{
	cout<<"CurrentConditionDisplay::Display - "<<_wd->GetTemperature()<<", "<<_wd->GetHumidity()<<", "<<_wd->GetPressure()<<endl;
}

ForecastDisplay::ForecastDisplay(WeatherData *wd)
{
	_wd = wd;
	_wd->AttachObserver(this);
}

ForecastDisplay::~ForecastDisplay()
{
	_wd->DettachObserver(this);
	delete _wd;
}

void ForecastDisplay::UpdateObserver(Subject *s)
{
	if (s == _wd)
	{
		Display();
	}
}

void ForecastDisplay::Display()
{
	cout<<"ForecastDisplay::Display - "<<_wd->GetTemperature()<<", "<<_wd->GetHumidity()<<", "<<_wd->GetPressure()<<endl;
}
