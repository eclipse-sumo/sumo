// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <utils/common/RandHelper.h>
#include "AGPerson.h"


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
void
AGPerson::print()
{
	cout << "- Person: Age=" << age << endl;
}

int
AGPerson::getAge()
{
	return age;
}

bool
AGPerson::decision(float proba)
{
	if(RandHelper::rand(1000) < (int)(1000 * proba))
		return true;
	else
		return false;
}

bool
AGPerson::isAdult()
{
	return adult;
}

/****************************************************************************/
