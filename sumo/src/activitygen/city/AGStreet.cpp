/****************************************************************************/
/// @file    AGStreet.cpp
/// @author  Piotr Woznica
/// @date    July 2010
/// @version $Id$
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
	cout << "- AGStreet: Name=" << edgeName << " Length=" << length << " pop=" << pop << " work=" << work << endl;
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
	return this->pop;
}

int
AGStreet::getWorkDensity()
{
	return this->work;
}

void
AGStreet::setPopulation(int population)
{
	pop = population;
}

void
AGStreet::setWorkPositions(int workPositions)
{
	work = workPositions;
}

void
AGStreet::computeLength()
{
	length = net->getEdge(edgeName)->getLength();
}

void
AGStreet::computeDesnsityEquivalents()
{
	pop = (int) (popDensity * length);
	work = (int) (workDensity * length);
}

/****************************************************************************/
