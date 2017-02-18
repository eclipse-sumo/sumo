/****************************************************************************/
/// @file    GNERerouterInterval.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2017
/// @version $Id$
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
#include <utils/common/MsgHandler.h>

#include "GNERerouterInterval.h"
#include "GNEEdge.h"
#include "GNELane.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif

// ===========================================================================
// member method definitions
// ===========================================================================

GNERerouterInterval::GNERerouterInterval(GNERerouter* rerouterParent, SUMOReal begin, SUMOReal end) :
    myRerouterParent(rerouterParent),
    myBegin(begin),
    myEnd(end),
    myTag(SUMO_TAG_INTERVAL) {
    assert(begin <= end);
}


GNERerouterInterval::~GNERerouterInterval() {
}


bool
GNERerouterInterval::insertClosinLanegReroutes(const GNEClosingLaneReroute& clr) {
    if (std::find(myClosingLaneReroutes.begin(), myClosingLaneReroutes.end(), clr) != myClosingLaneReroutes.end()) {
        WRITE_WARNING(toString(clr.getTag()) + " for lane '" + clr.getClosedLane()->getID() + "' was already inserted; duplicates aren't allowed");
        return false;
    } else {
        myClosingLaneReroutes.push_back(clr);
        return true;
    }
}


bool
GNERerouterInterval::removeClosingLaneReroutes(const GNEClosingLaneReroute& clr) {
    std::vector<GNEClosingLaneReroute>::iterator i = std::find(myClosingLaneReroutes.begin(), myClosingLaneReroutes.end(), clr);
    if (i == myClosingLaneReroutes.end()) {
        WRITE_WARNING(toString(clr.getTag()) + " for lane '" + clr.getClosedLane()->getID() + "' wasn't previously inserted");
        return false;
    } else {
        myClosingLaneReroutes.erase(i);
        return true;
    }
}


bool
GNERerouterInterval::insertClosingReroutes(const GNEClosingReroute& cr) {
    if (std::find(myClosingReroutes.begin(), myClosingReroutes.end(), cr) != myClosingReroutes.end()) {
        WRITE_WARNING(toString(cr.getTag()) + " for edge '" + cr.getClosedEdge()->getID() + "' was already inserted; duplicates aren't allowed");
        return false;
    } else {
        myClosingReroutes.push_back(cr);
        return true;
    }
}


bool
GNERerouterInterval::removeClosingReroutes(const GNEClosingReroute& cr) {
    std::vector<GNEClosingReroute>::iterator i = std::find(myClosingReroutes.begin(), myClosingReroutes.end(), cr);
    if (i == myClosingReroutes.end()) {
        WRITE_WARNING(toString(cr.getTag()) + " for edge '" + cr.getClosedEdge()->getID() + "' wasn't previously inserted");
        return false;
    } else {
        myClosingReroutes.erase(i);
        return true;
    }
}


bool
GNERerouterInterval::insertDestProbReroutes(const GNEDestProbReroute& dpr) {
    if (std::find(myDestProbReroutes.begin(), myDestProbReroutes.end(), dpr) != myDestProbReroutes.end()) {
        WRITE_WARNING(toString(dpr.getTag()) + " to edge '" + dpr.getNewDestination()->getID() + "' was already inserted; duplicates aren't allowed");
        return false;
    } else {
        myDestProbReroutes.push_back(dpr);
        return true;
    }
}


bool
GNERerouterInterval::removeDestProbReroutes(const GNEDestProbReroute& dpr) {
    std::vector<GNEDestProbReroute>::iterator i = std::find(myDestProbReroutes.begin(), myDestProbReroutes.end(), dpr);
    if (i == myDestProbReroutes.end()) {
        WRITE_WARNING(toString(dpr.getTag()) + " to edge '" + dpr.getNewDestination()->getID() + "' wasn't previously inserted");
        return false;
    } else {
        myDestProbReroutes.erase(i);
        return true;
    }
}


bool
GNERerouterInterval::insertRouteProbReroute(const GNERouteProbReroute& rpr) {
    if (std::find(myRouteProbReroutes.begin(), myRouteProbReroutes.end(), rpr) != myRouteProbReroutes.end()) {
        WRITE_WARNING(toString(rpr.getTag()) + " for route '" + rpr.getNewRouteId() + "' was already inserted; duplicates aren't allowed");
        return false;
    } else {
        myRouteProbReroutes.push_back(rpr);
        return true;
    }
}


bool
GNERerouterInterval::removeRouteProbReroute(const GNERouteProbReroute& rpr) {
    std::vector<GNERouteProbReroute>::iterator i = std::find(myRouteProbReroutes.begin(), myRouteProbReroutes.end(), rpr);
    if (i == myRouteProbReroutes.end()) {
        WRITE_WARNING(toString(rpr.getTag()) + " for route '" + rpr.getNewRouteId() + "' wasn't previously inserted");
        return false;
    } else {
        myRouteProbReroutes.erase(i);
        return true;
    }
}


GNERerouter*
GNERerouterInterval::getRerouterParent() const {
    return myRerouterParent;
}


SumoXMLTag
GNERerouterInterval::getTag() const {
    return myTag;
}


SUMOReal
GNERerouterInterval::getBegin() const {
    return myBegin;
}


SUMOReal
GNERerouterInterval::getEnd() const {
    return myEnd;
}


void
GNERerouterInterval::setBegin(SUMOReal begin) {
    myBegin = begin;
}


void
GNERerouterInterval::setEnd(SUMOReal end) {
    myEnd = end;
}


const std::vector<GNEClosingLaneReroute>&
GNERerouterInterval::getClosingLaneReroutes() const {
    return myClosingLaneReroutes;
}


const std::vector<GNEClosingReroute>&
GNERerouterInterval::getClosingReroutes() const {
    return myClosingReroutes;
}


const std::vector<GNEDestProbReroute>&
GNERerouterInterval::getDestProbReroutes() const {
    return myDestProbReroutes;
}


const std::vector<GNERouteProbReroute>&
GNERerouterInterval::getRouteProbReroutes() const {
    return myRouteProbReroutes;
}


void
GNERerouterInterval::setClosingLaneReroutes(const std::vector<GNEClosingLaneReroute>& closingLaneReroutes) {
    myClosingLaneReroutes = closingLaneReroutes;
}


void
GNERerouterInterval::setClosingReroutes(const std::vector<GNEClosingReroute>& closingReroutes) {
    myClosingReroutes = closingReroutes;
}


void
GNERerouterInterval::setDestProbReroutes(const std::vector<GNEDestProbReroute>& destProbReroutes) {
    myDestProbReroutes = destProbReroutes;
}


void
GNERerouterInterval::setRouteProbReroutes(const std::vector<GNERouteProbReroute>& rerouteProbabilityReroutes) {
    myRouteProbReroutes = rerouteProbabilityReroutes;
}


GNERerouterInterval&
GNERerouterInterval::operator=(const GNERerouterInterval& rerouterInterval) {
    if (this != &rerouterInterval) {
        myRerouterParent = rerouterInterval.myRerouterParent;
        myBegin = rerouterInterval.myBegin;
        myEnd = rerouterInterval.myEnd;
    }
    return *this;
}

/****************************************************************************/
