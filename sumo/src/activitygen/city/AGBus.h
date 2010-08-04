#ifndef AGBUS_H
#define AGBUS_H

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <string>


// ===========================================================================
// class definitions
// ===========================================================================
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

/****************************************************************************/
