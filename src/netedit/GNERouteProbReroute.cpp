/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2017 German Aerospace Center (DLR) and others.
/****************************************************************************/
//
//   This program and the accompanying materials
//   are made available under the terms of the Eclipse Public License v2.0
//   which accompanies this distribution, and is available at
//   http://www.eclipse.org/legal/epl-v20.html
//
/****************************************************************************/
/// @file    GNERouteProbReroute.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2017
/// @version $Id$
///
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

#include "GNERouteProbReroute.h"
#include "GNEEdge.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNERouteProbReroute::GNERouteProbReroute(GNERerouterInterval& rerouterIntervalParent, std::string newRouteId, double probability) :
    myRerouterIntervalParent(&rerouterIntervalParent),
    myNewRouteId(newRouteId),
    myProbability(probability),
    myTag(SUMO_TAG_ROUTE_PROB_REROUTE) {
    // set probability manually to avoid non valid values
    setProbability(probability);
}


GNERouteProbReroute::~GNERouteProbReroute() {
}


std::string
GNERouteProbReroute::getNewRouteId() const {
    return myNewRouteId;
}


void
GNERouteProbReroute::setNewRouteId(std::string newRouteId) {
    myNewRouteId = newRouteId;
}


double
GNERouteProbReroute::getProbability() const {
    return myProbability;
}


bool
GNERouteProbReroute::setProbability(double probability) {
    if (probability >= 0 && probability <= 1) {
        myProbability = probability;
        return true;
    } else {
        return false;
    }
}


SumoXMLTag
GNERouteProbReroute::getTag() const {
    return myTag;
}


const GNERerouterInterval&
GNERouteProbReroute::getRerouterIntervalParent() const {
    return *myRerouterIntervalParent;
}


bool
GNERouteProbReroute::operator==(const GNERouteProbReroute& routeProbReroute) const {
    if ((myRerouterIntervalParent == routeProbReroute.myRerouterIntervalParent) &&
            (myNewRouteId == routeProbReroute.myNewRouteId) &&
            (myProbability && routeProbReroute.myProbability)) {
        return true;
    } else {
        return false;
    }
}

/****************************************************************************/
