/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2013-2024 German Aerospace Center (DLR) and others.
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
/// @file    MSTransportableDevice_FCDReplay.cpp
/// @author  Michael Behrisch
/// @date    04.03.2024
///
// A device which replays recorded floating car data
/****************************************************************************/
#include <config.h>

#include <utils/options/OptionsCont.h>
#include <libsumo/Helper.h>
#include <microsim/transportables/MSTransportable.h>
#include "MSTransportableDevice_FCDReplay.h"

// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static initialisation methods
// ---------------------------------------------------------------------------
void
MSTransportableDevice_FCDReplay::insertOptions(OptionsCont& oc) {
    insertDefaultAssignmentOptions("fcd-replay", "FCD Replay Device", oc, true);
}


void
MSTransportableDevice_FCDReplay::buildDevices(MSTransportable& t, std::vector<MSTransportableDevice*>& into) {
    OptionsCont& oc = OptionsCont::getOptions();
    if (equippedByDefaultAssignmentOptions(oc, "fcd-replay", t, oc.isSet("device.fcd-replay.file"), true)) {
        MSTransportableDevice_FCDReplay* device = new MSTransportableDevice_FCDReplay(t, "fcdReplay_" + t.getID());
        into.push_back(device);
    }
}


// ---------------------------------------------------------------------------
// MSTransportableDevice_FCDReplay-methods
// ---------------------------------------------------------------------------
MSTransportableDevice_FCDReplay::MSTransportableDevice_FCDReplay(MSTransportable& holder, const std::string& id) :
    MSTransportableDevice(holder, id) {
}


MSTransportableDevice_FCDReplay::~MSTransportableDevice_FCDReplay() {
}


bool
MSTransportableDevice_FCDReplay::notifyMove(SUMOTrafficObject& t,
                         double /*oldPos*/,
                         double /*newPos*/,
                         double /*newSpeed*/) {
    if (myTrajectory == nullptr || myTrajectory->empty()) {
        // TODO remove person
        return false;
    }
    MSPerson* person = dynamic_cast<MSPerson*>(&t);
    if (person == nullptr) {
        return false;
    }
    const auto& p = myTrajectory->front();
    MSLane* lane = nullptr;
    double lanePos;
    double lanePosLat = 0;
    double bestDistance = std::numeric_limits<double>::max();
    int routeOffset = 0;
    ConstMSEdgeVector edges;
    libsumo::Helper::moveToXYMap_matchingRoutePosition(std::get<0>(p), std::get<1>(p),
            {person->getEdge()}, 0,
            person->getVehicleType().getVehicleClass(), true,
            bestDistance, &lane, lanePos, routeOffset);
    libsumo::Helper::setRemoteControlled(person, std::get<0>(p), lane, std::get<2>(p), lanePosLat,
                                         libsumo::INVALID_DOUBLE_VALUE, routeOffset, edges, SIMSTEP);
    // person->setPreviousSpeed(std::get<3>(p), std::numeric_limits<double>::min());
    myTrajectory->erase(myTrajectory->begin());
    if (myTrajectory->empty()) {
        // TODO remove person
        return false;
    }
    return true;
}


/****************************************************************************/
