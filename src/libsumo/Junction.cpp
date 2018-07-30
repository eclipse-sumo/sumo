/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    Junction.cpp
/// @author  Daniel Krajzewicz
/// @author  Mario Krumnow
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Robert Hilbrich
/// @date    30.05.2012
/// @version $Id$
///
// C++ TraCI client API implementation
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/shapes/PointOfInterest.h>
#include <utils/shapes/ShapeContainer.h>
#include <microsim/MSNet.h>
#include <microsim/MSJunctionControl.h>
#include <traci-server/TraCIConstants.h>
#include "Helper.h"
#include "Junction.h"


namespace libsumo {
// ===========================================================================
// static member initializations
// ===========================================================================
SubscriptionResults Junction::mySubscriptionResults;
ContextSubscriptionResults Junction::myContextSubscriptionResults;


// ===========================================================================
// member definitions
// ===========================================================================
std::vector<std::string>
Junction::getIDList() {
    std::vector<std::string> ids;
    MSNet::getInstance()->getJunctionControl().insertIDs(ids);
    return ids;
}


int
Junction::getIDCount() {
    return (int)getIDList().size();
}


TraCIPosition
Junction::getPosition(const std::string& junctionID) {
    return Helper::makeTraCIPosition(getJunction(junctionID)->getPosition());
}


TraCIPositionVector
Junction::getShape(const std::string& junctionID) {
    return Helper::makeTraCIPositionVector(getJunction(junctionID)->getShape());
}


MSJunction*
Junction::getJunction(const std::string& id) {
    MSJunction* j = MSNet::getInstance()->getJunctionControl().get(id);
    if (j == 0) {
        throw TraCIException("Junction '" + id + "' is not known");
    }
    return j;
}


void
Junction::subscribe(const std::string& objID, const std::vector<int>& vars, SUMOTime beginTime, SUMOTime endTime) {
    libsumo::Helper::subscribe(CMD_SUBSCRIBE_JUNCTION_VARIABLE, objID, vars, beginTime, endTime);
}


void
Junction::subscribeContext(const std::string& objID, int domain, double range, const std::vector<int>& vars, SUMOTime beginTime, SUMOTime endTime) {
    libsumo::Helper::subscribe(CMD_SUBSCRIBE_JUNCTION_CONTEXT, objID, vars, beginTime, endTime, domain, range);
}


const SubscriptionResults
Junction::getSubscriptionResults() {
    return mySubscriptionResults;
}


const TraCIResults
Junction::getSubscriptionResults(const std::string& objID) {
    return mySubscriptionResults[objID];
}


const ContextSubscriptionResults
Junction::getContextSubscriptionResults() {
    return myContextSubscriptionResults;
}


const SubscriptionResults
Junction::getContextSubscriptionResults(const std::string& objID) {
    return myContextSubscriptionResults[objID];
}


NamedRTree*
Junction::getTree() {
    NamedRTree* t = new NamedRTree();
    for (const auto& i : MSNet::getInstance()->getJunctionControl()) {
        Boundary b = i.second->getShape().getBoxBoundary();
        const float cmin[2] = {(float) b.xmin(), (float) b.ymin()};
        const float cmax[2] = {(float) b.xmax(), (float) b.ymax()};
        t->Insert(cmin, cmax, i.second);
    }
    return t;
}


void
Junction::storeShape(const std::string& id, PositionVector& shape) {
    shape.push_back(getJunction(id)->getPosition());
}


std::shared_ptr<VariableWrapper>
Junction::makeWrapper() {
    return std::make_shared<Helper::SubscriptionWrapper>(handleVariable, mySubscriptionResults, myContextSubscriptionResults);
}


bool
Junction::handleVariable(const std::string& objID, const int variable, VariableWrapper* wrapper) {
    switch (variable) {
    case ID_LIST:
        return wrapper->wrapStringList(objID, variable, getIDList());
    case ID_COUNT:
        return wrapper->wrapInt(objID, variable, getIDCount());
    case VAR_POSITION:
        return wrapper->wrapPosition(objID, variable, getPosition(objID));
    default:
        return false;
    }
}


}


/****************************************************************************/
