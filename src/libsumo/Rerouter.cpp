/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2020 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    Rerouter.cpp
/// @author  Jakob Erdmann
/// @date    16.03.2020
///
// C++ TraCI client API implementation
/****************************************************************************/
#include <config.h>

#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include <microsim/trigger/MSLaneSpeedTrigger.h>
#include <libsumo/TraCIConstants.h>
#include "Helper.h"
#include "Rerouter.h"


namespace libsumo {
// ===========================================================================
// static member initializations
// ===========================================================================
SubscriptionResults Rerouter::mySubscriptionResults;
ContextSubscriptionResults Rerouter::myContextSubscriptionResults;


// ===========================================================================
// static member definitions
// ===========================================================================
std::vector<std::string>
Rerouter::getIDList() {
    std::vector<std::string> ids;
    //for (auto& item : MSNet::getInstance()->getStoppingPlaces(SUMO_TAG_BUS_STOP)) {
    //    ids.push_back(item.first);
    //}
    //std::sort(ids.begin(), ids.end());
    return ids;
}

int
Rerouter::getIDCount() {
    return (int)getIDList().size();
}


std::string
Rerouter::getParameter(const std::string& /* rerouterID */, const std::string& /* param */) {
    return "";
}

void
Rerouter::setParameter(const std::string& /* rerouterID */, const std::string& /* key */, const std::string& /* value */) {
    //MSRerouter* r = const_cast<MSRerouter*>(getRerouter(rerouterID));
    //r->setParameter(key, value);
}


LIBSUMO_SUBSCRIPTION_IMPLEMENTATION(Rerouter, REROUTER)


MSTriggeredRerouter*
Rerouter::getRerouter(const std::string& id) {
    MSTriggeredRerouter* s = nullptr;
    if (s == nullptr) {
        throw TraCIException("Rerouter '" + id + "' is not known");
    }
    return s;
}


std::shared_ptr<VariableWrapper>
Rerouter::makeWrapper() {
    return std::make_shared<Helper::SubscriptionWrapper>(handleVariable, mySubscriptionResults, myContextSubscriptionResults);
}


bool
Rerouter::handleVariable(const std::string& objID, const int variable, VariableWrapper* wrapper) {
    switch (variable) {
        case TRACI_ID_LIST:
            return wrapper->wrapStringList(objID, variable, getIDList());
        case ID_COUNT:
            return wrapper->wrapInt(objID, variable, getIDCount());
        default:
            return false;
    }
}


}


/****************************************************************************/
