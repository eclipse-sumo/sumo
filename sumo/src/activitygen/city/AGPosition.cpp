/****************************************************************************/
/// @file    AGPosition.cpp
/// @author  Piotr Woznica & Walter Bamberger
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
#include "AGStreet.h"
#include "router/ROEdge.h"
#include "utils/common/RandHelper.h"
#include <iostream>
#include <limits>


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
AGPosition::AGPosition(const AGStreet& str, SUMOReal pos) throw() :
  street(&str), position(pos), pos2d(compute2dPosition())
  {
  }

/****************************************************************************/

AGPosition::AGPosition(const AGStreet& str) throw() :
  street(&str), position(randomPositionInStreet(str)), pos2d(compute2dPosition())
  {
  }

/****************************************************************************/

void
AGPosition::print() const throw()
{
	cout << "- AGPosition: *Street=" << street << " position=" << position << "/" << street->getLength() << endl;
}

/****************************************************************************/

bool
AGPosition::operator==(const AGPosition &pos) const throw()
{
	return pos2d.almostSame(pos.pos2d);
}

/****************************************************************************/

SUMOReal
AGPosition::distanceTo(const AGPosition& otherPos) const throw()
{
	return pos2d.distanceTo(otherPos.pos2d);
}

/****************************************************************************/

SUMOReal
AGPosition::minDistanceTo(const list<AGPosition>& positions) const throw()
{
	SUMOReal minDist = numeric_limits<SUMOReal>::infinity();
	SUMOReal tempDist;
	list<AGPosition>::const_iterator itt;

	for(itt=positions.begin() ; itt!=positions.end() ; ++itt)
	{
		tempDist = this->distanceTo(*itt);
		if(tempDist < minDist)
		{
			minDist = tempDist;
		}
	}
	return minDist;
}

/****************************************************************************/

SUMOReal
AGPosition::minDistanceTo(const map<int, AGPosition>& positions) const throw()
{
	SUMOReal minDist = numeric_limits<SUMOReal>::infinity();
	SUMOReal tempDist;
	map<int, AGPosition>::const_iterator itt;

	for(itt=positions.begin() ; itt!=positions.end() ; ++itt)
	{
		tempDist = this->distanceTo(itt->second);
		if(tempDist < minDist)
		{
			minDist = tempDist;
		}
	}
	return minDist;
}

/****************************************************************************/

const AGStreet&
AGPosition::getStreet() const throw()
{
	return *street;
}

/****************************************************************************/

SUMOReal
AGPosition::getPosition() const throw()
{
	return position;
}

/****************************************************************************/

SUMOReal
AGPosition::randomPositionInStreet(const AGStreet& s) throw()
{
	return RandHelper::rand(0.0, s.getLength());
}

/****************************************************************************/

Position2D
AGPosition::compute2dPosition() const throw()
{
	// P = From + pos*(To - From) = pos*To + (1-pos)*From
	Position2D From = street->edge->getFromNode()->getPosition();
	Position2D To = street->edge->getToNode()->getPosition();
	Position2D position2d(To);

	position2d.sub(From);
	position2d.mul(position / street->getLength());
	position2d.add(From);

	return position2d;
}

/****************************************************************************/
