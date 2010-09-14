/****************************************************************************/
/// @file    AGStreet.cpp
/// @author  Piotr Woznica & Walter Bamberger
/// @date    July 2010
/// @version $Id$
///
// Represents a SUMO edge and contains people and work densities
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
// activitygen module
// Copyright 2010 TUM (Technische Universitaet Muenchen, http://www.tum.de/)
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

#include "AGStreet.h"
#include "router/ROEdge.h"
#include <iostream>


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
AGStreet::AGStreet(const ROEdge* edge, float popDensity, float workDensity) :
  edge(edge)
  {
	pop = static_cast<int>(popDensity * edge->getLength());
	work = static_cast<int>(workDensity * edge->getLength());
  }

/****************************************************************************/

void
AGStreet::print() const throw()
{
	cout << "- AGStreet: Name=" << edge->getID() << " Length=" << edge->getLength() << " pop=" << pop << " work=" << work << endl;
}

/****************************************************************************/

SUMOReal
AGStreet::getLength() const throw()
{
	return edge->getLength();
}

/****************************************************************************/

const string&
AGStreet::getName() const throw()
{
	return edge->getID();
}

/****************************************************************************/

int
AGStreet::getPopulation() const throw()
{
	return pop;
}

/****************************************************************************/

void
AGStreet::setPopulation(const int& population) throw()
{
	pop = population;
}

/****************************************************************************/

int
AGStreet::getWorkplaceNumber() const throw()
{
	return work;
}

/****************************************************************************/

void
AGStreet::setWorkplaceNumber(const int& workPositions) throw()
{
	work = workPositions;
}

/****************************************************************************/
