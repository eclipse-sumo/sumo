/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2017-2024 German Aerospace Center (DLR) and others.
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
#include <microsim/trigger/MSTriggeredRerouter.h>
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
    MSNet::getInstance(); // just to check that we actually have a network
    std::vector<std::string> ids;
    for (const auto& item : MSTriggeredRerouter::getInstances()) {
        ids.push_back(item.first);
    }
    return ids;
}

int
Rerouter::getIDCount() {
    return (int)MSTriggeredRerouter::getInstances().size();
}


std::string
Rerouter::getParameter(const std::string& /* rerouterID */, const std::string& /* param */) {
    return "";
}


LIBSUMO_GET_PARAMETER_WITH_KEY_IMPLEMENTATION(Rerouter)


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
Rerouter::handleVariable(const std::string& objID, const int variable, VariableWrapper* wrapper, tcpip::Storage* paramData) {
    switch (variable) {
        case TRACI_ID_LIST:
            return wrapper->wrapStringList(objID, variable, getIDList());
        case ID_COUNT:
            return wrapper->wrapInt(objID, variable, getIDCount());
        case libsumo::VAR_PARAMETER:
            paramData->readUnsignedByte();
            return wrapper->wrapString(objID, variable, getParameter(objID, paramData->readString()));
        case libsumo::VAR_PARAMETER_WITH_KEY:
            paramData->readUnsignedByte();
            return wrapper->wrapStringPair(objID, variable, getParameterWithKey(objID, paramData->readString()));
        default:
            return false;
    }
}

}


/****************************************************************************/
