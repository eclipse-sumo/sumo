/****************************************************************************/
/// @file    AGPosition.cpp
/// @author  Piotr Woznica
/// @author  Walter Bamberger
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    July 2010
/// @version $Id$
///
// References a street of the city and defines a position in this street
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
// activitygen module
// Copyright 2010 TUM (Technische Universitaet Muenchen, http://www.tum.de/)
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
// method definitions
// ===========================================================================
AGPosition::AGPosition(const AGStreet& str, SUMOReal pos) :
    street(&str), position(pos), pos2d(compute2dPosition()) {
}


AGPosition::AGPosition(const AGStreet& str) :
    street(&str), position(randomPositionInStreet(str)), pos2d(compute2dPosition()) {
}


void
AGPosition::print() const {
    std::cout << "- AGPosition: *Street=" << street << " position=" << position << "/" << street->getLength() << std::endl;
}


bool
AGPosition::operator==(const AGPosition& pos) const {
    return pos2d.almostSame(pos.pos2d);
}


SUMOReal
AGPosition::distanceTo(const AGPosition& otherPos) const {
    return pos2d.distanceTo(otherPos.pos2d);
}


SUMOReal
AGPosition::minDistanceTo(const std::list<AGPosition>& positions) const {
    SUMOReal minDist = std::numeric_limits<SUMOReal>::infinity();
    SUMOReal tempDist;
    std::list<AGPosition>::const_iterator itt;

    for (itt = positions.begin(); itt != positions.end(); ++itt) {
        tempDist = this->distanceTo(*itt);
        if (tempDist < minDist) {
            minDist = tempDist;
        }
    }
    return minDist;
}


SUMOReal
AGPosition::minDistanceTo(const std::map<int, AGPosition>& positions) const {
    SUMOReal minDist = std::numeric_limits<SUMOReal>::infinity();
    SUMOReal tempDist;
    std::map<int, AGPosition>::const_iterator itt;

    for (itt = positions.begin(); itt != positions.end(); ++itt) {
        tempDist = this->distanceTo(itt->second);
        if (tempDist < minDist) {
            minDist = tempDist;
        }
    }
    return minDist;
}


const AGStreet&
AGPosition::getStreet() const {
    return *street;
}


SUMOReal
AGPosition::getPosition() const {
    return position;
}


SUMOReal
AGPosition::randomPositionInStreet(const AGStreet& s) {
    return RandHelper::rand(0.0, s.getLength());
}


Position
AGPosition::compute2dPosition() const {
    // P = From + pos*(To - From) = pos*To + (1-pos)*From
    Position From = street->edge->getFromNode()->getPosition();
    Position To = street->edge->getToNode()->getPosition();
    Position position2d(To);

    position2d.sub(From);
    position2d.mul(position / street->getLength());
    position2d.add(From);

    return position2d;
}

/****************************************************************************/
