/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2022 German Aerospace Center (DLR) and others.
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
/// @file    FrictionCoefficientSign.cpp
/// @author  Jakob Erdmann
/// @author  Thomas Weber
/// @date    10.03.2022
///
// C++ TraCI client API implementation
/****************************************************************************/
#include <config.h>

#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <microsim/trigger/MSFrictionCoefficientTrigger.h>
#include <libsumo/TraCIConstants.h>
#include "Helper.h"
#include "FrictionCoefficientSign.h"


namespace libsumo {
// ===========================================================================
// static member initializations
// ===========================================================================
SubscriptionResults FrictionCoefficientSign::mySubscriptionResults;
ContextSubscriptionResults FrictionCoefficientSign::myContextSubscriptionResults;


// ===========================================================================
// static member definitions
// ===========================================================================
std::vector<std::string>
FrictionCoefficientSign::getIDList() {
    std::vector<std::string> ids;
    for (auto& item : MSFrictionCoefficientTrigger::getInstances()) {
        ids.push_back(item.first);
    }
    std::sort(ids.begin(), ids.end());
    return ids;
}

int
FrictionCoefficientSign::getIDCount() {
    return (int)getIDList().size();
}

std::vector<std::string>
FrictionCoefficientSign::getLanes(const std::string& cofID) {
    std::vector<std::string> result;
    MSFrictionCoefficientTrigger* cof = getFrictionCoefficientSign(cofID);
    for (MSLane* lane : cof->getLanes()) {
        result.push_back(lane->getID());
    }
    return result;
}

std::string
FrictionCoefficientSign::getParameter(const std::string& /* cofID */, const std::string& /* param */) {
    return "";
}

LIBSUMO_GET_PARAMETER_WITH_KEY_IMPLEMENTATION(FrictionCoefficientSign)

void
FrictionCoefficientSign::setParameter(const std::string& /* cofID */, const std::string& /* key */, const std::string& /* value */) {
    //MSFrictionCoefficientSign* r = const_cast<MSFrictionCoefficientSign*>(getFrictionCoefficientSign(cofID));
    //r->setParameter(key, value);
}


LIBSUMO_SUBSCRIPTION_IMPLEMENTATION(FrictionCoefficientSign, FRICTIONCOEFFICIENT)


MSFrictionCoefficientTrigger*
FrictionCoefficientSign::getFrictionCoefficientSign(const std::string& id) {
    const auto& dict = MSFrictionCoefficientTrigger::getInstances();
    auto it = dict.find(id);
    if (it == dict.end()) {
        throw TraCIException("FrictionCoefficientSign '" + id + "' is not known");
    }
    return it->second;
}


std::shared_ptr<VariableWrapper>
FrictionCoefficientSign::makeWrapper() {
    return std::make_shared<Helper::SubscriptionWrapper>(handleVariable, mySubscriptionResults, myContextSubscriptionResults);
}


bool
FrictionCoefficientSign::handleVariable(const std::string& objID, const int variable, VariableWrapper* wrapper, tcpip::Storage* paramData) {
    switch (variable) {
        case TRACI_ID_LIST:
            return wrapper->wrapStringList(objID, variable, getIDList());
        case ID_COUNT:
            return wrapper->wrapInt(objID, variable, getIDCount());
        case VAR_LANES:
            return wrapper->wrapStringList(objID, variable, getLanes(objID));
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
