/****************************************************************************/
/// @file    AGPosition.h
/// @author  Piotr Woznica
/// @date    July 2010
/// @version $Id$
///
// references a street of the city and defines a position in this street
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
#ifndef POSITION_H
#define POSITION_H

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <list>
#include <map>
#include "AGStreet.h"
#include "../../utils/geom/Position2D.h"


// ===========================================================================
// class definitions
// ===========================================================================
class AGPosition
{
public:
	AGPosition(AGStreet *str, float pos) :
	  street(str), position(pos), pos2d()
	  {
		compute2dPosition();
	  };
	AGPosition(AGStreet *str) :
	  street(str), position((float)randomPositionInStreet()), pos2d()
	  {
		compute2dPosition();
	  };
	
	bool operator==(const AGPosition &pos);

	void print();
	float getDistanceTo(AGPosition *otherPos);
	float getDistanceTo(AGPosition otherPos);
	float getDistanceTo(std::list<AGPosition> *positions);
	float getDistanceTo(std::map<int, AGPosition> *myPositions);

	AGStreet *street;
	float position;
	Position2D pos2d;

private:
	float randomPositionInStreet();
	void compute2dPosition();


};

#endif

/****************************************************************************/
