/****************************************************************************/
/// @file    ROJTREdge.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Jan 2004
/// @version $Id$
///
// An edge the jtr-router may route through
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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
ROJTREdge::ROJTREdge(const std::string &id, RONode *from, RONode *to, unsigned int index) throw()
        : ROEdge(id, from, to, index, false) {}


ROJTREdge::~ROJTREdge() throw() {
    for (FollowerUsageCont::iterator i=myFollowingDefs.begin(); i!=myFollowingDefs.end(); i++) {
        delete(*i).second;
    }
}


void
ROJTREdge::addFollower(ROEdge *s) throw() {
    ROEdge::addFollower(s);
    ROJTREdge *js = static_cast<ROJTREdge*>(s);
    if (myFollowingDefs.find(js)==myFollowingDefs.end()) {
        myFollowingDefs[js] = new ValueTimeLine<SUMOReal>();
    }
}


void
ROJTREdge::addFollowerProbability(ROJTREdge *follower, SUMOTime begTime,
                                  SUMOTime endTime, SUMOReal probability) {
    FollowerUsageCont::iterator i = myFollowingDefs.find(follower);
    if (i==myFollowingDefs.end()) {
        MsgHandler::getErrorInstance()->inform("The edges '" + getID() + "' and '" + follower->getID() + "' are not connected.");
        return;
    }
    (*i).second->add(begTime, endTime, probability);
}


ROJTREdge *
ROJTREdge::chooseNext(const ROVehicle * const veh, SUMOTime time) const {
    // if no usable follower exist, return 0
    //  their probabilities are not yet regarded
    if (myFollowingEdges.size()==0 || (veh!=0 && allFollowersProhibit(veh))) {
        return 0;
    }
    // gather information about the probabilities at this time
    RandomDistributor<ROJTREdge*> dist;
    {
        // use the loaded definitions, first
        FollowerUsageCont::const_iterator i;
        for (i=myFollowingDefs.begin(); i!=myFollowingDefs.end(); i++) {
            if ((veh==0 || !(*i).first->prohibits(veh)) && (*i).second->describesTime(time)) {
                dist.add((*i).second->getValue(time), (*i).first);
            }
        }
    }
    // if no loaded definitions are valid for this time, try to use the defaults
    if (dist.getOverallProb()==0) {
        for (size_t i=0; i<myParsedTurnings.size(); ++i) {
            if (veh==0 || !myFollowingEdges[i]->prohibits(veh)) {
                dist.add(myParsedTurnings[i], static_cast<ROJTREdge*>(myFollowingEdges[i]));
            }
        }
    }
    // if still no valid follower exists, return null
    if (dist.getOverallProb()==0) {
        return 0;
    }
    // return one of the possible followers
    return dist.get();
}


void
ROJTREdge::setTurnDefaults(const std::vector<SUMOReal> &defs) {
    // I hope, we'll find a less ridiculous solution for this
    std::vector<SUMOReal> tmp(defs.size()*myFollowingEdges.size(), 0);
    // store in less common multiple
    size_t i;
    for (i=0; i<defs.size(); i++) {
        for (size_t j=0; j<myFollowingEdges.size(); j++) {
            tmp[i*myFollowingEdges.size()+j] = (SUMOReal)
                                               (defs[i] / 100.0 / (myFollowingEdges.size()));
        }
    }
    // parse from less common multiple
    for (i=0; i<myFollowingEdges.size(); i++) {
        SUMOReal value = 0;
        for (size_t j=0; j<defs.size(); j++) {
            value += tmp[i*defs.size()+j];
        }
        myParsedTurnings.push_back((SUMOReal) value);
    }
}



/****************************************************************************/

