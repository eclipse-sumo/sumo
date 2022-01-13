/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2022 German Aerospace Center (DLR) and others.
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
#include <microsim/MSVehicleControl.h>
#include <microsim/transportables/MSTransportable.h>
#include <utils/vehicle/SUMOTrafficObject.h>
#include <utils/vehicle/SUMOVehicle.h>
#include "MSDetectorFileOutput.h"

// ===========================================================================
// method definitions
// ===========================================================================

MSDetectorFileOutput::MSDetectorFileOutput(const std::string& id, const std::string& vTypes, const int detectPersons) :
    Named(id),
    myDetectPersons(detectPersons) {
    const std::vector<std::string> vt = StringTokenizer(vTypes).getVector();
    myVehicleTypes.insert(vt.begin(), vt.end());
}

MSDetectorFileOutput::MSDetectorFileOutput(const std::string& id, const std::set<std::string>& vTypes, const int detectPersons) :
    Named(id), myVehicleTypes(vTypes), myDetectPersons(detectPersons)
{ }

bool
MSDetectorFileOutput::vehicleApplies(const SUMOTrafficObject& veh) const {
    if (veh.isVehicle() == detectPersons()) {
        return false;
    } else if (myVehicleTypes.empty() || myVehicleTypes.count(veh.getVehicleType().getOriginalID()) > 0) {
        return true;
    } else {
        std::set<std::string> vTypeDists = MSNet::getInstance()->getVehicleControl().getVTypeDistributionMembership(veh.getVehicleType().getOriginalID());
        for (auto vTypeDist : vTypeDists) {
            if (myVehicleTypes.count(vTypeDist) > 0) {
                return true;
            }
        }
        return false;
    }
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
        int vClassCode;;
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
