#ifndef AGCAR_H
#define AGCAR_H

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
#include "AGAdult.h"


// ===========================================================================
// class definitions
// ===========================================================================
class AGCar
{
public:
	AGCar(std::string name) :
		idName(name)
	{};
	AGCar() :
		idName(createName())
	{};
	bool associateTo(AGAdult *pers);
	bool isAssociated();
	std::string getName();

private:
	std::string createName();

	std::string idName;
	AGAdult* currentUser;

};

#endif
 
/****************************************************************************/
