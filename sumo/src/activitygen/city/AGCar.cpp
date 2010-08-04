// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <sstream>
#include <string>
#include "AGCar.h"
#include "AGAdult.h"


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
string AGCar::createName()
{
	std::ostringstream os;
	os << this;
	return "c" + os.str();
}

bool AGCar::associateTo(AGAdult *pers)
{
	if(currentUser == NULL)
	{
		currentUser = pers;
		return true;
	}
	return false;
}

bool AGCar::isAssociated()
{
	return (currentUser != NULL);
}

string
AGCar::getName()
{
	return idName;
}

/****************************************************************************/
