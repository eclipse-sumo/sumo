#ifndef AGBUS_H
#define AGBUS_H

#include <iostream>
#include <string>

class AGBus
{
public:
	AGBus(std::string name, int depTime) :
		name(name),
		departureTime(depTime)
	{};
	AGBus(int depTime) :
		departureTime(depTime)
	{};
	void setName(std::string name);
	int getDeparture();
	std::string getName();
	void print();

private:
	std::string name;
	int departureTime;
};

#endif

