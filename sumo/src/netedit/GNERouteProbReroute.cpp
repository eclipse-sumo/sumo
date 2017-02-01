/****************************************************************************/
/// @file    GNERouteProbReroute.cpp
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

#include "GNERouteProbReroute.h"
#include "GNEEdge.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif

// ===========================================================================
// member method definitions
// ===========================================================================

GNERouteProbReroute::GNERouteProbReroute(GNERerouterInterval *rerouterIntervalParent, std::string newRouteId, SUMOReal probability) :
    myNewRouteId(newRouteId),
    myProbability(probability),
    myRerouterIntervalParent(rerouterIntervalParent),
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


SUMOReal
GNERouteProbReroute::getProbability() const {
    return myProbability;
}


bool
GNERouteProbReroute::setProbability(SUMOReal probability) {
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


GNERerouterInterval*
GNERouteProbReroute::getRerouterIntervalParent() const {
    return myRerouterIntervalParent;
}

/****************************************************************************/
