/****************************************************************************/
/// @file    ROJTREdge.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Yun-Pang Floetteroed
/// @date    Tue, 20 Jan 2004
/// @version $Id$
///
// An edge the jtr-router may route through
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2004-2017 DLR (http://www.dlr.de/) and contributors
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

#include <algorithm>
#include <cassert>
#include <utils/common/MsgHandler.h>
#include <utils/common/RandHelper.h>
#include "ROJTREdge.h"
#include <utils/common/RandomDistributor.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
ROJTREdge::ROJTREdge(const std::string& id, RONode* from, RONode* to, int index, const int priority)
    : ROEdge(id, from, to, index, priority) {}


ROJTREdge::~ROJTREdge() {
    for (FollowerUsageCont::iterator i = myFollowingDefs.begin(); i != myFollowingDefs.end(); ++i) {
        delete(*i).second;
    }
}


void
ROJTREdge::addSuccessor(ROEdge* s, std::string) {
    ROEdge::addSuccessor(s);
    ROJTREdge* js = static_cast<ROJTREdge*>(s);
    if (myFollowingDefs.find(js) == myFollowingDefs.end()) {
        myFollowingDefs[js] = new ValueTimeLine<SUMOReal>();
    }
}


void
ROJTREdge::addFollowerProbability(ROJTREdge* follower, SUMOReal begTime,
                                  SUMOReal endTime, SUMOReal probability) {
    FollowerUsageCont::iterator i = myFollowingDefs.find(follower);
    if (i == myFollowingDefs.end()) {
        WRITE_ERROR("The edges '" + getID() + "' and '" + follower->getID() + "' are not connected.");
        return;
    }
    (*i).second->add(begTime, endTime, probability);
}


ROJTREdge*
ROJTREdge::chooseNext(const ROVehicle* const veh, SUMOReal time, const std::set<const ROEdge*>& avoid) const {
    // if no usable follower exist, return 0
    //  their probabilities are not yet regarded
    if (myFollowingEdges.size() == 0 || (veh != 0 && allFollowersProhibit(veh))) {
        return 0;
    }
    // gather information about the probabilities at this time
    RandomDistributor<ROJTREdge*> dist;
    // use the loaded definitions, first
    for (FollowerUsageCont::const_iterator i = myFollowingDefs.begin(); i != myFollowingDefs.end(); ++i) {
        if (avoid.count(i->first) == 0) {
            if ((veh == 0 || !(*i).first->prohibits(veh)) && (*i).second->describesTime(time)) {
                dist.add((*i).second->getValue(time), (*i).first);
            }
        }
    }
    // if no loaded definitions are valid for this time, try to use the defaults
    if (dist.getOverallProb() == 0) {
        for (int i = 0; i < (int)myParsedTurnings.size(); ++i) {
            if (avoid.count(myFollowingEdges[i]) == 0) {
                if (veh == 0 || !myFollowingEdges[i]->prohibits(veh)) {
                    dist.add(myParsedTurnings[i], static_cast<ROJTREdge*>(myFollowingEdges[i]));
                }
            }
        }
    }
    // if still no valid follower exists, return null
    if (dist.getOverallProb() == 0) {
        return 0;
    }
    // return one of the possible followers
    return dist.get();
}


void
ROJTREdge::setTurnDefaults(const std::vector<SUMOReal>& defs) {
    // I hope, we'll find a less ridiculous solution for this
    std::vector<SUMOReal> tmp(defs.size()*myFollowingEdges.size(), 0);
    // store in less common multiple
    for (int i = 0; i < (int)defs.size(); ++i) {
        for (int j = 0; j < (int)myFollowingEdges.size(); ++j) {
            tmp[i * myFollowingEdges.size() + j] = (SUMOReal)(defs[i] / 100.0 / (myFollowingEdges.size()));
        }
    }
    // parse from less common multiple
    for (int i = 0; i < (int)myFollowingEdges.size(); ++i) {
        SUMOReal value = 0;
        for (int j = 0; j < (int)defs.size(); ++j) {
            value += tmp[i * defs.size() + j];
        }
        myParsedTurnings.push_back((SUMOReal) value);
    }
}



/****************************************************************************/

