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
#include "AGBus.h"


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
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

/****************************************************************************/
