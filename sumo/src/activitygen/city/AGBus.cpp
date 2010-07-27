#include <iostream>
#include <string>
#include "AGBus.h"

using namespace std;

void AGBus::setName(std::string name)
{
	this->name= name;
}

int
AGBus::getDeparture()
{
	return departureTime;
}

string
AGBus::getName()
{
	return name;
}

void
AGBus::print()
{
	cout << "- Bus:" << " name=" << name << " depTime=" << departureTime << endl;
}
