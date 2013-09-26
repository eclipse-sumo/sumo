/****************************************************************************/
/// @file    RODFDetectorFlow.cpp
/// @author  Daniel Krajzewicz
/// @author  Eric Nicolay
/// @author  Michael Behrisch
/// @date    Thu, 16.03.2006
/// @version $Id$
///
// Storage for flows within the DFROUTER
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

#include <iostream>
#include <cassert>
#include "RODFDetectorFlow.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
RODFDetectorFlows::RODFDetectorFlows(SUMOTime startTime, SUMOTime endTime,
                                     SUMOTime stepOffset)
    : myBeginTime(startTime), myEndTime(endTime), myStepOffset(stepOffset),
      myMaxDetectorFlow(-1) {}


RODFDetectorFlows::~RODFDetectorFlows() {}


void
RODFDetectorFlows::addFlow(const std::string& id, SUMOTime t, const FlowDef& fd) {
    if (myFastAccessFlows.find(id) == myFastAccessFlows.end()) {
        size_t noItems = (size_t)((myEndTime - myBeginTime) / myStepOffset);
        myFastAccessFlows[id] = std::vector<FlowDef>(noItems);
        std::vector<FlowDef>& cflows = myFastAccessFlows[id];
        // initialise
        for (std::vector<FlowDef>::iterator i = cflows.begin(); i < cflows.end(); ++i) {
            (*i).qPKW = 0;
            (*i).qLKW = 0;
            (*i).vPKW = 0;
            (*i).vLKW = 0;
            (*i).fLKW = 0;
            (*i).isLKW = 0;
            (*i).firstSet = true;
        }
    }
    assert((t - myBeginTime) / myStepOffset < (int) myFastAccessFlows[id].size());
    FlowDef& ofd = myFastAccessFlows[id][(t - myBeginTime) / myStepOffset];
    if (ofd.firstSet) {
        ofd = fd;
        ofd.firstSet = false;
    } else {
        ofd.qLKW = ofd.qLKW + fd.qLKW;
        ofd.qPKW = ofd.qPKW + fd.qPKW;
        ofd.vLKW = ofd.vLKW + fd.vLKW; //!!! mean value?
        ofd.vPKW = ofd.vPKW + fd.vPKW; //!!! mean value?
    }
    if (ofd.qPKW != 0) {
        ofd.fLKW = ofd.qLKW / (ofd.qLKW + ofd.qPKW);
    } else {
        ofd.fLKW = 1;
        ofd.isLKW = 1;
    }
}




void
RODFDetectorFlows::setFlows(const std::string& detector_id,
                            std::vector<FlowDef>& flows) {
    for (std::vector<FlowDef>::iterator i = flows.begin(); i < flows.end(); ++i) {
        FlowDef& ofd = *i;
        if (ofd.qLKW != 0 && ofd.qPKW != 0) {
            ofd.fLKW = ofd.qLKW / ofd.qPKW;
        } else {
            ofd.fLKW = 0;
        }
    }
    myFastAccessFlows[detector_id] = flows;
}


void
RODFDetectorFlows::removeFlow(const std::string& detector_id) {
    myFastAccessFlows.erase(detector_id);
}


bool
RODFDetectorFlows::knows(const std::string& det_id) const {
    return myFastAccessFlows.find(det_id) != myFastAccessFlows.end();
}


const std::vector<FlowDef>&
RODFDetectorFlows::getFlowDefs(const std::string& id) const {
    assert(myFastAccessFlows.find(id) != myFastAccessFlows.end());
    assert(myFastAccessFlows.find(id)->second.size() != 0);
    return myFastAccessFlows.find(id)->second;
}


SUMOReal
RODFDetectorFlows::getFlowSumSecure(const std::string& id) const {
    SUMOReal ret = 0;
    if (knows(id)) {
        const std::vector<FlowDef>& flows = getFlowDefs(id);
        for (std::vector<FlowDef>::const_iterator i = flows.begin(); i != flows.end(); ++i) {
            ret += (*i).qPKW;
            ret += (*i).qLKW;
        }
    }
    return ret;
}


SUMOReal
RODFDetectorFlows::getMaxDetectorFlow() const {
    if (myMaxDetectorFlow < 0) {
        SUMOReal max = 0;
        std::map<std::string, std::vector<FlowDef> >::const_iterator j;
        for (j = myFastAccessFlows.begin(); j != myFastAccessFlows.end(); ++j) {
            SUMOReal curr = 0;
            const std::vector<FlowDef>& flows = (*j).second;
            for (std::vector<FlowDef>::const_iterator i = flows.begin(); i != flows.end(); ++i) {
                curr += (*i).qPKW;
                curr += (*i).qLKW;
            }
            if (max < curr) {
                max = curr;
            }
        }
        myMaxDetectorFlow = max;
    }
    return myMaxDetectorFlow;
}


void
RODFDetectorFlows::mesoJoin(const std::string& nid,
                            const std::vector<std::string>& oldids) {
    for (std::vector<std::string>::const_iterator i = oldids.begin(); i != oldids.end(); ++i) {
        if (!knows(*i)) {
            continue;
        }
        std::vector<FlowDef>& flows = myFastAccessFlows[*i];
        size_t index = 0;
        for (SUMOTime t = myBeginTime; t != myEndTime; t += myStepOffset) {
            addFlow(nid, t, flows[index++]); // !!!
        }
        myFastAccessFlows.erase(*i);
    }
}


void
RODFDetectorFlows::printAbsolute() const {
    for (std::map<std::string, std::vector<FlowDef> >::const_iterator i = myFastAccessFlows.begin(); i != myFastAccessFlows.end(); ++i) {
        std::cout << (*i).first << ":";
        const std::vector<FlowDef>& flows = (*i).second;
        SUMOReal qPKW = 0;
        SUMOReal qLKW = 0;
        for (std::vector<FlowDef>::const_iterator j = flows.begin(); j != flows.end(); ++j) {
            qPKW += (*j).qPKW;
            qLKW += (*j).qLKW;
        }
        std::cout << qPKW << "/" << qLKW << std::endl;
    }
}

/****************************************************************************/

