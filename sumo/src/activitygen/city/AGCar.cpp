/****************************************************************************/
/// @file    AGCar.cpp
/// @author  Piotr Woznica
/// @date    July 2010
/// @version $Id$
///
// cars owned by people of the city: included in households.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2010 TUM (Technische Universität München, http://www.tum.de/)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/

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
string AGCar::createName(int idHH, int idCar)
{
	std::ostringstream os;
	os << "h" << idHH << "c" << idCar;
	return os.str();
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
