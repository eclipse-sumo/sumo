/****************************************************************************/
/// @file    AGPerson.cpp
/// @author  Piotr Woznica
/// @date    July 2010
/// @version $Id: AGPerson.cpp 8236 2010-02-10 11:16:41Z behrisch $
///
// Parent object of every person, contains age and any natural characteristic
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
