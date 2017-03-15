/****************************************************************************/
/// @file    TraCI.h
/// @author  Daniel Krajzewicz
/// @author  Mario Krumnow
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    30.05.2012
/// @version $Id$
///
// C++ TraCI client API implementation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2012-2017 DLR (http://www.dlr.de/) and contributors
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
#include "../../config.h"
#endif

#include "TraCI.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member definitions
// ===========================================================================
void
TraCI::connect(const std::string& host, int port) {
}


void
TraCI::close() {
}


void
TraCI::subscribe(int domID, const std::string& objID, SUMOTime beginTime, SUMOTime endTime, const std::vector<int>& vars) const {
}

void
TraCI::subscribeContext(int domID, const std::string& objID, SUMOTime beginTime, SUMOTime endTime, int domain, double range, const std::vector<int>& vars) const {
}

const TraCI::SubscribedValues&
TraCI::getSubscriptionResults() const {
    return mySubscribedValues;
}


const TraCI::TraCIValues&
TraCI::getSubscriptionResults(const std::string& objID) const {
    if (mySubscribedValues.find(objID) != mySubscribedValues.end()) {
        return mySubscribedValues.find(objID)->second;
    } else {
        throw; // Something?
    }
}


const TraCI::SubscribedContextValues&
TraCI::getContextSubscriptionResults() const {
    return mySubscribedContextValues;
}


const TraCI::SubscribedValues&
TraCI::getContextSubscriptionResults(const std::string& objID) const {
    if (mySubscribedContextValues.find(objID) != mySubscribedContextValues.end()) {
        return mySubscribedContextValues.find(objID)->second;
    } else {
        throw; // Something?
    }
}


/****************************************************************************/
