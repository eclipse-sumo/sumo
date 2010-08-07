/****************************************************************************/
/// @file    AGSchool.cpp
/// @author  Piotr Woznica
/// @date    July 2010
/// @version $Id$
///
// Correspond to given ages and referenced by children. has a precise location.
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
#include <string>
#include "AGSchool.h"
#include "AGPosition.h"


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
void
AGSchool::print()
{
	cout << "- school: " << " placeNbr=" << capacity << " hours=[" << opening << ";" << closing << "] ages=[" << beginAge << ";" << endAge << "]" << endl;
}

int
AGSchool::getPlaces()
{
	return capacity;
}

bool
AGSchool::addNewChild()
{
	if(capacity > 0)
	{
		--capacity;
		return true;
	}
	return false;
}

bool
AGSchool::removeChild()
{
	if(capacity < initCapacity)
	{
		++capacity;
		return true;
	}
	return false;
}

bool
AGSchool::acceptThisAge(int age)
{
	if(age <= endAge && age >= beginAge)
		return true;
	return false;
}

int
AGSchool::getBeginAge()
{
	return beginAge;
}

int
AGSchool::getEndAge()
{
	return endAge;
}

AGPosition
AGSchool::getPosition()
{
	return location;
}

int
AGSchool::getClosingHour()
{
	return closing;
}

int
AGSchool::getOpeningHour()
{
	return opening;
}

/****************************************************************************/
