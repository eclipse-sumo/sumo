/****************************************************************************/
/// @file    GNERerouterInterval.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2017
/// @version $Id: GNERerouter.cpp 22699 2017-01-25 14:56:03Z behrisch $
///
///
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
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

#include <utils/common/ToString.h>

#include "GNERerouterInterval.h"
#include "GNEClosingLaneReroute.h"
#include "GNEClosingReroute.h"
#include "GNEDestProbReroute.h"
#include "GNERouteProbReroute.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif

// ===========================================================================
// member method definitions
// ===========================================================================

GNERerouterInterval::GNERerouterInterval(GNERerouter *rerouterParent, SUMOReal begin, SUMOReal end) :
    std::pair<SUMOReal, SUMOReal>(begin, end), 
    myRerouterParent(rerouterParent),
    myTag(SUMO_TAG_INTERVAL) {
}


GNERerouterInterval::~GNERerouterInterval() {
    // delete all closing lane reroutes
    for(std::vector<GNEClosingLaneReroute*>::iterator i = myClosingLaneReroutes.begin(); i != myClosingLaneReroutes.end(); i++) {
        delete (*i); 
    }

    // delete all closing reroutes
    for(std::vector<GNEClosingReroute*>::iterator i = myClosingReroutes.begin(); i != myClosingReroutes.end(); i++) {
        delete (*i);
    }

    // delete all destiny Probability reroutes
    for(std::vector<GNEDestProbReroute*>::iterator i = myDestProbReroutes.begin(); i != myDestProbReroutes.end(); i++) {
        delete (*i);
    }

    // delete all route probability reroutes
    for(std::vector<GNERouteProbReroute*>::iterator i = myRouteProbReroutes.begin(); i != myRouteProbReroutes.end(); i++) {
        delete (*i);
    }
}


void 
GNERerouterInterval::insertClosinLanegReroutes(GNEClosingLaneReroute* clr) {
    // Check if was already inserted
    if(std::find(myClosingLaneReroutes.begin(), myClosingLaneReroutes.end(), clr) != myClosingLaneReroutes.end()) {
        throw ProcessError(clr->getTag() + " with ID = " + clr->getClosedEdgeId() + "' was already inserted");
    }
    // insert in vector
    myClosingLaneReroutes.push_back(clr);
}


void 
GNERerouterInterval::removeClosingLaneReroutes(GNEClosingLaneReroute* clr) {
    std::vector<GNEClosingLaneReroute*>::iterator i = std::find(myClosingLaneReroutes.begin(), myClosingLaneReroutes.end(), clr);
    // check if exists
    if(i == myClosingLaneReroutes.end()) {
        throw ProcessError(clr->getTag() + " with ID = " + clr->getClosedEdgeId() + "' wasn't inserted");
    }
    // remove it from vector
    myClosingLaneReroutes.erase(i);
}


void
GNERerouterInterval::insertClosingReroutes(GNEClosingReroute* cr) {
    // Check if was already inserted
    if(std::find(myClosingReroutes.begin(), myClosingReroutes.end(), cr) != myClosingReroutes.end()) {
        throw ProcessError(cr->getTag() + " with ID = " + cr->getClosedEdgeId() + "' was already inserted");
    }
    // insert in vector
    myClosingReroutes.push_back(cr);
}


void
GNERerouterInterval::removeClosingReroutes(GNEClosingReroute* cr) {
    std::vector<GNEClosingReroute*>::iterator i = std::find(myClosingReroutes.begin(), myClosingReroutes.end(), cr);
    // check if exists
    if(i == myClosingReroutes.end()) {
        throw ProcessError(cr->getTag() + " with ID = " + cr->getClosedEdgeId() + "' wasn't inserted");
    }
    // remove it from vector
    myClosingReroutes.erase(i);
}


void
GNERerouterInterval::insertDestProbReroutes(GNEDestProbReroute* dpr) {
    // Check if was already inserted
    if(std::find(myDestProbReroutes.begin(), myDestProbReroutes.end(), dpr) != myDestProbReroutes.end()) {
        throw ProcessError(dpr->getTag() + " with ID = " + dpr->getNewDestinationId() + "' was already inserted");
    }
    // insert in vector
    myDestProbReroutes.push_back(dpr);
}


void
GNERerouterInterval::removeDestProbReroutes(GNEDestProbReroute* dpr) {
    std::vector<GNEDestProbReroute*>::iterator i = std::find(myDestProbReroutes.begin(), myDestProbReroutes.end(), dpr);
    // check if exists
    if(i == myDestProbReroutes.end()) {
        throw ProcessError(dpr->getTag() + " with ID = " + dpr->getNewDestinationId() + "' wasn't inserted");
    }
    // remove it from vector
    myDestProbReroutes.erase(i);
}


void
GNERerouterInterval::insertRouteProbReroute(GNERouteProbReroute* rpr) {
    // Check if was already inserted
    if(std::find(myRouteProbReroutes.begin(), myRouteProbReroutes.end(), rpr) != myRouteProbReroutes.end()) {
        throw ProcessError(rpr->getTag() + " with ID = " + rpr->getNewRouteId() + "' was already inserted");
    }
    // insert in vector
    myRouteProbReroutes.push_back(rpr);
}


void
GNERerouterInterval::removeRouteProbReroute(GNERouteProbReroute* rpr) {
    std::vector<GNERouteProbReroute*>::iterator i = std::find(myRouteProbReroutes.begin(), myRouteProbReroutes.end(), rpr);
    // check if exists
    if(i == myRouteProbReroutes.end()) {
        throw ProcessError(rpr->getTag() + " with ID = " + rpr->getNewRouteId() + "' wasn't inserted");
    }
    // remove it from vector
    myRouteProbReroutes.erase(i);
}


SUMOReal
GNERerouterInterval::getBegin() const {
    return first;
}


SUMOReal
GNERerouterInterval::getEnd() const {
    return second;
}


SumoXMLTag 
GNERerouterInterval::getTag() const {
    return myTag;
}


const std::vector<GNEClosingLaneReroute*> &
GNERerouterInterval::getClosingLaneReroutes() const {
    return myClosingLaneReroutes;
}


const std::vector<GNEClosingReroute*> &
GNERerouterInterval::getClosingReroutes() const {
    return myClosingReroutes;
}


const std::vector<GNEDestProbReroute*> &
GNERerouterInterval::getDestProbReroutes() const {
    return myDestProbReroutes;
}


const std::vector<GNERouteProbReroute*> &
GNERerouterInterval::getRouteProbReroutes() const {
    return myRouteProbReroutes;
}

/****************************************************************************/
