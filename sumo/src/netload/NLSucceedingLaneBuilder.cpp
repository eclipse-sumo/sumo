/****************************************************************************/
/// @file    NLSucceedingLaneBuilder.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 22 Oct 2001
/// @version $Id$
///
// Temporary storage for a lanes succeeding lanes while parsing them
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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

#include <string>
#include <map>
#include <vector>
#include <microsim/MSLane.h>
#include <microsim/MSInternalLane.h>
#include <microsim/MSLink.h>
#include <microsim/MSLinkCont.h>
#include <microsim/MSGlobals.h>
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
#include <microsim/MSEdgeContinuations.h>
#include "NLBuilder.h"
#include "NLSucceedingLaneBuilder.h"
#include "NLJunctionControlBuilder.h"
#include <utils/options/OptionsCont.h>
#include <utils/common/UtilExceptions.h>
#include <utils/geom/GeomHelper.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
NLSucceedingLaneBuilder::NLSucceedingLaneBuilder(NLJunctionControlBuilder &jb,
        MSEdgeContinuations &ec) throw()
        : myJunctionControlBuilder(jb), myEdgeContinuations(ec) {
    mySuccLanes = new MSLinkCont();
    mySuccLanes->reserve(10);
}


NLSucceedingLaneBuilder::~NLSucceedingLaneBuilder() throw() {
    delete mySuccLanes;
}


void
NLSucceedingLaneBuilder::openSuccLane(const string &laneId) throw() {
    myCurrentLane = laneId;
}


void
NLSucceedingLaneBuilder::addSuccLane(bool yield, const string &laneId,
#ifdef HAVE_INTERNAL_LANES
                                     const std::string &viaID,
                                     SUMOReal pass,
#endif
                                     MSLink::LinkDirection dir,
                                     MSLink::LinkState state,
                                     bool internalEnd,
                                     const std::string &tlid, unsigned int linkNo) throw(InvalidArgument) {
    // check whether the link is a dead link
    if (laneId=="SUMO_NO_DESTINATION") {
        // build the dead link and add it to the container
#ifdef HAVE_INTERNAL_LANES
        mySuccLanes->push_back(new MSLink(0, 0, yield, MSLink::LINKDIR_NODIR, MSLink::LINKSTATE_DEADEND, false, 0.));
#else
        mySuccLanes->push_back(new MSLink(0, yield, MSLink::LINKDIR_NODIR, MSLink::LINKSTATE_DEADEND, 0.));
#endif
        return;
    }

    // get the lane the link belongs to
    MSLane *lane = MSLane::dictionary(laneId);
    if (lane==0) {
        throw InvalidArgument("An unknown lane ('" + laneId + "') should be set as a follower for lane '" + myCurrentLane + "'.");
    }
#ifdef HAVE_INTERNAL_LANES
    MSLane *via = 0;
    if (viaID!="" && MSGlobals::gUsingInternalLanes) {
        via = MSLane::dictionary(viaID);
        if (via==0) {
            throw InvalidArgument("An unknown lane ('" + viaID + "') should be set as a via-lane for lane '" + myCurrentLane + "'.");
        }
    }
    if (pass>=0) {
        static_cast<MSInternalLane*>(lane)->setPassPosition(pass);
    }
#endif
    MSLane *orig = MSLane::dictionary(myCurrentLane);
    if (orig==0) {
        return;
    }


    // build the link
    SUMOReal length = orig!=0&&lane!=0
                      ? orig->getShape()[-1].distanceTo(lane->getShape()[0])
                      : 0;
#ifdef HAVE_INTERNAL_LANES
    if (via!=0) {
        length = via->getLength();
    }
    MSLink *link = new MSLink(lane, via, yield, dir, state, internalEnd, length);
#else
    MSLink *link = new MSLink(lane, yield, dir, state, length);
#endif

    if (MSLane::dictionary(myCurrentLane)!=0) {
#ifdef HAVE_INTERNAL_LANES
        if (via!=0) {
            // from a normal in to a normal out via
            //  --> via incomes in out
            lane->addIncomingLane(via, link);
            //  --> in incomes in via
            via->addIncomingLane(MSLane::dictionary(myCurrentLane), link);
        } else {
            if (myCurrentLane[0]!=':') {
                // internal not wished; other case already set
                lane->addIncomingLane(MSLane::dictionary(myCurrentLane), link);
            }
        }
#else
        lane->addIncomingLane(MSLane::dictionary(myCurrentLane), link);
#endif
    }
    // if a traffic light is responsible for it, inform the traffic light
    // check whether this link is controlled by a traffic light
    if (tlid!="") {
        MSTLLogicControl::TLSLogicVariants &logics = myJunctionControlBuilder.getTLLogic(tlid);
        MSLane *current = MSLane::dictionary(myCurrentLane);
        if (current==0) {
            throw InvalidArgument("An unknown lane ('" + myCurrentLane + "') should be assigned to a tl-logic.");
        }
        logics.addLink(link, current, linkNo);
    }
    // add the link to the container
    mySuccLanes->push_back(link);
    myEdgeContinuations.add(&MSLane::dictionary(myCurrentLane)->getEdge(), &lane->getEdge());
}


void
NLSucceedingLaneBuilder::closeSuccLane() throw(InvalidArgument) {
    MSLane *current = MSLane::dictionary(myCurrentLane);
    if (current==0) {
        throw InvalidArgument("Trying to close connections of an unknown lane ('" + myCurrentLane + "').");
    }
    MSLinkCont *cont = new MSLinkCont();
    cont->reserve(mySuccLanes->size());
    copy(mySuccLanes->begin(), mySuccLanes->end(), back_inserter(*cont));
    current->initialize(cont);
    mySuccLanes->clear();
}


const std::string &
NLSucceedingLaneBuilder::getCurrentLaneName() const throw() {
    return myCurrentLane;
}



/****************************************************************************/

