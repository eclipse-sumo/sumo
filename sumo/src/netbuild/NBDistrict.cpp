/****************************************************************************/
/// @file    NBDistrict.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// A class representing a single district
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
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
#include <vector>
#include <string>
#include <utility>
#include <iostream>
#include <algorithm>
#include <utils/common/Named.h>
#include <utils/common/StringUtils.h>
#include <utils/iodevices/OutputDevice.h>
#include "NBEdge.h"
#include "NBDistrict.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member method definitions
// ===========================================================================
NBDistrict::NBDistrict(const std::string& id, const Position& pos)
    : Named(StringUtils::convertUmlaute(id)),
      myPosition(pos) {}


NBDistrict::NBDistrict(const std::string& id)
    : Named(id), myPosition(0, 0) {}


NBDistrict::~NBDistrict() {}


// -----------  Applying offset
void
NBDistrict::reshiftPosition(SUMOReal xoff, SUMOReal yoff) {
    myPosition.reshiftRotate(xoff, yoff, 0);
    myShape.reshiftRotate(xoff, yoff, 0);
}


bool
NBDistrict::addSource(NBEdge* const source, SUMOReal weight) {
    EdgeVector::iterator i = find(mySources.begin(), mySources.end(), source);
    if (i != mySources.end()) {
        return false;
    }
    mySources.push_back(source);
    mySourceWeights.push_back(weight);
    assert(source->getID() != "");
    return true;
}


bool
NBDistrict::addSink(NBEdge* const sink, SUMOReal weight) {
    EdgeVector::iterator i = find(mySinks.begin(), mySinks.end(), sink);
    if (i != mySinks.end()) {
        return false;
    }
    mySinks.push_back(sink);
    mySinkWeights.push_back(weight);
    assert(sink->getID() != "");
    return true;
}


void
NBDistrict::setCenter(const Position& pos) {
    myPosition = pos;
}


void
NBDistrict::replaceIncoming(const EdgeVector& which, NBEdge* const by) {
    // temporary structures
    EdgeVector newList;
    WeightsCont newWeights;
    SUMOReal joinedVal = 0;
    // go through the list of sinks
    EdgeVector::iterator i = mySinks.begin();
    WeightsCont::iterator j = mySinkWeights.begin();
    for (; i != mySinks.end(); i++, j++) {
        NBEdge* tmp = (*i);
        SUMOReal val = (*j);
        if (find(which.begin(), which.end(), tmp) == which.end()) {
            // if the current edge shall not be replaced, add to the
            //  temporary list
            newList.push_back(tmp);
            newWeights.push_back(val);
        } else {
            // otherwise, skip it and add its weight to the one to be inserted
            //  instead
            joinedVal += val;
        }
    }
    // add the one to be inserted instead
    newList.push_back(by);
    newWeights.push_back(joinedVal);
    // assign to values
    mySinks = newList;
    mySinkWeights = newWeights;
}


void
NBDistrict::replaceOutgoing(const EdgeVector& which, NBEdge* const by) {
    // temporary structures
    EdgeVector newList;
    WeightsCont newWeights;
    SUMOReal joinedVal = 0;
    // go through the list of sinks
    EdgeVector::iterator i = mySources.begin();
    WeightsCont::iterator j = mySourceWeights.begin();
    for (; i != mySources.end(); i++, j++) {
        NBEdge* tmp = (*i);
        SUMOReal val = (*j);
        if (find(which.begin(), which.end(), tmp) == which.end()) {
            // if the current edge shall not be replaced, add to the
            //  temporary list
            newList.push_back(tmp);
            newWeights.push_back(val);
        } else {
            // otherwise, skip it and add its weight to the one to be inserted
            //  instead
            joinedVal += val;
        }
    }
    // add the one to be inserted instead
    newList.push_back(by);
    newWeights.push_back(joinedVal);
    // assign to values
    mySources = newList;
    mySourceWeights = newWeights;
}


void
NBDistrict::removeFromSinksAndSources(NBEdge* const e) {
    size_t i;
    for (i = 0; i < mySinks.size(); ++i) {
        if (mySinks[i] == e) {
            mySinks.erase(mySinks.begin() + i);
            mySinkWeights.erase(mySinkWeights.begin() + i);
        }
    }
    for (i = 0; i < mySources.size(); ++i) {
        if (mySources[i] == e) {
            mySources.erase(mySources.begin() + i);
            mySourceWeights.erase(mySourceWeights.begin() + i);
        }
    }
}


void
NBDistrict::addShape(const PositionVector& p) {
    myShape = p;
}



/****************************************************************************/

