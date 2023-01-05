/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    MSDetectorFileOutput.cpp
/// @author  Christian Roessel
/// @author  Daniel Krajzewicz
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    2004-11-23
///
// Base of value-generating classes (detectors)
/****************************************************************************/
#include <config.h>

#include <utils/common/StringTokenizer.h>
#include <microsim/MSVehicleType.h>
#include <microsim/MSEdge.h>
#include <microsim/MSRoute.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/transportables/MSTransportable.h>
#include <utils/vehicle/SUMOTrafficObject.h>
#include <utils/vehicle/SUMOVehicle.h>
#include "MSDetectorFileOutput.h"

// ===========================================================================
// method definitions
// ===========================================================================

MSDetectorFileOutput::MSDetectorFileOutput(const std::string& id,
        const std::string& vTypes,
        const std::string& nextEdges,
        const int detectPersons) :
    Named(id),
    myDetectPersons(detectPersons) {
    const std::vector<std::string> vt = StringTokenizer(vTypes).getVector();
    myVehicleTypes.insert(vt.begin(), vt.end());
    for (const std::string& edgeID : StringTokenizer(nextEdges).getVector()) {
        const MSEdge* e = MSEdge::dictionary(edgeID);
        if (e) {
            myNextEdges.push_back(e);
        } else {
            throw ProcessError("Unknown edge '" + edgeID + "' given as nextEdges in detector '" + id + "'");
        }
    }
}


bool
MSDetectorFileOutput::vehicleApplies(const SUMOTrafficObject& veh) const {
    if (veh.isVehicle() == detectPersons()) {
        return false;
    }
    if (!myVehicleTypes.empty() && myVehicleTypes.count(veh.getVehicleType().getOriginalID()) == 0) {
        std::set<std::string> vTypeDists = MSNet::getInstance()->getVehicleControl().getVTypeDistributionMembership(veh.getVehicleType().getOriginalID());
        bool typeMatches = false;
        for (auto vTypeDist : vTypeDists) {
            if (myVehicleTypes.count(vTypeDist) > 0) {
                typeMatches = true;
                break;
            }
        }
        if (!typeMatches) {
            return false;
        }
    }
    if (!myNextEdges.empty()) {
        MSRouteIterator it;
        MSRouteIterator end;
        ConstMSEdgeVector route;
        if (veh.isVehicle()) {
            const SUMOVehicle& v = dynamic_cast<const SUMOVehicle&>(veh);
            it = v.getCurrentRouteEdge();
            end = v.getRoute().end();
        } else if (veh.isPerson()) {
            const MSTransportable& p = dynamic_cast<const MSTransportable&>(veh);
            route = p.getEdges(0);
            it = route.begin() + p.getRoutePosition();
            end = route.end();
        }
        for (const MSEdge* e : myNextEdges) {
            it = std::find(it, end, e);
            if (it == end) {
                if (e != veh.getNextEdgePtr()) {
                    //std::cout << SIMTIME << " det=" << getID() << " veh=" << veh.getID() << " e=" << e->getID() << " vehNext=" << Named::getIDSecure(veh.getNextEdgePtr()) << "\n";
                    return false;
                }
            }
        }
    }
    return true;
}

bool
MSDetectorFileOutput::personApplies(const MSTransportable& p, int dir) const {
    //std::cout << getID() << " p=" << p.getID() << " veh=" << Named::getIDSecure(p.getVehicle()) << "\n";
    if (p.getVehicle() == nullptr) {
        const int dirCode = dir < 0 ? 2 : dir;
        //std::cout << "   dir=" << dir << " dirCode=" << dirCode << " myDetectPersons=" << myDetectPersons << "\n";
        if ((dirCode & myDetectPersons) == 0) {
            // person walking in wrong direction or not walking at all
            return false;
        }
    } else {
        const SUMOVehicleClass svc = p.getVehicle()->getVClass();
        int vClassCode;
        if ((svc & SVC_PUBLIC_CLASSES) != 0) {
            vClassCode = (int)PersonMode::PUBLIC;
        } else if ((svc & SVC_BICYCLE) != 0) {
            vClassCode = (int)PersonMode::BICYCLE;
        } else if ((svc & SVC_TAXI) != 0) {
            vClassCode = (int)PersonMode::TAXI;
        } else {
            vClassCode = (int)PersonMode::CAR;
        }
        if ((vClassCode & myDetectPersons) == 0) {
            // person riding in the wrong vehicle
            return false;
        }
    }
    return true;
}



/****************************************************************************/
