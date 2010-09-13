/****************************************************************************/
/// @file    AGPosition.cpp
/// @author  Piotr Woznica
/// @date    July 2010
/// @version $Id$
///
// References a street of the city and defines a position in this street
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

#include "AGPosition.h"
#include "router/ROEdge.h"
#include "utils/common/RandHelper.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <limits>


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
void
AGPosition::print() const throw()
{
	cout << "- AGPosition: *Street=" << street << " position=" << position << "/" << street->getLength() << endl;
}

bool
AGPosition::operator==(const AGPosition &pos) const throw()
{
	if(street == pos.street && position < pos.position+0.1 && position > pos.position-0.1)
		return true;
	else
		return false;
}

float
AGPosition::getDistanceTo(AGPosition *otherPos) const throw()
{
	return pos2d.distanceTo(otherPos->pos2d);
}

float
AGPosition::getDistanceTo(AGPosition otherPos) const throw()
{
	return getDistanceTo(&otherPos);
}

float
AGPosition::getDistanceTo(list<AGPosition> *myPositions) const throw()
{
	float minDist = std::numeric_limits<float>::infinity();
	float tempDist;
	list<AGPosition>::iterator itt;

	for(itt=myPositions->begin() ; itt!=myPositions->end() ; ++itt)
	{
		tempDist = this->getDistanceTo(&*itt);
		if(tempDist < minDist)
		{
			minDist = tempDist;
		}
	}
	return minDist;
}

float
AGPosition::getDistanceTo(map<int, AGPosition> *myPositions) const throw()
{
	float minDist = std::numeric_limits<float>::infinity();
	float tempDist;
	map<int, AGPosition>::iterator itt;

	for(itt=myPositions->begin() ; itt!=myPositions->end() ; ++itt)
	{
		tempDist = this->getDistanceTo(&(itt->second));
		if(tempDist < minDist)
		{
			minDist = tempDist;
		}
	}
	return minDist;
}

float
AGPosition::randomPositionInStreet(const AGStreet& s) throw()
{
	return RandHelper::rand(0.0, s.getLength());
}

void
AGPosition::compute2dPosition()
{
	// P = From + pos*(To - From) = pos*To + (1-pos)*From
	Position2D From = street->getEdge()->getFromNode()->getPosition();
	Position2D To = street->getEdge()->getToNode()->getPosition();

	pos2d.set(0,0);
	pos2d.add(To);
	pos2d.sub(From);
	pos2d.mul(position/street->getLength());
	pos2d.add(From);

}

/****************************************************************************/
