/****************************************************************************/
/// @file    AGStreet.cpp
/// @author  Piotr Woznica
/// @date    July 2010
/// @version $Id: AGStreet.cpp 8236 2010-02-10 11:16:41Z behrisch $
///
// represents a SUMO edge and contains people and work densities
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
#include "AGStreet.h"


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
void
AGStreet::print()
{
	cout << "- AGStreet: Name=" << edgeName << " Length=" << length << " pop=" << popDensity << " work=" << workDensity << endl;
}

float
AGStreet::getLength()
{
	return length;
}

string
AGStreet::getName()
{
	return edgeName;
}

int
AGStreet::getPopDensity()
{
	return this->popDensity;
}

int
AGStreet::getWorkDensity()
{
	return this->workDensity;
}

void
AGStreet::setPopulation(int pop)
{
	popDensity = pop;
}

void
AGStreet::setWorkPositions(int work)
{
	workDensity = work;
}

void
AGStreet::computeLength()
{
	length = net->getEdge(edgeName)->getLength();
}

/****************************************************************************/
