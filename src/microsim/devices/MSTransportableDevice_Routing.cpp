/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2007-2024 German Aerospace Center (DLR) and others.
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
/// @file    MSTransportableDevice_Routing.cpp
/// @author  Michael Behrisch
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Christoph Sommer
/// @author  Jakob Erdmann
/// @date    Tue, 04 Dec 2007
///
// A device that performs vehicle rerouting based on current edge speeds
/****************************************************************************/
#include <config.h>

#include <microsim/MSNet.h>
#include <microsim/MSEventControl.h>
#include <microsim/transportables/MSTransportable.h>
#include <utils/options/OptionsCont.h>
#include <utils/xml/SUMOSAXAttributes.h>
#include <microsim/MSEdge.h>

#include "MSRoutingEngine.h"
#include "MSTransportableDevice_Routing.h"


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static initialisation methods
// ---------------------------------------------------------------------------
void
MSTransportableDevice_Routing::insertOptions(OptionsCont& oc) {
    insertDefaultAssignmentOptions("rerouting", "Routing", oc, true);
    oc.doRegister("person-device.rerouting.period", new Option_String("0", "TIME"));
    oc.addSynonyme("person-device.rerouting.period", "person-device.routing.period", true);
    oc.addDescription("person-device.rerouting.period", "Routing", TL("The period with which the person shall be rerouted"));

    oc.doRegister("person-device.rerouting.mode", new Option_String("0"));
    oc.addDescription("person-device.rerouting.mode", "Routing", TL("Set routing flags (8 ignores temporary blockages)"));

    oc.doRegister("person-device.rerouting.scope", new Option_String("stage"));
    oc.addDescription("person-device.rerouting.scope", "Routing", TL("Which part of the person plan is to be replaced (stage, sequence, or trip)"));
}


void
MSTransportableDevice_Routing::buildDevices(MSTransportable& p, std::vector<MSTransportableDevice*>& into) {
    const OptionsCont& oc = OptionsCont::getOptions();
    if (p.getParameter().wasSet(VEHPARS_FORCE_REROUTE) || equippedByDefaultAssignmentOptions(oc, "rerouting", p, false, true)) {
        // route computation is enabled
        const SUMOTime period = p.getTimeParam("person-device.rerouting.period");
        if (period > 0) {
            MSRoutingEngine::initWeightUpdate();
            // build the device
            into.push_back(new MSTransportableDevice_Routing(p, "routing_" + p.getID(), period));
        }
    }
}


// ---------------------------------------------------------------------------
// MSTransportableDevice_Routing-methods
// ---------------------------------------------------------------------------
MSTransportableDevice_Routing::MSTransportableDevice_Routing(MSTransportable& holder, const std::string& id, SUMOTime period)
    : MSTransportableDevice(holder, id), myPeriod(period), myLastRouting(-1), myRerouteCommand(0) {
    myScope = holder.getStringParam("person-device.rerouting.scope");
    // no need for initial routing here, personTrips trigger it themselves
    myRerouteCommand = new WrappingCommand<MSTransportableDevice_Routing>(this, &MSTransportableDevice_Routing::wrappedRerouteCommandExecute);
    MSNet::getInstance()->getInsertionEvents()->addEvent(myRerouteCommand, SIMSTEP + period);
    // the event will deschedule and destroy itself so it does not need to be stored
}


MSTransportableDevice_Routing::~MSTransportableDevice_Routing() {
    // make the rerouting command invalid if there is one
    if (myRerouteCommand != nullptr) {
        myRerouteCommand->deschedule();
    }
}


SUMOTime
MSTransportableDevice_Routing::wrappedRerouteCommandExecute(SUMOTime currentTime) {
    reroute(currentTime);
    return myPeriod;
}


void
MSTransportableDevice_Routing::reroute(const SUMOTime currentTime, const bool onInit) {
    MSRoutingEngine::initEdgeWeights(SVC_PEDESTRIAN);
    //check whether the weights did change since the last reroute
    if (myLastRouting >= MSRoutingEngine::getLastAdaptation()) {
        return;
    }
    myLastRouting = currentTime;
    MSRoutingEngine::reroute(myHolder, currentTime, "person-device.rerouting", onInit);
}


std::string
MSTransportableDevice_Routing::getParameter(const std::string& key) const {
    if (key == "period") {
        return time2string(myPeriod);
    }
    throw InvalidArgument("Parameter '" + key + "' is not supported for device of type '" + deviceName() + "'");
}


void
MSTransportableDevice_Routing::setParameter(const std::string& key, const std::string& value) {
    double doubleValue;
    try {
        doubleValue = StringUtils::toDouble(value);
    } catch (NumberFormatException&) {
        throw InvalidArgument("Setting parameter '" + key + "' requires a number for device of type '" + deviceName() + "'");
    }
    if (key == "period") {
        const SUMOTime oldPeriod = myPeriod;
        myPeriod = TIME2STEPS(doubleValue);
        if (myPeriod <= 0) {
            myRerouteCommand->deschedule();
        } else if (oldPeriod <= 0) {
            // re-schedule routing command
            MSNet::getInstance()->getInsertionEvents()->addEvent(new WrappingCommand<MSTransportableDevice_Routing>(this, &MSTransportableDevice_Routing::wrappedRerouteCommandExecute), SIMSTEP + myPeriod);
        }
    } else {
        throw InvalidArgument("Setting parameter '" + key + "' is not supported for device of type '" + deviceName() + "'");
    }
}


void
MSTransportableDevice_Routing::saveState(OutputDevice& out) const {
    out.openTag(SUMO_TAG_DEVICE);
    out.writeAttr(SUMO_ATTR_ID, getID());
    std::vector<std::string> internals;
    internals.push_back(toString(myPeriod));
    out.writeAttr(SUMO_ATTR_STATE, toString(internals));
    out.closeTag();
}


void
MSTransportableDevice_Routing::loadState(const SUMOSAXAttributes& attrs) {
    std::istringstream bis(attrs.getString(SUMO_ATTR_STATE));
    bis >> myPeriod;
}


/****************************************************************************/
