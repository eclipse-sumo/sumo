/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// activitygen module
// Copyright 2010 TUM (Technische Universitaet Muenchen, http://www.tum.de/)
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
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


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include "AGPosition.h"
#include "AGStreet.h"
#include "router/ROEdge.h"
#include "utils/common/RandHelper.h"
#include <iostream>
#include <limits>


// ===========================================================================
// method definitions
// ===========================================================================
AGPosition::AGPosition(const AGStreet& str, double pos) :
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


double
AGPosition::distanceTo(const AGPosition& otherPos) const {
    return pos2d.distanceTo(otherPos.pos2d);
}


double
AGPosition::minDistanceTo(const std::list<AGPosition>& positions) const {
    double minDist = std::numeric_limits<double>::infinity();
    double tempDist;
    std::list<AGPosition>::const_iterator itt;

    for (itt = positions.begin(); itt != positions.end(); ++itt) {
        tempDist = this->distanceTo(*itt);
        if (tempDist < minDist) {
            minDist = tempDist;
        }
    }
    return minDist;
}


double
AGPosition::minDistanceTo(const std::map<int, AGPosition>& positions) const {
    double minDist = std::numeric_limits<double>::infinity();
    double tempDist;
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


double
AGPosition::getPosition() const {
    return position;
}


double
AGPosition::randomPositionInStreet(const AGStreet& s) {
    return RandHelper::rand(0.0, s.getLength());
}


Position
AGPosition::compute2dPosition() const {
    // P = From + pos*(To - From) = pos*To + (1-pos)*From
    Position From = street->getFromJunction()->getPosition();
    Position To = street->getToJunction()->getPosition();
    Position position2d(To);

    position2d.sub(From);
    position2d.mul(position / street->getLength());
    position2d.add(From);

    return position2d;
}

/****************************************************************************/
