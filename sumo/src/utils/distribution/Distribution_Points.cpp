/****************************************************************************/
/// @file    Distribution_Points.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// The description of a distribution by a curve
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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

#include <cassert>
#include "Distribution.h"
#include <utils/geom/PositionVector.h>
#include "Distribution_Points.h"
#include <utils/common/StdDefs.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
Distribution_Points::Distribution_Points(const std::string& id,
        const PositionVector& points,
        bool interpolating)
    : Distribution(id), myPoints(points), myProbabilitiesAreComputed(false),
      myInterpolateDist(interpolating) {}


Distribution_Points::~Distribution_Points() {}


SUMOReal
Distribution_Points::getMax() const {
    assert(myPoints.size() > 0);
    const Position& p = myPoints[-1];
    return p.x();
}


int
Distribution_Points::getAreaNo() const {
    return (int)myPoints.size() - 1;
}


SUMOReal
Distribution_Points::getAreaBegin(int index) const {
    return myPoints[index].x();
}


SUMOReal
Distribution_Points::getAreaEnd(int index) const {
    return myPoints[index + 1].x();
}


SUMOReal
Distribution_Points::getAreaPerc(int index) const {
    if (!myProbabilitiesAreComputed) {
        SUMOReal sum = 0;
        if (myInterpolateDist) {
            for (int i = 0; i < (int)myPoints.size() - 1; i++) {
                SUMOReal width = getAreaEnd(i) - getAreaBegin(i);
                SUMOReal minval = MIN2(myPoints[i].y(), myPoints[i].y());
                SUMOReal maxval = MAX2(myPoints[i].y(), myPoints[i].y());
                SUMOReal amount = minval * width + (maxval - minval) * width / (SUMOReal) 2.;
                myProbabilities.push_back(amount);
                sum += amount;
            }
        } else {
            for (int i = 0; i < (int)myPoints.size() - 1; i++) {
                myProbabilities.push_back(myPoints[i].y());
                sum += myPoints[i].y();
            }
        }
        // normalize
        if (myInterpolateDist) {
            for (int i = 0; i < (int)myPoints.size() - 1; i++) {
                myProbabilities[i] = myProbabilities[i] / sum;
            }
        } else {
            for (int i = 0; i < (int)myPoints.size() - 1; i++) {
                myProbabilities[i] = myProbabilities[i] / sum;
            }
        }
        myProbabilitiesAreComputed = true;
    }
    return myProbabilities[index];
}



/****************************************************************************/

